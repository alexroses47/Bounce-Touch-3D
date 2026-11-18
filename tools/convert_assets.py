#!/usr/bin/env python3
"""Build the 3DS romfs from your own copy of Bounce Touch.

Usage:
  convert_assets.py "Bounce Touch v1.00(57).sis" romfs/ [--work DIR] [--force]
  convert_assets.py extracted_folder/ romfs/           (already extracted .sis)

Needs Python 3 (standard library only), tex3ds (devkitPro) and oggenc
(brew install vorbis-tools). Output layout and formats: docs/ROMFS.md.

All output names are lowercase. Nothing is downloaded or uploaded.
"""
import concurrent.futures as cf
import hashlib
import os
import shutil
import struct
import subprocess
import sys
import time
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import bsc  # noqa: E402
import hgr  # noqa: E402
import ntx  # noqa: E402
import sis_extract  # noqa: E402

CONVERTER_VERSION = 1
KNOWN_SIS = {
    "0ef6b509e4b55f7ae09624967fbb423158dbc844cb102940f597292d21748c0a": "Bounce Touch v1.00(57) EN",
}
DATA = "private/2000C4D4/data_S60"
TEX3DS = shutil.which("tex3ds") or "/opt/devkitpro/tools/bin/tex3ds"
OGGENC = shutil.which("oggenc") or "/opt/homebrew/bin/oggenc"
OGG_QUALITY = "6"
FONT_PAGE = 512
SKIP_IMAGES = {"nokia_splash"}   # phone-maker splash: not part of the game


def log(msg):
    print(msg, flush=True)


def sha256(path):
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def run(cmd):
    r = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if r.returncode != 0:
        raise RuntimeError(f"{os.path.basename(cmd[0])} failed: {r.stderr.strip() or r.stdout.strip()}")


def files_in(folder, ext):
    return sorted(f for f in os.listdir(folder) if f.lower().endswith(ext))


# ------------------------------------------------------------------ textures

def texture_format(info, rgba):
    alphas = set(rgba[3::4])
    if alphas == {255}:
        return "rgb565"
    if alphas <= {0, 255}:
        return "rgba5551"
    return "rgba4"


def ntx_to_t3x(src, dst, work, mipmaps):
    w, h, rgba, info = ntx.read_ntx(open(src, "rb").read())
    if max(w, h) > 1024:
        raise ValueError(f"{src}: {w}x{h} exceeds the 1024 texture limit")
    png = os.path.join(work, os.path.basename(dst) + ".png")
    ntx.write_png(png, w, h, rgba)
    fmt = texture_format(info, rgba)
    cmd = [TEX3DS, "-f", fmt, "-z", "auto", "-o", dst, png]
    if mipmaps:
        cmd[1:1] = ["-m", "box"]
    run(cmd)
    os.remove(png)
    return fmt


# ------------------------------------------------------------------ font

