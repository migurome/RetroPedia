---
titulo: Big Evil Corporation — primer "Hello World" en pantalla (serie Tanglewood)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bigevilcorporation, tanglewood, vdp, cram, vram, tiles, hello-world]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/03/23/sega-megadrive-4-hello-world/]
---

# Big Evil Corporation — primer "Hello World" en pantalla (serie Tanglewood)

## Resumen

Cuarta entrada de la serie de Matt (Big Evil Corporation, autor de Tanglewood): partiendo de un VDP ya inicializado (ver `bec-vdp-init.md`), este artículo construye el primer programa que muestra texto en pantalla, cubriendo paso a paso el formato de color de 9 bits, la definición manual de patrones de carácter, el formato del comando de dirección del puerto de control del VDP, y la escritura de tiles en el Plano A.

## Detalle técnico

### Paletas de color (CRAM)

El VDP usa 9 bits por color (3 bits por canal RGB), lo que da 512 colores posibles en total. Las paletas viven en CRAM: hasta 4 paletas simultáneas de 16 colores cada una (64 entradas en total). La entrada 0 de cada paleta determina el color de fondo/transparencia.

Ejemplo de paleta tal como aparece en el artículo:

```
Palette:
   dc.w 0x0000 ; Colour 0 - Transparent
   dc.w 0x000E ; Colour 1 - Red
   dc.w 0x00E0 ; Colour 2 - Green
   dc.w 0x0E00 ; Colour 3 - Blue
   dc.w 0x0EEE ; Colour 5 - White
   dc.w 0x00EE ; Colour 6 - Yellow
```

### Patrones gráficos (tiles) definidos a mano

Cada tile mide 8×8 píxeles, y cada píxel ocupa un nibble (4 bits) que indica el índice de color dentro de la paleta activa. Un tile completo ocupa 8 longwords (32 bytes). Ejemplo, el carácter "H":

```
CharacterH:
   dc.l 0x11000110
   dc.l 0x11000110
   dc.l 0x11000110
   dc.l 0x11111110
   dc.l 0x11000110
   dc.l 0x11000110
   dc.l 0x11000110
   dc.l 0x00000000
```

Donde `1` referencia el color 1 de la paleta y `0` es transparente (color 0).

### Formato del comando de dirección del puerto de control

Para leer o escribir en VRAM/CRAM/VSRAM hay que enviar primero un comando de 32 bits al puerto de control que combina la dirección de destino con el tipo de operación, según esta plantilla de bits:

```
BBAA AAAA AAAA AAAA 0000 0000 BBBB 00AA
```

Donde las `A` son bits de dirección y las `B` codifican la operación. Los 6 bits de operación se reparten así dentro del longword:

```
10-- ---- ---- ---- ---- ---- 5432 ----
```

Códigos de operación:

```
000000 – VRAM Read
100000 – VRAM Write
000100 – CRAM Read
110000 – CRAM Write
001000 – VSRAM Read
101000 – VSRAM Write
```

**Ejemplo de cálculo:** para escribir en VRAM la dirección `0xC000`, se toma su representación binaria, se reordena según la plantilla y se combinan los bits de operación VRAM Write; el resultado es el comando `0x40000003`, es decir:

```
move.l #0x40000003, 0x00C00004
```

### Autoincremento

```
move.w #0x8F02, 0x00C00004 ; Fija el autoincremento a 2 bytes
```

`0x8F` selecciona el registro 15 (autoincremento) del VDP; `0x02` es el valor: incrementar 2 bytes tras cada acceso a datos.

### Carga de la paleta en CRAM

```
move.l #0xC0000003, 0x00C00004 ; Comando: escribir en CRAM, dirección 0x0000
lea Palette, a0                 ; Dirección de la paleta
move.l #0x07, d0                ; 8 longwords - 1 (contador dbra)

@Loop:
move.l (a0)+, 0x00C00000
dbra d0, @Loop
```

El comando `0xC0000003` combina la operación CRAM Write (`110000`) con la dirección `0x0000`.

### Color de fondo (registro 7 del VDP)

