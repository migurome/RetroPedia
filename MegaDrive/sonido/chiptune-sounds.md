---
titulo: Sonidos chiptune con el YM2612 (onda seno, cuadrada, sierra, triángulo, tono NES)
plataforma: [MegaDrive]
categoria: [sonido]
tags: [ym2612, fm, chiptune, ondas, algoritmo, instrumento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/chiptune-sounds]
---

# Sonidos chiptune con el YM2612 (onda seno, cuadrada, sierra, triángulo, tono NES)

## Resumen

Instrumentos FM comunes para el YM2612 que imitan formas de onda "chiptune" clásicas: onda seno, onda cuadrada, onda sierra (sawtooth), onda triangular, y un tono al estilo NES (pulso al 25%). Incluye los valores de registro exactos de cada instrumento y el algoritmo/feedback usados.

## Detalle técnico

### Onda seno (sine)

La más básica: solo se necesita un operador conectado a nada (poner el TL de todos los demás a 127 para mantenerlos mudos). Se usa S4 por consistencia.

| Oper. | AR | DR | SR | RR | TL | SL | MUL | DT | RS | SSG-EG |
|---|---|---|---|---|---|---|---|---|---|---|
| S1 | 31 | 31 | 31 | 15 | 127 | 0 | 1 | 0 | 0 | Off |
| S2 | 31 | 31 | 31 | 15 | 127 | 0 | 1 | 0 | 0 | Off |
| S3 | 31 | 31 | 31 | 15 | 127 | 0 | 1 | 0 | 0 | Off |
| S4 | 24 | 31 | 0 | 12 | 6 | 0 | 1 | 0 | 0 | Off |

- Algorithm: 0
- Feedback: 0

### Onda cuadrada (square)

Para hacer una onda cuadrada se necesitan al menos dos operadores: el modulador debe tener un multiplicador de 2x. El TL debería estar entre 32 y 36 para mejores resultados: valores más bajos la hacen más afilada (y eventualmente demasiado dura), valores más altos la hacen más suave (y eventualmente apagada/muffled).

Añadir más moduladores a la cadena con los mismos valores ayuda a acercar la forma a una onda cuadrada real. Se puede ajustar su TL a gusto (cada eslabón tiene un efecto menor cuanto más largo sea la cadena), pero el multiplicador 2x es relativo al operador de salida, así que *no* hay que escalarlo en cada eslabón.

El feedback de S1 se usa para llevar el efecto más lejos; bajarlo simplemente lo apaga un poco más.

| Oper. | AR | DR | SR | RR | TL | SL | MUL | DT | RS | SSG-EG |
|---|---|---|---|---|---|---|---|---|---|---|
| S1 | 31 | 31 | 0 | 12 | 28 | 0 | 2 | 0 | 0 | Off |
| S2 | 31 | 31 | 0 | 12 | 32 | 0 | 2 | 0 | 0 | Off |
| S3 | 31 | 31 | 0 | 12 | 36 | 0 | 2 | 0 | 0 | Off |
| S4 | 24 | 31 | 0 | 12 | 16 | 0 | 1 | 0 | 0 | Off |

- Algorithm: 0
- Feedback: 7

### Onda sierra (sawtooth)

La onda sierra es igual que la onda cuadrada pero con un modulador 1x en vez de 2x.

| Oper. | AR | DR | SR | RR | TL | SL | MUL | DT | RS | SSG-EG |
|---|---|---|---|---|---|---|---|---|---|---|
| S1 | 31 | 31 | 0 | 12 | 28 | 0 | 1 | 0 | 0 | Off |
| S2 | 31 | 31 | 0 | 12 | 32 | 0 | 1 | 0 | 0 | Off |
| S3 | 31 | 31 | 0 | 12 | 36 | 0 | 1 | 0 | 0 | Off |
| S4 | 24 | 31 | 0 | 12 | 10 | 0 | 1 | 0 | 0 | Off |

- Algorithm: 0
- Feedback: 7

### Onda triangular (triangle)

Para la onda triangular se toma un enfoque distinto sumando ondas seno (por eso algoritmo 7). Una onda triangular se puede obtener sumando armónicos (múltiplos de la frecuencia base), reduciendo a la mitad la intensidad en cada paso (-6dB, o sumando 8 al TL).

Solo se dispone de cuatro operadores para trabajar, así que con suerte sonará lo bastante cerca para cumplir su función.

| Oper. | AR | DR | SR | RR | TL | SL | MUL | DT | RS | SSG-EG |
|---|---|---|---|---|---|---|---|---|---|---|
| S1 | 24 | 31 | 0 | 12 | 33 | 0 | 4 | 0 | 0 | Off |
| S2 | 24 | 31 | 0 | 12 | 25 | 0 | 3 | 0 | 0 | Off |
| S3 | 24 | 31 | 0 | 12 | 17 | 0 | 2 | 0 | 0 | Off |
| S4 | 24 | 31 | 0 | 12 | 9 | 0 | 1 | 0 | 0 | Off |

- Algorithm: 7
- Feedback: 0

### Tono al estilo NES (NES-like tone)

Este tema surgió en Twitter en el contexto de otro chip FM distinto. Cita original (D.M.88 en Twitter, japonés, paráfrasis del autor):

> Los ratios MUL que he llegado a usar para hacer una onda de pulso son: M2:C1 para 50%, M4:C321 para 25%, así que para 12.5% debería ser M8:C7654321. Intenté reproducirlo con 1 canal pero tuve dificultades con los algoritmos.
>
> Ahora, si se renuncia a 1ch, dividiendo en M8:C765 y M8:C432 y renunciando a M8:C1, se logra reproducir por poco.

El YM2612 es de 4 operadores (4-op) y el algoritmo 5 tiene exactamente esa disposición para la onda cuadrada al 25%, así que probemos hacerlo como un solo canal (el feedback cumple la misma función que con las ondas cuadradas aquí):

| Oper. | AR | DR | SR | RR | TL | SL | MUL | DT | RS | SSG-EG |
|---|---|---|---|---|---|---|---|---|---|---|
| S1 | 31 | 31 | 0 | 12 | 28 | 0 | 4 | 0 | 0 | Off |
| S2 | 24 | 31 | 0 | 12 | 25 | 0 | 3 | 0 | 0 | Off |
| S3 | 24 | 31 | 0 | 12 | 25 | 0 | 2 | 0 | 0 | Off |
| S4 | 24 | 31 | 0 | 12 | 25 | 0 | 1 | 0 | 0 | Off |

- Algorithm: 5
- Feedback: 7

La forma de onda real no se parece a una onda cuadrada al 25%, pero suena lo bastante cerca al menos. De nuevo, subir el TL de S1 o bajar su feedback se puede usar para apagar el sonido.

## Notas / gotchas

- Para la onda cuadrada y sierra, el multiplicador 2x/1x del modulador es siempre relativo al operador de salida — no hay que escalarlo en cadenas más largas de moduladores.
- El rango de TL sugerido para la onda cuadrada (32-36) es un punto de partida: valores más bajos = más afilada/dura, más altos = más suave/apagada.
- Con solo cuatro operadores disponibles, la onda triangular y el tono NES-like son aproximaciones — no reproducen la forma de onda exacta, pero suenan razonablemente cerca.
- En el tono NES-like, subir el TL de S1 o bajar su feedback sirve para apagar/suavizar el sonido.

## Referencias cruzadas

- (pendiente de enlazar)
