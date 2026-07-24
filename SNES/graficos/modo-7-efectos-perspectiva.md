---
titulo: Modo 7 — transformación afín y efectos de perspectiva
plataforma: [SNES]
categoria: [graficos]
tags: [modo7, ppu, hdma, rotacion, escalado, perspectiva, matriz]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Mode_7_perspective_effects, https://snes.nesdev.org/wiki/Mode_7_transform, https://snes.nesdev.org/wiki/Backgrounds, https://snes.nesdev.org/wiki/Tilemaps, https://snes.nesdev.org/wiki/Tiles, https://snes.nesdev.org/wiki/Registers]
---

# Modo 7 — transformación afín y efectos de perspectiva

## Resumen

El modo 7 es el modo de fondo más icónico de SNES: una única capa de 128x128 tiles a la que se le puede aplicar una transformación afín 2D completa (traslación, rotación, escalado, cizalla) controlada por una matriz de registros. Cambiando esa matriz scanline a scanline mediante HDMA se consiguen los famosos efectos de "pseudo-3D" tipo carretera o suelo en perspectiva (F-Zero, Mario Kart, etc.). Esta entrada cubre el formato de tile/tilemap propio del modo 7, la matriz de transformación y su fórmula, y el mecanismo de HDMA que produce la perspectiva.

## Detalle técnico

### La página "Mode 7 perspective effects" es un índice, no un tutorial

Conviene aclarar de entrada que la página del wiki dedicada específicamente a "Mode 7 perspective effects" es, en el propio SNESdev Wiki, solo una página-índice: no contiene prosa técnica propia, fórmulas ni código; se limita a enlazar a otras páginas del wiki ("Backgrounds: Mode 7", "Mode 7 transform", "Tilemaps: Mode 7", "Tiles: Mode 7") y a dos recursos externos:

- **Dizworld**: ejemplos de efectos con código fuente, por rainwarrior — https://github.com/bbbradsmith/SNES_stuff/tree/main/dizworld#readme
- **"Kulor's Guide to Mode 7 Perspective Planes"**: hilo del foro NESdev — https://forums.nesdev.org/viewtopic.php?t=24053

Por eso, para poder dar el máximo detalle real sobre el tema (tal como pide esta tarea), esta entrada combina el contenido de esas páginas enlazadas dentro del propio wiki de SNESdev (Backgrounds, Tilemaps, Tiles, Mode 7 transform, Registers), que sí son las que documentan de verdad el hardware. Los dos recursos externos (Dizworld y el hilo de Kulor) no se han volcado aquí — quedan anotados como pendiente de revisión si se necesita profundizar más en el efecto de perspectiva concreto tipo "carretera".

### Cómo es la capa de fondo del modo 7

El modo 7 funciona de forma muy distinta a los demás modos de fondo:

- Ocupa siempre la primera mitad de VRAM con un único tilemap de 128x128 tiles.
- Solo dispone de 256 tiles posibles (frente a los 1024 de los tilemaps normales).
- Es una única capa de fondo (BG1), sin las variantes de 2, 3 o 4 capas de otros modos.
- El registro **M7SEL** (`$211A`) controla comportamientos específicos del modo 7: se puede voltear la pantalla entera en horizontal y/o en vertical, y se puede elegir qué pasa en los bordes del mapa cuando la transformación "sale" de él: dejarlo transparente, rellenarlo con el tile 0, o hacer que el tilemap se repita infinitamente (wrap) tanto en horizontal como en vertical.
- Admite también un modo de color directo, saltándose la paleta indexada si se desea.

Aunque solo hay una capa de fondo transformable, es habitual combinarla con sprites alrededor (por ejemplo, un coche o personaje en primer plano) y usar HDMA para cambiar a otro modo de fondo en la parte de la pantalla que no necesita transformación (por ejemplo, un suelo sólido en la parte inferior de la pantalla en vez de terreno en perspectiva ahí).

**EXTBG**: activable desde SETINI, "duplica" BG1 como una segunda capa (BG2), pero partiendo la imagen en dos según el bit alto de cada píxel: los valores de 1-127 van a la capa inferior y los de 128-255 a la superior. Esto da, en la práctica, una selección de color de 7bpp con separación en dos capas con prioridades distintas. Limitación importante: EXTBG no admite color directo para BG2 — esa capa siempre usa color indexado, aunque BG1 sí puede seguir en color directo.

