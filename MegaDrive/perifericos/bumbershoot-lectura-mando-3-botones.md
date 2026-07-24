---
titulo: Bumbershoot Software — lectura de mando de 3 botones en ASM puro
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [bumbershoot, mando, joypad, controller, asm68000, io]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/03/17/genesis-sprites-and-controllers/]
---

# Bumbershoot Software — lectura de mando de 3 botones en ASM puro

## Resumen

Cuarta entrada de la serie Bumbershoot Software (2018), centrada en la lectura del mando de 3 botones desde ensamblador 68000 puro. La entrada original del blog también repasa a nivel conceptual el sistema de sprites del VDP (tabla de atributos como lista enlazada), pero esa parte no incluye código ni aporta detalle nuevo respecto a lo ya documentado en `../graficos/sprites.md`, `../graficos/sprite-table.md` y `../graficos/bec-sprites.md`; por eso aquí solo se documenta en profundidad la parte de mando, y la de sprites se referencia sin duplicarla.

## Detalle técnico

### Direcciones de puerto

Puerto de datos del mando 1 en `$a10003` (mandos 2 y 3 en `$a10005` y `$a10007` respectivamente).

### Rutina completa de lectura (3 botones)

El autor implementa la lectura en un único bloque que combina dos lecturas del puerto (con la línea TH en alto y en bajo) y usa un truco de desplazamiento aritmético para extraer el bit de Start con signo, en vez de enmascarar cada bit por separado:

```
moveq   #0, d0
moveq   #0, d1
movea.l #$a10003, a0
move.b  #$40, (a0)
move.b  (a0), d0
move.b  d0, d1
andi.b  #$0f, d1
asl.b   #1, d0
and.b   #$60,d0
or.b    d0, d1
move.b  #$00, (a0)
move.b  (a0), d0
asl.b   #2, d0
asr.b   #2, d0
and.b   #$90, d0
or.b    d1, d0
eor.b   #$ff, d0
rts
```

### Explicación paso a paso

1. `move.b #$40, (a0)` pone en alto el pin TH del puerto (bit 6) y `move.b (a0), d0` lee el primer estado: el byte devuelto tiene el formato `0CB0RLDU` (C y B en bits 5-4, dirección en bits 3-0).
2. Se copia a `d1` y se enmascaran solo los 4 bits de dirección (`andi.b #$0f, d1`).
3. Se desplaza `d0` un bit a la izquierda y se enmascara con `$60` para aislar C/B ya realineados, combinándolos con `or.b` sobre `d1`. En este punto `d1` contiene dirección + B + C.
4. Se pone TH a `$00` y se vuelve a leer el puerto: este segundo valor trae A y Start.
5. El truco `asl.b #2, d0` seguido de `asr.b #2, d0` desplaza el bit de Start hasta la posición de signo del byte y lo reintroduce por desplazamiento aritmético a la derecha (que replica el bit de signo) — un atajo para propagar el bit de Start sin usar `btst`+`bset` por separado. El resultado se enmascara con `$90` para quedarse solo con A y Start ya en sus posiciones finales.
6. Se combina todo con `or.b d1, d0`.
7. **Inversión final:** el hardware del mando reporta los botones con lógica invertida (bit a 1 = botón *no* pulsado); `eor.b #$ff, d0` invierte todos los bits para que el resultado final tenga 1 = pulsado, más cómodo de testear después con `btst`.

## Notas / gotchas

- Esta rutina solo cubre el protocolo de mando de **3 botones**; el propio autor advierte que un mando de 6 botones necesita ciclos de lectura adicionales (ver `controllers.md`, que documenta el protocolo completo de 7 escrituras para detectar y leer un mando de 6 botones).
- El truco `asl.b #2` / `asr.b #2` para extraer y reposicionar el bit de Start mediante desplazamiento aritmético con signo es una forma compacta de evitar instrucciones de test/set independientes, pero es menos legible que un enfoque con `btst`/`bset` explícitos — útil como referencia de optimización, no como primer ejemplo para aprender el protocolo.
- La inversión de lógica (`eor.b #$ff, d0`) es un detalle de hardware fácil de olvidar si se copia el patrón de lectura sin el paso final: sin ella, el código de más alto nivel tendría que comprobar bits a 0 en vez de a 1 para "pulsado".
- La parte de sprites de este mismo artículo del blog (tabla de atributos como lista enlazada de registros de 8 bytes, reordenable cambiando el campo "next" para simular profundidad o hacer parpadear sprites de baja prioridad) no incluye código y no añade nada sobre lo ya cubierto con más detalle y ejemplos reales en `../graficos/sprites.md`, `../graficos/sprite-table.md` y `../graficos/bec-sprites.md` — se omite su documentación en detalle aquí para no duplicar contenido sin valor añadido.

## Referencias cruzadas

- [Mandos de Mega Drive (3 y 6 botones)](controllers.md) — fuente plutiedev.com; protocolo completo incluyendo el mando de 6 botones y detección de tipo de mando, ausente en este artículo.
- [Lectura de mando y bucle principal — enfoque Big Evil Corporation](bec-input-game-loop.md) — mismo protocolo de 3 botones leído con una rutina distinta (sin el truco de desplazamiento aritmético usado aquí); buena comparación de dos estilos de implementación equivalentes.
- [Sprites en el VDP](../graficos/sprites.md), [Referencia de la tabla de sprites](../graficos/sprite-table.md) — cubren con más detalle y ejemplos la parte de sprites que este artículo solo menciona conceptualmente.
