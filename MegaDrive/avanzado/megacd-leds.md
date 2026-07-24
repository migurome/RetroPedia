---
titulo: Control de LEDs en Mega CD
plataforma: [MegaDrive]
categoria: [avanzado]
tags: [mega-cd, leds, sub-cpu, bios, cdsubctrl]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/megacd-leds]
---

# Control de LEDs en Mega CD

## Resumen

Esta entrada trata sobre la **Mega CD**: explica cómo controlar manualmente los LEDs verde (Ready) y rojo (Access) de la unidad desde el Sub-CPU, y qué combinaciones de estado usa convencionalmente el sistema.

## Detalle técnico

**Registro de control de LEDs:** los LEDs se controlan mediante el byte alto de `CdSubCtrl` en la dirección `$FF8000`, accesible solo desde el Sub-CPU usando escrituras de tamaño byte.

**Asignación de bits:**

- Bit 1: LED verde (Ready)
- Bit 0: LED rojo (Access)

**Constantes definidas:**

```
LED_NONE:   %00
LED_GREEN:  %10
LED_RED:    %01
LED_BOTH:   %11
```

**Ejemplo de implementación:** para activar solo el LED verde: `move.b #LED_GREEN, (CdSubCtrl)`

### Significados habituales de los estados de LED

La documentación proporciona una tabla que asocia combinaciones de LED con estados del sistema:

- Ambos apagados = no en ejecución
- Verde encendido, rojo apagado = listo (Ready)
- Verde encendido, rojo parpadeando = sin disco
- Ambos encendidos = acceso a CD
- Verde parpadeando, rojo apagado = standby
- Ambos parpadeando = condición de error

### Rutina de ejemplo del manejador de errores

La página incluye una rutina en ensamblador para el Sub-CPU que hace parpadear ambos LEDs simultáneamente durante un cuelgue (crash), usando instrucciones `dbf` para consumir ciclos de temporización en lugar de temporizadores.

## Notas / gotchas

- "Si estás usando la BIOS, entonces la BIOS se encargará de los LEDs por ti." Es posible anular este comportamiento, pero requiere consultar la documentación de la BIOS para la llamada `LEDSET`.
- El Mega CD modelo 2 carece del LED Ready.
- Los flashcarts y emuladores pueden no mostrar los LEDs de forma fiable, así que no deberían usarse para indicar información crítica.

## Referencias cruzadas

- (pendiente de enlazar)