### Formato del tilemap de modo 7

El tilemap de modo 7 es de 128x128 tiles, siempre empieza en la dirección 0 de VRAM, y ocupa la primera mitad de la memoria de vídeo. A diferencia de los tilemaps normales (entradas de 16 bits con paleta/prioridad/flip/número de tile), aquí:

- El índice de tile se guarda **solo en el byte bajo** de cada palabra de VRAM; el dato de tile en sí se guarda en los bytes altos.
- Cada byte de tilemap es un índice de 8 bits a uno de los 256 tiles disponibles.
- No hay atributos: no existe paleta por tile, ni prioridad, ni flip a nivel de tile (el "per-pixel priority" solo se consigue vía EXTBG, no por atributo de tile).
- Las filas se guardan de forma contigua: 128 tiles seguidos por fila, de arriba a abajo.

### Formato de los tiles de modo 7

También difiere radicalmente del formato "planar" normal:

- Los datos de tile se guardan **solo en el byte alto** de cada palabra de VRAM (el byte bajo se ignora para esto).
- Cada byte corresponde a **un píxel**, dando un índice de paleta de 8 bits directamente (formato "chunky", no planar).
- El orden de los píxeles dentro del tile es de izquierda a derecha y de arriba a abajo.
- Con EXTBG activo, esos mismos bytes de píxel se reparten en dos capas según su valor (1-127 capa inferior, 128-255 capa superior).
- Existe también una variante de color directo, con el mismo layout "chunky" de un byte por píxel, pero interpretando el byte directamente como color RGB en el formato `BBGG GRRR`, que se convierte a `RRR00 GGG00 BB000` — es decir, con menos precisión de color que otros modos de color directo de SNES.

### La matriz de transformación afín

El modo 7 aplica una transformación afín 2D arbitraria sobre el tilemap de 128x128 tiles (equivalente a 1024x1024 píxeles), permitiendo traslación, rotación, escalado uniforme y cizalla (shear). Esa transformación se puede cambiar en cada scanline mediante HDMA, que es justo lo que produce los efectos de perspectiva tipo "suelo/carretera".

**Registros de la matriz** (todos en formato de punto fijo 8.8):

- **M7A** (`$211B`), **M7B** (`$211C`), **M7C** (`$211D`), **M7D** (`$211E`): los cuatro coeficientes de la matriz de transformación 2x2.
- **M7X** (`$211F`), **M7Y** (`$2120`): punto de pivote / centro de la transformación, en coordenadas de texel del mapa.
- **M7HOFS** (`$210D`), **M7VOFS** (`$210E`): desplazamiento de scroll aplicado *después* de la transformación, en coordenadas de píxel de pantalla (estos registros son los mismos físicamente que BG1HOFS/BG1VOFS, reutilizados con otro significado en modo 7).
- **M7SEL** (`$211A`): flags de comportamiento (flip de pantalla, relleno/wrap en los bordes, color directo).

**Fórmula de la transformación** (mapea un píxel de pantalla `(Sx, Sy)` a un texel del mapa `(Tx, Ty)`):

```
| M7A  M7B |   | Sx + M7HOFS - M7X |   | M7X |   | Tx |
|          | × |                    | + |     | = |    |
| M7C  M7D |   | Sy + M7VOFS - M7Y |   | M7Y |   | Ty |
```

**Cómo leer la matriz en términos de movimiento**:

- Al moverse un píxel hacia la derecha en pantalla, **M7A** determina cuánto se mueve en horizontal dentro del mapa (texels) y **M7C** cuánto se mueve en vertical.
- Al moverse un píxel hacia abajo en pantalla, **M7B** determina el movimiento horizontal en el mapa y **M7D** el movimiento vertical.

Los valores por defecto `(M7A,M7B)=(1,0)` y `(M7C,M7D)=(0,1)` hacen que el modo 7 se comporte como un fondo normal sin transformar.

**Ejemplos de transformaciones**:

