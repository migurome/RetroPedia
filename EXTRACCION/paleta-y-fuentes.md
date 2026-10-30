---
titulo: Paletas cerradas y fuentes de texto a partir de fotogramas nativos
plataforma: [General]
categoria: [extraccion]
tags: [paleta, cuantizacion, median-cut, kmeans, aseprite, gpl, fuente, bitmap-font, bmfont, interfaz]
estado: borrador
fecha: 2026-10-23
fuentes: [Pylette, Atomic Object (median cut vs k-means), reductor de color de Sprite-AI, extractor de Wayline, FontCutter, Sprite2Font, CopyPxl, proyecto Extractor (paleta de Rick: 13 colores)]
---

# Paletas cerradas y fuentes de texto a partir de fotogramas nativos

> **Validación:** la paleta de Rick (13 colores, contorno 100% oscuro) está medida en el proyecto Extractor. Lo demás está investigado, **sin probar aún**.

## Resumen

- **Paleta:** el pixel art usa pocos colores. Sacar una paleta cerrada **por personaje** y **por nivel**, y ajustar cada píxel a su color más cercano (sin difuminado), elimina el ruido de compresión. Así las comparaciones son exactas y se exporta directamente a Aseprite o a una consola con límites de paleta.
- **Fuentes:** los textos de la interfaz (ventanas, carteles, letreros) se pueden convertir en una **fuente en mapa de bits** recogiendo cada letra de los fotogramas nativos.

## Detalle técnico

### 1. Extraer y aplicar una paleta

- **Median cut**: parte el espacio de color por su eje más largo, de forma recursiva. Suele dar la paleta más fiel.
- **k-means**: agrupa los colores en k centros. Alternativa cuando median cut deja colores raros.
- El reductor de Sprite-AI combina median cut con un refinado k-means y peso perceptual, sin difuminado.

```python
from PIL import Image
im = Image.fromarray(sprite_rgb)                       # solo píxeles opacos si hay alfa
q = im.quantize(colors=16, method=Image.Quantize.MEDIANCUT, kmeans=2, dither=Image.Dither.NONE)
pal = np.array(q.getpalette()[:16*3]).reshape(-1, 3)
```

- **Con transparencia:** la paleta se construye solo con los píxeles opacos (unirlos en una tira 1xN) y se aplica después al sprite completo.
- **Paleta por personaje:** la del sprite más visto, porque su mediana ya es casi exacta. En el caso real, la pose de stand de Rick tenía 13 colores, con el contorno en tres tonos oscuros. Hay que **fijar la paleta** y ajustar a ella todas las poses: así un mismo color nunca se divide en dos.
- **Exportar** como `.gpl` (GIMP/Aseprite) para cargarla en el editor.
- **Destino consola:** en Mega Drive son 4 paletas de 16 colores (15 más transparente) y el color va en 9 bits. Hay que reducir y redondear a ese espacio al final, no al principio.

### 2. Fuentes en mapa de bits

1. Se recortan las líneas de texto de los fotogramas nativos, donde cada letra ya es exacta.
2. Se separan las letras por columnas del color de fondo de la caja de texto.
3. Se deduplican con la misma clave exacta que los sprites.
4. Se etiqueta cada letra única a mano: son pocas decenas.
5. Se monta la hoja y el descriptor:
   - **FontCutter** (HTML5): rejilla de celda fija → `.fnt` de AngelCode (libGDX, Unity, Godot…).
   - **Sprite2Font**: hoja → fuente.
   - Si las letras tienen ancho variable, hay que ajustar los anchos a mano.
- **Descartar CopyPxl** y herramientas similares: **regeneran** las letras con IA a partir del estilo, en vez de copiar las reales.

## Notas / gotchas

- **Ajustar a paleta antes de deduplicar** sprites o tiles. Si no, el ruido de compresión crea "duplicados distintos".
- **Tintes de escena** (filtros de color de la intro, parpadeo blanco por daño): no deben entrar en la paleta base. Se detectan como un desplazamiento uniforme de todos los colores y se guardan aparte como **paletas alternativas**, que es como se haría en consola.
- **Fuentes con licencia:** las fuentes de un juego o serie suelen tener derechos. Sirven de referencia, no para redistribuirlas.

## Referencias cruzadas

- [sprites-por-plantillas.md](sprites-por-plantillas.md): filtro de paleta y contorno.
- [tiles-y-relleno-de-fondos.md](tiles-y-relleno-de-fondos.md): la detección de tiles necesita paleta ajustada.
- [MegaDrive/graficos/tiles-and-palettes.md](../MegaDrive/graficos/tiles-and-palettes.md), [MegaDrive/graficos/imitar-paletas-otras-consolas.md](../MegaDrive/graficos/imitar-paletas-otras-consolas.md), [MegaDrive/graficos/bec-fonts-text.md](../MegaDrive/graficos/bec-fonts-text.md)
- [Pylette](https://pypi.org/project/pylette/2.3.0), [median cut frente a k-means](https://spin.atomicobject.com/pixels-and-palettes-extracting-color-palettes-from-images/), [reductor de Sprite-AI](https://www.sprite-ai.art/tools/color-reducer), [extractor de Wayline (.gpl)](https://www.wayline.io/tools/color-palette-from-image)
- [FontCutter](https://github.com/fabienbk/fontcutter), [CopyPxl](https://forum.godotengine.org/t/copypxl-turn-any-image-into-a-bitmap-font-sheet-godot-unity-phaser-ready/137387)
