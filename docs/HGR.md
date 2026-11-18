# Formatos `.hgr` y `.ntx` de Bounce Touch

Descripción propia, obtenida leyendo los 15 `.hgr` y los 157 `.ntx` de `Bounce Touch v1.00(57).sis`. El lector de referencia de `ka3d` (GPL) se usó para entender el orden de los campos. Las partes que difieren del `ka3d` público van marcadas. Herramientas: `tools/hgr.py`, `tools/hgr_export.py`, `tools/ntx.py`.

**Validación:** los 15 `.hgr` se leen hasta el último byte (0 bytes sobrantes), con los 10 identificadores de control en su sitio y todos los índices (material, textura, primitiva, padre, vértice) dentro de rango.

## 1. Codificación

- Campos estructurados en **big-endian**: `i8/u8`, `i16/u16`, `i32`, `f32` (IEEE 754).
- Cadena: `u16` longitud + bytes UTF-8, sin terminador.
- Arrays crudos de vértices e índices en **little-endian**: se copian tal cual a memoria en el teléfono ARM.
- Matriz `3x4`: 12 `f32` por filas. Las columnas 0–2 son los ejes X/Y/Z del nodo (con escala) y la columna 3 es la traslación. `p' = M[:, :3] · p + M[:, 3]`.
- Sistema de coordenadas: **Y arriba, mano izquierda** (como Direct3D). Las cámaras miran hacia su +Z local.

## 2. Cabecera

| Campo | Tipo | Valor en Bounce Touch |
|---|---|---|
| magic | 4 bytes | `hgrf` |
| versión | `i16` | 193 (lectores KA3D: 170–193) |
| versión del exportador | `i32` (solo si versión > 190) | `0x020904` |
| flags de datos | `u16` | `0xFFFF` |
| plataforma | `i16` (versión ≥ 180) | 4 = Nokia N3D (0 DX, 1 EGL, 2 PSP, 3 SW) |
| niebla | `u8` tipo + `f32` inicio + `f32` fin + 3 `f32` color | tipo 0 (sin niebla) en todos |

## 3. Secciones

Antes de cada sección va un `u32` de control `0x12345600 + n`. La sección de materiales no lleva identificador propio: va pegada a la de texturas.

| n | Sección | Contenido por elemento |
|---|---|---|
| 0 | Texturas (`i32` n) | cadena con el nombre (`*.tga`; el juego carga el `.ntx` del mismo nombre) + `i32` tipo (0 = 2D, 1 = cubemap; todas 0) |
| – | Materiales (`i32` n) | nombre, shader, `i32` flags, `u8` n × (cadena parámetro + `i16` índice de textura), `u8` n × (cadena + 4 `f32`), `u8` n × (cadena + `f32`) |
| 1 | Primitivas (`i32` n) | ver §4 |
| 2 | Mallas (`i32` n) | nodo (§5) + `i32` n × `i32` índice de primitiva + `i32` n huesos × (`i32` índice de nodo + matriz 3x4 inversa de reposo) |
| 3 | Cámaras | nodo + `f32` plano cercano, `f32` lejano, `f32` FOV vertical (rad) |
| 4 | Luces | nodo + 3 `f32` color + 2 `f32` sin uso + `f32` inicio y `f32` fin de atenuación + `f32` cono interior y `f32` exterior (rad) + `u8` tipo (1 direccional, 2 omni, 3 foco) |
| 5 | Dummies | nodo + 3 `f32` mínimo + 3 `f32` máximo de su caja |
| 6 | Líneas | nodo + `i32` n líneas + `i32` n caminos + n × (3 `f32` inicio + 3 `f32` fin) + n × (`i32` inicio, `i32` fin) |
| 7 | Nodos genéricos | nodo |
| 8 | Animaciones (`i32` n = n.º de nodos) | cadena con el nombre del nodo + animación (§6) |
| 9 | Propiedades de usuario (`i32` n) | cadena con el nombre del nodo + cadena de texto (§7) |

Materiales: los shaders son `unlit-tex`, `unlit-plain`, `unlit-plain-mul`, `unlit-tex-nozwrite` y `unlit-plain-nozwrite`. Ninguno usa luz.
- `BASEMAP` es la textura.
- `DIFFUSEC` es el color de los `*-plain`. También aparecen `AMBIENTC`, `SPECULARC` y `SHININESS`, que son valores por defecto de 3ds Max y el render no usa.
- `-mul` indica mezcla multiplicativa (sombras proyectadas, por ejemplo `goal_shadow`). `-nozwrite` indica que no se escribe en el z-buffer.

