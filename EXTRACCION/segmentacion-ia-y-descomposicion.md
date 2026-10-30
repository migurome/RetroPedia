---
titulo: IA y descomposición de vídeo en capas para extraer sprites (qué sirve y qué no)
plataforma: [General]
categoria: [extraccion]
tags: [ia, segmentacion, sam2, rembg, isnet-anime, marionette, sprite-from-sprite, deformable-sprites, omnimatte, propainter]
estado: borrador
fecha: 2026-10-22
fuentes: [proyecto Extractor (pruebas con rembg/isnet-anime y flujo óptico DIS), MarioNette (NeurIPS 2021), Sprite-from-Sprite (SIGGRAPH Asia 2022), Deformable Sprites (CVPR 2022), Generative Omnimatte (CVPR 2025), SAM 2 (ICLR 2025), ProPainter (ICCV 2023)]
---

# IA y descomposición de vídeo en capas para extraer sprites

## Resumen

Hay investigación que resuelve justo "separar un vídeo de animación en sprites y fondo", pero casi toda **entrena una red por cada vídeo o plano**: necesita GPU NVIDIA y horas de cálculo. En un PC normal con GPU AMD, lo práctico es usar la IA **como apoyo puntual** de la búsqueda por plantillas ([sprites-por-plantillas.md](sprites-por-plantillas.md)), no como método principal.

## Detalle técnico

### Probado en el proyecto

| Técnica | Resultado | Por qué |
|---|---|---|
| **rembg + `isnet-anime`** (modelo de 176 MB, CPU, ~1 s por imagen) sobre el fotograma completo | ✗ Solo acierta cuando hay un objeto dominante (un jefe grande) | Es un modelo de "objeto principal": busca uno por imagen. En escenas recargadas no sabe cuál es. |
| **Flujo óptico (DIS)**, marcando lo que se mueve distinto de su entorno | ✗ Marca capas de parallax y fondos animados; no marca al personaje quieto | El movimiento no distingue personaje y fondo cuando la cámara y las capas se mueven a velocidades distintas. |

`isnet-anime` probablemente sí sirva **sobre un recorte** alrededor de un personaje ya localizado, porque ahí solo hay un objeto. Está pendiente de probar.

### Aplicable con matices

- **SAM 2 (Meta, *Segment Anything 2*).** Se marca el objeto una vez (clic, recuadro o máscara) y lo sigue durante todo el vídeo con memoria. Es lo más útil para **descubrir poses nuevas**: da la silueta, y las plantillas y el Venn la validan.
  - Dudas: no está entrenado con pixel art (hay que probarlo) y en CPU solo es viable el modelo pequeño (`sam2.1_hiera_tiny`).
  - Los ejemplos oficiales usan `torch.autocast("cuda", ...)`: en CPU hay que quitarlo.
  - Necesita PyTorch (1-2 GB).
- **ProPainter (ICCV 2023).** Inpainting de vídeo a partir de máscaras por fotograma, usando lo que se ve en otros fotogramas. Sirve para rellenar el fondo que los personajes tapan siempre, **una vez localizados**.
  - Pide bastante memoria de GPU; tiene opciones para reducirla (trozos más cortos, fp16, redimensionar).
  - El relleno sale algo suave y hay que reajustarlo a la paleta.

### Estado del arte (inspiración; poco práctico sin GPU NVIDIA)

- **MarioNette (NeurIPS 2021, código público `dmsm/MarioNette`).** Aprende sola un **diccionario de sprites** y dónde colocarlos a partir de vídeo de juegos con sprites. Es la versión automática de la biblioteca de poses. `--num_classes` fija el tamaño del diccionario.
- **Sprite-from-Sprite (SIGGRAPH Asia 2022, repo `lllyasviel/ToonDecompose`).** Descompone animación de dibujos en sprites: resuelve primero los sprites simples y con ellos los complejos. El código estaba a medio refactorizar.
- **Deformable Sprites (CVPR 2022).** Separa el vídeo en capas persistentes (textura más máscara y deformación por fotograma) usando el flujo óptico, sin intervención.
- **Generative Omnimatte (CVPR 2025) / OmnimatteZero.** Capas RGBA por objeto, incluidas sombras y efectos, con modelos de difusión de vídeo. Muy pesado.

## Notas / gotchas

- **Hardware:** con GPU AMD en Windows no hay CUDA. PyTorch corre en CPU, viable para inferencia con modelos pequeños sobre imágenes de 320x180, pero no para entrenar redes por vídeo.
- **Escalar antes de pasar a una red:** los fotogramas nativos (320x180) son diminutos para modelos entrenados con fotos. Se amplían ×4 con vecino más cercano y la máscara se reduce después.
- **Las herramientas de "vídeo a sprite" comerciales** (Totem, Sprite Extractor, Auto-Sprite…) suponen fondo liso tipo croma. No valen para gameplay real.
- **Ripear desde emulador** (desactivar capas en el visor) es lo ideal cuando existe el juego, pero no aplica si solo hay vídeo.

## Referencias cruzadas

- [sprites-por-plantillas.md](sprites-por-plantillas.md)
- [fondos-desde-video.md](fondos-desde-video.md)
- [MarioNette – GitHub](https://github.com/dmsm/MarioNette), [MarioNette – paper](https://papers.nips.cc/paper/2021/hash/2bcab9d935d219641434683dd9d18a03-Abstract.html)
- [ToonDecompose / Sprite-from-Sprite](https://github.com/lllyasviel/ToonDecompose)
- [Deformable Sprites (CVPR 2022)](https://openaccess.thecvf.com/content/CVPR2022/html/Ye_Deformable_Sprites_for_Unsupervised_Video_Decomposition_CVPR_2022_paper.html)
- [Generative Omnimatte](https://arxiv.org/html/2411.16683v2), [OmnimatteZero](https://arxiv.org/abs/2503.18033v1)
- [SAM 2 (ICLR 2025)](https://www.iclr.cc/virtual/2025/oral/31878)
- [ProPainter](https://sourceforge.net/projects/propainter.mirror/)
