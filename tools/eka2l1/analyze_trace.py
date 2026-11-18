#!/usr/bin/env python3
"""Mide en una traza de EKA2L1 (bt_ball_trace.lua) reglas de la bola del juego original.
Uso: analyze_trace.py EKA2L1.log"""
import math
import re
import sys
from collections import defaultdict

F = r"(-?[\d.]+)"
V = r"\(" + F + " " + F + " " + F + r"\)"


def main(path):
    ball, hits, cams = {}, [], {}
    cur = None
    for line in open(path, errors="replace"):
        i = line.find("BT")
        if i < 0:
            continue
        s = line[i:]
        m = re.match(r"BTB (\d+) dt=" + F + " p=" + V + " v=" + V + " F=" + V + " J=" + V + r" suelo=(\d+)", s)
        if m:
            g = [float(x) for x in m.groups()]
            ball[int(g[0])] = dict(dt=g[1], p=g[2:5], v=g[5:8], F=g[8:11], J=g[11:14], ground=int(g[14]))
            continue
        m = re.match(r"BTC (\d+) n=(\d+) v=" + V + " e=" + F, s)
        if m:
            cur = dict(frame=int(m.group(1)), n=int(m.group(2)), v=[float(m.group(k)) for k in (3, 4, 5)],
                       e=float(m.group(6)), c=[])
            hits.append(cur)
            continue
        m = re.match(r"BTC\s+(\d+) p=" + V + " n=" + V + r".*?bits8-17=(\d+)", s)
        if m and cur is not None:
            n = [float(m.group(k)) for k in (5, 6, 7)]
            cur["c"].append(dict(n=n, bits=m.group(8)))
            continue
        m = re.match(r"BTK (\d+) ojo=" + V, s)
        if m:
            cams[int(m.group(1))] = [float(m.group(k)) for k in (2, 3, 4)]

    # gravedad: Δvy/dt en el aire sin fuerzas
    gs = []
    fr = sorted(ball)
    for a, b in zip(fr, fr[1:]):
        A, B = ball[a], ball[b]
        if b == a + 1 and not A["ground"] and not B["ground"] and A["dt"] > 0.005 and all(abs(x) < 1e-6 for x in A["F"] + A["J"]):
            gs.append((A["v"][1] - B["v"][1]) / A["dt"])
    if gs:
        gs.sort()
        print(f"gravedad (Δvy/dt en el aire, {len(gs)} pasos): mediana {gs[len(gs)//2]:.1f}")

    # restitución efectiva: un contacto, impacto claro; v después = BTB del cuadro siguiente
    eff = defaultdict(list)
    for h in hits:
        if h["n"] != 1 or h["frame"] + 1 not in ball:
            continue
        c = h["c"][0]
        L = math.sqrt(sum(x * x for x in c["n"]))
        if L == 0:
            continue
        n = [x / L for x in c["n"]]
        vb = sum(a * b for a, b in zip(h["v"], n))
        va = sum(a * b for a, b in zip(ball[h["frame"] + 1]["v"], n))
        if vb < -150:
            eff[c["bits"][7] == "1"].append((-va / vb, h["frame"], vb))
    for k in (True, False):
        xs = sorted(x[0] for x in eff[k])
        if xs:
            print(f"bit15={int(k)}: {len(xs)} impactos (vn<-150): e efectiva mediana {xs[len(xs)//2]:.4f}  min {xs[0]:.4f} max {xs[-1]:.4f}")
    print("campo +0x134 en los choques:", sorted({h['e'] for h in hits}))

    # cámara: distancia y altura del ojo respecto de la bola
    ds = []
    for f, eye in cams.items():
        if f in ball:
            p = ball[f]["p"]
            d = [e - q for e, q in zip(eye, p)]
            dist = math.sqrt(sum(x * x for x in d))
            if dist > 1:
                ds.append((dist, math.degrees(math.asin(d[1] / dist)), f))
    if ds:
        for lo, hi in ((0, 2000), (2000, 4000), (4000, 6000), (6000, 8000), (8000, 10000)):
            part = sorted(x for x in ds if lo <= x[2] < hi)
            if part:
                print(f"cámara cuadros {lo}-{hi}: distancia mediana {part[len(part)//2][0]:.0f} "
                      f"(min {part[0][0]:.0f}, max {part[-1][0]:.0f}); elevación mediana {sorted(x[1] for x in part)[len(part)//2]:.1f}°")

    # saltos: mayor subida de vy en un cuadro
    jumps = []
    for a, b in zip(fr, fr[1:]):
        dv = ball[b]["v"][1] - ball[a]["v"][1]
        if dv > 300:
            jumps.append((b, ball[a]["v"][1], ball[b]["v"][1], ball[a]["J"]))
    print(f"subidas bruscas de vy (>300 en un cuadro): {len(jumps)}")
    for j in jumps[:12]:
        print("  cuadro %d vy %.1f -> %.1f  J=%s" % (j[0], j[1], j[2], j[3]))


if __name__ == "__main__":
    main(sys.argv[1])
