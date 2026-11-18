#!/usr/bin/env python3
"""KA3D .hgr scene reader (version 1.93, Nokia N3D platform) — pure Python.

Format summary (see docs/HGR.md for the full description):
  structured fields are big-endian; raw vertex/index arrays are little-endian.
  header, fog, then 10 sections each preceded by a check id 0x12345600+n:
  0 textures, 1 materials (no id between 0 and 1), 2 primitives... see SECTION_IDS.

Usage:
  hgr.py info  scene.hgr              summary
  hgr.py tree  scene.hgr              node hierarchy with types
  hgr.py props scene.hgr              user properties (level logic)
  hgr.py json  scene.hgr [-o f.json]  everything except vertex arrays
"""
import json
import struct
import sys
from dataclasses import dataclass, field

MAGIC = b"hgrf"
CHECK_ID = 0x12345600

VERTEX_FORMAT_SIZES = {
    "S_32": 4, "S_16": 2, "S_8": 1,
    "V2_32": 8, "V2_16": 4, "V2_8": 2,
    "V3_32": 12, "V3_16": 6, "V3_8": 3,
    "V4_32": 16, "V4_16": 8, "V4_8": 4, "V4_5": 2,
    "V2_16F": 4, "V3_16F": 6, "V4_16F": 8,
}
# Order in which vertex arrays are stored (KA3D VertexFormat::DataType order).
VERTEX_DATA_ORDER = ["DT_POSITION", "DT_POSITIONT", "DT_BONEWEIGHTS", "DT_BONEINDICES",
                     "DT_NORMAL", "DT_DIFFUSE", "DT_SPECULAR", "DT_TEX0", "DT_TEX1",
                     "DT_TEX2", "DT_TEX3", "DT_TANGENT"]
PRIM_TYPES = ["point", "line", "linestrip", "tri", "tristrip", "trifan", "sprite"]
FOG_TYPES = ["none", "linear", "exp", "exp2"]
LIGHT_TYPES = ["unknown", "directional", "omni", "spot"]
END_BEHAVIOURS = ["repeat", "oscillate", "reset", "stop"]
PLATFORMS = ["dx", "egl", "psp", "sw", "n3d"]


class HgrError(ValueError):
    pass


class Reader:
    def __init__(self, data):
        self.d = data
        self.p = 0

    def _take(self, fmt):
        try:
            v = struct.unpack_from(">" + fmt, self.d, self.p)
        except struct.error as e:
            raise HgrError(f"unexpected end of file at {self.p:#x}") from e
        self.p += struct.calcsize(">" + fmt)
        return v

    def i8(self): return self._take("b")[0]
    def u8(self): return self._take("B")[0]
    def i16(self): return self._take("h")[0]
    def u16(self): return self._take("H")[0]
    def i32(self): return self._take("i")[0]
    def u32(self): return self._take("I")[0]
    def f32(self): return self._take("f")[0]
    def floats(self, n): return list(self._take("%df" % n))

    def utf(self):
        n = self.u16()
        s = self.d[self.p:self.p + n]
        if len(s) != n:
            raise HgrError(f"unexpected end of file at {self.p:#x}")
        self.p += n
        return s.decode("utf-8", errors="replace")

    def raw(self, n):
        b = self.d[self.p:self.p + n]
        if len(b) != n:
            raise HgrError(f"unexpected end of file at {self.p:#x}")
        self.p += n
        return b

    def count(self, what, limit=1_000_000):
        n = self.i32()
        if not 0 <= n <= limit:
            raise HgrError(f"bad {what} count {n} at {self.p - 4:#x}")
        return n


@dataclass
class Material:
    name: str
    shader: str
    flags: int
    textures: dict      # param name -> texture index
    vectors: dict       # param name -> [x, y, z, w]
    scalars: dict       # param name -> float


