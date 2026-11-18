# Bounce Touch (Symbian S60v5) — Mapa técnico

Estado al 2026-11-16 (hitos 1–5). Solo hechos verificados sobre la copia `Bounce Touch v1.00(57).sis`.

## 1. Paquete

| Campo | Valor |
|---|---|
| Archivo | `Bounce Touch v1.00(57).sis` (2 995 920 bytes) |
| Formato | SIS de Symbian 9.x (contenido comprimido con zlib) |
| UID de la app | `0x2000C4D4` |
| Contenido extraído | `extracted_sis/` — 260 archivos, 8 616 193 bytes (8,2 MB) |

## 2. Árbol de archivos

```
extracted_sis/
├── sys/bin/Bounce.exe                          ejecutable (E32Image)
├── resource/apps/Bounce.rsc, bounce_aif.mif     recurso de la app e icono
├── private/10003a3f/import/apps/                registro de la app
└── private/2000C4D4/
    ├── backup_registration.xml
    └── data_S60/
        ├── resources.idx                        79 rutas de texturas (texto, CRLF)
        ├── bundles/original/                    9 .hgr  (994 KB)
        │   ├── skybean_level01.hgr … skybean_level08.hgr
        │   └── worldmap.hgr
        └── resources/
            ├── images/        78 .ntx  (2,4 MB)  interfaz y menús
            ├── textures/      79 .ntx + 3 .prs (2,6 MB)  texturas 3D
            ├── models/         6 .hgr  (168 KB)
            ├── musics/        10 .wav  (542 KB)
            ├── sounds/        67 .wav  (598 KB)
            └── localization/  localization.bin, FONT_MENU_MEDIUM.bin (133 KB)
```

Extensiones: 157 `.ntx` · 77 `.wav` · 15 `.hgr` · 3 `.prs` · 2 `.rsc` · 2 `.bin` · 1 `.xml` · 1 `.mif` · 1 `.idx` · 1 `.exe`.

### Modelos (`resources/models/`)

`bounce.hgr` · `bouncedeath.hgr` · `lifeitem.hgr` · `pickitem.hgr` · `objectlibrary.hgr` · `skybean_objectlibrary.hgr`

### Música (`resources/musics/`)

`maintheme_loop1-2` · `land_theme1_loop1-5` · `boss_theme` · `win_theme` · `lose_theme`

## 3. Formatos de assets

Especificación completa de `.hgr` y `.ntx`, lenguaje de acciones de nivel incluido: **`docs/HGR.md`**.

### `.hgr` — escenas / modelos (KA3D)

| Campo | Valor |
|---|---|
| Magic | `hgrf` |
| Versión | `0x00C1` (big-endian) = 193 → 1.93, igual en los 15 archivos |
| Lector de referencia | `ka3d/source/hgr/SceneInputStream.cpp` (`MAX_VERSION = 193`) |
| Versión del exportador | `0x020904` (int32 BE, tras la versión) |
| `dataFlags` | `0xFFFF` |
| Plataforma | `4` = `PLATFORM_N3D` ("Nokia N3D", `ka3d/include/gr/Context.h`). El backend N3D **no** está en el `ka3d` público |
| Orden | Todo big-endian. Cabecera → niebla (`u8` tipo + 5 float) → id `0x12345600` → texturas → materiales → id `0x12345601` → primitivas → … (`Scene.cpp`). Verificado hasta el recuento de primitivas en los 15 archivos |

Cadenas: `u16` BE de longitud + bytes. Textura: cadena (`*.tga`, resuelta a `.ntx`) + `int32` tipo (0 en todas). Material: nombre, shader, `int32` flags, y tres listas (`u8` n) de parámetros textura (`u16` índice) / float4 / float.

| Archivo | Texturas | Materiales | Primitivas |
|---|---|---|---|
| skybean_level01 | 22 | 24 | 143 |
| skybean_level02 | 19 | 22 | 110 |
| skybean_level03 | 20 | 25 | 158 |
| skybean_level04 | 24 | 28 | 193 |
| skybean_level05 | 18 | 19 | 109 |
| skybean_level06 | 19 | 21 | 92 |
| skybean_level07 | 23 | 33 | 133 |
| skybean_level08 | 22 | 26 | 52 |
| worldmap | 5 | 6 | 24 |
| bounce | 2 | 7 | 75 |
| bouncedeath | 2 | 2 | 7 |
| lifeitem / pickitem | 1 | 1 | 1 |
| objectlibrary | 5 | 8 | 10 |
| skybean_objectlibrary | 6 | 10 | 16 |

