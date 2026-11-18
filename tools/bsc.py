#!/usr/bin/env python3
"""BSC1: scene format for the 3DS port (written from a parsed .hgr).

Little-endian, every table 4-byte aligned, all offsets absolute from the file
start, so the 3DS loads the file once into linear memory and points the GPU at
the vertex and index blocks directly. Full layout with C structs: docs/ROMFS.md.

Usage: bsc.py check file.bsc [scene.hgr]   decode; with an .hgr, compare both
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hgr  # noqa: E402

MAGIC = b"BSC1"
VERSION = 1

KINDS = {"mesh": 0, "camera": 1, "light": 2, "dummy": 3, "lines": 4, "node": 5}
ROLES = {"visual": 0, "collision": 1, "skybox": 2, "trigger": 3, "dummy": 4, "other": 5}
MAT_TEXTURED, MAT_MULTIPLY, MAT_NO_ZWRITE = 1, 2, 4

HEADER = struct.Struct("<4sII" + "II" * 9)          # 84 bytes
TEXTURE = struct.Struct("<I")                        # name
MATERIAL = struct.Struct("<IiII")                    # flags, texture, rgba, name
PRIMITIVE = struct.Struct("<IIIII4ff6f")             # 64 bytes
NODE = struct.Struct("<IiBBH12fII6fiI")              # 100 bytes
ANIM = struct.Struct("<4BIIIIIIIf")                  # 36 bytes
VERTEX = struct.Struct("<6h")                        # x y z u v pad = 12 bytes


def _align(b, n=4):
    while len(b) % n:
        b.append(0)


class Strings:
    def __init__(self):
        self.data = bytearray(b"\0")
        self.index = {"": 0}

    def add(self, s):
        if s not in self.index:
            self.index[s] = len(self.data)
            self.data += s.encode("utf-8") + b"\0"
        return self.index[s]


def node_role(scene, node, is_sky):
    props = scene.user_properties.get(node.name, "")
    low = props.lower().replace(" ", "")
    if is_sky and node.kind == "mesh":
        return "skybox"
    if node.kind == "mesh":
        return "collision" if "collisionmesh=this" in low else "visual"
    if node.kind == "dummy":
        return "trigger" if node.name.startswith("T_") else "dummy"
    return "other"


def sky_set(scene):
    """Indices of nodes named *skybox* or with a `skybox` property != 0, plus subtrees."""
    def is_sky(n):
        if "skybox" in n.name.lower():
            return True
        props = hgr.parse_properties(scene.user_properties.get(n.name, ""))
        return any(k.lower() == "skybox" and v not in (None, "0") for k, v in props)
    own = [is_sky(n) for n in scene.nodes]
    out = set()
    for i, n in enumerate(scene.nodes):
        j = i
        while j >= 0:
            if own[j]:
                out.add(i)
                break
            j = scene.nodes[j].parent
    return out


def write(scene, texture_name):
    """scene: hgr.Scene; texture_name(hgr texture file) -> romfs texture name."""
    strings = Strings()
    out = bytearray(HEADER.size)

    # textures
    tex_off = len(out)
    for name, _ in scene.textures:
        out += TEXTURE.pack(strings.add(texture_name(name)))

    # materials
    mat_off = len(out)
    for m in scene.materials:
        flags, tex = 0, -1
        if "BASEMAP" in m.textures:
            flags |= MAT_TEXTURED
            tex = m.textures["BASEMAP"]
        if "-mul" in m.shader:
            flags |= MAT_MULTIPLY
        if "nozwrite" in m.shader:
            flags |= MAT_NO_ZWRITE
        c = m.vectors.get("DIFFUSEC", [1, 1, 1, 1])
        rgba = sum(max(0, min(255, round(v * 255))) << (8 * k) for k, v in enumerate(c[:3])) | (255 << 24)
        out += MATERIAL.pack(flags, tex, rgba, strings.add(m.shader))

    # vertex / index blocks (filled below, placed after the tables)
    vbo, ibo = bytearray(), bytearray()
    prim_rows = []
    for p in scene.primitives:
        fmt = dict(p.vertex_format)
        if fmt.get("DT_POSITION") != "V3_16" or fmt.get("DT_TEX0", "V2_16") != "V2_16" \
                or p.prim_type != "tri" or len(fmt) > 2:
            raise ValueError(f"unsupported primitive {p.vertex_format} {p.prim_type}")
        n = p.vertex_count
        pos = struct.unpack("<%dh" % (3 * n), p.arrays["DT_POSITION"])
        uv = struct.unpack("<%dh" % (2 * n), p.arrays["DT_TEX0"]) if "DT_TEX0" in p.arrays else (0,) * (2 * n)
        voff = len(vbo)
        for i in range(n):
            vbo += VERTEX.pack(pos[3 * i], pos[3 * i + 1], pos[3 * i + 2], uv[2 * i], uv[2 * i + 1], 0)
        ioff = len(ibo)
        ibo += struct.pack("<%dH" % len(p.indices), *p.indices)
        _align(ibo)
        s, bx, by, bz = p.pos_scale_bias
        xs, ys, zs = pos[0::3], pos[1::3], pos[2::3]
        bbox = [min(xs) * s + bx, min(ys) * s + by, min(zs) * s + bz,
                max(xs) * s + bx, max(ys) * s + by, max(zs) * s + bz]
        prim_rows.append([p.material, n, len(p.indices), voff, ioff, s, bx, by, bz,
                          p.uv_scale_bias[0], *bbox])

    # animations (dequantised floats)
    anim_index = {}
    anim_rows, anim_data = [], bytearray()
    for name, a in scene.animations.items():
        moving = len(a["pos"]) > 1 or len(a["rot"]) > 1 or len(a["scl"]) > 1
        if not moving:
            continue
        rows = []
        for keys in (a["pos"], a["rot"], a["scl"]):
            dim = len(keys[0]) if keys else 4
            rows.append((len(keys), dim, len(anim_data)))
            for k in keys:
                anim_data += struct.pack("<%df" % dim, *k)
        anim_index[name] = len(anim_rows)
        end = ["repeat", "oscillate", "reset", "stop"].index(a["end"])
        anim_rows.append((a["key_rate"], end, rows, a.get("end_time", 0.0)))

    # nodes + primitive reference list
    sky = sky_set(scene)
    primrefs = []
    node_rows = []
    for ni, n in enumerate(scene.nodes):
        params = [0.0] * 6
        if n.kind == "dummy":
            params = n.extra["box_min"] + n.extra["box_max"]
        elif n.kind == "camera":
            params = [n.extra["front"], n.extra["back"], n.extra["fov"], 0, 0, 0]
        first = len(primrefs)
        prims = n.extra.get("primitives", [])
        primrefs += prims
        props = scene.user_properties.get(n.name, "")
        node_rows.append((strings.add(n.name), n.parent, KINDS[n.kind], ROLES[node_role(scene, n, ni in sky)], 0,
                          *[v for row in n.transform for v in row], first, len(prims), *params,
                          anim_index.get(n.name, -1), strings.add(props)))

    prim_off = len(out)
    out += b"\0" * (PRIMITIVE.size * len(prim_rows))
    node_off = len(out)
    for r in node_rows:
        out += NODE.pack(*r)
    ref_off = len(out)
    out += struct.pack("<%dI" % len(primrefs), *primrefs)
    anim_off = len(out)
    out += b"\0" * (ANIM.size * len(anim_rows))
    anim_data_off = len(out)
    out += anim_data
    str_off = len(out)
    out += strings.data
    _align(out, 16)
    vbo_off = len(out)
    out += vbo
    _align(out, 16)
    ibo_off = len(out)
    out += ibo
    _align(out)

    for i, r in enumerate(prim_rows):
        r[3] += vbo_off
        r[4] += ibo_off
        PRIMITIVE.pack_into(out, prim_off + i * PRIMITIVE.size, *r)
    for i, (rates, end, rows, end_time) in enumerate(anim_rows):
        (pc, pd, po), (rc, rd, ro), (sc, sd, so) = rows
        ANIM.pack_into(out, anim_off + i * ANIM.size, rates[0], rates[1], rates[2], end,
                       pc, po + anim_data_off, rc, rd, ro + anim_data_off, sc, so + anim_data_off, end_time)

    HEADER.pack_into(out, 0, MAGIC, VERSION, len(out),
                     str_off, len(strings.data),
                     len(scene.textures), tex_off,
                     len(scene.materials), mat_off,
                     len(prim_rows), prim_off,
                     len(node_rows), node_off,
                     len(anim_rows), anim_off,
                     len(primrefs), ref_off,
                     vbo_off, len(vbo),
                     ibo_off, len(ibo))
    return bytes(out)


def read(data):
    h = HEADER.unpack_from(data, 0)
    magic, version, size = h[0:3]
    if magic != MAGIC or version != VERSION or size != len(data):
        raise ValueError("not a BSC1 file")
    (str_off, str_size, ntex, tex_off, nmat, mat_off, nprim, prim_off, nnode, node_off,
     nanim, anim_off, nref, ref_off, vbo_off, vbo_size, ibo_off, ibo_size) = h[3:]

    def s(off):
        return data[str_off + off:data.index(b"\0", str_off + off)].decode("utf-8")
    textures = [s(TEXTURE.unpack_from(data, tex_off + 4 * i)[0]) for i in range(ntex)]
    materials = [MATERIAL.unpack_from(data, mat_off + MATERIAL.size * i) for i in range(nmat)]
    prims = []
    for i in range(nprim):
        r = PRIMITIVE.unpack_from(data, prim_off + PRIMITIVE.size * i)
        mat, nv, ni, vo, io = r[0:5]
        verts = [VERTEX.unpack_from(data, vo + 12 * k) for k in range(nv)]
        idx = list(struct.unpack_from("<%dH" % ni, data, io))
        prims.append({"material": mat, "verts": verts, "indices": idx, "scale_bias": r[5:9],
                      "uv_scale": r[9], "bbox": r[10:16]})
    refs = list(struct.unpack_from("<%dI" % nref, data, ref_off))
    nodes = []
    for i in range(nnode):
        r = NODE.unpack_from(data, node_off + NODE.size * i)
        nodes.append({"name": s(r[0]), "parent": r[1], "kind": r[2], "role": r[3],
                      "transform": r[5:17], "prims": refs[r[17]:r[17] + r[18]],
                      "params": r[19:25], "anim": r[25], "props": s(r[26])})
    anims = [ANIM.unpack_from(data, anim_off + ANIM.size * i) for i in range(nanim)]
    return {"textures": textures, "materials": materials, "primitives": prims,
            "nodes": nodes, "animations": anims}


def compare(bsc, scene):
    """Raise if the decoded BSC differs from the parsed hgr scene."""
    assert len(bsc["primitives"]) == len(scene.primitives)
    for p, q in zip(scene.primitives, bsc["primitives"]):
        n = p.vertex_count
        pos = struct.unpack("<%dh" % (3 * n), p.arrays["DT_POSITION"])
        uv = struct.unpack("<%dh" % (2 * n), p.arrays["DT_TEX0"])
        assert [v[0:3] for v in q["verts"]] == [tuple(pos[3 * i:3 * i + 3]) for i in range(n)]
        assert [v[3:5] for v in q["verts"]] == [tuple(uv[2 * i:2 * i + 2]) for i in range(n)]
        assert q["indices"] == p.indices and q["material"] == p.material
    assert len(bsc["nodes"]) == len(scene.nodes)
    for n, m in zip(scene.nodes, bsc["nodes"]):
        assert m["name"] == n.name and m["parent"] == n.parent
        assert m["prims"] == n.extra.get("primitives", [])
        assert m["props"] == scene.user_properties.get(n.name, "")
        assert all(abs(a - b) <= 1e-5 * max(1, abs(b)) for a, b in
                   zip(m["transform"], [v for row in n.transform for v in row]))
    assert len(bsc["materials"]) == len(scene.materials)
    return True


def main(argv):
    if len(argv) < 2 or argv[0] != "check":
        print(__doc__)
        return 2
    b = read(open(argv[1], "rb").read())
    roles = {}
    for n in b["nodes"]:
        k = [r for r, v in ROLES.items() if v == n["role"]][0]
        roles[k] = roles.get(k, 0) + 1
    print(f"{argv[1]}: {len(b['primitives'])} prims, {len(b['nodes'])} nodes {roles}, "
          f"{len(b['animations'])} anims, textures {len(b['textures'])}")
    if len(argv) > 2:
        compare(b, hgr.load(argv[2]))
        print("matches", argv[2])
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
