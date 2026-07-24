---
titulo: Sega Mouse (ratón) en Mega Drive
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [raton, mouse, io, protocolo, peripheral-id, cursor]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/mouse]
---

# Sega Mouse (ratón) en Mega Drive

## Resumen

El Sega Mouse (comercializado como Mega Mouse en Estados Unidos) es un periférico apuntador que se comunica mediante un protocolo de paquetes de 9 nibbles a través del puerto I/O. Esta entrada documenta su identificación, inicialización, protocolo de lectura, formato de datos y las variantes de hardware existentes.

## Detalle técnico

### Identificación y configuración

El ratón tiene un ID de periférico de 3 (`%0011`). La inicialización requiere escribir `$60` en los puertos de control y datos (diferente de los controladores, que usan `$40`):

```
FastPauseZ80
move.b  #$60, (IoCtrl2)
move.b  #$60, (IoData2)
ResumeZ80
```

### Protocolo de lectura

Verificación inicial: leer del puerto de datos y confirmar que los bits 3-0 sean `0000`. De lo contrario, no hay ratón presente.

La lectura consiste en un paquete de 9 nibbles. El protocolo alternado es:

```
1. Escribir $20, esperar bit 4=1, leer bits 3-0
2. Escribir $00, esperar bit 4=0, leer bits 3-0
3-4. Repetir pasos 1-2
5-6. Repetir pasos 1-2
7-8. Repetir pasos 1-2
9. Escribir $20, esperar bit 4=1, leer bits 3-0
10. Escribir $60 para finalizar
```

### Formato del paquete (tabla de nibbles)

| Nibble | Bits 3-0 | Descripción |
|--------|----------|-------------|
| 1º ($20) | `1011` | Identificación (debe ser exacto) |
| 2º ($00) | `1111` | Ignorado |
| 3º ($20) | `1111` | Ignorado |
| 4º ($00) | `Yo Xo Ys Xs` | Flags de overflow y signo |
| 5º ($20) | `C M R L` | Botones (1=presionado) |
| 6º ($00) | `X7 X6 X5 X4` | Movimiento X (bits altos) |
| 7º ($20) | `X3 X2 X1 X0` | Movimiento X (bits bajos) |
| 8º ($00) | `Y7 Y6 Y5 Y4` | Movimiento Y (bits altos) |
| 9º ($20) | `Y3 Y2 Y1 Y0` | Movimiento Y (bits bajos) |

Interpretación de bits:
- `Xo/Yo`: overflow — si está activo, descartar todo el paquete.
- `Xs/Ys`: signo (requiere extensión de signo).
- `L/R/M/C`: estados de los botones izquierdo, derecho, central y Start.

### Extensión de signo

```
and.w   #$FF, d0      ; Asegurar tamaño palabra
and.w   #$FF, d1

btst    #0, d2        ; Verificar Xs (bit 0)
beq.s   @PositiveX
neg.b   d0
neg.w   d0
@PositiveX:

btst    #1, d2        ; Verificar Ys (bit 1)
beq.s   @PositiveY
neg.b   d1
neg.w   d1
@PositiveY:
```

Nota: evitar la extensión de cero para prevenir la conversión a -256.

### Movimiento del cursor

```
cursor_x = cursor_x + mouse_x
cursor_y = cursor_y + mouse_y

if cursor_x < min_x then cursor_x = min_x
if cursor_x > max_x then cursor_x = max_x
if cursor_y < min_y then cursor_y = min_y
if cursor_y > max_y then cursor_y = max_y
```

### Variantes de hardware

- Mega Mouse (US): 4 botones (L, R, M, Start).
- Sega Mouse (JP/EU): 2 botones; modo trackball disponible (presionar la bola equivale a clic izquierdo).

## Notas / gotchas

- Es necesario implementar un timeout para evitar bloqueos si el ratón se desconecta durante la lectura.
- Si el flag de overflow (`Xo`/`Yo`) está activo, se debe descartar todo el paquete.
- Evitar la extensión de cero al procesar el signo, ya que provocaría una conversión errónea a -256.

## Referencias cruzadas

- (pendiente de enlazar)
