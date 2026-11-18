#!/usr/bin/env python3
"""Test diferencial: reglas del port (tests/build/librules.dylib) frente a las funciones
originales de Bounce.exe ejecutadas en Unicorn (tools/e32emu.py).

Uso: tools/.venv/bin/python -I tests/test_rules_diff.py [n_aleatorios]
"""
import ctypes
import math
import os
import random
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.join(HERE, "..")
sys.path.insert(0, os.path.join(ROOT, "tools"))
import e32emu as M  # noqa: E402

lib = ctypes.CDLL(os.path.join(HERE, "build", "librules.dylib"))
F3, F12 = ctypes.c_float * 3, ctypes.c_float * 12
REL = 2e-6   # tolerancia relativa (mismas operaciones en float32; el orden puede variar)


def close(a, b, scale=1.0):
    return all(abs(x - y) <= REL * max(1.0, abs(x), abs(y), scale) for x, y in zip(a, b))


def rnd_vec(r, s=1000.0):
    return [r.uniform(-s, s) for _ in range(3)]


def rnd_mat(r):
    # giro aleatorio con escala y traslación (como los nodos del .hgr)
    ax = rnd_vec(r, 1.0)
    l = math.sqrt(sum(c * c for c in ax)) or 1.0
    ax = [c / l for c in ax]
    R = F12()
    lib.bt_axis_angle(F3(*ax), ctypes.c_float(r.uniform(-math.pi, math.pi)), R)
    m = list(R)
    sc = [r.uniform(0.2, 3.0) for _ in range(3)]
    for i in range(3):
        for j in range(3):
            m[i * 4 + j] *= sc[j]
        m[i * 4 + 3] = r.uniform(-20000, 20000)
    return m


def main(n):
    emu = M.E32Emu(os.path.join(ROOT, "re", "Bounce_unpacked.exe"), os.path.join(ROOT, "re", "imports.tsv"))
    r = random.Random(1234)
    buf_a, buf_b, buf_c, out = emu.alloc(64), emu.alloc(64), emu.alloc(64), emu.alloc(64)
    fails = {}

    def check(name, ok, info):
        if not ok:
            fails.setdefault(name, info)

    for i in range(n):
        m, v, n_ = rnd_mat(r), rnd_vec(r), rnd_vec(r, 1.0)
        # FUN_0007adcc: M(3x3) * v
        emu.write_floats(buf_a, m); emu.write_floats(buf_b, v)
        emu.call(0x7adcc, [out, buf_a, buf_b])
        o = F3(); lib.bt_rot_apply(F12(*m), F3(*v), o)
        check("RotApply / FUN_0007adcc", close(emu.read_floats(out, 3), list(o), 1000), (m, v))
        # FUN_0007af54: inversa afín
        emu.call(0x7af54, [out, buf_a])
        o12 = F12(); lib.bt_affine_inverse(F12(*m), o12)
        check("AffineInverse / FUN_0007af54", close(emu.read_floats(out, 12), list(o12), 20000), m)
        # FUN_000738ec: eje-ángulo
        l = math.sqrt(sum(c * c for c in n_)) or 1.0
        ax = [c / l for c in n_]
        ang = r.uniform(-7, 7)
        emu.write_floats(buf_c, ax)
        emu.call(0x738ec, [out, buf_c, M.bits(ang)])
        lib.bt_axis_angle(F3(*ax), ctypes.c_float(ang), o12)
        ours3 = [o12[k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10)]
        check("AxisAngle / FUN_000738ec", close(emu.read_floats(out, 9), ours3), (ax, ang))
        # FUN_0014a2fc: reflejo con restitución
        e = r.choice([0.0, 0.001, 0.4, 1.0, r.uniform(0, 1)])
        emu.call(0x14a2fc, [out] + [M.bits(x) for x in v] + [M.bits(x) for x in n_] + [M.bits(e)])
        o = F3(); lib.bt_reflect(F3(*v), F3(*n_), ctypes.c_float(e), o)
        check("Reflect / FUN_0014a2fc", close(emu.read_floats(out, 3), list(o), 1000), (v, n_, e))
        # RotateLocal = FUN_0007a984 + FUN_000738ec + FUN_00074054 (+ posición conservada)
        emu.call(0x7a984, [buf_b, buf_a])                    # 3x3 del local
        emu.call(0x738ec, [buf_c, buf_c if False else emu_axis(emu, ax), M.bits(ang)])
        emu.call(0x74054, [out, buf_b, buf_c])               # local * R
        orig = emu.read_floats(out, 9)
        lib.bt_rotate_local(F12(*m), F3(*ax), ctypes.c_float(ang), o12)
        ours = [o12[k] for k in (0, 1, 2, 4, 5, 6, 8, 9, 10)]
        check("RotateLocal / 0x7a984+0x738ec+0x74054", close(orig, ours, 3) and
              all(o12[k] == M.f32(m[k]) for k in (3, 7, 11)), (m, ax, ang))

    # casos límite
    for v, n_, e in (([0, 0, 0], [0, 1, 0], 0.4), ([5, -5, 5], [0, 0, 0], 0.4), ([1, 2, 3], [0, 2, 0], 0.001)):
        emu.call(0x14a2fc, [out] + [M.bits(x) for x in v] + [M.bits(x) for x in n_] + [M.bits(e)])
        o = F3(); lib.bt_reflect(F3(*v), F3(*n_), ctypes.c_float(e), o)
        check("Reflect límite", close(emu.read_floats(out, 3), list(o)), (v, n_, e))

    names = ["RotApply / FUN_0007adcc", "AffineInverse / FUN_0007af54", "AxisAngle / FUN_000738ec",
             "Reflect / FUN_0014a2fc", "RotateLocal / 0x7a984+0x738ec+0x74054", "Reflect límite"]
    for nm in names:
        print(("FALLA " if nm in fails else "ok    ") + nm + (f"  primer caso: {fails[nm]}" if nm in fails else ""))
    print(f"{n} entradas aleatorias por regla + casos límite")
    return 1 if fails else 0


_axis_buf = {}


def emu_axis(emu, ax):
    if "b" not in _axis_buf:
        _axis_buf["b"] = emu.alloc(16)
    emu.write_floats(_axis_buf["b"], ax)
    return _axis_buf["b"]


if __name__ == "__main__":
    sys.exit(main(int(sys.argv[1]) if len(sys.argv) > 1 else 300))