## 4. Primitiva

| Campo | Tipo |
|---|---|
| vértices | `i32` (< 65536) |
| índices | `i32` |
| formato de vértice | `u8` n × (cadena con el tipo `DT_*` + cadena con el formato `V3_16`, …) |
| material | `i16` |
| tipo | `i16`: 0 punto, 1 línea, 2 tira de líneas, 3 triángulos, 4 tira, 5 abanico, 6 sprite |
| escala/bias de posición | 4 `f32`: `escala, bx, by, bz` |
| escala/bias de UV | 4 `f32`: `escala, bu, bv, –` |
| arrays | uno por tipo presente, en el orden `POSITION, POSITIONT, BONEWEIGHTS, BONEINDICES, NORMAL, DIFFUSE, SPECULAR, TEX0–3, TANGENT`; tamaño = vértices × tamaño del formato |
| índices | `u16` LE × índices |
| huesos usados | `u8` n + n bytes |

**En la práctica las 1124 primitivas son iguales:** listas de triángulos con `DT_POSITION=V3_16` y `DT_TEX0=V2_16`, sin normales, color ni huesos.
- Posición: `int16 × escala + (bx, by, bz)`.
- UV: `int16 × 1/4096`, es decir, punto fijo 4.12 con bias 0. Permite repetir la textura hasta ±8 veces.
- El máximo es de 358 vértices por primitiva.
- El binario conoce además los formatos `V2_16F`, `V3_16F` y `V4_16F`, que no están en el `ka3d` público; ningún archivo los usa.

Totales: 31 785 vértices, 28 968 triángulos, 654 mallas, 1209 dummies, 4 cámaras, 4 nodos genéricos.

## 5. Nodo

| Campo | Tipo |
|---|---|
| nombre | cadena |
| transformación local | matriz 3x4 |
| flags | `i32` (KA3D ignora los bits de clase) |
| id | `i32` (versión > 190) |
| padre | `i32`: índice en la lista global de nodos, en el orden de lectura (mallas, cámaras, luces, dummies, líneas, nodos); −1 = raíz |

## 6. Animación de transformación

`u8` cadencia de claves de posición, `u8` de rotación, `u8` de escala, `u8` comportamiento al final (0 repetir, 1 oscilar, 2 reiniciar, 3 parar), `u8` optimizada (versión ≥ 192).

- **Optimizada**, como las 1784 del juego:
  1. Posición: `i32` n claves + array 4D.
  2. Rotación: secuencia de claves (`i32` n, `i32` dimensión 3 o 4, array).
  3. Escala: `i32` n + array 4D.
  4. `f32` tiempo final (versión ≥ 193).
- **Sin optimizar:** tres secuencias de claves (posición, rotación, escala).
- **Array de dimensión d con n claves:**
  - si n ≤ 2, n × d `f32`;
  - si n > 2, `f32` mínimo[d] y `f32` máximo[d] seguidos de n × d `u16`, con `valor = min + (max − min) × q / 65535`.

Con más de una clave hay 114 nodos animados. Fin de animación: 1450 en «repetir» y 334 en «parar».

## 7. Propiedades de usuario: la lógica del nivel

Texto `clave = valor` por líneas (CRLF), escrito en 3ds Max. 1172 bloques en total. Para leerlo: `tools/hgr.py props archivo.hgr`.

### Convenciones de nombres

| Prefijo | Nodos | Uso observado |
|---|---|---|
| `O_` | 493 | Objeto (grupo de mallas); con `load` instancia un objeto de librería |
| `T_` | 374 | Trigger: lista de acciones, a veces con volumen de colisión |
| `D_` | 102 | Punto de referencia: `D_start`, `D_checkpoint*`, entradas del mapa |
| `N_` | 70 | Sección del nivel (se enlazan y desenlazan con `link` / `unlink`) |
| `A_`, `B_` | 29, 14 | Animaciones y comportamientos de la bola (`bounce.hgr`) |
| `S_` | 15 | Fuente de sonido 3D (`sound`, `inradius`, `outradius`, `volume`) |
| `G_` | – | Referencias a objetos del juego (`G_balls_root`, …) |

### Claves (67 distintas; las más usadas)

