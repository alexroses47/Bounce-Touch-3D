#!/usr/bin/env python3
"""Find KA3D source string literals inside the game's code section.

Usage: ka3d_strings.py ka3d_dir code.bin out.tsv [base=0x8000]

For every C/C++ string literal in ka3d/source (length >= 6) that occurs in
code.bin as a NUL-terminated string, writes:
  string_addr  ka3d_file  ka3d_function  literal
ka3d_function is the enclosing function definition (Class::method), found
with a simple column-0 heuristic, matching the KA3D coding style.
"""
import os
import re
import sys

LIT = re.compile(r'"((?:[^"\\\n]|\\.)*)"')
# column-0 definition like "Type Ns::Class::method( ..." or "Class::Class( ..."
DEF = re.compile(r'^(?:[\w:<>,\*&\s]+?\s+[\*&]*)?((?:\w+::)*~?\w+)\s*\(')
SKIP = {"if", "for", "while", "switch", "return", "sizeof", "BEGIN_NAMESPACE",
        "END_NAMESPACE", "USING_NAMESPACE", "assert"}
PMACRO = re.compile(r"^P\([^)]*\)\s*")  # KA3D smart-pointer return type: P(Type)


ESC = {"n": "\n", "t": "\t", "r": "\r", '"': '"', "'": "'", "\\": "\\", "0": "\0"}


def unescape(s):
    # simple C escapes only; unknown ones (e.g. "\d" in paths) stay literal
    return re.sub(r"\\(.)", lambda m: ESC.get(m.group(1), m.group(0)), s).encode("latin1")


def scan_sources(root):
    for dirpath, _, files in os.walk(os.path.join(root, "source")):
        for fn in files:
            if not fn.endswith((".cpp", ".h", ".inl")):
                continue
            path = os.path.join(dirpath, fn)
            rel = os.path.relpath(path, root)
            func = None
            with open(path, encoding="latin1") as f:
                for line in f:
                    if line[:1].strip() and not line.startswith(("#", "//", "/*", "*")):
                        m = DEF.match(PMACRO.sub("", line))
                        if m and m.group(1).split("::")[-1] not in SKIP:
                            func = m.group(1)
                    for m in LIT.finditer(line):
                        try:
                            lit = unescape(m.group(1))
                        except UnicodeDecodeError:
                            continue
                        if len(lit) >= 6:
                            yield lit, rel, func or "-"


def main():
    root, code_path, out_path = sys.argv[1:4]
    base = int(sys.argv[4], 0) if len(sys.argv) > 4 else 0x8000
    code = open(code_path, "rb").read()
    hits, seen = [], set()
    for lit, rel, func in scan_sources(root):
        key = (lit, rel, func)
        if key in seen:
            continue
        seen.add(key)
        start = 0
        while True:
            i = code.find(lit + b"\0", start)
            if i < 0:
                break
            if i == 0 or code[i - 1] == 0:  # whole string, not a suffix
                hits.append((base + i, rel, func, lit.decode("latin1")))
            start = i + 1
    hits.sort()
    with open(out_path, "w", encoding="utf-8") as out:
        out.write("string_addr\tka3d_file\tka3d_function\tliteral\n")
        for a, rel, func, lit in hits:
            esc = lit.replace("\\", "\\\\").replace("\t", "\\t").replace("\n", "\\n")
            out.write(f"{a:#x}\t{rel}\t{func}\t{esc}\n")
    files = {h[1] for h in hits}
    funcs = {(h[1], h[2]) for h in hits}
    print(f"literals found: {len(hits)} in {len(files)} ka3d files, {len(funcs)} ka3d functions")


if __name__ == "__main__":
    main()
