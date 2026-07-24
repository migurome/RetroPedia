---
titulo: Matemáticas de 64 bits en el 68000
plataforma: [MegaDrive]
categoria: [cpu]
tags: [68000, asm, 64-bit, addx, subx, extend-flag]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/64bit-math]
---

# Matemáticas de 64 bits en el 68000

## Resumen

Esta entrada explica cómo el 68000 — un CPU fundamentalmente de 16 bits con capacidad de instrucciones de 32 bits — puede manejar números de 64 bits (e incluso mayores) mediante instrucciones extendidas (`addx`, `subx`, `negx`) y el mecanismo del flag Extend.

## Detalle técnico

### El flag Extend

El flag Extend es distinto de los flags Carry y Overflow. Como señala el autor: "the Extend flag holds the carry or borrow of the last operation (note that this is _not_ the same thing as the Carry or Overflow flags)" (el flag Extend contiene el acarreo o préstamo de la última operación; nótese que esto _no_ es lo mismo que los flags Carry u Overflow).

Distinción crítica: "Extend and Carry flags are _not_ the same, and a few instructions will affect them differently (e.g. `move` will clear Carry but not touch Extend). Extend is used for working with large numbers, Carry is used for comparisons." (Los flags Extend y Carry _no_ son lo mismo, y algunas instrucciones los afectan de forma diferente — por ejemplo, `move` limpia Carry pero no toca Extend. Extend se usa para trabajar con números grandes, Carry se usa para comparaciones.)

Las instrucciones extendidas (`addx`, `subx`, `negx`) propagan el flag Extend a través de las mitades de la operación, permitiendo aritmética de precisión arbitraria.

### Suma

La suma estándar de 64 bits usa dos instrucciones, con registros que representan las mitades superior e inferior:

```
add.l   d3, d1
addx.l  d2, d0
```

Para operaciones basadas en memoria con almacenamiento big-endian:

```
lea     8(a0), a0
lea     8(a1), a1

and.b   #$00, ccr
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
```

Limitación clave: "there's no immediate form for the `addx` instruction" (no existe una forma inmediata para la instrucción `addx`). Solución alternativa usando `moveq`:

```
moveq   #100, d2
add.l   d2, d1
moveq   #0, d2
addx.l  d2, d0
```

### Resta

Patrón idéntico al de la suma, sustituyendo `add` y `addx` por `sub` y `subx`:

```
sub.l   d3, d1
subx.l  d2, d0
```

### Negación

Invertir el signo usando `neg` y `negx`:

```
neg.l   d1
negx.l  d0
```

A diferencia de `addx`/`subx`, "`negx` can be used with most operands (except `a0-a7`)" (`negx` puede usarse con la mayoría de operandos, excepto `a0-a7`).

### Comparación

No existe una instrucción `cmpx`. La solución alternativa requiere registros temporales o memoria:

```
move.l  d1, d5
move.l  d0, d4
sub.l   d3, d5
subx.l  d2, d4
```

Para detección de desigualdad:

```
cmp.l   d3, d1
bne     Different
cmp.l   d2, d0
bne     Different
```

Comprobación del signo de la mitad superior:

```
tst.l   d0
bpl     Positive

tst.l   d0
bmi     Negative
```

### Operaciones booleanas

`and`, `or`, `eor`, `not` operan de forma independiente sobre cada mitad:

```
and.l   d3, d1
and.l   d2, d0

or.l    d3, d1
or.l    d2, d0

eor.l   d3, d1
eor.l   d2, d0

not.l   d1
not.l   d0
```

### Extensión de 32 bits a 64 bits

Expansión sin signo:

```
moveq   #0, d0
```

Expansión con signo:

```
tst.l   d1
smi.b   d0
ext.w   d0
ext.l   d0
```

### Valores más grandes

Suma de 128 bits a través de registros:

```
add.l   d7, d3
addx.l  d6, d2
addx.l  d5, d1
addx.l  d4, d0
```

Suma de 256 bits basada en memoria:

```
lea     32(a0), a0
lea     32(a1), a1

and.b   #$00, ccr
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
addx.l  -(a1), -(a0)
```

Las instrucciones extendidas soportan variantes de byte, word y long para tamaños que no sean múltiplos de 32 bits.

## Notas / gotchas

- El flag Extend no es lo mismo que Carry u Overflow — algunas instrucciones (como `move`) limpian Carry pero no tocan Extend.
- No existe forma inmediata para `addx`; hay que usar `moveq` para cargar el valor inmediato en un registro temporal primero.
- No existe instrucción `cmpx`; hay que emular la comparación de 64 bits con registros temporales (`sub`/`subx`) o comparando ambas mitades por separado con `cmp`.
- `negx` acepta la mayoría de operandos, salvo los registros de dirección (`a0-a7`).
- Antes de encadenar `addx`/`subx` hay que limpiar los flags con `and.b #$00, ccr`.

## Referencias cruzadas

- (pendiente de enlazar)
