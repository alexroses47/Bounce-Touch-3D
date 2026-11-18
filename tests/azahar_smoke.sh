#!/bin/zsh
# Verificación del .3dsx en Azahar sin intervención:
#   1. compila el port para la 3DS
#   2. lo arranca en Azahar con un guion automático (nivel 1, espera, avanza, capturas, traza)
#   3. comprueba que arranca y dibuja (log), que las capturas no están en negro ni uniformes
#   4. compara la traza de la 3DS emulada con la del build host para las mismas entradas
# Uso: tests/azahar_smoke.sh [carpeta_salida]   (por defecto tests/build/azahar)
set -eu
ROOT="${0:A:h:h}"
OUT="${1:-$ROOT/tests/build/azahar}"
SD="$HOME/Library/Application Support/Azahar/sdmc"
export DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM
export PATH=/opt/devkitpro/tools/bin:$PATH
rm -rf "$OUT"; mkdir -p "$OUT"

echo "== compilando"
(cd "$ROOT/port" && make -j8 > "$OUT/make.log" 2>&1) || { grep -E "error" "$OUT/make.log"; echo "FALLA: no compila"; exit 1; }
grep -E "warning:" "$OUT/make.log" || true

echo "== Azahar"
rm -f "$SD/bt_trace.txt"
"$ROOT/port/tools/emu_test.sh" "$OUT" "level skybean_level01" "trace" "wait 120" "shot" "hold UP 60" "shot" "log" > "$OUT/azahar.log"
cat "$OUT/azahar.log"
cp "$SD/bt_trace.txt" "$OUT/trace_3ds.txt" 2>/dev/null || true

FAIL=0
grep -q "primer cuadro dibujado" "$OUT/azahar.log" || { echo "FALLA: no llegó a dibujar"; FAIL=1; }
grep -q "^FIN" "$OUT/azahar.log" || { echo "FALLA: el guion no terminó (¿se colgó?)"; FAIL=1; }

echo "== capturas"
python3 -I - "$SD" <<'EOF' || FAIL=1
import glob, struct, sys
shots = sorted(glob.glob(sys.argv[1] + "/bt_shot_*.bmp"))
if len(shots) < 2:
    print("FALLA: faltan capturas", shots); sys.exit(1)
bad = 0
for p in shots:
    d = open(p, "rb").read()
    off, w, h = struct.unpack_from("<I", d, 10)[0], *struct.unpack_from("<ii", d, 18)
    px = d[off:off + w * abs(h) * 3]
    colors = len(set(px[i:i + 3] for i in range(0, len(px), 3 * 7)))
    lum = [px[i] + px[i + 1] + px[i + 2] for i in range(0, len(px), 3 * 7)]
    mean = sum(lum) / len(lum)
    var = sum((x - mean) ** 2 for x in lum) / len(lum)
    ok = colors > 200 and var > 100
    print(f"{p.split('/')[-1]}: {colors} colores, varianza {var:.0f} -> {'ok' if ok else 'FALLA (negra o uniforme)'}")
    bad += not ok
sys.exit(1 if bad else 0)
EOF

echo "== 3DS frente al build host (mismas entradas)"
"$ROOT/port/host/build.sh" > /dev/null
{
	echo "# guion de azahar_smoke: 121 cuadros quietos, 60 avanzando (acción 3), 1 quieto; 2 pasos de 1/120 por cuadro"
	for i in {1..242}; do echo "$i 0.0083333 0 0"; done
	for i in {243..362}; do echo "$i 0.0083333 8 0"; done
	for i in {363..364}; do echo "$i 0.0083333 0 0"; done
} > "$OUT/inputs.txt"
"$ROOT/port/host/build/bt_host" --data "$ROOT/port/romfs" --level skybean_level01 --inputs "$OUT/inputs.txt" --trace "$OUT/trace_host.txt"
if [[ -s "$OUT/trace_3ds.txt" ]]; then
	python3 -I "$ROOT/tools/compare_traces.py" "$OUT/trace_3ds.txt" "$OUT/trace_host.txt" | tee "$OUT/compare.txt"
	grep -E "^pos .* 100\.0%" "$OUT/compare.txt" > /dev/null || { echo "FALLA: la 3DS y el host no simulan lo mismo"; FAIL=1; }
else
	echo "FALLA: la 3DS no escribió la traza"; FAIL=1
fi

[[ $FAIL == 0 ]] && echo "AZAHAR OK (capturas en $OUT)" || { echo "AZAHAR FALLA"; exit 1; }
