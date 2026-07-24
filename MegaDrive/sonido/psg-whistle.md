---
titulo: Silbido (whistle) con el PSG
plataforma: [MegaDrive]
categoria: [sonido]
tags: [psg, silbido, eef, envolvente, instrumento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/psg-whistle]
---

# Silbido (whistle) con el PSG

## Resumen

Explica cómo lograr un silbido convincente usando un canal de onda cuadrada del PSG, alternando rápidamente entre el tono base y un tono ligeramente más grave. Incluye el dato exacto del instrumento en formato EEF de Echo y una anécdota sobre cómo se hizo el silbido de los puertos de 8 bits de Sonic 1.

## Detalle técnico

Los silbidos son instrumentos que pueden ser muy útiles de fondo para música alegre. Los canales de onda cuadrada del PSG pueden hacer silbidos bastante buenos, así que veamos cómo funciona.

El instrumento (en el formato PSG de Echo, EEF) se ve así:

```
00 91 FE 02 92 FF
```

lo que produce el siguiente comportamiento (descrito a partir del gráfico de la fuente): la envolvente es simplemente un instrumento PSG básico (empieza en el pico, decae un poco y luego sostiene ahí de forma plana). La parte importante, sin embargo, está en el tono: cada 1/60 de segundo, alterna entre el tono original y dos semitonos más grave. Esto es lo que da el sonido de silbido.

Yendo más allá: el tono base concreto usado aquí es E en la sexta octava del PSG en Echo (lo que significa que el silbido alterna entre E y D). Se puede elegir un tono distinto para hacer una melodía entera con silbidos.

### Silbidos de Sonic 1 (versión de 8 bits)

Crédito a quien corresponde. Los puertos de 8 bits de Sonic 1 tienen fases de bonus con un silbido de PSG, así que la pregunta surgió en Twitter sobre cómo se hizo. Esto es lo que respondió Yuzo Koshiro (traducido del japonés):

> Creo que era algo como `l128 o6 dedededede`.

El "código" ahí es sintaxis MML:

- `l128` es la duración de nota (1/128, que dura un frame al tempo usado)
- `o6` es la octava del PSG (nótese que esta es la sexta octava según el driver de sonido usado)
- `d` y `e` son sus respectivas notas

De cualquier forma, su memoria era correcta porque así suena exactamente en el juego (*y* mml2esf usó por casualidad los mismos ajustes). El instrumento de silbido de PSG explicado en esta página recrea este comportamiento, y añade una envolvente simple encima (en particular, ayuda a distinguir cuándo se toca dos veces seguidas con el mismo tono base).

## Notas / gotchas

- La alternancia de tono ocurre cada 1/60 de segundo (un tick/frame), no de forma continua.
- El tono base usado en el ejemplo es E en la 6ª octava del PSG (en la numeración de octavas de Echo), alternando con D (dos semitonos más grave).
- Añadir la pequeña envolvente encima del patrón de alternancia de tono ayuda a distinguir repeticiones consecutivas de la misma nota.

## Referencias cruzadas

- (pendiente de enlazar)
