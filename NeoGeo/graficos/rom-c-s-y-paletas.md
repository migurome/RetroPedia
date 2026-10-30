---
titulo: Neo Geo — formatos de la ROM C (sprites), la ROM S (capa fix) y las paletas
plataforma: [NeoGeo]
categoria: [graficos]
tags: [rom-c, rom-s, tiles, bitplanes, paletas, color, dark-bit, backdrop, python]
estado: revisado
fecha: 2026-10-23
fuentes: [tiletool.py y paltool.py de ngdevkit 0.5, proyecto RickAndMorty/neogeo (tools/build_gfx.py, tools/build_fix.py) probado en GnGeo]
---

# Formatos de la ROM C, la ROM S y las paletas

## Resumen

Los tiles de sprite (16×16, 4 bpp) van repartidos entre dos ROM, **C1** con los planos 0 y 1 y **C2** con los planos 2 y 3. Los de la capa fix (8×8, 4 bpp) van en la **ROM S**. Las paletas son de 16 colores de 16 bits, y el color 0 es siempre transparente. Con estos tres formatos se puede generar todo con Python y Pillow, sin las herramientas de ImageMagick de los ejemplos.

## Detalle técnico

### Tile de sprite (ROM C): 64 bytes en C1 y 64 en C2

- El tile se recorre por cuadrantes de 8×8 en este orden: **arriba-derecha, abajo-derecha, arriba-izquierda y abajo-izquierda**. Son los desplazamientos 8, 136, 0 y 128 en un búfer de 16×16.
- Por cada fila del cuadrante se escriben dos bytes en C1 (planos 0 y 1) y dos en C2 (planos 2 y 3). El bit `x` de cada byte es el píxel `x` de la fila, con el LSB a la izquierda.

```python
def encode_crom_tile(tile):            # tile: 256 índices de color 0-15
    c1, c2 = bytearray(), bytearray()
    for start in (8, 136, 0, 128):
        for y in range(8):
            p = [0, 0, 0, 0]
            for x in range(8):
                col = tile[start + y * 16 + x]
                for b in range(4):
                    p[b] |= ((col >> b) & 1) << x
            c1 += bytes(p[0:2]); c2 += bytes(p[2:4])
    return bytes(c1), bytes(c2)
```

- El número de tile es simplemente la posición en la ROM: el tile `n` ocupa los bytes `64*n` de C1 y de C2.
- Conviene reservar el **tile 0 vacío** y **quitar los tiles repetidos** con un diccionario. En el proyecto, los 22 fotogramas de dos personajes quedaron en 486 tiles (62 KB).

### Tile fix (ROM S): 32 bytes

- Columnas por pares en el orden (4,5), (6,7), (0,1) y (2,3).
- Para cada par, las 8 filas: un byte por fila con el píxel derecho en el nibble alto.

```python
for xa, xb in ((4, 5), (6, 7), (0, 1), (2, 3)):
    for y in range(8):
        out.append(tile[8*y + xb] << 4 | tile[8*y + xa])
```

- Disposición que espera libngdevkit: tile N = carácter ASCII N y tile 255 vacío (es el que usa `ng_cls()`). La fuente Unscii 8×8 (dominio público, la de los ejemplos) tiene los caracteres ASCII 32-127 en tres filas de 32, a partir de la posición (2,1) de `unscii8.png`.

### Color de 16 bits

```
bit 15     14 13 12      11-8     7-4      3-0
oscuro     R0 G0 B0      R4-R1    G4-G1    B4-B1
```

Conversión desde RGB de 8 bits, con el mismo criterio que `paltool.py`:
1. Se reduce cada canal a 6 bits (`c >> 2`).
2. El bit "oscuro" vale 1 solo si el LSB de los tres canales es 0.
3. Se guarda el siguiente bit de cada canal en los bits 14-12 y los 4 altos en los nibbles.

### Dónde van las paletas

- La RAM de paletas está en `0x400000` (`MMAP_PALBANK1` en ngdevkit): la paleta `p` y el color `i` están en `MMAP_PALBANK1[p*16 + i]`.
- La paleta de cada tile de sprite va en su atributo (bits 15-8). La de cada tile de la capa fix va en los bits 15-12 del mapa, así que la capa fix solo puede usar las paletas 0-15.
- **Color de fondo (*backdrop*):** es el último color de la RAM de paletas, en `0x401FFE`. Es lo que se ve donde no hay ningún sprite ni tile. ✔

## Notas / gotchas

- Una hoja de sprites "de 12 colores" puede traer decenas de tonos sueltos de 1 a 5 píxeles, por ruido de edición. Antes de asignar paletas hay que cerrar la paleta: contar los colores, quedarse con los que tienen presencia real (en el proyecto, ≥ 0,1 % de los píxeles) y llevar el resto al más cercano. Sin ese paso, Morty salía con 18 colores en lugar de 12 y no cabía en una paleta.
- Si un personaje necesita más de 15 colores, cada tile puede usar su propia paleta, pero ningún tile puede mezclar colores de dos paletas. Es la forma habitual de pasar de 15 colores por personaje (sin probar todavía en el proyecto).
- Para comprobar el formato sin emulador, el conversor reconstruye cada fotograma decodificando los tiles ya codificados (`preview_<pj>.png`). Si la vista previa sale bien, la ROM C está bien.

## Referencias cruzadas

- [Sprites: SCB1-SCB4, cadenas, espejo y orden](sprites-scb.md)
- [Caso práctico: SimuLockdown en Neo Geo](../herramientas/caso-practico-simulockdown.md)
- [Rejilla de pixel art y resolución nativa](../../EXTRACCION/rejilla-pixel-art-desde-video.md)
