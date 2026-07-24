---
titulo: Set de instrucciones del SPC-700
plataforma: [SNES]
categoria: [sonido]
tags: [spc700, s-smp, asm, opcodes, instrucciones, audio]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/SPC-700_instruction_set]
---

# Set de instrucciones del SPC-700

## Resumen

Referencia completa del set de instrucciones del procesador SPC-700 (el núcleo del S-SMP, la CPU de sonido de la SNES). Cubre los registros internos, los modos de direccionamiento, las diferencias respecto al 6502/65C02 en el que se basa, y la tabla completa de opcodes con su tamaño en bytes, ciclos de reloj y flags afectados.

## Detalle técnico

### Arquitectura y registros

El SPC-700 corre a 1.024 MHz dentro del S-SMP y tiene un espacio de direcciones de 16 bits, con un conjunto de registros similar al del 6502:

- **A**: acumulador de 8 bits
- **X, Y**: registros de índice de 8 bits
- **YA**: par de 16 bits (A es el byte bajo, Y es el byte alto)
- **PC**: contador de programa
- **SP**: puntero de pila
- **PSW**: palabra de estado del procesador, con flags en formato `NVPBHIZC`

### Modos de direccionamiento

El procesador soporta: inmediato (`#imm`), página directa (`dp`), absoluto (`!abs`), relativo (`rel`), indexado en página directa con post-incremento (`(X)+`), y varios modos indirectos indexados (`[dp+X]`, `[dp]+Y`, etc.).

### Diferencias frente al 6502

**Eliminado respecto al 6502:** modo decimal, `JMP` no indexado, ciertas operaciones de carga indexadas, la instrucción `BIT`, y los modos `abs,X` para instrucciones de lectura-modificación-escritura.

**Añadido respecto al 6502:** reubicación de la página directa, operaciones de 16 bits (`MOVW`, `ADDW`, `SUBW`, `CMPW`, `INCW`, `DECW`, `MUL`, `DIV`), instrucciones de bucle (`CBNE`, `DBNZ`), llamadas abreviadas (`PCALL`, `TCALL`), y operaciones de bit único sobre memoria.

En las tablas de flags, cada carácter representa un bit de `PSW` en el orden `N V P B H I Z C`; un punto (`.`) indica que la instrucción no modifica ese flag.

### Tabla completa de instrucciones

**Movimiento de 8 bits (memoria → registro)**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| MOV A, #imm | E8 | 2 | 2 | N.....Z. |
| MOV A, (X) | E6 | 1 | 3 | N.....Z. |
| MOV A, (X)+ | BF | 1 | 4 | N.....Z. |
| MOV A, dp | E4 | 2 | 3 | N.....Z. |
| MOV A, dp+X | F4 | 2 | 4 | N.....Z. |
| MOV A, !abs | E5 | 3 | 4 | N.....Z. |
| MOV A, !abs+X | F5 | 3 | 5 | N.....Z. |
| MOV A, !abs+Y | F6 | 3 | 5 | N.....Z. |
| MOV A, [dp+X] | E7 | 2 | 6 | N.....Z. |
| MOV A, [dp]+Y | F7 | 2 | 6 | N.....Z. |
| MOV X, #imm | CD | 2 | 2 | N.....Z. |
| MOV X, dp | F8 | 2 | 3 | N.....Z. |
| MOV X, dp+Y | F9 | 2 | 4 | N.....Z. |
| MOV X, !abs | E9 | 3 | 4 | N.....Z. |
| MOV Y, #imm | 8D | 2 | 2 | N.....Z. |
| MOV Y, dp | EB | 2 | 3 | N.....Z. |
| MOV Y, dp+X | FB | 2 | 4 | N.....Z. |
| MOV Y, !abs | EC | 3 | 4 | N.....Z. |

