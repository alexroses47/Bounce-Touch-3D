#!/usr/bin/env python3
"""Saca de una traza de EKA2L1 (bt_ball_trace.lua) la secuencia de entradas para el build host.

Por cada fotograma de la bola (línea BTB) escribe "frame dt held pressed": dt es el del original y
held/pressed son máscaras de las acciones del original (bit k = acción k) tomadas de las líneas BTI
(grabadas al entrar en Game::v03, antes de la bola en el mismo fotograma). Las trazas sin BTI dan
entradas vacías.

Uso: extract_inputs.py EKA2L1.log salida.txt [--level NOMBRE] [--from N] [--to N]
"""
import argparse
import re


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("log")
    ap.add_argument("out")
    ap.add_argument("--level", default="skybean_level01")
    ap.add_argument("--from", dest="first", type=int, default=1)
    ap.add_argument("--to", dest="last", type=int, default=10 ** 9)
    a = ap.parse_args()
    dts, inputs, touch, tstate = {}, {}, {}, {}
    for line in open(a.log, errors="replace"):
        i = line.find("BT")
        if i < 0:
            continue
        s = line[i:]
        m = re.match(r"BTB (\d+) dt=(\S+)", s)
        if m:
            dts[int(m.group(1))] = m.group(2)
            continue
        m = re.match(r"BTI (\d+) held=([0-9a-fA-F]+) pressed=([0-9a-fA-F]+) tap=(\d+)", s)
        if m:
            inputs[int(m.group(1))] = (int(m.group(2), 16), int(m.group(3), 16), int(m.group(4)))
            m2 = re.search(r"toque=\((-?\d+) (-?\d+)\) reloj=(\S+) ini=\((\S+) (\S+)\) tini=(\S+) ult=\((\S+) (\S+)\)", s)
            if m2:
                tstate[int(m.group(1))] = m2.groups()
            continue
        m = re.match(r"BTT (\d+) p2=(\S+) p3=(\S+)", s)
        if m:
            touch[int(m.group(1))] = (m.group(2), m.group(3))
    frames = [f for f in sorted(dts) if a.first <= f <= a.last]
    with open(a.out, "w") as o:
        o.write(f"# entradas de {a.log}\n# level {a.level}\n"
                "# frame dt held pressed tap dedo p2 p3 x y reloj iniX iniY tini ultX ultY"
                " (held/pressed: bit k = acción k; dedo = Game::v10 llamado en el cuadro)\n")
        for f in frames:
            h, p, tap = inputs.get(f, (0, 0, 0))
            p2, p3 = touch.get(f, ("0", "0"))
            ts = tstate.get(f, ("0", "0", "0", "0", "0", "0", "0", "0"))
            o.write(f"{f} {dts[f]} {h:x} {p:x} {tap} {int(f in touch)} {p2} {p3} {' '.join(ts)}\n")
    print(f"{len(frames)} fotogramas, {sum(1 for f in frames if f in inputs)} con entradas grabadas -> {a.out}")


if __name__ == "__main__":
    main()
