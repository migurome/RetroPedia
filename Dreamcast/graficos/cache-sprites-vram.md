---
titulo: Caché de sprites en VRAM — texturas PVR, subida y asignador buddy
plataforma: [Dreamcast]
categoria: [graficos, memoria]
tags: [powervr, pvr, texturas, vram, cache, buddy, argb1555, zlib, sprites, 2d]
estado: revisado
fecha: 2026-10-30
fuentes: [cabeceras de KOS (dc/pvr.h, pvr/pvr_txr.h, pvr/pvr_mem.h), proyecto hyperdbz-dreamcast-port (dc/proto/src/sprcache.c y vram.c)]
---

# Caché de sprites en VRAM

## Resumen

Un juego 2D con miles de sprites por personaje (un luchador de MUGEN tiene 2.000–4.000) no cabe descomprimido ni en RAM ni en VRAM. Lo que funciona en Dreamcast es tener los sprites **comprimidos en RAM** y convertir en textura **solo los que se dibujan**, con una caché LRU en VRAM. Las cifras son de Flycast (orientativas) con dos personajes de Hyper DBZ.

## Detalle técnico

### Formato de textura

- Lados potencia de dos (8–1024). Un sprite de 100×130 va en una textura de 128×256 y se dibuja con U/V de 100/128 y 130/256.
- **ARGB1555 no twiddled** (`PVR_TXRFMT_ARGB1555 | PVR_TXRFMT_NONTWIDDLED`), índice 0 de la paleta → alfa 0.
- Lista **PT** (*punch-through*) para sprites normales y **TR** con `blend.src = blend.dst = PVR_BLEND_ONE` para la transparencia aditiva. En `pvr_init` hay que dar tamaño de bin a PT: `opb_sizes = {16, 0, 16, 0, 16}` (OP, OP mod, TR, TR mod, PT).
- Cuadrado con `pvr_poly_cxt_txr` + 4 vértices en *strip*; poner `cxt.gen.culling = PVR_CULLING_NONE` para poder voltear con U invertida.
- Con paletas propias por sprite (MUGEN usa cientos: un personaje tenía 365), los 1024 colores de paleta del PVR no bastan. Convertir a 16 bits al subir la textura evita el límite a cambio del doble de VRAM.

### Coste medido por sprite nuevo (fallo de caché)

| Paso | Tiempo |
|---|---|
| zlib `inflate` (~22 KB de índices) | 0,67 ms (≈15 MB/s) |
| Índice → ARGB1555 con la paleta | 0,42 ms (≈15 Mpx/s) |
| Reserva de VRAM (buddy propio) | 0,06 ms |
| Subida con `pvr_txr_load` (store queues) | 0,05 ms |

Con dos personajes a velocidad real hay como mucho 2 fallos por frame: 0,26 ms de CPU media y 3,6 ms en el peor frame.

### El problema de `pvr_mem_malloc`

- Esperar a que `pvr_mem_malloc` falle para expulsar texturas es **muy lento**: cada fallo escribe un aviso por la consola serie (≈5 ms de media por sprite, con picos de cientos de ms).
- `pvr_mem_available()` recorre todo el asignador: no se debe llamar en cada reserva.
- Aun llevando un presupuesto propio, el asignador de KOS **se fragmenta** con texturas de tamaños variados (en una prueba de estrés, 167 reservas fallidas y un frame de 650 ms).

### Asignador buddy

Como cada textura mide `ancho × alto × 2` con lados potencia de dos, su tamaño en bytes también es potencia de dos. Un asignador **buddy** encaja sin desperdicio:

- Al iniciar, se reserva con `pvr_mem_malloc` la VRAM libre en bloques potencia de dos (4 MB + 1 MB + ...), dejando un margen para otras texturas.
- Bloque mínimo de 256 B y listas de libres por nivel. Al liberar, se fusiona con el compañero (`índice ^ (1 << nivel)`) mientras esté libre.
- Si no hay bloque del tamaño pedido, se expulsa la entrada LRU y se reintenta.
- **No expulsar** texturas usadas en este frame ni en el anterior: el PVR puede estar renderizando todavía la escena anterior.

Resultado: una sola expulsión por reserva fallida, sin cascadas. El peor frame de la prueba de estrés bajó de 650 ms a 34 ms (dos sprites grandes nuevos en el mismo frame).

## Notas / gotchas

- `pvr_txr_load` usa store queues: tamaño y direcciones alineados a 32 bytes. Basta con subir `ancho_textura × alto_sprite × 2` redondeado a 32, no la textura entera.
- Las paletas paletizadas del PVR (8 bpp) ahorrarían la conversión y la mitad de VRAM para los sprites que comparten la paleta del personaje. Es una optimización pendiente de medir.
- Las cifras de CPU vienen de Flycast; falta medirlas en la consola.

## Referencias cruzadas

- [Dreamcast — overview](../README.md)
- [Formatos de MUGEN](../../MOTORES/mugen/formatos-sff-air-snd.md) — de dónde salen los sprites
