---
titulo: Separar las capas de parallax de un vídeo por su movimiento (capas de Wang y Adelson)
plataforma: [General]
categoria: [extraccion]
tags: [parallax, capas, movimiento, mosaico, panorama, correlacion-de-fase, kmeans, fondos]
estado: borrador
fecha: 2026-10-23
fuentes: [Wang & Adelson "Representing Moving Images with Layers" (IEEE TIP 1994), Kumar/Torr/Zisserman "Learning Layered Motion Segmentations of Video" (IJCV 2007), tesis de mosaicos por capas (U. Tennessee 2003, McGill)]
---

# Separar las capas de parallax de un vídeo por su movimiento

> **Validación:** técnica investigada, **sin probar aún** en el proyecto Extractor.

## Resumen

Un fondo con parallax son varias capas que se desplazan a velocidades distintas. Si se mide el movimiento **por bloques** y se agrupan los bloques que se mueven igual, cada grupo es una capa. Se monta **un panorama por capa**, y la relación entre velocidades da el **factor de parallax** de cada una. Es justo lo que necesita un juego: fondo lejano, medio y cercano por separado, cada uno con su velocidad. Con un único panorama, en cambio, las capas lejanas quedan borrosas.

## Detalle técnico

### Modelo (Wang y Adelson, 1994)

Cada capa tiene tres mapas: **imagen** (textura), **opacidad** (alfa, binaria en su versión) y **movimiento**. Las capas se ordenan por profundidad y se tapan unas a otras. El procedimiento original:
1. flujo óptico,
2. ajuste de un modelo de movimiento por bloque cuadrado,
3. agrupación de los modelos con k-means,
4. asignación de cada píxel al modelo que mejor lo explica,
5. un mosaico por capa.

### Versión simplificada para un juego 2D

En scroll lateral cada capa se mueve casi en **traslación pura**, así que no hace falta un modelo afín.

```python
import cv2, numpy as np

def block_shifts(a, b, bs=32, min_resp=0.3):
    """desplazamiento (dx, dy) de cada bloque entre dos fotogramas nativos en gris (float32)"""
    out = []
    for y in range(0, a.shape[0] - bs + 1, bs):
        for x in range(0, a.shape[1] - bs + 1, bs):
            (dx, dy), resp = cv2.phaseCorrelate(a[y:y+bs, x:x+bs], b[y:y+bs, x:x+bs])
            if resp > min_resp:                    # descarta bloques lisos (cielo uniforme)
                out.append((x, y, dx, dy))
    return np.array(out, np.float32)

def layer_speeds(shifts, k=3):
    """agrupa los desplazamientos en k velocidades de capa"""
    crit = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 50, 0.1)
    _, lab, centers = cv2.kmeans(shifts[:, 2:4], k, None, crit, 5, cv2.KMEANS_PP_CENTERS)
    return lab.ravel(), centers                    # centers[i] = (dx, dy) de la capa i
```

Pasos:
1. **Medir** con un salto de varios fotogramas (`t` y `t+4…8`), para que las diferencias de velocidad sean de varios píxeles.
2. **Agrupar** en 2-4 velocidades. Hay que **excluir antes** los bloques de los personajes ya localizados con plantillas ([sprites-por-plantillas.md](sprites-por-plantillas.md)): forman su propia "capa" y confunden la agrupación.
3. **Asignar por píxel**: se desplaza el fotograma anterior con la velocidad de cada capa y cada píxel va a la que da menos error. Se limpia con apertura y cierre morfológicos.
4. **Un mosaico por capa**: se acumula la posición de cada capa por separado (suma de sus desplazamientos) y se hace la mediana solo con los píxeles asignados a esa capa.
5. **Factor de parallax** = velocidad de la capa / velocidad de la capa del suelo, la que pisan los personajes. Ese número va directo al motor.

### Alternativa: fondo de bajo rango (RPCA)

Ver [fondos-desde-video.md](fondos-desde-video.md): tras alinear la cámara, RPCA separa un fondo "de bajo rango" de lo que se mueve. Se puede aplicar **por capa** una vez separadas.

## Notas / gotchas

- **Los bloques sin textura no dan movimiento fiable** (cielo liso, paredes planas). Se descartan con la respuesta de `phaseCorrelate` y se rellenan con la capa de sus vecinos.
- **Fondos animados** (monitores, cielos de hexágonos): su "movimiento" es animación, no desplazamiento. Se tratan aparte como bucles ([elementos-animados-efectos-y-timing.md](elementos-animados-efectos-y-timing.md)).
- **Cámara cinematográfica:** paneos verticales y temblores mueven todas las capas a la vez. La velocidad relativa entre capas se mantiene; la absoluta no.
- **Límites entre capas:** los píxeles de borde quedan mal asignados. Hay que dilatar la máscara de la capa cercana 1-2 px.
- Las tesis de mosaicos por capas señalan una ventaja extra: lo que una capa tapa en un momento suele verse en otro, porque las capas se deslizan unas sobre otras. Así **se rellenan huecos con datos reales**.

## Referencias cruzadas

- [fondos-desde-video.md](fondos-desde-video.md)
- [sprites-por-plantillas.md](sprites-por-plantillas.md): para excluir a los personajes.
- [MegaDrive/herramientas/md-engine/camara-y-scroll.md](../MegaDrive/herramientas/md-engine/camara-y-scroll.md): parallax en el motor.
- [Wang & Adelson (1994)](https://courses.cs.washington.edu/courses/cse576/06sp/papers/wangadelson.pdf), [apuntes Motion Layers](https://courses.cs.washington.edu/courses/cse576/06sp/notes/MotionLayers.pdf), [Kumar et al. (IJCV 2007)](https://www.doi.org/10.1007/S11263-007-0064-X)
- [Mosaicos por capas (U. Tennessee)](https://trace.tennessee.edu/entities/publication/19d63736-e280-4bb6-a72a-37d7f28a474d), [mosaicos dinámicos con parallax (McGill)](https://escholarship.mcgill.ca/downloads/1v53jz97b)
