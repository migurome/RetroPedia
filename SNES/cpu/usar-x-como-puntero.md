---
titulo: Usar X (o Y) como puntero de 16 bits en 65816
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, optimizacion, punteros, direct-page, ensamblador]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Using_X_as_a_pointer]
---

# Usar X (o Y) como puntero de 16 bits en 65816

## Resumen
Al trabajar con registros índice de 16 bits en el 65816, X (o Y) puede contener una dirección completa y usarse como puntero base para acceder a campos de una estructura mediante offsets, en vez de depender solo de indexación clásica sobre arrays paralelos. Esta técnica es especialmente eficiente cuando la estructura vive en la direct page.

## Detalle técnico

### Comparativa de modos de direccionamiento

| Instrucción | Bytes | Ciclos |
|---|---|---|
| `LDA directpage,x` | 2 | 4 (+1 si A es 16 bits, +1 si el byte bajo de la direct page ≠ 0) |
| `LDA absolute,x` | 3 | 4 (+1 si A es 16 bits, +1 si X es 16 bits o cruza de página) |
| `LDA (directpage)` | 2 | 5 (+1 si A es 16 bits, +1 si el byte bajo de la direct page ≠ 0) |
| `LDA (directpage),y` | 2 | 5 (+1 si A es 16 bits, +1 si el byte bajo de la direct page ≠ 0, +1 si cruza de página) |

### Ventajas y limitaciones
Usar `directpage,x` en vez de `absolute,x` ahorra 1 byte y 1 ciclo, y funciona mejor cuando la direct page está alineada (en $0000 o en un múltiplo de $100), sobre los primeros 8 KB de RAM.

Limitaciones: esta técnica es incompatible con el patrón de bucle clásico `DEX \ BPL` (que evita una comparación explícita), porque el puntero en X ya no es un simple contador. La direct page está limitada a los primeros 64 KB (en la práctica, un espejo de los 8 KB de RAM interna de la SNES). También hay que considerar la diferencia de velocidad de reloj entre RAM (2.68 MHz, 8 ciclos maestros) y ROM (3.58 MHz, 6 ciclos maestros) al estimar tiempos.

### Instrucciones compatibles
Admiten `directpage,x` y `absolute,x`: `BIT`, `ASL`, `LSR`, `ROL`, `ROR`, `INC`, `DEC`, `LDY`, `STZ`.

Los modos indirectos no están disponibles indexados por X (salvo para `LDA`/`STA`, que sí los soportan).

Caso especial: `STY directpage,x` existe, pero `STY absolute,x` no existe.

### Ejemplo completo — array de estructuras (ca65)
```asm
.struct Actor
  Type .word
  PositionX .word
  PositionY .word
  VelocityX .word
  VelocityY .word
.endstruct

.segment "BSS"
  ActorCount = 10
  ActorStart: .res ActorCount * .sizeof(Actor)
  ActorEnd:

.segment "CODE"

.a16
.i16
; Procesa actores activos (Type != 0) sumando velocidad a posición
.proc ApplyVelocityToActiveActors
  ldx #ActorStart
Loop:
  lda Actor::Type,x
  beq Skip

  lda Actor::PositionX,x
  clc
  adc Actor::VelocityX,x
  sta Actor::PositionX,x

  lda Actor::PositionY,x
  clc
  adc Actor::VelocityY,x
  sta Actor::PositionY,x

Skip:
  txa
  clc
  adc #.sizeof(Actor)
  tax

  cpx #ActorEnd
  bcc Loop
  rts
.endproc
```
X se carga con la dirección base de un actor, y luego `Actor::Campo,x` accede a cada campo desplazándose desde esa posición base.

## Notas / gotchas
- Al no poder usarse `DEX \ BPL` para recorrer estructuras vía puntero, hay que sustituirlo por comparaciones explícitas (`cpx #ActorEnd \ bcc Loop` en el ejemplo).
- `STY absolute,x` no existe como modo de direccionamiento, aunque `STY directpage,x` sí; no asumir simetría total entre STX/STY/STZ en cuanto a modos soportados.
- Las ganancias de ciclos dependen de si el acceso final cae en direct page (RAM interna, más rápida y con instrucciones más cortas) o en absoluto; conviene revisar la tabla de ciclos si el rendimiento es crítico.
- Esta técnica se solapa con la disyuntiva "struct vs. registro" tratada en otra entrada de esta base: usar X como puntero de "this" tiene sus propios costes de instrucciones no indexables por X.

## Referencias cruzadas
- (pendiente de enlazar) — relacionado con la entrada de tradeoffs entre structs y registros, y con la de diferencias 65816 vs 6502.
