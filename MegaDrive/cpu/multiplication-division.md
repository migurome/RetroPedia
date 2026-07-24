---
titulo: Multiplicación y división en el 68000
plataforma: [MegaDrive]
categoria: [cpu]
tags: [68000, asm, multiplicacion, division, muls, divs]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/multiplication-division]
---

# Multiplicación y división en el 68000

## Resumen

Esta entrada describe las instrucciones de multiplicación (`muls.w`, `mulu.w`) y división (`divs.w`, `divu.w`) del Motorola 68000, sus restricciones de operandos, cómo se organiza el resultado, y su coste en ciclos de CPU — relevante para el desarrollo en Mega Drive/Genesis.

## Detalle técnico

### Instrucciones de multiplicación

El 68000 ofrece dos instrucciones de multiplicación:
- `muls.w` para operaciones con signo
- `mulu.w` para operaciones sin signo

Una característica clave: "the two operands are 16-bit, but the result is 32-bit" (los dos operandos son de 16 bits, pero el resultado es de 32 bits). Esto asegura que el resultado siempre cabe, sin importar la magnitud.

**Requisitos de operandos:**
El destino debe ser un registro de datos (d0-d7). Los orígenes son más flexibles, aceptando registros de datos, valores inmediatos como `#123`, o referencias a memoria, con la excepción de que los registros de dirección (a0-a7) no pueden ser origen.

Ejemplos válidos proporcionados:

```
muls.w  d1, d0
muls.w  #123, d0
muls.w  (MyVar), d0
muls.w  (a0)+, d0
```

### Instrucciones de división

Existen dos variantes de división:
- `divs.w` para división con signo
- `divu.w` para división sin signo

**Estructura:** "The destination operand is 32-bit" (el operando destino es de 32 bits) mientras que "The source operand is 16-bit" (el operando origen es de 16 bits). El resultado se organiza así: el cociente en la palabra (word) inferior, y el resto en la palabra superior del destino.

**Comportamiento de redondeo:** cuando el resultado no es un número entero, "the quotient is 'truncated'" (el cociente se "trunca"), "rounding towards zero" (redondeando hacia cero).

**La comprobación de errores requiere vigilancia:** la división por cero lanza una excepción. Comprobar el flag de overflow tras la ejecución, o comprobar el divisor de antemano, evita fallos:

```
tst.w   d1
beq     @Error
divs.w  d1, d0
bvs     @Error
```

### Características de rendimiento

La multiplicación requiere "between 38 and 70 cycles" (entre 38 y 70 ciclos), y la velocidad se correlaciona con la cantidad de bits activados (a 1) en el operando origen. La división demanda consistentemente "around 140 cycles" (alrededor de 140 ciclos), con pocas oportunidades de optimización disponibles.

## Notas / gotchas

- Los registros de dirección (a0-a7) no pueden usarse como origen en `muls`/`mulu`.
- La división por cero provoca una excepción — hay que comprobar el divisor antes de dividir, o comprobar el flag de overflow (`bvs`) después.
- El cociente de la división se trunca hacia cero, no redondea al entero más cercano.
- La multiplicación es más rápida cuantos menos bits a 1 tenga el operando origen (entre 38 y 70 ciclos); la división ronda siempre los 140 ciclos, sin mucho margen de optimización.

## Referencias cruzadas

- (pendiente de enlazar)
