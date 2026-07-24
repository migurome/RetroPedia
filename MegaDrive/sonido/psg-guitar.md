---
titulo: Guitarra con el PSG
plataforma: [MegaDrive]
categoria: [sonido]
tags: [psg, guitarra, vibrato, eef, envolvente]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/psg-guitar]
---

# Guitarra con el PSG

## Resumen

Explica cómo simular una de esas guitarras agudas de metal usando el PSG, mediante un pequeño slide de tono ascendente al inicio de la nota. Incluye los bytes exactos del instrumento en formato EEF de Echo, tanto la versión básica como una variante con vibrato para notas largas.

## Detalle técnico

¿Alguna vez quisiste hacer una de esas guitarras agudas del metal, pero te ha costado hacerlo con FM? Bueno, se puede simular razonablemente bien usando el PSG.

### El instrumento básico

Empecemos con el instrumento de guitarra básico del que partimos (recordar que esta guitarra solo funciona decentemente en tonos agudos).

El instrumento PSG (en el formato PSG de Echo, EEF) se ve así:

```
90 80 01 01 FE 02 FF
```

La parte de decaimiento es simplemente lo habitual en un instrumento (usar lo que resulte más cómodo). El truco importante aquí es el slide ascendente en los dos primeros ticks (la parte azul en el gráfico de la fuente): eso es lo que da la sensación de que el instrumento es más que una simple onda cuadrada.

Nótese que también es importante hacerlo rápido (o se sentirá torpe). En este caso pasa un tick en -2 semitonos, un tick en -1 semitono, y luego se asienta en la nota prevista (donde un tick es 1/60 de segundo).

### Añadiendo vibrato

Lo anterior funciona para notas cortas, pero para notas largas sonará un poco falso. En la práctica, conviene añadir algo de vibrato en medio de una nota larga (a discreción de cada uno, depende de cada tema en particular).

**No añadir vibrato a notas cortas, eso tiende a ser contraproducente.**

Si el driver de sonido soporta vibrato de forma nativa para el PSG (y no consume mucho espacio en el proceso), simplemente usar eso. Si no, puede que haya que simular el vibrato usando más instrumentos PSG (cambiando de instrumento al activar/desactivar el vibrato).

Este es el instrumento para el vibrato (con una vibración de ±1 semitono, visible como las franjas rojas y azules en la envolvente):

```
FE 02 12 12 02 02 82 82 02 FF
```

Y este es un instrumento para detener el vibrato (sin el ataque de la nota):

```
FE 02 FF
```

## Notas / gotchas

- Esta guitarra "solo funciona decentemente en tonos agudos" — no es adecuada para notas graves.
- El slide de tono ascendente debe hacerse rápido (dos ticks: -2 semitonos, -1 semitono, luego la nota) o el efecto se siente torpe.
- No añadir vibrato a notas cortas: solo tiende a ser contraproducente.
- El instrumento de "detener vibrato" reutiliza solo la parte ya decaída del instrumento original, en el semitono base, sin repetir el ataque de la nota.

## Referencias cruzadas

- (pendiente de enlazar)