| Clave | Veces | Significado (inferido del nombre y del uso; se verificará en el binario en el hito 5) |
|---|---|---|
| `collisionmesh = this` | 362 | La malla es geometría de colisión (y no se dibuja) |
| `perspectivecorrection` | 358 | Nivel 0–10 de corrección de perspectiva del rasterizador N3D (KA3D) |
| `group` | 258 | Máscara de bits de colisión/superficie (valores frecuentes: `0x20100`, `0x3`, `0x2C111`, `0x2C102`, `0xC311`, `0xC003`) |
| `load = archivo.hgr[,nodo]` | 241 | Instancia un nodo de otro `.hgr` en este punto |
| `collisiontype` | 194 | Volumen del trigger: `OBB` (181) o `SPHERE` (13) |
| `executetype`, `state`, `activatorid`, `activatetype` | 132, 72, 16, 5 | Cómo y cuándo se dispara un trigger |
| `executetrigger`, `executelibrarytrigger` | 46, 24 | Trigger que se lanza al tocar el objeto |
| `aNN` | – | Acción número NN del trigger, en orden |
| `eNN` | – | Acciones que se ejecutan al terminar o interrumpir el trigger |
| `music`, `bgcolor`, `scoreachievement`, `skybox` | – | Configuración del nivel (en `D_start`) |
| `surfacevelocity`, `movementareain/out`, `radius`, `useballposition`, `balltype` | – | Superficies móviles, zonas y cambios de bola |
| `level`, `left`, `right`, `nextlevel`, `perfect` | – | Entradas del mapa del mundo |

### Acciones (`aNN = verbo,arg1,…`): 48 verbos, 2280 usos (más 1 acción vacía)

Los verbos coinciden con las 45 clases `Action*` del binario (§5 de MAPA.md).

| Grupo | Verbos (usos; n.º de argumentos) |
|---|---|
| Flujo | `delay` (317; 1–2), `randomdelay` (1; 2), `jumptoaction` (72; 1–2), `jumptorandomaction` (1; 5), `nop` (27), `waitforidle` (1), `waitforkey` (6; 1–3), `waitforkeyhold` (4; 1) |
| Triggers | `activatetrigger` (111), `disabletrigger` (102), `enabletrigger` (45), `pausetrigger` (9), `finalizetrigger` (3), `activaterandomtrigger` (1; 3) |
| Variables | `set` (78; 2), `add` (15), `sub` (9), `equal` (20; 2–3), `greater` (8), `less` (2), `counter` (5; 1) |
| Objetos | `moveobject` (264; 6–8), `rotateobject` (198; 6–8), `setobjectpos` (35), `link` (90), `unlink` (106; 1–12), `playanimation` (88; 5–8), `moveuv` (12; 5–6), `setrenderstate` (26; 3), `switchrenderstate` (83; 2), `particlefx` (2; 3) |
| Bola | `speedmul` (77; 2), `maxspeed` (1), `addimpulse` (2; 2), `setballdirection` (14; 1), `pressure` (16; 1), `checkpoint` (69; 1–4), `addlife` (1), `addscore` (1) |
| Cámara y juego | `setgamemode` (97; 1), `setguidingobject` (51; 1), `setcamprops` (56; 2–4), `setcameraview` (34; 2), `showtext` (5; 1), `levelcomplete` (4), `levelfailed` (26) |
| Sonido | `playsound` (83; 1–2), `stopsound` (2; 1) |

Las líneas que empiezan por `--` (`--a01`, `--collisionmesh`) están comentadas.

### Mapa del mundo (`worldmap.hgr`)

| Nivel | Archivo |
|---|---|
| 1 | `skybean_level01.hgr` |
| 2 | `skybean_level02.hgr` |
| 3 | `skybean_level03.hgr` |
| 4 | `skybean_level05.hgr` |
| 5 | `skybean_level07.hgr` |
| 6 | `skybean_level06.hgr` |
| 7 | `skybean_level04.hgr` |
| 8 | `skybean_level08.hgr` |

### Cielo

En cada nivel, `O_skybox` carga `skybean_objectlibrary.hgr,O_skybox_bean`, que contiene las mallas `skybox01` (propiedad `skybox = hidas`) y `Plane01`. Los visores las dibujan detrás de todo.

## 8. `.ntx`