**Movimiento de 8 bits (registro → memoria)**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| MOV (X), A | C6 | 1 | 4 | ........ |
| MOV (X)+, A | AF | 1 | 4 | ........ |
| MOV dp, A | C4 | 2 | 4 | ........ |
| MOV dp+X, A | D4 | 2 | 5 | ........ |
| MOV !abs, A | C5 | 3 | 5 | ........ |
| MOV !abs+X, A | D5 | 3 | 6 | ........ |
| MOV !abs+Y, A | D6 | 3 | 6 | ........ |
| MOV [dp+X], A | C7 | 2 | 7 | ........ |
| MOV [dp]+Y, A | D7 | 2 | 7 | ........ |
| MOV dp, X | D8 | 2 | 4 | ........ |
| MOV dp+Y, X | D9 | 2 | 5 | ........ |
| MOV !abs, X | C9 | 3 | 5 | ........ |
| MOV dp, Y | CB | 2 | 4 | ........ |
| MOV dp+X, Y | DB | 2 | 5 | ........ |
| MOV !abs, Y | CC | 3 | 5 | ........ |

**Movimiento de 8 bits (registro → registro)**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| MOV A, X | 7D | 1 | 2 | N.....Z. |
| MOV A, Y | DD | 1 | 2 | N.....Z. |
| MOV X, A | 5D | 1 | 2 | N.....Z. |
| MOV Y, A | FD | 1 | 2 | N.....Z. |
| MOV X, SP | 9D | 1 | 2 | N.....Z. |
| MOV SP, X | BD | 1 | 2 | ........ |
| MOV dp, dp | FA | 3 | 5 | ........ |
| MOV dp, #imm | 8F | 3 | 5 | ........ |

**Aritmética de 8 bits — ADC / SBC / CMP**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| ADC A, #imm | 88 | 2 | 2 | NV..H.ZC |
| ADC A, (X) | 86 | 1 | 3 | NV..H.ZC |
| ADC A, dp | 84 | 2 | 3 | NV..H.ZC |
| ADC A, dp+X | 94 | 2 | 4 | NV..H.ZC |
| ADC A, !abs | 85 | 3 | 4 | NV..H.ZC |
| ADC A, !abs+X | 95 | 3 | 5 | NV..H.ZC |
| ADC A, !abs+Y | 96 | 3 | 5 | NV..H.ZC |
| ADC A, [dp+X] | 87 | 2 | 6 | NV..H.ZC |
| ADC A, [dp]+Y | 97 | 2 | 6 | NV..H.ZC |
| ADC (X), (Y) | 99 | 1 | 5 | NV..H.ZC |
| ADC dp, dp | 89 | 3 | 6 | NV..H.ZC |
| ADC dp, #imm | 98 | 3 | 5 | NV..H.ZC |
| SBC A, #imm | A8 | 2 | 2 | NV..H.ZC |
| SBC A, (X) | A6 | 1 | 3 | NV..H.ZC |
| SBC A, dp | A4 | 2 | 3 | NV..H.ZC |
| SBC A, dp+X | B4 | 2 | 4 | NV..H.ZC |
| SBC A, !abs | A5 | 3 | 4 | NV..H.ZC |
| SBC A, !abs+X | B5 | 3 | 5 | NV..H.ZC |
| SBC A, !abs+Y | B6 | 3 | 5 | NV..H.ZC |
| SBC A, [dp+X] | A7 | 2 | 6 | NV..H.ZC |
| SBC A, [dp]+Y | B7 | 2 | 6 | NV..H.ZC |
| SBC (X), (Y) | B9 | 1 | 5 | NV..H.ZC |
| SBC dp, dp | A9 | 3 | 6 | NV..H.ZC |
| SBC dp, #imm | B8 | 3 | 5 | NV..H.ZC |
| CMP A, #imm | 68 | 2 | 2 | N.....ZC |
| CMP A, (X) | 66 | 1 | 3 | N.....ZC |
| CMP A, dp | 64 | 2 | 3 | N.....ZC |
| CMP A, dp+X | 74 | 2 | 4 | N.....ZC |
| CMP A, !abs | 65 | 3 | 4 | N.....ZC |
| CMP A, !abs+X | 75 | 3 | 5 | N.....ZC |
| CMP A, !abs+Y | 76 | 3 | 5 | N.....ZC |
| CMP A, [dp+X] | 67 | 2 | 6 | N.....ZC |
| CMP A, [dp]+Y | 77 | 2 | 6 | N.....ZC |
| CMP (X), (Y) | 79 | 1 | 5 | N.....ZC |
| CMP dp, dp | 69 | 3 | 6 | N.....ZC |
| CMP dp, #imm | 78 | 3 | 5 | N.....ZC |
| CMP X, #imm | C8 | 2 | 2 | N.....ZC |
| CMP X, dp | 3E | 2 | 3 | N.....ZC |
| CMP X, !abs | 1E | 3 | 4 | N.....ZC |
| CMP Y, #imm | AD | 2 | 2 | N.....ZC |
| CMP Y, dp | 7E | 2 | 3 | N.....ZC |
| CMP Y, !abs | 5E | 3 | 4 | N.....ZC |

