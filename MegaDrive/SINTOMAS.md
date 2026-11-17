---
titulo: Mega Drive — síntomas y causas
plataforma: [MegaDrive]
categoria: [referencia]
tags: [depuracion, sgdk, md-engine, dma]
estado: borrador
comprobado: emulador
comprobado_en: "parcial: BlastEm, emulador WASM de MD Engine y FPGA, port de Super Smash Land; solo las filas sin marca"
fecha: 2026-11-06
fuentes: [MegaDrive/memoria/hardware-issues.md, MegaDrive/herramientas/sgdk/tuto-sprites.md, MegaDrive/herramientas/md-engine/depuracion-y-generacion.md, MegaDrive/cpu/optimizar-c-gcc-68000.md, MegaDrive/graficos/dma-transfer.md, MegaDrive/graficos/sprites.md]
---

# Mega Drive — síntomas y causas

Las filas sin marca pasaron en el port de Super Smash Land. **(doc)** significa que viene de documentación de terceros y
no lo hemos visto.

## Cuelgues y pantallas de error

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| `ADDRESS ERROR` | Acceso de palabra a dirección impar; a menudo un puntero basura | El `Offset` es el PC: buscarlo en `out/symbol.txt` y desensamblar con `objdump` | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| Cuelga en BlastEm al llegar a un frame de animación, y en WASM no | rescomp quita los frames vacíos del final y `SPR_setAnimAndFrame` lee fuera del array | Envolver la llamada y comparar con `numAnimation` y `numFrame` | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| `ADDRESS ERROR` tras cambiar de hoja | `SPR_setDefinition` devolvió FALSE por falta de VRAM y conserva la hoja vieja | Mirar lo que devuelve; ocultar el sprite si falla | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| `LINE 1111 EMULATOR` con el PC en RAM, con la ROM cerca de 4 MB | El guardado en flash se reserva la parte alta y pisa la ROM | En `symbol.txt`, `_stext` + `_sdata`; probar `cartSave: "sram_only"` | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |
| Falla en consola y no en emulador | RAM sin inicializar (el emulador o el flashcart la ponen a cero) **(doc)** | En BlastEm, poner Initial RAM Value en `random` | [problemas de hardware](memoria/hardware-issues.md) |
| Funciona en WASM y falla en BlastEm | WASM tolera lecturas impares | Probar siempre en BlastEm | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |
| Falla solo en un emulador | Rigor distinto con DMA, IRQ o mando; a veces el emulador es más estricto que el hardware **(doc)** | Probar en varios antes de reescribir | [fallos de emuladores](herramientas/bumbershoot-cca-bugs-emuladores-lanzamiento.md) |
| Se niega a arrancar y muestra basura como título, con la cabecera bien | Padding incorrecto: el emulador o el flashcart identifica mal el formato **(doc)** | Revisar el tamaño de la ROM; `romfix` (también corrige el checksum) | [padding](memoria/rom-padding.md) |
| Fallos raros a partir de un blob del Z80 | Falta `align 2` tras el blob: desalinea el código siguiente **(doc)** | Mirar en el mapa si las direcciones son impares | [driver Z80](sonido/bumbershoot-z80-driver-psg.md) |
| Fallos de consola en V30 (240 líneas) | V30 en NTSC solo es seguro en PAL **(doc)** | Comprobar la región antes de activarlo | [resolución](graficos/screen-resolution.md) |
| La Sub-CPU no ejecuta su programa en modo 1 | Falta el segundo reset tras cargar en PRG-RAM **(doc)** | Revisar la secuencia | [Sub-CPU](avanzado/subcpu-in-mode1.md) |
| Una Mega Drive original se cuelga, y en un clon no | Se leen los registros de AtGames en `0xB00000+` **(doc)** | No sondearlos para detectar el clon | [Firecore](memoria/bec-atgames-firecore.md) |

