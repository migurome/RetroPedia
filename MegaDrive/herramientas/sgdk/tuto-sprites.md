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

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Backgrounds](tuto-background.md)
- [Tutorial: Input](tuto-input.md)
- Comparar con la tabla de sprites "cruda" del VDP en [`../../graficos/sprite-table.md`](../../graficos/sprite-table.md) y [`../../graficos/sprites.md`](../../graficos/sprites.md) (fuente plutiedev.com)
