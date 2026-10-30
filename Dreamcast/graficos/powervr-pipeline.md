---
titulo: PowerVR2 — render por tiles, listas OP/TR/PT, bucle de escena y fillrate
plataforma: [Dreamcast]
categoria: [graficos]
tags: [powervr, pvr, tile-accelerator, ta, isp, tsp, listas, punch-through, fillrate, direct-rendering, framebuffer, registros]
estado: borrador
fecha: 2026-10-30
fuentes: [https://mc.pp.se/dc/pvr.html, https://www.dreamcast.wiki/PowerVR_Introduction, https://www.dreamcast.wiki/Notes_on_fillrate_and_drawing_large_textures, https://www.dreamcast.wiki/2D_Rendering_Without_PVR, https://www.dreamcast.wiki/Graphics_APIs, ejemplos y cabeceras de KOS (dc/pvr.h)]
---

# PowerVR2: render por tiles y listas

## Resumen

El PowerVR2 (CLX2) no rasteriza polígono a polígono contra un framebuffer como una GPU clásica. Primero recibe **toda la escena**, la reparte en *tiles* de 32×32 píxeles y luego renderiza cada tile entero en una memoria interna, resolviendo la visibilidad antes de leer texturas. De ahí salen sus reglas: los polígonos se envían agrupados por **listas** (opacos, transparentes, *punch-through*), en orden, y la escena tiene latencia. A cambio, el *overdraw* de opacos sale casi gratis y la transparencia se ordena sola.

## Detalle técnico

### Dos etapas

1. **Binning (TA, Tile Accelerator):** la CPU envía primitivas al TA (por DMA, por store queues o con escrituras directas en el área 4). El TA las compila a un formato interno y apunta en cada *tile bin* qué primitivas lo tocan.
2. **Render (ISP/TSP):** se recorren los tiles. El ISP resuelve la profundidad de cada píxel dentro del tile y el TSP aplica textura y sombreado, solo para lo visible. Después el tile se copia al framebuffer en VRAM.

Como la textura no se lee hasta que el tile está resuelto, **pintar encima de lo opaco no gasta ancho de banda de textura**. Las dos etapas pueden trabajar en paralelo sobre frames distintos, con dos juegos de listas y dos framebuffers:

| Frame | Binning en | Render desde | Render a FB | Se muestra FB |
|---|---|---|---|---|
| 1 | TB1 | TB2 | 2 | 1 |
| 2 | TB2 | TB1 | 1 | 2 |
| 3 | TB1 | TB2 | 2 | 1 |

Esto implica **dos frames de latencia**: lo que se envía en el frame 1 se ve en el 3. Por eso no se puede liberar una textura usada en el frame anterior (ver [caché de sprites](cache-sprites-vram.md)).

### Listas

| Lista | Constante KOS | Para qué |
|---|---|---|
| Opacos | `PVR_LIST_OP_POLY` | geometría sólida. La más barata |
| Volúmenes modificadores de opacos | `PVR_LIST_OP_MOD` | sombras y efectos por volumen |
| Transparentes | `PVR_LIST_TR_POLY` | alfa y mezcla. **El hardware las ordena por profundidad** |
| Volúmenes modificadores de transparentes | `PVR_LIST_TR_MOD` | — |
| Punch-through | `PVR_LIST_PT_POLY` | alfa de 1 bit (píxel visible o no). Ideal para sprites |

- El hardware espera que los opacos vayan juntos y luego los transparentes. Un polígono transparente enviado a la lista OP **no se ve**.
- `pvr_list_begin(PVR_LIST_PT_POLY)` cierra implícitamente las listas anteriores del frame: hay que mandar antes lo opaco y lo transparente.
- Cada lista que se use necesita tamaño de *bin* en `pvr_init` (`opb_sizes`). Con `pvr_init_defaults()` la PT no tiene bin asignado (ver [caché de sprites](cache-sprites-vram.md)).
- Los triángulos se envían en **sentido horario**. Con el *culling* por defecto, un triángulo al revés no aparece. Para voltear sprites con U invertida: `cxt.gen.culling = PVR_CULLING_NONE`.

### Bucle de escena con la API de KOS

```c
pvr_init_defaults();                  /* o pvr_init(&params) con los bins que hagan falta */

for(;;) {
    pvr_wait_ready();                 /* esperar a que el PVR acepte otra escena */
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);
    /* fondos opacos */
    pvr_list_finish();

    pvr_list_begin(PVR_LIST_TR_POLY);
    /* texto, efectos con alfa */
    pvr_list_finish();

    pvr_scene_finish();
}
```

Cada primitiva es una **cabecera** (contexto compilado con `pvr_poly_cxt_col` / `pvr_poly_cxt_txr` + `pvr_poly_compile`, que se envía con `pvr_prim`) seguida de **vértices** `pvr_vertex_t`. Los vértices llevan `flags = PVR_CMD_VERTEX`, salvo el último del *strip*, que lleva `PVR_CMD_VERTEX_EOL`. El color se empaqueta con `PVR_PACK_COLOR(a, r, g, b)` usando floats de 0 a 1.

Para muchos vértices, `pvr_prim` (una llamada por vértice) es lento. El envío directo (*direct rendering*) escribe cada vértice en una store queue y lo vacía al TA:

```c
pvr_vertex_t *v = pvr_dr_target();    /* destino de 32 bytes (alterna entre las dos SQ) */
v->flags = PVR_CMD_VERTEX; v->x = x; v->y = y; v->z = z; /* ... */
pvr_dr_commit(v);                     /* = sq_flush */
```

(En el KOS actual, `pvr_dr_init` y `pvr_dr_finish` están obsoletos y no hacen falta.) El ejemplo `pvr/pvrmark_strips_direct` de KOS mide la diferencia.

### Fillrate

Medidas de la wiki con texturas ARGB1555 y GLdc (proyectos Reaperi Cycle y Summoning Signals), antes de bajar de 60 fps:

| Lista | Píxeles por frame |
|---|---|
| Transparente (TR) | ~500–600 K |
| Punch-through (PT) | ~2,2 M |

- Lo que cuenta es el área **dibujada** (ancho × escala × alto × escala), no el tamaño de la textura. Escalar no cambia la memoria usada pero sí el coste.
- A 640×480 (307 K píxeles), la lista TR da para cubrir la pantalla menos de dos veces: los fondos grandes deben ir en **OP** y los sprites recortados en **PT**, dejando TR para los efectos.
- La wiki sugiere (sin haberlo medido) que muchos polígonos pequeños rinden mejor que pocos grandes.

### Registros principales

Todos en P2. Bits exactos en las imágenes de mc.pp.se o en `dc/pvr/pvr_regs.h` de KOS.

| Dirección | Registro |
|---|---|
| `0xA05F8040` | color del borde |
| `0xA05F8044` | modo de display: doblar reloj (VGA), formato de color (RGB555/565/888 de 24 o 32 bits), *scan double*, activar |
| `0xA05F8050` / `8054` | inicio del framebuffer (campo largo / corto) |
| `0xA05F805C` | tamaño de display y módulo |
| `0xA05F80D0` | salida de vídeo: entrelazado, NTSC/PAL, polaridad de sincronías |
| `0xA05F80D8` | tamaño total: 60 Hz NTSC/VGA V=524 H=857; 50 Hz PAL V=624 H=863 |
| `0xA05F8020` / `802C` | entrada del ISP: lista de display / cabecera de tile bins |
| `0xA05F8048` / `804C` / `8060` | formato, módulo y dirección de salida del render |
| `0xA05F8108` | formato de la paleta de texturas (ARGB1555, RGB565, ARGB4444, ARGB8888) |
| `0xA05F8124` / `8128` / `813C` | TA: salida de tile bins (64 B por tile), salida de lista de display y tamaño del array de tiles |

### Alternativa: dibujar sin el PVR

KOS permite escribir píxeles directamente en el framebuffer:

```c
vid_set_mode(DM_640x480, PM_RGB565);
#define PACK_PIXEL(r, g, b) ( ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3) )
vram_s[x + y * 640] = PACK_PIXEL(255, 0, 0);
```

Es flexible (emuladores, *raytracers*, efectos de píxel) pero lento, porque todo lo hace la CPU. Formatos: `PM_RGB555`, `PM_RGB565` (el recomendado) y `PM_RGB888`. Hay modos de 320×240, 640×480, 768×480/576 y 800×608 (solo VGA), en variantes de 60 y 50 Hz. Un modo de 60 Hz sigue siendo de 60 Hz en una consola PAL, y algunas teles PAL no lo muestran, así que conviene ofrecer 50 Hz.

## Notas / gotchas

- El tutorial "PowerVR Introduction" de la wiki calcula los colores con `a / 255` sobre `uint8`. Es una división entera y siempre da 0 o 1: hay que usar `255.0f`.
- No hay z-buffer clásico que leer ni *scissor* cómodo: GLdc lo emula con varias pasadas. Es otra razón para usar la API PVR en 2D.
- El PVR trabaja internamente a 32 bpp y luego reduce al formato de salida (con *dither* opcional).

## Referencias cruzadas

- [Texturas, paletas y sprites](texturas-paletas-y-sprites.md)
- [Caché de sprites en VRAM](cache-sprites-vram.md) — costes medidos y bins de PT
- [Mapa de memoria](../memoria/mapa-de-memoria.md) — VRAM de 32/64 bits y área del TA
- [Optimización del SH-4](../cpu/sh4-optimizacion.md) — store queues
- [Bibliotecas, motores y recursos](../herramientas/bibliotecas-motores-y-recursos.md) — software, GLdc o API PVR