## Gráficos corruptos

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Tiles corruptos, sin error, al crear una escena | Las hojas comprimidas no caben en el búfer de DMA (`dmaBufferSize`) | Probar la hoja sin comprimir o más búfer | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |
| Sprites como rectángulos lisos del color 15, solo en el Play de MD Engine | La flash guardada tapa la parte alta de la ROM nueva | Comparar con el emulador sin pantalla; mirar si los recursos están arriba; borrar los datos de guardado | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |
| Basura tras un DMA | Cruce de un límite de 128 KB de la fuente, o longitud 0 interpretada como 65536 **(doc)** | Revisar dirección y longitud | [DMA](graficos/dma-transfer.md) |
| Basura ocasional en un DMA | El Z80 accede al bus del 68000 al empezar **(doc)** | Pausar el Z80 durante la transferencia | [problemas de hardware](memoria/hardware-issues.md) |
| Un sprite no cambia de posición Y o de tamaño cuando se espera | Caché de sprites del VDP **(doc)** | Escribir en la tabla según la entrada | [sprites](graficos/sprites.md) |
| Sprites que desaparecen o se cortan por líneas | Más de 20 sprites o 320 píxeles por línea en H40 **(doc)** | Contar los sprites de esa línea | [sprites](graficos/sprites.md) |
| Lista de sprites cortada | Campo `link` mal gestionado; `link` 0 termina la lista **(doc)** | Recorrer la lista desde el sprite 0 | [tabla de sprites](graficos/sprite-table.md) |
| La ventana de pausa sale rosa o transparente en MD Engine | MD Engine no tiene los tiles de sistema de SGDK | Volcar la VRAM; usar tiles de un color de la fuente propia | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |
| Unas piezas pasan por detrás de otras | Sin `SPR_setDepth`, un sprite queda en `SPR_MAX_DEPTH`, la de más atrás | Dar profundidad a todos, también al recrearlos | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| El sprite tiembla al animar a 1,5x | El reparto de píxeles dobles se cuenta desde cada frame | Contarlo desde el origen del sprite | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| La hoja de un luchador no cabe en VRAM | Los sprites reservados ocupan su frame más grande aunque estén ocultos | Crearlos al mostrarlos y soltarlos al ocultarlos | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| Se pierden todos los sprites, también al reiniciar la escena | Hoja vacía con `maxNumTile = 0`: `VRAM_alloc(0)` corrompe la lista de VRAM (visto en MD Engine, Retrovania) | Buscar hojas transparentes; dejarles un píxel | [eventos](herramientas/md-engine/eventos-y-scripts.md) |
| El fondo se llena de basura y un actor no aparece | `preloadFrames: true` con una hoja grande pisa los tiles del fondo (DB Fighter B002) | Probar con `false` | [formato de proyecto](herramientas/md-engine/formato-proyecto.md) |
| Basura en una pantalla nueva, sin error | Tabla de `Image*` a mano con un hueco: puntero nulo | `_Static_assert` del tamaño de la tabla | [fondos de SGDK](herramientas/sgdk/tuto-background.md) |
| Captura de prueba con tiles nuevos y mapa viejo | El fotograma del botón ya está cargando la pantalla siguiente: no es un fallo | Capturar más tarde | [fondos de SGDK](herramientas/sgdk/tuto-background.md) |
| Puntos de colores en una línea de la pantalla | Puntos de CRAM al cambiar la paleta a mitad de frame **(doc)** | Ocultarlos con sprites, como Sonic | [agua de Sonic](graficos/sonic-water.md) |
| VRAM o CRAM corrompidas de forma progresiva | Escrituras al puerto de depuración del VDP **(doc)** | Revisar esas escrituras | [depuración del VDP](graficos/vdp-debug.md) |
| El plano sale mal con cierto tamaño | Tamaño de tilemap fuera de las seis combinaciones válidas **(doc)** | Revisar `$90xx` | [registros del VDP](graficos/vdp-registers.md) |
| Lectura errónea del contador V | Se lee justo cuando cambia **(doc)** | Leer como byte y dos veces | [fade y pixel bleed](graficos/bumbershoot-cca-fade-pixel-bleed.md) |

