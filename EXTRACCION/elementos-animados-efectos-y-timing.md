---
titulo: Elementos animados del fondo, efectos (explosiones, brillos) y timing de la animación desde vídeo
plataforma: [General]
categoria: [extraccion]
tags: [bucles, video-textures, tiles-animados, efectos, fusion-aditiva, screen, timing, fotogramas-repetidos, animacion]
estado: borrador
fecha: 2026-10-23
fuentes: [Szeliski "Video Textures", Liao/Finch/Hoppe "Fast computation of seamless video loops", Durrant & Brostow "Seam hiding for looping videos", extracción de fluidos (SIGGRAPH 2012), documentación de modos de fusión (Autodesk, Boris FX), detector de repeticiones de Fora Soft]
---

# Elementos animados del fondo, efectos y timing de la animación

> **Validación:** técnicas investigadas, **sin probar aún** en el proyecto Extractor.

## Resumen

Tres tipos de elemento que ni son fondo fijo ni son personaje, y que conviene sacar aparte:
- **Elementos animados del fondo** (monitores, cielos, carteles que parpadean): se detecta su **bucle** y se exportan como tiles animados.
- **Efectos** (explosiones, brillos, rayos): si se suman al fondo, se recuperan **restando el fondo conocido**.
- **Timing**: los fotogramas repetidos dicen a cuántos dibujos por segundo se animó y cuánto dura cada pose.

## Detalle técnico

### 1. Bucles de elementos animados ("video textures")

Idea de Szeliski y sucesores: dentro de la zona que se mueve, se buscan dos fotogramas casi idénticos. La distancia entre ellos es el **periodo del bucle**.

```python
def periodo_bucle(R, max_l=120):
    """R: pila (T, h, w, 3) de la MISMA zona del fondo (cámara fija o alineada)"""
    R = R.astype(np.int16)
    d = np.array([np.abs(R[L:] - R[:-L]).mean() for L in range(2, max_l)])
    L = 2 + int(np.argmin(d))
    return L, d   # comprobar que también hay mínimo en 2L (bucle real, no casualidad)
```

- La zona se aísla con la máscara de **actividad** (píxeles que cambian a menudo; ver [fondos-desde-video.md](fondos-desde-video.md)), quitando antes a los personajes.
- Se exportan los `L` fotogramas como **tile o sprite animado** de fondo. El fondo fijo se queda con la mediana.
- Si el bucle no cierra limpio, las técnicas de Liao, Finch y Hoppe (consistencia espacio-temporal y fusión de Poisson) o de Durrant y Brostow (esconder el salto) lo suavizan. Para pixel art suele bastar con elegir bien el periodo.

### 2. Efectos con fusión aditiva o "screen"

Los efectos luminosos suelen componerse **sumando** (o con *screen*) sobre lo que hay debajo. Con el fondo limpio conocido:

```python
obs, bg = frame.astype(np.float32), fondo.astype(np.float32)
efecto_suma   = np.clip(obs - bg, 0, 255)                                 # modo suma (add)
efecto_screen = 255 - (255 - obs) * 255 / np.maximum(255 - bg, 1)         # modo screen
```

- **Cómo saber si un efecto es aditivo:** en su zona, casi todos los píxeles son **iguales o más claros que el fondo** en los tres canales. Si no, es un sprite **opaco** y se trata como un sprite normal ([sprites-por-plantillas.md](sprites-por-plantillas.md)).
- El efecto recuperado **se reutiliza en el motor con el mismo modo de fusión**. En Mega Drive no hay fusión real; se aproxima con shadow/highlight o con paletas ([MegaDrive/graficos/shadow-highlight.md](../MegaDrive/graficos/shadow-highlight.md)).
- **Humo y fuego semitransparentes** (fusión normal con alfa): el método de extracción de fluidos estima el fondo con los píxeles quietos, saca el alfa de la diferencia de color y lo refina.
- **Los negros "sucios"** del efecto dan resultados lechosos. Se corrigen recortando los negros, como el *Black Clip* de los compositores.

### 3. Timing: fotogramas repetidos ("animar en dos o en tres")

- La animación tradicional dibuja **cada 2 fotogramas** (12 dibujos por segundo a 24 fps) o cada 3 (8 por segundo). En pixel art es habitual animar a 8-15 dibujos por segundo, aunque el vídeo vaya a 30.
- **Detección global:** fracción de píxeles que cambian entre fotogramas consecutivos. Los valores casi nulos son repeticiones, y su patrón (2-2-2…, 3-2-3…) da la cadencia.

```python
cambio = (np.abs(F[1:].astype(np.int16) - F[:-1]).max(3) > 24).mean((1, 2))
repetido = cambio < 0.001       # ajustar al ruido del códec (en nativo es muy bajo)
```

- **Por personaje (más útil):** la búsqueda por plantillas da la pose de cada fotograma. Las rachas de la misma pose son su **duración**. Para pasar a un motor a 60 Hz, cada fotograma de vídeo a 29,97 equivale a unos 2 fotogramas de juego.
- Hay que separar lo que **se mueve sin cambiar de dibujo** (el sprite se desplaza con la misma pose) de lo que **cambia de dibujo**. Para el timing de animación cuenta lo segundo; lo primero da la velocidad.

## Notas / gotchas

- **El umbral de "repetido" depende del ruido.** Sobre fotogramas nativos el ruido del códec casi desaparece y el umbral puede ser muy bajo. Sobre el vídeo original no.
- **Un temblor de pantalla** anula las repeticiones globales aunque el dibujo no cambie. Por eso el timing por personaje, con plantillas, es más fiable.
- **Bucles y personajes:** si un personaje pasa por delante del elemento animado, contamina la pila. Hay que enmascararlo o elegir tramos limpios.

## Referencias cruzadas

- [fondos-desde-video.md](fondos-desde-video.md), [capas-parallax-por-movimiento.md](capas-parallax-por-movimiento.md)
- [sprites-por-plantillas.md](sprites-por-plantillas.md)
- [MegaDrive/graficos/bec-animated-sprites.md](../MegaDrive/graficos/bec-animated-sprites.md), [MOTORES/arquitectura/game-timers-framerate.md](../MOTORES/arquitectura/game-timers-framerate.md)
- [Liao, Finch y Hoppe — bucles rápidos](https://www.hhoppe.com/fastloops.pdf), [bucles infinitos desde un clip](https://arxiv.org/pdf/2011.02579), [seam hiding](https://discovery-pp.ucl.ac.uk/10117777/1/Seam_hiding_for_Looping_Videos.pdf), [cinemagraphs](https://jankautz.com/publications/MomImCVMP11.pdf)
- [Extracción de fluidos desde vídeo](https://unpaywall.org/10.1145%2F2370919.2370928), [modos de fusión (Autodesk)](https://download.autodesk.com/us/systemdocs/help/2011/smoke/files/WScba3ee2b36d8cb6f5f53495f1162be3bfe5-7ffa.htm), [Holdout Composite](https://borisfx.com/documentation/silhouette-2026.0/Silhouette-2026/Nodes-Holdout%20Composite.html)
- [Detector de repeticiones (Fora Soft)](https://www.forasoft.com/learn/downloads/judder-stutter-detector), [animar en unos, doses y treses](https://www.idtech.com/blog/what-does-animating-on-ones-twos-and-threes-mean)
