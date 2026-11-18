#!/usr/bin/env python3
"""El tallo que corta el topo en el nivel 1 debe quedar tendido sobre el camino.

Reproduce, con las reglas del port (tests/build/librules.dylib), las acciones de T_mole:
  a15 = rotateobject,O_fallbean,O_fallbean,-90,0,0,1
  a16 = moveobject,O_fallbean,O_fallbean,0,-600,0,1,1
y comprueba que los puntos del tronco caído tienen debajo la colisión del camino. Como control,
el giro de signo contrario debe dejar el tronco sobre el vacío.

Referencia visual: fotograma 1:00 del vídeo (el tronco cruza el camino, obstáculo para saltar).
Uso: python3 -I tests/test_tree_fall.py <carpeta extraída del .sis>
"""
import ctypes
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "tools"))
import hgr  # noqa: E402
import hgr_export as E  # noqa: E402

lib = ctypes.CDLL(os.path.join(HERE, "build", "librules.dylib"))
F3 = ctypes.c_float * 3
F12 = ctypes.c_float * 12
lib.bt_rotation_angle.restype = ctypes.c_float


def action_vector(v, world):
    out = F3()
    lib.bt_action_vector(F3(*v), int(world), out)
    return list(out)


def rotate_local(local, axis, angle):
    out = F12()
    lib.bt_rotate_local(F12(*[x for row in local for x in row]), F3(*axis), ctypes.c_float(angle), out)
    return [list(out[0:4]), list(out[4:8]), list(out[8:12])]


def norm(v):
    l = sum(c * c for c in v) ** 0.5
    return [c / l for c in v]


def ground_below(tris, x, z, ymax):
    best = None
    for a, b, c in tris:
        d = (b[2] - c[2]) * (a[0] - c[0]) + (c[0] - b[0]) * (a[2] - c[2])
        if abs(d) < 1e-9:
            continue
        l1 = ((b[2] - c[2]) * (x - c[0]) + (c[0] - b[0]) * (z - c[2])) / d
        l2 = ((c[2] - a[2]) * (x - c[0]) + (a[0] - c[0]) * (z - c[2])) / d
        l3 = 1 - l1 - l2
        if min(l1, l2, l3) < 0:
            continue
        y = l1 * a[1] + l2 * b[1] + l3 * c[1]
        if y < ymax and (best is None or y > best):
            best = y
    return best


def move_step(local, basis, d, step):
    out = F12()
    lib.bt_move_step(F12(*[x for row in local for x in row]), F12(*[x for row in basis for x in row]), F3(*d),
                     ctypes.c_float(step), out)
    return [list(out[0:4]), list(out[4:8]), list(out[8:12])]


def fallen_trunk_points(scene, sign, fps=60):
    """Simula T_mole a15 (giro, sin espera) y a16 (desplazamiento) como FUN_00039698: cada cuadro,
    en orden de registro, primero el giro y luego el desplazamiento con la matriz local ya girada."""
    nodes = scene.nodes
    node = next(n for n in nodes if n.name == "O_fallbean")
    parent = [[1.0, 0, 0, 0], [0, 1.0, 0, 0], [0, 0, 1.0, 0]]
    chain = []
    p = node.parent
    while p >= 0:
        chain.append(nodes[p].transform)
        p = nodes[p].parent
    for m in reversed(chain):
        parent = E.mat_mul(parent, m)
    local = [row[:] for row in node.transform]
    rx = [-90.0 * sign, 0.0, 0.0]
    rot_axis = norm(action_vector(rx, False))
    rot_speed = lib.bt_rotation_angle(F3(*rx)) / 1.0          # a15: 1 s
    mv = action_vector([0.0, -600.0, 0.0], False)
    mv_dir, mv_speed = norm(mv), (sum(c * c for c in mv) ** 0.5) / 1.0   # a16: 1 s
    dt = 1.0 / fps
    t = 0.0
    while t < 1.0 - 1e-9:
        step = min(dt, 1.0 - t)
        local = rotate_local(local, rot_axis, rot_speed * step)
        local = move_step(local, local, mv_dir, mv_speed * step)
        t += step
    world = E.mat_mul(parent, local)
    mesh = next(n for n in nodes if n.name == "fallbean")
    # puntos repartidos por la superficie (el tallo tiene pocos vértices, todos en los extremos)
    pts = []
    mw = E.mat_mul(world, mesh.transform)
    for pi in mesh.extra["primitives"]:
        pr = scene.primitives[pi]
        P = [E.mat_apply(mw, v) for v in pr.positions()]
        for i in range(0, len(pr.indices), 3):
            a, b, c = (P[j] for j in pr.indices[i:i + 3])
            for u in range(5):
                for w in range(5 - u):
                    k = (u + 0.5) / 5, (w + 0.5) / 5
                    if k[0] + k[1] > 1:
                        continue
                    pts.append([a[d] + (b[d] - a[d]) * k[0] + (c[d] - a[d]) * k[1] for d in range(3)])
    return pts


def main(extracted):
    path = os.path.join(extracted, "private", "2000C4D4", "data_S60", "bundles", "original", "skybean_level01.hgr")
    scene = hgr.load(path)
    tris = []
    for sc, n, tm, role in E.flatten(scene, None):
        if role == "collision" and n.name != "beancol":
            for pi in n.extra["primitives"]:
                pr = sc.primitives[pi]
                P = [E.mat_apply(tm, v) for v in pr.positions()]
                for i in range(0, len(pr.indices), 3):
                    tris.append([P[j] for j in pr.indices[i:i + 3]])

    def profile(pts):
        """(fracción del tronco con suelo debajo, mayor separación tronco-suelo, altura del tronco)"""
        step = max(1, len(pts) // 60)
        sample = pts[::step]
        gaps = []
        for p in sample:
            g = ground_below(tris, p[0], p[2], p[1] + 1)
            if g is not None:
                gaps.append(p[1] - g)
        ys = [p[1] for p in pts]
        return len(gaps) / len(sample), (min(gaps) if gaps else None), max(ys) - min(ys)

    ok_sup, ok_gap, ok_h = profile(fallen_trunk_points(scene, +1))
    bad_sup, bad_gap, _ = profile(fallen_trunk_points(scene, -1))
    print(f"signo del binario: {ok_sup:.0%} del tronco sobre suelo, hueco mínimo tronco-suelo {ok_gap:.0f}, alto {ok_h:.0f}")
    print(f"signo contrario:   {bad_sup:.0%} del tronco sobre suelo, hueco mínimo {bad_gap}")
    assert ok_h < 600, "el tronco no quedó tumbado"
    assert ok_gap is not None and ok_gap < 150, "el tronco flota sobre el camino"
    assert bad_sup < ok_sup, "el signo contrario no debería apoyarse mejor"
    print("OK: el tronco cae tumbado y a ras del camino")
    return 0


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "..", "extracted_sis"))
