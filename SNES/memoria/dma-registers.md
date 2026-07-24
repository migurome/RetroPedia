---
titulo: Registros de DMA y HDMA
plataforma: [SNES]
categoria: [memoria]
tags: [dma, hdma, registros, mmio, transferencia]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/DMA_registers]
---

# Registros de DMA y HDMA

## Resumen

La SNES tiene una unidad de DMA con 8 canales independientes que permite copiar datos a alta velocidad entre el bus A (ROM/RAM del cartucho, WRAM de la consola) y el bus B (registros de hardware del PPU/APU en el rango `$2100`-`$21FF`). Cada canal comparte el mismo bloque de 16 registros, repetido en `$4300`-`$437F` (uno por canal, `n` = 0-7). Además del DMA "general" (a petición, bloquea la CPU hasta terminar), los mismos canales pueden operar en modo HDMA (Horizontal DMA), que transfiere automáticamente una pequeña cantidad de datos en cada línea de rastreo durante el resto del frame — la base de casi todos los efectos de "raster" en SNES (scroll por línea, degradados, ventanas dinámicas, Mode 7 por línea, etc.).

## Detalle técnico

### Registros maestros de activación

**`MDMAEN` ($420B, escritura) — activación de DMA general**

- Bits 7-0: un flag por canal (0-7); al escribir, se inicia inmediatamente la transferencia de los canales marcados y la CPU se detiene hasta que termina.
- Valor por defecto: `$00`.

**`HDMAEN` ($420C, escritura) — activación de HDMA**

- Bits 7-0: un flag por canal (0-7) que activa HDMA para ese canal.
- Solo debería activarse durante el vertical-blank con la pantalla activa.
- Valor por defecto: `$00`.

### Registros por canal (`$43n0`-`$43nA`, siendo `n` el número de canal 0-7)

| Offset | Registro | Dirección | Bits | Acceso | Función |
|---|---|---|---|---|---|
| +0 | `DMAPn` | `$43n0` | `DI.AAPPP` | RW8 | Dirección, modo indirecto, modo de dirección, patrón de transferencia |
| +1 | `BBADn` | `$43n1` | `AAAAAAAA` | RW8 | Dirección del registro del bus B (`$2100`-`$21FF`) |
| +2 | `A1TnL` | `$43n2` | `LLLLLLLL` | RW8 | Byte bajo de la dirección origen (DMA) / tabla HDMA |
| +3 | `A1TnH` | `$43n3` | `HHHHHHHH` | RW8 | Byte medio de la dirección origen (DMA) / tabla HDMA |
| +4 | `A1Bn` | `$43n4` | `BBBBBBBB` | RW8 | Banco de origen (DMA) / banco de la tabla HDMA |
| +5 | `DASnL` | `$43n5` | `LLLLLLLL` | RW8 | Byte bajo del contador de bytes (DMA) / dirección indirecta HDMA |
| +6 | `DASnH` | `$43n6` | `HHHHHHHH` | RW8 | Byte alto del contador de bytes (DMA) / dirección indirecta HDMA |
| +7 | `DASBn` | `$43n7` | `BBBBBBBB` | RW8 | Banco indirecto de HDMA (hay que fijarlo a mano) |
| +8 | `A2AnL` | `$43n8` | `LLLLLLLL` | RW8 | Byte bajo de la dirección actual dentro de la tabla HDMA |
| +9 | `A2AnH` | `$43n9` | `HHHHHHHH` | RW8 | Byte alto de la dirección actual dentro de la tabla HDMA |
| +A | `NLTRn` | `$43nA` | `RLLLLLLL` | RW8 | Flag de repetición (R) y contador de líneas restantes |
| +B | (sin uso)n | `$43nB` | `DDDDDDDD` | RW8 | Byte libre compartido, sin función de DMA |
| +F | (sin uso)n | `$43nF` | `DDDDDDDD` | RW8 | Byte libre compartido (dirección alternativa) |

### `DMAPn` ($43n0) — parámetros de transferencia

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `D` | `I` | — | `A` | `A` | `P` | `P` | `P` |

- **`PPP` (bits 2-0):** patrón de transferencia
  - `0`: 1 byte, B+0 (WRAM, Mode 7)
  - `1`: 2 bytes, B+0, B+1 (VRAM)
  - `2`: 2 bytes, B+0, B+0 (OAM, CGRAM)
  - `3`: 4 bytes, B+0, B+0, B+1, B+1 (scroll, Mode 7)
  - `4`: 4 bytes, B+0, B+1, B+2, B+3 (ventanas)
  - `5`-`7`: variantes indocumentadas
- **`AA` (bits 4-3):** modo de ajuste de dirección (solo DMA)
  - `0`: incrementar la dirección del bus A tras cada copia
  - `1` / `3`: dirección fija
  - `2`: decrementar la dirección del bus A tras cada copia
- **`I` (bit 6):** modo indirecto (solo HDMA; `0` = directo, `1` = indirecto)
- **`D` (bit 7):** dirección de la transferencia (`0` = A→B, `1` = B→A)

