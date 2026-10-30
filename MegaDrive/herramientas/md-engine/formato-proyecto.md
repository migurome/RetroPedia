---
titulo: MD Engine — formato de proyecto (.mdep / .mderes)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [md-engine, formato, json, sprites, colisiones, rle, png-indexado]
estado: borrador
fecha: 2026-10-15
fuentes: [plantillas de MD Engine 1.0.9 (/appData/mde/templates), proyectos generados y compilados (Akane, Carrion, Retrovania)]
---

# MD Engine — formato de proyecto

## Resumen

Un proyecto de MD Engine es una carpeta de JSON (`.mdep` + `.mderes`) y PNG. Todo recurso lleva `_resourceType` y un `id` UUID; las referencias entre recursos van por ese UUID. Si se respeta el formato, se pueden crear proyectos completos sin abrir el editor.

## Detalle técnico

### Árbol

```
MiJuego/
├── MiJuego.mdep                 {"_resourceType":"project","name","author","notes","_version":"1.0.9","_release":"0","_format":"a3f2"}
├── .gitignore                   build/, cache/, *.bak, *.sav, user_settings.*
├── plugins/
├── assets/  backgrounds/ sprites/ tilesets/ fonts/ music/ sounds/ binaries/ localizations/  icon.png
└── project/
    ├── settings.mderes          startSceneId, collisionGroupNames, collisionMask, cart*, vdp*, controles…
    ├── variables.mderes         {"variables":[{"id":"0","name","symbol"}], "constants":[]}
    ├── engine_field_values.mderes {"engineFieldValues":[{"id":"physicsMaxVelocityX","value":110}]}
    ├── sprites/<x>.mderes   backgrounds/<x>.mderes   tilesets/   palettes/   fonts/
    ├── prefabs/actors/<x>.mderes      scripts/<x>.mderes
    └── scenes/<carpeta>/scene.mderes + actors/<x>.mderes + triggers/<x>.mderes
```

Un `settings.mderes` que solo tenga `{"_resourceType":"settings"}` es válido (lo usa la plantilla `blank`): la app rellena los valores por defecto. Para controlar grupos de colisión conviene partir de un `settings.mderes` completo de una plantilla (por ejemplo `top-down`).

### Sprite

```json
{"_resourceType":"sprite","id":"<uuid>","name":"hero","filename":"hero.png","symbol":"sprite_hero",
 "compression":"none","optType":"balanced","animSpeedType":"global","animSpeed":8,
 "width":64,"height":72,"cellWidth":2,"cellHeight":3,
 "boundsX":3,"boundsY":11,"boundsWidth":10,"boundsHeight":13,
 "animNames":["Idle","Walk","Jump"],"animSpeeds":[[]],"checksum":"<sha1 opcional>"}
```

- **Hoja de sprites:** cada **fila es una animación** y cada **columna, un fotograma**. `cellWidth/cellHeight` van en tiles de 8 px (2×3 = 16×24 px).
- **Velocidad de animación** (`animSpeedType`): `global` (un `animSpeed` para todo), `animation` (uno por fila en `animSpeeds`) y `frame` (por fotograma; opción del panel de sprites, sin probar). En DKAS Demake, para respetar duraciones desiguales del original (p. ej. 34/5/10), se repiten fotogramas en la fila con una velocidad base. Para sincronizar un evento con un fotograma concreto (bala con el fogonazo, impacto de una patada), se comprueba la propiedad `frame` del actor (`TBCE_ActorGetFrameIndex`) e `isAnimDone` para saber cuándo termina.
- `bounds*` es la caja de colisión en píxeles, relativa al fotograma.
- **PNG:** 8 bits **indexado** (tipo de color 3) con PLTE de 16 entradas; el **índice 0 es transparente** (las plantillas lo pintan magenta `FF00FF`).

### Fondo (background)

```json
{"_resourceType":"background","id":"<uuid>","name":"lab","type":"image","filename":"lab.png","symbol":"bg_lab",
 "width":80,"height":56,"imageWidth":640,"imageHeight":448,
 "compression":"none","optimization":"all","mode":"map","bgLayer":"","fgLayer":"","streaming":""}
```