def convert_font(bin_path, ntx_path, out_dir, name, work):
    """KA3D bitmap font (glyph table + tall atlas) -> BFT1 table + 512x512 pages."""
    data = open(bin_path, "rb").read()
    count, line_h, size_b, _ = struct.unpack_from(">4H", data, 0)
    glyphs = [struct.unpack_from(">7H", data, 8 + 14 * i) for i in range(count)]
    if 8 + 14 * count != len(data):
        raise ValueError(f"{bin_path}: unexpected size")
    W, H, rgba, _ = ntx.read_ntx(open(ntx_path, "rb").read())

    # shelf packing, tallest first, 1 px gap
    order = sorted(range(count), key=lambda i: (-glyphs[i][4], -glyphs[i][3]))
    place = {}
    page, x, y, shelf = 0, 0, 0, 0
    for i in order:
        _, _, _, w, h, _, _ = glyphs[i]
        if x + w > FONT_PAGE:
            x, y, shelf = 0, y + shelf + 1, 0
        if y + h > FONT_PAGE:
            page, x, y, shelf = page + 1, 0, 0, 0
        place[i] = (page, x, y)
        x += w + 1
        shelf = max(shelf, h)
    pages = [bytearray(FONT_PAGE * FONT_PAGE * 4) for _ in range(page + 1)]
    for i, (p, px, py) in place.items():
        _, sx, sy, w, h, _, _ = glyphs[i]
        for row in range(h):
            s = ((sy + row) * W + sx) * 4
            d = ((py + row) * FONT_PAGE + px) * 4
            pages[p][d:d + 4 * w] = rgba[s:s + 4 * w]
    table = bytearray(struct.pack("<4sHHHHHH", b"BFT1", count, len(pages), line_h, size_b,
                                  FONT_PAGE, FONT_PAGE))
    for i in sorted(range(count), key=lambda i: glyphs[i][0]):
        code, _, _, w, h, bearing, _ = glyphs[i]
        p, px, py = place[i]
        table += struct.pack("<IHHBBBb", code, px, py, w, h, p, bearing)
    open(os.path.join(out_dir, name + ".bft"), "wb").write(table)
    for p, img in enumerate(pages):
        png = os.path.join(work, f"{name}_{p}.png")
        ntx.write_png(png, FONT_PAGE, FONT_PAGE, bytes(img))
        run([TEX3DS, "-f", "rgba4", "-z", "auto", "-o", os.path.join(out_dir, f"{name}_{p}.t3x"), png])
        os.remove(png)
    return count, len(pages)


# ------------------------------------------------------------------ main steps

def locate_data(src, work, force):
    if os.path.isdir(src):
        root = src
        source_id = "folder " + os.path.abspath(src)
    else:
        digest = sha256(src)
        known = KNOWN_SIS.get(digest)
        if not known and not force:
            raise SystemExit(f"Unknown .sis (sha256 {digest}). This converter was written for "
                             f"{', '.join(KNOWN_SIS.values())}. Use --force to try anyway.")
        log(f"[sis] {known or 'unknown version'} ({digest[:16]}…)")
        root = os.path.join(work, "sis")
        if os.path.isdir(root):
            shutil.rmtree(root)
        _, files = sis_extract.extract(src, root)
        log(f"[sis] {len(files)} files extracted, SHA-1 verified")
        source_id = f"sis sha256 {digest}"
    data = os.path.join(root, *DATA.split("/"))
    if not os.path.isdir(data):
        raise SystemExit(f"{data} not found: is this Bounce Touch?")
    return data, source_id


