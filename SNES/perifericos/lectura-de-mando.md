---
titulo: Lectura de mando en SNES (protocolo serie)
plataforma: [SNES]
categoria: [perifericos]
tags: [mando, controller, lectura-automatica, io, protocolo, registros]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Controller_reading]
---

# Lectura de mando en SNES (protocolo serie)

## Resumen

SNES ofrece un mecanismo de lectura automática de mandos que libera al 65C816 de tener que hacer "bit-banging" manual en cada cuadro, además de permitir lectura manual bit a bit (heredada del esquema de NES). Esta entrada documenta ambos métodos: los registros involucrados, el orden de bits y el código de ejemplo típico.

## Detalle técnico

### Registros de hardware involucrados

- `$4016` (JOYOUT / JOYSER0): control de latch y lectura serie del puerto 1.
- `$4017` (JOYSER1): lectura serie del puerto 2.
- `$4200` (NMITIMEN): habilita/deshabilita la lectura automática.
- `$4212` (HVBJOY): indica si la lectura automática está en curso o ha terminado.
- `$4218`-`$421F` (JOY1–JOY4): registros de resultado de la lectura automática.

### Lectura automática

Se activa mediante el bit menos significativo de NMITIMEN ($4200). Cuando está activo, la consola lee los mandos automáticamente durante aproximadamente las primeras tres líneas de barrido (scanlines) del vblank.

Para saber si la lectura ha terminado se consulta el bit 0 de HVBJOY ($4212):

```
lda HBVJOY
and #1
bne :-
```

Solo tras confirmar que ese bit se ha puesto a 0 es seguro leer los registros JOY1-JOY4 con los resultados.

Punto crítico documentado: el bit 0 de JOYOUT ($4016) debe permanecer en 0 durante toda la lectura automática para que esta funcione correctamente; si se deja en 1, la lectura automática queda inhibida.

Se recomienda copiar los valores de JOY1-JOY4 a variables propias del juego cuanto antes, para evitar inconsistencias si la lógica de un cuadro se extiende más de lo previsto y se solapa con el siguiente ciclo de lectura.

### Lectura manual

Es el mismo esquema que usa el mando de NES. El registro de desplazamiento interno del mando se resetea escribiendo primero un 1 y después un 0 en el bit menos significativo de JOYOUT.

Cada lectura de JOYSER0 ($4016) o JOYSER1 ($4017) devuelve en realidad dos bits de datos, aunque los mandos estándar solo hacen uso del bit menos significativo (el otro bit se usa en periféricos con más líneas de datos, como el ratón).

Ejemplo de lectura manual de 16 bits adicionales (más allá de los que ya trajo la lectura automática), usando un patrón de "1 centinela" que se desplaza hasta salir por el acarreo:

```
lda #1
sta result_lo
stz result_hi
Loop:
lda JOYSER0
lsr
rol result_lo
rol result_hi
bcc Loop
```

### Detección de pulsación nueva ("edge detection")

Se recomienda guardar en cada cuadro una copia del estado de botones anterior, de forma que pueda compararse con el estado actual y así detectar qué botones "acaban de presionarse" (no estaban presionados en el cuadro anterior), lo cual es necesario para acciones que solo deben dispararse una vez por pulsación y no repetirse mientras el botón se mantiene apretado.

## Notas / gotchas

- Existe una ventana temporal entre el inicio del vblank y el momento en que realmente arranca la lectura automática; el código no debe asumir que los registros JOY1-JOY4 están listos inmediatamente al entrar en vblank.
- Leer los resultados justo después de comenzar el vblank puede requerir un pequeño retardo adicional antes de que los datos sean válidos.
- El bit 0 de JOYOUT ($4016) debe mantenerse en 0 durante la lectura automática; escribir ahí sin cuidado (por ejemplo al hacer lectura manual de otro periférico) puede romper la lectura automática de los mandos estándar.

## Referencias cruzadas

- (pendiente de enlazar)
