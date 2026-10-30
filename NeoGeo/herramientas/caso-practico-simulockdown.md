---
titulo: Caso práctico — personajes de Rick and Morty SimuLockdown (Godot 3) en Neo Geo
plataforma: [NeoGeo, General]
categoria: [herramientas]
tags: [port, godot, gdscript, tscn, pixel-art, escala, paleta, punto-fijo, ngdevkit, caso-practico]
estado: revisado
fecha: 2026-10-23
fuentes: [proyecto RickAndMorty/neogeo (ROM rmneo, probada en GnGeo), proyecto de Godot reconstruido de SimuLockdown (RickAndMorty/extraido/simulockdown_godot)]
---

# Caso práctico: SimuLockdown (Godot 3) → Neo Geo

## Resumen

Primer paso del port: dos personajes del juego (Morty, jugador 1, y Scary Terry, jugador 2) se mueven sin fondo, animan quieto y corriendo, se giran y se ordenan por profundidad. Aquí van las decisiones que hicieron falta para pasar de Godot a Neo Geo, con cifras reales. El proyecto está en `RickAndMorty/neogeo/` y se compila con `make`.

## Detalle técnico

### Leer los datos del original, no rehacerlos a mano

- Las animaciones salen de la escena `.tscn` de Godot, que es texto:
  - Cada fotograma es un `AtlasTexture` con `atlas = ExtResource(n)` y `region = Rect2(x, y, w, h)` dentro de la hoja.
  - El `SpriteFrames` da por animación `name`, `speed` (fps) y la lista de fotogramas.
- Datos reales:
  - Morty: idle con 4 fotogramas a 8 fps y run con 8 a 16 fps, en regiones de 320×256.
  - Terry: idle con 4 a 8 fps y run con 6 a 10 fps, en regiones de 256×256.
- El `AnimatedSprite` está centrado en el nodo: el centro de la región es la posición del personaje en Godot.

### El arte mezcla escalas distintas

- **Terry** es pixel art ×2 exacto.
- **Morty** está reescalado **×3,5** con vecino más próximo, así que sus rachas de color miden 3 y 4 píxeles alternos (3+4 = 7 = 2 píxeles lógicos).
  - Un detector de periodo entero lo da por "no pixel art" (escala 1).
  - Se descubre con el histograma de longitudes de racha: picos en 3, 4, 7, 10, 11 y 14.
- Método que funcionó (ver [rejilla de pixel art](../../EXTRACCION/rejilla-pixel-art-desde-video.md)):
  1. Se ajusta una rejilla de periodo fraccionario con un **periodo común a la hoja por eje y una fase por fotograma**. La fase vertical era igual en todos los fotogramas y la horizontal cambiaba.
  2. El ajuste dio exactamente 3,5 para Morty y 2,0 para Terry.
  3. Se muestrea cada celda **votando entre 3×3 puntos** interiores.
- Lo que **no** funcionó: partir las celdas entre los bordes reales detectados. Los píxeles de ruido crean "astillas" de celda, y salían columnas y filas duplicadas (costuras visibles en el brazo de Morty).

### Escala de salida: enteros y consistencia

- Neo Geo no amplía sprites por hardware (solo reduce), así que la escala va precalculada en la ROM. Con un factor entero el pixel art se mantiene nítido.
- La escala del juego en la arena (cámara con zoom 0,5 y nodos a 0,7) llevada a 320×224 da 1,05 px por píxel lógico de Terry y 1,83 de Morty. Se eligieron **Terry ×1 y Morty ×2**:
  - Terry queda casi exacto.
  - Morty queda un 14 % más grande, en proporción, que en el original. Es una decisión propia documentada.
- Con esa escala, 1 px de Godot ≈ **0,625 px** de Neo Geo.

### Paletas

- Morty tiene 12 colores reales, pero la hoja de carrera trae 29 tonos sueltos de 1 a 5 píxeles. Terry tiene 14 colores.
- Tras imantar a la paleta cerrada, cada personaje cabe en una paleta hardware (15 colores más el transparente): la 1 para Morty y la 2 para Terry. La paleta 0 es la del texto.

### Unidades de movimiento

- En Godot, `SPEED = 300` es px/s y la velocidad es instantánea: `velocity.x = ±SPEED` mientras se pulsa y 0 al soltar. En Neo Geo, a una actualización por VBlank: 300 × 0,625 / 60 = **3,125 px por fotograma**, `800` en punto fijo 24.8.
- La animación avanza `fps × 256 / 60` en un acumulador 8.8 por VBlank (idle a 8 fps: 34 por fotograma). Así la media es exacta aunque 60/fps no sea entero.
- El original no se mueve en vertical. En el port se añadió a la mitad de velocidad para probar la profundidad.

### Presupuesto resultante

| | Morty | Terry |
|---|---|---|
| Caja de todos los fotogramas | 138×100 px, 9×7 tiles | 81×57 px, 6×4 tiles |
| Sprites hardware | 9 | 6 |
| Colores | 12 | 14 |

En total, 22 fotogramas en **486 tiles únicos (62 KB de ROM C)**, sobre unas ROM C de 2 MB. La ROM P usa una fracción mínima de su 1 MB.

### Verificación

- Se compiló con ngdevkit 0.5 y se ejecutó en GnGeo, con pruebas automáticas que pulsan teclas solo en su ventana (ver [ngdevkit en Windows](ngdevkit-windows.md)).
- Comprobado: los dos mandos, la velocidad (134 px en ~0,72 s), el cambio de animación, el giro, la ordenación por profundidad en los dos sentidos y los límites de pantalla.

## Notas / gotchas

- **Medir antes de decidir la escala.** "Pixel art ×1, 12 colores" (lo que decía el primer análisis de Morty) era falso: era ×3,5. Muchas hojas de juegos de fans se exportan con escalas no enteras.
- **El ruido de color de la hoja rompe las cuentas de paleta.** Hay que cerrar la paleta antes de contar colores.
- El texto de la capa fix pisaba los pies de los personajes: conviene reservar desde el principio filas de HUD y limitar la `y` de los personajes.
- Pendiente para un port completo:
  - Gravedad y salto (`GRAVITY = 20` sumado cada fotograma a una velocidad en px/s y `JUMP_POWER = 550`).
  - Ataques y sus cajas de golpe.
  - Fondos con sprites: muchas `ParallaxLayer` en el original, con el límite de sprites por línea.
  - Sonido ADPCM: las voces y los efectos son WAV de 48-96 kHz y la música son OGG y MP3 de varios minutos.

## Referencias cruzadas

- [ngdevkit en Windows](ngdevkit-windows.md)
- [Sprites: SCB1-SCB4](../graficos/sprites-scb.md) · [Formatos de la ROM C, la ROM S y las paletas](../graficos/rom-c-s-y-paletas.md)
- [Portar un juego de Game Maker a una consola retro](../../MOTORES/arquitectura/port-desde-game-maker.md): mismo enfoque de leer el original y conservar sus unidades.
- [Velocidad y gravedad](../../GLOSARIO/speed-and-gravity.md) · [Punto fijo](../../GLOSARIO/fixed-point.md)
