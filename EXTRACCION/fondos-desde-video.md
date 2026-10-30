---
titulo: Reconstruir fondos y escenarios a partir de vídeo
plataforma: [General]
categoria: [extraccion]
tags: [fondos, mediana-temporal, registro, parallax, panorama, inpainting, propainter, camara]
estado: borrador
fecha: 2026-10-22
fuentes: [proyecto Extractor (73 y 27 fondos, varias versiones), CreateMap, Retro Game Cartography, ProPainter (ICCV 2023)]
---

# Reconstruir fondos y escenarios a partir de vídeo

## Resumen

La idea base es tomar, en cada píxel, el valor que más se repite a lo largo del tiempo (la **mediana temporal**). Así desaparece lo que se mueve y queda el escenario. Solo funciona bien con **cámara fija**. Con cámara en movimiento hay que alinear los fotogramas, y el **parallax** rompe cualquier alineación única. Lo que tapa un personaje que nunca se mueve **no está en el vídeo**: solo se puede rellenar o redibujar.

## Detalle técnico

### Qué tipo de cámara tiene el vídeo (mirarlo antes de elegir método)

Hay que medir qué fracción de píxeles coincide entre fotogramas (umbral de 18 niveles por canal):

| Situación | Consecutivos | A 15 fotogramas | Método |
|---|---|---|---|
| Cámara fija, fondo quieto | > 0,9 | > 0,9 | mediana temporal directa |
| Cámara fija, fondo animado | 0,7–0,9 | 0,6–0,8 | mediana y votación entre tramos |
| Cámara cinematográfica (paneos, temblor, parallax) | 0,65–0,86 | 0,24–0,5 | **quitar personajes primero**, después fondo |

En el caso real, en los niveles de acción solo ~3.000 de 28.000 fotogramas tenían cámara fija.

### Mediana por tramos de cámara fija ("plates")

- Un tramo empieza en un fotograma y sigue mientras cada fotograma coincida ≥ 70% con la referencia del tramo. Se toleran hasta 6 fotogramas raros seguidos (flashes, temblores), y el mínimo de tramo es de 12 fotogramas.
- La referencia se refresca con la mediana de lo ya aceptado (a los 5, 15 y 45 fotogramas).
- El fondo del tramo es la mediana de hasta 150 fotogramas espaciados.

### Votación entre tramos (para quitar lo que no es fondo)

- Se agrupan los tramos que muestran la misma vista (la misma "arena"), alineados con correlación de fase y un ajuste de ±1 px.
- En cada píxel gana el valor que comparten más tramos. Cada tramo pesa `√frames / (0,05 + actividad)`, donde la *actividad* es la frecuencia con que ese píxel se aparta de la mediana del tramo, dilatada 15 px para cubrir el cuerpo entero de un personaje que solo parpadea.
- Así desaparecen las ventanas emergentes, los enemigos de paso y los personajes que cambian de sitio entre tramos.

### Alternativa a la mediana: fondo de bajo rango (RPCA) — sin probar

- **Robust PCA** descompone la pila de fotogramas (alineados) en un fondo de **bajo rango** más lo que se mueve, que es **disperso**.
  - **DECOLOR** añade que lo que se mueve forma regiones contiguas (un campo aleatorio de Markov). Depende de cómo se inicialice.
  - Hay variantes que **estiman a la vez el movimiento global de la cámara** (arXiv 1603.05875) o que registran primero y descomponen después (*Augmented Robust PCA*).
- **Ventaja sobre la mediana:** los fondos animados **repetitivos** (monitores, cielos) caben en el bajo rango y se quedan en el fondo, en vez de ensuciar los sprites.
- **No resuelve** a los personajes que nunca se mueven, ni el parallax si no se separan antes las capas ([capas-parallax-por-movimiento.md](capas-parallax-por-movimiento.md)).

### Marca de agua fija en pantalla

Se buscan los píxeles claros en la esquina de la marca (más brillantes que su mediana local 5x5 por encima de 14) y se rellenan con `cv2.inpaint(..., INPAINT_TELEA)`. La marca va pegada a la pantalla, así que en un panorama hay que borrarla en cada posición de cámara.

### Panoramas de nivel

- Encadenar los fondos de tramos consecutivos con correlación de fase da panoramas de 2 a 11 pantallas. **Con parallax, las capas lejanas quedan desalineadas.**
- Las herramientas de mapas desde vídeo (CreateMap, Autostitcher, Retro Game Cartography) reconocen esa misma limitación: funcionan en juegos sin parallax.

## Notas / gotchas

- **No encadenar el registro fotograma a fotograma.** Cada paso mete ±1 px de error, que se acumula como un paseo aleatorio: una escena casi fija acabó en un "panorama" borroso de 772 px de ancho. Hay que registrar contra un **lienzo acumulado** de la escena y suponer "la cámara no se ha movido" salvo que otro desplazamiento encaje claramente mejor (+0,03 de coincidencia).
- **La mediana se come a los personajes quietos.** En una escena de diálogo, Rick y Morty estaban en el mismo sitio en los 14 tramos: quedan pintados en el fondo y no salen como sprites. Ninguna votación lo arregla, porque el dato no existe.
- **El flujo óptico no separa personaje y fondo** con parallax y fondos animados: marca las capas lejanas como "objeto" y no marca al personaje quieto mientras la cámara se mueve.
- **Orden correcto:** primero localizar a los personajes con plantillas ([sprites-por-plantillas.md](sprites-por-plantillas.md)) y enmascararlos en cada fotograma. Después, mediana o votación solo con los píxeles no enmascarados. Lo que quede siempre tapado se rellena:
  - con `cv2.inpaint` si es poco,
  - con **ProPainter** (inpainting de vídeo con máscaras, usa la información de otros fotogramas) si es mucho; luego hay que reajustar a la paleta, porque deja un relleno algo suave,
  - o se redibuja a mano.
- **Antivirus y miles de PNG:** escribir decenas de miles de archivos pequeños dispara el análisis en tiempo real (MsMpEng) y deja el PC lento. Conviene agrupar (npz o hojas) y exportar PNG solo al final.

## Referencias cruzadas

- [rejilla-pixel-art-desde-video.md](rejilla-pixel-art-desde-video.md)
- [sprites-por-plantillas.md](sprites-por-plantillas.md)
- [segmentacion-ia-y-descomposicion.md](segmentacion-ia-y-descomposicion.md): ProPainter y descomposición en capas.
- [capas-parallax-por-movimiento.md](capas-parallax-por-movimiento.md): un panorama por capa de parallax.
- [tiles-y-relleno-de-fondos.md](tiles-y-relleno-de-fondos.md): completar lo tapado con tiles repetidos o relleno por ejemplos.
- [elementos-animados-efectos-y-timing.md](elementos-animados-efectos-y-timing.md): bucles de elementos animados y efectos aditivos.
- RPCA: [DECOLOR](https://deepai.org/publication/moving-object-detection-by-detecting-contiguous-outliers-in-the-low-rank-representation), [RPCA con cámara móvil](https://arxiv.org/pdf/1603.05875), [Augmented RPCA](https://astro.paperswithcode.com/paper/augmented-robust-pca-for-foreground).
- [MegaDrive/herramientas/md-engine/camara-y-scroll.md](../MegaDrive/herramientas/md-engine/camara-y-scroll.md): parallax en el lado del motor.
- [CreateMap](https://github.com/hateif/createmap), [Retro Game Cartography](https://Codeproject.Com/Articles/5274896/Retro-Game-Cartography), [ProPainter](https://sourceforge.net/projects/propainter.mirror/).
