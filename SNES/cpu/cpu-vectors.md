---
titulo: Vectores de interrupción y reset del 65816 en SNES
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, interrupciones, vectores, reset, nmi, irq]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/CPU_vectors]
---

# Vectores de interrupción y reset del 65816 en SNES

## Resumen
Los vectores de CPU son direcciones fijas al final del mapa de memoria del banco $00 donde el 65816 busca la dirección de la rutina a ejecutar cuando ocurre un reset o una interrupción (NMI, IRQ, BRK, COP). Existen dos tablas distintas según el modo del procesador: modo nativo y modo emulación (6502).

## Detalle técnico
Cuando se produce una interrupción, la CPU lee la dirección del manejador correspondiente desde la tabla de vectores ubicada en el banco $00. El banco de destino del salto siempre se fuerza a $00, y cada vector ocupa 16 bits (2 bytes, little-endian).

**Tabla de vectores — modo nativo (65C816):**

| Vector | Dirección | Uso |
|--------|-----------|-----|
| COP | $FFE4-FFE5 | Instrucción COP |
| BRK | $FFE6-FFE7 | Instrucción BRK |
| (ABORT) | $FFE8-FFE9 | No usado en el S-CPU 5A22 |
| NMI | $FFEA-FFEB | Interrupción de vblank o entrada /NMI |
| (ninguno) | $FFEC-FFED | Reservado |
| IRQ | $FFEE-FFEF | Interrupción por timer H/V o /IRQ externa |

**Tabla de vectores — modo emulación (6502):**

| Vector | Dirección | Uso |
|--------|-----------|-----|
| COP | $FFF4-FFF5 | Instrucción COP |
| (ninguno) | $FFF6-FFF7 | Reservado |
| (ABORT) | $FFF8-FFF9 | No usado en el S-CPU 5A22 |
| NMI | $FFFA-FFFB | Entrada /NMI |
| RESET | $FFFC-FFFD | Reset (siempre arranca en modo 6502) |
| IRQ/BRK | $FFFE-FFFF | Instrucción BRK, o interrupción externa |

La diferencia clave entre ambas tablas es el rango de direcciones: el modo nativo usa $FFE4-$FFEF, mientras que el modo emulación usa $FFF4-$FFFF.

## Notas / gotchas
- El vector RESET solo existe en la tabla de modo emulación: el 65816 siempre arranca en modo emulación (6502), sin importar en qué modo terminó la ejecución anterior.
- El vector ABORT no tiene uso real en el S-CPU 5A22 de la SNES (a diferencia de otros sistemas basados en 65816), por lo que normalmente se ignora en desarrollo homebrew.
- Hay huecos "reservados" en ambas tablas ($FFEC-FFED en modo nativo, $FFF6-FFF7 en emulación) que no deben usarse.
- Es fácil olvidar que, tras un reset, el código arranca en modo emulación con acumulador e índices de 8 bits y stack pointer fijo en la página $01xx; el cambio a modo nativo (vía `CLC`/`XCE`) debe hacerse explícitamente antes de usar vectores/rutinas pensadas para modo nativo.

## Referencias cruzadas
- (pendiente de enlazar)
