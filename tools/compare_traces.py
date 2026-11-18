#!/usr/bin/env python3
"""Comparador original vs port, fotograma a fotograma (formato BTB/BTC/BTK de tools/eka2l1/bt_ball_trace.lua
y port/source/trace.h).

Para cada canal (posición y velocidad de la bola, suelo, contactos, ojo y objetivo de la cámara) dice
en qué fotograma empieza a divergir (primer error por encima de la tolerancia), por cuánto, el error
máximo y el porcentaje de fotogramas dentro de tolerancia.

Uso:
  compare_traces.py original.log port.log [--from N] [--to N] [--json metrics.json]
                    [--baseline base.json [--update-baseline]]
Con --baseline es un test de regresión: falla (código 1) si algún canal empeora respecto a la línea base
(diverge antes o baja el porcentaje dentro de tolerancia más de 0,5 puntos).
"""
import argparse
import json
import math
import re
import sys

F = r"(-?[\d.]+(?:e[-+]?\d+)?|-?nan|-?inf)"
V = r"\(" + F + " " + F + " " + F + r"\)"

# tolerancias por canal (unidades del .hgr, u/s)
TOL = {"pos": 1.0, "vel": 5.0, "ground": 0.5, "contact": 0.5, "normal": 0.02, "eye": 5.0, "target": 5.0}


def parse(path):
    frames = {}
    cur = None
    v1cam = False
    for line in open(path, errors="replace"):
        i = line.find("BT")
        if i < 0:
            continue
        s = line[i:]
        m = re.match(r"BTB (\d+) dt=" + F + " p=" + V + " v=" + V + r".*?suelo=(\d+)", s)
        if m:
            n = int(m.group(1))
            g = [float(x) for x in m.groups()[1:8]]
            frames.setdefault(n, {}).update(dt=g[0], pos=g[1:4], vel=g[4:7], ground=int(m.group(9)))
            continue
        m = re.match(r"BTC (\d+) n=(\d+) v=" + V, s)
        if m:
            cur = int(m.group(1))
            frames.setdefault(cur, {}).update(ncontacts=int(m.group(2)), normals=[], bits=[])
            continue
        m = re.match(r"BTC\s+\d+ p=" + V + " n=" + V + r".*?bits8-17=(\d+)", s)
        if m and cur is not None:
            nv = [float(m.group(k)) for k in (4, 5, 6)]
            L = math.sqrt(sum(x * x for x in nv)) or 1.0
            frames[cur]["normals"].append([x / L for x in nv])
            frames[cur]["bits"].append(m.group(7))
            continue
        m = re.match(r"BTK (\d+) ojo=" + V + r"(?: obj=" + V + ")?", s)
        if m:
            d = frames.setdefault(int(m.group(1)), {})
            d["eye"] = [float(m.group(k)) for k in (2, 3, 4)]
            if m.group(5) is not None:
                d["target"] = [float(m.group(k)) for k in (5, 6, 7)]
            else:
                v1cam = True
    # La v1 de bt_ball_trace.lua leía la cámara al ENTRAR en GameCamera::v02 (estado del fotograma
    # anterior): su ojo del fotograma n+1 es el del port en n.
    if v1cam:
        eyes = {n: d.pop("eye") for n, d in frames.items() if "eye" in d}
        for n, e in eyes.items():
            if n - 1 in frames:
                frames[n - 1]["eye"] = e
    return frames


def dist(a, b):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(a, b)))