- Shaders usados (5, todos sin luz): `unlit-tex`, `unlit-plain`, `unlit-plain-mul`, `unlit-tex-nozwrite`, `unlit-plain-nozwrite`.
- Todas las `.tga` referenciadas tienen su `.ntx` (0 ausentes).
- Se leen completos (0 bytes sobrantes) con `tools/hgr.py`. Las 1124 primitivas son listas de triángulos `POSITION=V3_16` + `TEX0=V2_16` (UV en punto fijo 4.12), sin normales ni huesos. Máximo 358 vértices por primitiva. Total: 28 968 triángulos.
- La lógica de nivel va en las propiedades de usuario: 1172 bloques `clave = valor`, 48 verbos de acción (`aNN = verbo,args`) que se corresponden con las clases `Action*`.
- Orden de los niveles en el mapa del mundo: 01, 02, 03, 05, 07, 06, 04, 08.

### `.ntx` — imágenes / texturas (KA3D)

Cabecera: 7 × `uint16` LE — `version, width, height, format, palettesize, flags, userflags` (`ka3d/include/img/ImageReader.h`, `struct NTXHeader`).
Flags: `1` = color key, `2` = comprimido. `palettesize > 0` → píxeles indexados de 8 bits y `format` es el de la paleta.

| Carpeta | `format` | Con paleta | `flags` | Archivos |
|---|---|---|---|---|
| images | 16 | no | 0 | 13 |
| images | 16 | sí | 0 | 65 |
| textures | 7 | no | 0 | 16 |
| textures | 7 | no | 1 (color key) | 7 |
| textures | 7 | sí | 0 | 34 |
| textures | 7 | sí | 1 (color key) | 22 |

- Versión en todos los archivos: `0x104` (el código de `ka3d` documenta `0x102`).
- Verificado visualmente: `7` = RGB565 y `16` = ARGB4444 (nibbles A, R, G, B desde el bit alto). El enum del `ka3d` público llama al 16 `A4B4G4R4`; este build guarda ARGB.
- Los 157 archivos cumplen la fórmula de tamaño `14 + paleta×2 + ancho×alto×(1 o 2)`; 8 llevan 1 byte de relleno.
- Tamaños más frecuentes: 128×128 (35), 256×256 (17), 64×64 (12), 210×120 (8), 128×64 (6).

### Audio

Los 77 `.wav`: PCM, 8 bits, mono, 8000 Hz. Duración total 145 s; música 79 s (bucles de 2 a 16 s).

### Otros

| Archivo | Contenido observado |
|---|---|
| `resources.idx` | 79 rutas `resources\textures\….ntx` separadas por CRLF |
| `localization.bin` | 69 claves × 56 idiomas (español incluido), UTF-8. Documentado en `docs/HGR.md` §10 |
| `FONT_MENU_MEDIUM.bin` | 1379 glifos sobre un atlas `.ntx` de 256×3412. Documentado en `docs/HGR.md` §11 |
| `textures/*.prs` | `bounce.prs`, `life.prs`, `pickup.prs` — texto plano: propiedades de sistema de partículas de KA3D (`ka3d/source/hgr/ParticleSystem.cpp`; `Scene.cpp` añade la extensión `.prs`) |

## 4. Ejecutable `Bounce.exe`

### Cabecera E32Image

| Campo | Valor |
|---|---|
| UIDs | `0x1000007A` (EXE) · `0x100039CE` · `0x2000C4D4` |
| Compresión | BytePair (`0x102822AA`) |
| Flags | `0x1200022A` — imports en formato ELF-derived |
| Tamaño comprimido / descomprimido | 873 684 / 1 506 544 bytes (`0x16FCF0`) |
| Cabecera | `0x9C` bytes |
| Código (`.text`) | `0x16BF70` bytes (1,4 MB) en `0x8000` |
| Datos (`.data`) | `0x150` bytes en `0x400000` |
| BSS | `0x4C` bytes |
| Punto de entrada | `0x8000` (offset 0) |
| Heap mín. / máx. | 5 MB / 16 MB |
| Pila | `0x5000` (20 KB) |
| DLL importadas | 20 |
| Relocaciones de código | 5325 entradas |

