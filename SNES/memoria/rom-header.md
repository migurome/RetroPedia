---
titulo: Cabecera de la ROM (SNES)
plataforma: [SNES]
categoria: [memoria]
tags: [rom-header, checksum, region, chipset, mapa-memoria]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/ROM_header]
---

# Cabecera de la ROM (SNES)

## Resumen

Bloque de metadatos de 32 bytes (más una extensión opcional de 16 bytes) que toda ROM de SNES incluye para describir el título del juego, el modo de mapeo, la configuración de hardware/coprocesador, el tamaño de ROM/RAM, la región y un checksum de verificación. Su posición física dentro del archivo cambia según el modo de mapeo (LoROM/HiROM/ExHiROM), pero siempre queda proyectada en la misma dirección del espacio de memoria del CPU.

## Detalle técnico

### Ubicación de la cabecera según el modo de mapeo

| Modo de mapeo | Offset en el archivo de ROM |
|---|---|
| LoROM | `$007Fxx` |
| HiROM | `$00FFxx` |
| ExHiROM | `$40FFxx` |

En el espacio de direcciones del CPU, la cabecera principal siempre cae en `$00FFC0`-`$00FFDF`, con una cabecera expandida opcional en `$00FFB0`-`$00FFBF`.

### Estructura de la cabecera principal (`$FFC0`-`$FFDF`)

| Dirección | Tamaño | Campo | Detalle |
|---|---|---|---|
| `$FFC0` | 21 bytes | Título del cartucho | ASCII `$20`-`$7E`; relleno con espacios |
| `$FFD5` | 1 byte | Velocidad de ROM / modo de mapeo | `001smmmm` (bit 7 = velocidad; bits 3-0 = modo de mapeo) |
| `$FFD6` | 1 byte | Chipset | Configuración de hardware/coprocesador |
| `$FFD7` | 1 byte | Tamaño de ROM | `1 << N` kilobytes |
| `$FFD8` | 1 byte | Tamaño de RAM | `1 << N` kilobytes |
| `$FFD9` | 1 byte | Código de país | Identificador de región/idioma |
| `$FFDA` | 1 byte | ID de desarrollador | Código de publisher |
| `$FFDB` | 1 byte | Versión de ROM | `0` = primera versión |
| `$FFDC` | 2 bytes | Complemento del checksum | `checksum XOR $FFFF` |
| `$FFDE` | 2 bytes | Checksum | Suma de 16 bits de toda la ROM |
| `$FFE0` | 32 bytes | Vectores de interrupción | Vectores del CPU |

### Valores del modo de mapeo (`$FFD5`, bits 3-0)

- `$0`: LoROM
- `$1`: HiROM
- `$2`: S-DD1
- `$3`: SA-1
- `$5`: ExHiROM
- `$A`: SPC7110

### Valores de chipset (`$FFD6`)

**Sin coprocesador:**

- `$00`: solo ROM
- `$01`: ROM + RAM
- `$02`: ROM + RAM + batería

**Con coprocesador** (el nibble alto indica el tipo):

- `$0x`: DSP (DSP-1, 2, 3, 4)
- `$1x`: GSU (Super FX)
- `$2x`: OBC1
- `$3x`: SA-1
- `$4x`: S-DD1
- `$5x`: S-RTC
- `$Ex`: otro (Super Game Boy/Satellaview)
- `$Fx`: personalizado (ver `$FFBF`)

El nibble bajo indica la configuración de memoria adicional:

- `x3`: ROM + coprocesador
- `x4`: ROM + coprocesador + RAM
- `x5`: ROM + coprocesador + RAM + batería
- `x6`: ROM + coprocesador + batería

### Códigos de país (`$FFD9`)

| Código | Región | Letra de juego |
|---|---|---|
| `$00` | Japón | J |
| `$01` | Norteamérica | E |
| `$02` | Europa | P |
| `$03` | Escandinavia | W |
| `$04` | Finlandia | — |
| `$05` | Dinamarca | — |
| `$06` | Francia | F |
| `$07` | Holandés | H |
| `$08` | Español | S |
| `$09` | Alemán | D |
| `$0A` | Italiano | I |
| `$0B` | Chino | C |
| `$0C` | Indonesia | — |
| `$0D` | Corea del Sur | K |
| `$0E` | Común | A |
| `$0F` | Canadá | N |
| `$10` | Brasil/Gateway | B/G |
| `$11` | Australia | U |
| `$12`-`$14` | Otras variantes | X/Y/Z |

### Cabecera expandida (`$FFB0`-`$FFBF`)

Se considera activa cuando el ID de desarrollador (`$FFDA`) vale `$33`.

| Dirección | Tamaño | Campo | Detalle |
|---|---|---|---|
| `$FFB0` | 2 bytes | Código de fabricante | ASCII en mayúsculas |
| `$FFB2` | 4 bytes | Código de juego | ASCII en mayúsculas |
| `$FFB6` | 6 bytes | Reservado | Debe ser cero |
| `$FFBC` | 1 byte | Tamaño de flash de expansión | `1 << N` kilobytes |
| `$FFBD` | 1 byte | Tamaño de RAM de expansión | `1 << N` kilobytes |
| `$FFBE` | 1 byte | Versión especial | Normalmente cero |
| `$FFBF` | 1 byte | Subtipo de chipset | Usado si `$FFD6 = $Fx` |

### Valores de RAM de expansión (`$FFBD`)

| Valor | Tamaño |
|---|---|
| `$00` | Ninguna |
| `$01` | 16 kbit |
| `$02` | Indefinido (probablemente 32 kbit) |
| `$03` | 64 kbit |
| `$04` | Indefinido (probablemente 64 kbit) |
| `$05` | 256 kbit |
| `$06` | 512 kbit |
| `$07` | 1 mbit |

### Subtipos de chipset personalizado (`$FFBF` cuando `$FFD6 = $Fx`)

- `$00`: SPC7110
- `$01`: ST010/ST011
- `$02`: ST018
- `$03`: CX4

### Cálculo del checksum

Pasos:

1. Rellenar (pad) la ROM hasta el tamaño de potencia de 2 que indica la cabecera.
2. Poner a cero `$FFDC`-`$FFDD` y a `$0000` los bytes `$FFDE`-`$FFDF` antes de calcular (ambos campos de checksum se limpian primero).
3. Sumar todos los bytes de la ROM en aritmética de 16 bits, descartando el desbordamiento (overflow).
4. Guardar esa suma en `$FFDE`.
5. Guardar `suma XOR $FFFF` en `$FFDC`.

Como la propia cabecera forma parte de los bytes sumados, es imprescindible poner primero a `$0000`/`$FFFF` los campos de checksum y su complemento antes de calcular la suma real; si no, el resultado sería inconsistente consigo mismo.

## Notas / gotchas

- El byte de velocidad/modo de mapeo (`$FFD5`) es el que declara si la ROM es LoROM, HiROM, ExHiROM, SA-1, S-DD1 o SPC7110 — no hay autodetección de hardware real, todo depende de que este byte y el cableado físico del cartucho coincidan.
- Los valores `$02` y `$04` de tamaño de RAM de expansión (`$FFBD`) están marcados como indefinidos/no confirmados oficialmente.
- La cabecera expandida (`$FFB0`-`$FFBF`) solo se debe interpretar si `$FFDA` (ID de desarrollador) vale exactamente `$33`.
- Olvidar poner a cero los campos de checksum/complemento antes de calcular el checksum produce un valor incorrecto que no se autoverifica.

## Referencias cruzadas

- (pendiente de enlazar)