- `type: "image"` para PNG; `type: "map"` para TMX de Tiled (con `bgLayer`/`fgLayer`).
- El PNG es indexado de 16 colores: va a la paleta de fondo de la escena (`PAL0`). `optimization: all` deduplica tiles, así que los fondos grandes y repetitivos caben bien (un laboratorio de 640×448 = 45 tiles únicos). Límite: `vdpBGFG` (1.004 por defecto).
- Mapas grandes funcionan: la plantilla `camera-follow` usa 192×192 tiles.

### Escena

```json
{"_resourceType":"scene","id":"<uuid>","_index":0,"name":"Zone 1","symbol":"scene_zone1",
 "backgroundId":"<uuid fondo>","foregroundId":"","midgroundId":"","overlayId":"","tilesetId":"",
 "width":120,"height":28,"mode":"H40","bgColor":0,"manualPalettes":false,
 "backgroundPaletteId":"PAL0","foregroundPaletteId":"PAL1","paletteIds":[],
 "collisions":"<RLE>","x":100,"y":100,"visible":true,
 "onStartScript":[…],"onUpdateScript":[…],"onEndScript":[],"onPauseScript":[]}
```

`width/height` van en tiles de 8 px; `x/y` es la posición en el lienzo del editor.

### Mapa de colisiones (`collisions`, RLE)

Recorre los tiles de colisión (`collisionTileSize`, 8 px) fila a fila. Cada tramo es:

- `<valor 8 hex><cantidad hex>+` para una racha de N tiles, o
- `<valor 8 hex>!` para un tile suelto.

`00000000` = libre, `0000000f` = sólido por los 4 lados. Una cadena vacía `""` significa escena sin colisiones. Ejemplo: `00000000460+` = 1.120 tiles libres (40×28).

```js
// codificador (Node)
function rle(tiles) { // tiles: array de '0000000f' | '00000000'
  let out = '';
  for (let i = 0; i < tiles.length;) { let j = i; while (j < tiles.length && tiles[j] === tiles[i]) j++;
    out += tiles[i] + (j - i === 1 ? '!' : (j - i).toString(16) + '+'); i = j; }
  return out;
}
```

### Actor

```json
{"_resourceType":"actor","id":"<uuid>","name":"Hero","prefabId":"","spriteSheetId":"<uuid>","paletteId":"PAL2",
 "frame":0,"direction":"right","isPlayer":false,"isPinned":false,"flipX":false,"flipY":false,"priority":false,
 "preloadFrames":true,"persistent":true,"collisionGroup":"1","updateMode":"parallel","disableSceneCollision":false,
 "prefabScriptOverrides":{},"symbol":"actor_hero","x":32,"y":184,"_index":0,
 "onStartScript":[…],"onUpdateScript":[…],"onEndScript":[],"hit1Script":[],…,"hit8Script":[]}
```

- `collisionGroup` "1".."8" o "" (sin grupo). `hitNScript` se ejecuta al solaparse con un actor del grupo N.
- `paletteId`: `AUTO` o una ranura fija `PAL0..PAL3`. Si varios sprites comparten ranura, conviene que sus PNG tengan **la misma paleta**.
- `symbol` debe ser único en el proyecto.

### Fuente en modo de texto simple (`textMode`)

- Un carácter por tile de 8x8. **El PNG ya va en el orden de los tiles**, que no es ASCII puro (`VDP_drawTextEx` en `TBCE_Hal.c`):
  - **0:** espacio.
  - **1–31:** ASCII 33–63.
  - **32–60:** ASCII 65–93, sin `@`.
  - **61–86:** ASCII 97–122, sin `^ _ ``.
  - **87–95:** 9 piezas del marco y el cursor de los diálogos. El texto no puede llegar a ellas.
  - **96–117** (solo en `SimpleEFIGSP`): `áéíóúüñäöàâêôãõçèùîû¿¡`. La fuente pasa a 118 tiles: hay que subir `vdpFont` a 118 y bajar 22 tiles de `vdpBGFG` (o de sprites), porque el total es fijo (1.520).
  - `@ ^ _ `` y los caracteres a partir de `{` se dibujan como espacio.
