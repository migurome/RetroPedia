---
titulo: Ventanas de recorte (windows)
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, windows, mascara, recorte, color-math]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Windows]
---

# Ventanas de recorte (windows)

## Resumen

Las "windows" (ventanas) son un mecanismo del PPU para ocultar, por scanline, regiones horizontales de cualquier capa de fondo, de la capa de sprites, de la pantalla principal/subpantalla o de la zona donde se aplica color math. La SNES dispone de dos ventanas independientes (ventana 1 y ventana 2) que se pueden combinar mediante lógica booleana para formar recortes más complejos.

## Detalle técnico

### Posición de las ventanas

| Registro | Dirección | Función |
|---|---|---|
| WH0 | $2126 | Borde izquierdo de la ventana 1 |
| WH1 | $2127 | Borde derecho de la ventana 1 |
| WH2 | $2128 | Borde izquierdo de la ventana 2 |
| WH3 | $2129 | Borde derecho de la ventana 2 |

Cada registro es un valor de 8 bits (columna 0-255). Una ventana enmascara la región donde `izquierda <= X <= derecha`; si el valor de la izquierda es mayor que el de la derecha, esa ventana no cubre ninguna columna.

### Habilitación e inversión por capa

| Registro | Dirección | Bits | Capas que controla |
|---|---|---|---|
| W12SEL | $2123 | `DdCc BbAa` | BG1 (`Aa`/`Bb`), BG2 (`Cc`/`Dd`) |
| W34SEL | $2124 | `HhGg FfEe` | BG3 (`Ee`/`Ff`), BG4 (`Gg`/`Hh`) |
| WOBJSEL | $2125 | `LlKk JjIi` | OBJ (`Ii`/`Jj`), ventana de color (`Kk`/`Ll`) |

En cada par de bits, la letra minúscula habilita esa ventana sobre esa capa y la mayúscula invierte su efecto sobre esa misma capa (una ventana individual sin invertir vale 0 donde deja pasar la capa y 1 donde la oculta; al invertirla se intercambia ese comportamiento).

### Lógica de combinación cuando actúan ambas ventanas

| Registro | Dirección | Bits | Función |
|---|---|---|---|
| WBGLOG | $212A | `4433 2211` | Lógica de combinación para BG1-BG4 (2 bits por BG) |
| WOBJLOG | $212B | `....CCOO` | Lógica de combinación para OBJ (`OO`) y para la ventana de color (`CC`) |

Códigos de lógica (2 bits): `00`=OR (activa si cualquiera de las dos ventanas enmascara), `01`=AND (solo si ambas enmascaran a la vez), `10`=XOR (si exactamente una de las dos enmascara), `11`=XNOR (si ambas coinciden, enmascarando o no).

### Aplicación a pantalla principal/subpantalla y a color math

- **TMW ($212E) / TSW ($212F):** deciden, capa por capa, si la ventana configurada para esa capa se aplica a la pantalla principal (TMW) o a la subpantalla (TSW).
- **CGWSEL ($2130):** además de otras funciones de color math, define en qué región (fuera/dentro/toda/ninguna) la ventana de color fuerza negro en pantalla principal o dejar pasar el color math en la subpantalla.

El detalle bit a bit completo de todos estos registros está en `ppu-registers.md`; la mecánica de color math en sí se documenta en `color-math.md`.

## Notas / gotchas

- Si el borde izquierdo de una ventana es mayor que el derecho, esa ventana queda completamente "apagada" (no enmascara ninguna columna) en vez de invertirse.
- La distinción entre habilitar una ventana e invertirla es independiente por capa: una misma ventana física (posición) puede comportarse de forma opuesta en BG1 y en OBJ si se configuran bits de inversión distintos.
- Cuando ambas ventanas están habilitadas sobre la misma capa, el resultado depende del código de lógica elegido (OR/AND/XOR/XNOR) en WBGLOG/WOBJLOG — un error común es dejar el valor por defecto (normalmente OR) cuando se pretendía una intersección (AND).
- Las ventanas también pueden recortar la zona de color math de forma independiente a las capas gráficas, mediante la "ventana de color" seleccionada en WOBJSEL y aplicada vía CGWSEL.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Color math](color-math.md)
- [Fondos y modos de BG](backgrounds.md)
- [Sprites (OAM)](sprites.md)
