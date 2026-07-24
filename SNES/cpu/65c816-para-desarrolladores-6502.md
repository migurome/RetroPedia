---
titulo: El 65C816 para desarrolladores que ya conocen 6502
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, 65816, 6502, migracion, registros, modos-direccionamiento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/65c816_for_6502_developers]
---

# El 65C816 para desarrolladores que ya conocen 6502

## Resumen
El 65C816 es una extensión del 6502 que añade registros y acumulador conmutables entre 8 y 16 bits, direccionamiento de 24 bits (16 MB) mediante bancos, un stack de 16 bits reubicable, una "direct page" (zeropage) reubicable, y varias instrucciones nuevas. Esta entrada resume las diferencias clave para quien ya domina el 6502.

## Detalle técnico

### Flags M y X (tamaño de registros)
Dos flags nuevos en el registro de estado controlan el tamaño de los registros:

- **Flag M (bit #$20)**: controla si el acumulador opera en 8 o 16 bits. También afecta a instrucciones como `BIT`, `INC` o `ASL` cuando operan directamente sobre memoria.
- **Flag X (bit #$10)**: controla si X e Y son de 8 bits (flag set) o 16 bits (flag clear).

Se modifican con `REP` (resetea bits, pone a 0) y `SEP` (setea bits, pone a 1), ambas con operando inmediato de 8 bits.

**Tabla de bits del registro de estado (para REP/SEP):**

| Valor | Flag | Significado |
|---|---|---|
| #$01 | C | Carry |
| #$02 | Z | Zero |
| #$04 | I | Interrupt disable |
| #$08 | D | Decimal mode |
| #$10 | X | Tamaño de registros índice (8/16 bits) |
| #$20 | M | Tamaño del acumulador (8/16 bits) |
| #$40 | V | Overflow |
| #$80 | N | Negative |

Ejemplo: `SEP #$30` pone a 8 bits el acumulador y los índices simultáneamente.

### Acumulador de 16 bits
El acumulador siempre ocupa 2 bytes internamente. En modo 8 bits solo se usa el byte bajo; `XBA` intercambia los dos bytes entre sí. En modo 16 bits se usan ambos bytes, y un acceso a memoria de 16 bits lee la dirección indicada (byte bajo) más la siguiente (byte alto).

### Registros índice de 16 bits
Cuando X e Y están en modo 8 bits, su byte alto queda forzado a cero. Esto genera comportamientos especiales en las transferencias entre registros de distinto tamaño:

| Instrucción | Tamaño acumulador | Tamaño índices | Resultado |
|---|---|---|---|
| TAX | 8 bits | 16 bits | Ambos bytes del acumulador pasan a X |
| TAX | 16 bits | 8 bits | Byte bajo del acumulador pasa a X (byte alto = 0) |
| TXA | 8 bits | 16 bits | X pasa a A (byte alto de A queda en 0) |
| TXA | 16 bits | 8 bits | X pasa al byte bajo de A (byte alto de A no cambia) |

Implicación importante: si se cambia X/Y a 8 bits temporalmente y luego se vuelve a 16 bits, hay que preservar aparte el valor del otro registro índice si se quiere conservarlo.

Los registros índice de 16 bits pueden usarse como punteros de dirección completa, lo que permite acceder a campos de estructuras con offsets desde X usando instrucciones más compactas; el modo `(zeropage,x)` gana utilidad para acceder a punteros dentro de estructuras. A diferencia del 6502 (donde los arrays paralelos solían ser más eficientes), en 65816 puede convenir más usar arrays de estructuras (campos consecutivos) para aprovechar los accesos de 16 bits; el coste es que iterar requiere algo como `TXA \ CLC \ ADC #Size \ TAX`.

### Instrucciones nuevas de acumulador/índices
- `INA` — incrementar acumulador
- `DEA` — decrementar acumulador
- `PHX` / `PLX` — push/pull de X
- `PHY` / `PLY` — push/pull de Y
- `TXY` — transferir X a Y
- `TYX` — transferir Y a X
- `STZ` — almacenar cero (indexable con X, en zeropage o absoluto)
- `BRA` — branch incondicional (3 ciclos, igual que JMP pero un byte menos)

### Direccionamiento indirecto y pruebas de bits
El direccionamiento indirecto ya no requiere indexación obligatoria: por ejemplo `LDA ($00)` es válido.

`BIT` ahora admite indexación y operando inmediato; con inmediato actúa como un `AND` que solo afecta flags, sin modificar el acumulador.

`TRB` (Test and ReseT Bits) y `TSB` (Test and SeT Bits) operan sobre zeropage o absoluto: ponen el flag zero según el AND entre acumulador y memoria; `TRB` limpia en memoria los bits marcados en el acumulador, `TSB` los pone a 1.

Ejemplo — ensamblar un valor combinando dos partes:
```
lda XPos
sta Temp
lda YPos
asl
asl
asl
asl
tsb Temp
```

### Tablas de salto
Nuevas instrucciones `JMP (absolute,x)` y `JSR (absolute,x)` permiten tablas de salto sin manipular la pila:
```
asl
tax
jmp (Table,x)

Table:
.addr Routine1
.addr Routine2
.addr Routine3
```

### Espacio de direcciones de 24 bits y bancos
El espacio direccionable total es de 24 bits (16 MB), por lo que normalmente no hace falta bank switching manual como en 6502.

`JMP`, `JSR` y `RTS` siguen operando con direcciones de 16 bits y no tocan el byte de banco. Instrucciones nuevas de 24 bits: `JML` (jump long), `JSL` (jump to subroutine long), `RTL` (return long); `RTI` también admite direcciones de 24 bits. Las rutinas llamadas con `JSL` deben retornar con `RTL`; las llamadas con `JSR` deben usar `RTS`.

Los accesos a datos con direcciones de 16 bits (no zeropage) se extienden a 24 bits usando el registro de "data bank" (DBR). Solo se manipula mediante `PHB`/`PLB` (push/pull data bank) y `PHK` (push program bank, útil para igualar data bank a program bank). Patrón típico:
```
php ; Guardar tamaños de registros
phb ; Guardar data bank original
phk ; Pushear program bank
plb ; Almacenarlo en data bank
; ... código que modifica el data bank ...
plb ; Restaurar data bank original
plp ; Restaurar tamaños de registros
rtl
```

**Nuevos modos de direccionamiento de 24 bits** (para `LDA`, `STA`, `ADC`, `SBC`, `ORA`, `AND`, `EOR`, `CMP`):
- `f:absolute,x` — dirección de 24 bits indexada por X
- `[zeropage]` — versión de 24 bits de `(zeropage)`
- `[zeropage],y` — versión de 24 bits de `(zeropage),y` (no existe el equivalente `[zeropage,x]`)

En ca65 se usan operadores especiales para descomponer direcciones: `<` (byte bajo), `>` (byte siguiente/alto), `^` (byte de banco).

### Stack de 16 bits
El stack pointer es de 16 bits y puede apuntar a cualquier posición dentro de los primeros 64 KB (típicamente $0000-$1FFF en la SNES).

**Nuevos modos de direccionamiento de stack** (mismo conjunto de instrucciones que arriba):
- `stack,s` — indexa una dirección de 8 bits con el stack pointer
- `(stack,s),y` — modo indirecto con stack pointer, indexado por Y

El stack pointer apunta siempre al siguiente slot libre, así que `1,s` accede al byte pusheado más recientemente, `2,s` al siguiente, etc. Instrucciones relacionadas: `TSC` (transfer stack pointer to accumulator) y `TCS` (transfer accumulator to stack pointer). Limitación: solo estas instrucciones soportan el modo stack; `LDX` o `INC`, por ejemplo, no lo soportan.

### Direct page reubicable
El 65816 permite mover la "zeropage" (ahora llamada "direct page") a cualquier posición dentro de los primeros 64 KB. Instrucciones: `TDC` (transfer direct page to accumulator) y `TCD` (transfer accumulator to direct page). No es obligatorio alinear la direct page a un límite de página, pero es recomendable para evitar un ciclo extra en instrucciones de direct page. Ubicaciones habituales: $2100 (registros de PPU), $4300 (registros de DMA).

### Modo decimal
A diferencia del 6502 en sistemas como la NES, el modo decimal del 65816 sí funciona. Solo afecta a `ADC` y `SBC`; los incrementos/decrementos deben resolverse aparte con estas instrucciones si se necesita aritmética decimal.

### Modo de emulación
El 65816 arranca siempre en "modo de emulación", que simula un 65C02:
- Stack pointer de 8 bits, fijo en la página $01xx.
- Registros forzados a 8 bits.
- Las interrupciones usan direcciones de 16 bits (tabla de vectores distinta).
- Algunas instrucciones consumen más ciclos que en modo nativo.

En desarrollo SNES normalmente se sale de este modo casi de inmediato con: `CLC \ XCE`.

### Preservar el tamaño de registros
Patrón típico para preservar los tamaños M/X al salir de una rutina:
```
php
; código aquí
plp
rts
```
Si además se preservan valores (el orden es crítico):
```
pha
phx
phy
php
; código aquí
plp
ply
plx
pla
rts
```
Invertir este orden provocaría un cuelgue, porque `RTS` acabaría leyendo una dirección de retorno incorrecta.

### Matemática específica de la SNES
La consola aporta registros de E/S de multiplicación y división: multiplicación sin signo 8×8 (resultado 16 bits), división sin signo 16÷8 (cociente 16 bits y residuo 16 bits), y multiplicación con signo 16×8 (resultado 24 bits, esta última vía Modo 7 de la PPU). Todas requieren esperar cierto número de ciclos (rellenables con `nop` u otro trabajo útil) antes de que el resultado sea válido.

### Directivas de ca65 relacionadas
- `.a8` / `.a16` — tamaño del acumulador
- `.i8` / `.i16` — tamaño de los registros índice
- `.smart` — hace que ca65 infiera automáticamente los cambios de tamaño provocados por REP/SEP

Algunos ensambladores, como asar, requieren en cambio especificar el tamaño por instrucción: `LDA.B #1` o `LDA.W #1`.

## Notas / gotchas
- Olvidar que tras un reset la CPU siempre arranca en modo emulación (8 bits, stack fijo en $01xx), no en modo nativo, es un error común al portar código.
- Un desajuste entre el estado real de los flags M/X y lo que el ensamblador cree (directivas `.a8`/`.a16`/`.i8`/`.i16`) desalinea el ensamblado de operandos inmediatos sin generar ningún error visible: el código simplemente se ejecuta mal.
- Cambiar X/Y de 16 a 8 bits y volver a 16 bits puede perder el byte alto del registro que no se tocó explícitamente; hay que preservarlo aparte si importa su valor.
- El orden de push/pull al guardar registros y flags junto con `php`/`plp` es crítico: invertirlo rompe el `RTS`/`RTL` final.
- `JSL` debe emparejarse con `RTL`, nunca con `RTS`, y viceversa con `JSR`/`RTS`.
- Los modos `[zeropage]` y `[zeropage],y` de 24 bits no tienen contraparte indexada por X (`[zeropage,x]` no existe).

## Referencias cruzadas
- (pendiente de enlazar) — relacionado con las entradas de vectores de CPU, multiplicación/división y tamaños de registro en ca65 de esta misma base.
