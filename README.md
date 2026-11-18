# Bounce Touch 3D

Port nativo para Nintendo 3DS de **Bounce Touch** (Symbian S60v5, 2009), reconstruido a partir del
comportamiento del juego original, al estilo de Ship of Harkinian, re3 u OpenGOAL.

> **Demo en desarrollo.** El nivel 1 se juega en la consola con la cámara y el control táctil del original;
> los 8 niveles cargan y ejecutan sus guiones. Faltan animaciones, textos y menús, audio, guardado y más.
> Estado detallado: [`docs/GUIONES.md`](docs/GUIONES.md) (lista de reglas y su evidencia) y [`MAPA.md`](MAPA.md).

*Native Nintendo 3DS port of Bounce Touch (Symbian, 2009), rebuilt from the original game's behaviour. Work in
progress. Contains no game data or code: you need your own copy of the game.*

## Qué hay y qué no

Este repositorio contiene **solo código propio**. No incluye ningún archivo del juego (modelos, texturas,
niveles, sonido, textos) ni código del juego, tampoco decompilado. Para jugar necesitas **tu propia copia**
del instalador `Bounce Touch v1.00(57).sis`. Las herramientas lo extraen y lo convierten **en tu equipo**;
nada se descarga ni se sube.

El código del juego se ha estudiado (Ghidra, emulación) solo para **entender** su comportamiento, que se
reimplementa con código nuevo. Cada regla del motor lleva la dirección del binario de la que sale y, cuando se
puede, un test que la compara con el original (ver «Método»).

Proyecto no oficial, sin relación con Rovio ni Nokia. Bounce y Bounce Touch son marcas de sus propietarios.

## Controles (3DS)

| Entrada | Acción |
|---|---|
| Pantalla táctil: deslizar | Empuja la bola en la dirección del gesto (como el original) |
| Pantalla táctil: mantener quieto a la izquierda / derecha | Gira la cámara |
| Pantalla táctil: mantener quieto en el centro | Frena |
| Pantalla táctil: tocar, o A / B | Salta |
| Circle Pad / cruceta | Control por teclas del original |
| L / R | Nivel anterior / siguiente |
| SELECT | Captura a la SD |
| START | Salir |

## Compilar

Requisitos: [devkitPro](https://devkitpro.org) con `3ds-dev` (citro3d, tex3ds), Python 3 y `oggenc`
(`brew install vorbis-tools`).

```sh
# 1. convierte tu copia del juego (deja los datos en port/romfs) y compila port/bounce.3dsx
BOUNCE_SIS="/ruta/a/Bounce Touch v1.00(57).sis" port/build.sh

# 2. envíalo a la consola con 3dslink (en el Homebrew Launcher pulsa Y); el log del juego llega aquí
BOUNCE_3DS_IP=x.x.x.x port/build.sh send
```

## Build host y tests (macOS)

El núcleo del juego (nivel, bola, guiones, cámara) compila también sin gráficos para el ordenador:

```sh
port/host/build.sh                       # port/host/build/bt_host
port/host/build/bt_host --data port/romfs --level skybean_level01 --frames 600 --trace traza.log
zsh tests/run_all.sh                     # reglas frente al original + regresión de trazas
zsh tests/azahar_smoke.sh                # compila, arranca en Azahar, revisa capturas y compara con el host
```

Algunos tests necesitan archivos que se generan localmente desde tu `.sis` (`tools/sis_extract.py`,
`tools/e32_unpack.py`, `tools/make_import_map.py`) y un entorno con `unicorn` (`tools/.venv`).

## Método

1. **Formatos**: el `.sis`, los ejecutables E32 y los modelos `.hgr` del motor se documentan en
   [`docs/HGR.md`](docs/HGR.md) y [`docs/ROMFS.md`](docs/ROMFS.md); `tools/` los convierte.
2. **Mapa de clases** desde la RTTI del ejecutable: [`docs/CLASES.md`](docs/CLASES.md).
3. **Banco diferencial** (`tools/e32emu.py`, Unicorn): ejecuta funciones del original y las compara con las
   del port (`tests/test_rules_diff.py`).
4. **Trazas del original** en el emulador EKA2L1 (`tools/eka2l1/`): bola, choques, entradas y cámara por
   fotograma. El **build host** reproduce las mismas entradas y `tools/compare_traces.py` dice dónde y por
   cuánto diverge el port; es el test de regresión de cada cambio.
5. **Regla**: ninguna regla del motor sin evidencia del binario o de una traza; lo que no se puede verificar
   se marca como pendiente en [`docs/GUIONES.md`](docs/GUIONES.md).

La guía del proceso para otros ports está en [`docs/GUIA_PORT.md`](docs/GUIA_PORT.md).

## Licencia

[GPL-3.0](LICENSE).