### Compresión BytePair (como está en este binario)

- Bloque 1 (código) y bloque 2 (resto: datos, imports, relocaciones), uno tras otro desde `0x9C`.
- Índice de cada bloque: `u32 tamaño_comprimido (incluye el índice)`, `u32 tamaño_descomprimido`, `u16 num_páginas`, `u16 tamaño[num_páginas]`.
- Páginas de 4096 bytes. Código: 364 páginas.
- Página: `u8 num_tokens`; si es 0, bytes literales. Si no: `u8 marcador`, tabla de pares (< 32 tokens: tripletas `token, a, b`; ≥ 32: máscara de 32 bytes + pares) y luego el flujo, donde `marcador` precede a un byte literal.

### Arquitectura del código

- ARM (ARMv6, ARM11) mezclado con Thumb.
- `0x8000`: arranque estándar de EKA2. Guarda registros, alinea la pila y salta con `blx 0xF324` (Thumb).
- Sin FPU: la coma flotante se hace con llamadas a `dfpaeabi.dll`.

### Imports

Mecanismo: cada import es un stub `ldr pc, [pc, #-4]` (`0xE51FF004`) seguido de una palabra `ordinal | addend << 16`. Primer stub en `0x148544`.

Total: **539** — 338 funciones y 201 de datos (typeinfo `_ZTI*` y vtables referenciadas desde la RTTI del juego). Los 539 resueltos a nombre.

| DLL | Imports | Función |
|---|---|---|
| `drtaeabi` | 220 | Runtime del compilador ARM (división, excepciones C++, RTTI) |
| `cone` | 66 | Entorno de controles (`CCoeControl`, `CCoeEnv`) |
| `eikcore` | 57 | Framework de aplicación (Uikon) |
| `avkon` | 41 | Interfaz S60 |
| `libc` | 38 | Open C (POSIX) |
| `euser` | 36 | Biblioteca de usuario del kernel |
| `dfpaeabi` | 23 | Coma flotante por software |
| `libm` | 16 | Matemáticas (Open C) |
| `remconcoreapi` | 7 | Control remoto: teclas multimedia/volumen |
| `apparc` | 6 | Arquitectura de aplicaciones |
| `ws32` | 5 | Servidor de ventanas, acceso directo a pantalla |
| `bitgdi` | 5 | Dibujo en bitmaps |
| `libpthread` | 5 | Hilos (Open C) |
| `libstdcpp` | 5 | `new`/`delete` de C++ |
| `fbscli` | 3 | Bitmaps (`CFbsBitmap`) |
| `remconinterfacebase` | 2 | Base de control remoto |
| `mediaclientaudiostream` | 1 | Salida de audio en streaming |
| `scdv` | 1 | Controlador de pantalla |
| `hal` | 1 | Capa de abstracción de hardware |
| `ProfileEngine` | 1 | Perfil del teléfono (silencio) |

No importa ninguna biblioteca de OpenGL ES: el dibujo pasa por `bitgdi`/`fbscli`/`ws32` (`CDirectScreenAccess`).

Imports más llamados:

| Llamadas | Símbolo |
|---|---|
| 2391 | `__aeabi_fmul` |
| 1263 | `__aeabi_fadd` |
| 514 | `__aeabi_fsub` |
| 483 | `operator delete[]` |
| 378 | `operator new` |
| 271 | `__aeabi_i2f` |
| 264 | `operator new[]` |
| 211 | `__aeabi_f2iz` |
| 204 | `__aeabi_fdiv` |
| 189 | `__cxa_pure_virtual` |

## 5. Funciones identificadas