**Lógica booleana de 8 bits — AND / OR / EOR**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| AND A, #imm | 28 | 2 | 2 | N.....Z. |
| AND A, (X) | 26 | 1 | 3 | N.....Z. |
| AND A, dp | 24 | 2 | 3 | N.....Z. |
| AND A, dp+X | 34 | 2 | 4 | N.....Z. |
| AND A, !abs | 25 | 3 | 4 | N.....Z. |
| AND A, !abs+X | 35 | 3 | 5 | N.....Z. |
| AND A, !abs+Y | 36 | 3 | 5 | N.....Z. |
| AND A, [dp+X] | 27 | 2 | 6 | N.....Z. |
| AND A, [dp]+Y | 37 | 2 | 6 | N.....Z. |
| AND (X), (Y) | 39 | 1 | 5 | N.....Z. |
| AND dp, dp | 29 | 3 | 6 | N.....Z. |
| AND dp, #imm | 38 | 3 | 5 | N.....Z. |
| OR A, #imm | 08 | 2 | 2 | N.....Z. |
| OR A, (X) | 06 | 1 | 3 | N.....Z. |
| OR A, dp | 04 | 2 | 3 | N.....Z. |
| OR A, dp+X | 14 | 2 | 4 | N.....Z. |
| OR A, !abs | 05 | 3 | 4 | N.....Z. |
| OR A, !abs+X | 15 | 3 | 5 | N.....Z. |
| OR A, !abs+Y | 16 | 3 | 5 | N.....Z. |
| OR A, [dp+X] | 07 | 2 | 6 | N.....Z. |
| OR A, [dp]+Y | 17 | 2 | 6 | N.....Z. |
| OR (X), (Y) | 19 | 1 | 5 | N.....Z. |
| OR dp, dp | 09 | 3 | 6 | N.....Z. |
| OR dp, #imm | 18 | 3 | 5 | N.....Z. |
| EOR A, #imm | 48 | 2 | 2 | N.....Z. |
| EOR A, (X) | 46 | 1 | 3 | N.....Z. |
| EOR A, dp | 44 | 2 | 3 | N.....Z. |
| EOR A, dp+X | 54 | 2 | 4 | N.....Z. |
| EOR A, !abs | 45 | 3 | 4 | N.....Z. |
| EOR A, !abs+X | 55 | 3 | 5 | N.....Z. |
| EOR A, !abs+Y | 56 | 3 | 5 | N.....Z. |
| EOR A, [dp+X] | 47 | 2 | 6 | N.....Z. |
| EOR A, [dp]+Y | 57 | 2 | 6 | N.....Z. |
| EOR (X), (Y) | 59 | 1 | 5 | N.....Z. |
| EOR dp, dp | 49 | 3 | 6 | N.....Z. |
| EOR dp, #imm | 58 | 3 | 5 | N.....Z. |

**Incremento / decremento de 8 bits**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| INC A | BC | 1 | 2 | N.....Z. |
| INC dp | AB | 2 | 4 | N.....Z. |
| INC dp+X | BB | 2 | 5 | N.....Z. |
| INC !abs | AC | 3 | 5 | N.....Z. |
| INC X | 3D | 1 | 2 | N.....Z. |
| INC Y | FC | 1 | 2 | N.....Z. |
| DEC A | 9C | 1 | 2 | N.....Z. |
| DEC dp | 8B | 2 | 4 | N.....Z. |
| DEC dp+X | 9B | 2 | 5 | N.....Z. |
| DEC !abs | 8C | 3 | 5 | N.....Z. |
| DEC X | 1D | 1 | 2 | N.....Z. |
| DEC Y | DC | 1 | 2 | N.....Z. |

