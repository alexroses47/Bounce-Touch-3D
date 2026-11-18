#!/usr/bin/env python3
"""Convierte las líneas BT* de EKA2L1.log (tools/eka2l1/bt_ball_trace.lua) en tablas.
Uso: parse_trace.py EKA2L1.log carpeta_salida
Escribe ball.tsv (cuadro, dt, p, v, F, J, suelo, forma), contacts.tsv y camera.tsv."""
import os
import re
import sys

NUM = r"(-?[\d.]+(?:e[-+]?\d+)?|-?nan|-?inf)"


def floats(s):
    return [float(x) for x in re.findall(NUM, s)]


def main(log, out):
    os.makedirs(out, exist_ok=True)
    ball = open(os.path.join(out, "ball.tsv"), "w")
    cont = open(os.path.join(out, "contacts.tsv"), "w")
    cam = open(os.path.join(out, "camera.tsv"), "w")
    ball.write("frame\tdt\tpx\tpy\tpz\tvx\tvy\tvz\tFx\tFy\tFz\tJx\tJy\tJz\tground\tform\tvmax\tlim\n")
    cont.write("frame\tn\ti\tpx\tpy\tpz\tnx\tny\tnz\tdx\tdy\tdz\tmode\tbits\n")
    cam.write("frame\tex\tey\tez\tzx\tzy\tzz\n")
    frame_c = n_c = 0
    for line in open(log, errors="replace"):
        i = line.find("BT")
        if i < 0:
            continue
        s = line[i:].strip()
        if s.startswith("BTB "):
            m = re.match(r"BTB (\d+) dt=(\S+) p=\(([^)]*)\) v=\(([^)]*)\) F=\(([^)]*)\) J=\(([^)]*)\) suelo=(\d+) forma=(\d+) vmax=(\S+) lim=(\S+)", s)
            if m:
                vals = [m.group(1), m.group(2)] + m.group(3).split() + m.group(4).split() + m.group(5).split() + \
                    m.group(6).split() + [m.group(7), m.group(8), m.group(9), m.group(10)]
                ball.write("\t".join(vals) + "\n")
        elif s.startswith("BTC   "):
            m = re.match(r"BTC\s+(\d+) p=\(([^)]*)\) n=\(([^)]*)\) d=\(([^)]*)\) w=\(([^)]*)\) f40=(\d+)(?: modo=(\d+) f14c=(\d+) bits8-17=(\d+))?", s)
            if m:
                cont.write("\t".join([str(frame_c), str(n_c), m.group(1)] + m.group(2).split() + m.group(3).split() +
                                     m.group(4).split() + [m.group(7) or "", m.group(9) or ""]) + "\n")
        elif s.startswith("BTC "):
            m = re.match(r"BTC (\d+) n=(\d+)", s)
            if m:
                frame_c, n_c = int(m.group(1)), int(m.group(2))
        elif s.startswith("BTK "):
            m = re.match(r"BTK (\d+) ojo=\(([^)]*)\) x=\(([^)]*)\) y=\(([^)]*)\) z=\(([^)]*)\)", s)
            if m:
                cam.write("\t".join([m.group(1)] + m.group(2).split() + m.group(5).split()) + "\n")
    for f in (ball, cont, cam):
        f.close()
    print("ok", out)


if __name__ == "__main__":
    main(*sys.argv[1:3])
