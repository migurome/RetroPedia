---
titulo: Color math
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, color-math, transparencia, subpantalla, blending]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Color_math]
---

# Color math

## Resumen

Color math es el sistema del PPU para mezclar (sumar o restar) el color de la pantalla principal con un segundo color — proveniente de la subpantalla o de un color fijo — logrando efectos de transparencia, iluminación o sombreado sin necesidad de dibujar sprites o tiles adicionales. Se controla principalmente mediante los registros TM/TS (qué capas van a cada pantalla), CGWSEL y CGADSUB.

## Detalle técnico

### Registros implicados

- **TM ($212C) / TS ($212D):** eligen qué capas (BG1-4, OBJ) se dibujan en la pantalla principal y cuáles en la subpantalla respectivamente. El detalle bit a bit está en `ppu-registers.md`.
- **CGWSEL ($2130)** — bits `MMSS..AD`: `D` activa color directo; `A` elige el origen del sumando/sustraendo (`0` color fijo de COLDATA, `1` subpantalla); `SS` y `MM` definen en qué región (ninguna/fuera de ventana/dentro de ventana/toda la pantalla) actúa la ventana de color sobre la subpantalla y sobre la pantalla principal respectivamente.
- **CGADSUB ($2131)** — bits `MHBO4321`: `1-4` activan color math en BG1-BG4; `O` lo activa en sprites (solo con paletas 4-7, ya que las paletas 0-3 de sprite quedan siempre opacas); `B` lo activa sobre el color de backdrop; `H` activa "half color"; `M` elige la operación (`0` suma, `1` resta).
- **COLDATA ($2132)** — bits `BGRCCCCC`: fija el color de "sumando fijo" cuando CGWSEL selecciona esa fuente en vez de la subpantalla; cada escritura fija un valor de intensidad de 0-31 en uno o varios canales (R/G/B) a la vez.

### Modos de mezcla

Los dos bits superiores de CGADSUB seleccionan uno de cuatro comportamientos:

- **Suma:** suma directamente el color de pantalla principal y el de la fuente secundaria. Solo puede aclarar colores, nunca oscurecerlos.
- **Resta:** resta la fuente secundaria (subpantalla o color fijo) del color de pantalla principal. Solo puede oscurecer, nunca aclarar; por eso suele combinarse con paletas ya invertidas cuando se busca un efecto de "iluminación" restando negro decreciente.
- **Suma + half:** mezcla al 50% entre pantalla principal y fuente secundaria; es una ponderación fija que no puede ajustarse manipulando la paleta.
- **Resta + half:** reduce a la mitad el resultado ya oscurecido de la resta; su uso es poco frecuente porque la resta simple ya limita bastante el rango de resultados.

### Fuentes de color y enmascarado por capa

La fuente secundaria de la mezcla puede ser un color fijo (COLDATA) o el contenido dinámico de la subpantalla, según el bit `A` de CGWSEL. Los bits de CGADSUB permiten activar o desactivar color math por capa de forma selectiva: por ejemplo, los sprites con paleta 0-3 quedan siempre excluidos del color math (garantizando sprites opacos), mientras que los de paleta 4-7 sí pueden participar si el bit `O` está activo.

## Notas / gotchas

- La suma nunca oscurece y la resta nunca aclara: para lograr un fundido completo en ambos sentidos hay que combinar ambas técnicas o ajustar el color/paleta de la fuente secundaria.
- El modo "half" aplica siempre una ponderación del 50% fija; no es posible obtener otras proporciones de mezcla ajustando la paleta.
- Solo las paletas de sprite 4-7 pueden participar en color math; las paletas 0-3 quedan excluidas por diseño para preservar sprites siempre opacos.
- La ventana de color (configurada a través de WOBJSEL y aplicada vía CGWSEL) permite limitar espacialmente dónde se aplica el color math, de forma independiente a las ventanas normales de recorte de capas — ver `windows.md`.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Ventanas de recorte (windows)](windows.md)
- [Paletas y CGRAM](palettes.md)
- [Fondos y modos de BG](backgrounds.md)
