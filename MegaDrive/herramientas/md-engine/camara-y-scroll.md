---
titulo: MD Engine — cámara, niveles largos con scroll y parallax
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [md-engine, camara, scroll, parallax, deadzone, h32, h40, niveles-largos]
estado: borrador
fecha: 2026-10-15
fuentes: [definiciones de eventos de MD Engine 1.0.9; plantillas camera-follow, squarebrothers, parallax, visual-novel]
---

# MD Engine — cámara y scroll

## Resumen

Para niveles largos con principio y final (estilo Super Mario Bros. 3 o Sonic), MD Engine ofrece seguimiento de cámara con varios estilos, zona muerta, límites, bloqueo por eje y scroll por líneas o tiles para el parallax. La escena puede ser mucho mayor que la pantalla: el motor hace streaming del plano.

## Detalle técnico

| Evento | Campos | Uso |
|---|---|---|
| `EVENT_CAMERA_FOLLOW_ACTOR` | actorId, style (`simple`, `platformer`, `topdown`, `screenbyscreen`), lerp (0–100), offsetX, offsetY | Cámara que sigue al héroe; `platformer` para plataformas, `screenbyscreen` para salas tipo Zelda o Metroid clásico |
| `EVENT_CAMERA_COPY_BOUNDS` | plane (`background`) | Limita la cámara al tamaño del fondo |
| `EVENT_CAMERA_SET_BOUNDS` | x, y, width, height | Límites manuales (por ejemplo, cerrar una arena de jefe) |
| `EVENT_CAMERA_SET_DEADZONE` | x, y, width, height | Zona muerta: el héroe se mueve dentro sin mover la cámara |
| `EVENT_CAMERA_SET_ALLOWED_SCROLL` | plane, scrollX, scrollY | Bloquear un eje (el scroll vertical de SMB3 solo se libera en ciertas zonas) |
| `EVENT_CAMERA_SET_LERP` / `SET_OFFSET` / `SET_POSITION` / `SCROLL_TO` | | Suavizado, adelantar la cámara, cinemáticas |
| `EVENT_CAMERA_SHAKE` / `EVENT_CAMERA_FLASH` | time, frames, magnitude / r, g, b | Impactos |
| `EVENT_SCROLL_SET_MODE` | horizontal (`plane`, `tile`, `line`), vertical (`plane`, `column`) | Activa el scroll por línea o tile para parallax |
| `EVENT_SCROLL_SET_VALUE` / `SET_VALUE_RELATIVE` / `SET_VALUE_HBLANK` | plane, value, direction, startH, countH… | Desplaza franjas a distinta velocidad |

**Parallax sin eventos (plantilla `parallax`):**
- La escena lleva el nivel en `foregroundId` (plano A, transparente en el índice 0) y el paisaje en `backgroundId` (plano B).
- El campo `parallaxBG` define franjas de filas con su velocidad: `[{ "type": 0, "value": <filas>, "speed": 0.25 }, …]`, de arriba abajo. ~~Las filas deben sumar la altura de la escena.~~ **Corrección (DKAS Demake B002, leído en `TBCE_Scroll.inc`):** las franjas son filas de tile del **plano/pantalla** (scroll horizontal `HSCROLL_TILE` por fila), no de la escena. En una escena más alta que la pantalla deben sumar las filas de la imagen (28 para 224 px).
- **⚠ El parallax solo afecta al eje X.** En `TBCE_ScrollPlaneBTo`, con franjas horizontales el plano B sigue a la cámara en **vertical 1:1** (`SetVertValue(Y)`). En una escena más alta que la imagen del fondo (p. ej. 528 px con un fondo de 224), la imagen se sale y el plano de 256 px se repite. Arreglo: `EVENT_CAMERA_SET_ALLOWED_SCROLL` con `plane: "background"`, `scrollX: true`, `scrollY: false` en el `onStart` de la escena.
- **Franjas que cortan objetos altos:** los árboles o edificios que cruzan dos franjas se parten al moverse a velocidades distintas. Si el dibujo no está separado en bandas horizontales limpias, es mejor usar una sola franja.
- El compilador lo traduce a `TBCE_ScrollSetParallax`.
- **⚠ Las franjas solo se aplican a capas en modo `image`.** En `TBCE_Camera.c`, una capa en modo `map` se desplaza entera con `TBCE_MapScrollTo` e ignora `parallaxBG` (comprobado en Retrovania B012: el fondo no se movía). La capa lejana debe ser `"mode": "image"`.
- Una capa `image` ocupa el plano del VDP (512 px de ancho en H40). Si el dibujo mide 512 px y se repite cada 256 px, el plano da la vuelta sin costura y no hace falta streaming. Para imágenes más anchas que no se repiten: `EVENT_IMAGE_GET_LAYER_HANDLE` + `EVENT_IMAGE_ENABLE_STREAMING`.
- Las capas del nivel (modo `map`) sí hacen streaming solas: Retrovania tiene escenas de 2.560 px.
- Con un fondo lejano corto, los límites de la cámara deben salir del nivel: `EVENT_CAMERA_COPY_BOUNDS` con `plane: "foreground"`.
- Máximo de franjas: `TBCE_MAX_PARALLAX_LAYERS` = 3 en Retrovania, pero 4 en el `TBCE_Globals.h` compilado de DKAS Demake B001 (que usaba 4 franjas). Parece que el compilador lo ajusta al proyecto; está por confirmar.
- Truco de profundidad sin coste de CPU: líneas del suelo en perspectiva dibujadas con 4 índices de paleta y `EVENT_PALETTE_SET_COLOR` rotando cuál brilla. Las líneas parecen avanzar o retroceder (Retrovania, zona Virtual Boy).

