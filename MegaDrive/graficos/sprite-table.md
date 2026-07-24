---
titulo: Referencia de la tabla de sprites
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, referencia, sprites, vram, tiles]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/sprite-table]
---

# Referencia de la tabla de sprites

## Resumen

Documenta el formato exacto de la Sprite Attribute Table del VDP: la estructura de 8 bytes por entrada, el sistema de coordenadas, el campo de tile/flags, el campo de tamaño y la lista enlazada (`link`) que determina qué sprites se renderizan. Es la referencia imprescindible para construir o depurar cualquier código que escriba sprites directamente en VRAM.

## Detalle técnico

### Formato de la tabla de sprites

La ubicación de la tabla de sprites en VRAM la determina el registro del VDP `$85xx` (ver `vdp-registers`).

La tabla de sprites consiste en 80 o 64 entradas (dependiendo de si la resolución horizontal es H40 o H32, respectivamente). Cada entrada ocupa 8 bytes ("Sprite table entry"):

| Offset | Size | Description |
|---|---|---|
| +0 | 2 bytes | Y coordinate |
| +2 | 1 byte | Sprite size |
| +3 | 1 byte | Sprite link |
| +4 | 2 bytes | Tile ID and flags |
| +6 | 2 bytes | X coordinate |

### Coordenadas X e Y

Estos dos campos indican la posición de la esquina superior izquierda del sprite en pantalla. El origen está en -128;-128, es decir, hay que sumar 128 a ambas coordenadas para que coincidan con su posición en pantalla.

Estos campos son de solo 9 bits (el resto de bits se ignoran), así que las coordenadas dan la vuelta cada 512 píxeles. Esto solo es relevante si no se recortan los sprites en los bordes de la pantalla.

### Tile ID y flags

Este es el campo que indica qué gráfico usa el sprite y qué efectos se le aplican (flip, paleta, etc.) ("Sprite tile ID and flags"):

| | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|---|
| **1st byte** | `PRI` | `CP1` | `CP0` | `VF` | `HF` | `SN10` | `SN9` | `SN8` |
| **2nd byte** | `SN7` | `SN6` | `SN5` | `SN4` | `SN3` | `SN2` | `SN1` | `SN0` |

- **`PRI`:** 1 para prioridad alta, 0 para prioridad baja
- **`CP1-0`:** qué paleta usar
- **`VF`:** 1 para voltear verticalmente, 0 para no
- **`HF`:** 1 para voltear horizontalmente, 0 para no
- **`SN10-0`:** primer tile ID

Los sprites usan IDs de tile consecutivos, es decir, si el sprite es de 3×2 tiles y el primer tile ID es 100, usará los tiles 100, 101, 102, 103, 104 y 105.

Las banderas de flip se aplican al sprite completo, no a cada tile individual. Las coordenadas siguen refiriéndose a la esquina superior izquierda del sprite ya volteado, así que habrá que ajustarlas si no era el efecto deseado.

Para facilitar las cosas, se pueden combinar (OR) estas banderas con el tile ID para aplicar los efectos respectivos (se puede combinar como máximo una de cada grupo):

```asm
NOFLIP:  equ $0000  ; Don't flip
HFLIP:   equ $0800  ; Flip horizontally
VFLIP:   equ $1000  ; Flip vertically
HVFLIP:  equ $1800  ; Flip both ways

PAL0:    equ $0000  ; 1st palette
PAL1:    equ $2000  ; 2nd palette
PAL2:    equ $4000  ; 3rd palette
PAL3:    equ $6000  ; 4th palette

LOPRI:   equ $0000  ; Low priority
HIPRI:   equ $8000  ; High priority
```

### Tamaño del sprite

Este campo indica el tamaño del sprite (en tiles). Tiene el siguiente formato ("Sprite size field"):

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `0` | `HS1` | `HS0` | `VS1` | `VS0` |

- **`HS1-0`:** anchura del sprite
- **`VS1-0`:** altura del sprite

Tanto anchura como altura se miden en tiles _menos uno_ (es decir, `00` es 1 tile y `11` es 4 tiles), se permite cualquier combinación. Los bits restantes se ignoran. Se pueden usar las siguientes constantes:

```asm
; Name is SPR_*x# where * is the
; width and # is the height

SPR_1x1:  equ %0000  ; 1x1 tiles
SPR_2x1:  equ %0100  ; 2x1 tiles
SPR_3x1:  equ %1000  ; 3x1 tiles
SPR_4x1:  equ %1100  ; 4x1 tiles
SPR_1x2:  equ %0001  ; 1x2 tiles
SPR_2x2:  equ %0101  ; 2x2 tiles
SPR_3x2:  equ %1001  ; 3x2 tiles
SPR_4x2:  equ %1101  ; 4x2 tiles
SPR_1x3:  equ %0010  ; 1x3 tiles
SPR_2x3:  equ %0110  ; 2x3 tiles
SPR_3x3:  equ %1010  ; 3x3 tiles
SPR_4x3:  equ %1110  ; 4x3 tiles
SPR_1x4:  equ %0011  ; 1x4 tiles
SPR_2x4:  equ %0111  ; 2x4 tiles
SPR_3x4:  equ %1011  ; 3x4 tiles
SPR_4x4:  equ %1111  ; 4x4 tiles
```

### Sprite link

La tabla de sprites no es lineal, sino una _lista enlazada_. Esto significa que cada sprite indica cuál sigue en la lista. Este campo indica qué sprite es el siguiente (1 = segunda entrada, 2 = tercera entrada, etc.). Si este campo es 0, indica el final de la lista.

La lista siempre empieza con el sprite 0. Se usan los bits 6-0 (el bit 7 se ignora).

### Modo 128k VRAM

En modo 128k VRAM hay espacio para 4096 tiles únicos, pero solo se pueden direccionar 2048 a la vez. El VDP resuelve esto dividiendo la VRAM en dos "bancos" de 2048 tiles. El registro del VDP `$86xx` determina cuál de los dos bancos usan todos los sprites:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `AP16` | `0` | `0` | `0` | `0` | `0` |

`AP16` es 0 para usar los 64KB inferiores de VRAM y 1 para usar los 64KB superiores de VRAM.

La tabla de sprites en sí no se ve afectada por este registro, sino que la dirección de la tabla de sprites simplemente gana un bit extra. Esto significa que se puede colocar la tabla en el banco opuesto si se desea.

## Notas / gotchas

- El origen de coordenadas es -128;-128: hay que sumar 128 a X e Y para obtener la posición real en pantalla.
- Los campos de coordenada son de 9 bits y dan la vuelta cada 512 píxeles; esto solo importa si no se recortan los sprites en los bordes de pantalla.
- Las banderas de flip (`HF`/`VF`) afectan al sprite completo, no tile por tile, y las coordenadas siguen refiriéndose a la esquina superior izquierda ya volteada.
- El número de entradas disponibles depende de la resolución horizontal: 80 en H40, 64 en H32.
- La lista de renderizado es una lista enlazada que siempre empieza en el sprite 0; un valor de `link` igual a 0 termina la lista.
- En modo H40, `AT9` del registro `$85xx` debe ser 0 (ver `vdp-registers`).
- En modo 128k VRAM, el bit `AP16` del registro `$86xx` selecciona el banco de tiles (bajo/alto) usado por todos los sprites; la propia dirección de la tabla de sprites no se ve afectada, solo gana un bit extra.

## Referencias cruzadas

- (pendiente de enlazar)