| Dirección | Nombre provisional | Qué hace |
|---|---|---|
| `0x8000` | `_E32Startup` | Arranque EKA2 → `0xF324` (Thumb) |
| `0x5DF64` | (`FUN_0005df64`) | Crea el objeto de pantalla (`0x60` bytes): `RWindowBase::SetRequiredDisplayMode(7)` (`EColor64K`, RGB565), `CDirectScreenAccess::NewL`, `CFbsBitmap::Create(…, 7)` → `+0x28`, `CFbsBitmapDevice::NewL` → `+0x2C`, `CreateContext` |
| `0x5E3EC` | (`FUN_0005e3ec`) | Devuelve `CFbsBitmap::DataAddress()` del bitmap en `+0x28` (puntero a los píxeles) |

### Funciones de KA3D (por cadenas del código fuente de `ka3d`)

`tools/ka3d_strings.py` encuentra 336 literales de `ka3d/source` en el binario (40 archivos, 68 funciones). Con los inequívocos (≥ 10 caracteres, de una sola función, fuera de `dx/`, `win32/` y `ode/`) se nombraron **31 funciones**. Las más útiles:

| Dirección | Función KA3D | Literales |
|---|---|---|
| `0x966F0` | `hgr::SceneInputStream::SceneInputStream` (comprueba `hgrf`) | 1 |
| `0x9233C` | `hgr::Scene::Scene` (carga de escena) | 16 |
| `0x98400` | `hgr::SceneInputStream::readPrimitive` | 6 |
| `0x981FC` | `hgr::SceneInputStream::readVertexFormat` | 1 |
| `0x970C4` | `hgr::SceneInputStream::readKeyframeSequence` | 1 |
| `0x9755C` | `hgr::SceneInputStream::readTransformAnimation` | 1 |
| `0x97968` | `hgr::SceneInputStream::readUserPropertySet` | 1 |
| `0x8BFF0` | `hgr::ParticleSystem::Description::read` (lector de `.prs`) | 31 |
| `0x625A0` | `gr::SurfaceFormat::SurfaceFormat` | 1 |
| `0x718AC` / `0x71DEC` | `img::ImageReader::readHeader_tga` / `_bmp` | 3 / 4 |
| `0x665A0` | `lang::Format::format` | 3 |

Lista completa en `re/logs/apply_classinfo.log`. Las funciones con cadenas de varias funciones KA3D llevan el comentario `uses strings of ka3d …`.

### Clases (RTTI)

El binario conserva la RTTI de C++ (ABI ARM/Itanium): cada typeinfo es `[vptr importado][nombre]` y, según el tipo, `[base]` (si) o `[flags][n][base, offset]…` (vmi). `tools/rtti_scan.py` recupera **190 clases** (166 si, 19 sin base, 5 con herencia múltiple) y **177 vtables** (16 secundarias) con 1657 entradas que apuntan a 1037 funciones distintas.

| Origen | Clases | Entradas de vtable |
|---|---|---|
| Juego | 97 | 748 |
| KA3D (`lang`, `io`, `gr`, `hgr`, `img`, `math`, `framework`) | 68 | 418 |
| Backend N3D (`gr::N3D_*`, `nlibs::CIN3d*`), **sin fuente pública** | 15 | 265 |
| Plataforma S60 (`CFramework*`, `CBackBuffer`, `M*`) | 10 | 137 |

Clases del juego relevantes (árbol completo en `re/classes.txt`):

- `GameObject` → `ActiveGameObject` → `Ball` → `PlayerBall`; además `GameCamera` y `Trigger`.
- `GameScene` → `Game`, `Menu`, `WorldMap`, `CutsceneScene`, `DeathScene`, `PopupScene`, `LoadingScreenScene`, `AttractionScene`.
- `ActionBase` → **45 acciones** de guion de nivel (`ActionSetGravity`, `ActionAddImpulse`, `ActionMoveObject`, `ActionCheckPoint`, `ActionLevelComplete`, `ActionTestVariable`…). La lógica de nivel es por datos.
- Colisión propia: `CollisionObject` (hereda de `hgr::Node`), `CollisionGeometry`, `CollisionSpace`. ODE del `ka3d` público no aparece en la RTTI.
- Menús: `MenuItem` → `ContainerItem`, `ImageItem`, `TextItem`, `SliderController`, `KeyConfigurator`; `MenuPage`.
- Otros: `IAudioManager` → `AudioManagerS60`, `IGameSaves` → `GameSaves`, `IAchievements` → `Achievements`, `GameSettings`, `IResources` → `Resources`, `SpriteSheet`, `CustomBitmapFontResource`.

