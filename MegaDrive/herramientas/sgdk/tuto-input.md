---
titulo: SGDK — Tutorial de Input (mandos)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, tutorial, input, joypad, joy, multitap]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/Tuto-Input]
---

# SGDK — Tutorial de Input (mandos)

## Resumen

API de manejo de mandos de SGDK: inicialización, manejo por callback de eventos de botones, soporte multi-mando (EA 4-Way, Sega TeamPlayer) y detección del tipo de periférico conectado (incluye ratón).

## Detalle técnico

### Inicialización

Hace falta llamar a `JOY_init()` para resetear el manejador y detectar los mandos conectados, y luego a `JOY_setEventHandler()` para registrar una función callback.

### Firma del callback

Los eventos de input disparan un callback con esta firma exacta:
```c
void myJoyHandler(u16 joy, u16 changed, u16 state)
```

Parámetros:
- **joy** — identificador del mando (`JOY_1`, `JOY_2`, etc.)
- **state** — estado actual de botones/direcciones
- **changed** — flags que indican qué inputs acaban de cambiar

### Lógica de detección de estado de botón

El callback distingue entre input mantenido pulsado y recién soltado:
- `if (state & BUTTON_START)` — el botón está pulsado ahora mismo
- `if (changed & BUTTON_START)` — el botón acaba de cambiar (soltarse/pulsarse)

### Soporte multi-mando

SGDK reconoce hasta 4 jugadores vía adaptador EA 4-Way y hasta 8 combinando dos unidades Sega TeamPlayer. Identificadores de puerto:
- Puerto primario: `JOY_1`, `JOY_1A`, `JOY_1B`, `JOY_1C`, `JOY_1D`
- Puerto secundario: `JOY_2`, `JOY_2A`, `JOY_2B`, `JOY_2C`, `JOY_2D`

Detección mediante `JOY_isMultipad(port)` o `JOY_getJoyID(joy)`, que devuelve flags de tipo de adaptador como `JOY_SUPPORT_EA4WAY`.

### Detección del tipo de dispositivo

`JOY_getJoyID()` devuelve flags de capacidad:
- `JOY_SUPPORT_3BTN` / `JOY_SUPPORT_6BTN`
- `JOY_SUPPORT_TEAMPLAY` / `JOY_SUPPORT_EA4WAY`
- `JOY_SUPPORT_MOUSE` / `JOY_SUPPORT_JUSTIFIER`

### Input de ratón

Se registra un callback separado con `JOY_setMouseEventHandler()`, con esta firma:
```c
void myMouseHandler(u16 joy, s8 xDelta, s8 yDelta, u16 buttonState)
```

### Funciones de utilidad

- `JOY_update()` — refresca el estado de los mandos (se llama automáticamente cada VBlank)
- `JOY_readJoypad(joy)` — sondea un mando concreto bajo demanda
- `JOY_waitPressBtn()` — bloquea hasta que se pulse cualquier botón (no direccional)
- `JOY_waitPress(joy, BUTTON_A | BUTTON_UP)` — bloquea hasta una combinación de botones concreta

## Notas / gotchas

- Esta API envuelve el protocolo crudo de los puertos I/O y periféricos documentado en `plutiedev.com` — ver [`../../perifericos/controllers.md`](../../perifericos/controllers.md), [`../../perifericos/mouse.md`](../../perifericos/mouse.md), [`../../perifericos/sega-multitap.md`](../../perifericos/sega-multitap.md) y [`../../perifericos/ea-multitap.md`](../../perifericos/ea-multitap.md) para el detalle a nivel de hardware/protocolo que SGDK abstrae.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Sprites](tuto-sprites.md)
- [Tutorial: Matemáticas](tuto-maths.md)