**Desplazamientos y rotaciones de 8 bits**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| ASL A | 1C | 1 | 2 | N.....ZC |
| ASL dp | 0B | 2 | 4 | N.....ZC |
| ASL dp+X | 1B | 2 | 5 | N.....ZC |
| ASL !abs | 0C | 3 | 5 | N.....ZC |
| LSR A | 5C | 1 | 2 | N.....ZC |
| LSR dp | 4B | 2 | 4 | N.....ZC |
| LSR dp+X | 5B | 2 | 5 | N.....ZC |
| LSR !abs | 4C | 3 | 5 | N.....ZC |
| ROL A | 3C | 1 | 2 | N.....ZC |
| ROL dp | 2B | 2 | 4 | N.....ZC |
| ROL dp+X | 3B | 2 | 5 | N.....ZC |
| ROL !abs | 2C | 3 | 5 | N.....ZC |
| ROR A | 7C | 1 | 2 | N.....ZC |
| ROR dp | 6B | 2 | 4 | N.....ZC |
| ROR dp+X | 7B | 2 | 5 | N.....ZC |
| ROR !abs | 6C | 3 | 5 | N.....ZC |
| XCN A | 9F | 1 | 5 | N.....Z. |

**Operaciones de 16 bits**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| MOVW YA, dp | BA | 2 | 5 | N.....Z. |
| MOVW dp, YA | DA | 2 | 4 | ........ |
| INCW dp | 3A | 2 | 6 | N.....Z. |
| DECW dp | 1A | 2 | 6 | N.....Z. |
| ADDW YA, dp | 7A | 2 | 5 | NV..H.ZC |
| SUBW YA, dp | 9A | 2 | 5 | NV..H.ZC |
| CMPW YA, dp | 5A | 2 | 4 | N.....ZC |
| MUL YA | CF | 1 | 9 | N.....Z. |
| DIV YA, X | 9E | 1 | 12 | NV..H.Z. |

**Ajuste decimal**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| DAA A | DF | 1 | 3 | N.....ZC |
| DAS A | BE | 1 | 3 | N.....ZC |

**Saltos condicionales / incondicionales**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| BRA rel | 2F | 2 | 4 | ........ |
| BEQ rel | F0 | 2 | 2/4 | ........ |
| BNE rel | D0 | 2 | 2/4 | ........ |
| BCS rel | B0 | 2 | 2/4 | ........ |
| BCC rel | 90 | 2 | 2/4 | ........ |
| BVS rel | 70 | 2 | 2/4 | ........ |
| BVC rel | 50 | 2 | 2/4 | ........ |
| BMI rel | 30 | 2 | 2/4 | ........ |
| BPL rel | 10 | 2 | 2/4 | ........ |
| BBS dp, bit, rel | x3 | 3 | 5/7 | ........ |
| BBC dp, bit, rel | y3 | 3 | 5/7 | ........ |
| CBNE dp, rel | 2E | 3 | 5/7 | ........ |
| CBNE dp+X, rel | DE | 3 | 6/8 | ........ |
| DBNZ dp, rel | 6E | 3 | 5/7 | ........ |
| DBNZ Y, rel | FE | 2 | 4/6 | ........ |
| JMP !abs | 5F | 3 | 3 | ........ |
| JMP [!abs+X] | 1F | 3 | 6 | ........ |

Nota de notación: en `BBS`/`BBC`, `x3`/`y3` indican que el opcode varía según el número de bit probado (son familias de 8 opcodes cada una, uno por bit 0-7).

**Subrutinas**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| CALL !abs | 3F | 3 | 8 | ........ |
| PCALL up | 4F | 2 | 6 | ........ |
| TCALL n | n1 | 1 | 8 | ........ |
| BRK | 0F | 1 | 8 | ...1.0.. |
| RET | 6F | 1 | 5 | ........ |
| RETI | 7F | 1 | 6 | NVPBHIZC |

