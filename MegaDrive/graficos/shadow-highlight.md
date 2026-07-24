---
titulo: Modo Shadow/Highlight
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, shadow-highlight, sombra, brillo, prioridad, paletas]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/shadow-highlight]
---

# Modo Shadow/Highlight

## Resumen

Describe el modo shadow/highlight (sombra/resalte) del VDP, que oscurece o aclara píxeles individuales según reglas basadas en colores especiales de sprite y en el bit de prioridad de capas, permitiendo efectos de iluminación sin gastar colores adicionales de paleta.

## Detalle técnico

### Activación del modo

El modo S/H se habilita mediante el registro que controla la resolución de pantalla. El código de ejemplo proporciona:

```
SetGfxMode GFXMODE_320x224_SH
```

Según el documento: "Shadow/highlight is enabled by the same register that controls the screen resolution" (el modo shadow/highlight se habilita mediante el mismo registro que controla la resolución de pantalla) y se debe consultar la documentación de resolución de pantalla para su configuración específica.

### Funcionamiento técnico

El mecanismo opera **por píxel** considerando dos factores principales:

#### Factor 1: colores especiales de sprite

- **Paleta 3, Color 14**: genera brillo; el sprite se torna transparente.
- **Paleta 3, Color 15**: genera oscuridad; el sprite se torna transparente.
- Otros colores: sin cambio.

#### Factor 2: bit de prioridad de capas

Después de combinar sprites, plano A y plano B, se evalúa:

- **Si color 14 de sprite (cualquier paleta):** sin cambio.
- **Si cualquier capa tiene prioridad alta:** sin cambio.
- **De lo contrario:** el píxel se oscurece.

### Casos especiales críticos

El autor ("Iwis") destaca:

"If both rules above end up being 'pixel is darker'...then the pixel is still just 50% darker, not made even darker." (Si ambas reglas anteriores terminan siendo "el píxel es más oscuro", el píxel sigue siendo solo un 50% más oscuro, no se oscurece más todavía).

Además: "brighter + darker gives a normal pixel, not some ugly muddled color (highlight from sprite undoes shadow from layers)." (brillo + oscuridad da un píxel normal, no un color mezclado feo — el resalte de un sprite anula la sombra de las capas).

### Estructura recomendada

Para explotar eficientemente este modo:

- Plano A (foreground): tiles de prioridad alta.
- Plano B (background): tiles de prioridad baja.
- Sprites: prioridad variable según posición.

Típicamente requiere un "priority map" (mapa de prioridad) que indique si cada zona debe mostrarse sobre o bajo el primer plano.

## Notas / gotchas

- El modo se activa con el mismo registro que la resolución de pantalla (ver entrada de resoluciones de pantalla), no con un registro independiente.
- Los colores especiales de sprite (paleta 3, color 14 para brillo; paleta 3, color 15 para oscuridad) hacen que el sprite en sí se vuelva transparente — solo actúan como modificador del píxel de fondo.
- Cuando ambas reglas indican "más oscuro", el resultado sigue siendo solo un 50% más oscuro, no se acumula el oscurecimiento.
- Brillo + oscuridad se cancelan mutuamente y dan un píxel normal, evitando colores mezclados extraños.
- Requiere planificar cuidadosamente qué tiles/sprites llevan el bit de prioridad alta o baja (un "priority map") para lograr el efecto deseado.

## Referencias cruzadas

- (pendiente de enlazar)
