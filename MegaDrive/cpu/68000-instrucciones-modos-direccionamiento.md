---
titulo: Motorola 68000 — instrucciones y modos de direccionamiento (base)
plataforma: [MegaDrive]
categoria: [cpu]
tags: [68000, asm, instrucciones, modos-direccionamiento, fundamentos]
estado: borrador
fecha: 2026-07-21
fuentes: [https://en.wikibooks.org/wiki/68000_Assembly/Addressing_Modes, https://www.nxp.com/docs/en/reference-manual/M68000PRM.pdf, https://www.liquisearch.com/motorola_68000/instruction_set_details]
---

# Motorola 68000 — instrucciones y modos de direccionamiento (base)

## Resumen

Entrada base que faltaba en esta carpeta: las demás entradas de `cpu/` (multiplicación/división, BCD, 64-bit, MOVEP) asumen que ya conoces el set de instrucciones y los modos de direccionamiento del 68000. Esta entrada cubre esos fundamentos — el "ABC" para leer o escribir cualquier ASM de Mega Drive, incluidos los disassemblies de Sonic Retro y las entradas de plutiedev.com ya presentes en esta base.

## Detalle técnico

### Registros

- **8 registros de datos**: `D0`-`D7` — de propósito general, operan como byte (`.b`, 8 bits), word (`.w`, 16 bits) o long (`.l`, 32 bits).
- **8 registros de dirección**: `A0`-`A7` — para direcciones de memoria; `A7` es el puntero de pila (SP). No soportan operaciones byte (siempre se tratan como word o long, y las operaciones word sobre `A7` afectan igualmente los 32 bits para mantener el stack alineado a word).
- **PC** (contador de programa) y **SR** (registro de estado: bits de condición `X N Z V C` en el byte bajo — CCR — más los bits de modo/máscara de interrupción en el byte alto, solo accesibles en modo supervisor).

### Modos de direccionamiento (14 modos, agrupados en 6 tipos)

| Tipo | Sintaxis | Descripción |
|---|---|---|
| Registro directo | `D0`, `A0` | El operando está en el registro. |
| Indirecto por registro | `(A0)` | El registro contiene la dirección de memoria del operando. |
| Indirecto con post-incremento | `(A0)+` | Como el anterior, y tras el acceso se incrementa `A0` en 1 (byte), 2 (word) o 4 (long) según el tamaño del operando. Típico para recorrer un array hacia adelante. |
| Indirecto con pre-decremento | `-(A0)` | Se decrementa `A0` (1/2/4 según tamaño) **antes** del acceso. Típico para construir una pila o recorrer hacia atrás. |
| Indirecto con desplazamiento de 16 bits con signo | `16(A0)` | Dirección = `A0` + desplazamiento. Útil para acceder a campos de una estructura apuntada por `A0`. |
| Indirecto indexado con desplazamiento de 8 bits con signo | `8(A0,D0)` o `8(A0,A1)` | Dirección = `A0` + registro índice + desplazamiento de 8 bits. Muy usado para tablas (p. ej. tablas de sprites, tablas de salto). |
| Relativo a PC, 16 bits | `16(PC)` | Como el desplazamiento de `A0` pero relativo al contador de programa — permite código reubicable. |
| Relativo a PC, indexado con 8 bits | `8(PC,D2)` | Igual que el indexado por registro de dirección, pero relativo a PC. |
| Absoluto | `$4000` o una etiqueta que el ensamblador traduce | Dirección fija en memoria (corta de 16 bits o larga de 32 bits). |
| Inmediato | `#$1234` | El propio valor va codificado en la instrucción, no una dirección. |

**Nota clave:** en `(A0)+` y `-(A0)`, el incremento/decremento depende del tamaño del operando de la instrucción (`.b`/`.w`/`.l`), no es siempre 1 — un error común es olvidar esto al recorrer arrays de words o longs.

### Categorías de instrucciones (56 instrucciones básicas en total)

- **Movimiento de datos:** `MOVE`, `MOVEA` (a registro de dirección), `MOVEM` (múltiples registros, típico para guardar/restaurar contexto en una interrupción), `MOVEP` (ver [`movep.md`](movep.md)), `LEA` (carga dirección efectiva), `PEA` (apila dirección efectiva), `EXG` (intercambia registros), `SWAP` (intercambia las dos words de un long).
- **Aritmética:** `ADD`/`ADDA`/`ADDI`/`ADDQ`, `SUB` (variantes equivalentes), `MULU`/`MULS`, `DIVU`/`DIVS` (ver [`multiplication-division.md`](multiplication-division.md)), `NEG`, `CLR`, `CMP`, `EXT` (extiende el signo de byte→word o word→long), y las variantes `X` (`ADDX`, `SUBX`, `NEGX`) para aritmética multi-precisión (ver [`64bit-math.md`](64bit-math.md)).
- **BCD:** `ABCD`, `SBCD`, `NBCD` (ver [`bcd-math.md`](bcd-math.md)).
- **Lógicas:** `AND`, `OR`, `EOR`, `NOT` — operan sobre byte, word o long.
- **Desplazamiento y rotación:** `ASL`/`ASR` (aritmético), `LSL`/`LSR` (lógico), `ROL`/`ROR` (rotación simple), `ROXL`/`ROXR` (rotación a través del bit Extend).
- **Manipulación de bits:** `BTST` (test), `BSET`, `BCLR`, `BCHG` — operan sobre un bit concreto de un registro o byte de memoria.
- **Control de programa:** `Bcc` (branch condicional, p. ej. `BEQ`, `BNE`, `BCS`), `BRA` (branch incondicional), `BSR`/`JSR` (llamada a subrutina), `RTS` (retorno), `JMP` (salto), `DBcc` (decrementa y branch — bucles clásicos en 68000), `Scc` (setea byte según condición).
- **Control de sistema:** `TRAP`, `STOP`, `RESET`, y las instrucciones que leen/modifican el `SR` — privilegiadas, solo ejecutables en modo supervisor (relevante para el manejo de interrupciones, ver [`../graficos/segaretro-interrupts.md`](../graficos/segaretro-interrupts.md)).

### Bits de condición (CCR, byte bajo del SR)

`X` (extend, usado por las instrucciones `X` para acarreo multi-precisión), `N` (negativo), `Z` (cero), `V` (overflow), `C` (carry) — se actualizan según el resultado de la mayoría de instrucciones aritméticas/lógicas y determinan qué ramas `Bcc`/`Scc`/`DBcc` se toman.

## Notas / gotchas

- `A7` es el stack pointer; en el 68000 hay en realidad dos: SSP (supervisor) y USP (usuario) — en Mega Drive, al no haber modo usuario real en el desarrollo típico, casi todo el código corre en supervisor y usa el SSP.
- Los accesos de word/long deben estar alineados a dirección par — acceder a una dirección impar con `.w`/`.l` genera una excepción de bus error (`Address Error`), un fallo clásico de principiante.
- Ver [`64bit-math.md`](64bit-math.md) para el uso práctico del bit `X` en aritmética extendida, y [`z80-jump-tables.md`](z80-jump-tables.md)/disassemblies de Sonic Retro para ver el modo indexado (`8(A0,D0)`) aplicado a tablas de salto reales.

## Referencias cruzadas

- [`multiplication-division.md`](multiplication-division.md), [`bcd-math.md`](bcd-math.md), [`64bit-math.md`](64bit-math.md), [`movep.md`](movep.md) (fuente plutiedev.com — todas asumen esta base)
- [Build y debug de ASM puro para Mega Drive](../herramientas/build-debug-asm-puro.md)
- [`../graficos/segaretro-interrupts.md`](../graficos/segaretro-interrupts.md)
