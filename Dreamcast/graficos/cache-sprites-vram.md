---
titulo: Caché de sprites en VRAM — texturas PVR, subida y asignador buddy
plataforma: [Dreamcast]
categoria: [graficos, memoria]
tags: [powervr, pvr, texturas, vram, cache, buddy, argb1555, zlib, sprites, 2d, precarga, fragmentacion]
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
| zlib `inflate` (~22 KB de índices) | 0,67–0,71 ms (≈15 MB/s) |
| Índice → ARGB1555 con la paleta | 0,42–0,53 ms → **0,28 ms** con el bucle de 4 píxeles (abajo) |
| Reserva de VRAM (buddy propio, con expulsiones) | 0,06–0,12 ms |
| Subida con `pvr_txr_load` (store queues) | 0,05 ms |

Con dos personajes animándose sin más hay como mucho 2 fallos por frame. En un combate real (helpers, explods, intros) los fallos se juntan: en 90 s de Goku contra Krillin, 1654 fallos y un frame de 29 ms.

**Conversión de paleta más rápida:** leer 4 índices por vuelta y escribir dos palabras de 32 bits (`pal[a] | pal[b] << 16`) en vez de un `uint16_t` por píxel. Cada fila de la textura empieza alineada (ancho × 2 es múltiplo de 16). Para escribir con `uint32_t*` en un búfer de `uint16_t` sin romper el *strict aliasing* de GCC, usar un tipo con `__attribute__((may_alias))`. Mitad de tiempo en Flycast.

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

### Reservar solo las filas del sprite

Redondear el alto a potencia de dos casi duplica la VRAM: en los sprites de dos personajes de Hyper DBZ, ×1,9 y ×2,2 sobre lo que ocupan de verdad, frente a ×1,4–1,5 con el ancho en potencia de dos y el alto real. El PVR no lee filas que no use: con filtro `PVR_FILTER_NONE` y la V máxima en `alto / alto_textura`, las filas sobrantes no hacen falta.

Con el buddy se puede: se toma el bloque potencia de dos de siempre, pero solo se marca como usado el prefijo necesario, partido en piezas buddy contiguas de tamaño decreciente (los bits del número de unidades). El resto vuelve a las listas de libres. Para liberar, se recorren las mismas piezas leyendo el nivel de cada una en la tabla de estado.

```c
static void mark_used(pool_t *p, int u, int lvl, int n) {   /* n unidades de un bloque de 2^lvl */
    while(n < (1 << lvl)) {
        lvl--;
        if(n > (1 << lvl)) { p->state[u] = lvl + 1; u += 1 << lvl; n -= 1 << lvl; }  /* mitad baja entera */
        else list_push(p, lvl, u + (1 << lvl));                                      /* mitad alta sobra */
    }
    p->state[u] = lvl + 1;
}
```

**Ojo:** con piezas más pequeñas la VRAM se fragmenta más. Con la expulsión LRU de siempre (expulsar y reintentar hasta que algo se fusione) los fallos *aumentaron* (1654 → 1703) y apareció una reserva imposible. Hace falta cambiar también la expulsión.

### Expulsar por región, no por LRU

Cuando no hay bloque libre de tamaño B, se buscan todas las **regiones alineadas de tamaño B** de la VRAM gestionada. A cada una se le asigna el `last_frame` más reciente de las texturas que la tocan, o «intocable» si alguna se ha usado en este frame o en el anterior. Se vacía la región más antigua: al quedar libre entera, el buddy la fusiona en un bloque de tamaño B y la reserva sale a la primera. Una textura que ocupa varias regiones cuenta en todas ellas.

Junto con la reserva por filas, en el mismo combate: 1554 fallos (−6 %), 918 reintentos en vez de 1491, 0 reservas imposibles y hasta 173 sprites en caché en vez de ~160.

La búsqueda recorre la tabla de entradas: dimensionarla a lo que de verdad se usa. Con 2048 entradas (72 KB, no caben en los 16 KB de caché de datos del SH-4) la reserva costaba 0,19 ms; con 512, 0,12 ms. En combate nunca hubo más de ~175 a la vez.

