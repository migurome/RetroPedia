---
titulo: Matemáticas BCD en el 68000
plataforma: [MegaDrive]
categoria: [cpu]
tags: [68000, asm, bcd, abcd, sbcd, decimal-codificado-en-binario]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/bcd-math]
---

# Matemáticas BCD en el 68000

## Resumen

Esta entrada explica qué es BCD (Binary Coded Decimal / Decimal Codificado en Binario) y cómo usarlo en el 68000 para extraer dígitos decimales fácilmente y realizar sumas/restas BCD mediante las instrucciones `abcd` y `sbcd`.

## Detalle técnico

### Concepto central

"BCD (Binary Coded Decimal) is a way to store decimal numbers that uses four bits per digit" (BCD es una forma de almacenar números decimales que usa cuatro bits por dígito), con la ventaja clave de que "you can split away the digits with only bit shifting (instead of slow divisions)" (se pueden separar los dígitos usando sólo desplazamientos de bits, en lugar de divisiones lentas).

### Método de extracción de dígitos

La técnica consiste en rotar cuatro bits y aplicar una máscara:
- Rotar a la izquierda 4 bits usando `rol.l`
- Aplicar máscara con `and.w #$0F` para aislar los cuatro bits inferiores
- Procesar los 8 dígitos de un valor BCD de 32 bits usando un contador de bucle `dbf`

### Suma de un solo byte

La instrucción `abcd` realiza suma BCD sobre pares de registros. El proceso requiere limpiar los flags primero con `and #0, ccr`, y luego ejecutar `abcd d1, d0` para sumar dos registros de datos que contienen valores BCD.

### Suma multi-byte

Para números más grandes en memoria, se usan registros de dirección apuntando a las posiciones después de los datos. La instrucción `abcd -(a1), -(a0)` suma bytes con direccionamiento predecremento, repitiéndose cuatro veces para valores de 32 bits.

### Resta

"Substraction works the same way but you use `sbcd` instead of `abcd`. That's all." (La resta funciona igual, pero se usa `sbcd` en lugar de `abcd`. Eso es todo.)

### Detección de desbordamiento (overflow)

Se comprueba el flag de carry (acarreo) tras la operación usando `bcc` (branch if carry clear / saltar si el carry está limpio) o `bcs` (branch if carry set / saltar si el carry está activado) para detectar resultados que exceden la capacidad de almacenamiento.

## Notas / gotchas

- Antes de usar `abcd`/`sbcd` hay que limpiar los flags con `and #0, ccr`.
- La suma/resta multi-byte con `abcd -(a1), -(a0)` / direccionamiento predecremento se repite tantas veces como bytes tenga el valor (cuatro veces para 32 bits).
- Para comprobar overflow (desbordamiento) tras `abcd`/`sbcd`, se usa el flag de carry (`bcc`/`bcs`).
- La extracción de dígitos BCD mediante `rol.l` + `and.w #$0F` evita divisiones lentas, aprovechando que cada dígito ocupa exactamente 4 bits.

## Referencias cruzadas

- (pendiente de enlazar)
