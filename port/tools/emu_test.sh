#!/bin/zsh
# Prueba automática en Azahar: escribe el guion en la SD del emulador, espera a que termine,
# cierra el emulador y convierte las capturas a PNG.
#   tools/emu_test.sh <carpeta_salida> "<línea1>" "<línea2>" ...
#   líneas: "level NOMBRE", "wait N", "hold TECLAS N", "shot", "log"
set -u
OUT=$1; shift
SD="$HOME/Library/Application Support/Azahar/sdmc"
ROOT="${0:A:h:h}"
mkdir -p "$SD" "$OUT"
print -l -- "$@" > "$SD/bt_auto.txt"
rm -f "$SD"/bt_shot_*.bmp(N) "$SD/bt_log.txt"

~/Applications/Azahar.app/Contents/MacOS/azahar "${BOUNCE_3DSX:-$ROOT/bounce.3dsx}" > /dev/null 2>&1 &
P=$!
for i in {1..120}; do
	grep -q "FIN" "$SD/bt_log.txt" 2>/dev/null && break
	/bin/sleep 1
done
/bin/sleep 1
kill -9 $P 2>/dev/null
wait $P 2>/dev/null
cat "$SD/bt_log.txt" 2>/dev/null || echo "(sin log: ¿arrancó?)"
for f in "$SD"/bt_shot_*.bmp(N); do
	sips -s format png "$f" --out "$OUT/${f:t:r}.png" > /dev/null
done
ls "$OUT"
