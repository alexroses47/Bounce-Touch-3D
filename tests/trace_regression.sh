#!/bin/zsh
# Test de regresión original vs port: reproduce en el build host las entradas de cada traza de EKA2L1
# guardada en re/traces (local, no va al repositorio) y la compara con tools/compare_traces.py contra la
# línea base de tests/baselines. Falla si algún canal diverge antes o empeora.
#   tests/trace_regression.sh [--update]   (--update guarda los resultados actuales como línea base)
set -eu
ROOT="${0:A:h:h}"
cd "$ROOT"
port/host/build.sh > /dev/null
UPDATE=""; [[ "${1:-}" == "--update" ]] && UPDATE="--update-baseline"
FAIL=0
# nombre | log original | nivel | último fotograma sin entradas desconocidas
for spec in "level01_idle|re/traces/eka_level01_raw.log|skybean_level01|1367" \
            "level01_v2|re/traces/eka_level01_v2_raw.log|skybean_level01|999999999"; do
	IFS='|' read -r name log level last <<< "$spec"
	[[ -f "$log" ]] || { echo "-- $name: sin traza ($log), se omite"; continue; }
	mkdir -p "tests/build/$name"
	python3 -I tools/eka2l1/extract_inputs.py "$log" "tests/build/$name/inputs.txt" --level "$level" --to "$last" > /dev/null
	port/host/build/bt_host --data port/romfs --level "$level" --inputs "tests/build/$name/inputs.txt" --trace "tests/build/$name/port.log" 2> /dev/null
	echo "-- $name"
	python3 -I tools/compare_traces.py "$log" "tests/build/$name/port.log" --to "$last" \
		--baseline "tests/baselines/$name.json" $UPDATE || FAIL=1
done
# cámara aislada: la bola del original impuesta en cada fotograma (bt_host --camara-aislada)
log=re/traces/eka_level01_v2_raw.log
if [[ -f "$log" ]]; then
	name=level01_v2_camara
	mkdir -p "tests/build/$name"
	python3 -I tools/eka2l1/extract_inputs.py "$log" "tests/build/$name/inputs.txt" > /dev/null
	port/host/build/bt_host --data port/romfs --inputs "tests/build/$name/inputs.txt" --trace "tests/build/$name/port.log" --camara-aislada "$log" 2> /dev/null
	echo "-- $name"
	python3 -I tools/compare_traces.py "$log" "tests/build/$name/port.log" --baseline "tests/baselines/$name.json" $UPDATE || FAIL=1
fi
exit $FAIL
