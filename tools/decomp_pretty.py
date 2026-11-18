#!/usr/bin/env python3
"""Make Ghidra soft-float pseudo-C readable (RE helper, output stays local).

Usage: decomp_pretty.py code.bin file.c [function-name-substring] > out.c
  - __aeabi_fadd/fsub/fmul/fdiv(a,b) -> (a + b) ...   (simple operands)
  - __aeabi_i2f(x) -> (float)x, __aeabi_f2iz(x) -> (int)x
  - DAT_xxxxxxxx that holds a plausible float -> its value (comment keeps the label)
  - float-looking hex immediates (0x3f800000) -> 1.0f
"""
import re
import struct
import sys

BASE = 0x8000
OPS = {"fadd": "+", "fsub": "-", "fmul": "*", "fdiv": "/"}
ARG = r"([^(),]+|\([^()]*\))"


def as_float(v):
    f = struct.unpack("<f", struct.pack("<I", v & 0xFFFFFFFF))[0]
    if f != 0 and (1e-5 < abs(f) < 1e7) and abs(f - round(f, 5)) < 1e-6 * max(1, abs(f)) * 10:
        return f
    if f != 0 and 1e-5 < abs(f) < 1e7:
        return f
    return None


def fmt(f):
    s = f"{f:.6g}"
    return s + ("f" if "." in s or "e" in s else ".0f")


def main():
    code = open(sys.argv[1], "rb").read()
    txt = open(sys.argv[2]).read()
    if len(sys.argv) > 3:
        parts = re.split(r"(?m)^(?=// ===== )", txt)
        txt = "".join(p for p in parts if sys.argv[3] in p.split("\n", 1)[0])

    def dat(m):
        a = int(m.group(1), 16)
        o = a - BASE
        if 0 <= o <= len(code) - 4:
            v = struct.unpack_from("<I", code, o)[0]
            f = as_float(v)
            if f is not None and (v >> 24) not in (0x00,):
                return f"{fmt(f)}/*{m.group(0)}*/"
        return m.group(0)
    txt = re.sub(r"DAT_([0-9a-f]{8})", dat, txt)

    def imm(m):
        v = int(m.group(0), 16)
        f = as_float(v)
        return fmt(f) if f is not None else m.group(0)
    txt = re.sub(r"\b0x(?:3[a-f]|4[0-9a-b]|b[a-f]|c[0-9a-b])[0-9a-f]{6}\b", imm, txt)
    txt = re.sub(r"(?<![\w.])0x[0-9a-f]{8}\b(?=\))", imm, txt)

    prev = None
    while prev != txt:
        prev = txt
        txt = re.sub(r"__aeabi_(fadd|fsub|fmul|fdiv)\(" + ARG + "," + ARG + r"\)",
                     lambda m: f"({m.group(2).strip()} {OPS[m.group(1)]} {m.group(3).strip()})", txt)
        txt = re.sub(r"__aeabi_i2f\(" + ARG + r"\)", lambda m: f"(float){m.group(1)}", txt)
        txt = re.sub(r"__aeabi_f2iz\(" + ARG + r"\)", lambda m: f"(int){m.group(1)}", txt)
    # drop the long local declarations block
    txt = re.sub(r"(?m)^  (undefined\d?|int|uint|float|bool|undefined4 \*|int \*|byte|char|short|ushort) [\w*\[\]]+;\n", "", txt)
    print(txt)


if __name__ == "__main__":
    main()
