---
titulo: Detectar la rejilla de pixel art en un vídeo y bajar a resolución nativa
plataforma: [General]
categoria: [extraccion]
tags: [pixel-art, rejilla, escala, fft, ffmpeg, vp9, submuestreo, deteccion-de-planos]
estado: borrador
fecha: 2026-10-22
fuentes: [proyecto Extractor (vídeo 3840x2160 VP9, 232 planos), proper-pixel-art (Hough), perfect-pixel (FFT), Pixel Snapper]
---

# Detectar la rejilla de pixel art en un vídeo y bajar a resolución nativa

## Resumen

Un vídeo de pixel art suele estar escalado por un factor entero: 320x180 ×12 = 3840x2160. Si se detectan el **tamaño del píxel lógico (periodo)** y su **fase (desplazamiento)**, cada fotograma se reduce a su resolución original sin perder nada. Además, la compresión del vídeo desaparece casi por completo. Es el primer paso de cualquier extracción: las plantillas, las paletas y las comparaciones exactas dependen de él.

## Detalle técnico

### 1. Cortar en planos antes de nada

- Se decodifica a baja resolución (`ffmpeg -vf scale=240:135:flags=area`, a unos 135 fps en 4K) y se mide la diferencia media entre fotogramas consecutivos.
- Hay corte si la diferencia es > 12 **y** mayor que 4 veces la mediana local (±8 fotogramas) + 4, con un mínimo de 4 fotogramas entre cortes.
- La escala puede **cambiar entre planos**: en el caso real había planos a 320x180 (×12) y otros a 240x135 (×16). Por eso la rejilla se detecta **por plano**, no una vez para todo el vídeo.

### 2. Detectar el periodo: error de reconstrucción (el método fiable)

Para cada periodo candidato `p` (3..24) y su fase:
1. La fase sale del histograma de bordes: se suma `|diff|` por columna y fila, se agrupa por `índice mod p` y se toma el máximo; el bloque empieza en el índice siguiente.
2. Se sustituye cada bloque `p×p` por su píxel central y se mide el error medio frente al original.
3. Con el periodo correcto el error cae en picado. En el caso real, con periodo 12 era 0,79 y con 16 era 6,57.
4. Se elige el **mayor** `p` con error ≤ 0,35 × la mediana de errores. Los divisores del periodo real (6, 4, 3) también dan error bajo, pero no son el píxel lógico.

```python
def recon_err(g, p, ox, oy):              # g = fotograma en gris, float
    ny, nx = (g.shape[0]-oy)//p, (g.shape[1]-ox)//p
    v = g[oy:oy+ny*p, ox:ox+nx*p].reshape(ny, p, nx, p)
    c = v[:, p//2, :, p//2]
    return np.abs(v - c[:, None, :, None]).mean()
```

### 3. Bajar a nativo: muestrear el centro del bloque

- **No** hay que promediar el bloque entero: los bordes vienen difuminados por el códec y mezclarían colores vecinos.
- Se promedian 3x3 puntos en `p/4, p/2, 3p/4` de cada bloque. En 4K tarda unos 12 ms por fotograma, frente a 80 ms del promedio completo.
- Se guardan los fotogramas nativos por plano (`npz` comprimido), y todos los pasos siguientes trabajan sobre ellos. Volver a decodificar el 4K es lo caro.

### 4. Escala no entera (hojas de sprites reescaladas)

Las hojas de sprites de un juego, no las de un vídeo, pueden venir reescaladas por un factor **fraccionario** con vecino más próximo. En SimuLockdown, Morty está a ×3,5: sus píxeles lógicos miden 3 y 4 píxeles alternos. Ver el [caso práctico de Neo Geo](../NeoGeo/herramientas/caso-practico-simulockdown.md).

