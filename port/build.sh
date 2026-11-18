#!/bin/zsh
# Compila bounce.3dsx. El romfs sale de tu .sis con tools/convert_assets.py.
#   ./build.sh                 compila (convierte antes si falta el romfs)
#   ./build.sh convert         reconvierte los assets y compila
#   ./build.sh send            compila y envía por 3dslink (pulsa Y en el Homebrew Launcher);
#                              se queda escuchando: el log del juego (stderr) llega aquí
#   BOUNCE_3DS_IP=x.x.x.x      IP de la consola (la búsqueda automática falla en algunas redes)
set -e
cd "${0:A:h}"
export DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM
export PATH=/opt/devkitpro/tools/bin:$PATH
SIS="${BOUNCE_SIS:-../Bounce Touch v1.00(57).sis}"
BOUNCE_3DS_IP="${BOUNCE_3DS_IP:-}"

if [[ "$1" == "convert" || ! -d romfs/scenes ]]; then
	python3 -I ../tools/convert_assets.py "$SIS" romfs --work build/convert_work
fi

rm -f bounce.3dsx   # make no sigue los cambios del romfs
make

if [[ "$1" == "send" ]]; then
	if [[ -n "$BOUNCE_3DS_IP" ]]; then
		3dslink -a "$BOUNCE_3DS_IP" -s bounce.3dsx
	else
		3dslink -s bounce.3dsx
	fi
fi
