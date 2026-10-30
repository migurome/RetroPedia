---
titulo: MD Engine — física, grupos de colisión y gotchas de hitbox
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [md-engine, fisica, colisiones, hitbox, plataformas, gravedad, bug-conocido]
estado: borrador
fecha: 2026-10-15
fuentes: [TBCE_Physics.c, TBCE_Actor.h/.c, TBCE_Collisions_Advanced.c y TBCE_Collisions_Simple.c de MD Engine 1.0.9; C generado de la escena]
---

# MD Engine — física y colisiones

## Resumen

La física de TBCE es simple (velocidad + aceleración + deceleración, en px/s), y la gravedad solo afecta a actores en modo plataformas o con aceleración Y explícita. Las colisiones entre actores son por **solapamiento de cajas** y disparan `hitN`. Solo empujan si los dos actores son sólidos. La trampa principal: la **caja de colisión solo se actualiza en el paso de movimiento**.

## Detalle técnico

### Física (`TBCE_ActorUpdateMotion`)

Solo se aplica si el actor está activo y tiene `FLAG_MOVES` (`EVENT_ACTOR_ENABLE_MOVEMENT`):

```c
velocityX = computeVelocityX(velocityX, accelerationX);  // si accel != 0: suma y limita a ±maxVelX
pX += velocityX; x = ...;                                // si accel == 0 y decelX != 0: frena hacia 0
velocityY = computeVelocityY(velocityY, accelerationY);
...
TBCE_ActorUpdateHitbox(actor);                            // <-- único sitio habitual donde se recalcula
```

- Las velocidades de los eventos van en **px/s** (`TBCE_PixelPerSecToFF16`).
- **Campos de motor** (`engine.json`, valores por defecto): `physicsMaxVelocityX` 140, `physicsMaxVelocityY` 290, `physicsAcceleration` 1120, `physicsDecelerationX` 1120, `physicsDecelerationY` 0, `physicsGravity` 1150, `physicsJump` 350.
- **Gravedad:** `TBCE_ActorEnablePlatformer` pone `accelerationY = physicsGravity`. Un actor normal no cae, salvo que se le dé `EVENT_ACTOR_SET_ACCELERATION_Y` con `{"type":"engineField","value":"physicsGravity"}`.
- **Altura de salto aproximada:** v² / (2g). Con v = 330 y g = 1150, unos 47 px. Ojo: con aceleración ≠ 0, la velocidad Y se limita a `±physicsMaxVelocityY`, así que un salto de 330 se recorta si la velocidad máxima Y es 290.
- **Deceleración X:** con aceleración 0 frena a **cualquier** actor que se mueva por velocidad. Los objetos que deben ir a velocidad constante (balas, enemigos) tienen que fijar la velocidad en cada fotograma, o hay que poner `physicsDecelerationX = 0`.

### Grupos y máscaras

- `settings.collisionGroupNames[8]` y `settings.collisionMask[8]`: la máscara del grupo i lista los grupos con los que colisiona (`[["2","3"],["1"],…]`). El compilador genera `TBCE_CollisionMaskTable`.
- En la escena compilada, por cada grupo hay llamadas `TBCE_ActorCheckCollisions(actor, actoresDelGrupo, n)`, que comprueban el solape de cajas.
- `TBCE_ActorCheckCollisionOtherActor`: si se solapan, **y si los dos son sólidos** (activo + `COLLISIONS_ENABLE`), los separa (`separateX/Y`, respetando `IMMOVABLE`). Después llama al `hitScript` de **cada** actor con `TBCE_ActorIsOnCollideEnabled` (se desactiva con `EVENT_ACTOR_ON_COLLIDE_DISABLE`).
- El script de golpe de un actor se ejecuta **aunque el otro tenga su on-collide desactivado**. Un enemigo "muriéndose" que sigue en contacto puede matar al jugador: hay que sacarlo de pantalla o comprobar su estado.
- Estar de pie sobre un actor sólido marca `touching |= DIR_DOWN`, así que `isTouchingFloor` es cierto sobre plataformas móviles.
- `overlapType: Continuous` hace que `hitN` se repita cada fotograma mientras haya solape: protege con una variable de estado.
- **Modo Advanced vs Simple:** los proyectos compilados con `collisionType: AllActors` generaron `TBCE_COLLISIONS_ADVANCED`. En ese modo `TBCE_ActorCheckCollisions` solo exige que el actor esté **activo** (la condición de "en pantalla" está comentada en el código). Un actor aparcado fuera de pantalla sigue colisionando con otro aparcado en el mismo punto. Para sacarlo de verdad: `EVENT_ACTOR_DEACTIVATE`, o comprobar su estado en el script de golpe. En el modo Simple con rejilla, en cambio, solo cuentan los actores con `FLAG_ACTIVE | FLAG_ON_SCREEN`.

