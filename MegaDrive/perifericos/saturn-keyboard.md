---
titulo: Teclado de Saturn en Mega Drive
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [teclado, saturn-keyboard, scancode, io, protocolo]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/saturn-keyboard]
---

# Teclado de Saturn en Mega Drive

## Resumen

El teclado de Saturn puede usarse en la Mega Drive y funciona mediante notificaciones de eventos (no consultas de estado): cada pulsación se comunica como un scancode único, no como un código ASCII directo. Esta entrada documenta su identificación, protocolo de lectura y la conversión de scancodes a caracteres.

## Detalle técnico

### Conceptos fundamentales

"Cuando un usuario presiona una tecla, el teclado comunica este evento mediante un código único llamado scancode, no mediante códigos ASCII directos."

### Configuración inicial

Se requiere escribir el valor `$60` en los puertos de control y datos:

```
FastPauseZ80
move.b  #$60, (IoCtrl2)
move.b  #$60, (IoData2)
ResumeZ80
```

El identificador de periférico es 5 (`%0101`).

### Protocolo de lectura

Procedimiento de 6 pasos, iterativo:

1. Escribir `$00` al puerto de datos.
2. Esperar hasta que el bit 4 sea 0.
3. Leer los bits 3-0 (nibble).
4. Escribir `$20` al puerto.
5. Esperar hasta que el bit 4 sea 1.
6. Leer los bits 3-0 (nibble).

Repetir 12 veces, luego escribir `$60` para finalizar.

### Estructura del paquete (12 nibbles)

| Posición | Contenido |
|----------|-----------|
| 1-2 | Validación: `%0011 %0100` |
| 3-6 | Compatibilidad (ignorar) |
| 7 | Estado de luces (CapsLock, NumLock, ScrollLock) |
| 8 | Bit Make/Break (1=presión/liberación) |
| 9-10 | Scancode de 8 bits |
| 11-12 | Relleno (ignorar) |

### Tabla de scancodes (parcial)

```
$00: F9        $01: F5        $02: F3        $03: F1
$04: F2        $05: F12       $06: F10       $07: F8
$10: Left Alt  $11: L Shift   $12: L Ctrl    $13: Q
$1C: Z         $1D: S         $1E: A         $1F: W
$39: Space     $45: NumLock   $46: ScrollLk  $52: Insert
$53: Delete    $54: Left      $55: Home      $56: End
$57: Up        $58: Down      $59: PageUp    $5A: PageDn
$5B: Right
```

### Conversión a ASCII

"Las tablas de búsqueda mapean scancodes a códigos ASCII, con conjuntos separados para teclas con y sin Shift presionado."

Consideraciones especiales:
- Manejar Caps Lock comparando si el código resultante es una letra y alternando mayúsculas/minúsculas.
- No modificar las teclas Shift para evitar desplazamiento de números/símbolos.
- Las teclas de edición (retroceso, enter, flechas) requieren manejo separado.

### Verificación de modificadores

Se debe consultar manualmente el estado de las teclas Ctrl, Alt y Shift almacenadas en RAM, ya que "el teclado no gestiona automáticamente estas modificaciones."

## Notas / gotchas

- El teclado notifica eventos (make/break), no un estado consultable directamente; hay que mantener el estado de modificadores manualmente en RAM.
- La tabla de scancodes documentada en la fuente es parcial, no exhaustiva.

## Referencias cruzadas

- (pendiente de enlazar)