@dataclass
class Primitive:
    material: int
    prim_type: str
    vertex_format: list     # [(data type, data format)]
    pos_scale_bias: list    # [scale, bx, by, bz]: pos = int16 * scale + bias
    uv_scale_bias: list     # [scale, bu, bv, -]
    arrays: dict            # data type -> raw little-endian bytes
    indices: list
    used_bones: list
    vertex_count: int

    def positions(self):
        """Decoded positions [(x, y, z)] in scene units."""
        df = dict(self.vertex_format)["DT_POSITION"]
        if df != "V3_16":
            raise HgrError(f"position format {df} not supported")
        s, bx, by, bz = self.pos_scale_bias
        v = struct.unpack("<%dh" % (3 * self.vertex_count), self.arrays["DT_POSITION"])
        return [(v[i] * s + bx, v[i + 1] * s + by, v[i + 2] * s + bz) for i in range(0, len(v), 3)]

    def uvs(self):
        """Decoded texture coordinates [(u, v)] (layer 0)."""
        df = dict(self.vertex_format).get("DT_TEX0")
        if df is None:
            return None
        if df != "V2_16":
            raise HgrError(f"uv format {df} not supported")
        s, bu, bv, _ = self.uv_scale_bias
        v = struct.unpack("<%dh" % (2 * self.vertex_count), self.arrays["DT_TEX0"])
        return [(v[i] * s + bu, v[i + 1] * s + bv) for i in range(0, len(v), 2)]


@dataclass
class Node:
    kind: str           # mesh, camera, light, dummy, lines, node
    name: str
    transform: list     # 3x4 row-major; column 3 is translation
    flags: int
    node_id: int
    parent: int         # index into Scene.nodes, -1 = scene root
    extra: dict = field(default_factory=dict)


@dataclass
class Scene:
    version: int
    exporter: int
    data_flags: int
    platform: int
    fog: dict
    textures: list      # [(file name, type)]
    materials: list
    primitives: list
    nodes: list
    animations: dict    # node name -> dict
    user_properties: dict   # node name -> raw text
    trailing: int = 0   # bytes after the last section


def _vec_array16(r, count, dim):
    """KA3D readFloat{3,4}Array16: >2 keys are quantised to u16 between min and max."""
    if count > 2:
        mn = r.floats(dim)
        mx = r.floats(dim)
        out = []
        for _ in range(count):
            q = r._take("%dH" % dim)
            out.append([mn[k] + (mx[k] - mn[k]) * (q[k] / 65535.0) for k in range(dim)])
        return out
    return [r.floats(dim) for _ in range(count)]


def _keyframes(r):
    keys = r.count("keyframe", 1_000_000)
    dim = r.i32()
    if dim not in (3, 4):
        raise HgrError(f"keyframe dimension {dim} at {r.p - 4:#x}")
    return _vec_array16(r, keys, dim)


def _transform_animation(r, version):
    pos_rate, rot_rate, scl_rate, end = r.u8(), r.u8(), r.u8(), r.u8()
    if end >= len(END_BEHAVIOURS):
        raise HgrError(f"bad end behaviour {end}")
    optimized = r.u8() != 0 if version >= 192 else False
    anim = {"key_rate": [pos_rate, rot_rate, scl_rate], "end": END_BEHAVIOURS[end],
            "optimized": optimized}
    if not optimized:
        anim["pos"], anim["rot"], anim["scl"] = _keyframes(r), _keyframes(r), _keyframes(r)
    else:
        anim["pos"] = _vec_array16(r, r.count("pos key"), 4)
        anim["rot"] = _keyframes(r)
        anim["scl"] = _vec_array16(r, r.count("scale key"), 4)
        if version >= 193:
            anim["end_time"] = r.f32()
    return anim


def _check(r, n):
    v = r.u32()
    if v != CHECK_ID + n:
        raise HgrError(f"check id {v:#x} at {r.p - 4:#x}, expected {CHECK_ID + n:#x}")


def _primitive(r, version):
    verts = r.count("vertex", 0xFFFF)
    inds = r.count("index")
    vf = []
    for _ in range(r.u8()):
        dt, df = r.utf(), r.utf()
        if dt not in VERTEX_DATA_ORDER or df not in VERTEX_FORMAT_SIZES:
            raise HgrError(f"unknown vertex format {dt}={df}")
        vf.append((dt, df))
    mat = r.i16()
    ptype = r.i16()
    if not 0 <= ptype < len(PRIM_TYPES):
        raise HgrError(f"bad primitive type {ptype}")
    psb = r.floats(4)
    usb = r.floats(4)
    fmt = dict(vf)
    arrays = {}
    for dt in VERTEX_DATA_ORDER:
        if dt in fmt:
            arrays[dt] = r.raw(verts * VERTEX_FORMAT_SIZES[fmt[dt]])
    indices = list(struct.unpack("<%dH" % inds, r.raw(2 * inds)))
    if indices and max(indices) >= verts:
        raise HgrError("index out of range")
    bones = list(r.raw(r.u8()))
    return Primitive(mat, PRIM_TYPES[ptype], vf, psb, usb, arrays, indices, bones, verts)


