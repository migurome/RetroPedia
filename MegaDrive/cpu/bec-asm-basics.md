---
titulo: Big Evil Corporation — bases del ensamblador 68000 (serie Tanglewood)
plataforma: [MegaDrive]
categoria: [cpu]
tags: [bigevilcorporation, tanglewood, 68000, asm, registros, fundamentos]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/03/01/sega-megadrive-2-so-assembly-language-then/]
---

# Big Evil Corporation — bases del ensamblador 68000 (serie Tanglewood)

## Resumen

Segunda entrada de la serie de Matt (Big Evil Corporation, autor de Tanglewood) dirigida a quien no ha tocado nunca ensamblador: explica los registros del 68000, la sintaxis de `MOVE` y sus modos de direccionamiento, los opcodes mínimos con los que el autor afirma haber podido llegar a renderizar texto y sprites, y cierra con un ejemplo completo (contar la longitud de una cadena).

## Detalle técnico

### Naturaleza del ensamblador frente a C/C++

El autor insiste en que cada línea de ASM 68000 se corresponde 1:1 con una instrucción real de la CPU (opcode) y sus operandos — no hay compilación en el sentido de C/C++, solo un ensamblado directo, y no existen las abstracciones habituales de un lenguaje de alto nivel (funciones, clases, structs, templates). Todo lo que se puede lograr hay que construirlo manualmente a partir de estas piezas mínimas.

### Registros del 68000

16 registros en total:
- **`d0`-`d7`** — registros de datos de propósito general, 32 bits cada uno, para aritmética.
- **`a0`-`a7`** — registros de direcciones, actúan como punteros a memoria RAM. `a7` es el stack pointer (`sp`).

### Sintaxis de MOVE y modos de direccionamiento

```
move.l #$10, d0        ; Valor hexadecimal inmediato -> registro
move.l #%0101, d0      ; Valor binario inmediato -> registro
move.l #12, d0         ; Valor decimal inmediato -> registro
move.l d1, d0           ; Contenido de registro -> registro
move.l 0x8000, d0      ; Dato leído de una dirección de memoria -> registro
move.l d0, 0x8000      ; Contenido de registro -> dirección de memoria
move.l (a0), d0         ; Dato en la dirección apuntada por a0 -> registro
move.l d0, (a0)         ; Contenido de registro -> dirección apuntada por a0
```

Notación de valores inmediatos: hexadecimal con `#$12` o `#0x12`, binario con `#%0011`, decimal sin prefijo (`#128`). Los paréntesis `(a0)` indican desreferenciación (equivalente al `*` de C/C++); sin paréntesis se mueve la dirección en sí, no el dato apuntado.

Sufijos de tamaño de operación: `.b` (byte, 8 bits), `.w` (word, 2 bytes), `.l` (longword, 4 bytes). No todos los opcodes admiten los tres tamaños.

### Opcodes básicos cubiertos

**Aritmética (`ADD`/`SUB`/`MUL`/`DIV`):**
```
add.l #0x10, d1        ; Suma 0x10 a d1, resultado en d1
add.w d1, d2            ; Suma d1 a d2 (word), resultado en d2
addq.b #0x5, d3         ; Suma rápida de 5 a d3 (byte)
```
El autor menciona las variantes `ADDI` (suma inmediata), `ADDA` (suma a registro de dirección), `ADDQ` (suma rápida de valores pequeños 1-8) y `ADDX`; `MUL`, `SUB` y `DIV` siguen el mismo patrón de variantes.

**Limpiar un registro o dirección (`CLR`):**
```
clr.l d0                ; d0 = 0 (32 bits completos)
clr.w (a0)               ; Limpia la word (2 bytes) en la dirección de a0
clr.b d0                 ; Limpia solo el byte inferior de d0
```

**Salto incondicional (`JMP`):**
```
SomeLabel:
   jmp SomeLabel        ; Bucle infinito
```

**Llamada y retorno de subrutina (`JSR`/`RTS`):**
```
move.l #0x8 d0
jsr Label                ; Salta a Label, guardando la dirección de retorno
move.l #0x12 d0          ; Se ejecuta al volver de Label

Label:
   move.l #0x04, d0
   rts                    ; Retorna al punto tras el jsr
```

**Bucle con decremento (`DBRA`):**
```
move.b #0x6, d0          ; Contador inicial (7 iteraciones: 0-6)

Label:
   add.l #0x1, d1        ; Incrementa d1
   dbra d0, Label         ; Decrementa d0, repite mientras d0 != 0
   clr.l d1                ; Tras salir del bucle
```

**Comparación y salto condicional (`CMP`/`Bcc`):**
```
cmp.b #0x0, d1           ; Compara d1 con 0, actualiza flags de estado
beq.b @End                ; Salta a @End si el resultado fue cero
```

### Ejemplo completo: longitud de una cadena terminada en cero

```
GetStringLength:
   clr.l d0              ; Inicializa el contador

   @FindTerm:
   move.b (a0)+, d1      ; Lee byte en a0, luego incrementa a0 en 1
   cmp.b #0x0, d1        ; ¿Es el terminador (0)?
   beq.b @End             ; Si es cero, termina
   addq.l #0x1, d0        ; Incrementa contador
   jmp @FindTerm           ; Repite

   @End:
   rts                     ; Retorna con la longitud en d0

; Uso:
   move.l #StringAddr, a0
   jsr GetStringLength
   ; Resultado en d0

StringAddr:
   dc.b "HELLO WORLD", 0
```

Conceptos usados en el ejemplo: `(a0)+` es post-incremento (lee primero, luego suma 1 a `a0` — el tamaño del incremento depende del tamaño del operando); `@FindTerm` y `@End` son etiquetas locales, reutilizables dentro del mismo contexto sin colisionar con etiquetas globales de otras rutinas.

### Referencias documentales citadas por el autor

- Motorola 68000 Instruction Set (PDF oficial).
- Página de ayuda de ASM68k.
- Manual de referencia de ASM68k.

## Notas / gotchas

- El autor remarca que con un puñado de opcodes (unos 10) fue suficiente para llegar a renderizar texto y sprites en pantalla — el objetivo del artículo es desmitificar el ensamblador para quien viene de lenguajes de alto nivel, no dar una referencia exhaustiva del set de instrucciones.
- Señala explícitamente que hay variantes más eficientes de las mismas operaciones (p. ej. `ADDQ` frente a `ADDI`) que se pueden adoptar más adelante como optimización, una vez el código básico funciona.
- Se mencionan operadores de rotación de bits (`ROL`/`ROR`) como útiles para trabajar con direcciones del VDP, pero sin desarrollarlos en este artículo (se retoman en artículos posteriores de la serie).

## Referencias cruzadas

- [Motorola 68000 — instrucciones y modos de direccionamiento (base)](68000-instrucciones-modos-direccionamiento.md) — cubre el mismo terreno con más profundidad y en formato de referencia (14 modos de direccionamiento, todas las categorías de instrucciones); esta entrada es el enfoque didáctico/introductorio de Big Evil Corporation para quien parte de cero.
