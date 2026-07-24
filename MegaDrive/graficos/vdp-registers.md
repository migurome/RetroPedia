---
titulo: Referencia de registros del VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, referencia, registros, dma, scroll, sprites]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/vdp-registers]
---

# Referencia de registros del VDP

## Resumen

Referencia completa de todos los registros del VDP útiles en una Mega Drive estándar (no incluye bits indocumentados). Cada registro se configura escribiendo un valor de 16 bits en el puerto de control siguiendo el formato descrito en `vdp-commands` (`$8000 OR (register << 8) OR value`). Esta es la tabla de referencia que hay que consultar al inicializar el VDP o al ajustar scroll, tamaño de tilemap, DMA, etc.

## Detalle técnico

### `$80xx`: mode set register #1

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `LCB` | `IE1` | `0` | `1` | `M3` | `0` |

- **`LCB`:** 1 para hacer blanking de la columna más a la izquierda (8px de ancho)
- **`IE1`:** 1 para activar la interrupción de hblank
- **`M3`:** 1 para congelar el contador HV

### `$81xx`: mode set register #2

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `DISP` | `IE0` | `M1` | `M2` | `1` | `0` | `0` |

- **`DISP`:** 1 para activar el renderizado
- **`IE0`:** 1 para activar la interrupción de vblank
- **`M1`:** 1 para permitir operaciones DMA
- **`M2`:** resolución vertical en tiles
  - `0`: 28 tiles (modo V28)
  - `1`: 30 tiles (modo V30)

Nota: el modo V30 solo funciona correctamente en sistemas PAL.

### `$82xx`: plane A table address

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `SA15` | `SA14` | `SA13` | `0` | `0` | `0` |

- **`SA15-13`:** dirección base de la tabla del plano A (dividida entre `$2000`)

### `$83xx`: window table address

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `WD15` | `WD14` | `WD13` | `WD12` | `WD11` | `0` |

- **`WD15-11`:** dirección base de la tabla window (dividida entre `$800`). En modo H40, `WD11` debe ser 0.

### `$84xx`: plane B table address

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `0` | `0` | `SB15` | `SB14` | `SB13` |

- **`SB15-13`:** dirección base de la tabla del plano B (dividida entre `$2000`)

### `$85xx`: sprite table address

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `AT15` | `AT14` | `AT13` | `AT12` | `AT11` | `AT10` | `AT9` |

- **`AT15-9`:** dirección base de la tabla de sprites (dividida entre `$200`). En modo H40, `AT9` debe ser 0.

### `$87xx`: background color

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `CPT1` | `CPT0` | `COL3` | `COL2` | `COL1` | `COL0` |

- **`CPT1-0`:** paleta del color de fondo
- **`COL3-0`:** índice del color de fondo

### `$8Axx`: hblank interrupt rate

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `HIT7` | `HIT6` | `HIT5` | `HIT4` | `HIT3` | `HIT2` | `HIT1` | `HIT0` |

- **`HIT7-0`:** cuántas líneas esperar entre interrupciones de hblank

### `$8Bxx`: mode set register #3

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `0` | `IE2` | `VSCR` | `HSCR` | `LSCR` |

- **`IE2`:** 1 para activar la interrupción externa
- **`VSCR`:** modo de scroll vertical
  - `0`: scroll completo
  - `1`: scroll cada dos tiles
- **`H/LSCR`:** modo de scroll horizontal
  - `00`: scroll completo
  - `01`: scroll cada ocho líneas, luego se repite
  - `10`: scroll cada tile
  - `11`: scroll cada línea

### `$8Cxx`: mode set register #4

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `RS0` | `0` | `0` | `0` | `S/TE` | `LSM1` | `LSM0` | `RS1` |

- **`RS0-1`:** resolución horizontal en tiles
  - `00`: 32 tiles (modo H32)
  - `11`: 40 tiles (modo H40)
- **`S/TE`:** 1 para activar shadow/highlight
- **`LSM1-0`:** modo entrelazado
  - `00`: sin entrelazado
  - `01`: modo 1 (entrelazado sin incremento de resolución)
  - `11`: modo 2 (entrelazado con mayor resolución)