**HUD fijo (plano Window):**
- `EVENT_WINDOW_ENABLE` (`stickY: "top"`, `sizeY: 3`, `sizeX: 0`), como la plantilla `action-platformer`.
- **⚠ `sizeX` y `sizeY` se suman, no se cruzan.** En el VDP la ventana ocupa todas las columnas de las filas indicadas *y* todas las filas de las columnas indicadas. Una barra superior es `sizeX: 0` + `sizeY: n`. Con `sizeX: 40` + `sizeY: 1`, la ventana tapó el plano A completo y el nivel desapareció (comprobado en DKAS Demake B002).
- Texto con `EVENT_DRAW_TEXT` `plane: "window"` y gráficos con `EVENT_IMAGE_LOAD` + `EVENT_IMAGE_DRAW` `plane: "window"`.
- Caben como mucho 6 imágenes cargadas (`TBCE_MAX_IMAGES`). `TBCE_ImageUnload` asume orden de pila: descarga siempre la última que cargaste.
- El índice 0 es transparente en Window: el fondo del HUD debe usar otro índice, aunque sea el mismo color.

**Modo de pantalla por escena:** `"mode":"H40"` (320 px de ancho) o `"H32"` (256 px, el ancho de NES y SNES; lo usa la plantilla `visual-novel`).

**Tamaño:** la plantilla `camera-follow` usa una escena de 192×192 tiles (1.536×1.536 px). Lo que limita es la VRAM de tiles únicos (`vdpBGFG`), no el tamaño en sí, siempre que el arte se repita.

**Cambiar el fondo de la escena en caliente** (leído en `TBCE_Hal.c`, `TBCE_Scroll.inc` y `TBCE_Camera.c` de la 1.0.9; usado en DB Fighter B002/B003 para un zoom por niveles; comprobado en el emulador de MD Engine):
- `EVENT_IMAGE_LOAD` (`TBCE_ImageLoad`) carga el tileset de otra imagen a continuación de lo ya cargado en la zona de fondos y guarda un handle en una variable. Todo lo cargado a la vez tiene que caber en `vdpBGFG`. `EVENT_IMAGE_GET_LAYER_HANDLE` (`plane: "background"`) da el handle de la imagen inicial de la escena.
- `EVENT_IMAGE_REPLACE` (`TBCE_ReplaceBackgroundImage`) solo apunta la capa de la escena a otro handle (para el scroll y los redibujados); **no pinta nada**. Hay que seguir con `EVENT_IMAGE_DRAW` (`plane: "background"`, x/y en tiles).
- `TBCE_ImageDraw` pinta como mucho el ancho del plano (`VDP_WIDTH_TILE` = `planeWidth`, 64 tiles en H40) con `VDP_setTileMapEx` por CPU. Una imagen de hasta 512 px dibujada en (0,0) queda en las mismas columnas que su x de escena, así que la cámara la desplaza bien sin streaming. Si es más ancha, hace falta `EVENT_IMAGE_ENABLE_STREAMING`.
- Al ser por CPU durante el fotograma, puede verse un fotograma a medio pintar (en las capturas de B002 no se ha visto).
- Ojo con la VRAM: si algo más escribe en la zona de fondos, como la precarga de sprites, el fondo sale con basura (ver [formato-proyecto.md](formato-proyecto.md)).
- `EVENT_CAMERA_SET_POSITION` recibe la **esquina superior izquierda** en px (`TBCE_CameraSetPosition` le suma media pantalla). Sin `CAMERA_FOLLOW` no se aplican los límites (`bounds`): hay que limitarla a mano.
- Escalado simulado de sprites con este motor: una tanda de filas de animación por escala en la misma hoja (animación = nivel × nº de animaciones + acción). No hay que cambiar de sprite, y la VRAM reservada es la del fotograma más grande. Ver [../../graficos/scaling-sprites.md](../../graficos/scaling-sprites.md).

- **⚠ La cámara arranca limitada a una pantalla** (DB Fighter B002, comprobado en el emulador leyendo `cameraInformation`). Sin `CAMERA_FOLLOW`, los `bounds` valen 320×224 y `TBCE_CameraUpdate` devuelve la cámara a x = 0 en cada fotograma, aunque `EVENT_CAMERA_SET_POSITION` la mueva. Hay que llamar a `EVENT_CAMERA_SET_BOUNDS` (x, y, width, height en px) con el tamaño de la escena en el `onStart`.

## Notas / gotchas

- Plantillas de referencia: `squarebrothers` (plataformas tipo Mario, usa zona muerta y scroll), `parallax`, `camera-follow` (los 4 estilos), `auto-scrolling`, `hblank-*` (efectos raster).
- El texto con `pinToScreen: true` en el plano `foreground` se mantiene fijo con la cámara (HUD); se ha usado en escenas con scroll sin problemas.
- Por verificar (experimento E4/E5 de Retrovania): coste de CPU del scroll por líneas con muchos sprites, y comportamiento de la zona muerta con `platformer`.

## Referencias cruzadas

- [eventos-y-scripts.md](eventos-y-scripts.md)
- [../../graficos/bec-maps-scrolling.md](../../graficos/bec-maps-scrolling.md) · [../../graficos/bumbershoot-scroll-pantalla-completa-vblank.md](../../graficos/bumbershoot-scroll-pantalla-completa-vblank.md)
