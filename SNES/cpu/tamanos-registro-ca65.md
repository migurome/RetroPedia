---
titulo: Gestión de tamaños de registro (8/16 bits) en ca65
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, ca65, ensamblador, registros, flags-m-x]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Register_sizes_in_ca65]
---

# Gestión de tamaños de registro (8/16 bits) en ca65

## Resumen
El 65816 cambia dinámicamente entre modos de 8 y 16 bits para acumulador e índices (flags M y X), pero el propio ensamblador no sabe en qué modo está la CPU en cada punto del código: necesita que se lo indiquen mediante directivas. ca65 resuelve esto con las directivas `.a8`/`.a16`/`.i8`/`.i16` y el modo `.smart`.

## Detalle técnico

### El problema de fondo
Cuando el ensamblador procesa una instrucción como `LDA #valor`, necesita saber si debe generar 1 o 2 bytes de operando inmediato, ya que el opcode (por ejemplo $A9 para LDA inmediato) es el mismo en ambos casos: lo que cambia es cuántos bytes le siguen. Un desajuste entre lo que el ensamblador cree y el tamaño real en tiempo de ejecución desalinea todo el código posterior, sin generar ningún error visible.

### Directivas de ca65

| Directiva | Afecta | Comportamiento |
|-----------|--------|---|
| `.a8` | Acumulador | Ensambla instrucciones siguientes asumiendo 8 bits |
| `.a16` | Acumulador | Ensambla instrucciones siguientes asumiendo 16 bits |
| `.i8` | Registros índice (X, Y) | Ensambla instrucciones siguientes asumiendo 8 bits |
| `.i16` | Registros índice (X, Y) | Ensambla instrucciones siguientes asumiendo 16 bits |

Estas directivas afectan a todas las instrucciones que aparecen después de ellas en el código fuente, hasta la siguiente directiva que cambie el tamaño.

### Ejemplos de ensamblado
```
.a8
lda #1    ; Genera: A9 01
cmp #1    ; Genera: C9 01

.a16
lda #1    ; Genera: A9 01 00
cmp #1    ; Genera: C9 01 00

.i8
ldx #1    ; Genera: A2 01
cpx #1    ; Genera: E0 01

.i16
ldx #1    ; Genera: A2 01 00
cpx #1    ; Genera: E0 01 00
```

### Modo `.smart`
Al activar `.smart`, ca65 hace que las propias instrucciones `REP` y `SEP` actúen también como directivas de cambio de tamaño automáticamente, reflejando los cambios reales que provocan sobre los flags M (acumulador) y X (índices) del procesador — sin tener que añadir `.a8`/`.a16`/`.i8`/`.i16` manualmente después de cada REP/SEP.

### Detección de tamaño en macros
Dentro de macros se puede consultar el tamaño actual con `.asize` (devuelve 8 o 16) e `.isize`, para adaptar el código generado:
```
.macro neg
  .if .asize = 8
    eor #$ff
  .else
    eor #$ffff
  .endif
  ina
.endmacro
```

## Notas / gotchas
- Fijar explícitamente los tamaños al inicio de cada rutina (en vez de asumir el estado heredado del código anterior) documenta el código y evita dependencias ocultas entre rutinas.
- Omitir las directivas tras un cambio real de tamaño (por ejemplo tras un REP/SEP sin `.smart`) desalinea catastróficamente el ensamblado de las instrucciones siguientes: no se produce ningún error de compilación, el binario resultante simplemente ejecuta datos como si fueran opcodes o viceversa.
- `.smart` ayuda mucho a evitar el problema anterior, pero delega en el ensamblador la detección del tamaño en cada REP/SEP; sigue siendo responsabilidad del programador asegurarse de que el operando de REP/SEP sea el correcto.
- Estas directivas son puramente informativas para el ensamblador: no generan código, no cambian el estado real de la CPU. El cambio real de flags M/X solo ocurre en tiempo de ejecución vía `REP`/`SEP`.

## Referencias cruzadas
- (pendiente de enlazar) — relacionado con la entrada de diferencias 65816 vs 6502 (flags M/X, REP/SEP).
