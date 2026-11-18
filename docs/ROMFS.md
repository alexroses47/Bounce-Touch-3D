# romfs de la 3DS: contenido y formatos

`tools/convert_assets.py` lo genera a partir de tu `.sis`. El repositorio no lleva ningún asset.

```bash
python3 -I tools/convert_assets.py "Bounce Touch v1.00(57).sis" romfs/ [--work DIR]
```

- **Requisitos:** Python 3 (solo biblioteca estándar), `tex3ds` (devkitPro) y `oggenc` (`brew install vorbis-tools`).
- **Proceso:**
  1. Comprueba el SHA-256 del `.sis`; acepta otras versiones con `--force`.
  2. Lo extrae con `tools/sis_extract.py`, verificando el SHA-1 de cada archivo.
  3. Convierte y valida.
- **Resultado medido:** 6,8 s, 256 archivos, 3,67 MB.
- **Salida reproducible:** partiendo del `.sis` o de la carpeta extraída, el romfs sale idéntico byte a byte (salvo `manifest.txt`). Los `.ogg` usan un número de serie fijo, derivado del nombre.

## Contenido

| Carpeta | Archivos | Origen | Conversión |
|---|---|---|---|
| `scenes/*.bsc` | 15 | `bundles/original/*.hgr`, `resources/models/*.hgr` | Formato BSC1 (abajo). Se valida leyéndolo de vuelta contra el `.hgr` |
| `tex/*.t3x` | 79 | `resources/textures/*.ntx` | `tex3ds` con mipmaps (`-m box`), comprimido LZ11 |
| `ui/*.t3x` | 76 | `resources/images/*.ntx`, salvo `FONT_*` y `Nokia_splash` | `tex3ds` sin mipmaps. Lo que no es potencia de 2 se rellena; el tamaño real queda como subtextura del `.t3x` |
| `font/menu_medium.bft` + `menu_medium_{0..3}.t3x` | 5 | `FONT_MENU_MEDIUM.bin` + `.ntx` (256×3412, más de 1024) | 1379 glifos reempaquetados en 4 páginas de 512×512, `rgba4` |
| `audio/music/*.ogg` | 10 | `resources/musics/*.wav` | Ogg Vorbis q6, 8 kHz mono (como celeste64-3ds: Tremor + ndsp). Misma longitud exacta en muestras (granulepos comprobado) para que los bucles enlacen |
| `audio/sfx/*.ogg` | 67 | `resources/sounds/*.wav` | Igual |
| `text/localization.bin` | 1 | `resources/localization/localization.bin` | Copia (formato en `HGR.md` §10) |
| `particles/*.prs` | 3 | `resources/textures/*.prs` | Copia (texto) |
| `manifest.txt` | 1 | – | Versión del convertidor, SHA-256 del origen, archivos, bytes |

Todos los nombres van en minúsculas. El motor debe pasar a minúsculas los nombres que lee del juego (`skybean_wood.tga` → `tex/skybean_wood.t3x`, `metal_machine.wav` → `audio/sfx/metal_machine.ogg`).

Formato de textura según el alfa de la imagen decodificada (mismo criterio que celeste64-3ds):

| Formato | Cuándo | Cuántas |
|---|---|---|
| `rgb565` | opaca | 57 |
| `rgba5551` | alfa todo o nada (color key) | 29 |
| `rgba4` | alfa parcial | 69 |

En los tres casos la conversión es sin pérdida respecto al original, salvo el bit verde de más de RGB565 en las texturas con color key.

## BSC1: escena

Todo en little-endian. Cada tabla está alineada a 4 bytes, y los bloques de vértices e índices a 16. **Los offsets son absolutos** desde el inicio del archivo: se carga entero en memoria lineal (`linearAlloc`) y la GPU usa los bloques sin copiarlos.

