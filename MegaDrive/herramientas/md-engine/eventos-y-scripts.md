---
titulo: MD Engine — eventos, valores y variables en los scripts
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [md-engine, eventos, scripts, variables, expresiones, json]
estado: borrador
fecha: 2026-10-15
actualizado: 2026-10-22
fuentes: [/src/lib/events/*.js de MD Engine 1.0.9, plantillas, C generado en %TEMP%/_MDEngine/src, port de Super Smash Land (MD Engine 1.1.0)]
---

# MD Engine — eventos, valores y variables

## Resumen

Los scripts son listas JSON de eventos. Cada evento tiene `command` (su id), `args` e `id` (UUID); los que bifurcan llevan `children`. Los valores son árboles de expresión (`number`, `variable`, `property`, operadores). Conocer esta forma permite escribir lógica de juego por código.

## Detalle técnico

### Forma de un evento

```json
{"command":"EVENT_IF","id":"<uuid>",
 "args":{"condition":{"type":"gt","valueA":{"type":"variable","value":"L0"},"valueB":{"type":"number","value":0}},"__disableElse":false},
 "children":{"true":[ … ],"false":[ … ]}}
```

- `EVENT_SWITCH`: `args.variable`, `choices`, `value0..valueN` (valores `number`) y `children.true0..trueN` + `false` (rama por defecto).
- Entrada: `EVENT_IF_INPUT_PRESSED` / `EVENT_IF_INPUT_JUST_PRESSED` con `input: ["left","right","up","down","a","b","c","x","y","z","start","mode"]` y `playerIndex: 1`.
- Las claves `__collapse*`, `__section` y `__comment` son solo del editor; se pueden omitir.

### Valores (árbol de expresión)

| `type` | Forma |
|---|---|
| `number` | `{"type":"number","value":110}` |
| `variable` | `{"type":"variable","value":"3"}` (global), `"L0"` (local), `"T0"` (temporal) |
| `property` | `{"type":"property","target":"$self$" o "<uuid actor>","property":"pxpos"}` |
| `true` / `false` | `{"type":"true"}` |
| binarios | `add sub mul div mod min max eq ne lt lte gt gte and or` con `valueA`/`valueB` |
| `not` | `{"type":"not","value":…}` |
| `rnd` | `{"type":"rnd","value":{"type":"number","value":60}}`: compila a `TBCE_RandomRange(0, 60)` |
| `engineField` | `{"type":"engineField","value":"physicsGravity"}` |
| `expression` | texto: `"$L2$ - 1"`, `"-$89$"`, `"-#physicsJump#"` (`$id$` variable, `#campo#` campo de motor) |

**Propiedades de actor usadas en las plantillas:** `pxpos`, `pypos` (posición en píxeles, esquina superior izquierda), `pxpos_center`, `pypos_center`, `pypos_bottom`, `xpos`/`ypos` (en tiles), `px_width`, `px_height`, `velx`, `vely`, `accx`, `isTouchingFloor`, `isTouchingLeft`, `isTouchingRight`, `isTouchingUp`, `curAnimIdx`, `isAnimDone`, `frame`.

### Variables

- **Globales:** `variables.mderes` les da id numérico ("0", "1"…). Sobreviven a los cambios de escena y a `EVENT_SCENE_RESTART`, así que sirven para el progreso entre escenas. En los textos se insertan con `$id$` a dos cifras: `"KILLS $00$"` es la variable 0. **⚠ El valor no se rellena con ceros:** 9 se escribe «9», no «09». Al redibujar un texto en el mismo sitio, si el número pierde una cifra, el último carácter del texto anterior se queda en pantalla («10/30» → «9/300», DKAS Demake B005). Arreglo: un `EVENT_IF` que, para valores menores de 10, dibuje una variante con un espacio delante del número, o terminar el texto con espacios.
- **Locales de actor:** `L0`, `L1`… (las plantillas usan hasta L5). Viven en `__self->localVars[n]`.
- **Temporales:** `T0..T2`, válidas dentro del script en curso.

### Scripts de actor y de escena

- `onStartScript`: al cargar. `onUpdateScript`: cada fotograma (`updateMode: "parallel"`). `onEndScript`.
- `hit1Script..hit8Script`: el compilador los une en una función `actor_x_hit` que mira `TBCE_ActorGetLastCollisionGroup(self) == CGROUP_N`.
- Un script con esperas (`EVENT_WAIT`, `FADE … wait:true`, `CAMERA_SHAKE`) se compila como corrutina (`ctx->pc`, `return false` hasta terminar).
- `EVENT_STOP` termina el script en ese fotograma: patrón "si está muerto, no hagas nada".

### Ids de evento verificados (nombre de archivo ≠ id)

| Archivo | `command` |
|---|---|
| eventTextDraw.js | `EVENT_DRAW_TEXT` (text, x, y en tiles, pinToScreen, alignment, plane) |
| eventTextClear.js | `EVENT_CLEAR_TEXT` |
| eventTextSetPalette.js | `EVENT_SET_TEXT_PALETTE` (paletteIdx) |
| eventSceneSwitch.js | `EVENT_SWITCH_SCENE` (sceneId, setPosition, x, y, direction, flipX, flipY) |
| eventSceneRestart.js | `EVENT_SCENE_RESTART` |
| eventVariableSetToValue.js | `EVENT_SET_VALUE` |
| eventVariableInc/Dec.js | `EVENT_INC_VALUE` / `EVENT_DEC_VALUE` |
| eventScriptStop.js | `EVENT_STOP` |
| eventIfActorAnimationDone.js | `EVENT_IF_ACTOR_ANIM_DONE` |
| eventDataSave/Load.js | `EVENT_SAVE_DATA` / `EVENT_LOAD_DATA` |

Para listar todos: cargar cada `events/*.js` en Node con un `require` falso de `helpers/l10n` y leer `module.exports.id` y `fields` (ver [depuracion-y-generacion.md](depuracion-y-generacion.md)).

**Otros eventos útiles:** `EVENT_ACTOR_SET_POSITION`, `EVENT_ACTOR_SET_VELOCITY_X/Y`, `EVENT_ACTOR_SET_ACCELERATION_X/Y`, `EVENT_ACTOR_ENABLE_MOVEMENT`, `EVENT_ACTOR_COLLISIONS_ENABLE/DISABLE`, `EVENT_ACTOR_ON_COLLIDE_DISABLE`, `EVENT_ACTOR_IMMOVABLE`, `EVENT_ACTOR_SET_ANIMATION` (animation como string con el índice de fila), `EVENT_ACTOR_FLIP_X`, `EVENT_ACTOR_FLICKER`, `EVENT_ACTOR_HIDE/SHOW`, `EVENT_ACTOR_ACTIVATE/DEACTIVATE`, `EVENT_ACTOR_SEND_TO_BACK`, `EVENT_PLATFORMER_ENABLE`, `EVENT_PLATFORMER_SIMULATE_CONTROL` (left/right/jump), `EVENT_PROJECTILE_PRELOAD` / `EVENT_PROJECTILE_LAUNCH`, `EVENT_CAMERA_SHAKE`, `EVENT_CAMERA_FLASH`, `EVENT_FADE_IN/OUT`, `EVENT_ENGINE_FIELD_SET`, `EVENT_COLLISIONS_ENABLE/DISABLE_GROUP`, `EVENT_PALETTE_LOAD`.

## Notas / gotchas

- `EVENT_ACTOR_SET_ANIMATION` reinicia la animación: llamarlo en cada fotograma la deja congelada en el primer fotograma. Guarda la animación actual en una local y cámbiala solo cuando difiera. En cambio, `EVENT_ACTOR_SET_ANIMATION_BY_INDEX` acaba en `TBCE_SpriteSetAnimation` (`TBCE_Hal.h`), que solo llama a `setAnim` si `animInd` cambia. Por eso **sí se puede llamar en cada fotograma**, y así lo hace la plantilla `action-platformer`.
- Para crear actores dinámicamente no hay "spawn": se colocan N actores en la escena (pool) y se activan, desactivan o mueven fuera de pantalla.
- `CAMERA_SHAKE` en un script de golpe lo convierte en asíncrono (espera la duración); protege con una variable para que no se dispare varias veces.
- **Orden de dibujo entre actores:** por defecto un prop (puerta, portal) se puede pintar encima del jugador. `EVENT_ACTOR_SEND_TO_BACK` / `SEND_TO_FRONT` en el `onStart` fijan la profundidad (`TBCE_SpriteSetDepth` con `ACTOR_DEPTH_BOTTOM + n` / `ACTOR_DEPTH_TOP - n`); `EVENT_ACTOR_SET_DEPTH` la da a mano. Comprobado en DKAS Demake B003→B004.
- **Orden de arranque de una escena** (C generado): primero se inicializan los actores, después el `onStart` de la **escena** y al final el `onStart` de cada **actor**. Un `FADE_IN` en la escena llama a `PAL_fadeIn` de SGDK, que copia la paleta destino **al empezar**. Por eso un cambio de paleta hecho en el `onStart` de un actor (por ejemplo `EVENT_ACTOR_SET_SPRITE` con `loadPalette`) se pierde al acabar el fundido. Hay que hacerlo en el `onStart` de la escena, antes del fundido, apuntando al actor por su id.
- **Ranuras de paleta:** el compilador asigna solo las paletas de los sprites (en Retrovania, todos los sprites de una sala con la misma paleta acabaron en PAL1) e ignora el `paletteId` del actor. Para dar a un actor colores propios, `EVENT_ACTOR_SET_SPRITE` con `paletteIdx: "PAL3"` y `loadPalette: true` copia la paleta de su sprite a esa ranura.
- **Mensajes entre actores con globales:** los scripts `onUpdate` se ejecutan en el orden de los actores en la escena (`_index`). En DKAS Demake el jugador es el 0. Al principio de su script pone a falso los avisos del fotograma («golpe ahora», «enemigo pegado»). Los enemigos, que van después, leen el golpe y escriben si están pegados, y el jugador lo lee en el fotograma siguiente. Así un aviso dura exactamente un fotograma sin necesidad de temporizadores.
- **Pausa del motor** (`TBCE_Scene.c`): `EVENT_SCENE_PAUSE` solo pide la pausa, que empieza en el fotograma siguiente. Entonces se ocultan todos los sprites, no se actualizan actores, proyectiles, temporizadores ni cámara, y cada fotograma se ejecuta el `onPauseScript` de la escena. El mando se sigue leyendo (`TBCE_InputUpdate` va antes de la escena), así que un menú funciona con `IF_INPUT_JUST_PRESSED`. `EVENT_SCENE_UNPAUSE` la quita al final de ese fotograma. **⚠ `tbceState.paused` solo se pone a 0 al arrancar la consola**, no al cambiar de escena: para reiniciar desde un menú de pausa, primero quita la pausa y pide el reinicio con una global que el `onUpdate` atienda en el fotograma siguiente (patrón de DKAS Demake B016). Para el panel del menú, alarga el plano Window con `EVENT_WINDOW_ENABLE` (`sizeY`) y devuélvelo a su tamaño al salir.
- **Fundir un solo sprite:** `EVENT_PALETTE_FADE_OUT` / `FADE_IN` con `paletteIdx` (`"PAL2"`…) funden solo esa ranura de paleta (`PAL_fadeOut/In` de SGDK, asíncrono). El fundido de entrada vuelve a la paleta de la escena (`currentScene.palettes`). Si la ranura es exclusiva del jugador, se oscurece solo él. Así se hace en DKAS Demake la transición al entrar por una puerta entre plantas (B017). Ojo con lo que comparta la ranura: el texto (`EVENT_SET_TEXT_PALETTE`) y los proyectiles con `paletteIdx` AUTO también se funden.
- **Física propia por script** (granadas de DKAS Demake B018): la física del motor tiene velocidad máxima y frenado X globales, así que un objeto con trayectoria propia se mueve con `EVENT_ACTOR_SET_POSITION_RELATIVE` y valores calculados. Por ejemplo, la velocidad vertical se guarda en cuartos de píxel en una local, con `{"type":"div"}` para pasarla a píxeles. Las paredes se detectan con `EVENT_IF_SCENE_COLLISION_AT_POSITION` (`x`/`y` en tiles con `units: "tiles"`, más los lados `up/down/left/right`), calculando el tile con `div` entre 8. Los operadores `min`, `div` y `mul` funcionan en cualquier valor (por ejemplo, para topar un contador: `min(SP + 1, 12)`).
- **Texto de otro color en una sola línea:** `EVENT_SET_TEXT_PALETTE` solo afecta a lo que se dibuja después (la ranura se guarda en los tiles al dibujar). Para resaltar una opción de un menú: cambia la paleta, dibuja esa línea y devuélvela a la normal. El color sale de la otra ranura con los mismos índices de la fuente (en DKAS Demake B020, PAL1, la del nivel, da ocre en vez de gris).
- **No hay escala de tiempo:** ningún evento de la 1.0.9 ralentiza la escena (cámara lenta). Habría que frenar a mano velocidades, temporizadores y animaciones.
- **Ids de variable:** el compilador renumera las globales (en el C, `HasWallJump` id 7 era `TBCE_Variables[2]`). Para depurar, busca la asignación que conoces (`= 1` al coger un objeto) en vez de fiarte del id.

- **Botón que sigue pulsado al cerrar un menú:** el `onPauseScript` atiende A con `EVENT_IF_INPUT_JUST_PRESSED`. Al despausar, si el jugador dispara con `EVENT_IF_INPUT_PRESSED` (mantener pulsado), en el fotograma siguiente dispara solo. Para evitarlo, se usa una global de bloqueo: se activa al cerrar el menú y al empezar la escena (después de un reinicio con A), y el jugador la limpia cuando ya no está pulsado ninguno de los botones de acción.
- **Ventana que se despliega:** `EVENT_WINDOW_ENABLE` (`TBCE_MDWindowPlaneEnable`) solo cambia los registros de posición de la ventana (`VDP_setWindowVPos`) y no borra el plano. Se puede dibujar un panel entero con la ventana a 1 fila y luego hacerla crecer una fila por fotograma desde el `onPauseScript`. `sizeY` es un número fijo, así que se pone un `EVENT_IF` por cada tamaño, comparando con un contador.
- **Prioridad de los efectos (XGM2):** `priority` va de 0 (la más baja) a 15. Un sonido de disparo que se repite 12 veces por segundo, con prioridad menor que las voces, no las corta al quedarse sin canales PCM.

- **Cambiar de escena desde la pausa:** `tbceState.paused` no se borra al cambiar de escena, así que `EVENT_SWITCH_SCENE` no debe lanzarse desde el `onPauseScript`. La pausa deja una petición en una global y se despausa; el jugador hace el cambio (o el `EVENT_SCENE_RESTART`) en el fotograma siguiente.
- **Varios personajes en un actor:** `EVENT_ACTOR_SET_SPRITE` cambia la hoja del actor en marcha. Si las hojas tienen las mismas filas de animación y la misma caja, un actor «Player» puede llevar los scripts de cada personaje dentro de un `EVENT_IF` sobre una global, y la escena no se duplica.
- **`EVENT_ACTOR_SET_SPRITE` manda el actor al fondo:** rehace el sprite y lo deja detrás de los demás. Un `EVENT_ACTOR_SEND_TO_FRONT` anterior se pierde, así que el cambio de hoja va antes (en DKAS Demake, B024, el Breacher pasaba por detrás de las puertas).
- **Reiniciar una animación que ya suena:** `EVENT_ACTOR_SET_ANIMATION_BY_INDEX` no reinicia si el índice no cambia. Para disparos seguidos con la misma animación (escopeta con bombeo), se usa `EVENT_ACTOR_SET_FRAME` con `frame` 0.
- **Paleta en una escena sin enemigos:** si el texto usa una ranura que en esa escena nadie llena (en DKAS, PAL3 con la paleta de los enemigos), un actor oculto con `EVENT_ACTOR_SET_SPRITE` (`paletteIdx` PAL3, `loadPalette` true) carga la paleta que necesita.

- **El cambio de escena corta los efectos que suenan:** una música de fin de partida en PCM (XGM2) se corta si el jugador sale del panel antes de que termine. Conviene mostrar «Continuar» cuando acaba (duración del WAV = muestras / frecuencia) y darle `priority` 15 para que los clics no le quiten el canal.
  - ⚠ **Quien lo corta no es el motor**, sino lo que suena en la escena nueva. En MD Engine 1.1.0, ni `TBCE_Scene.c` ni la HAL llaman a `XGM2_stopPCM` ni a `XGM2_stop` al cambiar de escena: un PCM que nadie pisa sigue sonando en la escena siguiente. Comprobado en Super Smash Land MD, B036: la música de los menús seguía de una escena a otra.
  - El adaptador de C suponía lo contrario: al empezar cada escena daba la música por parada (`MUS_NONE`) y luego ignoraba la orden de parar «porque ya estaba parada». Resultado: quitar la música en OPTIONS no la callaba. Si llevas la cuenta de qué suena, no la reinicies al cambiar de escena, y la orden de parar mándala siempre.

- **Un actor = una función de C:** el compilador genera `actor_<símbolo>_start/update` por cada actor de cada escena. En `Actors.c` van todas juntas, así que dos actores con el mismo nombre en escenas distintas chocan. Hay que dar nombres (y UUID) únicos en todo el proyecto. Tamaño medido en DKAS Demake: unos 7 KB de ROM por enemigo con IA de unos 300 eventos.
- **Prefabs:** las instancias llevan los scripts vacíos y `prefabScriptOverrides` (argumentos de eventos concretos, por id de evento). Las funciones de actor reciben `const u16* params`, lo que apunta a que un prefab se compila una vez y sus diferencias llegan como parámetros. Sin verificar todavía. Es la vía para no repetir la IA de 100 enemigos.
- **Scripts propios:** `project/scripts/<x>.mderes` (`_resourceType: "script"`, con `variables` V0… por valor o referencia y `actors` 0…), que se llaman con `EVENT_CALL_CUSTOM_EVENT` (`customEventId`, `$actor[0]$`, `$variable[V0]$`).
- **Proyectiles:** 16 ranuras (`PROJ_SLOT_0..15`, `TBCE_TOTAL_PROJ_USED`).

- **Máximo de 80 sprites:** el motor de sprites de SGDK admite 80 objetos (`allocStack` de 320 bytes; TBCE avisa en `TBCE_SpriteCreate`). Cuentan **cada actor y cada proyectil precargado** (`EVENT_PROJECTILE_PRELOAD` crea `totalAmount` sprites). Si se pasa, `SPR_addSpriteSafe` devuelve NULL y el actor se queda sin sprite. Con 40 o más actores, conviene recortar las reservas de proyectiles y pintar en el fondo los objetos estáticos.
- **La VRAM de sprites se reserva al crear el sprite** (`SPR_FLAG_AUTO_VRAM_ALLOC`), al empezar la escena, con el tamaño del fotograma más grande, aunque el actor esté fuera de pantalla. `EVENT_ACTOR_SET_SPRITE` libera el sprite y crea otro (`TBCE_ActorSetSpriteSheet`): una hoja vacía de 1 tile sirve para «aparcar» los actores lejanos. `EVENT_ACTOR_DEACTIVATE` **no** libera el sprite (solo quita `FLAG_ACTIVE`).

- **Nunca un sprite de 0 tiles:** una hoja totalmente transparente sale de rescomp con `maxNumTile = 0`. Con la reserva automática de VRAM, SGDK llama a `VRAM_alloc(0)`, que marca el bloque libre como ocupado con tamaño 0 y corrompe la lista de VRAM (comprobado desensamblando `libmd_release.a`). Después fallan los `SPR_addSpriteSafe` y se pierden todos los sprites, también al reiniciar la escena. Una hoja «vacía» necesita al menos un píxel.
- **`EVENT_ACTOR_SET_SPRITE` fuera de pantalla:** `TBCE_ActorSetSpriteSheet` crea el sprite en `actor->x/y` (coordenadas de mundo). `TBCE_ActorSync` solo lo recoloca si el actor está en pantalla, y solo lo oculta al pasar de dentro a fuera. Un sprite cambiado fuera de pantalla se pinta, pues, en su posición de mundo hasta que el actor entra. Hay que cambiar la hoja con el actor en pantalla (`EVENT_IF_ACTOR_ON_SCREEN`) u ocultarlo (`EVENT_ACTOR_HIDE`).
- **Hitbox y hoja:** al cambiar de hoja, el actor toma la caja de la nueva (`TBCE_SpriteCreateForActor`), y `pxpos_center` cambia con ella. Una hoja provisional debe tener la misma caja que la real.

- **RAM por escena (montón de SGDK, unos 54 KB tras el `.bss` y la pila):**
  - Con `collisionStore: OnRAM`, el mapa de colisiones se copia con `TBCE_Malloc(ancho*alto)`, a 1 byte por tile de 8x8. Hace falta para `EVENT_SCENE_SET_COLLISIONS`.
  - Cada actor ocupa `sizeof(TBCE_Actor)` = 112 bytes, más la cabecera.
  - En el montón también van el búfer de DMA (`dmaBufferSize`), los 80 `Sprite` de SGDK (46 bytes cada uno) y las reservas de proyectiles (32 bytes por proyectil).
  - Un nivel grande con mucho cielo vacío gasta RAM sin motivo, así que conviene recortarlo.
- **Medir la RAM:** `EVENT_INLINE_C` con `kprintf("RAM libre %u B, bloque mayor %u B", MEM_getFree(), MEM_getLargestFreeBlock());` escribe en el registro del depurador, también en modo Release.
- **Reiniciar la consola:** `EVENT_INLINE_C` con `SYS_hardReset();`. No hay evento propio para esto. `EVENT_INLINE_C` mete el código C tal cual (`code`).

- **Misiones largas por tramos:** MD Engine crea todos los actores de una escena al empezarla, sin streaming de actores. Para un nivel lineal largo se puede poner una escena por tramo, con su imagen recortada y solo sus actores. Las globales llevan el estado de uno a otro (salud, bajas, tiempo). Para pasar, `EVENT_SWITCH_SCENE` cuando el jugador llega al borde. Cada tramo cuesta un cambio de escena (fundido, y lo que la escena nueva ponga a sonar). Caso práctico: DKAS Demake, B034.

- **Juego en C dentro de MD Engine: lo que el compilador no ve** (Super Smash Land MD, B011-B016, MD Engine 1.1.0). Cuando la lógica va en un plugin de C (`plugins/<x>/engine/src`) y los eventos solo arrancan el juego, el compilador de proyecto no sabe qué usa ese C:
  - **Solo compila los recursos y las escenas que cita algún evento.** Un sonido que solo se reproduce desde C no entra en la ROM (falla el enlazado con `sound_<nombre>_wav` sin declarar). Una escena a la que solo se llega con `TBCE_SceneRequestChange` desde C no entra en `TBCEG_SceneDescriptors`. En ese caso el cambio salta a basura: *ILLEGAL INSTRUCTION* en 0x101 o *LINE 1111 EMULATOR*.
  - **Solución:** en el `onStart` de cada escena, un `EVENT_IF` que nunca se cumple y que contiene un `EVENT_SOUND_PLAY_EFFECT` por sonido y un `EVENT_SWITCH_SCENE` por escena. No se ejecuta, pero hace que se compilen. Como condición sirve una global que nadie escribe comparada con un valor imposible (`{"type":"eq","valueA":{"type":"variable","value":"0"},"valueB":{"type":"number","value":31337}}`, declarando la global en `variables.mderes`). El compilador no puede eliminar esa rama.
  - **⚠ No uses `EVENT_IF_CHEAT_MODE_ON` para esto** (SSB B031). El modo trucos es el bit 0 de `tbceState.settings`, y el depurador del editor lo escribe en la RAM en cada fotograma (`writeModes`) si está marcado. Con él activo, el bloque se ejecuta: suenan todos los efectos y la escena pide cambiar a todas las demás. Síntoma: juego colgado en el Play con depurador, con sprites rotos o pantallas al azar, mientras que sin depurador va bien.
  - **Para reproducirlo sin pantalla:** en el runner, poner a 1 el bit 0 de la palabra `tbceState + 8` antes de cada fotograma.
  - **Comprobación:** en `%LOCALAPPDATA%\Temp\_MDEngine\src\game\<PROYECTO>.c`, `TBCEG_SceneDescriptors[]` debe tener todas las escenas.
  - **Cambiar de escena desde C:** `TBCE_SceneRequestChange(id, -1, -1, -1, false, false)`. El id no es siempre `_index` + 1: la escena inicial pasa a ser la 1. Usa `SC_SCENE_<SIMBOLO>` de `src/game/<PROYECTO>_symbols.h` (ver depuracion-y-generacion.md). Las variables `static` del C sobreviven al cambio, así que sirven para pasar estado entre escenas (por ejemplo, las estadísticas de la partida a la escena de resultados).
  - **Los símbolos de sonido se cortan a 27 caracteres.** `sound_<nombre>_wav` con un nombre de 18 letras (`sfx_playerdefeated`) queda en `sound_sfx_playerdefeated_wa` y el C que lo nombra completo no compila. El nombre del recurso debe tener como mucho 17 caracteres.
  - **Música PCM a media velocidad (la mitad de ROM).** Los WAV se compilan a 13,3 kHz (`TBCE_SX_HALF_RATE` es FALSE) y rescomp remuestrea lo que venga a otra frecuencia. El truco:
    1. Convertir la música a 6.650 Hz y escribir 13.300 en la cabecera del WAV, para que rescomp no la toque.
    2. Reproducirla desde C con `XGM2_playPCMEx(datos, tamaño, SOUND_PCM_CH1, 15, TRUE /*halfRate*/, bucle)`.

    Los efectos van en los canales 2 y 3. El motor **no** para el PCM al cambiar de escena: la música sigue hasta que el C la cambie o la pare (SSB B037).
  - **Los módulos del motor también se activan por eventos.** `TBCE_CameraShake` (`TBCE/modules/TBCE_Camera.c`) solo existe con `TBCE_NEEDS_CAMERA`, que el compilador define si algún evento usa la cámara. Para llamarlo desde C basta con citar `EVENT_CAMERA_SHAKE` (`units: "frames"`, `frames`, `shakeDirection: "diagonal"`, `magnitude`) y `EVENT_CAMERA_STOP_SHAKE` en el bloque que nunca se ejecuta. Se comprueba en `TBCE_Globals.h`.
  - **Temblor de pantalla desde C:** `TBCE_CameraShake(TRUE, TRUE, px)` lo activa y `TBCE_CameraStopShake()` lo para; no tiene duración, la cuenta el juego. Se suma a la cámara **después** de aplicar los límites de la escena, así que funciona en una escena de una sola pantalla. Mueve el fondo y los actores.
  - **Primer plano por delante de los sprites:**
    - Una imagen aparte (índice 0 transparente) como recurso de fondo, cargada en el `onStart` con `EVENT_IMAGE_LOAD` (`backgroundId`, `variable: "T0"`, `paletteIdx`).
    - Se dibuja con `EVENT_IMAGE_DRAW` (`variable: "T0"`, `x`/`y` en tiles, `plane: "foreground"` = plano A, `priority: true`). Los valores de `plane` son `background` (B), `foreground` (A) y `window`.
    - Los sprites que deben quedar detrás necesitan prioridad baja: `SPR_setPriority(actor->sprite, FALSE)`. Los que deben ir delante de todo (cuenta atrás, retratos), `TRUE`.
    - Ojo con el texto en el plano A: escribir espacios para borrar una fila también borra el primer plano de esa fila.

- **Colisiones de DKAS (`.dkas`), byte de tipo (`flags[0]`):**

| Tipo | Qué es |
|---|---|
| 0 | Pared o suelo |
| 7 | Niebla de sala oculta: sólida en el fichero, pero no es pared, así que hay que ignorarla |
| 10 | Plataforma fina (pasarela o tejadillo) |
| 4 | Piezas pequeñas sólidas junto a las escaleras |

### Más de 2 mandos (MD Engine 1.1.0, comprobado en el código del motor, SSB 2026-10-23)

- **El motor solo lee 2 mandos:** `TBCE_Hal.h` define `MAX_PLAYERS 2` («We only support 2 players for now»). `TBCE_InputPressed` y los eventos de entrada no ven más allá de `JOY_2`.
- **SGDK, debajo, sí lee los adaptadores.** `JOY_init` (llamado al arrancar) detecta solo el **Sega Team Player** y el **EA 4-Way Play**.
  - Sus 4 mandos son `JOY_1`, `JOY_3`, `JOY_4` y `JOY_5` (tabla `xlt` de `joy.c`). Un mando normal en el puerto 2 sigue siendo `JOY_2`.
  - El código C de un plugin puede leer los jugadores 3 y 4 con `JOY_readJoypad(JOY_3...)`, sin pasar por el motor.
  - `JOY_getJoypadType(j) != JOY_TYPE_UNKNOWN` dice qué mandos hay enchufados.
- El emulador del Play (y el WASM de las pruebas) solo tiene 2 mandos. Para 3-4 jugadores reales hay que probar en BlastEm (emula los dos adaptadores) o en una consola.
- ⚠ **`JOY_init` solo enciende el Team Player si en el otro puerto no hay un mando** (`joy.c`: «no pads - look for teamplayer»). Con un Team Player en el puerto 1 y un mando normal en el 2, `JOY_readJoypad(JOY_3)` devuelve 0. Hay que mirar `JOY_getPortType(PORT_x) == PORT_TYPE_TEAMPLAYER` y llamar a `JOY_setSupport(PORT_x, JOY_SUPPORT_TEAMPLAYER)`. Para el Team Player solo cambia el modo de lectura: los registros del puerto ya los dejó listos `JOY_init`.
  - Team Player en el **puerto 2**: sus mandos son `JOY_2`, `JOY_6`, `JOY_7` y `JOY_8`. El EA 4-Way Play ocupa los dos puertos y da `JOY_1`, `JOY_3`, `JOY_4` y `JOY_5`.
  - Reparto usado en Super Smash Land MD (B043): adaptador en el puerto 1 o EA → jugadores 1-4 = `JOY_1`, `JOY_3`, `JOY_4`, `JOY_5`; Team Player en el puerto 2 → `JOY_1`, `JOY_2`, `JOY_6`, `JOY_7`.

### Orden de sprites con `SPR_setDepth` (SGDK)

- `SPR_addSprite` da a cada sprite la profundidad **máxima** (`SPR_MAX_DEPTH`, detrás de todo), salvo con `SPR_FLAG_INSERT_HEAD`. Un sprite al que nunca se le pone profundidad queda detrás de cualquiera que la tenga, aunque se haya creado antes. Cuanto menor es la profundidad, más delante.
- La prioridad del tile (`SPR_setPriority`) solo ordena frente a los planos, no entre sprites.
- En Super Smash Land MD (B043), los retratos de los resultados quedaban detrás de los recuadros, que son piezas con profundidad 100 + ranura. Se arregló poniéndoles profundidad −50.

### Hojas que ningún actor lleva (MD Engine 1.1.0)

- MD Engine solo mete en la ROM los recursos que cita algún actor o evento. Para una hoja que solo pone el C (`SPR_setDefinition`), un actor oculto la cita, pero reserva su VRAM en esa escena.
- Alternativa sin coste de VRAM: citarla con `EVENT_ACTOR_SET_SPRITE` (`actorId` de cualquier actor de la escena, `spriteSheetId`, `paletteIdx: "AUTO"`, `loadPalette: false`) dentro de un `EVENT_IF` que nunca se cumple, como los sonidos y las escenas. En DKAS Demake, la hoja «de aparcar» solo se ponía con este evento y entraba en la ROM. En Super Smash Land MD B043 se usa para las hojas `_hi` del cuarto luchador (pendiente de confirmar al compilar).

## Referencias cruzadas

- [formato-proyecto.md](formato-proyecto.md) · [fisica-y-colisiones.md](fisica-y-colisiones.md) · [camara-y-scroll.md](camara-y-scroll.md)
