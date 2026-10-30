---
titulo: Escenarios 2D en el PVR — paletas de 4/8 bpp, baldosas potencia de 2, twiddling en PC y direct rendering
plataforma: [Dreamcast]
categoria: [graficos, memoria]
tags: [powervr, escenarios, parallax, pal4bpp, pal8bpp, twiddled, baldosas, direct-rendering, pvr_dr, uv-clamp, mugen, fsin]
estado: borrador
fecha: 2026-10-30
fuentes: [proyecto hyperdbz-dreamcast-port (tools/build_stage.py, dc/proto/src/stage.c, medido en Flycast 2.7), cabeceras y ejemplos de KOS (pvr_txr.h, pvr_texture.c, pvr.h, examples/dreamcast/pvr/fb_tex), Texturas del PVR (esta wiki)]
---

# Escenarios 2D en el PVR

## Resumen

Un escenario de juego de lucha 2D (aquí, los de MUGEN) son capas grandes con pocos colores: cielo, edificios,
suelo, mar... En ARGB1555 y con texturas de lado potencia de 2, Kame House de Hyper DBZ pediría 2,5 MB de VRAM
o más; bien convertido ocupa **1 MB** y se dibuja con **0,14 ms de CPU por frame**. La receta tiene cuatro
partes:
1. contar los colores y usar paletas de 4 u 8 bpp siempre que se pueda;
2. trocear cada capa en baldosas de potencia de 2;
3. hacer el *twiddling* en el PC;
4. enviar los polígonos por *direct rendering* con cabeceras precompiladas.

## Detalle técnico

### Formato por sprite

- Contar los colores **ya convertidos a ARGB1555**: dos colores de 24 bits que dan el mismo de 16 son uno, y
  más sprites caben en 4 bpp.
- Casi todas las capas grandes tienen entre 5 y 12 colores: van a **4 bpp** (paleta de 16 entradas, con la
  0 transparente). En Kame House solo el cielo (255 colores) y algunas animaciones necesitan **8 bpp**.
- Varios sprites pueden **compartir un banco de 8 bpp** si la unión de sus colores cabe en 255: se reasignan
  sus índices a las entradas del banco.
- Reparto de la RAM de paletas (1.024 entradas): bancos de 8 bpp desde abajo (entradas 256·n) y paletas de
  4 bpp desde arriba (16·n). Un banco ocupa el sitio de 16 paletas de 4 bpp. Si no queda sitio, ARGB1555.
- Si los personajes van en ARGB1555 (por sus cientos de paletas, ver
  [caché de sprites](cache-sprites-vram.md)), el escenario tiene la RAM de paletas entera para él.

### Baldosas potencia de 2

Una capa de 840×36 en una textura de 1024×64 desperdicia más de la mitad. Mejor trocear cada dimensión en
tramos de potencia de 2: el mayor que quepa, y el último redondeado hacia arriba si desperdicia poco (≤ 1/8).
Por ejemplo, 701 = 512 + 128 + 64 (61 útiles). Cada baldosa es un quad con su textura y la imagen se recompone
en pantalla. El mínimo del PVR es 8 y el máximo, 1024.

### Twiddling en el PC

Las texturas con paleta **tienen que** ir *twiddled*. `pvr_txr_load_ex` de KOS lo hace al cargar, pero avisa de
que no está probado con texturas más altas que anchas, y es lento. Mejor guardarlas ya reordenadas y subirlas
con `pvr_txr_load` (copia directa):

- índice del texel (x, y) = `Morton(y en bits pares, x en bits impares)` dentro de bloques cuadrados del lado
  menor, que van uno tras otro: `T = tw(y & m) | tw(x & m) << 1 + (x / s + y / s) · s²`, con
  `s = min(ancho, alto)` y `m = s − 1`;
- 8 bpp: el byte T; **4 bpp: el nibble T, el par en el nibble bajo**.

Un test que traduce a Python el bucle de `pvr_txr_load_ex` confirma que da exactamente los mismos bytes para
16, 8 y 4 bpp en texturas cuadradas y anchas.

### Dibujar

- **Listas:** el cielo sin transparencia va a la lista opaca; las capas con máscara, a la punch-through (el
  bit de alfa de la paleta hace de máscara); las aditivas, a la translúcida con
  `src = SRCALPHA, dst = ONE` y la intensidad en el alfa del vértice (`PVR_TXRENV_MODULATEALPHA`). Las capas
  de delante de los personajes llevan una z mayor.
- **Parallax:** posición = `start − cámara · delta`. Las capas tipo *parallax* de MUGEN (suelo o mar que se
  desplaza más abajo que arriba) son una cizalla: basta con desplazar distinto los vértices de arriba y los de
  abajo de cada quad. Es afín, así que sale exacta con un solo quad.
- **`PVR_UVCLAMP_UV` es obligatorio con baldosas.** Sin él, el último píxel (u = 1,0) muestrea la primera columna
  de la textura (se repite) y salen líneas finas en las juntas.

### Coste de CPU: qué importa

Medido en Flycast con unos 44 quads por frame:

| Versión | CPU por frame |
|---|---|
| `pvr_poly_compile` + `pvr_prim` por baldosa, capas recalculadas en cada lista | 0,68 ms |
| cabeceras precompiladas al cargar (una por lista) | 0,55 ms |
| capas calculadas una vez por frame con `fsin` (FSCA) + *direct rendering* | **0,14 ms** |

El *direct rendering* del KOS actual: dentro de `pvr_list_begin`/`pvr_list_finish`, escribir cada cabecera o
vértice (32 bytes) en `pvr_dr_target()` y enviarlo con `pvr_dr_commit()`. `pvr_dr_init` y `pvr_dr_finish` están
obsoletos. El `sinf` de newlib es caro; `fsin` de `dc/fmath.h` usa la instrucción FSCA, pero solo admite
ángulos entre 0 y 2π.

## Notas / gotchas

- Subir todo el escenario de una vez, **antes** de crear la caché de sprites de los personajes, y en un solo
  bloque de VRAM (cada baldosa alineada a 32 bytes): menos fragmentación y la caché se queda con lo que sobra.
  En Kame House la caché tuvo los mismos fallos con 1 MB menos de VRAM.
- El PVR no resta: `trans = sub` de MUGEN solo se puede aproximar. `addalpha` con alfa de destino distinto de 255
  tampoco tiene equivalente exacto.
- Cifras de los 47 escenarios de Hyper DBZ con esta conversión: la mayoría ocupa 0,5–1,5 MB y 38 caben en 3,5 MB.
  Los 9 restantes piden de 4 a 15 MB por capas animadas con muchos fotogramas grandes. Esos fotogramas
  tendrán que pasar por una caché, o comprimirse con VQ.

## Referencias cruzadas

- [Texturas del PVR — formatos, twiddling, VQ y paletas](texturas-paletas-y-sprites.md)
- [Caché de sprites en VRAM](cache-sprites-vram.md)
- [PowerVR2 — render por tiles y listas](powervr-pipeline.md)
- [Formatos de MUGEN — SFF, AIR y SND](../../MOTORES/mugen/formatos-sff-air-snd.md)