def main(argv):
    import argparse
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", help="Bounce Touch .sis or the folder it was extracted to")
    ap.add_argument("romfs", help="output folder (replaced)")
    ap.add_argument("--work", help="scratch folder (default: convert_work next to romfs)")
    ap.add_argument("--force", action="store_true", help="accept an unknown .sis")
    a = ap.parse_args(argv)
    src, out, force = a.source, a.romfs, a.force
    work = a.work or os.path.join(os.path.dirname(os.path.abspath(out)), "convert_work")
    for tool in (TEX3DS, OGGENC):
        if not os.path.isfile(tool):
            raise SystemExit(f"missing tool: {tool}")
    t0 = time.time()
    os.makedirs(work, exist_ok=True)
    data, source_id = locate_data(src, work, force)
    res = os.path.join(data, "resources")

    if os.path.isdir(out):
        shutil.rmtree(out)
    dirs = {k: os.path.join(out, k) for k in ("scenes", "tex", "ui", "font", "audio/music",
                                               "audio/sfx", "text", "particles")}
    for d in dirs.values():
        os.makedirs(d, exist_ok=True)
    tmp = os.path.join(work, "tmp")
    os.makedirs(tmp, exist_ok=True)
    counts = {}

    # scenes
    tex_names = {os.path.splitext(f)[0].lower() for f in files_in(os.path.join(res, "textures"), ".ntx")}

    def tex_name(hgr_tex):
        key = os.path.splitext(os.path.basename(hgr_tex.replace("\\", "/")))[0].lower()
        if key not in tex_names:
            raise ValueError(f"texture {hgr_tex} has no .ntx")
        return key
    scenes = [os.path.join(data, "bundles", "original", f) for f in files_in(os.path.join(data, "bundles", "original"), ".hgr")]
    scenes += [os.path.join(res, "models", f) for f in files_in(os.path.join(res, "models"), ".hgr")]
    for path in scenes:
        scene = hgr.load(path)
        blob = bsc.write(scene, tex_name)
        bsc.compare(bsc.read(blob), scene)
        name = os.path.splitext(os.path.basename(path))[0].lower()
        open(os.path.join(dirs["scenes"], name + ".bsc"), "wb").write(blob)
    counts["scenes"] = len(scenes)
    log(f"[scenes] {len(scenes)} .hgr -> .bsc (round-trip checked)")

    jobs = []
    with cf.ThreadPoolExecutor(max_workers=os.cpu_count() or 4) as pool:
        for f in files_in(os.path.join(res, "textures"), ".ntx"):
            n = os.path.splitext(f)[0].lower()
            jobs.append(("tex", pool.submit(ntx_to_t3x, os.path.join(res, "textures", f),
                                            os.path.join(dirs["tex"], n + ".t3x"), tmp, True)))
        for f in files_in(os.path.join(res, "images"), ".ntx"):
            n = os.path.splitext(f)[0].lower()
            if n in SKIP_IMAGES or n.startswith("font_"):
                continue
            jobs.append(("ui", pool.submit(ntx_to_t3x, os.path.join(res, "images", f),
                                           os.path.join(dirs["ui"], n + ".t3x"), tmp, False)))
        for sub, key in (("musics", "audio/music"), ("sounds", "audio/sfx")):
            for f in files_in(os.path.join(res, sub), ".wav"):
                n = os.path.splitext(f)[0].lower()
                serial = str(zlib.crc32(n.encode()) & 0x7FFFFFFF)  # fixed: reproducible output
                jobs.append((key, pool.submit(run, [OGGENC, "-Q", "-q", OGG_QUALITY, "-s", serial, "-o",
                                                    os.path.join(dirs[key], n + ".ogg"),
                                                    os.path.join(res, sub, f)])))
        formats = {}
        for kind, fut in jobs:
            r = fut.result()
            counts[kind] = counts.get(kind, 0) + 1
            if kind in ("tex", "ui"):
                formats[r] = formats.get(r, 0) + 1
    log(f"[tex] {counts.get('tex', 0)} 3D textures (mipmapped), {counts.get('ui', 0)} UI images; formats {formats}")
    log(f"[audio] {counts.get('audio/music', 0)} music + {counts.get('audio/sfx', 0)} sfx -> Ogg Vorbis q{OGG_QUALITY}")

    loc = os.path.join(res, "localization")
    for f in files_in(loc, ".bin"):
        if f.lower().startswith("font_"):
            name = os.path.splitext(f)[0].lower()[5:]
            ntx_path = os.path.join(res, "images", os.path.splitext(f)[0] + ".ntx")
            n, p = convert_font(os.path.join(loc, f), ntx_path, dirs["font"], name, tmp)
            log(f"[font] {name}: {n} glyphs on {p} pages of {FONT_PAGE}x{FONT_PAGE}")
        else:
            shutil.copyfile(os.path.join(loc, f), os.path.join(dirs["text"], f.lower()))
    for f in files_in(os.path.join(res, "textures"), ".prs"):
        shutil.copyfile(os.path.join(res, "textures", f), os.path.join(dirs["particles"], f.lower()))
    log("[text] localization.bin copied; [particles] .prs copied")

    shutil.rmtree(tmp, ignore_errors=True)
    total = sum(os.path.getsize(os.path.join(dp, f)) for dp, _, fs in os.walk(out) for f in fs)
    nfiles = sum(len(fs) for _, _, fs in os.walk(out))
    with open(os.path.join(out, "manifest.txt"), "w", encoding="utf-8") as m:
        m.write(f"converter {CONVERTER_VERSION}\nsource {source_id}\nfiles {nfiles}\nbytes {total}\n")
    log(f"done: {nfiles} files, {total / 1048576:.2f} MB in {out} ({time.time() - t0:.1f} s)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