### `$8Dxx`: hscroll table address

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `HS15` | `HS14` | `HS13` | `HS12` | `HS11` | `HS10` |

- **`HS15-10`:** dirección base de la tabla de hscroll (dividida entre `$400`)

### `$8Fxx`: autoincrement

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `INC7` | `INC6` | `INC5` | `INC4` | `INC3` | `INC2` | `INC1` | `INC0` |

- **`INC7-0`:** cantidad de autoincremento (en bytes)

### `$90xx`: tilemap size

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `VSZ1` | `VSZ0` | `0` | `0` | `HSZ1` | `HSZ0` |

- **`VSZ1-0`:** tamaño vertical (ver tabla siguiente)
- **`HSZ1-0`:** tamaño horizontal (ver tabla siguiente)

Tamaños de tilemap posibles ("Possible tilemap sizes"):

| `VSZ1-0` | `HSZ1-0` | Tamaño en tiles |
|---|---|---|
| `00` | `00` | 32×32 |
| `00` | `01` | 64×32 |
| `00` | `11` | 128×32 |
| `01` | `00` | 32×64 |
| `01` | `01` | 64×64 |
| `11` | `00` | 32×128 |

Otros tamaños de tilemap no funcionarán como se espera.

### `$91xx`: window X division

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `RIGT` | `0` | `0` | `WHP5` | `WHP4` | `WHP3` | `WHP2` | `WHP1` |

- **`RIGT`:** dónde mostrar la window
  - `0`: a la izquierda de la coordenada X de división
  - `1`: a la derecha de la coordenada X de división
- **`WHP5-1`:** coordenada X de la división (en pasos de 16px)

### `$92xx`: window Y division

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `DOWN` | `0` | `0` | `WVP4` | `WVP3` | `WVP2` | `WVP1` | `WVP0` |

- **`DOWN`:** dónde mostrar la window
  - `0`: por encima de la coordenada Y de división
  - `1`: por debajo de la coordenada Y de división
- **`WVP4-0`:** coordenada Y de la división (en pasos de 8px)

### `$93xx`, `$94xx`: DMA length

`$93xx`: DMA length (low)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `LG7` | `LG6` | `LG5` | `LG4` | `LG3` | `LG2` | `LG1` | `LG0` |

`$94xx`: DMA length (high)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `LG15` | `LG14` | `LG13` | `LG12` | `LG11` | `LG10` | `LG9` | `LG8` |

- **`LG15-0`:** longitud de la transferencia DMA

### `$95xx`, `$96xx`, `$97xx`: DMA source

`$95xx`: DMA source address (low)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `SA8` | `SA7` | `SA6` | `SA5` | `SA4` | `SA3` | `SA2` | `SA1` |

`$96xx`: DMA source address (middle)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `SA16` | `SA15` | `SA14` | `SA13` | `SA12` | `SA11` | `SA10` | `SA9` |

`$97xx`: DMA source address (high)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `DMD1` | `DMD0` | `SA22` | `SA21` | `SA20` | `SA19` | `SA18` | `SA17` |

- **`SA22-1`:** origen del DMA
- **`DMD1-0`:** tipo de operación DMA
  - `0*`: DMA transfer (`DMD0` se convierte en `SA23`)
  - `10`: VRAM fill
  - `11`: VRAM copy

## Notas / gotchas

- El modo V30 (30 tiles verticales, registro `$81xx` bit `M2`) solo funciona correctamente en sistemas PAL.
- En modo H40, `WD11` (registro `$83xx`) y `AT9` (registro `$85xx`) deben ser 0 obligatoriamente.
- Los tamaños de tilemap fuera de los seis combinaciones listadas en `$90xx` no funcionan como se espera.
- El tipo de operación DMA (`DMD1-0` en `$97xx`) reutiliza el bit `DMD0` como bit `SA23` de la dirección de origen cuando la operación es DMA transfer (valor `0*`).

## Referencias cruzadas

- (pendiente de enlazar)
