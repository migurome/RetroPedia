---
titulo: Texturas del PVR — formatos, twiddling, VQ, paletas de 4/8 bpp y modo sprite
plataforma: [Dreamcast]
categoria: [graficos]
tags: [powervr, texturas, twiddled, vq, paletas, pal4bpp, pal8bpp, mipmaps, strided, sprites, spritesheet, texconv, kmg]
estado: borrador
fecha: 2026-10-30
fuentes: [https://www.dreamcast.wiki/Texture_Formats, https://www.dreamcast.wiki/Twiddling, https://www.dreamcast.wiki/Paletted_Textures, https://www.dreamcast.wiki/PVR_Spritesheets, https://mc.pp.se/dc/pvr.html, cabeceras de KOS (dc/pvr.h, pvr/pvr_pal.h, pvr/pvr_txr.h)]
---

# Texturas, paletas y sprites en el PVR

## Resumen

La VRAM es el recurso más escaso en un juego 2D de Dreamcast: quedan ~5,4 MB para texturas. Elegir bien el formato puede dividir el consumo por 2, 4 o 6. Las herramientas son las texturas **paletizadas** (4 u 8 bits por píxel con una paleta global de 1024 colores) y la compresión **VQ**, que el hardware descomprime gratis al dibujar. Además, el **modo sprite** del PVR dibuja un rectángulo con una sola estructura de vértices. Todas las texturas tienen lados potencia de dos (8–1024), salvo las *strided*.

## Detalle técnico

### Formatos de color

| Formato | Constante KOS | Uso |
|---|---|---|
| ARGB1555 | `PVR_TXRFMT_ARGB1555` | color de 15 bits + alfa de 1 bit. Lo normal para sprites en lista PT |
| RGB565 | `PVR_TXRFMT_RGB565` | sin alfa. Fondos opacos |
| ARGB4444 | `PVR_TXRFMT_ARGB4444` | alfa con gradiente (humo, sombras suaves) |
| YUV422 | `PVR_TXRFMT_YUV422` | vídeo |
| BUMP | `PVR_TXRFMT_BUMP` | ángulos para *bump mapping* |
| paletizada 4 bpp | `PVR_TXRFMT_PAL4BPP` | 16 colores, la mitad que 8 bpp |
| paletizada 8 bpp | `PVR_TXRFMT_PAL8BPP` | 256 colores, la mitad que 16 bpp |

### Atributos

- **Twiddled** (por defecto): los píxeles se guardan en orden en "Z" (estilo Morton), no fila a fila, para que los vecinos 2D estén cerca en memoria. El filtrado sale más barato y la wiki recomienda usarlo siempre. Exige lados potencia de dos. Lo contrario es `PVR_TXRFMT_NONTWIDDLED`. Para una textura rectangular (por ejemplo 4×12) se reordena cada bloque cuadrado del lado menor por separado.
- **VQ** (`PVR_TXRFMT_VQ_ENABLE`): compresión con pérdida por cuantización vectorial, que el PVR decodifica en hardware sin coste al dibujar. La herramienta de KOS no admite VQ no cuadradas; **texconv** (tvspelsfreak) sí, y tiene vista previa.
- **Strided** (`PVR_TXRFMT_STRIDE`): permite anchos que no son potencia de dos. Ejemplo `pvr/strided_texture` de KOS.
- **Mipmaps**: versiones reducidas de la textura, con ~1/3 más de memoria. Abaratan el filtrado cuando la textura se dibuja más pequeña. En un 2D a escala 1:1 no hacen falta.

Comparativa de la wiki con una imagen de 1024×1024:

| Forma | Tamaño |
|---|---|
| RGB888 sin comprimir | 3 MB |
| RGB565 | 2 MB |
| RGB565 + VQ | 344 KB (~6 veces menos que RGB565) |
| `.kmg` (VQ) comprimido con gzip, en disco | 108 KB |

Conviene convertir los recursos (PNG, TGA) a formato PVR **al compilar**, no en la consola.

### Paletas

- El PVR tiene un banco de **1024 entradas de 32 bits** (4 KB, a partir de `PVR_PALETTE_TABLE_BASE`). La página "Texture Formats" de la wiki dice "1024 bytes", pero es un error: tanto mc.pp.se como la página de paletas y KOS hablan de 1024 entradas.
- El **formato de la paleta es global**: `pvr_set_pal_format(PVR_PAL_ARGB1555 | PVR_PAL_RGB565 | PVR_PAL_ARGB4444 | PVR_PAL_ARGB8888)` afecta a todas las texturas paletizadas del frame. Hay que elegir uno que sirva para todo.
- Organización:
  - **4 bpp** → 64 paletas de 16 colores (paleta *n* = entradas `16n`…`16n+15`).
  - **8 bpp** → 4 paletas de 256 colores (paleta *n* = entradas `256n`…`256n+255`).
- Escribir una entrada: `pvr_set_pal_entry(indice_0_a_1023, color_en_el_formato_actual)`.
- La paleta de una textura se elige al crear el contexto, con un OR: `PVR_TXRFMT_PAL8BPP | PVR_TXRFMT_8BPP_PAL(n)` (o `PVR_TXRFMT_4BPP_PAL(n)`). **`n` es el número de paleta, no el índice de entrada.** Si no se indica, se usa la 0.
- Subir los índices: `pvr_txr_load_ex(src, dst, w, h, PVR_TXRLOAD_8BPP)` (o `PVR_TXRLOAD_4BPP`), que también hace el *twiddling*. **Los cargadores de imágenes de KOS convierten todo a RGB565** y destrozan los datos paletizados: hace falta un cargador propio o texconv.

Usos: ahorrar VRAM, animar o reutilizar con otra paleta (*palette swap* de personajes), fuentes (las de la BIOS ya son de 4 bpp) y rampas de luminancia o alfa. Como el PVR mezcla internamente a 32 bpp, una paleta ARGB8888 de blanco con alfa da menos artefactos que ARGB4444.

### Spritesheets

Flujo de la wiki:

1. **TexturePacker** junta los sprites en una hoja potencia de dos (≤1024×1024) y genera un texto con nombre, x, y, ancho y alto de cada uno.
2. **texconv** la convierte a PAL8BPP: un `.dtex` (índices) y un `.dtex.pal` (paleta). Si la hoja tiene más de 256 colores pierde calidad, así que hay que revisar la vista previa.
3. En la consola: `pvr_mem_malloc` + carga del `.dtex`, y la paleta a un bloque de 256 entradas (`pvr_set_pal_entry(i + 256 * paleta, color)`).
4. Coordenadas de textura: `u0 = x / ancho_hoja`, `v0 = y / alto_hoja`, `u1 = (x + w) / ancho_hoja`, `v1 = (y + h) / alto_hoja`.

### Modo sprite

En vez de 4 vértices, se envía una cabecera de sprite y **una sola estructura** con las cuatro esquinas (`pvr_sprite_txr_t`: A arriba-izquierda, B arriba-derecha, C abajo-derecha y D abajo-izquierda, de la que solo se dan x e y):

```c
pvr_sprite_cxt_t cxt;
pvr_sprite_hdr_t hdr;

pvr_sprite_cxt_txr(&cxt, PVR_LIST_PT_POLY,
                   PVR_TXRFMT_PAL8BPP | PVR_TXRFMT_8BPP_PAL(pal),
                   tex_w, tex_h, tex, PVR_FILTER_NONE);
pvr_sprite_compile(&hdr, &cxt);
pvr_prim(&hdr, sizeof(hdr));          /* una vez por lote de sprites con la misma textura */
/* ... rellenar pvr_sprite_txr_t con PVR_PACK_16BIT_UV(u, v) y enviarlo ... */
```

- **No se puede rotar** ni dar color por vértice. Para eso hay que usar polígonos.
- **Las UV son de 16 bits** (medio float), no de 32 como en los polígonos. Un sprite mal colocado sale **deformado** (la wiki lo muestra en un personaje animado). Hay que alinear las posiciones para que se representen bien o volver a polígonos (*strips* de 4 vértices).
- Con escala 1:1, `PVR_FILTER_NONE`.
- Para muchos sprites, enviar con store queues (*direct rendering*, ver [PowerVR](powervr-pipeline.md)) en vez de un `pvr_prim` por sprite.

## Notas / gotchas

- **Paletas y MUGEN**: con cientos de paletas por personaje, los 1024 colores no bastan para todas a la vez. Por eso el proyecto de este repositorio convierte a ARGB1555 al subir (ver [caché de sprites](cache-sprites-vram.md)). Una opción intermedia es usar 8 bpp con las 4 paletas de 256 para las paletas *activas* en pantalla (dos luchadores + escenario + efectos) y reescribirlas por frame. Es solo una idea: no se ha medido.
- La página de *twiddling* de la wiki da el algoritmo a mano y su autor avisa de que solo lo probó con pocos ejemplos. Para código real, mejor `pvr_txr_load_ex` o texconv.
- El formato de paleta global también cuenta para las fuentes paletizadas: cambiarlo a mitad de frame afecta a todo.

## Referencias cruzadas

- [PowerVR: pipeline y listas](powervr-pipeline.md)
- [Caché de sprites en VRAM](cache-sprites-vram.md)
- [Formatos de MUGEN](../../MOTORES/mugen/formatos-sff-air-snd.md) — sprites con paleta propia
- [Imitar paletas de otras consolas (Mega Drive)](../../MegaDrive/graficos/imitar-paletas-otras-consolas.md)