| Campo | Tipo |
|---|---|
| cabecera | 7 × `u16` LE: versión (`0x104`), ancho, alto, formato, tamaño de paleta, flags, flags de usuario |
| paleta | tamaño de paleta × 2 bytes en el formato indicado (solo si es > 0) |
| píxeles | ancho × alto: `u8` índice si hay paleta, `u16` LE si no |

Formatos (**verificados visualmente**: la bola y el logo salen rojos y el sol amarillo):
- **7: RGB565**, con el rojo en los bits altos.
- **16: ARGB4444**, con los nibbles A, R, G y B desde el bit alto. El enum del `ka3d` público llama al 16 `A4B4G4R4`; este build guarda ARGB.

Flag 1, color key: el índice 0 de la paleta (o el color 0) es transparente. Flag 2 (comprimido): no aparece en ningún archivo. Los 157 archivos cumplen exactamente la fórmula de tamaño; 8 llevan un byte de relleno al final.

## 9. Herramientas

```bash
python3 -I tools/hgr.py info|tree|props|json escena.hgr
python3 -I tools/hgr_export.py glb  escena.hgr salida.glb    # glTF 2.0 para Blender
python3 -I tools/hgr_export.py html escena.hgr salida.html   # visor three.js autónomo
python3 -I tools/hgr_export.py png  escena.hgr salida.png [--view iso|top|camera] [--size 960x540]
python3 -I tools/ntx.py *.ntx -o carpeta_png
```

- Sin dependencias: solo Python 3 estándar. El visor HTML carga three.js 0.160.0 desde jsDelivr.
- Las texturas se buscan en `resources/textures` y los `load` en `resources/models` (se cambian con `--textures` y `--lib`).
- La salida lleva incrustados assets del juego. Es para uso local y no se publica.
- En glTF el nodo raíz invierte Z (para pasar a mano derecha) y escala ×0,01. Las propiedades de cada nodo van en `extras.props` y su rol (`visual`, `collision`, `skybox`, `trigger`, `dummy`) en `extras.role`.
- Todavía no se exportan las animaciones ni los volúmenes de colisión de los triggers como geometría.

## 10. `localization.bin`

Todo en big-endian. Cadena = `u16` longitud + UTF-8. Leído hasta el último byte (116 825).

| Campo | Contenido |
|---|---|
| `u16` n claves, `u16` 0 | 69 |
| n × cadena | Claves (`LANGUAGE_CHOOSE`, `MI_MAIN_PLAY`, `TEXT_STORY1`…) |
| `u16` n idiomas | 56 |
| n × `u16` | Id de idioma (probablemente el `TLanguage` de Symbian; English = 0) |
| n × cadena | Nombre del idioma (`English`, `Spanish`, `Spanish AM`…) |
| n × `u32` | Offset del bloque de cada idioma, relativo al final de esta tabla |
| bloques | Por idioma: 69 cadenas en el orden de las claves |

`LANGUAGE_CODE` da el código ISO (`en`, `es`…), útil para elegir el idioma según la configuración de la consola.

## 11. Fuente `FONT_*.bin` + `FONT_*.ntx`

`u16` BE × 4 de cabecera: n glifos (1379), 19, 22, 0. Siguen n registros de 7 × `u16` BE: código Unicode (ordenados), x, y, ancho y alto en el atlas `.ntx`, `bearing` y 0. La parte superior del glifo queda `bearing` píxeles por encima de una línea de referencia situada 6 px sobre la línea base. No guarda el avance. El atlas es una sola columna de 256×3412 en ARGB4444: glifos blancos con contorno oscuro.

## 12. Paquete `.sis` (Symbian 9)

Implementado en `tools/sis_extract.py`. Extrae los 260 archivos byte a byte iguales a la extracción de referencia y verifica el SHA-1 que guarda el paquete para cada uno.

- **Cabecera:** 4 `u32` LE (UID1 `0x10201A7A`, UID2, UID3 = `0x2000C4D4`, checksum).
- **Árbol de campos:** cada campo es `u32` tipo, `u32` longitud (con el bit 31 activo, la longitud es de 64 bits) y datos con relleno a 4 bytes. Los elementos de un array omiten el tipo.
- **Estructura:** `SISContents` contiene `SISCompressed(SISController)` y `SISData`. El controlador lleva un `SISInstallBlock` con las `SISFileDescription` (destino UTF-16, hash, índice) y los bloques `SISIf`. `SISData` contiene `SISDataUnit` → `SISFileData` → `SISCompressed` (0 sin comprimir, 1 zlib).