### ⚠ Bug/gotcha confirmado: hitbox que no sigue al actor

`EVENT_ACTOR_SET_POSITION` compila a `TBCE_ActorTeleportXY`, que cambia `x/y` y `pX/pY` pero **no** llama a `TBCE_ActorUpdateHitbox`. La caja solo se recalcula en `TBCE_ActorUpdateMotion`, que solo se ejecuta para actores en la lista `movingActors`, es decir, con `ENABLE_MOVEMENT`.

**Síntoma:** un efecto (tajo de espada, hitbox de ataque) colocado por script se ve en su sitio, pero colisiona donde estaba al principio y nunca toca nada.

**Solución:** `EVENT_ACTOR_ENABLE_MOVEMENT` en su `onStart`, aunque su velocidad sea 0. Así entra en `movingActors` y su caja se actualiza cada fotograma.

### Colisión con el escenario

Mapa RLE de la escena (ver [formato-proyecto.md](formato-proyecto.md)); solo afecta a actores sólidos (`COLLISIONS_ENABLE`) con `disableSceneCollision: false`. **Fuera del mapa no hay colisión:** sin una fila sólida arriba, el jugador sale de la pantalla al saltar alto. En salas abiertas, como las de estilo Pong, limítalo por script: si `pypos < 0`, `SET_POSITION` a y = 0 y anular `vely` negativa; igual para `pxpos` en los laterales sin pared. `isTouchingLeft/Right/Floor/Up` sirven para patrullas (darse la vuelta al chocar) y para el salto en pared.

**Valor de cada tile de colisión** (byte bajo del valor hex de 8 cifras; `TBCE_Actor.h`, `TBCE_Collisions_Advanced.c`):

| Bits | Significado |
|---|---|
| 0–3 | Lados sólidos: `DIR_UP` 0x1, `DIR_DOWN` 0x2, `DIR_LEFT` 0x4, `DIR_RIGHT` 0x8 → `0000000f` sólido por los 4 lados |
| 4 | `TILE_LADDER` 0x10 → `00000010` escalera (`EVENT_IF_LADDER_AT_POSITION`, `TBCE_SceneLadderOnTile`) |
| 5–7 | Tipo de tile / pendientes |

Un tile `00000001` (solo `DIR_UP`) es una **plataforma de un sentido**: se atraviesa desde abajo y se pisa desde arriba (el código trata aparte el caso `(tile & DIR_ANY) == DIR_UP`). *(Leído en el código fuente de la 1.0.9, falta probarlo en una ROM.)*

**⚠ Cambiar colisiones en tiempo de ejecución exige `"collisionStore": "OnRAM"`** en `settings.mderes`. Por defecto el mapa se compila como `static const u8 scene_x_collisionMap[]` (ROM), y `EVENT_SCENE_SET_COLLISIONS` / `EVENT_SCENE_SET_TILE_TYPE` (que acaban en `TBCE_CollisionMapAndAt`, un `&=` sobre `collisionMap`) no tienen efecto. Síntoma en DKAS Demake B002: la puerta cambiaba de sprite al abrirse, pero seguía bloqueando el paso. Las plantillas traen `OnRAM`; un `settings.mderes` mínimo, no.

## Notas / gotchas

