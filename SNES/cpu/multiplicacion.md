---
titulo: Multiplicación en SNES (hardware 5A22 y Modo 7 de la PPU)
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, multiplicacion, hardware, registros, mode7]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Multiplication]
---

# Multiplicación en SNES (hardware 5A22 y Modo 7 de la PPU)

## Resumen
El 65816 no tiene instrucción de multiplicación nativa, pero la SNES ofrece dos multiplicadores por hardware: uno integrado en el chip 5A22 (8×8→16 bits, sin signo) y otro dentro de la PPU asociado al Modo 7 (16×8→24 bits, con signo). También es posible multiplicar por constantes conocidas usando shifts y sumas, que puede ser más rápido que esperar al hardware.

## Detalle técnico

### Multiplicador del 5A22
Realiza una multiplicación de 8 bits × 8 bits sin signo, con resultado de 16 bits. Ejemplo: $ff (255) × $ff (255) = $fe01 (65025). Tras iniciar la operación se necesitan 8 ciclos de CPU antes de que el resultado sea válido. Comparte el registro de resultado con la división (ver entrada de división).

**Registros:**

| Registro | Dirección | Propósito |
|----------|-----------|-----------|
| WRMPYA | $4202 | Primer operando (8 bits sin signo) |
| WRMPYB | $4203 | Segundo operando; escribir aquí inicia la multiplicación |
| RDMPYL | $4216 | Byte bajo del resultado |
| RDMPYH | $4217 | Byte alto del resultado |

El tiempo que tarda la propia instrucción de lectura del resultado también cuenta como parte de la espera necesaria. Por ejemplo, un `LDA absolute` consume 3 ciclos antes de leer, por lo que solo hacen falta 5 ciclos adicionales de espera para completar los 8 totales.

Ejemplo de multiplicación en cadena de 8 números distintos:
```
; Multiplicar 8 números diferentes
lda 0,y
.repeat 8, I
  sta WRMPYB   ; Inicia multiplicador
  lda I+1,y    ; 5 ciclos
  ldx RDMPYL   ; 3 ciclos antes de lectura
  stx I*2      ; =8 ciclos totales
.endrep
```

Precauciones: no escribir en WRMPYB dos veces dentro de la ventana de 8 ciclos, y no escribir mientras hay una división en curso (comparten circuitería).

### Multiplicador de la PPU (Modo 7)
Realiza una multiplicación de 16 bits × 8 bits con signo, con resultado de 24 bits, y sin latencia (resultado disponible de inmediato). Ejemplo: $ffff (-1) × $ff (-1) = $000001 (1). No puede usarse mientras el Modo 7 está renderizando en pantalla.

**Registros:**

| Registro | Dirección | Propósito |
|----------|-----------|-----------|
| M7A | $211B | Factor de 16 bits con signo (requiere dos escrituras: byte bajo y luego alto) |
| M7B | $211C | Factor de 8 bits con signo |
| MPYL | $2134 | Byte bajo del resultado |
| MPYM | $2135 | Byte medio del resultado |
| MPYH | $2136 | Byte alto del resultado |

M7A requiere escribirse en dos pasos (primero el byte bajo, luego el alto). Para escribir un valor de 8 bits con la parte alta a cero puede usarse `STA M7A \ STZ M7A`.

Ejemplo:
```
.a8
lda s16_l
sta M7A
lda s16_h
sta M7A

lda s8
sta M7B
; MPY contiene s16 * s8
```

Advertencia importante: M7A comparte el latch interno con M7HOFS/M7VOFS. Si una interrupción o una transferencia HDMA escribe en los registros de scroll de BG1 entre las dos escrituras a M7A, el resultado queda corrompido.

### Multiplicación por constante
Cuando el multiplicador es una constante conocida en tiempo de compilación, usar tablas o una secuencia de shifts/sumas suele ser más rápido que esperar al hardware. Ejemplo de multiplicación por 3:
```
sta Temp
asl
adc Temp  ; Asumir que el carry ya está despejado por el ASL previo
```

### Comparativa

| Aspecto | Multiplicador 5A22 | Multiplicador PPU (Modo 7) |
|---------|---------------------|------------------------------|
| Operandos | 8×8 | 16×8 |
| Signo | Sin signo | Con signo |
| Latencia | 8 ciclos | 0 ciclos |
| Restricciones | Comparte hardware con división | No usable durante render de Modo 7 |

## Notas / gotchas
- No reescribir WRMPYB antes de que pasen los 8 ciclos de latencia: corrompe el resultado en curso.
- El multiplicador del 5A22 y el divisor comparten el mismo bloque de registros/resultado ($4216/$4217): no lanzar una multiplicación mientras hay una división pendiente, y viceversa.
- El multiplicador PPU es exclusivo del hardware de Modo 7 y queda inutilizable mientras la pantalla está renderizando ese modo.
- El latch compartido de M7A con los offsets de scroll de BG1 (M7HOFS/M7VOFS) puede corromperse si una NMI/HDMA se cuela entre las dos escrituras de 8 bits a M7A; conviene deshabilitar interrupciones o ser cuidadoso con el timing.
- Para multiplicadores constantes pequeños, el enfoque por software (shift-and-add) suele ganarle en velocidad al hardware dedicado, que tiene 8 ciclos de latencia fija.

## Referencias cruzadas
- (pendiente de enlazar) — ver también la entrada de división (comparten registros de resultado).
