# Física de la bola: comportamiento observado

**Estado de cada regla: `docs/GUIONES.md`, sección «Reglas del motor y su evidencia».** Lo que hace el juego original, descrito en palabras propias a partir del análisis en Ghidra (proyecto local `re/`). El port lo reimplementa en `port/source/ball.cpp` sin copiar código. Unidades: las del `.hgr`; el radio visual de la bola es de unas 51 unidades.

## Integración (todos los objetos activos)

Cuerpo rígido simple, una vez por fotograma con el `dt` real:

1. Se acumulan **fuerzas** (por ejemplo la gravedad, `g · masa`) e **impulsos** (control y saltos).
2. `v += F / m · dt` y `v += J / m`; luego se vacían los acumuladores.
3. `p += v · dt`.

| Parámetro | Valor | Origen |
|---|---|---|
| Masa | 1,0 | Constructor de `ActiveGameObject` |
| Gravedad | (0, −2000, 0) u/s² | Global; la acción `setgravity` la cambia |
| Velocidad máxima | 2000 u/s (por defecto; acción `maxspeed`) | Se aplica recortando el módulo de `v` cada fotograma |
| Límite de velocidad por forma | normal 800, roca 1500, barro 800 | Tabla constante; acción `maxspeed` con tipo ≠ 3 |
| Impulso de control | 1750 | Constante |
| Radio de colisión | 50 | `D_bounce radius=50` en `bounce.hgr`; traza EKA2L1 (centro a 49,9–50,0 del suelo plano). El 55 del constructor de `Ball` (+0x138) no es el radio |
| Restitución | 0,1 solo en superficies con bit 15 de `group`; 0,001 en el resto | `PlayerBall::v05` (0x4f2c8) lee el campo +0x134; el constructor pone 0,4 pero en ejecución vale 0,1 (traza EKA2L1; escritor pendiente); bits en `FUN_00021fe8` |

## Control (tecla → efecto, solo en el modo de juego normal)

Las direcciones salen de la cámara proyectada en horizontal. «En el suelo» significa que hubo contacto este fotograma (flag `+0x108`).

| Entrada | Efecto |
|---|---|
| Izquierda / derecha | **Gira** la velocidad alrededor de Y: ±90°/s parada, ±135°/s en marcha (v² horizontal > 15). En el aire, ×0,25. Se atenúa con `1 − |v| / (1,5 · vmax)`. La orientación de la bola (y por tanto la cámara) gira ×1,2 de ese ángulo. Además aplica un impulso lateral `1750 · (1,4 en el suelo / 0,25 en el aire) · dt` si la velocidad en esa dirección no pasa del límite de la forma |
| Adelante | Impulso `1750 · (1,4 / 0,25) · dt` hacia delante si la velocidad en esa dirección no pasa del límite de la forma |
| Atrás | Igual, hacia atrás, con la mitad de límite |
| Acción | Salto (abajo) |
| Otras | Cambiar de forma en el modo 4 (rota entre 3 formas); cámara: inclinación ±89° a 45°/s; teclas de depuración para volar |

## Salto

- **Requisito:** estar en el suelo, con una normal de contacto de componente vertical ≥ 0,5.
- **Preparación:**
  1. Se vacían fuerzas e impulsos y se quita la componente de `v` a lo largo de la normal.
  2. `v ×= 0,2`.
  3. `k = max(0, 1 − |v| / límite_forma)`.

| Forma | Dirección (se normaliza) | Impulso |
|---|---|---|
| Normal | `(0, 4,2 · (0,25 + 0,75k), 0) + dirección_cámara · k` | 1300 |
| Barro | `(0, 0,8 · (0,25 + 0,75k), 0) + dirección_cámara · k` | 800 |
| Roca | (sin leer todavía) | – |

Con la forma normal parada, el salto sube unas 1300² / (2 · 2000) ≈ 420 unidades, unas 4 bolas.

## Contactos y respuesta

- El sistema de colisión genera, cada fotograma, **contactos** con punto, normal, velocidad de la superficie y el objeto tocado. La bola usa el más cercano.
- **Rebote (jugador):** `PlayerBall::v05` (0x4f2c8) refleja con `e` = campo +0x134 (0,1 en ejecución) solo si el objeto tocado tiene el bit 15 de `group` (`+0x157`); en el resto `e = 0,001`: la bola se asienta. La versión de `Ball::v05` (con `|n.y|`) es la de las otras bolas, no la del jugador (ver `docs/CLASES.md`).
- **Rozamiento:** fuerza contra la velocidad tangencial de módulo `m · 100 · cos(pendiente)`, limitada a la necesaria para frenar en un paso.
- **Aplastamiento:** si dos contactos tienen normales opuestas (producto escalar < −0,985), se dispara el trigger `T_fail`.
- **Forma roca:** al caer (`vy < 0`) con su flag activo, recibe un impulso hacia abajo de 4000.
- **Visual:** la malla rueda alrededor de `normal × v` con ángulo `|v_t| / radio · dt`.