- **No hay mayúsculas acentuadas** en EFIGSP: los textos van en mayúsculas y minúsculas, o se cambia la letra acentuada por la normal.
- **Signos que no se escriben = iconos:** cualquier casilla de un signo que el juego nunca escriba (`# % & * + = ( ) [ ] ; < > " \`) puede guardar un icono o una pieza de marco. Se dibuja con `EVENT_DRAW_TEXT` como si fuera texto. `$` no sirve: marca las variables (`$05$`).
- **Modo avanzado** (fuentes proporcionales de hasta 16 px): reparte los tiles de la fuente en anillo (`TBCE_AdvanceText.c`). Un texto que se redibuja a menudo, como un marcador, acaba pisando otros que siguen en pantalla. Para un HUD conviene el modo simple.
- Los glifos van en un solo índice de la paleta (o en varios, para iconos de colores). El color se cambia por línea con `EVENT_SET_TEXT_PALETTE` (ver [eventos-y-scripts.md](eventos-y-scripts.md)).

## Notas / gotchas

- **Paletas de escena:** el compilador llena las 4 ranuras en orden: PAL0 el fondo, PAL1 el primer plano y luego las paletas de los sprites, por orden de aparición de los actores. Si una paleta coincide con una ya cargada, no ocupa ranura. Un prop recoloreado con la paleta de **otro** nivel (por ejemplo, un archivo de sprite compartido entre escenas y sobrescrito) ocupa PAL3 y deja sin color a los sprites que la esperaban. Cada escena necesita su propia hoja recoloreada.
- **VRAM:** `vdpBGFG` + `vdpSprites` + `vdpFont` = 1.520 tiles. Un sprite reserva la VRAM de su fotograma **más grande**: una animación ocasional enorme (una puerta que revienta) conviene ponerla en otra hoja y cambiarla con `EVENT_ACTOR_SET_SPRITE` solo cuando haga falta.
- **Fondos en modo mapa:** rescomp deduplica tiles contando los volteos. Un fondo en parallax de 512 px puede quedarse con su mitad izquierda y el espejo de esta: encaja sin costura y ocupa casi la mitad de tiles.

- **Tipos de proyectil:** el compilador admite 5 por defecto. Para usar más ranuras en `EVENT_PROJECTILE_PRELOAD`, hay que poner `maxProjectileSlots` en `settings.mderes` (en el editor, Ajustes > Elementos máximos > Tipos de proyectil máx.). El motor llega a 16. Error si falta: «usa el slot de proyectil 6, pero el proyecto permite 5».

- Los `id` pueden ser cualquier UUID (las plantillas `endless-runner` incluso usan ids "forjados"); lo importante es que las referencias cuadren.
- `width/height` del fondo y de la escena deben coincidir en tiles.
- Las paletas de fondo solo tienen 16 colores: un PNG con más no entra en una sola paleta.

- **⚠ `preloadFrames: true` en un actor con una hoja grande destroza el fondo** (DB Fighter B002, leído en `TBCE_SpritePreloadFrames`, `TBCE_Hal.c`). La escena copia **todos** los fotogramas de la hoja en una zona de VRAM reservada justo **antes** de la de sprites, es decir, al final de `vdpBGFG`, encima de los tiles de los fondos. Con 2 hojas de 9 filas de 9×13 tiles, el fondo se llenó de basura (con letras de la fuente) y el segundo actor no llegó a aparecer. Con `false`, SGDK reserva solo el fotograma más grande y sube cada fotograma por DMA al cambiar. La precarga solo compensa con hojas pequeñas y cambios de fotograma muy frecuentes.

## Referencias cruzadas

- [eventos-y-scripts.md](eventos-y-scripts.md) · [fisica-y-colisiones.md](fisica-y-colisiones.md) · [depuracion-y-generacion.md](depuracion-y-generacion.md)
- [../../graficos/tiles-and-palettes.md](../../graficos/tiles-and-palettes.md)
