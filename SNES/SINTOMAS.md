---
titulo: SNES — síntomas y causas
plataforma: [SNES]
categoria: [referencia]
tags: [depuracion, sprites, audio, dma]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [SNES/herramientas/errata.md, SNES/graficos/sprites.md, SNES/graficos/ppu-registers.md, SNES/herramientas/timing.md, SNES/graficos/backgrounds.md]
---

# SNES — síntomas y causas

Todo viene de la documentación (SNESdev Wiki): **no lo hemos visto en un proyecto propio**. Sirve como lista de sospechosos
hasta que haya uno. Al confirmar un caso, conviene anotar dónde.

## Sprites

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Sprites que parpadean o se cortan por líneas | Más de 32 sprites o de 34 franjas de 8 px por línea | Leer los bits *range over* y *time over* | [sprites](graficos/sprites.md), [registros PPU](graficos/ppu-registers.md) |
| Al superar el límite se pierden los sprites importantes | La SNES descarta las franjas de mayor prioridad | Reducir sprites por línea | [erratas](herramientas/errata.md) |
| Parpadeo con pocos sprites visibles | Sprites en X=$100 cuentan para los límites | Buscar sprites ocultos en esa X | [erratas](herramientas/errata.md) |
| Cambiar la prioridad no cambia qué sprite tapa a cuál | El orden entre sprites es fijo, por índice OAM | Reordenar las entradas de OAM | [sprites](graficos/sprites.md) |
| Sprites 16×32 entrelazados rotos | Se tratan como 16×16 | Evitar ese tamaño | [erratas](herramientas/errata.md) |
| Flip vertical raro en 16×32 o 32×64 | Las dos mitades se voltean por separado | Evitar esos tamaños no oficiales | [erratas](herramientas/errata.md) |
| Color math sin efecto en un sprite | Solo afecta a las últimas 4 paletas de sprite | Mirar qué paleta usa | [erratas](herramientas/errata.md) |
| Un color de sprite que debía ser negro sale transparente | El píxel 0 es transparente en todos los formatos | Usar otro índice con color negro | [tiles](graficos/tiles.md) |

## Fondos y escrituras a la PPU

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Escrituras a VRAM u OAM que se pierden | Fuera de vblank o forced blank | Comprobar en qué momento se escribe | [registros PPU](graficos/ppu-registers.md) |
| No cabe la actualización de gráficos en el vblank | Más bytes que el presupuesto: 6.123 en NTSC a 224 líneas | Sumar los DMA del frame | [límites](LIMITES.md), [timing](herramientas/timing.md) |
| Un píxel erróneo al cambiar INIDISP | Lectura anticipada de la PPU | Usar direccionamiento largo: `STA $8F2100` | [erratas](herramientas/errata.md) |
| Fundido gradual distinto entre consolas | Retardo de brillo en la SNES de 1 chip | Probar en ambas revisiones | [erratas](herramientas/errata.md) |
| El primer tile no respeta offset-per-tile | Nunca le afecta | Es lo normal: planificar con ello | [erratas](herramientas/errata.md) |
| BG3 no se dibuja en modos 2, 4 o 6 | Su tilemap se usa como tabla de desplazamiento (BG2 en el modo 6) | Mirar el modo activo | [fondos](graficos/backgrounds.md) |
| Modo 7 sin scroll propio | Reutiliza `$210D` y `$210E` | Revisar esos registros | [fondos](graficos/backgrounds.md) |
| Datos corruptos en el frame en que se activa HDMA | Tablas sin terminar antes de la línea 0 | Prepararlas durante el vblank | [HDMA](graficos/hdma-ejemplos.md) |
| Tearing con tablas de HDMA dinámicas | Se reconstruyen sin doble búfer | Usar dos tablas | [HDMA](graficos/hdma-ejemplos.md) |
| Formas de ventana corruptas | Posiciones fuera de 0-255 | Recortar el cálculo | [formas de ventana](graficos/formas-de-ventana.md) |
| Lecturas o escrituras de memoria de la PPU sin efecto y sin error | Fuera de las ventanas de tiempo | Revisar el momento del acceso | [memoria de la PPU](graficos/leer-escribir-memoria-ppu.md) |

## Interrupciones y CPU

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Varias NMI en un mismo vblank | Activar NMITIMEN con RDNMI ya activo | Leer RDNMI antes de activarla | [erratas](herramientas/errata.md) |
| Resultado erróneo al multiplicar | Escribir `$4203` dos veces demasiado rápido | Separar las escrituras | [tests de emuladores](herramientas/tests-emuladores.md) |
| Copia de bloques que no pasa de banco | MVN y MVP no cruzan bancos solos | Partir la copia | [MVN y MVP](cpu/mvn-mvp-copia-bloques.md) |
| Resultado corrupto en MPYL/MPYM/MPYH | HDMA o una interrupción escribe en el scroll de BG1 o en la matriz de modo 7 | Evitar esas escrituras durante el cálculo | [registros de la PPU](graficos/ppu-registers.md) |
| Multiplicación o WMDATA corruptos de vez en cuando | La NMI usa los mismos registros que el bucle principal | Dejarlos a un solo contexto | [NMI](graficos/vblank-interrupts.md) |
| Fallo en los primeros modelos al combinar DMA y HDMA | La DMA termina justo cuando arranca el HDMA de la línea siguiente | Seguir las secuencias de la entrada | [ejemplos de DMA](memoria/dma-ejemplos.md) |

## Sonido

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Datos erróneos entre CPU y SPC700 | Carrera en `$2140`-`$2143` | Leer dos veces hasta que coincida | [erratas](herramientas/errata.md) |
| El SPC700 se cuelga o se ralentiza | Se ha tocado el registro TEST (`$F0`) | Revisar escrituras a `$F0` | [erratas](herramientas/errata.md) |
| Envolvente incorrecta al iniciar una nota | Carrera entre ADSR y GAIN | Escribir ADSR2/GAIN antes que ADSR1 | [erratas](herramientas/errata.md) |
| ARAM corrupta con eco | `EDL = 0` sobrescribe 4 bytes al inicio del búfer | Con `ESA = $00` afecta a `$0000-$0003`; revisar `FLG` | [erratas](herramientas/errata.md) |
| No se puede ajustar el release | Su tasa es fija | Cambiar a GAIN a mitad de nota | [erratas](herramientas/errata.md) |
| Audio desincronizado entre consolas | La frecuencia varía entre 32.000 y 32.160 Hz | No sincronizar por muestras exactas | [timing](herramientas/timing.md) |
| «El sonido no arranca» y el juego se queda parado | El sondeo de `APUIO0`-`APUIO3` nunca recibe respuesta | Revisar el protocolo de carga | [arranque del SPC700](sonido/arranque-spc700.md) |
| Chasquido al empezar una muestra BRR | Falta silencio inicial para la interpolación gaussiana | Añadir muestras en silencio | [BRR](sonido/brr-samples.md) |