- **Síntoma:** el detector de periodo entero (sección 2) no encuentra rejilla (sale escala 1), pero la imagen tiene pocos colores y bloques evidentes a simple vista.
- **Diagnóstico rápido:** el histograma de longitudes de racha (píxeles iguales seguidos, por filas y columnas). A ×3,5 salen picos en 3, 4 y 7 y luego en 10, 11 y 14. A ×2 exacto salen 2, 4, 6…
- **Método que funcionó:**
  1. Estimación inicial: la media de los huecos de una sola celda entre bordes, es decir, los huecos menores que el doble del hueco mínimo frecuente.
  2. Ajuste del **periodo común a toda la hoja, por eje**: se minimiza la distancia media, ponderada por cuántas filas cambian en cada borde, de los bordes a la rejilla. Cada fotograma tiene **su propia fase**. Primero se barre en pasos de 0,01 y luego de 0,001. Resultado: 3,500 para Morty y 2,000 para Terry.
  3. Muestreo con **votación de 3×3 puntos** (en 1/4, 1/2 y 3/4 de la celda, el central con doble peso). Elimina los píxeles de ruido.
  4. El índice nativo de la celda `k` es `k + round(fase/periodo)`, consistente entre fotogramas y sin colisiones.
- **Lo que no funcionó:** partir las celdas entre los bordes reales detectados. Un píxel de ruido crea una "astilla" de celda que acaba como columna o fila duplicada (costuras en el dibujo).
- La fase vertical era la misma en todos los fotogramas de Morty, pero la horizontal cambiaba de uno a otro. Hay que estimarla **por fotograma**.

## Notas / gotchas

- **El método de autocorrelación de bordes engaña.** El submuestreo de color 4:2:0 del códec crea un falso periodo de 2. Además, los múltiplos y divisores del periodo real puntúan alto. En el caso real eligió 16 en vez de 12 en el plano más largo del primer nivel, y todo lo posterior salió mal.
- **No estimar la rejilla con el primer fotograma del plano:** suele ser un flash o una transición. Hay que verificar en 3 puntos (inicio, mitad y final) y quedarse con la moda.
- **Cámara que se mueve en medios píxeles:** si la animación desplaza la cámara por debajo del píxel lógico, esos fotogramas salen ligeramente mezclados. Se nota como error de coincidencia más alto, no como fallo total.
- **Planos que no son pixel art** (ilustraciones pintadas, créditos): la confianza de la rejilla sale baja (< 0,35). Se exportan como fotogramas clave en alta resolución, no se intentan "pixelar".
- **Rendimiento:** pasar 4K en bruto (25 MB por fotograma) por una tubería a Python va a unos 4 fps por proceso. Hay que paralelizar por planos (`ffmpeg -ss` por plano) y no pedir más fotogramas de los necesarios.
- **Ajuste a paleta:** las herramientas de pixel art (Pixel Snapper, perfect-pixel) añaden un paso final que "imanta" cada píxel al color más cercano de una paleta cerrada. Elimina el ruido de compresión que queda y hace exactas las comparaciones posteriores.
  - Probado en hojas de sprites (caso Neo Geo): paleta = colores con al menos el 0,1 % de los píxeles (mínimo 20) y el resto al más cercano en RGB.
  - Morty bajó de 18 colores a 12 y cupo en una paleta de 15. En vídeo sigue sin probar.

## Referencias cruzadas

- [sprites-por-plantillas.md](sprites-por-plantillas.md): las plantillas necesitan fotogramas nativos.
- [fondos-desde-video.md](fondos-desde-video.md)
- [caso-rick-and-morty-pixel.md](caso-rick-and-morty-pixel.md)
- [NeoGeo/herramientas/caso-practico-simulockdown.md](../NeoGeo/herramientas/caso-practico-simulockdown.md): escala ×3,5 y ajuste a paleta en hojas de sprites.
- [MegaDrive/graficos/screen-resolution.md](../MegaDrive/graficos/screen-resolution.md): resoluciones nativas típicas de consola.
- Herramientas de rejilla: [proper-pixel-art](https://www.sourcepulse.org/projects/13911015), [Pixel Snapper](https://help.scenario.com/articles/1753861053-pixel-snapper-the-essentials), [flujo pixel-perfect](https://aigaming.nuilab.org/workflows/pixel-perfect-output/).