- **Escalado**: `(2,0)(0,1)` encoge el mapa a la mitad en horizontal; `(1,0)(0,-0.1)` estira el mapa 10 veces en vertical y además lo invierte (voltea) verticalmente.
- **Rotación**: `(cos θ, sin θ)(-sin θ, cos θ)` rota la vista en sentido antihorario en pantalla.
- **Cizalla (shear)**: `(1,0)(0.2,1)` produce un desplazamiento progresivo hacia la izquierda a medida que se baja en pantalla.

**Flujo de trabajo resumido**: en pantalla, primero se establece el punto de pivote con M7X/M7Y, luego se aplica la rotación/escalado con la matriz A-B-C-D, y por último se añade el scroll con M7HOFS/M7VOFS. Visto desde el mapa, el paralelogramo definido por los vectores de la matriz determina qué región del mapa 128x128 queda realmente visible en pantalla en ese instante.

### De la matriz estática a la perspectiva (HDMA scanline a scanline)

El "efecto de perspectiva" (el suelo o carretera que se aleja hacia un punto de fuga, como en los clásicos de carreras de SNES) se consigue recalculando la matriz de transformación **una vez por scanline** y volcando los nuevos valores mediante HDMA justo antes de que la PPU dibuje esa línea. La idea es que cada scanline representa una "distancia" distinta de la cámara virtual: las líneas más cercanas a la parte inferior de la pantalla corresponden a puntos más cercanos (mapa mostrado con menos escalado/más grande), y las líneas más arriba corresponden a puntos más lejanos (mapa mostrado más escalado/más pequeño), lo que da la sensación de profundidad. Como M7A-M7D, M7X, M7Y y M7HOFS/M7VOFS son en su mayoría registros de escritura doble, para actualizarlos por HDMA scanline a scanline hace falta usar el patrón DMAP correspondiente a "un registro, escritura doble" (patrón 2) o "dos registros, escritura doble" (patrón 3), igual que para el resto de registros de escritura doble de la PPU.

## Notas / gotchas

- La página oficial "Mode 7 perspective effects" del wiki no trae ni fórmulas ni código propios: es solo un índice de enlaces a otras páginas del propio wiki y a dos recursos externos (Dizworld en GitHub, y un hilo del foro NESdev). El contenido técnico real del modo 7 vive repartido en "Backgrounds", "Tilemaps", "Tiles" y "Mode 7 transform".
- El tilemap y los tiles de modo 7 usan un layout completamente distinto al resto de modos (byte bajo vs. byte alto de cada palabra de VRAM, formato "chunky" de 1 byte = 1 píxel en vez de planar): no se puede reutilizar directamente el código de generación de tiles/tilemaps de otros modos.
- No hay atributos por tile en modo 7 (ni paleta, ni flip, ni prioridad); el único mecanismo de "capas"/prioridad es EXTBG, y EXTBG no admite color directo en su segunda capa.
- Todos los registros de la matriz (M7A-M7D, M7X, M7Y) y M7HOFS/M7VOFS son de escritura doble en formato 8.8 de punto fijo: cualquier HDMA que los actualice debe usar un patrón DMAP compatible con escritura doble (patrón 2 o 3), nunca patrón 0 o 1.
- M7HOFS/M7VOFS comparten dirección física con BG1HOFS/BG1VOFS (`$210D`/`$210E`); su significado cambia según si el modo de fondo activo es el modo 7 o no.
- Pendiente de revisar en detalle (no volcado en esta entrada): los recursos externos enlazados por la página original — el código fuente de "Dizworld" (rainwarrior, GitHub) y el hilo "Kulor's Guide to Mode 7 Perspective Planes" del foro NESdev — que sí entran en el detalle práctico de tablas de perspectiva/horizonte para el efecto tipo carretera.

## Referencias cruzadas

- Ver `hdma-ejemplos.md` (mecánica general de HDMA, patrones DMAP y formato de tabla usados para animar la matriz de modo 7 scanline a scanline).
- Ver `leer-escribir-memoria-ppu.md` (VMAIN y direccionamiento de VRAM aplicables también al tilemap/tiles de modo 7).
- Ver `ppu-para-desarrolladores-nes.md` (contexto general de modos de fondo y registros de la PPU de SNES).