def compare(orig, port, first, last):
    channels = {}

    def add(ch, frame, err, detail):
        c = channels.setdefault(ch, {"n": 0, "ok": 0, "first": None, "first_err": None, "first_detail": None,
                                     "max": 0.0, "max_frame": None})
        c["n"] += 1
        if err <= TOL[ch]:
            c["ok"] += 1
        elif c["first"] is None:
            c["first"], c["first_err"], c["first_detail"] = frame, err, detail
        if err > c["max"]:
            c["max"], c["max_frame"] = err, frame

    common = [f for f in sorted(orig) if f in port and first <= f <= last]
    for f in common:
        o, p = orig[f], port[f]
        if "pos" in o and "pos" in p:
            add("pos", f, dist(o["pos"], p["pos"]), (o["pos"], p["pos"]))
            add("vel", f, dist(o["vel"], p["vel"]), (o["vel"], p["vel"]))
            add("ground", f, abs(o["ground"] - p["ground"]), (o["ground"], p["ground"]))
        oc, pc = o.get("ncontacts", 0), p.get("ncontacts", 0)
        add("contact", f, 0 if (oc > 0) == (pc > 0) else 1, (oc, pc))
        if oc and pc and o["normals"] and p["normals"]:
            # normal más parecida del port a la primera del original
            best = min(dist(o["normals"][0], q) for q in p["normals"])
            add("normal", f, best, (o["normals"][0], p["normals"]))
        if "eye" in o and "eye" in p:
            add("eye", f, dist(o["eye"], p["eye"]), (o["eye"], p["eye"]))
        if "target" in o and "target" in p:
            add("target", f, dist(o["target"], p["target"]), (o["target"], p["target"]))
    for c in channels.values():
        c["pct"] = 100.0 * c["ok"] / c["n"] if c["n"] else 100.0
    return common, channels


def fmt(v):
    if isinstance(v, list):
        return "(" + " ".join(fmt(x) for x in v) + ")"
    if isinstance(v, float):
        return f"{v:.2f}"
    return str(v)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("original")
    ap.add_argument("port")
    ap.add_argument("--from", dest="first", type=int, default=1)
    ap.add_argument("--to", dest="last", type=int, default=10 ** 9)
    ap.add_argument("--json")
    ap.add_argument("--baseline")
    ap.add_argument("--update-baseline", action="store_true")
    a = ap.parse_args()

    orig, port = parse(a.original), parse(a.port)
    common, ch = compare(orig, port, a.first, a.last)
    if not common:
        print("sin fotogramas en común")
        return 2
    print(f"fotogramas comparados: {common[0]}..{common[-1]} ({len(common)})")
    print(f"{'canal':8} {'tol':>6} {'dentro':>8} {'diverge en':>11} {'por':>9} {'máximo':>10} {'en':>7}")
    for name in ("pos", "vel", "ground", "contact", "normal", "eye", "target"):
        c = ch.get(name)
        if not c:
            continue
        fd = str(c["first"]) if c["first"] is not None else "-"
        fe = f"{c['first_err']:.2f}" if c["first_err"] is not None else "-"
        print(f"{name:8} {TOL[name]:6.2f} {c['pct']:7.1f}% {fd:>11} {fe:>9} {c['max']:10.2f} {str(c['max_frame']):>7}")
    for name in ("pos", "vel", "eye", "target"):
        c = ch.get(name)
        if c and c["first"] is not None:
            o, p = c["first_detail"]
            print(f"  {name} en {c['first']}: original {fmt(o)}  port {fmt(p)}")

    metrics = {k: {"first": v["first"], "pct": round(v["pct"], 2), "max": round(v["max"], 3)} for k, v in ch.items()}
    metrics["_frames"] = [common[0], common[-1]]
    if a.json:
        json.dump(metrics, open(a.json, "w"), indent=1)
    if a.baseline:
        if a.update_baseline:
            json.dump(metrics, open(a.baseline, "w"), indent=1)
            print(f"línea base actualizada: {a.baseline}")
            return 0
        base = json.load(open(a.baseline))
        worse = []
        for k, b in base.items():
            if k.startswith("_") or k not in metrics:
                continue
            m = metrics[k]
            bf = b["first"] if b["first"] is not None else 10 ** 9
            mf = m["first"] if m["first"] is not None else 10 ** 9
            if mf < bf:
                worse.append(f"{k}: diverge en {mf} (antes {b['first']})")
            if m["pct"] < b["pct"] - 0.5:
                worse.append(f"{k}: {m['pct']}% dentro (antes {b['pct']}%)")
        better = [k for k, b in base.items() if not k.startswith("_") and k in metrics and
                  ((metrics[k]["first"] or 10 ** 9) > (b["first"] or 10 ** 9) or metrics[k]["pct"] > b["pct"] + 0.5)]
        if worse:
            print("REGRESIÓN: " + "; ".join(worse))
            return 1
        print("sin regresión respecto a la línea base" + (f" (mejoran: {', '.join(better)}; actualiza con --update-baseline)" if better else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
