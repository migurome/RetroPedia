---
titulo: SGDK — Tutorial del motor de Sprites
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, tutorial, sprites, spr, vdp, recursos, animacion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/Tuto-Sprites]
---

# SGDK — Tutorial del motor de Sprites

## Resumen

Cómo gestionar sprites en SGDK mediante el "Sprite Engine" de alto nivel (recomendado para casi todos los casos) y, alternativamente, mediante la API de bajo nivel `VDP_spr` para escenarios de rendimiento crítico con muchos sprites pequeños.

## Detalle técnico

### API de alto nivel: Sprite Engine

Usa internamente el recurso `SPRITE`, compilado como `SpriteDefinition`. Según la documentación, es "mucho más fácil de usar que la API de bajo nivel y debería funcionar para casi cualquier situación".

**Declaración de recurso:**
```
SPRITE sonic_sprite "sprite/sonic.png" 6 6 FAST 5
```

Los parámetros especifican: imagen de la hoja de sprites, ancho/alto en tiles, tipo de compresión y un valor de timing. La documentación desaconseja usar compresión `BEST` para sprites que se transmiten (streamed) por rendimiento, y recomienda `FAST` solo cuando sea necesario.

**Flujo de trabajo principal:**

Inicialización y creación:
```c
SPR_init();
Sprite* player = SPR_addSprite(&sonic_sprite,
    fix32ToInt(posX) - camPosX,
    fix32ToInt(posY) - camPosY,
    TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
```

Control de posición y animación:
- `SPR_setPosition(Sprite* sprite, s16 x, s16 y)`
- `SPR_setHFlip(Sprite* sprite, u16 value)`
- `SPR_setVisibility(Sprite* sprite, SpriteVisibility value)`
- `SPR_setAnimAndFrame(Sprite* sprite, s16 anim, s16 frame)`
- `SPR_setFrame(Sprite* sprite, s16 frame)`
- `SPR_nextFrame(Sprite* sprite)`

Configuración avanzada:
- `SPR_setVRAMTileIndex(Sprite* sprite, s16 value)`
- `SPR_setSpriteTableIndex(Sprite* sprite, s16 value)`
- `SPR_setAutoTileUpload(Sprite* sprite, bool value)`
- `SPR_setDelayedFrameUpdate(Sprite* sprite, bool value)`
- `SPR_setFrameChangeCallback(Sprite* sprite, FrameChangeCallback* callback)`

Aplicar cambios:
```c
SPR_update();
SPR_releaseSprite(sprite);
```

### API de bajo nivel: VDP_spr

Para escenarios de rendimiento crítico con muchos sprites pequeños, la API de bajo nivel da control directo. Consideraciones clave: el plano de sprites mide 512x512, la ventana de visibilidad empieza en (128,128), y los límites máximos son 80 sprites por frame y 20 sprites por línea de barrido en modo PAL.

**Definición básica de sprite:**
```c
_spritedef mySprite;
mySprite.posx = 0;
mySprite.posy = 0;
mySprite.size = SPRITE_SIZE(2,2);
mySprite.tile_attr = TILE_ATTR_FULL(PAL0,1,0,0,TILE1);
mySprite.link = 0;
VDP_setSpriteP(0, &mySprite);
```

La documentación remarca que "un mal uso de la propiedad `link` es la principal causa de bugs" — la cadena debe construirse sin ciclos y terminar siempre de vuelta al sprite 0.

### Animaciones

El recurso `SPRITE` organiza los datos de animación en formato de hoja, donde cada fila representa una secuencia de animación y cada celda un frame individual. Patrón de acceso: `sonic.animations[0]->frames[6]` para la animación 0, frame 6.

## Notas / gotchas

- Límites duros de hardware (80 sprites/frame, 20 sprites/línea en PAL) aplican igual que en el VDP crudo — ver [`../../graficos/sprites.md`](../../graficos/sprites.md) (fuente plutiedev.com) para el detalle a nivel de hardware.
- El campo `link` de la tabla de sprites es la fuente más común de bugs si se gestiona mal manualmente con la API de bajo nivel.
- **rescomp quita los frames vacíos del final de cada fila** (los del medio los conserva). Si el código cree que la animación tiene N frames y la última celda es transparente, en la ROM solo hay N-1. En la versión release, `SPR_setAnimAndFrame` no comprueba rangos: con el frame que falta lee `animations[a]->frames[N-1]` más allá del array. Ese puntero basura llega a `SPR_update`, que en la carga automática de tiles lee `frame->tileset`. Si la dirección es impar, el 68000 da *address error* (pantalla `ADDRESS ERROR` de SGDK con el PC dentro de `SPR_update`). Si es par, no se nota, y el fallo aparece o desaparece al cambiar la disposición de la ROM.
  - **Caso real (Super Smash Land MD, B047):** la explosión de la Crash Bomb tenía 14 frames en el original, con el último vacío. Colgaba BlastEm en cuanto explotaba una bomba; en B046 el puntero basura caía par.
  - **Prevención:** el generador de recursos calcula cuántos frames quedan en la ROM (hasta el último no vacío) y da error si una fila con cola vacía no está tratada. El código oculta el sprite en esos frames en vez de pedírselos a SGDK.
  - **Detección:** una envoltura de `SPR_setAnimAndFrame` que compara con `s->definition->numAnimation` y `animations[a]->numFrame`, apunta el primer caso en una variable global y lo ignora. Las pruebas automáticas leen esa variable de la RAM. Los emuladores tolerantes, como el WASM de MD Engine, no avisan del acceso impar.
    - La envoltura conviene compilarla solo en una variante de prueba (`EXTRA_FLAGS=-DSSB_ANIM_CHECK` en `makefile.gen`). En un juego que va justo de CPU (4 luchadores, unas 30 llamadas por fotograma), las tres lecturas encadenadas costaban un 3-4 % de fotogramas perdidos.