```
move.w #0x8708, 0x00C00004
```

`0x87` selecciona el registro 7; el byte de datos `0x08` empaqueta color 8 (nibble bajo) de la paleta 0 (nibble alto) — en el ejemplo del autor esto produce una pantalla de fondo rosa.

### Carga de los patrones de carácter en VRAM

Los caracteres se cargan a partir de la dirección VRAM `0x0020`, dejando libre el primer patrón (índice 0) para usarlo como espacio en blanco:

```
move.l #0x40200000, 0x00C00004 ; Comando: escribir en VRAM, dirección 0x0020
lea Characters, a0
move.l #0x37, d0                ; 56 longwords - 1 (7 caracteres x 8 longwords)

@Loop:
move.l (a0)+, 0x00C00000
dbra d0, @Loop
```

### Mapeo de patrones al Plano A (nametable)

El Plano A empieza en la dirección VRAM `0xC000`. Cada entrada de tile es una word con el formato:

```
ABBC DEEE EEEE EEEE
```

- `A` — prioridad (plano alto/bajo)
- `BB` — índice de paleta (0-3)
- `C` — flip horizontal
- `D` — flip vertical
- `EEEE EEEE EEEE` — ID del patrón (pattern ID)

Ejemplo, escribir el patrón 1 (la "H") en el primer tile del Plano A:

```
move.l #0x40000003, 0x00C00004 ; Comando: Plano A, dirección 0xC000
move.w #0x0001, 0x00C00000      ; Pattern ID 1, paleta 0, sin flip
```

### Secuencia completa "Hello World"

```
move.l #0x40000003, 0x00C00004

; Pattern IDs: H=1, E=2, L=3, O=4, W=5, R=6, D=7
move.w #0x0001, 0x00C00000  ; H
move.w #0x0002, 0x00C00000  ; E
move.w #0x0003, 0x00C00000  ; L
move.w #0x0003, 0x00C00000  ; L
move.w #0x0004, 0x00C00000  ; O
move.w #0x0000, 0x00C00000  ; (espacio)
move.w #0x0005, 0x00C00000  ; W
move.w #0x0004, 0x00C00000  ; O
move.w #0x0006, 0x00C00000  ; R
move.w #0x0003, 0x00C00000  ; L
move.w #0x0007, 0x00C00000  ; D
```

### Tabla resumen de direcciones usadas en este artículo

| Elemento | Dirección |
|---|---|
| Puerto de control del VDP | `0x00C00004` |
| Puerto de datos del VDP | `0x00C00000` |
| Plano A (VRAM) | `0xC000` |
| CRAM paleta 0 | `0x0000` |

## Notas / gotchas

- El propio autor reconoce que esta primera implementación es rudimentaria y admite que la "plantilla de bits" del comando del VDP resulta confusa al principio; en artículos posteriores de la serie construye subrutinas más robustas y reutilizables (ver `bec-fonts-text.md`) en vez de escribir cada tile a mano como aquí.
- Escribir cada letra "a mano" con una instrucción `move.w` por carácter (como en la secuencia final) es deliberadamente la forma más tosca posible de mostrar texto — sirve como demostración mínima, no como técnica recomendable para texto dinámico.
- El artículo menciona el uso de documentación oficial (sega2f) y herramientas JavaScript de terceros para calcular manualmente los comandos del VDP mientras no se domina la fórmula de memoria.

## Referencias cruzadas

- [Referencia de comandos del VDP](vdp-commands.md) — fuente plutiedev.com; documenta el mismo formato de comando de dirección del puerto de control de forma más sistemática.
- [Tiles y paletas](tiles-and-palettes.md)
- [Escritura en memoria de vídeo (VRAM/CRAM/VSRAM)](writing-video.md)
- [Big Evil Corporation — inicialización de hardware al arranque](bec-vdp-init.md) (artículo anterior de la serie)
- [Big Evil Corporation — fuentes y renderizado de texto](bec-fonts-text.md) (artículo siguiente de la serie, sustituye este enfoque manual por subrutinas reutilizables)
