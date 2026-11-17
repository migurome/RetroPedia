---
titulo: Comparativa entre plataformas — fondos, parallax, transparencias, paletas y sprites
plataforma: [General, MegaDrive, SNES, NeoGeo, Dreamcast]
categoria: [general]
tags: [port, scroll, efectos, paletas, sprites, limites, referencia]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [MegaDrive/LIMITES.md, SNES/LIMITES.md, NeoGeo/LIMITES.md, Dreamcast/LIMITES.md, MegaDrive/graficos/layer-priority.md, MegaDrive/graficos/shadow-highlight.md, SNES/graficos/color-math.md, SNES/graficos/hdma-ejemplos.md, NeoGeo/graficos/sprites-scb.md, Dreamcast/graficos/escenarios-2d-paletas-y-baldosas.md]
---

# Comparativa entre plataformas

## Resumen

Cómo resuelve cada consola lo mismo. Sirve para planificar un demake o un port: qué efecto del original se traduce
directo, cuál hay que falsear y cuál conviene quitar. Resume entradas de RetroPedia; las cifras y su grado de confianza
están en el `LIMITES.md` de cada plataforma.

## Detalle técnico

### Lo esencial

| | Mega Drive | SNES | Neo Geo | Dreamcast |
|---|---|---|---|---|
| Modelo de dibujo | 2 planos de tiles y sprites | Hasta 4 capas de tiles, según el modo, y sprites | **Solo sprites** en columnas, más la capa fija | Polígonos con textura |
| Resolución habitual | 320×224 | 256×224 | 320×224 | 640×480 |
| Colores | 4 paletas de 16; 64 en pantalla, de 512 | 256 de 15 bits | 256 paletas de 16; 4096 en pantalla | Paletas de 1024 entradas o color directo |
| Tile o textura | 8×8, 15 colores más transparente | 8×8, según el modo: 4, 16 o 256 colores | Tile con una paleta: 15 colores más transparente | Lados potencia de 2, de 8 a 1024 |
| Sprites | 80; 20 por línea y 320 px por línea en H40 | 128; 32 por línea y 34 franjas de 8 px | Columnas de 16 px y hasta 32 tiles; 448 entradas | Sin límite fijo: manda el relleno |
| Lo que frena antes | VRAM por frame (~7,2 KB) | Bytes en vblank (~6,1 KB) | Sprites por línea | Relleno en lista translúcida |

### Parallax

| Plataforma | Cómo se hace | Coste |
|---|---|---|
| Mega Drive | Plano B para el fondo; scroll por línea o por bloques para las bandas; a mitad de pantalla, por interrupción | 2 capas reales: la tercera sale de bandas o de sprites |
| SNES | Una capa por plano en el modo 1 (2 de 16 colores y 1 de 4), o 4 de 4 colores en el modo 0. HDMA cambia el scroll por línea | Barato: la tabla se prepara una vez y HDMA la aplica en cada línea, robando pocos ciclos |
| Neo Geo | Cada capa son columnas de sprites encadenadas con *sticky* | Gasta el presupuesto de sprites por línea |
| Dreamcast | `posición = start − cámara · delta` por capa, y polígonos | Casi gratis si las capas son opacas o con punch-through |

### Transparencias

| Plataforma | Qué hay | Lo que no hay | Se falsea con |
|---|---|---|---|
| Mega Drive | Sombra y resalte, por píxel | Mezcla de colores | Parpadeo cada frame, tramas o sombra y resalte |
| SNES | Color math: suma, resta y sus mitades, con subpantalla o color fijo | Mezcla variable por sprite. En sprites, solo paletas 4 a 7 | Ventanas y HDMA para limitar la zona |
| Neo Geo | Solo color transparente en los sprites | Mezcla por hardware, según lo documentado aquí | Parpadeo o tramas |
| Dreamcast | Alfa por vértice y textura, y mezcla aditiva | Resta real: solo se aproxima | Cuidado con el relleno de la lista translúcida |

### Paletas

| Plataforma | Cambiar la paleta | Truco típico |
|---|---|---|
| Mega Drive | CRAM en vblank o a mitad de línea (con puntos de CRAM) | Repartir índices en una paleta, como en [SSB](decisiones-de-proyectos.md) |
| SNES | CGRAM; HDMA la cambia por línea | Degradados por línea |
| Neo Geo | Escribir en la RAM de paletas | Una paleta para cada columna, sin apenas escasez |
| Dreamcast | 4 bancos de 256 en 8 bpp; con 4 bpp, de 16 en 16 | Formato por sprite: el más pequeño donde quepan sus colores |

### Al llevar un juego de una a otra

| De → a | Lo más probable |
|---|---|
| Neo Geo → Mega Drive | Sobran colores y faltan sprites. Convertir fondos grandes a planos y reducir paletas |
| SNES → Mega Drive | Perder capas y color math. Cambiar transparencias por sombra y resalte o por parpadeo |
| Mega Drive → SNES | Menos ancho de pantalla: recolocar el HUD y revisar la cámara |
| Cualquiera → Dreamcast | El hardware sobra; el trabajo está en la VRAM de las texturas y el relleno |

## Notas / gotchas

- **Comparación de documentación:** no se ha medido ni comparado lado a lado. Lo probado de cada plataforma está en su
  `LIMITES.md`.
- **Neo Geo:** las transparencias son lo menos documentado; hay que confirmarlo antes de planificar con ello.
- **El presupuesto de vblank no es comparable sin más:** depende del modo, de la región y de lo que ya use la rutina.

## Referencias cruzadas

- [Límites de Mega Drive](../MegaDrive/LIMITES.md), [SNES](../SNES/LIMITES.md), [Neo Geo](../NeoGeo/LIMITES.md) y [Dreamcast](../Dreamcast/LIMITES.md)
- [Imitar paletas de otras consolas](../MegaDrive/graficos/imitar-paletas-otras-consolas.md)
- [Color math](../SNES/graficos/color-math.md)
- [Sombra y resalte](../MegaDrive/graficos/shadow-highlight.md)
