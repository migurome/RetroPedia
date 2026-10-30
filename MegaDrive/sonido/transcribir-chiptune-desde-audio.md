---
titulo: Transcribir chiptune (Game Boy) desde audio grabado sin oírlo
plataforma: [General]
categoria: [sonido]
tags: [analisis, transcripcion, nnls, espectrograma, game-boy, chiptune, python]
estado: borrador
fecha: 2026-10-25
fuentes: [experimentos propios con versus.ogg de Super Smash Land (musica_fm/herramientas)]
---

# Transcribir chiptune (Game Boy) desde audio grabado sin oírlo

## Resumen

Método que funcionó para sacar notas, ritmo y timbres de una pista de estilo Game Boy (2 pulsos, canal de onda y ruido) en OGG, solo con numpy/scipy: rejilla temporal, separación de voces por síntesis y comparación del resultado renderizado. Los detectores de un solo tono y las imágenes «a ojo» no bastan.

## Detalle técnico

1. **Afinación global:** se miden los picos de notas largas con FFT larga e interpolación parabólica. En el caso estudiado todo estaba unos +16 cents por encima de A = 440 Hz. Furnace permite fijar la afinación (A-4 en Hz).
2. **Rejilla:** los onsets genéricos (flujo espectral) dan ruido. Funcionó más:
   - mirar dónde cambia la nota dominante (voz más fuerte por fotograma);
   - ajustar periodo y fase a esos cambios.

   El tempo no tiene por qué ser un número entero de fotogramas de 60 Hz (salió una celda de 0,0802 s = 4,81 fotogramas). Fíjate también en los tresillos.
3. **Separación de voces por síntesis:**
   - **Modelo:** cada celda de la rejilla se explica como bajo + pulso 1 + pulso 2, con amplitudes no negativas (`scipy.optimize.nnls`) y búsqueda por descenso coordinado de la nota de cada voz.
   - **Las plantillas se generan sintetizando la nota y pasándola por el mismo análisis** (misma ventana y FFT). Con plantillas teóricas (gaussianas por armónico), la nota grave se reparte entre el semitono de arriba y el de abajo, porque con 4096 muestras el lóbulo principal ocupa ±2 semitonos a 140 Hz. Con plantillas sintetizadas, el residuo bajó de 0,4 a 0,1.
   - **Timbres:** pulso de ancho d, armónico h = |sin(π·h·d)|/h (el 25 % no tiene los armónicos 4, 8, 12…). Para el canal de onda se usa el perfil de armónicos medido en una nota sostenida.
   - **Elegir el timbre por voz** (dejar que cualquier voz tome cualquier timbre y ordenar después por altura) evita que el bajo se «cuele» en las voces de pulso.
4. **Volumen por voz:** el nivel del fundamental de cada nota en el original. Así se ve qué voz es melodía (más fuerte) y cuál armonía, y se reparten las notas por canal.
5. **Ruido:** el canal de ruido no se distingue por la planitud espectral en agudos, porque los pulsos con aliasing también la tienen alta. Funcionó comparar la energía de 6-16 kHz en los primeros 25 ms de cada celda con la del resto de la celda. Los golpes salen +4 a +8 dB por encima y no aparecen en las celdas de control.
6. **Comprobación:** se renderiza el `.fur` con Furnace y se pasa el render por los mismos análisis que el original (voces por celda, cromagrama por celda, nivel por tramo, vibrato con el fundamental). Las diferencias señalan errores de transcripción o de volumen.

## Notas / gotchas

- **Medir el vibrato sobre un armónico:** cuidado, el de otra voz puede caer en la ventana de búsqueda. Mejor el fundamental con ventana estrecha (±2,5 %).
- **Mediana espectral:** el «suelo» calculado con mediana espectral sube en las transiciones de nota y con bajos de armónicos muy juntos; no indica por sí solo que haya ruido.
- **Graves espurios:** la salida de la Game Boy tiene un condensador que genera graves al cambiar los niveles de continua. Pueden parecer una voz y no lo son.

## Referencias cruzadas

- [Generar proyectos .fur de Furnace por script y exportarlos a VGM](furnace-fur-generar-y-exportar-vgm.md)
- [PSG (Programmable Sound Generator)](psg.md)