### Precarga con el tiempo que sobra del frame

Las animaciones dicen qué sprite viene después. Si la lógica expone, para cada actor, el primer sprite distinto que mostrará en los próximos 2 ticks (si la animación sigue), se puede descomprimir **después de enviar la escena**, mientras el CPU esperaría el vblank. Antes de cada sprite se estima su coste (~60 ns por píxel en Flycast + 0,2 ms) y no se empieza si se pasaría de un límite (12 ms desde el inicio del frame). El sprite precargado se marca como usado **en este frame**: la protección normal («ni este frame ni el anterior») ya lo guarda durante el siguiente. Marcarlo como usado en el frame siguiente lo protegía un frame más. Con efectos enormes, esos sprites precargados que al final no se usaban dejaban sin VRAM al que sí hacía falta: una reserva imposible en 90 s, que desapareció al quitarlo.

Resultado: los fallos al dibujar bajan de 1554 a 442, la CPU media de 3,59 a 3,32 ms y el peor frame de 29,5 a 16,6 ms, con 0 vblanks perdidos.

### El límite que no se arregla con caché: efectos enormes

Algunos efectos usan un sprite distinto **en cada tick** y de hasta 100.000 píxeles (el Taiyoken de Krillin: 2–3 sprites de ~368×283 por tick durante ~12 ticks; el aura del súper de Goku: 16 fotogramas de 416×256). Son ~15 ms de descompresión por tick, sostenidos: la precarga solo reparte trabajo y no añade capacidad, y los fotogramas no caben todos en VRAM (~290 KB cada uno). Opciones:

- **Media resolución** al convertir el banco (bloques de 2×2 al color más cercano de la paleta) y dibujarlos al doble: 4 veces menos de todo, pero los rayos finos se ven escalonados. En Hyper DBZ se probó y se descartó por cómo quedaba.
- **Texturas de 8 bpp con paleta** (sin conversión y con la mitad de VRAM, pero el PVR las quiere *twiddled* y solo hay 4 bancos de 256 colores) o **VQ**.
- Un `inflate` más rápido que el de zlib.

## Notas / gotchas

- `pvr_txr_load` usa store queues: tamaño y direcciones alineados a 32 bytes. Basta con subir `ancho_textura × alto_sprite × 2` redondeado a 32, no la textura entera.
- Las paletas paletizadas del PVR (8 bpp) ahorrarían la conversión y la mitad de VRAM para los sprites que comparten la paleta del personaje. Es una optimización pendiente de medir.
- Las cifras de CPU vienen de Flycast; falta medirlas en la consola.
- Para contar frames perdidos, usar el contador de vblanks del PVR y no el periodo medido con el reloj: en Flycast el reloj se adelanta al vblank (ver [Flycast automatizado](../herramientas/flycast-automatizado.md)).
- Un `printf` por la consola o el buzón dentro del frame falsea la medida: con el registro de cada fallo activo, un frame de 16 ms pasaba a 24.
- **Probarla en PC:** un `<dc/pvr.h>` sustituto con una VRAM simulada en RAM (`pvr_mem_malloc` que avanza un puntero, `pvr_txr_load` como `memcpy`), un banco sintético con píxeles calculables y miles de frames de un combate simulado. En cada frame se comprueba que:
  - lo usado en este frame y en el anterior sigue en su sitio y con sus píxeles;
  - ninguna textura viva solapa a otra;
  - la VRAM ocupada cuadra con las entradas.

  Al vaciarlo todo, cada bloque debe volver a fusionarse. **Hace falta una fase de presión**, en la que lo de dos frames seguidos no quepa: con una carga normal, la región más antigua nunca contiene nada reciente. Así, un error que ignoraba la protección de los frames en uso pasaba todos los tests hasta que se añadió la presión.

## Referencias cruzadas

- [Dreamcast — overview](../README.md)
- [Formatos de MUGEN](../../MOTORES/mugen/formatos-sff-air-snd.md) — de dónde salen los sprites