- **Balas contra actores** (`TBCE_Projectiles.c`): al solaparse, el proyectil llama al `hitScript` del actor con `lastCollisionGroup` igual al grupo del proyectil (se ejecuta `hitN`, con N el grupo de la bala), siempre que el actor tenga `ON_COLLIDE_ENABLE` y esté en pantalla. Dentro del script, `EVENT_IF_ACTOR_OVERLAP_PROJECTILE` con `typeSlot` distingue qué tipo de bala fue (en DKAS Demake: la normal quita 20 y la apuntada, 150). No hace falta un grupo de colisión por tipo de bala.
- **⚠ Una bala con `destroyOnHit` se destruye al tocar a cualquier actor de su lista** (los grupos de su máscara, en pantalla), **aunque ese actor tenga el «on collide» desactivado**: solo se salta su script de golpe. Un enemigo muerto con su caja de pie se convierte en un escudo invisible (DKAS Demake B012: los cadáveres junto a las puertas se tragaban los disparos). Al morir, reduce la caja con `EVENT_ACTOR_SET_BOUNDING_BOX` (`TBCE_ActorSetBoundingBox`: desplazamiento y tamaño respecto a x/y del actor; por ejemplo 1×1 a ras de suelo) o desactiva el actor si no tiene que quedar el cadáver.
- **Disparos a quemarropa:** en cada fotograma, la escena mueve los proyectiles (`TBCE_ProjectileUpdateMotion`) **antes** de comprobar los impactos. Por eso una bala recién lanzada ya ha avanzado su velocidad completa (7 px a 420 px/s) cuando se mira si toca a alguien. Si nace en la boca del arma, un enemigo pegado al tirador queda por detrás y no recibe el impacto (DKAS Demake B013). Solución: lanzarla desde el centro del cuerpo del tirador. Además, la velocidad por fotograma tiene que ser menor que el ancho de las cajas, para que la bala no se salte a nadie.
- **`physicsMaxVelocityX` es global**: limita a todos los actores con plataformas, enemigos incluidos. Para enemigos con velocidades propias, en DKAS Demake se mueven por script con `EVENT_ACTOR_SET_POSITION_RELATIVE` (`units: "pixels"`; 1 px cada 2 fotogramas = 30 px/s) dentro de límites calculados al generar, y sin física de escenario. Necesitan `ENABLE_MOVEMENT` para que su caja siga al sprite.
- `EVENT_ACTOR_FLICKER` no bloquea (solo fija un contador de fotogramas): se puede usar en un script de golpe sin volverlo asíncrono.

- Patrón de **doble salto**: una local con los saltos aéreos que se recarga con `isTouchingFloor`; el salto es `EVENT_ACTOR_SET_VELOCITY_Y` negativo, tanto en el suelo como en el aire.
- Patrón de **plataforma móvil**: actor sólido + `IMMOVABLE` + **`EVENT_ACTOR_SET_COLLISIONS` solo con `up`** (plataforma de un sentido, como en las plantillas `squarebrothers` e `inkventure`) + velocidad Y que cambia de signo entre dos límites (script), con `disableSceneCollision: true`. Si es sólida por los 4 lados, `separateX` también actúa y el héroe se engancha en los laterales al subir o al caminar sobre los bordes (comprobado en Retrovania, build B001).
- **`isTouchingFloor` parpadea sobre actores:** el contacto con otro actor solo se marca en los fotogramas en que las cajas se solapan. En una plataforma que baja, el héroe cae, toca y se separa una y otra vez, y la animación alterna entre suelo y salto. Solución: un contador de "coyote time" (fotogramas desde el último suelo) y considerarse en el suelo mientras sea menor que 3–4. Así también se gana margen para saltar al borde de una cornisa.
- Patrón de **pool de enemigos**: N actores colocados que se aparcan fuera de pantalla y se recolocan al "aparecer".

## Referencias cruzadas

- [eventos-y-scripts.md](eventos-y-scripts.md) · [depuracion-y-generacion.md](depuracion-y-generacion.md)
- [../../../GLOSARIO/speed-and-gravity.md](../../../GLOSARIO/speed-and-gravity.md) · [../../../GLOSARIO/basic-collision.md](../../../GLOSARIO/basic-collision.md)
