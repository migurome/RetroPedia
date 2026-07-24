---
titulo: La instrucción MOVEP
plataforma: [MegaDrive]
categoria: [cpu]
tags: [68000, asm, movep, dma, vdp, sram]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/movep]
---

# La instrucción MOVEP

## Resumen

MOVEP es una instrucción del 68000 que mueve bytes alternos entre un registro de datos y memoria, útil para escribir en periféricos organizados en bytes (como comandos DMA del VDP o SRAM de 8 bits) sin tener que usar rotaciones y movimientos manuales.

## Detalle técnico

### Funcionalidad central

La instrucción MOVEP escribe cada byte alterno de los registros de datos (d0-d7) en memoria, o lee cada byte alterno de memoria hacia un registro. Como explica la fuente, `"movep.l d0, 0(a0)"` logra en una sola operación lo que de otro modo requeriría múltiples instrucciones de rotación y movimiento.

### Variantes de la instrucción

Existen cuatro combinaciones de sintaxis:
- `movep.w dn, nn(an)` — escribir word (palabra)
- `movep.w nn(an), dn` — leer word
- `movep.l dn, nn(an)` — escribir long
- `movep.l nn(an), dn` — leer long

Donde n va de 0 a 7, y nn abarca de -32768 a +32767.

### Ejemplo de ensamblado de comando DMA

```
move.l  #(VDPREG_DMALEN_H<<16)|VDPREG_DMALEN_L, (a0)+
move.l  #(VDPREG_DMASRC_H<<16)|VDPREG_DMASRC_M, (a0)+
move.w  #VDPREG_DMASRC_L, (a0)+
movep.l d0, -7(a0)
movep.w d1, -9(a0)
```

### Aplicación en SRAM

Para sistemas de SRAM de 8 bits, valores multi-byte (como puntuaciones altas de 32 bits) pueden almacenarse usando "movep.l for each score" (movep.l para cada puntuación) sin manipulación compleja de bits.

**Ventaja clave**: reduce la complejidad del código a la vez que mejora la velocidad de ejecución comparado con los enfoques manuales de desplazamiento de bits.

## Notas / gotchas

- MOVEP mueve bytes alternos (no consecutivos) — está pensado para periféricos/buses organizados en bytes, no para mover bloques de memoria contiguos normales.
- El rango del desplazamiento `nn` está limitado a -32768…+32767, igual que otros modos de direccionamiento con desplazamiento del 68000.
- Es especialmente útil para comandos DMA del VDP y para acceso a SRAM de 8 bits (por ejemplo, tablas de puntuaciones altas).

## Referencias cruzadas

- (pendiente de enlazar)
