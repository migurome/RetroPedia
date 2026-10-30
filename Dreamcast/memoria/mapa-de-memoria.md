---
titulo: Mapa de memoria — regiones P0-P4, áreas físicas, VRAM de 32/64 bits y SDRAM
plataforma: [Dreamcast]
categoria: [memoria]
tags: [mapa-de-memoria, sh4, p1, p2, cache, vram, sdram, store-queues, ta]
estado: borrador
fecha: 2026-10-30
fuentes: [https://mc.pp.se/dc/memory.html, https://mc.pp.se/dc/pvr.html, https://mc.pp.se/dc/hw.html, https://www.dreamcast.wiki/Efficient_usage_of_the_Dreamcast_RAM]
---

# Mapa de memoria

## Resumen

El SH-4 ve un espacio físico de 29 bits (512 MB) dividido en 8 áreas de 64 MB. Ese espacio se "ve" varias veces en el espacio lógico de 32 bits, y lo que cambia de una copia a otra es si el acceso pasa por la caché y si hace falta modo privilegiado. Saber qué dirección usar (cacheada P1 o no cacheada P2) es la base para entender por qué hay que vaciar la caché antes de un DMA o para qué sirven las *store queues*.

## Detalle técnico

### Regiones lógicas (MMU desactivada)

| Región | Rango lógico | Acceso | Caché |
|---|---|---|---|
| U0/P0 | `0x00000000`–`0x7FFFFFFF` | usuario y privilegiado | según la MMU (desactivada por defecto). El espacio físico se repite 4 veces |
| P1 | `0x80000000`–`0x9FFFFFFF` | privilegiado | **cacheada** |
| P2 | `0xA0000000`–`0xBFFFFFFF` | privilegiado | **sin caché** |
| P3 | `0xC0000000`–`0xDFFFFFFF` | privilegiado | traducida por la MMU si está activa |
| P4 | `0xE0000000`–`0xFFFFFFFF` | privilegiado | registros internos de E/S, sin caché |

Las store queues son la excepción a la regla de "solo privilegiado". KOS corre en modo privilegiado, así que en la práctica todo es accesible.

Ejemplo: la RAM principal (física `0x0C000000`) se lee cacheada en `0x8C000000` y sin caché en `0xAC000000`. Por eso los programas se cargan en `0x8C010000` y los registros de hardware se suelen escribir en `0xA05F....`.

### Áreas físicas

| Rango físico | Área | Contenido |
|---|---|---|
| `0x00000000`–`0x03FFFFFF` | 0 | Boot ROM (2 MB), Flash ROM (256 KB), registros de hardware |
| `0x04000000`–`0x07FFFFFF` | 1 | VRAM (8 MB) |
| `0x08000000`–`0x0BFFFFFF` | 2 | — |
| `0x0C000000`–`0x0FFFFFFF` | 3 | RAM principal (16 MB) |
| `0x10000000`–`0x13FFFFFF` | 4 | entrada de comandos del Tile Accelerator |
| `0x14000000`–`0x17FFFFFF` | 5 | puerto de expansión (módem) |
| `0x18000000`–`0x1BFFFFFF` | 6 | — |
| `0x1C000000`–`0x1FFFFFFF` | 7 | registros internos (= P4). Exige modo privilegiado incluso desde U0 |

Dentro de la RAM principal, la BIOS usa `0x8C000000`–`0x8C00FFFF`: vectores de syscalls, `IP.BIN` en `0x8C008000` y el programa a partir de `0x8C010000` (ver [arranque](../avanzado/arranque-ip-bin-y-syscalls.md)).

### VRAM: dos formas de verla

Los 8 MB de VRAM son dos bancos de 4 MB. El bit 24 de la dirección elige cómo se accede:

| Interfaz | Direcciones (P2) | Organización |
|---|---|---|
| **64 bits** | `0xA4000000`–`0xA47FFFF8` | los dos bancos en paralelo: bits 0–31 en uno y 32–63 en otro. Aquí viven las **texturas** |
| **32 bits** | `0xA5000000` (banco 1) y `0xA5400000` (banco 2) | cada banco seguido. Lo usa el framebuffer |

Las palabras se intercalan: `0xA4000000-03` equivale a `0xA5000000-03`, `0xA4000004-07` a `0xA5400000-03`, `0xA4000008-0B` a `0xA5000004-07`... **No se puede escribir en VRAM byte a byte**: las escrituras tienen que ser de 16 bits o más. Las lecturas pueden ser de cualquier tamaño.

### Memoria de sonido

El AICA tiene 2 MB propios, que el SH-4 solo ve a través del bus G2 (lento). Los detalles medidos están en [ADPCM del AICA](../sonido/aica-adpcm.md).

### Cómo es la SDRAM principal

- Dos chips de 8 MB que funcionan como un único dispositivo de 64 bits a **100 MHz** (el reloj de bus del SH-4). Simplificando: filas de 2 KB, 512 filas por banco y 4 bancos de 4 MB.
- El SH-4 **siempre mueve bloques de 32 bytes** (ráfagas de 4), tanto con caché como sin ella o por DMA.
- La BIOS deja el controlador en modo "RASDown": las filas quedan abiertas tras cada acceso.
- Coste de una ráfaga (ciclos de bus):

| Operación | Fila abierta | Ninguna fila activa | Otra fila abierta |
|---|---|---|---|
| Lectura CPU | 7 | 10 | 12 |
| Escritura CPU | 6 | 7 | 9 |
| DMA (lectura o escritura) | 4 | — | — |

- Picos aproximados: la CPU lee a ~450 MB/s y escribe a ~600 MB/s, y el DMA llega a ~800 MB/s.
- Una ráfaga de 32 bytes al Tile Accelerator cuesta ~9 ciclos, pero se han visto **esperas de hasta 500 ciclos** cuando el TA está ocupado.

### Reglas para usar bien la RAM

- Alinear vectores a 16 bytes y matrices a 32 (una línea de caché).
- Agrupar datos que se usan juntos dentro de una misma fila de 2 KB, y poner cada flujo de datos grande en un banco distinto.
- Si dos arrays grandes están alineados a 16 KB, desplazar uno: la caché es de **correspondencia directa** y se pisarían (ver [optimización SH-4](../cpu/sh4-optimizacion.md)).
- `pref` para traer datos por adelantado, `movca.l` para reservar líneas de salida sin leerlas, y store queues u `ocbwb` para volcar flujos de escritura.

## Notas / gotchas

- Un puntero a RAM cacheada (`0x8C...`) que se pasa a un DMA sin vaciar antes la caché envía datos viejos. Pasa lo mismo al revés: tras un DMA de entrada hay que invalidar (`ocbi`) antes de leer.
- KOS deja libres unos **15,4 MB** de RAM principal y unos **5,4 MB** de VRAM tras `pvr_init` a 640×480. Son cifras medidas en este proyecto, ver el [overview](../README.md).

## Referencias cruzadas

- [Optimización del SH-4](../cpu/sh4-optimizacion.md)
- [PowerVR: pipeline y listas](../graficos/powervr-pipeline.md)
- [Arranque: IP.BIN, 1ST_READ.BIN y syscalls](../avanzado/arranque-ip-bin-y-syscalls.md)
