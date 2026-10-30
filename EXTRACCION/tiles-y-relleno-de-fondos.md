---
titulo: Completar fondos tapados — detección de tiles y relleno por ejemplos
plataforma: [General]
categoria: [extraccion]
tags: [tiles, tileset, tilemap, tiled, inpainting, criminisi, resynthesizer, propainter, e2fgvi, fondos]
estado: borrador
fecha: 2026-10-23
fuentes: [Map-Tiler, TileMap2TileSet, Tile Extractor (retrospec), Criminisi et al. (CVPR 2003 / IEEE TIP 2004), Buyssens et al. (2015), Resynthesizer (GIMP), ProPainter (ICCV 2023), E2FGVI (CVPR 2022)]
---

# Completar fondos tapados — detección de tiles y relleno por ejemplos

> **Validación:** técnicas investigadas, **sin probar aún** en el proyecto Extractor.

## Resumen

Lo que un personaje tapa siempre no aparece en el vídeo. Se puede recuperar de tres formas, en orden de preferencia:
1. **Con datos reales**: si el fondo está hecho de **tiles repetidos**, el tile tapado ya se ha visto en otro sitio.
2. **Copiando parches reales del entorno** (relleno por ejemplos). Encaja con el pixel art mucho mejor que el relleno que difumina (Telea).
3. **Inventándolo** con IA: inpainting de vídeo o generativo. Solo como último recurso, y siempre reajustado a la paleta.

## Detalle técnico

### 1. ¿El fondo está hecho de tiles? (prueba rápida)

Los juegos de consola componen los fondos con tiles de 8x8 o 16x16. Una animación pintada puede no hacerlo. Se mide cuánto se repite:

```python
def repeticion_tiles(img, ts=16):
    """fracción de tiles repetidos; probar todas las fases (ox, oy) y quedarse con la mejor"""
    best = (0, 0, 0)
    for oy in range(ts):
        for ox in range(ts):
            h, w = (img.shape[0]-oy)//ts*ts, (img.shape[1]-ox)//ts*ts
            t = img[oy:oy+h, ox:ox+w].reshape(h//ts, ts, w//ts, ts, 3).swapaxes(1, 2).reshape(-1, ts*ts*3)
            rep = 1 - len({x.tobytes() for x in t}) / len(t)
            best = max(best, (rep, ox, oy))
    return best   # (fracción repetida, fase x, fase y)
```

- Antes hay que **ajustar a paleta** ([paleta-y-fuentes.md](paleta-y-fuentes.md)): con ruido de compresión, ningún tile es idéntico.
- Hay que contar también las **versiones volteadas** (horizontal, vertical y diagonal), como hace Map-Tiler con las 8 orientaciones de Tiled.
- **Por encima del 30-50% de repetición, el fondo es de tiles.** Entonces:
  - **tileset** con los tiles únicos y **tilemap** con su posición, listos para Tiled o para el VDP;
  - **relleno de lo tapado**: un tile parcialmente oculto se busca en el tileset; si solo uno coincide con todos los píxeles visibles, se completa con él.

Herramientas: **Map-Tiler** (Python; recorta, quita duplicados con volteos y genera un mapa de Tiled), **TileMap2TileSet** (detecta el tamaño de tile y la rotación; exporta a Godot o imagen) y **Tile Extractor** (retrospec, antigua).

### 2. Relleno por ejemplos (Criminisi; "Heal selection")

- **Criminisi et al.** rellenan el hueco desde fuera hacia dentro: primero los píxeles rodeados de píxeles conocidos o que continúan un borde fuerte. Cada parche se copia del parche más parecido de la propia imagen. Resultado: **texturas y bordes continuos, sin difuminar**.
- **Resynthesizer** (plugin de GIMP, GPL3): *Filtros > Mejora > Heal selection*. Sirve para hacerlo a mano. Las versiones antiguas dependían de Python 2; la 3.0 parece haberlo resuelto, sin confirmar.
- **OpenCV contrib** (`cv2.xphoto.inpaint`, con métodos *shift-map* y *FSR*) es una opción programable para probar. Necesita `opencv-contrib-python`.
- Después de rellenar: **ajustar a la paleta** del nivel y revisar a mano en Aseprite.

### 3. Inpainting de vídeo e IA generativa (último recurso)

- **ProPainter** (ICCV 2023) y **E2FGVI** (CVPR 2022): rellenan a partir de máscaras por fotograma usando lo que se ve en otros fotogramas.
  - E2FGVI se entrenó a **432x240**, cerca de la resolución nativa de muchos juegos.
  - Ambos están pensados para GPU NVIDIA (CUDA). En CPU no consta rendimiento.
- **Generativos** (Stable Diffusion con modelos de pixel art, Pixel-Art.ai) y luego limpieza con **Sprite Lab** (quita antialias y ajusta a paleta). Inventan contenido: hay que revisar siempre.

## Notas / gotchas

- **La fase de la rejilla de tiles no tiene por qué coincidir con la pantalla**: el scroll la desplaza. Hay que probar todas las fases, o trabajar sobre el panorama.
- **Interfaz y personajes encima** rompen la repetición. Se enmascaran antes de contar.
- **Telea y Navier-Stokes** (`cv2.inpaint`) difuminan: valen para huecos de 1-2 px, no para un personaje entero.
- Si el relleno se apoya en IA, **hay que marcarlo como inventado** en los metadatos. Al reconstruir, conviene saber qué es original y qué no.

## Referencias cruzadas

- [fondos-desde-video.md](fondos-desde-video.md)
- [capas-parallax-por-movimiento.md](capas-parallax-por-movimiento.md): las capas también rellenan huecos con datos reales.
- [paleta-y-fuentes.md](paleta-y-fuentes.md)
- [MegaDrive/graficos/tiles-and-palettes.md](../MegaDrive/graficos/tiles-and-palettes.md), [MegaDrive/graficos/tile-id.md](../MegaDrive/graficos/tile-id.md): destino de un tileset en Mega Drive (flip H/V en el Tile ID).
- [Map-Tiler](https://github.com/NonlinearIdeas/Map-Tiler), [TileMap2TileSet](https://yukkitimmy.itch.io/tilemap2tileset), [Tile Extractor](https://gendev.spritesmind.net/forum/viewtopic.php?p=3898), [mapa a TMX (foro de Tiled)](https://discourse.mapeditor.org/t/map-image-to-tmx/2897)
- [Criminisi et al.](https://www.microsoft.com/en-us/research/?p=150053), [Buyssens et al. 2015](https://tschumperle.users.greyc.fr/publications/tschumperle_tip15.pdf), [Resynthesizer](https://github.com/bootchk/resynthesizer/wiki/Quick-user's-guide-to-the-Resynthesizer-plugins-for-GIMP)
- [E2FGVI](https://openaccess.thecvf.com/content/CVPR2022/html/Li_Towards_an_End-to-End_Framework_for_Flow-Guided_Video_Inpainting_CVPR_2022_paper.html), [ProPainter](https://sourceforge.net/projects/propainter.mirror/), [Sprite Lab](https://morpheus-code.itch.io/spritelab)