- **Los sprites ocultos también cuestan.** `SPR_update` recorre en cada fotograma todos los sprites reservados con `SPR_addSprite`, también los que están en `HIDDEN`. En un juego que va justo de CPU, reservar de más se nota: en Super Smash Land MD, 5 piezas de menú ocultas durante el combate costaban con 4 CPU un 1,5 % de fotogramas perdidos.
  - Los sprites que solo se usan en una pantalla (menús) o en un modo (las etiquetas de equipo) se crean con `SPR_addSprite` al mostrarse y se sueltan con `SPR_releaseSprite` al ocultarse.
  - Al crearlo de nuevo hay que repetir su configuración: `SPR_setAutoAnimation`, paleta y profundidad. La profundidad por defecto es `SPR_MAX_DEPTH`, al final de la lista; `SPR_setDepth` reordena, y menor profundidad es más delante.
- **Mirar siempre lo que devuelve `SPR_setDefinition`.** Si no hay VRAM para la hoja nueva, SGDK (2.11):
  1. suelta la VRAM que tenía el sprite;
  2. le deja la hoja **vieja**;
  3. devuelve FALSE.

  Si el código sigue y pide una animación o frame de la hoja nueva, `SPR_setAnimAndFrame` lee fuera de la vieja y `SPR_update` acaba en `ADDRESS ERROR`. Además, el sprite apunta a VRAM ya libre.
  - **Qué hacer:** con FALSE, ocultar el sprite y no volver a animarlo hasta que una hoja se ponga bien.
  - **Mejor aún, que no falte VRAM:** los sprites que no siempre se ven (proyectiles, cuenta atrás) se crean al mostrarse y se sueltan al ocultarse. Reservados todos, cada uno ocupa la VRAM de su frame más grande aunque esté oculto.
  - **Caso real (Super Smash Land MD, B053 a 2x):** 10 proyectiles de 29 tiles más una cuenta atrás de 117 se comían 400 de los 700 tiles de sprites, y la hoja de un luchador no cabía. Se colgaba en la FPGA.
- **Dar profundidad a todo sprite que deba ir delante de otros.** Si unos sprites tienen `SPR_setDepth` y otros no, los que no la tienen se quedan con la de por defecto, `SPR_MAX_DEPTH`, que es la de más atrás. Cualquier profundidad explícita queda delante de ellos.
  - **Caso real (Super Smash Land MD, B056):** las piezas del escenario (cara del árbol de Dreamland, luna de Clock Town) iban a `100 + ranura` para quedar «detrás». Los luchadores nunca recibieron profundidad, así que pasaban por detrás de las piezas. Se arregló poniendo los luchadores a 0 y los proyectiles a 1.
  - **Regla:** reservar rangos para cada capa (delante < 0, luchadores 0, proyectiles 1, piezas de fondo 100+) y asignar la profundidad al crear el sprite, también al volver a crearlo tras `SPR_releaseSprite`.
- **rescomp no admite más de 16 sprites de hardware por frame** (`Sprite frame '…' uses 32 internal sprites, that is above the limit (16)`). Un frame de 176x176 (6x6 bloques de 32x32) no compila. Hay que partirlo en varios sprites de SGDK o dejarlo más pequeño.
  - Pasó en Super Smash Land MD al escalar a 2x la luna de Clock Town.
- **Escalar pixel art a 1,5x por vecino más cercano:** unos píxeles salen dobles y otros sencillos.
  - **Que no tiemble al animar:** el reparto tiene que contarse desde el **origen del sprite** y no desde la esquina de cada frame. Si no, el mismo píxel sale doble en un frame y sencillo en el siguiente.
  - **Sin cambiar la lógica:** el juego sigue en sus unidades y solo cambia la conversión a píxeles de pantalla. A 1,5x, desde unidades de 4 por píxel del original, es `(v * 3) >> 3`.
  - **Coste en Super Smash Land MD** (prototipo B053), frente a 1x:
    - los fondos piden el doble de tiles (el peor pasó de 293 a 586);
    - con 4 luchadores, los fotogramas perdidos pasaron del 7-10 % al 19-21 %, por los sprites de hardware de más que mueve `SPR_update`;
    - con 2 jugadores, 0 %.
- **Diagnosticar un `ADDRESS ERROR` de SGDK:** el `Offset` es el PC. Se busca en `out/symbol.txt` (el símbolo con la dirección inmediatamente inferior) y se desensambla con `objdump -d --start-address=… out/rom.out`. Las palabras de la pila que parecen direcciones de código (`0001 6B24`…) dan los llamantes. Los registros que apuntan a símbolos `*_animationN_frameM_tileset` indican qué hoja se estaba procesando.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Backgrounds](tuto-background.md)
- [Tutorial: Input](tuto-input.md)
- Comparar con la tabla de sprites "cruda" del VDP en [`../../graficos/sprite-table.md`](../../graficos/sprite-table.md) y [`../../graficos/sprites.md`](../../graficos/sprites.md) (fuente plutiedev.com)
