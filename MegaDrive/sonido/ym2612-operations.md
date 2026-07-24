---
titulo: Operaciones comunes del YM2612
plataforma: [MegaDrive]
categoria: [sonido]
tags: [ym2612, fm, volumen, panning, pcm, registros]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/ym2612-operations]
---

# Operaciones comunes del YM2612

## Resumen

Guía práctica de los pasos necesarios para realizar operaciones habituales con el chip FM YM2612 de la Mega Drive: cargar un instrumento, reproducir/detener una nota, hacer pitch bend, cambiar volumen y panning, detener un canal, y reproducir muestras PCM. Remite al mapa de registros del YM2612 para el detalle de cada registro.

## Detalle técnico

Nota del original: revisar la [referencia de registros del YM2612](ym2612-registers) para más detalles, y recordar respetar los retardos necesarios después de cada escritura a un registro.

### Cargar un instrumento

El concepto de "instrumento" varía de un driver a otro, pero normalmente el formato de cada driver es un volcado de todos los registros del YM2612 afectados (*al menos* `$B0` y de `$30+` a `$8C+`, normalmente unos cuantos más). El procedimiento básico es:

1. Detener el canal si es necesario (para evitar ruidos extraños) — ver "Detener canal" más abajo.
2. Escribir todos los registros relevantes (según el formato).

Nota: hacer lo anterior probablemente reiniciará el volumen, así que si se quiere preservar el volumen a través de cambios de instrumento hay que hacer los mismos cambios de valor a TL que al "cambiar el volumen" (ver más abajo).

### Key-on (iniciar nota)

1. Key-off (registro `$28` con los bits 7-4 despejados/clear)
2. Escribir el byte alto de la frecuencia (registro `$A4+`)
3. Escribir el byte bajo de la frecuencia (registro `$A0+`)
4. Key-on (registro `$28` con los bits 7-4 activados/set)

### Key-off (detener nota)

1. Key-off (registro `$28` con los bits 7-4 despejados/clear)

### Pitch bend

1. Escribir el byte alto de la frecuencia (registro `$A4+`)
2. Escribir el byte bajo de la frecuencia (registro `$A0+`)

### Cambiar el volumen

No existe un registro de "volumen" por canal; en su lugar hay que reescribir las envolventes. En concreto, hay que cambiar el valor TL de cada operador de *salida* (no los moduladores, ya que eso cambiaría el timbre en vez del volumen).

- **TL de S1** (registro `$40+`): si el algoritmo es 7
- **TL de S2** (registro `$48+`): si el algoritmo es 4 o superior
- **TL de S3** (registro `$44+`): si el algoritmo es 5 o superior
- **TL de S4** (registro `$4C+`): siempre

La atenuación debe sumarse al TL del operador, p. ej. si se quiere atenuar 3dB (0.75dB × 4) y el TL del instrumento es 8, hay que escribir 8 + 4 = 12. Cuidado con no pasarse de silencio (si el resultado es mayor que 127, escribir 127 en su lugar).

### Cambiar el panning

Escribir en el registro `$B4` los siguientes valores en los bits 7 y 6:

- Centrado: activar ambos bits 7-6
- Pan a la izquierda: activar solo el bit 7
- Pan a la derecha: activar solo el bit 6

### Detener canal

Las siguientes operaciones detienen rápidamente la salida de un canal FM. Nota: esto sobrescribe parte de la envolvente del instrumento, así que si se quiere reutilizar el instrumento habrá que restaurar los valores después.

1. Escribir `$0F` en todos los valores RR (registros `$80+`, `$84+`, `$88+`, `$8C+`)
2. Key-off (registro `$28` con los bits 7-4 despejados/clear)

### Reproducir muestras PCM

Reproducir PCM a través del YM2612 es un tema completo en sí mismo. Hay que activar el modo DAC y luego escribir las muestras *en el momento exacto en que deben sonar*. En esencia, se reduce a estos pasos:

1. Escribir `$80` en el registro `$2B` para cambiar a modo DAC.
2. Escribir muestras en el registro `$2A` a intervalos regulares.
3. Escribir `$00` en el registro `$2B` cuando se termine.

Cómo abordar el segundo paso depende de cada implementación (algunos juegos cachean muestras en la RAM del Z80 y otros no; algunos almacenan todo el set de batería en RAM del Z80 y dejan el resto al 68000; algunos usan el timer A del YM2612 y otros cuentan ciclos, etc.). Hay muchos compromisos en cada enfoque y encontrar un buen equilibrio es crítico para la calidad de la reproducción.

Es muy recomendable hacer esto con el Z80. No hacer como hizo Tiertex en *Strider II*, donde todo el juego se congela para reproducir PCM porque el 68000 se encarga del trabajo.

## Notas / gotchas

- Detener un canal (escribiendo `$0F` en RR y haciendo key-off) *no* es instantáneo. Puede tardar hasta 128 muestras del YM2612 (unos 2.4ms), que al oído puede parecer instantáneo pero *no* lo es para las CPUs (son 18432 ciclos para el 68000 o 8601.6 ciclos para el Z80). No hacer nada que dependa de que la envolvente se reinicie completamente en la siguiente muestra porque no funcionará.
- Al cargar un instrumento se reinicia el volumen salvo que se reapliquen los mismos ajustes de TL usados al cambiar el volumen manualmente.
- Reproducir PCM con el 68000 en vez del Z80 puede congelar el juego (ejemplo citado: *Strider II* de Tiertex).

## Referencias cruzadas

- (pendiente de enlazar)
