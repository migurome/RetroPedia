---
titulo: Sistemas numéricos — decimal, binario y hexadecimal
plataforma: [General]
categoria: [fundamentos]
tags: [numeros, binario, hexadecimal, decimal, conversion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/dec-bin-hex]
---

# Sistemas numéricos — decimal, binario y hexadecimal

## Resumen

Esta entrada introduce los tres sistemas numéricos usados habitualmente en el desarrollo de videojuegos retro: decimal (base 10), binario (base 2) y hexadecimal (base 16), junto con la forma de convertir entre ellos y las convenciones de notación usadas en el 68000.

## Detalle técnico

### Decimal (Base 10)

El decimal usa diez dígitos (0-9). Cada posición representa una potencia de 10:

"123 = 1×10² + 2×10¹ + 3×10⁰" o "123 = 1×100 + 2×10 + 3×1"

### Binario (Base 2)

El binario usa únicamente los dígitos 0 y 1, expresando valores como potencias de dos:

"1010 (bin) = 1×2³ + 0×2² + 1×2¹ + 0×2⁰" que equivale a "1×8 + 0×4 + 1×2 + 0×1"

### Hexadecimal (Base 16)

El hexadecimal emplea dieciséis dígitos: "0 1 2 3 4 5 6 7 8 9 A B C D E F" donde A-F representan los valores 10-15.

Ejemplo de conversión: "5E6A (hex) = 5×16³ + 14×16² + 6×16¹ + 10×16⁰" o "5×4096 + 14×256 + 6×16 + 10×1"

**Correspondencia hexadecimal-binario:**

Cada dígito hexadecimal equivale exactamente a 4 bits binarios:

- 0=0000, 1=0001, 2=0010, 3=0011, 4=0100, 5=0101, 6=0110, 7=0111
- 8=1000, 9=1001, A=1010, B=1011, C=1100, D=1101, E=1110, F=1111

**Ejemplo:** el hexadecimal 1A0 se convierte a binario como "0001 1010 0000" (o 110100000).

### Convenciones de programación

La mayoría de los lenguajes usan el prefijo "0x" para hexadecimal (por ejemplo, 0x1A0). En ensamblador del 68000, se usa el prefijo "$" (por ejemplo, $1A0).

## Notas / gotchas

- No confundir el prefijo "$" (usado en ensamblador 68000 para hexadecimal) con el prefijo "0x" habitual en lenguajes como C.
- Cada dígito hexadecimal corresponde exactamente a un nibble (4 bits), lo que hace muy directa la conversión hex ↔ binario dígito a dígito.

## Referencias cruzadas

- (pendiente de enlazar)
