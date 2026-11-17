---
titulo: Dreamcast — límites y presupuestos
plataforma: [Dreamcast]
categoria: [referencia]
tags: [limites, memoria, vram, optimizacion, audio]
estado: borrador
comprobado: emulador
comprobado_en: "parcial: Flycast 2.7 con KOS y DreamSDK R4, hyperdbz-dreamcast-port; solo las filas marcadas como Medido"
fecha: 2026-11-06
fuentes: [Dreamcast/README.md, entradas enlazadas en cada fila]
---

# Dreamcast — límites y presupuestos

**Origen:** *Medido* es de nuestro proyecto en Flycast 2.7, nunca en la consola. *Documentado* viene de cabeceras de KOS o de
documentación de terceros, sin comprobar aquí. Una cifra de tiempo medida en Flycast es [solo orientativa](herramientas/flycast-automatizado.md).

## Máquina

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| CPU | SH-4 a 200 MHz, little endian; sin accesos desalineados de 16/32 bits | Medido | [overview](README.md) |
| Caché de datos | 16 KB | Documentado | [caché de sprites](graficos/cache-sprites-vram.md) |
| RAM principal | 16 MB; ~15,4 MB libres con KOS | Medido | [overview](README.md) |
| VRAM | 8 MB; ~5,4 MB libres tras `pvr_init` a 640×480 | Medido | [overview](README.md) |
| RAM de sonido | 2 MB; ~1,8 MB libres tras `snd_init` | Medido | [overview](README.md) |
| Disco homebrew | CD-R de 700 MB (CDI) o GDEMU (GDI o CDI) | Documentado; velocidad real sin medir | [overview](README.md) |
| Salida | 640×480 por VGA (progresivo) o compuesto/RGB (entrelazado) | Documentado | [overview](README.md) |

## Vídeo

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| Texturas | Lados potencia de 2, de 8 a 1024 | Documentado | [overview](README.md) |
| Paletas | 1024 colores compartidos; 4 bancos de 256 en 8 bpp | Documentado | [texturas](graficos/texturas-paletas-y-sprites.md) |
| Relleno en lista TR | ~500–600 K píxeles por frame a 60 fps; menos de 2 pantallas a 640×480 | Documentado (wiki, con GLdc) | [PowerVR](graficos/powervr-pipeline.md) |
| Relleno en lista PT | ~2,2 M píxeles por frame a 60 fps | Documentado (wiki, con GLdc) | [PowerVR](graficos/powervr-pipeline.md) |
| Latencia | 2 frames: no liberar una textura usada en el frame anterior | Documentado | [PowerVR](graficos/powervr-pipeline.md) |
| Transparencia `sub` | El PVR no resta; solo se aproxima | Documentado | [escenarios 2D](graficos/escenarios-2d-paletas-y-baldosas.md) |

## Sonido

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| Canales | 64 PCM/ADPCM | Documentado | [overview](README.md) |
| Efecto cargado con `snd_sfx_load_raw_buf` | 65.534 muestras (≈3 s a 22.050 Hz); más largo, por streaming | Documentado (código de KOS) | [ADPCM](sonido/aica-adpcm.md) |
| Streams de KOS (`snd_stream_alloc`) | 4 a la vez (`SND_STREAM_MAX`); con la música, 3 para voces | Documentado (código de KOS) | [ADPCM](sonido/aica-adpcm.md) |

## Costes de referencia (Hyper DBZ)

El frame a 60 Hz dura 16,7 ms.

| Operación | Coste | Origen | Entrada |
|---|---|---|---|
| Lógica de 2 personajes de MUGEN con VM optimizada | 2,66–5,47 ms por tick; peor tick, 8,0 ms | Medido | [CNS](../MOTORES/mugen/cns-compilacion.md) |
| `inflate` de zlib de ~22 KB | 0,67–0,71 ms (≈15 MB/s) | Medido | [caché de sprites](graficos/cache-sprites-vram.md) |
| Subir una baldosa VQ de 256×256 comprimida (`uncompress` + `pvr_txr_load`) | ~0,7 ms (Flycast 2.7) | Medido | [escenarios 2D: streaming](graficos/escenarios-2d-paletas-y-baldosas.md#streaming-de-baldosas-lo-que-no-cabe-en-la-vram) |
| Índice a ARGB1555 | 0,28 ms con el bucle de 4 píxeles | Medido | [caché de sprites](graficos/cache-sprites-vram.md) |
| Reserva de VRAM (buddy propio) | 0,06–0,12 ms | Medido | [caché de sprites](graficos/cache-sprites-vram.md) |
| Subida con `pvr_txr_load` | 0,05 ms | Medido | [caché de sprites](graficos/cache-sprites-vram.md) |
| Escribir 5 bloques en la VMU | ~835 ms | Medido | [Maple y VMU](perifericos/maple-mando-y-vmu.md) |

## Tamaños reales de contenido (Hyper DBZ)

| Contenido | Cifra | Entrada |
|---|---|---|
| Personaje de MUGEN | 2.000–4.000 sprites | [caché de sprites](graficos/cache-sprites-vram.md) |
| Escenarios | La mayoría, 0,5–1,5 MB; 38 de 47 caben en 3,5 MB; los otros 9 piden de 4 a 15 MB | [escenarios 2D](graficos/escenarios-2d-paletas-y-baldosas.md) |
| Sonidos de 2 personajes | 613 sonidos, 11 MB en ADPCM a 22 kHz mono; caben 143 a la vez | [ADPCM](sonido/aica-adpcm.md) |

## Pendiente de medir en la consola

Velocidad real del disco, coste de CPU de todo lo anterior y si el cuelgue de [lectura del CD](avanzado/lectura-del-cd-kos.md) también ocurre allí.
