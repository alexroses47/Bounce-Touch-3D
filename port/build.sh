#!/bin/zsh
# Compila bounce.3dsx. El romfs sale de tu .sis con tools/convert_assets.py.
#   ./build.sh                 compila (convierte antes si falta el romfs)
#   ./build.sh convert         reconvierte los assets y compila
#   ./build.sh send            compila y envía por 3dslink (pulsa Y en el Homebrew Launcher);
#                              se queda escuchando: el log del juego (stderr) llega aquí
#   ./build.sh release         compila release/Bounce-Touch-3D.cia y .3dsx SIN datos del juego (el juego
#                              los lee de sdmc:/3ds/bounce-touch-3d/); necesita makerom y bannertool
#                              (o MAKEROM=... BANNERTOOL=...)
#   BOUNCE_3DS_IP=x.x.x.x      IP de la consola (la búsqueda automática falla en algunas redes)
set -e
cd "${0:A:h}"
export DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM
export PATH=/opt/devkitpro/tools/bin:$PATH
SIS="${BOUNCE_SIS:-../Bounce Touch v1.00(57).sis}"
BOUNCE_3DS_IP="${BOUNCE_3DS_IP:-}"

if [[ "$1" == "release" ]]; then
	MAKEROM="${MAKEROM:-makerom}"
	BANNERTOOL="${BANNERTOOL:-bannertool}"
	make
	mkdir -p build/cia/romfs release
	# romfs mínimo, sin nada del juego
	print "Bounce Touch 3D: los datos del juego se leen de sdmc:/3ds/bounce-touch-3d/" > build/cia/romfs/LEEME.txt
	"$BANNERTOOL" makebanner -i meta/banner.png -a meta/banner.wav -o build/cia/banner.bnr
	"$BANNERTOOL" makesmdh -s "Bounce Touch 3D" -l "Demo experimental (usa tu propio .sis)" \
		-p "Not made or endorsed by Rovio or Nokia" -i meta/icon.png -o build/cia/icon.icn
	"$MAKEROM" -f cia -o release/Bounce-Touch-3D.cia -target t -exefslogo -elf bounce.elf -rsf meta/app.rsf \
		-icon build/cia/icon.icn -banner build/cia/banner.bnr -DAPP_ROMFS=build/cia/romfs -major 0 -minor 1 -micro 0
	3dsxtool bounce.elf release/Bounce-Touch-3D.3dsx --smdh=bounce.smdh
	ls -lh release
	exit 0
fi

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