Valor por defecto: `$FF`.

### `BBADn` ($43n1) — destino en el bus B

Selecciona el registro de hardware dentro de `$2100`-`$21FF`; el byte completo son los 8 bits bajos de la dirección (los bits altos siempre son `$21`). Se recomienda evitar el valor `$00` y usar `$FF` con patrón 1 si se necesita controlar `INIDISP`, para no escribir por accidente sobre él.

Valor por defecto: `$FF`.

### Registros `A1Tn` (`A1TnL`/`A1TnH`/`A1Bn`, `$43n2`-`$43n4`) — dirección de origen / tabla

- **En modo DMA:** dirección de origen de 24 bits en el bus A, en little-endian. Los 16 bits bajos se incrementan/decrementan durante la transferencia; el byte de banco (`A1Bn`) permanece fijo — la transferencia **no puede cruzar un límite de banco**. Tampoco puede acceder a los rangos de registros de hardware (`$2100`-`$21FF`, `$4000`-`$41FF`, `$4200`-`$421F`, `$4300`-`$437F`).
- **En modo HDMA:** dirección de 24 bits donde empieza la tabla HDMA. Se copia a `A2An` al iniciarse el HDMA (en la línea de rastreo 0).

Valor por defecto: `$FFFFFF`.

### Registros `DASn` (`DASnL`/`DASnH`, `$43n5`-`$43n6`) — contador de bytes / dirección indirecta

- **En modo DMA:** contador de 16 bits (little-endian) de bytes a transferir; el valor `0` significa 65.536 bytes. El contador es independiente del patrón de transferencia elegido. Al terminar la transferencia queda a `$0000`, así que hay que volver a escribirlo antes de cada nueva transferencia.
- **En modo HDMA indirecto:** 16 bits bajos de la dirección indirecta, que se actualizan automáticamente desde la tabla. El banco (`DASBn`) hay que fijarlo a mano y permanece constante.

Valor por defecto: `$FFFF`.

### `DASBn` ($43n7) — banco indirecto de HDMA

Byte de banco de 8 bits usado en modo HDMA indirecto. Debe configurarse manualmente por el programa; los 16 bits bajos se actualizan automáticamente desde la tabla. El banco permanece fijo — el HDMA tampoco puede cruzar un límite de banco.

Valor por defecto: `$FF`.

### Registros `A2An` (`A2AnL`/`A2AnH`, `$43n8`-`$43n9`) — dirección actual dentro de la tabla HDMA

Dirección de 16 bits (little-endian) dentro de la tabla HDMA; el byte de banco proviene de `A1Bn`. Se inicializa automáticamente desde `A1Tn` en la línea de rastreo 0. Si el HDMA se activa a mitad de frame, hay que fijarlo manualmente.

Valor por defecto: `$FFFF`.

### `NLTRn` ($43nA) — contador de líneas de HDMA

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `R` | `L` | `L` | `L` | `L` | `L` | `L` | `L` |

- **`LLLLLLL` (bits 6-0):** líneas de rastreo restantes; se decrementa en cada línea hasta llegar a cero.
- **`R` (bit 7):** flag de repetición (se carga desde la tabla).

Se carga automáticamente desde la tabla HDMA; se puede fijar manualmente para activar el HDMA a mitad de frame.

Valor por defecto: `$FF`.

### Formato de la tabla HDMA

**Entradas en modo directo:**

- 1 byte: contador de líneas + flag de repetición.
- N bytes de datos (la cantidad depende del patrón de transferencia elegido).

**Entradas en modo indirecto:**

- 1 byte: contador de líneas + flag de repetición.
- 2 bytes: puntero de 16 bits a los datos.

**Valores del byte de contador de líneas:**

- `$00`: detiene el procesado de HDMA para el resto del frame.
- `$01`-`$80`: una única escritura seguida de una espera de X líneas de rastreo.
- `$81`-`$FF`: escribe en cada línea de rastreo durante (X-`$80`) líneas (modo repetición).

La primera entrada de la tabla se ejecuta al final de la línea de rastreo 0 (oculta dentro del blanking).

## Notas / gotchas

- El DMA no puede copiar dentro de la propia WRAM (de WRAM a WRAM); para eso hay que usar las instrucciones `MVN`/`MVP` del 65C816.
- En algunas revisiones de hardware de SNES, el DMA puede colgarse si su finalización coincide exactamente con el inicio del HDMA; por seguridad, conviene restringir el DMA general al vertical-blank o desactivar el HDMA mientras se ejecuta.
- Todo el acceso a estos registros ocurre a 3,58 MHz; los registros de canales DMA que no se estén usando pueden aprovecharse como RAM rápida improvisada (salvo los bytes "sin uso", que no sirven como origen de DMA).
- La dirección del bus A se envuelve (wrap) dentro del mismo banco; el byte de banco nunca se incrementa automáticamente, ni en DMA ni en HDMA.
- Ver `dma-ejemplos` para casos de uso concretos (copiar OAM, rellenar VRAM, actualizar tilemaps por columnas).

## Referencias cruzadas

- (pendiente de enlazar)
