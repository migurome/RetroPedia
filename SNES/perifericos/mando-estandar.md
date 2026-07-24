---
titulo: Mando estándar de SNES
plataforma: [SNES]
categoria: [perifericos]
tags: [mando, controller, joypad, protocolo, io]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Standard_controller]
---

# Mando estándar de SNES

## Resumen

El mando estándar de SNES envía 16 bits de datos (12 botones más 4 bits de firma) a través de un registro de desplazamiento serie. Es compatible en sus primeros 8 bits con el protocolo del mando de NES, lo que en teoría permitiría usar un adaptador. Esta entrada documenta la distribución exacta de bits; el protocolo de lectura (latch/clock, registros de hardware) se trata en la entrada de lectura de mando.

## Detalle técnico

### Distribución de bits

Los 16 bits se reparten en dos registros de lectura automática, JOY1H ($4219, byte alto) y JOY1L ($4218, byte bajo):

```
   JOY1H       JOY1L
   $4219       $4218
15  bit  8   7  bit  0
 ---- ----   ---- ----
 BYsS UDLR   AXlr 0000
```

Donde cada letra corresponde a un botón: B, Y, Select (s), Start (S), D-pad (Up/Down/Left/Right), A, X, L, R. Los 4 bits menos significativos son la "firma" del dispositivo.

### Notas sobre el protocolo

- En lectura manual (bit a bit), los datos salen empezando por el bit más significativo, es decir el botón B primero.
- La firma de 4 bits está garantizada en `0000` para un mando estándar; un periférico distinto (ratón, Super Scope, multitap, NTT Data Keypad, etc.) reporta una firma distinta, lo que permite identificarlo por software.
- Tras los primeros 16 bits, seguir leyendo el registro de desplazamiento en un mando oficial devuelve únicamente unos (`1`).
- Los primeros 8 bits leídos son idénticos en formato a los del mando de NES, con la salvedad de qué botón físico corresponde a cada posición.
- Los dos primeros puertos se leen mediante los registros JOYSER0 ($4016) y JOYSER1 ($4017). Existe un accesorio Multitap que permite ampliar hasta 5 mandos estándar (ver entrada correspondiente).

## Notas / gotchas

- La página fuente no detalla temporización de las señales de latch/clock/data ni el pinout físico del conector: esos detalles están en las entradas de lectura de mando y de conector de mando respectivamente.
- Comprobar la firma (bits 0-3 en `0000`) es la forma recomendada de descartar que el periférico conectado no sea un mando estándar antes de interpretar el resto de bits.

## Referencias cruzadas

- (pendiente de enlazar)