def _node(r, kind, version):
    name = r.utf()
    tm = r.floats(12)
    flags = r.i32()
    node_id = r.i32() if version > 190 else -1
    parent = r.i32()
    return Node(kind, name, [tm[0:4], tm[4:8], tm[8:12]], flags, node_id, parent)


def parse(data):
    r = Reader(data)
    if r.raw(4) != MAGIC:
        raise HgrError("not an hgr file (magic)")
    version = r.i16()
    if not 170 <= version <= 193:
        raise HgrError(f"unsupported version {version}")
    exporter = r.i32() if version > 190 else 0x020903
    data_flags = r.u16()
    platform = r.i16() if version >= 180 else 0
    fog_type = r.u8()
    fog = {"type": FOG_TYPES[fog_type] if fog_type < len(FOG_TYPES) else fog_type,
           "start": r.f32(), "end": r.f32(), "color": r.floats(3)}

    _check(r, 0)
    textures = []
    for _ in range(r.count("texture")):
        name = r.utf()
        textures.append((name, r.i32() if version >= 160 else 0))

    materials = []
    for _ in range(r.count("material")):
        name, shader, flags = r.utf(), r.utf(), r.i32()
        tex = {r.utf(): r.i16() for _ in range(r.u8())}
        vec = {r.utf(): r.floats(4) for _ in range(r.u8())}
        sca = {r.utf(): r.f32() for _ in range(r.u8())}
        for k, ix in tex.items():
            if not 0 <= ix < len(textures):
                raise HgrError(f"material {name}: bad texture index {ix}")
        materials.append(Material(name, shader, flags, tex, vec, sca))

    _check(r, 1)
    primitives = [_primitive(r, version) for _ in range(r.count("primitive"))]
    for p in primitives:
        if not 0 <= p.material < len(materials):
            raise HgrError(f"bad material index {p.material}")

    nodes = []
    _check(r, 2)
    for _ in range(r.count("mesh")):
        n = _node(r, "mesh", version)
        n.extra["primitives"] = [r.i32() for _ in range(r.count("mesh primitive"))]
        n.extra["bones"] = [{"node": r.i32(), "inv_rest": r.floats(12)}
                            for _ in range(r.count("bone"))]
        nodes.append(n)

    _check(r, 3)
    for _ in range(r.count("camera")):
        n = _node(r, "camera", version)
        n.extra.update(front=r.f32(), back=r.f32(), fov=r.f32())
        nodes.append(n)

    _check(r, 4)
    for _ in range(r.count("light")):
        n = _node(r, "light", version)
        n.extra["color"] = r.floats(3)
        r.floats(2)  # unused near attenuation
        n.extra.update(far_atten_start=r.f32(), far_atten_end=r.f32(),
                       inner_cone=r.f32(), outer_cone=r.f32())
        t = r.u8()
        n.extra["type"] = LIGHT_TYPES[t] if t < len(LIGHT_TYPES) else t
        nodes.append(n)

    _check(r, 5)
    for _ in range(r.count("dummy")):
        n = _node(r, "dummy", version)
        n.extra.update(box_min=r.floats(3), box_max=r.floats(3))
        nodes.append(n)

    _check(r, 6)
    for _ in range(r.count("lines")):
        n = _node(r, "lines", version)
        nl, npath = r.count("line", 100000), r.count("path", 100000)
        n.extra["lines"] = [(r.floats(3), r.floats(3)) for _ in range(nl)]
        n.extra["paths"] = [(r.i32(), r.i32()) for _ in range(npath)]
        nodes.append(n)

    _check(r, 7)
    for _ in range(r.count("node")):
        nodes.append(_node(r, "node", version))

    for i, n in enumerate(nodes):
        if n.parent >= len(nodes):
            raise HgrError(f"node {n.name}: bad parent {n.parent}")
        for p in n.extra.get("primitives", []):
            if not 0 <= p < len(primitives):
                raise HgrError(f"mesh {n.name}: bad primitive {p}")

    _check(r, 8)
    animations = {}
    for _ in range(r.count("animation")):
        name = r.utf()
        animations[name] = _transform_animation(r, version)

    _check(r, 9)
    props = {}
    for _ in range(r.count("user property")):
        key = r.utf()
        props[key] = r.utf()

    return Scene(version, exporter, data_flags, platform, fog, textures, materials,
                 primitives, nodes, animations, props, trailing=len(data) - r.p)


