---
titulo: Registros MMIO del CPU (fuera del PPU)
plataforma: [SNES]
categoria: [memoria]
tags: [mmio, registros, joypad, interrupciones, matematicas]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/MMIO_registers]
---

# Registros MMIO del CPU (fuera del PPU)

## Resumen

Registros de entrada/salida mapeados en memoria que controlan funciones del propio 5A22 (el CPU de la SNES) y su lógica auxiliar: interrupciones (NMI/IRQ por posición de rastreo), comunicación con el APU, acceso a WRAM por dirección indexada, velocidad de acceso a ROM, lectura de mandos (tanto el modo "auto-read" moderno como el protocolo serie estilo NES), y las unidades de multiplicación/división por hardware. No incluye los registros del PPU (esos son un bloque aparte) ni el bloque de DMA/HDMA (ver `dma-registers`), aunque varios de estos registros interactúan directamente con ambos.

## Detalle técnico

### Interrupciones y estado

**`NMITIMEN` ($4200, escritura)**

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3-1 | Bit 0 |
|---|---|---|---|---|---|
| `N` | — | `V` | `H` | — | `J` |

- **`N` (bit 7):** activa la NMI de vertical-blank.
- **`VH` (bits 5-4):** modo de IRQ por temporizador H/V — `00` desactivado, `01` coincidencia de H, `10` coincidencia de V con H=0, `11` coincidencia de V y H.
- **`J` (bit 0):** activa la lectura automática de mandos (auto-read).
- Valor en power-on/reset: `$00`.

**`RDNMI` ($4210, lectura)**

| Bit 7 | Bit 6-4 | Bit 3-0 |
|---|---|---|
| `N` | — | `VVVV` |

- **`N` (bit 7):** flag de vertical-blank; se borra al leer el registro o al terminar el vblank.
- **`VVVV` (bits 3-0):** versión de CPU.
- Valor en power-on/reset: `RDNMI & $7F`.

**`TIMEUP` ($4211, lectura)**

| Bit 7 | Bits 6-0 |
|---|---|
| `T` | — |

- **`T` (bit 7):** flag de temporizador (H/V IRQ); se borra al leer.
- Valor en power-on/reset: `TIMEUP & $7F`.

**`HVBJOY` ($4212, lectura)**

| Bit 7 | Bit 6 | Bits 5-1 | Bit 0 |
|---|---|---|---|
| `V` | `H` | — | `J` |

- **`V` (bit 7):** flag de vertical-blank.
- **`H` (bit 6):** flag de horizontal-blank.
- **`J` (bit 0):** flag de "lectura automática de mandos en curso".

### Registros de temporizador H/V

**`HTIMEL` ($4207, escritura) / `HTIMEH` ($4208, escritura)**

Objetivo del contador H combinado en 9 bits (rango 0-339). Valor en power-on: `$1FF`.

**`VTIMEL` ($4209, escritura) / `VTIMEH` ($420A, escritura)**

Objetivo del contador V combinado en 9 bits. Valor en power-on: `$1FF`. Máximo: 261 (NTSC) o 311 (PAL).

### Comunicación con el APU

**`APUIO0`-`APUIO3` ($2140-$2143, lectura/escritura)**

Cuatro puertos de datos de 8 bits, bidireccionales, para comunicación con el SPC700 (APU). Se reflejan (mirror) en todo el rango `$2140`-`$217F`.

### Acceso a WRAM por dirección indexada

**`WMDATA` ($2180, lectura/escritura)**

Byte de datos de la WRAM del sistema (S-WRAM); cada acceso incrementa automáticamente `WMADD`.

**`WMADDL` ($2181, escritura) / `WMADDM` ($2182, escritura) / `WMADDH` ($2183, escritura)**

Dirección de 24 bits dentro de la S-WRAM usada por `WMDATA` (bit alto de `WMADDH` es el único bit significativo, el resto son ceros).

### Velocidad de acceso a ROM

**`MEMSEL` ($420D, escritura)**

| Bit 7-1 | Bit 0 |
|---|---|
| — | `F` |

- **`F` (bit 0):** activa FastROM (6 ciclos de acceso si está activo, 8 ciclos si no).
- Valor en power-on: `$00`.

### Interfaz de mando estilo NES (protocolo serie clásico)

**`JOYOUT` ($4016, escritura)**

- Bit 0 (`OUT0`): señal de latch para el mando estándar.

**`JOYSER0` ($4016, lectura)**

- Bits 1-0: datos serie del puerto de mando 1. Leer el registro genera el pulso de reloj (clock) del puerto 1.