Campos del objeto de pantalla (`0x60` bytes) vistos en `0x5DF64`:

| Offset | Contenido |
|---|---|
| `+0x00` | vtable |
| `+0x04` | interfaz `MDirectScreenAccess` (vtable + `0x1C`) |
| `+0x08` | parámetro 1 (control dueño) |
| `+0x0C` | resultado de `SetRequiredDisplayMode` |
| `+0x1C` | `CDirectScreenAccess*` |
| `+0x28` | `CFbsBitmap*` (framebuffer) |
| `+0x2C` | `CFbsBitmapDevice*` |
| `+0x34`, `+0x38` | parámetros 2 y 3 (tamaño, usado como `TSize` en `CFbsBitmap::Create`) |
| `+0x54` | `1` al terminar |
| `+0x58`, `+0x5C` | parámetros 4 y 5 |

## 6. Proyecto de Ghidra

| Elemento | Valor |
|---|---|
| Ghidra | 12.2 · macOS: `brew install ghidra` (arm64 nativo, OpenJDK 25 de Homebrew) · Linux: `~/.local/opt/ghidra_12.2_PUBLIC` |
| Proyecto | `Bounce.gpr` + `Bounce.rep/` en la raíz · programa `code.bin` |
| Abrir | `./abrir-ghidra.sh` |
| Lenguaje | `ARM:LE:32:v6` |
| `.text` | `0x8000`, cargado con BinaryLoader desde `re/code.bin` |
| `.data` / `.bss` | `0x400000` / `0x400150` |
| `IMPORTS` | `0x10000000`, bloque sintético: 1 slot de 4 bytes por import |
| Funciones | 3162 (antes 2621; el post-script creó 539 desde vtables), 1233 con nombre |
| Nombres | Stubs con nombre demanglado (sin firma) y comentario `import <dll> @<ordinal>` |
| Clases | 190 namespaces de clase con etiquetas `typeinfo` y `vtable` (`vtable_<offset>` en las secundarias) |
| Métodos virtuales | 865 nombrados `Clase::vNN` (clase que introduce o redefine la entrada); 233 cuerpos compartidos entre clases sin relación llevan el comentario `also …`. En descendientes de `lang::Object` las entradas 0/1 son `~Clase` y `~Clase_deleting` |
| Funciones KA3D | 31 nombradas por cadenas (ver §5) |

## 7. Archivos de trabajo