Nota de notación: en `TCALL n`, `n1` indica que el opcode varía según el número de llamada (0-15), formando una familia de 16 opcodes (`01`, `11`, `21`, … `F1`).

**Pila**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| PUSH A | 2D | 1 | 4 | ........ |
| PUSH X | 4D | 1 | 4 | ........ |
| PUSH Y | 6D | 1 | 4 | ........ |
| PUSH PSW | 0D | 1 | 4 | ........ |
| POP A | AE | 1 | 4 | ........ |
| POP X | CE | 1 | 4 | ........ |
| POP Y | EE | 1 | 4 | ........ |
| POP PSW | 8E | 1 | 4 | NVPBHIZC |

**Operaciones de bit único en memoria**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| SET1 dp, bit | x2 | 2 | 4 | ........ |
| CLR1 dp, bit | y2 | 2 | 4 | ........ |
| TSET1 !abs | 0E | 3 | 6 | N.....Z. |
| TCLR1 !abs | 4E | 3 | 6 | N.....Z. |
| AND1 C, abs, bit | 4A | 3 | 4 | .......C |
| AND1 C, /abs, bit | 6A | 3 | 4 | .......C |
| OR1 C, abs, bit | 0A | 3 | 5 | .......C |
| OR1 C, /abs, bit | 2A | 3 | 5 | .......C |
| EOR1 C, abs, bit | 8A | 3 | 5 | .......C |
| NOT1 abs, bit | EA | 3 | 5 | ........ |
| MOV1 C, abs, bit | AA | 3 | 4 | .......C |
| MOV1 abs, bit, C | CA | 3 | 6 | ........ |

Nota de notación: en `SET1`/`CLR1`, `x2`/`y2` indican familias de 8 opcodes (uno por bit 0-7), igual que en `BBS`/`BBC`.

**Flags de estado**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| CLRC | 60 | 1 | 2 | .......0 |
| SETC | 80 | 1 | 2 | .......1 |
| NOTC | ED | 1 | 3 | .......C |
| CLRV | E0 | 1 | 2 | .0..0... |
| CLRP | 20 | 1 | 2 | ..0..... |
| SETP | 40 | 1 | 2 | ..1..... |
| EI | A0 | 1 | 3 | ......1. |
| DI | C0 | 1 | 3 | ......0. |

**No-operación y parada**

| Instrucción | Opcode | Bytes | Ciclos | Flags |
|---|---|---|---|---|
| NOP | 00 | 1 | 2 | ........ |
| SLEEP | EF | 1 | 3 | ........ |
| STOP | FF | 1 | 2 | ........ |

### Notas sobre instrucciones especiales

- **MUL YA**: multiplica Y por A (resultado de 16 bits en YA); los flags N/Z se calculan solo a partir del registro Y resultante.
- **DIV YA, X**: divide YA entre X; A recibe el cociente, Y el resto; el flag de overflow (V) almacena el bit 8 del cociente (se usa como indicador de desbordamiento de la división).
- **TSET1 / TCLR1**: para fijar los flags usan una comparación de igualdad (A - valor_anterior), lo cual difiere del comportamiento equivalente en el 65C02.
- **Saltos condicionales**: cuestan 2 ciclos si no se toman y 4 si se toman (o 5/7, 6/8 según la variante, ver tabla).

## Notas / gotchas

- El SPC-700 elimina varias facilidades del 6502 (modo decimal nativo vía `DAA`/`DAS` en vez de un flag D, `JMP` no indexado, `BIT`, ciertos modos indexados de lectura-modificación-escritura), así que el código 6502 no es directamente portable.
- Las familias de opcodes `TCALL`, `SET1`/`CLR1` y `BBS`/`BBC` codifican el número de vector/bit dentro del propio opcode; hay que generarlas con una tabla o macro en el ensamblador en vez de tratar cada una como instrucción independiente.
- Los ciclos "2/4", "5/7", etc. dependen de si el salto se toma o no — hay que tenerlo en cuenta al calcular presupuestos de ciclos para el driver de sonido.
- `RETI` y `POP PSW` son las únicas instrucciones que afectan a todos los flags de `PSW` a la vez (restauran el estado completo).

## Referencias cruzadas

- (pendiente de enlazar)
