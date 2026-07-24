---
titulo: Tablas de salto en Z80
plataforma: [MegaDrive]
categoria: [cpu]
tags: [z80, jump-table, ensamblador, optimizacion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/z80-jump-tables]
---

# Tablas de salto en Z80

## Resumen

Las tablas de salto permiten bifurcar la ejecución a diferentes subrutinas según un valor de índice concreto. El Z80 no dispone de un salto indexado directo como el 68000, por lo que se recurre a un truco que aprovecha la instrucción `jr` (salto relativo) y la auto-modificación de su operando.

## Detalle técnico

El enfoque optimizado para Z80 utiliza la instrucción `jr` con un operando de byte. La estrategia consiste en colocar la tabla de saltos (una serie de instrucciones `jp`) inmediatamente después de la instrucción `jr`, sobrescribiendo el operando de dicho `jr` para dirigirlo a la entrada deseada de la tabla. El símbolo `$` representa la dirección de la instrucción actual.

```
ld b, a
add a, b
add a, b

ld ($+4), a
jr $

jp Routine1
jp Routine2
jp Routine3
...
```

Este fragmento asume que el índice reside en el registro A. El código multiplica el índice por 3 (el tamaño en bytes de la instrucción `JP`), sobrescribe el operando del salto relativo, y a continuación ejecuta el salto hacia la tabla.

## Notas / gotchas

- Índice máximo soportado: 42 (multiplicaciones mayores generan desbordamiento del operando de byte usado por `jr`).
- Optimización recomendada por el autor: mantener los índices ya premultiplicados por 3 evita cálculos repetidos y reduce los ciclos de procesamiento.

## Referencias cruzadas

- (pendiente de enlazar)
