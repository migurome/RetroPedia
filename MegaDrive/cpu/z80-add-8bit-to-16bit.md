---
titulo: Sumar y restar un valor de 8 bits a uno de 16 bits en Z80
plataforma: [MegaDrive]
categoria: [cpu]
tags: [z80, ensamblador, aritmetica, signo, 8-bit, 16-bit]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/z80-add-8bit-to-16bit]
---

# Sumar y restar un valor de 8 bits a uno de 16 bits en Z80

## Resumen

Esta guía aborda cómo combinar un valor de 8 bits (registro A) con un valor de 16 bits (HL) sin necesidad de expandirlo primero a 16 bits completos, lo que preserva la disponibilidad de registros. Cubre suma y resta, tanto con signo (signed) como sin signo (unsigned), además de las variantes equivalentes usando operaciones de 16 bits directamente.

## Detalle técnico

### Suma unsigned (sin signo)

```asm
add   a, l    ; A = A+L
ld    l, a    ; L = A+L
adc   a, h    ; A = A+L+H+carry
sub   l       ; A = H+carry
ld    h, a    ; H = H+carry
```

Sumar A a HL es complicado. Primero se suma el byte bajo. Luego se suma el byte alto con acarreo y se resta el byte bajo para obtener el resultado del byte alto.

### Suma signed (con signo)

```asm
or    a
jp    p, Positive
dec   h
Positive:

add   a, l
ld    l, a
adc   a, h
sub   l
ld    h, a
```

Se extiende A a 16 bits comprobando el signo. El byte alto de A es 0 si es positivo, o -1 si es negativo. Luego se procede como en la suma unsigned.

### Resta unsigned (sin signo)

```asm
neg
jp    z, Skip
dec   h

add   a, l
ld    l, a
adc   a, h
sub   l
ld    h, a
Skip:
```

HL - A equivale a HL + (-A). Se invierte el signo de A y se resta 1 al byte alto para compensar que el byte superior se convierte en -1. Si A=0, se omite la operación.

### Resta signed (con signo)

```asm
neg

jp    p, Positive
jp    pe, Positive
dec   h
Positive:

add   a, l
ld    l, a
adc   a, h
sub   l
ld    h, a
```

Se construye sobre la suma signed: se invierte A y luego se suma. Se maneja el caso especial de -$80 comprobando el flag de overflow (paridad/overflow, `pe`).

### Con operaciones de 16 bits

**Suma unsigned:**
```asm
ld    e, a
ld    d, 0
add   hl, de
```

**Suma signed:**
```asm
ld    e, a
add   a, a
sbc   a
ld    d, a
add   hl, de
```

**Resta unsigned:**
```asm
ld    e, a
ld    d, 0
or    a
sbc   hl, de
```

**Resta signed:**
```asm
ld    e, a
add   a, a
sbc   a
ld    d, a
or    a
sbc   hl, de
```

## Notas / gotchas

- "Only A gets clobbered" en todos los métodos de 8 bits: sólo el registro A se ve modificado como efecto secundario.
- "Destination must be HL": las variantes de 16 bits requieren que HL sea siempre el destino.
- "You must clobber BC or DE": las alternativas de 16 bits exigen sacrificar BC o DE como registro temporal.
- "For whatever reason there's no 16-bit `sub`, only `sbc`": no existe una instrucción `sub` de 16 bits en el Z80, solo `sbc`, por lo que hay que limpiar el acarreo (carry) antes de las operaciones de resta (de ahí el `or a` previo a `sbc hl, de`).

## Referencias cruzadas

- (pendiente de enlazar)
