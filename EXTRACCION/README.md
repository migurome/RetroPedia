---
titulo: EXTRACCION — sacar assets y datos de un juego a partir de vídeo
plataforma: [General]
categoria: [extraccion]
tags: [ingenieria-inversa, video, sprites, fondos, pixel-art, opencv]
estado: borrador
fecha: 2026-10-22
fuentes: [proyecto Extractor (corto pixel art de Rick and Morty, 4K, 2026-10-22)]
---

# EXTRACCION — sacar assets y datos de un juego a partir de vídeo

## Propósito

Técnicas para reconstruir un juego cuando **solo se tiene vídeo**: sin ROM, sin código y sin sprites. Sirve para gameplays grabados, tráilers y animaciones en pixel art que se quieren convertir en juego.

Lo que se puede sacar de un vídeo:
- **Fondos** por escena o nivel.
- **Sprites** de cada personaje, agrupados en poses y movimientos.
- **Datos de animación**: qué pose sale en cada fotograma, dónde y cuánto dura. Con eso se deducen velocidades, alcances y tiempos para reprogramar la lógica.

A diferencia de `MegaDrive/` o `SNES/`, aquí no hay hardware: el "formato de origen" es un vídeo comprimido.

## Orden recomendado

El orden importa. Cada paso deja los datos que necesita el siguiente.

1. **Cortar en planos** y **bajar a resolución nativa** → [rejilla-pixel-art-desde-video.md](rejilla-pixel-art-desde-video.md). Todo lo demás trabaja sobre fotogramas nativos pixel-perfect. Sin este paso, nada encaja al píxel.
2. **Personajes primero**, con una biblioteca de poses y búsqueda por plantilla → [sprites-por-plantillas.md](sprites-por-plantillas.md). Los personajes son lo que tapa el fondo. Si se sabe dónde están en cada fotograma, se pueden quitar.
3. **Fondos después**, quitando a los personajes ya localizados → [fondos-desde-video.md](fondos-desde-video.md).
   - Con parallax: **una capa por velocidad** → [capas-parallax-por-movimiento.md](capas-parallax-por-movimiento.md).
   - Lo tapado: **tiles repetidos** primero y **relleno por ejemplos** después → [tiles-y-relleno-de-fondos.md](tiles-y-relleno-de-fondos.md).
4. **Lo que no es ni fondo ni personaje**: elementos animados del fondo (bucles), efectos aditivos y timing de la animación → [elementos-animados-efectos-y-timing.md](elementos-animados-efectos-y-timing.md).
5. **Paleta cerrada** (antes de deduplicar sprites o tiles) y **fuentes de la interfaz** → [paleta-y-fuentes.md](paleta-y-fuentes.md).
6. **IA solo como apoyo**, para poses nuevas o zonas que nunca se ven → [segmentacion-ia-y-descomposicion.md](segmentacion-ia-y-descomposicion.md).

Las entradas del 2026-10-23 (capas, tiles, elementos animados, paleta y fuentes) vienen de una búsqueda bibliográfica. Están marcadas como **«sin probar aún»** hasta validarlas en un proyecto.

Caso real con cifras y errores cometidos: [caso-rick-and-morty-pixel.md](caso-rick-and-morty-pixel.md).

## Lección principal

La resta de fondo genérica (mediana temporal, flujo óptico, segmentación automática) **falla** cuando:
- la cámara se mueve casi siempre,
- hay parallax,
- hay fondos animados, o
- los protagonistas no salen nunca de pantalla.

Lo que funciona es lo mismo que haría un juego: los sprites **se reutilizan al píxel**. Se construye un diccionario de poses y se buscan en cada fotograma. El fondo deja de importar, porque la comparación solo mira los píxeles del sprite.

## Entradas

Ver [INDEX.md](../INDEX.md), sección «Extracción de assets desde vídeo».
