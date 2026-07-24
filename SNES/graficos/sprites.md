---
titulo: Sprites (OAM)
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, sprites, oam, obj, prioridad, limites-hardware]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Sprites]
---

# Sprites (OAM)

## Resumen

Los sprites (llamados "OBJ" en la documentación oficial) son objetos gráficos independientes del tilemap de fondo, descritos en una memoria dedicada de 544 bytes llamada OAM (Object Attribute Memory) que soporta hasta 128 sprites simultáneos. Esta entrada documenta la estructura exacta de OAM, los atributos de cada sprite y los límites de hardware por scanline.

## Detalle técnico

### Estructura de OAM

OAM tiene 544 bytes divididos en dos tablas:

**Tabla baja** (512 bytes / 256 words): 4 bytes por sprite, para los 128 sprites (índice `i` de 0 a 127):

```
byte i*4+0: XXXX XXXX   bits bajos (0-7) de la posición X
byte i*4+1: YYYY YYYY   posición Y
byte i*4+2: TTTT TTTT   bits bajos (0-7) del número de tile
byte i*4+3: VHPP CCCt
            V   = volteo vertical
            H   = volteo horizontal
            PP  = prioridad (3 = más alta, 0 = más baja)
            CCC = paleta (0-7)
            t   = bit alto del número de tile (name select)
```

**Tabla alta** (32 bytes / 16 words finales): agrupa 4 sprites por byte:

```
byte i/4: DdCc BbAa
  a = sprite (4n+0): bit alto de X
  b = sprite (4n+0): bit de selección de tamaño
  c = sprite (4n+1): bit alto de X
  d = sprite (4n+1): bit de selección de tamaño
  C = sprite (4n+2): bit alto de X
  ... (mismo patrón para los 4 sprites del grupo)
```

### Atributos del sprite

- **Posición X:** valor de 9 bits (8 bits de la tabla baja + 1 bit alto de la tabla alta); el bit alto actúa como desplazamiento de -256, lo que permite colocar sprites parcialmente fuera de pantalla por la izquierda.
- **Posición Y:** 8 bits; da la vuelta (wrap) al llegar al borde superior de la pantalla.
- **Número de tile:** 9 bits (permite seleccionar entre dos páginas de 256 tiles de sprite, configuradas mediante OBJSEL en `ppu-registers.md`).
- **Paleta:** selecciona una de las 8 paletas de sprite en la mitad superior de CGRAM (las paletas 4-7 pueden además participar en color math).
- **Prioridad:** determina el orden de composición del sprite frente a las capas de fondo (no afecta al orden entre sprites, ver más abajo).
- **Volteo horizontal/vertical:** independientes entre sí.
- **Tamaño:** cada sprite elige entre dos tamaños posibles (uno "pequeño" y uno "grande") configurados globalmente en OBJSEL — ver la tabla de combinaciones en `ppu-registers.md`. Tamaños disponibles: 8×8, 16×16, 32×32, 64×64 (cuadrados) y 16×32, 32×64 (rectangulares).

Los sprites de más de un tile se componen de tiles contiguos: horizontalmente en offsets de +1 y verticalmente en +16, siguiendo la misma rejilla de 16 tiles de ancho descrita en `tiles.md`.

### Límites de renderizado por scanline

1. **32 sprites por línea como máximo.** Si más de 32 sprites tienen píxeles en la misma línea, solo se dibujan los de menor índice OAM (evaluados en orden de índice ascendente).
2. **34 "slivers" de 8 píxeles por línea como máximo.** Cada sprite se descompone en franjas de 8 píxeles de ancho para el renderizado; si el total de franjas en una línea supera 34, se evalúan en orden inverso de índice OAM y se descartan primero los sprites de índice más bajo que ya hubieran cabido, hasta ajustarse al límite.

**Errata de hardware:** los sprites situados exactamente en X=$100 cuentan incorrectamente para estos límites de línea sin importar si están realmente visibles en esa posición horizontal.

### Orden de dibujo entre sprites

Dentro de la propia capa de OBJ, el sprite con menor índice en OAM siempre se dibuja por encima del de mayor índice, independientemente del atributo de prioridad. El atributo de prioridad de cada sprite solo determina su posición relativa frente a las capas de fondo (BG), no el orden entre sprites.

## Notas / gotchas

- El límite de 32 sprites y el de 34 slivers por línea se evalúan de forma independiente y en direcciones opuestas de la tabla OAM (ascendente para el límite de sprites, descendente para el de slivers), lo que puede producir parpadeo (flicker) si hay demasiados sprites/tiles por línea.
- Los sprites en X=$100 activan indebidamente los contadores de límite por una errata conocida del hardware — hay que tenerlo en cuenta al depurar parpadeos inesperados.
- La CPU solo puede escribir en OAM de forma segura durante VBlank o forced blanking (ver `ppu-registers.md`, registros $2102-$2104).
- El orden de prioridad entre sprites (por índice OAM) es fijo; para cambiar qué sprite tapa a cuál hay que reordenar físicamente las entradas en OAM, no basta con cambiar el atributo de prioridad.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Tiles y formatos de bitplane](tiles.md)
- [Paletas y CGRAM](palettes.md)
- [Fondos y modos de BG](backgrounds.md)
