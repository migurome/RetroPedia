---
titulo: Mandos de Mega Drive (3 y 6 botones)
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [mando, controller, joypad, io, 6-botones, sacbrldu]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/controllers]
---

# Mandos de Mega Drive (3 y 6 botones)

## Resumen

Esta entrada documenta cómo leer los mandos de Mega Drive, tanto el modelo clásico de 3 botones como el de 6 botones (que añade compatibilidad hacia atrás). Cubre la inicialización de los puertos, la secuencia de lectura mediante la línea TH, la detección de pulsaciones y la detección del tipo de mando conectado.

## Detalle técnico

### Direcciones de puertos I/O

| Puerto | Data port | Control port |
|--------|-----------|---------------|
| Player 1 | `$A10003` | `$A10009` |
| Player 2 | `$A10005` | `$A1000B` |

### Configuración inicial

Se requiere escribir `$40` tanto en el puerto de control como en el de datos para cada jugador. Es crítico detener el Z80 durante este proceso debido a un error de hardware.

### Lectura de mando de 3 botones

Proceso de dos pasos: escribir `$40`, esperar, leer; luego escribir `$00`, esperar, leer.

| Bit | Paso 1 ($40) | Paso 2 ($00) |
|-----|--------------|--------------|
| 5 | C | Start |
| 4 | B | A |
| 3 | Right | 0 |
| 2 | Left | 0 |
| 1 | Down | Down |
| 0 | Up | Up |

Esquema SACBRLDU recomendado: los bits 5-0 del paso 1 conservan su posición; los bits 5-4 del paso 2 se desplazan a las posiciones 7-6.

### Detección de pulsaciones

Para acciones únicas al presionar:
```
just_pressed = pressed_now AND (NOT pressed_before)
```

Para detectar liberación:
```
just_released = (NOT pressed_now) AND pressed_before
```

### Lectura de mando de 6 botones

Requiere siete escrituras alternadas (cuatro de ellas ignorables, mantenidas por compatibilidad):

| Paso | Acción | Botones detectados |
|------|--------|---------------------|
| 1-2 | Estándar | D-pad, A/B/C, Start |
| 3-5 | Ignorar lectura | Compatibilidad |
| 6 | Verificar bits 3-0 | Detectar tipo de mando |
| 7 | Si paso 6 = `0000` | X/Y/Z, Mode |

Paso 6 crítico: si los bits 3-0 son distintos de `0000`, es un mando de 3 botones. Si son `0000`, se procede al paso 7.

### Detección de controlador

Los bits 3-2 del segundo paso deben ser `00`. Valores diferentes indican ausencia de mando.

## Notas / gotchas

- No leer los mandos más de una vez por fotograma: causa fallo de compatibilidad en los mandos de 6 botones.
- No mantener Mode presionado al encender la consola (activa el modo de 3 botones).
- No presionar Mode+X+C simultáneamente en mandos inalámbricos.
- Es crítico "detener el Z80 durante este proceso [de configuración inicial] debido a un error de hardware".

## Referencias cruzadas

- (pendiente de enlazar)