## Va lento o la medida engaña

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Fotogramas perdidos sin cambiar la lógica | `SPR_update` recorre también los sprites ocultos | Soltar los que no se usan y medir | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| Las pérdidas saltan del 1 % al 14 % sin tocar el combate | La semilla del azar depende de los fotogramas en los menús | Fijar la semilla antes de medir | [optimizar C](cpu/optimizar-c-gcc-68000.md) |
| Una prueba con azar cambia entre builds | `random()` de SGDK mezcla el contador H/V | Cubrir el peor caso o repetir el lance | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |
| Una función pequeña cuesta mucho | Coste de la llamada: 200-300 ciclos con `movem` y 8 argumentos | Perfilar por funciones | [optimizar C](cpu/optimizar-c-gcc-68000.md) |
| Una prueba sin pantalla da un falso fallo | Se lee la RAM en el mismo fotograma de la pulsación | Leer antes de pulsar | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |

## Sonido y mandos

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Música con tempo incorrecto en algunas consolas | Se lee el estado del YM2612 desde una dirección no oficial **(doc)** | Usar solo `$A04000` o `$4000` | [problemas de hardware](memoria/hardware-issues.md) |
| Ruido ensordecedor en clones AtGames | Usa SSG-EG, mal implementado allí **(doc)** | Evitarlo o detectar el hardware | [problemas de hardware](memoria/hardware-issues.md) |
| Sonido que funciona en emulador y falla en consola | Sin retrasos entre escrituras al YM2612 **(doc)** | Revisar los retrasos | [problemas de hardware](memoria/hardware-issues.md) |
| Datos inválidos al leer el mando | El Z80 accede al bus durante el acceso a I/O **(doc)** | Detener el Z80 durante la lectura | [problemas de hardware](memoria/hardware-issues.md) |
| Mando de 6 botones que falla | Alternancias de TH excesivas o sin el retraso de 2 µs **(doc)** | Revisar la rutina de lectura | [problemas de hardware](memoria/hardware-issues.md) |
| Solo suena la primera nota | El driver del Z80 no reactiva las interrupciones **(doc)** | Revisar el final de la rutina | [driver Z80](sonido/bumbershoot-z80-driver-psg.md) |
| Un instrumento suena mal, salvo en el canal 1 | Offset de canal mal aplicado en la herramienta: con offset 0 no se nota **(doc)** | Probar en otros canales | [offset de canal](sonido/bumbershoot-bug-offset-canal-ym2612.md) |
| Una nota no se corta del todo al instante | El key-off con `$0F` en RR tarda hasta ~2,4 ms **(doc)** | Esperar antes de reutilizar el canal | [operaciones del YM2612](sonido/ym2612-operations.md) |
| Se oye también el canal 2 del PSG con el ruido | El ruido toma prestada su frecuencia **(doc)** | Silenciar el canal 2 | [PSG](sonido/psg.md) |
| Mando de 6 botones que falla con lecturas repetidas | Se lee más de una vez por fotograma **(doc)** | Leer una sola vez | [mandos](perifericos/controllers.md) |
| Datos de guardado sin sentido en el primer arranque | La SRAM trae basura al principio **(doc)** | Firma y checksum | [SRAM](memoria/saving-sram.md) |

## Al compilar

| Síntoma | Causa | Solución | Entrada |
|---|---|---|---|
| `Sprite frame ... uses 32 internal sprites, ... limit (16)` | rescomp admite 16 sprites de hardware por frame | Partir el frame o reducirlo | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| La ROM muestra un título con basura en emuladores | ROM sin padding **(doc)** | Rellenarla | [problemas de hardware](memoria/hardware-issues.md) |
| Error `This method is deprecated` con `fix16ToInt` | SGDK 2.11 la ha sustituido por `F16_toInt` | Usar los nombres de `inc/maths.h` | [snippet](../snippets/README.md) |
| El binario de ASM68K no carga en el emulador | Sin `/p` lleva metadatos del depurador **(doc)** | Añadir `/p` | [primeros pasos](herramientas/bec-getting-started.md) |
