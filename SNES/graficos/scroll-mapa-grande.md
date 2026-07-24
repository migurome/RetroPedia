---
titulo: Scroll de un mapa grande (streaming de tilemap)
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, scroll, tilemap, vram, streaming, vmain]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Scrolling_a_large_map]
---

# Scroll de un mapa grande (streaming de tilemap)

## Resumen

Técnica para simular un mapa de nivel mucho más grande que cualquier tilemap físico de la PPU: en vez de tener todo el nivel cargado en VRAM, se va escribiendo solo la franja de tiles justo por delante de la dirección de scroll, aprovechando que los tilemaps de la PPU "envuelven" (wrap) al llegar al final. Es la base de cualquier scroll de nivel más grande que la pantalla en SNES.

## Detalle técnico

### El problema

Muchos juegos necesitan que la acción transcurra en un área más grande de lo que cabe en una sola pantalla. Aunque los tilemaps de SNES pueden ser más grandes que la resolución de pantalla, incluso el tamaño máximo de tilemap se queda corto para la mayoría de niveles reales.

### La solución: scroll "infinito" simulado

La técnica consiste en dar la ilusión de un mapa infinitamente grande determinando la dirección en la que se está desplazando el scroll y escribiendo continuamente datos nuevos justo por delante de lo que el jugador puede ver en cada momento. Esto funciona porque los tilemaps de la PPU "dan la vuelta": intentar mostrar datos más allá del final de un tilemap simplemente hace que se vuelva a leer desde el principio (izquierda o arriba), en vez de salirse de rango. Aprovechando ese wrap, basta con ir sobrescribiendo la franja de tiles que va a "reaparecer" por el lado opuesto con los datos correctos del nivel, justo antes de que el jugador llegue a verla.

### Algoritmo, por cada eje (X e Y)

1. Guardar una copia de la posición de scroll actual.
2. Calcular la nueva posición de scroll (tras aplicar el movimiento del frame).
3. Comparar la posición de scroll anterior con la nueva para saber si hace falta actualizar el tilemap. Un truco práctico para esto: `(Old ^ New) & TileSize` — un XOR entre la posición vieja y la nueva, enmascarado con el tamaño de tile, permite detectar de forma barata si el scroll ha cruzado un límite de tile (borde de tile), que es el único momento en que hace falta escribir tiles nuevos.
4. Si hace falta actualizar, comparar la posición nueva con la vieja para saber la dirección del movimiento (si aumentó o disminuyó).
5. Elegir, según la dirección y la posición de scroll, qué columna o fila del tilemap toca actualizar (la que va a "entrar" en pantalla por el borde correspondiente).
6. Calcular la dirección en VRAM de esa columna/fila, así como la dirección correspondiente dentro de los datos de nivel (en ROM/RAM) de donde hay que sacar los tiles.
7. Rellenar un buffer con los datos de tile correspondientes, de forma específica al juego (por ejemplo, resolviendo metatiles, animaciones, etc.).
8. Copiar ese buffer a VRAM durante el siguiente VBlank (nunca fuera de VBlank/Force-Blank, por las restricciones normales de acceso a VRAM).

### VMAIN e incremento por columnas

Se menciona explícitamente el registro **VMAIN**: su modo de "incremento de 32 palabras" es justo el que conviene usar para escribir columnas de datos de tile, ya que cada fila del tilemap ocupa 32 palabras y ese modo de incremento permite avanzar directamente de una fila a la siguiente dentro de la misma columna sin tener que recalcular la dirección manualmente en cada palabra.

## Notas / gotchas

- El truco `(Old ^ New) & TileSize` es la forma barata de detectar cruces de límite de tile sin tener que hacer divisiones o comparaciones más costosas en cada frame.
- Solo hace falta escribir en VRAM cuando el scroll cruza un límite de tile, no en cada frame ni en cada píxel de movimiento — hacerlo con más frecuencia desperdicia tiempo de VBlank sin necesidad.
- La escritura real a VRAM debe reservarse para VBlank (o Force-Blank), igual que cualquier otro acceso a memoria de la PPU.
- Usar el modo de incremento de 32 palabras de VMAIN simplifica mucho escribir una columna completa del tilemap de una sola tirada.
- Esta técnica depende del tamaño del buffer intermedio: conviene dimensionarlo para que la transferencia a VRAM quepa cómodamente en el tiempo de VBlank disponible (ver la entrada sobre la rutina de VBlank para las consideraciones de colas/buffers).

## Referencias cruzadas

- Ver `vblank-routine.md` (patrones de buffers/colas usados para preparar y transferir estos datos de tilemap durante VBlank).
- Ver `leer-escribir-memoria-ppu.md` (detalle de VMAIN y el direccionamiento de VRAM usado en este algoritmo).
