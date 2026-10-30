---
titulo: Caso práctico — extraer sprites y fondos de un corto pixel art en 4K (Rick and Morty)
plataforma: [General]
categoria: [extraccion]
tags: [caso-practico, rick-and-morty, adult-swim, 4k, vp9, beat-em-up, lecciones]
estado: borrador
fecha: 2026-10-22
fuentes: [proyecto Extractor (C:\Users\migur\Desktop\Extractor), scripts en Extractor\tools\]
---

# Caso práctico — extraer sprites y fondos de un corto pixel art en 4K

## Resumen

Partíamos de un `.webm` de 17 minutos que parecía gameplay. En realidad es un **corto de animación en pixel art de Rick and Morty** ([adult swim]) con estructura de beat'em up. No había código ni sprites. Esta entrada recoge qué se hizo, en qué orden, qué falló y las cifras reales. Sirve de referencia para el siguiente vídeo.

> Propiedad intelectual: los assets son de Adult Swim. Valen para estudio o un proyecto personal o fan, no para uso comercial.

## Detalle técnico

### Datos del vídeo

- 3840x2160, VP9, 29,97 fps, 1.022 s, **30.630 fotogramas**, **232 planos**.
- Escalas del píxel: ×12 (320x180, 171 planos) y ×16 (240x135, 43 planos, la intro de la casa). El resto son flashes y el epílogo pintado (12 planos sin pixel art, exportados como 73 fotogramas clave a 1080p).

### Cronología: qué se probó y qué pasó

1. **Fotogramas nativos** (rejilla por plano, 4 procesos, ~25 min). ✓ Funciona, salvo que el primer detector eligió ×16 en vez de ×12 en el primer nivel. Se cambió al método de error de reconstrucción y se reextrajeron 8 planos.
2. **Mediana por plano sin alinear.** ✗ La cámara se mueve: 227 "escenas" para 232 planos y niveles largos con 0 sprites.
3. **Registro encadenado fotograma a fotograma para hacer panoramas.** ✗ Deriva de ±1 px acumulada: un fondo de 772 px emborronado.
4. **Registro contra el lienzo acumulado.** ✗ A medias: el parallax lo rompe.
5. **Tramos de cámara fija con mediana (73 fondos) y resta de fondo.** Fondos ✓ nítidos. Sprites ✗: 7.962 recortes, casi todos trozos, porque Rick y Morty se quedaban dentro del fondo.
6. **Votación entre tramos y umbral con histéresis.** 27 fondos sin ventanas emergentes; 3.848 sprites en 304 grupos (Rick, Morty, Summer, monstruos…) mezclados con ruido. Rick y Morty siguen dentro del fondo en las escenas de diálogo: el dato no existe.
7. **Flujo óptico** ✗ e **isnet-anime sobre el fotograma completo** ✗. Ver [segmentacion-ia-y-descomposicion.md](segmentacion-ia-y-descomposicion.md).
8. **Biblioteca de poses de Rick (84 plantillas) con búsqueda por plantilla.** ✓ El 25% de los fotogramas tiene coincidencia casi exacta; la pose de stand sale 1.507 veces.
9. **Refinado por apilado.** Las poses frecuentes salen limpias; las de pocas posiciones dejan halos de fondo.
10. **Refinado "Venn" iterativo** (fondos distintos, intersección al 80%, 3 rondas). En curso cuando se escribió esto.
11. **Propuestas pendientes:** paleta y contorno (Rick usa 13 colores con contorno 100% oscuro), SAM 2 para poses nuevas y ProPainter para los fondos tapados.

### Por qué fallaba la resta de fondo en este vídeo

- **Cámara cinematográfica:** coincidencia entre fotogramas consecutivos de 0,65 a 0,86 y a 15 fotogramas de 0,24 a 0,5. Solo unos 3.000 de 28.000 fotogramas tenían cámara fija.
- **Parallax** de varias capas y **fondos animados** (cielos de hexágonos, monitores, ventanas emergentes).
- **Protagonistas siempre en pantalla**, a menudo en el mismo sitio.

### Dónde está todo

- `Extractor\tools\`: scripts.
  - `scenes.py`: corte de planos.
  - `native.py`, `grid2.py` y `gridcheck.py`: rejilla y fotogramas nativos.
  - `assets4.py`: tramos, fondos, sprites y grupos.
  - `rick_test.py`: validación por plantilla.
  - `rick_refine.py` y `rick_venn.py`: refinado.
  - `declutter.py`: quitar personajes del fondo.
  - `motionfg.py`: prueba de flujo óptico descartada.
- `Extractor\assets_v4\`: fondos, sprites, grupos, `data\manifest.json` (fondos, sprites, grupos) y `placements.json` (`[fotograma, fondo, sprite, x, y]`).
- `Extractor\assets_v4\characters_named\rick\`: poses refinadas de Rick y su hoja de comparación.

## Notas / gotchas

- **Comprobar qué es el vídeo antes de diseñar nada:** créditos y logo al final, marca de agua… Aquí cambió las expectativas: es animación, no un juego, aunque reutiliza sprites como si lo fuera.
- **Mirar el vídeo antes de automatizar.** Varias iteraciones se perdieron por suponer cámara fija. Una hoja con 16 fotogramas de un plano largo lo habría mostrado al momento.
- **Medir antes de cambiar de método.** La prueba de reutilización (¿cuántas veces aparece idéntico un sprite?) llevó 10 minutos y decidió la estrategia.
- **Recursos:** la decodificación 4K en paralelo pone la CPU al máximo. Los miles de PNG disparan el antivirus. La memoria la llenan sobre todo los programas abiertos, no los scripts.

## Referencias cruzadas

- [README.md](README.md): orden recomendado.
- [rejilla-pixel-art-desde-video.md](rejilla-pixel-art-desde-video.md)
- [fondos-desde-video.md](fondos-desde-video.md)
- [sprites-por-plantillas.md](sprites-por-plantillas.md)
- [segmentacion-ia-y-descomposicion.md](segmentacion-ia-y-descomposicion.md)
- [MOTORES/arquitectura/port-desde-game-maker.md](../MOTORES/arquitectura/port-desde-game-maker.md): el paso siguiente, convertir assets y lógica en un juego.