def load(path):
    with open(path, "rb") as f:
        return parse(f.read())


def parse_properties(text):
    """Split a user-property block into [(key, value)] (one 'key=value' per line)."""
    out = []
    for line in text.replace("\r", "\n").split("\n"):
        line = line.strip()
        if not line:
            continue
        k, sep, v = line.partition("=")
        out.append((k.strip(), v.strip() if sep else None))
    return out


def summary(s):
    kinds = {}
    for n in s.nodes:
        kinds[n.kind] = kinds.get(n.kind, 0) + 1
    tris = sum(len(p.indices) // 3 for p in s.primitives)
    verts = sum(p.vertex_count for p in s.primitives)
    return {
        "version": s.version, "exporter": f"{s.exporter:#08x}",
        "platform": PLATFORMS[s.platform] if s.platform < len(PLATFORMS) else s.platform,
        "fog": s.fog["type"], "textures": len(s.textures), "materials": len(s.materials),
        "primitives": len(s.primitives), "vertices": verts, "triangles": tris,
        "nodes": kinds, "animations": len(s.animations),
        "animated": sum(1 for a in s.animations.values()
                        if len(a["pos"]) > 1 or len(a["rot"]) > 1 or len(a["scl"]) > 1),
        "user_properties": len(s.user_properties), "trailing_bytes": s.trailing,
    }


def to_json(s):
    def prim(p):
        return {"material": p.material, "type": p.prim_type,
                "format": [f"{a}={b}" for a, b in p.vertex_format],
                "vertices": p.vertex_count, "indices": len(p.indices),
                "pos_scale_bias": p.pos_scale_bias, "uv_scale_bias": p.uv_scale_bias,
                "bones": p.used_bones}
    return {
        "summary": summary(s), "fog": s.fog,
        "textures": [t for t, _ in s.textures],
        "materials": [m.__dict__ for m in s.materials],
        "primitives": [prim(p) for p in s.primitives],
        "nodes": [n.__dict__ for n in s.nodes],
        "animations": s.animations,
        "user_properties": s.user_properties,
    }


def main(argv):
    if len(argv) < 2 or argv[0] not in ("info", "tree", "props", "json"):
        print(__doc__)
        return 2
    cmd, path = argv[0], argv[1]
    s = load(path)
    if cmd == "info":
        for k, v in summary(s).items():
            print(f"{k:16s} {v}")
    elif cmd == "tree":
        children = {}
        for i, n in enumerate(s.nodes):
            children.setdefault(n.parent, []).append(i)

        def walk(i, depth):
            n = s.nodes[i]
            extra = ""
            if n.kind == "mesh":
                extra = f" prims={len(n.extra['primitives'])}"
            anim = s.animations.get(n.name)
            if anim and (len(anim["pos"]) > 1 or len(anim["rot"]) > 1):
                extra += " [anim]"
            if n.name in s.user_properties:
                extra += " [props]"
            print("  " * depth + f"{n.name} ({n.kind}){extra}")
            for c in children.get(i, []):
                walk(c, depth + 1)
        for root in children.get(-1, []):
            walk(root, 0)
    elif cmd == "props":
        for key, text in s.user_properties.items():
            print(f"[{key}]")
            for k, v in parse_properties(text):
                print(f"  {k}" + (f" = {v}" if v is not None else ""))
    elif cmd == "json":
        out = json.dumps(to_json(s), indent=1, ensure_ascii=False)
        if "-o" in argv:
            open(argv[argv.index("-o") + 1], "w", encoding="utf-8").write(out)
        else:
            print(out)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