| Ruta | Contenido |
|---|---|
| `re/Bounce_unpacked.exe` | E32Image descomprimido (cabecera con compresión = 0) |
| `re/code.bin` | Sección de código sola |
| `re/data.bin` | Sección `.data` |
| `re/imports.tsv` | `tipo · dirección · dll · ordinal · símbolo` (539 filas) |
| `re/defs/*.def` | 20 tablas de ordinales (Symbian Foundation, EPL) |
| `re/logs/` | Logs de Ghidra headless |
| `tools/e32_unpack.py` | Descompresor BytePair |
| `tools/make_import_map.py` | Genera `imports.tsv` y `data.bin` |
| `tools/ghidra/SymbianE32Setup.java` | Pre-script: bloques, nombres de imports, demangle |
| `tools/ghidra/ReportImports.java` | Estadísticas de imports y decompilación de ejemplo |
| `tools/rtti_scan.py` | RTTI → `re/rtti.tsv`, `re/vtables.tsv`, `re/classes.txt` |
| `tools/ka3d_strings.py` | Literales de `ka3d` en el binario → `re/ka3d_strings.tsv` |
| `tools/ghidra/ApplyClassInfo.java` | Post-script: clases, vtables, métodos virtuales y nombres KA3D |
| `re/rtti.tsv` | `typeinfo · tipo · clase · bases · vtable · entradas` (190 filas) |
| `re/vtables.tsv` | `vtable · clase · offset_to_top · entrada · destino · dueño` (1657 filas) |
| `re/classes.txt` | Árbol de herencia |
| `re/ka3d_strings.tsv` | `dirección · archivo ka3d · función ka3d · literal` (336 filas) |
| `tools/hgr.py` | Lector `.hgr` (`info`, `tree`, `props`, `json`) |
| `tools/hgr_export.py` | `.hgr` → glTF (`.glb`), visor HTML (three.js) o render PNG por software |
| `tools/ntx.py` | `.ntx` → PNG |
| `docs/HGR.md` | Especificación de `.hgr`, `.ntx`, `localization.bin`, fuente y `.sis`, y del lenguaje de niveles |
| `tools/sis_extract.py` | Extractor de `.sis` (Symbian 9) con verificación SHA-1 |
| `tools/bsc.py` | Escritor/lector del formato de escena BSC1 para la 3DS |
| `tools/convert_assets.py` | `.sis` → romfs de la 3DS (escenas, `.t3x`, `.ogg`, fuente, textos) |
| `docs/ROMFS.md` | Contenido del romfs y formatos BSC1 / BFT1 con estructuras en C |
| `docs/FISICA.md` | Física de la bola observada en el binario |
| `docs/GUIONES.md` | Triggers y verbos de los guiones de nivel; lista de reglas del motor con su evidencia |
| `docs/CLASES.md` | Vtables de las 190 clases: qué versión de cada método se ejecuta (`tools/classes_doc.py`) |
| `tools/e32emu.py` | Banco diferencial: carga `Bounce.exe` en Unicorn con relocaciones y stubs de Symbian y llama a funciones originales |
| `tools/eka2l1/bt_ball_trace.lua` | Script de traza para EKA2L1 (solo lee memoria): bola por cuadro (`0x4ee68`), choques con bits del grupo (`0x4f2c8`) y ojo de la cámara (`0x35470`). Se copia a `~/Library/Application Support/EKA2L1/scripts/`. Dispositivo que funciona: RM-356 (5800 XpressMusic); con RM-505 (N97) el juego se cuelga al arrancar |
| `tools/eka2l1/parse_trace.py`, `analyze_trace.py` | Pasan `EKA2L1.log` a tablas y miden gravedad, restitución por bit 15, salto y distancia de cámara. Las trazas (`re/traces/`) son locales y no van al repositorio |
| `tools/eka2l1/make_trace_script.py` → `bt_trace_v2.lua` | Traza v2 para EKA2L1: añade entradas por fotograma (BTI), estado completo de la cámara al salir de `GameCamera::v02` (BTK), tabla de `setcameraview` y desplazamiento del objetivo (BTV) y vigilancia de la restitución (BTR: EKA2L1 no tiene watchpoints, se comprueba en cada importación y método de la bola) |
| `tools/eka2l1/extract_inputs.py` | Saca de una traza de EKA2L1 las entradas por fotograma (dt + acciones) para el build host |
| `port/host/` (`build.sh`, `host_main.cpp`) | Build host de macOS: el núcleo del juego (`port/source/game.cpp`) sin gráficos; reproduce entradas y escribe traza con el formato de EKA2L1 (`port/source/trace.h`) |
| `bt_host --camara-aislada TRAZA` | Impone en cada fotograma el estado de la bola del original (BTB) antes de la cámara: compara la cámara sola, sin el error acumulado de la física |
| `tools/compare_traces.py` | Comparador original vs port fotograma a fotograma (bola, choques, cámara): dónde empieza a divergir, por cuánto, máximo y % dentro de tolerancia; con `--baseline` es test de regresión |
| `tests/trace_regression.sh` | Reproduce cada traza de `re/traces` en el host y compara con `tests/baselines/*.json` (en `tests/run_all.sh`) |
| `tests/azahar_smoke.sh` | Compila el `.3dsx`, lo arranca en Azahar con guion automático, revisa log y capturas (no negras) y compara la traza de la 3DS emulada con la del host |
| `port/source/rules.*` | Reglas del motor como funciones puras, con su dirección de origen |
| `tests/` | `run_all.sh`: tests diferenciales (`test_rules_diff.py`) y de geometría (`test_tree_fall.py`) |
| `tools/ghidra/DumpClasses.java` | Decompila clases o funciones (`@0xADDR`) a `re/decomp/` (solo local) |
| `tools/decomp_pretty.py` | Hace legible el pseudo-C con coma flotante por software |
| `abrir-ghidra.sh` | Abre el proyecto en Ghidra |

