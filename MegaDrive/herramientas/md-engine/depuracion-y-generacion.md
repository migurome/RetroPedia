---
titulo: MD Engine — generar proyectos por código y depurar el C generado
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [md-engine, generador, node, asar, depuracion, flujo-de-trabajo, validacion, pruebas-automaticas, perfilado]
estado: borrador
fecha: 2026-10-15
actualizado: 2026-10-22
fuentes: [experiencia directa generando Akane, Carrion y Retrovania con Node.js (Desktop/Engine/*-generator)]
---

# MD Engine — generación programática y depuración

## Resumen

Como el formato es JSON + PNG, un script de Node puede generar el proyecto entero (arte, mapas, colisiones y lógica) y regenerarlo cuando cambie el diseño. MD Engine solo se usa para compilar y probar. Para depurar, el C que genera cada compilación queda en una carpeta temporal y se puede leer.

## Detalle técnico

### Leer el contenido de `app.asar` sin herramientas extra

El asar es una cabecera JSON + datos concatenados: bytes 12–15 = longitud de la cabecera JSON (que empieza en el byte 16) y bytes 4–7 = tamaño del pickle (los datos empiezan en `8 + ese tamaño`). Cada archivo tiene `size` y `offset` (en texto).

```js
const fs=require('fs');const fd=fs.openSync(asar,'r');const b=Buffer.alloc(16);fs.readSync(fd,b,0,16,0);
const hdr=JSON.parse(readStr(16,b.readUInt32LE(12)));const base=8+b.readUInt32LE(4);
// archivo: fs.readSync(fd, buf, 0, f.size, base + Number(f.offset))
```

En Git Bash para Windows, exporta `MSYS_NO_PATHCONV=1` antes de pasar rutas internas como `/appData/mde/templates`; si no, se convierten en rutas de Windows y no coincide nada.

### Listar los eventos y sus campos

Cargar cada `src/lib/events/*.js` con `Module._load` interceptado: devolver un Proxy para `../helpers/l10n` y para cualquier `require` que falle. Después leer `m.id` y recorrer `m.fields` (`key`, `type`, `defaultValue`, `options`). Así se obtiene el catálogo exacto de comandos y argumentos de esa versión.

### Estructura recomendada del generador

- `png.js`: escritor de PNG indexado (IHDR tipo 3 + PLTE de 16 + IDAT con zlib) y un lienzo con `rect`, `circle`, `line`, `outline`, `mirrorX`, `rotateCW`.
- `art.js`: sprites y fondos procedurales o carga de PNG dibujados a mano.
- `gen.js`: helpers de eventos (`IF`, `SET`, `SWITCH`, `PRESSED`…), fábricas de actores, escenas, `settings`, `variables` y `engine_field_values`.
- Salida primero a una carpeta temporal; validar; copiar al proyecto solo con MD Engine cerrado.

### Validaciones automáticas útiles

1. **Ids de evento:** todos los `command` del proyecto existen en el catálogo de eventos.
2. **Colisiones:** la suma de tramos RLE es igual a `width × height` de la escena.
3. **Tiles únicos del fondo:** se cuentan los bloques de 8×8 distintos del PNG; debe estar por debajo de `vdpBGFG` (1.004).
4. **Paleta:** los colores realmente usados en cada PNG pertenecen a la paleta permitida (útil para zonas con paletas restringidas).
5. **JSON:** cada `.mderes` se puede parsear.

### Comprobar colores en la CRAM (pruebas sin pantalla)

- `emu_runner.js` vuelca la CRAM (`_tbce_get_cram`, 128 bytes) con `"vram": [fotogramas]`. Son 64 colores de 16 bits en **big-endian** (`0000BBB0GGG0RRR0`), como en la consola.
- Para compararlos con una paleta del código, conviértela como `RGB24_TO_VDPCOLOR` de SGDK (`pal.h`): **redondea**. Suma medio paso a cada canal (`+0x10`, con tope en `0xFF`) antes de quedarse con 3 bits. `0x301010` da `0x224`, no `0x002`. Si se trunca sin sumar, todos los colores salen «mal» aunque la CRAM esté bien.
- Super Smash Land MD (B043, prueba `cuatro_jugadores`): así se comprueba que cada luchador tiene sus colores en su paleta y que el cuarto va en los índices 6-10 de PAL3.

### Depurar: el C de la última compilación

Tras compilar, MD Engine deja el proyecto traducido en `%LOCALAPPDATA%\Temp\_MDEngine\`:

- `src/game/scenes/scene_<x>.c`: init de la escena, listas `collidingActors`, `movingActors`, `actorsInGroupN` y el bucle de actualización con las llamadas a `TBCE_ActorCheckCollisions`.
- `src/game/actors/Actors.c`: cada script de actor traducido a C (`actor_<x>_update`, `actor_<x>_hit`), con comentarios de qué evento generó cada línea.
- `src/game/<PROYECTO>.c`: tablas globales (máscaras de colisión, prototipos de actor).
- `src/TBCE/`: el motor completo, con los `#define` activos en `TBCE_Globals.h` (por ejemplo `TBCE_COLLISIONS_ADVANCED`, `TBCE_OVERLAP_TYPE_CONTINUOUS`).

Así se localizó el bug de la caja de colisión que no seguía al actor (ver [fisica-y-colisiones.md](fisica-y-colisiones.md)): el actor no aparecía en `movingActors`.

### Probar la ROM sin abrir el editor: emulador WASM y recompilación (DB Fighter, 2026-10-17)

**Emulador por script.** Cada Build deja una versión web en `%LOCALAPPDATA%\Temp\_MDEngine\build\web\`, con el emulador de MD Engine compilado a WASM (`scripts/tbce_emu_wasm.js` + `.wasm`). Es un módulo de Emscripten que funciona en Node:

```js
const M = await require(".../scripts/tbce_emu_wasm.js")({print(){}, printErr(){}});
M._tbce_setup(false);
const rom = fs.readFileSync("rom/X.bin"), size = (rom.length + 0x7fff) & ~0x7fff, p = M._malloc(size);
new Uint8Array(M.HEAP8.buffer, p, size).fill(0).set(rom);
M._tbce_load_rom(p, size, 0, 0, 0, 0);
// cada fotograma:
M._tbce_set_input(jugador0o1, boton, 1);   // 1 dcha, 2 izda, 3 arriba, 4 abajo, 5 A, 6 B, 7 C, 14 Start
M._tbce_tick(0, 0);
const px = new Uint8Array(M.HEAP8.buffer, M._tbce_get_pixels(), M._tbce_pixels_width() * M._tbce_pixels_height() * 4); // RGBA
```

También exporta `_tbce_get_ram` (64 KB, **con los bytes de cada palabra intercambiados**: una s16 en `0xFFxxxx` se lee como `ram[a^1] << 8 | ram[(a+1)^1]`), `_tbce_get_vram`, `_tbce_get_cram` y `_tbce_get_vdp_reg(n)`. Las direcciones de `TBCE_Variables`, `cameraInformation`, etc. salen de `out/release/symbol.txt`. La escena aparece hacia el fotograma 95 (antes va el logo del motor). Implementación: `MDB_T/tools/emu_run.js` + `emu_shots.py`.

**Recompilar el C a mano.** La toolchain está extraída en `%LOCALAPPDATA%\Temp\_mdetools\` (makefile de SGDK). Copiando `_MDEngine/src` y `_MDEngine/res` a otra carpeta se puede tocar el C generado y recompilar sin la GUI:

```sh
T="$LOCALAPPDATA/Temp/_mdetools"; PATH="$T/bin:$PATH" "$T/bin/make.exe" -f "$T/makefile.gen" GDK="$(cygpath -m "$T")" release
# -> out/release/rom.bin
```

Sirve para probar hipótesis (comentar una llamada, añadir otra) en minutos. Después, el cambio se lleva al generador de JSON, porque el paso de JSON a C solo lo hace la GUI.

### Batería de pruebas automáticas que lee el estado del juego (Super Smash Land MD, 2026-10-22)

Las capturas sirven para mirar, pero no para comprobar. Una batería que **lee la RAM** puede afirmar cosas concretas, como «tras caerse, Mega Man está en y = salida − 64 con la animación del halo». Implementación: `SSB Port/tools/tests/run_tests.py` + `emu_runner.js`. Son 34 pruebas en unos 30 s y no tocan el escritorio.

1. **Emulador fijo.** Un Build normal borra `build/web`. Por eso se copian `tbce_emu_wasm.js` y `.wasm` (de `resources/app.asar`, `appData/mde/emulator/scripts`) a una carpeta del proyecto.
2. **El guion pide RAM en fotogramas concretos.** Cada uno vuelca los 64 KB de `_tbce_get_ram()`. Un byte en `a` se lee en `ram[a ^ 1]`.
3. **Estado público para las pruebas.** Con LTO, las `static` del C cambian de nombre en `symbol.txt` (`state.lto_priv.0`). Además puede haber dos con el mismo nombre (otra `state` de SGDK). La solución estable es una estructura global (`SsbDebug ssb_debug`) que el juego copia al final de cada `update`: estado, paso del menú, temporizadores y un contador de fotogramas de juego.
4. **Desplazamientos de los campos desde el compilador.** No se escriben a mano. Se compila una sonda con el gcc de SGDK (`-S`) y se lee el `.s`:
   ```c
   const unsigned short o_x = __builtin_offsetof(Player, x);   // no hay stddef.h: usar el builtin
   const unsigned short s_P = sizeof(Player);
   ```
   - `C:\sgdk\bin\gcc.exe` necesita `-BC:/sgdk/bin/` o no encuentra `cc1`.
   - Un valor 0 sale como `.zero 2` en vez de `.word 0`; hay que aceptar las dos formas.
   - Así las pruebas no se rompen cuando cambia la estructura.
   - **El sonido también se puede comprobar sin oírlo** (SSB B040). Tras cada `_tbce_tick`, `_tbce_get_audio_frame()` apunta a las muestras del fotograma: `Int16Array(HEAP8.buffer, ptr, _tbce_get_audio_frame_size() * 2)`, en estéreo intercalado. El pico por medio segundo dice si hay música o efectos en cada pantalla.
     - Así se descartó que la ROM se hubiera quedado muda cuando el usuario no oía nada.
     - El Play de MD Engine tiene un botón de **silenciar** en la barra del depurador (`onToggleMute`): mira ahí antes de buscar el fallo en el juego.
   - **Escribir en la RAM también sirve para preparar casos** (SSB B040). Antes de un `_tbce_tick` se puede poner una palabra en la RAM de `_tbce_get_ram()`, con el mismo `^ 1`. Así se coloca a un luchador fuera del escenario en un fotograma concreto y se comprueba si la IA vuelve. Los campos de 32 bits son dos palabras, la alta primero.
5. **Ralentizaciones.** El contador de fotogramas de juego (`frames`) se compara con los fotogramas del emulador entre dos volcados. Si avanza menos, el bucle no llegó a tiempo al refresco.
6. **Perfilado barato.** `M._tbce_debug_cpu_pc()` tras cada `_tbce_tick` da dónde estaba la CPU al acabar el fotograma; se cruza con `symbol.txt`. Si casi siempre sale `VDP_waitVBlank`, sobra tiempo y los problemas son picos sueltos. Volcando la RAM en cada fotograma se ve qué pasaba justo en los picos (posición, animación, proyectiles activos). **Pero el PC no dice qué alargó el fotograma**, solo dónde acabó: en SSB apuntó a `SPR_update` y el culpable era el movimiento.
7. **~~Perfilado con las líneas del VDP~~ (no fiable en este emulador).** Leer `VDP_getAdjustedVCounter()` al empezar y acabar cada parte parecía funcionar, pero el contador **avanza a saltos dentro del fotograma**. Un bucle de ~16 líneas medía 0 en 566 de 600 fotogramas, y el tiempo se apuntaba a la parte que "pillaba" el salto. Los totales por fotograma eran aproximados y el reparto por partes engañaba. En SSB hizo perseguir un pico que no estaba donde parecía (SSB B035, 2026-10-23).
8. **Perfilado exacto: el perfilador por secciones del emulador.** Es el mismo mecanismo que `TBCE_Profiler` de MD Engine (`appData/engine/modules/TBCE_Profiler.h`, lectura en `appData/_shared/emulator/scripts/debugger-base.js`, `readProfiler`).
   - **En la ROM:** una variable global `volatile u16` (con `__attribute__((used))`) en la que el código escribe el número de sección en curso, de 0 a 32. Cada escritura cuesta una instrucción, así que puede quedarse en la versión final.
   - **En el runner:** `M._tbce_set_prof_section_addrs(0xFF0000 | (dir & 0xFFFF))` una vez tras cargar la ROM. Después de cada `_tbce_tick`:
     - `M._tbce_snapshot_prof_section_cycles()`;
     - leer 33 `u32` en `M.HEAPU32` a partir de `M._tbce_get_prof_section_cycles() >> 2` (si un valor tiene el bit 31 puesto, cuenta como 0);
     - `M._tbce_reset_prof_section_cycles()`.
   - **Qué da:** ciclos exactos del 68000 por sección y fotograma; 488 ciclos son una línea NTSC. Las llamadas a la plataforma o al motor cuentan en la sección que las hace. Las interrupciones cuentan en la sección en curso.
   - **Para afinar** dentro de una función, se guarda la sección, se escribe una temporal y se restaura al salir.
   - En SSB está en `tools/tests/perfil.py` y la prueba de rendimiento usa la suma de las secciones del núcleo.
9. **Coste real de las cosas en el 68000 (medido así, SSB B035).**
   - Una consulta caja-contra-sólidos con 2 sólidos cerca (uno en rampa) son ~1000 ciclos, aunque todo vaya inline.
   - Llamar a una función con 5 argumentos en pila y 11 registros guardados son ~250 ciclos más.
   - Recorrer 16 structs para copiar 5 campos son ~5 líneas por fotograma.
   - Correr por una rampa (5 píxeles por paso, ~25 consultas) llega a 70 líneas.

Ejemplos de prueba: cada escenario carga y los dos jugadores tocan suelo; nadie pierde fotogramas en 10 s de combate; la cuenta atrás termina; la reaparición queda en la posición correcta; un personaje sube una rampa; la partida acaba en la pantalla de resultados; cada personaje hace todos sus ataques sin colgarse.

### VRAM de sprites: reparto y actores ocultos (port SSB, B025)

- El proyecto reparte la VRAM en `settings.mderes`: `vdpBGFG` (1004 por defecto), `vdpSprites` (420, como `SPR_init` de SGDK) y `vdpFont` (96). Suman 1520 tiles; BG + sprites = 1424.
- Cada actor reserva tiles para el fotograma **más grande de su hoja** al crearse la escena, **aunque esté oculto**. Muchos actores de reserva con una hoja grande agotan los 420 tiles sin que se vea nada raro hasta que algo cambia de hoja.
- `SPR_setDefinition` devuelve FALSE si no hay VRAM y el sprite **se queda con la hoja anterior**, pero con la paleta y la posición nuevas. Síntoma visto: al elegir Kirby salía un Mega Man rosa mal colocado. Conviene contar esos fallos en una variable de depuración y comprobarla en las pruebas.
- Soluciones: una hoja de relleno de 1 tile para los actores que no se usan, poner los actores que solo sirven para meter una hoja en la ROM en una sola escena (los recursos son de toda la ROM) y ajustar el reparto. En SSB: fondos 724 (160x144 con primer plano: 360 + 360 como mucho) y sprites 700.
- **Un banco SGDK que imita a MD Engine tiene que imitar también las hojas de relleno** (SSB B042).
  - MD Engine rehace los actores en cada escena; en SGDK los sprites duran de una pantalla a otra. Con 2 luchadores y 2 retratos más, la VRAM de sprites del banco se agotaba (y se fragmentaba) al pasar por VS OPTIONS y elegir escenario.
  - `SPR_setDefinition` falló, y luego SGDK leyó una hoja basura: el emulador avisó con `Unmapped IO read at $A40004`... (campos de una `SpriteDefinition` en una dirección imposible).
  - Arreglo: al cambiar de pantalla, el banco pone la hoja de relleno a lo que esa pantalla no usa, como el generador de MD Engine.
  - Las pruebas fallan ahora ante cualquier `[MEM WARN]` del emulador, que antes solo se imprimía.
- **Orden de dibujo de los actores:** al crear la escena, MD Engine da a cada actor `ACTOR_DEPTH_MIDDLE - índice` (en SGDK, menos profundidad = más delante), así que los actores de índice alto se dibujan **delante**. En un banco SGDK puro manda el orden de `SPR_addSprite` (el primero queda delante): justo al revés. Para decorados que deben ir detrás de los personajes hay que fijar `SPR_setDepth` desde C.
- El emulador WASM de MD Engine manda los mensajes de la ROM a `Debug.instance.log(ptr)`. Sin ese objeto, el runner de Node falla con `ReferenceError: Debug is not defined` en cuanto la ROM escribe algo. El argumento es un puntero: `Module.UTF8ToString(ptr)`.

### Medir el motor por fases: perfilador de TBCE y ROM compilada a mano (port SSB, B046)

Las builds de MD Engine que no son MASTER traen un perfilador de secciones: el motor escribe la fase en curso en `__profSectionBegin` (`TBCE_Profiler.h`: `PROF_INPUT`, `PROF_SCRIPTS` y sus `PROF_SCR_*`, `PROF_CAMERA`, `PROF_ACTOR_SYNC`, `PROF_SPR_UPDATE`, `PROF_AUDIO`, `PROF_VBLANK_WAIT`...). Pasando esa dirección a `_tbce_set_prof_section_addrs` del emulador WASM, se obtienen los ciclos de cada fase por fotograma.

Con la recompilación a mano (arriba) se pueden probar cambios sin la GUI. Por ejemplo:
- el plugin nuevo sobre el C de la última build;
- `EXTRA_FLAGS=-DSSB_FN_PROF` (el makefile lo añade a las opciones) para el perfil por funciones;
- marcadores en el adaptador.

Medido con 4 luchadores CPU (Super Smash Land):
- **Cada paso, además del código propio**, el motor gasta ~34 líneas en `TBCE_SE_update` (sprites) y ~15 en `TBCE_ActorsSyncAll`, con 35 actores en la escena.
- **Efecto umbral.** Si el paso se pasa del fotograma aunque sea poco, `SYS_doVBlankProcess` espera a la VBlank siguiente y se pierde un fotograma entero. Con 4 CPU, Underground completaba 326 pasos de 600 fotogramas, y el banco SGDK con el mismo núcleo, 590.
- **La VBlank se apunta a lo que corra.** Si el paso se pasa, la interrupción cae al final, mientras se dibuja, y su coste aparece en las funciones de dibujo. El perfil dice entonces que dibujar es caro aunque cada llamada al motor (`TBCE_ActorTeleportXY`, `SPR_setAnimAndFrame`) cueste poco.
- **La partida no es la misma que en el banco**: el azar se siembra con `random()` y el motor lo usa antes. Para comparar, mejor por llamada que por fotograma.

## Notas / gotchas

- **Perfil por funciones en el emulador de pruebas.** `_tbce_set_prof_script_addrs(dir_inicio, dir_fin, n)` vigila dos variables `u16` de la RAM. El juego escribe el número de una función en la primera al entrar y en la segunda al salir, y el emulador acumula ciclos totales, propios (sin las anidadas) y llamadas de cada número.
  - Se lee con `_tbce_snapshot_prof_script_cycles` y `_tbce_get_prof_script_cycles` / `_self_cycles` / `_call_counts`.
  - Es el mismo mecanismo que `PROF_SCRIPT_CALL` de MD Engine, pero vale para C propio: macro `FN(id, llamada)` que solo existe con `-DSSB_FN_PROF`.
  - El emulador no tiene muestreo por PC: para bajar al detalle se ponen marcadores temporales dentro de la función.
  - La interrupción de VBlank de SGDK es mínima (contador y XGM), así que no ensucia la atribución.
- **Búfer temporal de DMA y sprites comprimidos.** Las hojas de sprite comprimidas se descomprimen en el búfer temporal de DMA (`dmaBufferSize`, 10000 bytes por defecto en MD Engine). Si al crear la escena se piden varias a la vez y no caben, los tiles salen corruptos, sin error.
  - En SSB B043, cuatro recuadros de resultados de 80 tiles comprimidos se rompían cuando ganaba otro jugador.
  - Solución: esa hoja sin comprimir, o más búfer.

- **Compresión de recursos (MD Engine 1.1.0).** Sprites, fondos y tilesets tienen `"compression"`: `"none"`, `"fast"` (LZ4W), `"small"` o `"best"` (`COMPRESSIONS` en el editor).
  - Los fondos se descomprimen una vez al cargar la escena, así que comprimirlos es casi gratis.
  - Las hojas de sprite se descomprimen en cada cambio de fotograma (`preloadFrames: false`). Comprimir solo las que cambian poco (menús, título), no las de los luchadores.
  - En SSB B036 hizo falta para meter cinco pantallas de menú con la ROM a ~130 KB del límite.
- **Cartuchos de más de 4 MB.** La Mega Drive solo ve 4 MB de cartucho. MD Engine ofrece `cartSize: "8MB"` (`CART_SIZES`), que usa el *mapper* de Super Street Fighter II: los registros 0xA130F3-0xA130FF eligen qué bloque de 512 KB aparece en cada ventana.
  - BlastEm, Genesis Plus GX y los flashcarts (Mega EverDrive) lo soportan.
  - Un cartucho real necesita una placa con ese chip.
  - SSF2 son 40 megabits, es decir 5 MB: en los 90 el tamaño se anunciaba en megabits.
- **Un actor tiene la hoja fija por escena.** El adaptador puede cambiar fotograma, posición, paleta y visibilidad, pero no la hoja (`SPR_setDefinition` sobre el actor gasta VRAM y no está previsto). Si una pieza de menú alterna entre dos dibujos distintos (STOCK / TIME en SSB), van dos actores y se enseña uno.
- **Pantallas de distinto tamaño en el mismo proyecto.** Las escenas de MD Engine son de 320x224. Para un port de una pantalla pequeña (160x144 centrada con borde) se pueden hacer menús a pantalla completa:
  - el generador escribe ese fondo a 320x224 tal cual;
  - una tabla de bits (`FULL_SCREENS`) dice a la plataforma que esa escena no lleva borde;
  - en esas escenas, borrar una fila de texto pinta el tile 0 en vez del color del borde.
  - Las piezas se siguen colocando en coordenadas del recuadro; las de fuera llevan valores negativos.
- **Cifras de un menú: con sprites, no con texto** (SSB MD, B037). El texto de SGDK/TBCE va en la rejilla de 8x8 del plano, y un `draw_text` del original cae donde quiera: con el recuadro en y = 32 y los botones cada 4 px, los números de vidas y tiempo quedaban medio tile por debajo de su hueco. Solución:
  - la fuente de números del original (`lightFont_spr`, 10 fotogramas) como hoja de pieza;
  - dos piezas por número (decenas y unidades), en el x exacto del `draw_text`;
  - con una sola cifra, la de las unidades se corre lo que corría el original (452 → 472).
  - Lo mismo sirve para rótulos nuevos: el generador puede componer «PRACTICE» o «JUMP» con letras sueltas de otros botones del juego (columnas con tinta separadas por columnas vacías), con el mismo aspecto.
- **Fuente propia sin gastar más VRAM** (SSB MD, B038). El modo de texto simple de MD Engine (`SimpleENG`, 96 tiles) pinta un tile por carácter en este orden (`VDP_drawTextEx` de `TBCE_Hal.c`):
  - el espacio, ASCII 33-63, 65-93 (sin @) y 97-122 (sin ^ _ `);
  - `SimpleEFIGSP` añade 22 tiles con letras acentuadas (118 en total).
  - Si el juego solo escribe en mayúsculas, los 26 huecos de minúsculas sirven para lo que falte: Á É Í Ó Ú Ñ Ü ¿ ¡, iconos o las piezas del marco de una ventana.
  - El C traduce el UTF-8 a esos códigos antes de llamar a `TBCE_TextDrawBGEx`.
  - La fuente es un recurso del proyecto (`project/fonts/*.mderes`, `symbol: "font_font_default"`): PNG indexado de 128x48, con el 0 transparente. Los índices se usan tal cual con la paleta del texto.
  - En un banco de SGDK, la misma fuente en orden ASCII 32-127 se carga con `VDP_loadFont` (rescomp: `TILESET nombre "font.png" NONE NONE`, sin optimizar para que no reordene los tiles).
- **⚠ MD Engine no tiene los tiles de sistema de SGDK** (SSB MD, B038). SGDK deja en `TILE_SYSTEM_INDEX` 0-15 dieciséis tiles de un solo color (el tile i, todo del color i), y en un proyecto de SGDK suelto sirven para rellenar con un color. En MD Engine 1.1.0 están vacíos (volcado de VRAM: ceros), así que un relleno con ellos es transparente.
  - En SSB, la ventana de pausa salió de color rosa chillón: se veía el color de fondo del VDP, el índice 0 de la paleta (el rosa de transparencia del PNG), y los sprites pasaban por delante.
  - Un borde que use ese tile «funciona» solo si lo que hay detrás ya es del mismo color.
  - **Solución:** tiles de un color dentro de la fuente propia (un hueco de carácter libre, con todos los píxeles del índice deseado), pintados como texto con `VDP_drawTextBG(plano, "mmmm", x, y)`. Funciona igual en SGDK y en MD Engine, y lleva la paleta y la prioridad del texto.
- **Ventana encima del juego (pausa) sin sprites** (SSB MD, B038-B039):
  - El fondo de la ventana va en el plano B, con tiles de un color de la fuente (ver el punto anterior) y **prioridad alta**, para que tape a los sprites de prioridad baja. El marco y el texto van en el plano A. El texto de SGDK, y el de MD Engine, sale por defecto con prioridad alta y PAL0.
  - Los sprites con prioridad alta (en SSB, la cuenta atrás «3-2-1 GO») siguen encima: se ocultan mientras la ventana está abierta.
  - Para reponer lo que había, se lee el mapa de tiles de la VRAM antes de pintar: `SYS_disableInts()`, `VDP_setAutoInc(2)`, `*(vu32*)VDP_CTRL_PORT = VDP_READ_VRAM_ADDR(VDP_getPlaneAddress(plano, x, y))` y leer palabras de `VDP_DATA_PORT`.
  - Al cerrar se escribe con `VDP_setTileMapDataRect`. Así no hace falta saber dónde cargó MD Engine el primer plano del escenario.
- **Orden de pintado de piezas solapadas.** Con `SPR_setDepth`, el valor más bajo se pinta delante. Si las piezas usan una profundidad de `100 + hueco`, una cifra o una casilla en un hueco posterior queda **detrás** de su botón y no se ve. Las que van encima llevan una marca por pieza (`MenuDef.front`) y la profundidad de delante (`-100 - hueco`).

- **Pruebas sin pantalla: lee la RAM antes de la siguiente pulsación.** Si el guion pulsa en el fotograma `f` y lee la RAM en `f`, esa lectura ya incluye la pulsación (falso fallo del menú debug en SSB).
- **Lo que no detecta el emulador WASM:** no saca sonido y tolera lecturas de palabra en dirección impar, que en BlastEm y en hardware real son un *address error*. Una batería que pasa en WASM no basta: hay que probar en BlastEm.
- **Diferencias de tiempo entre ROM de SGDK y de MD Engine:** la de MD Engine arranca unos 30-40 fotogramas más tarde (logo) y cada cambio de escena tarda varias decenas más. Los guiones deben llevar los tiempos como parámetros, no fijos.
- Regenerar con nuevos UUID en cada pasada es seguro si se sustituye el proyecto entero, pero rompe los cambios que el usuario haya hecho a mano en el editor. Decide qué es la "fuente de verdad": el generador o el editor.
- Un error de compilación de la app normalmente aparece en su consola de build; el C generado ayuda a entenderlo.
- **`random()` de SGDK no es reproducible entre builds** (SSB MD, B029). Mezcla el contador H/V del VDP, así que cualquier cambio de tiempos antes de la tirada cambia el resultado: por ejemplo, poner una pantalla de título delante. Esperar más fotogramas en el guion no ayuda, porque la tirada cae en el mismo punto del fotograma. Si una prueba depende del azar del juego, debe:
  - cubrir el peor caso de la tirada (ventanas de tiempo amplias), o
  - repetir el lance dentro del juego hasta que se dé la condición previa (en SSB: START y volver al combate, que vuelve a tirar), en vez de fiarse de la semilla.
- **Números de escena para `TBCE_SceneRequestChange`:** no son el orden de los `_index` + 1. MD Engine pone la escena inicial (`startSceneId`) la primera y mueve la que estaba ahí. En SSB, con el título como inicial, el título pasó a ser la 1 y Battlefield la 13. Desde C conviene usar los `SC_SCENE_<SIMBOLO>` de `src/game/<PROYECTO>_symbols.h`, que MD Engine genera al compilar y un plugin puede incluir con `#include "../game/<PROYECTO>_symbols.h"`.
- **ROM cerca de 4 MB con guardado en flash = cuelgue al arrancar** (SSB MD, B029). Con `cartSave: "flash_only"` (el valor por defecto), el gestor de guardado (`sm_init` en `TBCE_SaveModuleInitialize`) se reserva la parte alta del cartucho. Son 242 ranuras de 1 KB por debajo de `TBCE_CART_SIZE - 64 KB`.
  - **Qué pasa:** si la ROM llega a esa zona, los datos y funciones que SGDK copia a RAM al arrancar (`_stext`, al final de la ROM) se leen como flash borrada (`FFFF`).
  - **Síntoma:** pantalla `LINE 1111 EMULATOR` con el PC en RAM. En SSB fue en `FF01B6` (`array_read_id`), llamada desde `cmd_profile_probe`, el sondeo del chip de flash.
  - **Arreglo:** si el juego no guarda, usar `cartSave: "sram_only"`. Así se define `TBCE_DISABLE_FLASH_SAVE`: no hay sondeo ni zona reservada, y la SRAM solo se activa al guardar o cargar.
  - **Comprobación:** en `symbol.txt`, `_stext` (dirección de carga de `.data`) más `_sdata` (su tamaño) da dónde acaba de verdad la ROM.
  - Opciones de `cartSave` en 1.1.0: `auto`, `sram_only`, `flash_only`.
- **El reproductor de MD Engine guarda la SRAM y la flash del juego** en su `localStorage` (`md_save_<proyecto>` y `md_flash_<proyecto>`, en `%APPDATA%/MD Engine/Local Storage/leveldb`). En cada Play las vuelve a pasar al emulador (`_tbce_load_rom(rom, size, sram, sramSize, flash, flashSize)`), **aunque la ROM sea de otra build**.
  - Con guardado en flash, el gestor de guardado formatea sectores de la parte alta del cartucho en el primer arranque. Desde entonces, esa flash guardada tapa la ROM nueva.
  - **Riesgo confirmado (SSB B030-B032):** tapa la parte alta de la ROM aunque la build nueva ya no use flash (`cartSave: "sram_only"`).
    - **Cómo reproducirlo:** hay que hacerlo con el reproductor real (`build/web/index.html` servido por HTTP, en Chrome sin ventana con puppeteer), metiendo el valor en `localStorage` con `evaluateOnNewDocument` antes de cargar la página.
    - **Lo que no sirve:** pasar esa misma flash a `_tbce_load_rom` desde un runner de Node no lo reproduce.
    - **Cómo sacar la flash guardada:** está en `%APPDATA%/MD Engine/Local Storage/leveldb/*.ldb`, como bloques con compresión snappy. El valor es un JSON de bytes, precedido de un byte de codificación.
    - **Arreglo:** borrar los datos de guardado. El botón de papelera del depurador no siempre bastó; el usuario los borró a mano.
    - **Sesión con depurador:** con `?debug=true`, el reproductor no avanza solo. Hay que llamar a `window.debug.handleCommand("listener-ready", {memoryMap, gameValues: {}})`, con `memoryMap` = símbolo → dirección hexadecimal de `symbol.txt`.
  - **Síntomas:** los sprites y la fuente cuyos tiles caían por encima de `0x3D0000` se veían como rectángulos lisos del color 15 (tiles leídos como `FF`), el juego daba tirones, y lo que estaba un poco más abajo (PRESS START, en `0x3CF1A4`) se veía bien.
  - En el emulador sin pantalla no pasa, porque no se le pasa nada guardado.
  - **Arreglo:** el botón de papelera de los controles del depurador, «Restablecer datos de guardado» (`DIALOG_CLEAR_SAVES`, orden `clear-saves`).
  - **Diagnóstico:** cuando algo se ve mal solo en el Play de MD Engine, comparar con el emulador sin pantalla. Mirar si los recursos que fallan están en la parte alta de la ROM (punteros `tiles` de los `TileSet` en `symbol.txt` y `rom.bin`).
- **Escenas sin personajes (título, menús):** cada actor reserva VRAM para el fotograma más grande de su hoja aunque esté oculto. Si una escena no usa los actores fijos del juego (luchadores, proyectiles...), el generador debe ponerles una hoja de relleno de 1 tile en esa escena. En SSB, la de título llegaba a ~680 de 700 tiles de sprites solo por eso.

## Referencias cruzadas

- [overview.md](overview.md) · [formato-proyecto.md](formato-proyecto.md) · [eventos-y-scripts.md](eventos-y-scripts.md)
- [Colisión con máscaras de píxel: trapecios y rampas](../../../GLOSARIO/mask-collision-slopes.md)
