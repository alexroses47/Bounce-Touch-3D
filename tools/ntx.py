#!/usr/bin/env python3
"""KA3D .ntx image reader and PNG writer (pure Python, no dependencies).

Layout (all little-endian):
  header  7 x u16: version, width, height, format, palettesize, flags, userflags
  palette palettesize entries in `format` (only if palettesize > 0)
  pixels  width*height: u8 palette indices if palettesize > 0, else u16 in `format`
flags: 1 = color key (palette index 0 / colour 0 is transparent), 2 = compressed (unused here).

Formats seen in Bounce Touch (checked visually: the ball and logo are red):
  7  = R5G6B5   (red in the top 5 bits)
  16 = 4444 with nibbles A,R,G,B from the top bit down. The public ka3d enum
       calls value 16 A4B4G4R4; this build stores ARGB.

Usage: ntx.py input.ntx [more.ntx ...] -o outdir     (writes outdir/<name>.png)
"""
import os
import struct
import sys
import zlib

R5G6B5 = 7
ARGB4444 = 16


class NtxError(ValueError):
    pass


def _rgba_565(v):
    r = (v >> 11) & 0x1F
    g = (v >> 5) & 0x3F
    b = v & 0x1F
    return ((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2), 255)


def _rgba_4444(v):
    a = (v >> 12) & 0xF
    r = (v >> 8) & 0xF
    g = (v >> 4) & 0xF
    b = v & 0xF
    return (r * 17, g * 17, b * 17, a * 17)


DECODERS = {R5G6B5: _rgba_565, ARGB4444: _rgba_4444}


def read_ntx(data):
    """Return (width, height, rgba_bytes, info dict)."""
    if len(data) < 14:
        raise NtxError("file too small")
    version, w, h, fmt, palsize, flags, userflags = struct.unpack_from("<7H", data)
    if fmt not in DECODERS:
        raise NtxError(f"unsupported pixel format {fmt}")
    if flags & 2:
        raise NtxError("compressed NTX not supported")
    conv = DECODERS[fmt]
    colorkey = bool(flags & 1)
    pos = 14
    out = bytearray(w * h * 4)
    if palsize:
        pal = [conv(v) for v in struct.unpack_from("<%dH" % palsize, data, pos)]
        pos += palsize * 2
        if colorkey:
            pal[0] = (pal[0][0], pal[0][1], pal[0][2], 0)
        if pos + w * h > len(data):
            raise NtxError("truncated pixel data")
        idx = data[pos:pos + w * h]
        lut = [bytes(c) for c in pal] + [b"\0\0\0\0"] * (256 - len(pal))
        out = b"".join(lut[i] for i in idx)
    else:
        if pos + w * h * 2 > len(data):
            raise NtxError("truncated pixel data")
        px = struct.unpack_from("<%dH" % (w * h), data, pos)
        cache = {}
        parts = []
        for v in px:
            c = cache.get(v)
            if c is None:
                r, g, b, a = conv(v)
                if colorkey and v == 0:
                    a = 0
                c = cache[v] = bytes((r, g, b, a))
            parts.append(c)
        out = b"".join(parts)
    info = {"version": version, "width": w, "height": h, "format": fmt,
            "palette": palsize, "colorkey": colorkey, "userflags": userflags}
    return w, h, bytes(out), info


def png_bytes(w, h, rgba):
    def chunk(tag, body):
        return struct.pack(">I", len(body)) + tag + body + struct.pack(">I", zlib.crc32(tag + body))
    stride = w * 4
    raw = b"".join(b"\0" + rgba[y * stride:(y + 1) * stride] for y in range(h))
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0)) \
        + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")


def write_png(path, w, h, rgba):
    with open(path, "wb") as f:
        f.write(png_bytes(w, h, rgba))


def main(argv):
    if "-o" not in argv or argv.index("-o") == len(argv) - 1:
        print(__doc__)
        return 2
    i = argv.index("-o")
    outdir = argv[i + 1]
    inputs = argv[:i] + argv[i + 2:]
    os.makedirs(outdir, exist_ok=True)
    for p in inputs:
        w, h, rgba, info = read_ntx(open(p, "rb").read())
        name = os.path.splitext(os.path.basename(p))[0] + ".png"
        write_png(os.path.join(outdir, name), w, h, rgba)
    print(f"{len(inputs)} images -> {outdir}")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