Regenerar todo:

```bash
python3 -I tools/e32_unpack.py extracted_sis/sys/bin/Bounce.exe re/Bounce_unpacked.exe
python3 -I -c "d=open('re/Bounce_unpacked.exe','rb').read(); open('re/code.bin','wb').write(d[0x9c:0x9c+0x16bf70])"
python3 -I tools/make_import_map.py re/Bounce_unpacked.exe re/defs re
python3 -I tools/rtti_scan.py re/code.bin re/imports.tsv re
python3 -I tools/ka3d_strings.py ka3d re/code.bin re/ka3d_strings.tsv
# macOS: GHIDRA=/opt/homebrew/opt/ghidra/libexec  (y JAVA_HOME=/opt/homebrew/opt/openjdk@25/libexec/openjdk.jdk/Contents/Home)
# Linux: GHIDRA=~/.local/opt/ghidra_12.2_PUBLIC
$GHIDRA/support/analyzeHeadless "$PWD" Bounce -import re/code.bin -overwrite \
  -loader BinaryLoader -loader-baseAddr 0x8000 -processor ARM:LE:32:v6 \
  -scriptPath "$PWD/tools/ghidra" -preScript SymbianE32Setup.java re/imports.tsv re/data.bin 0x400000 0x4c \
  -postScript ApplyClassInfo.java re/rtti.tsv re/vtables.tsv re/ka3d_strings.tsv
```

En macOS, las salidas de `e32_unpack.py` y `make_import_map.py` son idénticas byte a byte a las de Linux (SHA-256 comprobado).

## 8. Port para 3DS (`port/`)

| Elemento | Estado |
|---|---|
| Compilar | `port/build.sh` (convierte el `.sis` si falta `port/romfs`, que es un enlace a `out/romfs`) · `./build.sh send` lo envía por 3dslink |
| Pruebas | `port/tools/emu_test.sh <salida> "level X" "wait N" "hold UP+A N" "shot" "log"` en Azahar; capturas en PNG |
| Render | BSC1 cargado en memoria lineal; shader PICA con posición y UV en `int16` + escala/bias; texturas `.t3x`; cielo centrado en la cámara; estéreo con `Mtx_PerspStereoTilt` (mano izquierda) |
| Colisión | Triángulos de las mallas `collisionmesh` (nivel + `load` de librería) en rejilla hash de 512; esfera-triángulo; las juntas internas (aristas) se ignoran si hay contacto con una cara |
| Física | `docs/FISICA.md` (gravedad −2000, masa 1, impulso de control 1750, salto 1300, vmax 2000, rozamiento 100) |
| Medido en Azahar | 8 niveles cargan; 60 FPS; 942–3605 triángulos por cuadro |
| Guiones | `port/source/script.cpp` + `world.cpp`: triggers con su máquina de estados, 61 verbos (`docs/GUIONES.md`), link/unlink, movimientos, cámara del original (55°/555 con muelle) y cámara guiada |
| Pendiente | `playanimation`, grupos de colisión, otras formas de bola, sonido, partículas, textos en pantalla, menús, control táctil |

## 9. Fuentes de referencia

| Fuente | Ubicación | Licencia | Contenido |
|---|---|---|---|
| `pent0/ka3d` | `ka3d/` (commit `0efab42`, 2018-05-29) | GPL-2.0 | Motor KA3D (versión con DirectX 9). Lectores de `.hgr` y `.ntx` |
| `GZHYBFHHJ/KA3DConvert` | `KA3DConvert/` (commit `ef4bc20`, 2023-12-14) | BSD-3 | Conversor `.dat` ↔ XML (`SPRT`, `FONT`, `TEXT`, `COMP`). No cubre `.ntx`/`.hgr` |
| Symbian Foundation | `github.com/SymbianSource/oss.FCL.sf.*` | EPL | Archivos `.def` de ordinales |