**`JOYSER1` ($4017, lectura)**

- Bits 1-0: datos serie del puerto de mando 2.
- Bits 4-2: siempre a 1 (líneas atadas a tierra/pull-up). Leer genera el pulso de reloj del puerto 2.

### Interfaz de E/S general de los puertos de mando

**`WRIO` ($4201, escritura)**

| Bit 7 | Bit 6 | Bits 5-0 |
|---|---|---|
| `2` | `1` | `DDDDDD` |

- Bit 7: línea de E/S del puerto de mando 2 (también usada por el lápiz óptico del PPU, `/EXTLATCH`).
- Bit 6: línea de E/S del puerto de mando 1.
- Bits 5-0: E/S del CPU, no conectadas.
- Valor en power-on: `$FF`.

**`RDIO` ($4213, lectura)**

Misma disposición de bits que `WRIO`, pero en lectura: bit 7 = entrada del puerto 2, bit 6 = entrada del puerto 1, bits 5-0 = entrada de E/S del CPU (no conectadas).

### Resultados de la lectura automática de mandos (16 bits por mando)

**`JOY1L`/`JOY1H` ($4218/$4219, lectura):** primer resultado de auto-read del puerto 1 (16 bits, little-endian).

**`JOY2L`/`JOY2H` ($421A/$421B, lectura):** primer resultado de auto-read del puerto 2.

**`JOY3L`/`JOY3H` ($421C/$421D, lectura):** segundo resultado de auto-read del puerto 1 (usado por accesorios como el multitap).

**`JOY4L`/`JOY4H` ($421E/$421F, lectura):** segundo resultado de auto-read del puerto 2.

### Registros de matemáticas: multiplicación

**`WRMPYA` ($4202, escritura)**

Multiplicando A de 8 bits sin signo. Valor en power-on: `$FF`.

**`WRMPYB` ($4203, escritura)**

Multiplicando B de 8 bits sin signo; escribirlo dispara la multiplicación (tarda 8 ciclos internos en completarse).

### Registros de matemáticas: división

**`WRDIVL` ($4204, escritura) / `WRDIVH` ($4205, escritura)**

Dividendo de 16 bits sin signo. Valor en power-on: `$FFFF`.

**`WRDIVB` ($4206, escritura)**

Divisor de 8 bits sin signo; escribirlo dispara la división (tarda 16 ciclos internos en completarse).

### Resultados de las operaciones matemáticas

**`RDDIVL`/`RDDIVH` ($4214/$4215, lectura):** cociente de 16 bits (resultado de la división).

**`RDMPYL`/`RDMPYH` ($4216/$4217, lectura):** producto de 16 bits (resultado de la multiplicación) **o** resto de la división — el mismo par de registros se reutiliza para ambas operaciones matemáticas.

### Registros de DMA (referencia cruzada)

**`MDMAEN` ($420B, escritura)** y **`HDMAEN` ($420C, escritura)** activan, respectivamente, el DMA general y el HDMA por canal. Su detalle completo (bits por canal, timing, restricciones) está en la entrada `dma-registers`; se listan aquí solo porque comparten el mismo bloque de direcciones `$42xx` que el resto de registros de esta página.

## Notas / gotchas

- La lectura automática de mandos (`auto-read`, activada por el bit `J` de `NMITIMEN`) empieza entre H=32,5 y H=95,5 de la primera línea de vertical-blank, y dura 4224 ciclos de reloj maestro — durante ese intervalo `HVBJOY` bit 0 indica que sigue en curso; no hay que leer `JOY1`-`JOY4` mientras ese flag esté activo.
- El campo de versión de CPU en `RDNMI` (bits 3-0) deja de incrementarse a partir de la versión 2 en todas las revisiones conocidas del 5A22 (A, B, N-A) — no sirve para distinguir revisiones más allá de esa.
- `RDMPYL`/`RDMPYH` cambian de significado según cuál fue la última operación disparada (multiplicación → producto; división → resto), no hay dos pares de registros separados.
- El protocolo serie clásico de mando (`JOYOUT`/`JOYSER0`/`JOYSER1`, direcciones heredadas de NES en `$4016`/`$4017`) coexiste con el sistema de auto-read moderno; la mayoría de juegos comerciales usa auto-read y reserva el modo serie manual para lectores especiales o accesorios que no funcionan bien con auto-read (por ejemplo, algunos multitaps).
- Los registros de DMA/HDMA completos no se repiten aquí en detalle para evitar duplicidad; ver la entrada dedicada `dma-registers`.

## Referencias cruzadas

- (pendiente de enlazar)
