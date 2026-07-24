---
titulo: Caja (snare drum) FM con el YM2612
plataforma: [MegaDrive]
categoria: [sonido]
tags: [ym2612, fm, percusion, snare, algoritmo, instrumento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/fm-snare-drum]
---

# Caja (snare drum) FM con el YM2612

## Resumen

Análisis de una caja (snare drum) hecha con síntesis FM en el YM2612: cómo interactúan sus distintos componentes (impacto y ruido) para producir el sonido final. Hacer percusión convincente con FM es difícil y muchos juegos que lo intentan se quedan cortos, por lo que a menudo se recurre a PCM en su lugar; sin embargo, muchos drivers de sonido no permiten mezclar PCM libremente y el PCM consume mucho más espacio que un instrumento FM, de ahí el atractivo de hacer percusión con FM.

## Detalle técnico

### Valores del instrumento

| Oper. | AR | DR | SR | RR | TL | SL | MUL | DT | RS | SSG-EG |
|---|---|---|---|---|---|---|---|---|---|---|
| S1 | 31 | 24 | 12 | 6 | 0 | 2 | 15 | -3 | 0 | Off |
| S2 | 31 | 24 | 16 | 8 | 8 | 2 | 1 | 3 | 2 | Off |
| S3 | 31 | 31 | 20 | 10 | 4 | 0 | 1 | 3 | 0 | Off |
| S4 | 31 | 31 | 12 | 6 | 8 | 0 | 0 | -3 | 3 | Off |

- Algorithm: 4
- Feedback: 7

**Nota de la fuente ("Iwis says"):** si se usa esta caja pero resulta demasiado silenciosa o demasiado alta, cambiar el TL de los operadores S2 y S4 (asegurándose de mantenerlos iguales entre sí). Esos dos son los que dan el volumen final en este caso.

### Configuración del instrumento

La caja usa el algoritmo 4, que es básicamente dos fuentes FM de 2 operadores sumadas. Los dos "lados" aportan partes distintas del instrumento: una mitad aporta el impacto cuando empieza el golpe, y la otra mitad aporta el ruido residual (trailing noise).

Algoritmo 4 del YM2612: el operador S1 modula a S2, el operador S3 modula a S4, y luego sus salidas se mezclan.

### Mitad de impacto (Impact half)

Los operadores S3 y S4 aportan el impacto cuando se golpea el tambor.

- Por sí solo suena como un bombo (kick) sintetizado.
- El operador S3 empieza fuerte pero luego decae muy rápido, lo que resulta en un pequeño "thud" y la salida rápidamente se convierte en una onda senoidal (curiosamente, la forma de onda resultante se parece bastante a lo que haría una caja real).
- El operador S4 tiene un multiplicador de 0.5x ya que el "thud" necesita ser de tono grave. Cuando el operador S3 decae, lo que queda es apenas audible, lo que deja que el ruido destaque.

### Mitad de ruido (Noise half)

Los operadores S1 y S2 aportan el ruido residual de la caja.

- El ruido se produce sobremodulando el operador S2 (logrado subiendo al máximo el multiplicador y el feedback del operador S1, y haciéndolo lo más fuerte posible). El feedback del operador S1 en particular es crítico para lograr esto.
- El operador S1 decae más lento que el operador S2 para reducir la "ruidosidad" a medida que el ruido mismo decae (de lo contrario el ruido se sentiría demasiado fuerte cerca del final).

### Otros detalles

- La caja está pensada para tocarse con la nota C-2.
- El release rate de todos los operadores se fija de modo que siga decayendo a la misma velocidad que el sustain rate (porque a un tambor no se le puede hacer "key off").
- Se usan detunes distintos entre sí para que el instrumento se sienta un poco menos "perfecto". Solo tiene un efecto muy pequeño en la salida resultante, sin embargo.
- No se usa SSG-EG.

### Bonus: caja al estilo Sonic 3

Por diversión: si se cambia el valor MUL de S1 a 2, la caja FM resultante suena a algo que encajaría en *Sonic 3*. No, en serio, el parecido es asombroso.

## Notas / gotchas

- Para ajustar el volumen de este instrumento en concreto, cambiar el TL de S2 y S4 juntos (son los operadores de salida en el algoritmo 4); no tocar S1 ni S3, que son moduladores.
- El feedback de S1 es crítico para generar el componente de ruido; sin feedback alto no se logra la sobremodulación necesaria.
- Al ser percusión, el release rate se iguala al sustain rate porque el instrumento nunca recibe un "key off" real.

## Referencias cruzadas

- (pendiente de enlazar)