```c
typedef struct { u32 off, count; } BscRange;      // en la cabecera van como pares {count, off}

typedef struct {            // 84 bytes
    char magic[4];          // "BSC1"
    u32  version;           // 1
    u32  file_size;
    u32  strings_off, strings_size;   // UTF-8 terminadas en NUL; el offset 0 es ""
    u32  tex_count,  tex_off;         // BscTexture[]
    u32  mat_count,  mat_off;         // BscMaterial[]
    u32  prim_count, prim_off;        // BscPrimitive[]
    u32  node_count, node_off;        // BscNode[]
    u32  anim_count, anim_off;        // BscAnim[]
    u32  ref_count,  ref_off;         // u32[]: primitivas de cada malla
    u32  vbo_off, vbo_size;           // BscVertex[]
    u32  ibo_off, ibo_size;           // u16[]
} BscHeader;

typedef struct { u32 name; } BscTexture;          // offset de cadena: nombre sin extensión → romfs:/tex/<name>.t3x

enum { BSC_MAT_TEXTURED = 1, BSC_MAT_MULTIPLY = 2, BSC_MAT_NO_ZWRITE = 4 };
typedef struct {            // 16 bytes
    u32 flags;
    s32 texture;            // índice en BscTexture, -1 sin textura
    u32 rgba;               // DIFFUSEC (R en el byte bajo), para los "plain"
    u32 shader;             // cadena con el shader original (unlit-tex, …)
} BscMaterial;

typedef struct { s16 x, y, z, u, v, pad; } BscVertex;   // 12 bytes

typedef struct {            // 64 bytes; siempre lista de triángulos
    u32   material;
    u32   vertex_count, index_count;
    u32   vbo_off, ibo_off; // absolutos
    float pos_scale, pos_bias[3];   // posición = (x,y,z) * pos_scale + pos_bias
    float uv_scale;                 // uv = (u,v) * uv_scale   (1/4096: punto fijo 4.12)
    float bbox_min[3], bbox_max[3]; // en espacio local de la malla
} BscPrimitive;

enum { BSC_MESH, BSC_CAMERA, BSC_LIGHT, BSC_DUMMY, BSC_LINES, BSC_NODE };
enum { BSC_VISUAL, BSC_COLLISION, BSC_SKYBOX, BSC_TRIGGER, BSC_DUMMYROLE, BSC_OTHER };
typedef struct {            // 100 bytes
    u32   name;
    s32   parent;           // índice en BscNode, -1 = raíz
    u8    kind, role;
    u16   reserved;
    float transform[12];    // 3x4 por filas; columna 3 = traslación (Y arriba, mano izquierda)
    u32   first_ref, ref_count;     // primitivas: refs[first_ref .. +ref_count)
    float params[6];        // dummy: caja min[3], max[3]; cámara: near, far, fov
    s32   anim;             // índice en BscAnim, -1 sin animación
    u32   props;            // cadena: propiedades de usuario tal cual ("clave = valor" por líneas)
} BscNode;

typedef struct {            // 36 bytes
    u8    pos_rate, rot_rate, scl_rate, end;   // end: 0 repetir, 1 oscilar, 2 reiniciar, 3 parar
    u32   pos_count, pos_off;                  // float[4] por clave
    u32   rot_count, rot_dim, rot_off;         // float[rot_dim] por clave (3 o 4)
    u32   scl_count, scl_off;                  // float[4] por clave
    float end_time;
} BscAnim;
```

**Rol del nodo** (lo calcula el convertidor; el motor puede ignorarlo):
- `collision`: malla con `collisionmesh = this`.
- `skybox`: nodo que se llama `*skybox*` o tiene la propiedad `skybox`, y todo lo que cuelga de él.
- `trigger`: dummy `T_*`.

**Valores de ejemplo:**
- Para el PICA200, los atributos `x, y, z` y `u, v` se declaran como `GPU_SHORT`. El shader aplica `pos_scale`, `pos_bias` y `uv_scale` como uniforms, sin convertir nada en la CPU.
- Las animaciones guardadas (114) son solo las que tienen más de una clave. Las claves ya están descuantizadas a `float`.
- Tamaño de las 15 escenas: 1,14 MB.

## BFT1: fuente

```c
typedef struct {            // 16 bytes
    char magic[4];          // "BFT1"
    u16  glyph_count, page_count;
    u16  line_height;       // 19 (valor original de la cabecera)
    u16  size;              // 22 (valor original; probablemente altura máxima)
    u16  page_w, page_h;    // 512, 512 → romfs:/font/<nombre>_<página>.t3x
} BftHeader;

typedef struct {            // 12 bytes, ordenados por code
    u32 code;               // Unicode
    u16 x, y;               // en la página
    u8  w, h;
    u8  page;
    s8  bearing;            // parte superior del glifo = referencia − bearing
} BftGlyph;
```

Probado componiendo texto en español a partir de las páginas: el glifo se coloca con su parte superior en `línea_base − 6 − bearing` y se avanza `w − 2` píxeles, porque el contorno de 1 px se solapa. El espaciado exacto del original se comprobará en el hito 6. La fuente solo trae los glifos que usan los textos del juego: hay `ñ` pero no `Ñ`. En 6 de los 56 idiomas faltan `–`, `“` o `’`, que ya faltaban en el original y se sustituirán por `-` y `"`.
