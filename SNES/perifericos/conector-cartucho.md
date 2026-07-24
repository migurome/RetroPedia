---
titulo: Conector de cartucho de SNES (62 pines)
plataforma: [SNES]
categoria: [perifericos]
tags: [pinout, cartucho, hardware, conector, cic]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Cartridge_connector]
---

# Conector de cartucho de SNES (62 pines)

## Resumen

El conector de cartuchos de SNES es una interfaz de 62 pines (31 por cada lado del borde dorado del cartucho) que expone el bus de direcciones y datos del 65C816, líneas de control de lectura/escritura, el bus de periféricos (B-Bus), señales del chip de seguridad CIC, audio analógico de entrada y alimentación. La vista de pinout se describe desde la perspectiva del lado derecho de la consola mirando hacia el conector.

## Detalle técnico

### Pinout completo (62 pines)

| Pin | Señal | Dirección | Pin | Señal | Dirección |
|-----|-------|-----------|-----|-------|-----------|
| 01 | SYSTEM CLK | → | 32 | /WRAMSEL | ← |
| 02 | EXPAND | <> | 33 | REFRESH | ← |
| 03 | PA6 | → | 34 | PA7 | ← |
| 04 | /PRD | → | 35 | /PWR | ← |
| 05 | GND | -- | 36 | GND | -- |
| 06 | CPU A11 | → | 37 | CPU A12 | ← |
| 07 | CPU A10 | → | 38 | CPU A13 | ← |
| 08 | CPU A9 | → | 39 | CPU A14 | ← |
| 09 | CPU A8 | → | 40 | CPU A15 | ← |
| 10 | CPU A7 | → | 41 | CPU A16 | ← |
| 11 | CPU A6 | → | 42 | CPU A17 | ← |
| 12 | CPU A5 | → | 43 | CPU A18 | ← |
| 13 | CPU A4 | → | 44 | CPU A19 | ← |
| 14 | CPU A3 | → | 45 | CPU A20 | ← |
| 15 | CPU A2 | → | 46 | CPU A21 | ← |
| 16 | CPU A1 | → | 47 | CPU A22 | ← |
| 17 | CPU A0 | → | 48 | CPU A23 | ← |
| 18 | /IRQ | ← | 49 | /ROMSEL | ← |
| 19 | CPU D0 | <> | 50 | CPU D4 | <> |
| 20 | CPU D1 | <> | 51 | CPU D5 | <> |
| 21 | CPU D2 | <> | 52 | CPU D6 | <> |
| 22 | CPU D3 | <> | 53 | CPU D7 | <> |
| 23 | CPU /RD | → | 54 | CPU /WR | ← |
| 24 | CIC data 1 | <> | 55 | CIC data 2 | <> |
| 25 | key CIC reset | → | 56 | CIC CLK | → |
| 26 | /RESET | <> | 57 | PHI2 | ← |
| 27 | +5V | -- | 58 | +5V | -- |
| 28 | PA0 | → | 59 | PA1 | ← |
| 29 | PA2 | → | 60 | PA3 | ← |
| 30 | PA4 | → | 61 | PA5 | ← |
| 31 | left audio in | ← | 62 | right audio in | → |

(Dirección de la señal indicada desde el punto de vista de la consola: → salida de la consola hacia el cartucho, ← entrada de la consola desde el cartucho, <> bidireccional, -- alimentación/masa.)

### Señales de reloj

- **Pin 1 (SYSTEM CLK)**: reloj de sistema de 21.47727 MHz.
- **Pin 57 (PHI2)**: reloj de la CPU. Los datos solo están garantizados como válidos (para lectura o escritura) en el flanco de bajada de esta señal.

### Bus de direcciones de CPU (A0-A23)

Repartido en los pines 6-17 (A11 a A0) y 37-48 (A12 a A23). Los 8 bits más altos, A23-A16, también se conocen como el "banco" de dirección (BA7-BA0).

### Bus de datos (D0-D7)

Pines 19-22 (D0-D3) y 50-53 (D4-D7). Este bus se comparte entre el acceso a memoria de la CPU y el acceso a periféricos.

### Control de lectura/escritura

- CPU /RD: pin 23.
- CPU /WR: pin 54.
- /PRD (lectura de periféricos): pin 4.
- /PWR (escritura de periféricos): pin 35.

### Selección de memoria

- **/ROMSEL (pin 49)**: se activa al acceder a ROM en los rangos $00-3F,80-BF:8000-FFFF y $40-7D,C0-FF:0000-FFFF.
- **/WRAMSEL (pin 32)**: se activa al acceder a la RAM de trabajo en los rangos $00-3F,80-BF:0000-1FFF y $7E-7F:0000-FFFF.

### Bus de periféricos (B-Bus)

Las líneas PA0-PA7 están repartidas en los pines 3, 28-30, 34 y 59-61.

### Señales del chip de seguridad CIC

- CIC data 1 (pin 24) y CIC data 2 (pin 55): líneas de datos bidireccionales del CIC.
- CIC CLK (pin 56): reloj del CIC, de 4 MHz o 3.072 MHz según el modelo de consola.
- key CIC reset (pin 25).

### Alimentación y masa

- +5V: pines 27 y 58.
- GND: pines 5 y 36.

### Señales adicionales

- **EXPAND (pin 2)**: conecta con el pin 24 del puerto de expansión y se mantiene en estado alto (pull-up).
- **REFRESH (pin 33)**.
- **/IRQ (pin 18)**: línea de solicitud de interrupción hacia la CPU.
- **/RESET (pin 26)**: línea bidireccional de reset.
- **left audio in / right audio in (pines 31 y 62)**: entradas de audio analógico que el cartucho puede mezclar con la salida de sonido de la consola.

## Notas / gotchas

- Los datos en el bus solo son válidos en el flanco de bajada de PHI2; diseñar hardware de cartucho (o emulación a nivel de pines) que muestree en otro momento produce lecturas/escrituras corruptas.
- /ROMSEL y /WRAMSEL cubren rangos de direcciones distintos y no se solapan; son la base del mapeo de memoria estándar (LoROM/HiROM) que debe respetar cualquier cartucho.
- El reloj del CIC varía entre 4 MHz y 3.072 MHz según el modelo concreto de consola, algo relevante para hardware de cartuchos que deba sincronizarse con el chip de seguridad.

## Referencias cruzadas

- (pendiente de enlazar)
