---
titulo: Tarjetas de título de Sonic (fade-in parcial)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [sonic, efecto, paletas, fade, tecnica-visual]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/sonic-title-cards]
---

# Tarjetas de título de Sonic (fade-in parcial)

## Resumen

Explica cómo los juegos de Sonic hacen el efecto de fade-in parcial en las "title cards" (las tarjetas con el nombre de la zona que aparecen al empezar un acto): la tarjeta de título entra, se ve el escenario, y la tarjeta se va volando, con un fundido que afecta solo al escenario y no a la propia tarjeta.

## Detalle técnico

Los juegos de Sonic suelen mostrar una tarjeta de título al empezar un acto. La tarjeta con el nombre del escenario aparece, luego se puede ver el área de juego, y después la tarjeta de título se va. En algunos de los juegos esta transición se logra con un efecto de fade-in que solo afecta al propio escenario (pero no a la tarjeta de título).

El truco es que la tarjeta de título usa la paleta común ("common palette"), mientras que las paletas del escenario son las que se van desvaneciendo (fade in). Los objetos no se dibujan hasta que ocurre el fade-in… es decir, simplemente dejan que aparezcan de golpe ("pop in") y esperan que no se note demasiado (de hecho el autor tuvo que ralentizar mucho el fundido en su GIF de ejemplo para que se notara el efecto).

En el ejemplo visual (Sonic 1, Green Hill Zone acto 1): al empezar la tarjeta de título solo se ve el nombre del escenario sobre fondo negro; cuando la tarjeta está a punto de irse ya se ve el nombre del escenario superpuesto sobre el área de juego. En la animación ralentizada se aprecia que Sonic, así como el texto amarillo y blanco del HUD, aparecen de inmediato (porque usan la paleta común), mientras que todo lo demás se desvanece suavemente.

## Notas / gotchas

- Los elementos que usan la paleta común (Sonic, ciertos textos del HUD) aparecen instantáneamente ("pop in") en vez de desvanecerse, porque el fade-in solo afecta a las paletas del escenario.
- El autor tuvo que ralentizar deliberadamente el GIF de demostración para que el "pop in" resultara visible; en el juego real a velocidad normal apenas se nota.
- La página fuente no incluye código en C/ASM, tablas de datos ni valores hexadecimales concretos, ni detalles sobre el manejo de scroll o tiles: es una explicación conceptual acompañada de una captura y un GIF comparativos.

## Referencias cruzadas

- (pendiente de enlazar)
