---
titulo: Generar proyectos .fur de Furnace por script y exportarlos a VGM
plataforma: [MegaDrive]
categoria: [sonido]
tags: [furnace, fur, vgm, ym2612, sn76489, xgm2, linea-de-ordenes]
estado: borrador
fecha: 2026-10-25
actualizado: 2026-10-25
fuentes: [furnace-0.6.8.3-win32/papers/format.md, furnace-0.6.8.3-win32/papers/newIns.md, manual.pdf de Furnace 0.6.8.3, experimentos propios (port de Super Smash Land a Mega Drive)]
---

# Generar proyectos .fur de Furnace por script y exportarlos a VGM

## Resumen

Furnace (tracker multisistema) guarda las canciones en `.fur`, un formato binario por bloques documentado en `papers/format.md`. Se puede escribir un `.fur` desde Python sin tocar la interfaz y exportarlo a VGM (o renderizarlo a WAV) desde la línea de órdenes. Esta ficha recoge lo comprobado con Furnace 0.6.8.3 (formato 232) para el sistema «Sega Genesis/Mega Drive». Incluye las trampas que no vienen en la documentación.

## Detalle técnico

### Estructura mínima que Furnace acepta (versión 232)

- **Cabecera de 32 bytes:** `-Furnace module-`, versión (u16 = 232), reservado, puntero a `INFO` (= 32) y 8 bytes reservados. El archivo puede ir comprimido con zlib o sin comprimir; Furnace guarda comprimido.
- **Bloque `INFO`.**
  - **Chips:** `0x83` (YM2612, 6 canales) + `0x03` (SN76489, 4 canales). Es lo que crea la plantilla «Sega Genesis/Mega Drive» en las versiones modernas; `0x02` es el compuesto antiguo de DefleMask.
  - **Canales:** quedan 10: FM1-FM6 y PSG1-PSG3 + ruido.
  - **Campos:** son los de `format.md`, en orden. Los indicadores de compatibilidad se pueden copiar de un demo reciente (`demos/genesis/*.fur` v219-223).
  - **Al final:** el patchbay (con «automático» = 1), el *speed pattern* (hasta 16 valores) y tres punteros a bloques `ADIR`. Valen `ADIR` vacíos (número de carpetas = 0).
- **Flags de chip:** puntero 0 = valores por defecto, que es lo que hacen los demos. Un bloque `FLAG` es texto `clave=valor`.
- **Instrumentos `INS2`:** una lista de *features* (`NA` nombre, `FM`, `MA` macros) que acaba en `EN`.
  - **FM, a partir de la versión 224:** la base tiene 4 bytes (el último es el *block*), más 8 bytes por operador.
  - **Orden de los operadores:** es el de los registros, S1, S3, S2, S4, no el de los diagramas de algoritmos (S1-S2-S3-S4). Comprobado con el render: con el algoritmo 4, solo suenan el 3.º y el 4.º operador del archivo cuando son los únicos con TL 0. Un patch escrito en el orden del diagrama hay que reordenarlo, `[s1, s3, s2, s4]`, o los moduladores hacen de portadoras.
  - **Detune (DT):** se guarda desplazado: 3 = sin detune.
- **Patrones `PATN`:** codificación compacta por filas.
  - **Saltos de filas vacías:** `0x80|N` salta N+2 filas; `0x00` salta 1.
  - **Fin de datos:** `0xFF`.
  - **Byte de máscara:** bits nota / instrumento / volumen / efecto 0 / valor 0, o un byte extra para los efectos 0-3.
  - **Notas:** 0 = C-(-5), así que la nota de Furnace = MIDI + 48. 180 = note off.

La forma más segura de validar el escritor es un lector propio que también lea los demos de Furnace. Si el lector consume exactamente `tam_info` bytes de `INFO` en los demos y en lo generado, el diseño es correcto.

### Exportar sin abrir ventana

```
furnace.exe -loops 0 -vgmout salida.vgm cancion.fur
furnace.exe -loops 0 -output salida.wav cancion.fur   # render para comparar
```

`-help` no imprime nada (es un programa de ventana). Las opciones salen en `manual.pdf`. Tarda menos de 1 s por canción.

### Qué produce el VGM

- **Cabecera:** VGM 1.71 con relojes SN76489 = 3579545 e YM2612 = 7670454, y tasa de 60 Hz.
- **Canción con bucle:** el VGM lleva el punto de bucle. Furnace lo coloca algo después del inicio (en una prueba, 25 fotogramas) y añade ese mismo trozo al final, de modo que la vuelta es continua aunque «duración total ≠ duración del bucle».
- **Canción con `FF00` (parar):** VGM sin bucle (`loop offset = 0`).

### Dónde pone Furnace la marca de bucle (medido)

- **La regla:** la marca va en la primera escritura de cada canal *posterior* a la fila del bucle (las de esa misma fila no cuentan), y Furnace se queda con la más tardía. Lo que queda entre la fila del bucle y la marca lo vuelve a añadir al final, así que el bucle sigue siendo continuo. Lo que crece es el archivo.
- **En una canción de 4 compases con salto al compás 1 y redondas en un canal:** la marca queda en el compás 2 y el VGM dura 5 compases.
- **El problema:** un canal que calla al empezar el bucle y no vuelve a sonar hasta mucho después retrasa la marca hasta entonces. Un canal que solo suena en la intro la lleva hasta el final, y el VGM incluye el bucle dos veces.
- **Qué cuenta como escritura:**
  - un note off o un volumen igual al que ya tenía el canal no cuentan, porque Furnace no los escribe;
  - una nota a volumen 0 sí cuenta.
- **La solución:** en esos canales, poner una nota muda (volumen 0) una fila después de la del bucle y un note off en la siguiente. La marca queda a menos de un tiempo del punto de bucle.

## Notas / gotchas

- **El SN76489 suena dos octavas por encima de la nota escrita.** Medido en Furnace 0.6.8.3, con la WAV renderizada y con los periodos escritos en el VGM: un A-2 escrito da periodo 254 = 440 Hz. Lo mismo pasa con flags explícitas (`clockSel=0`, `chipType=0/1`) y sin tono lineal. Los demos de Mega Drive escriben las melodías del PSG en las octavas 2-3. El YM2612 sí suena a la altura escrita (A-4 = 440 Hz). Si se genera por script a partir de alturas reales, hay que restar 24 semitonos en los canales de tono del PSG.
- **No usar macros de volumen en el PSG si importa el tamaño del VGM.** En cuanto un canal del SN76489 toca un instrumento con macro de volumen, Furnace reescribe el volumen de ese canal en cada fotograma hasta el final de la canción, aunque la macro sea de 2 pasos y sin bucle (medido: 197 escrituras en 193 fotogramas; sin macro, 13). Con 4 canales del PSG son unos 450 bytes por segundo.
  - **Alternativa:** escribir la envolvente como pasos de la columna de volumen (una fila por paso), solo mientras dura la nota.
  - **Resultado en Super Smash Land MD:** los temas pasaron de 40-60 KB a 12-22 KB.
  - **La macro *duty* (modo de ruido) no tiene este problema.**
  - **El YM2612 tampoco:** sus envolventes son del chip y no cuestan escrituras. Lo que más ocupa en FM son los key on/off (3 bytes cada uno) y la frecuencia (6 bytes por nota). Un note off explícito antes de cada nota (staccato) añade otro key off; es más barato un instrumento que decae solo.
- **Ruido blanco con frecuencias fijas:** se puede poner con el efecto `2001` en vez de la macro *duty* = 1; suena igual (misma planitud espectral).
- **Ruido del SN76489:** la macro *duty* vale:
  - 0/1: ruido corto/largo con las 3 frecuencias fijas;
  - 2/3: ruido con la frecuencia de PSG3, que deja de poder usarse para tonos.

  Con frecuencias fijas, las notas C, C# y D eligen el preset (D = el más agudo).
- **Tempos que no son múltiplos de 1/60 s:** usar un *speed pattern*. Por ejemplo, `5,5,5,5,4` da 4,8 fotogramas por fila (0,08 s). La alternativa es el «tempo virtual», aunque el speed pattern es más fácil de leer en el editor.
- **Vibrato `04xy` en el YM2612:** `x` = 3 da unos 0,35 s por ciclo e `y` = 4 da unos ±21 cents (medido).
- **Instrumentos aditivos (algoritmo 7):** con los 4 operadores como portadoras a multiplicadores 1/2/3/5 y TL = nivel de cada armónico en dB / 0,75, se reproduce el principio del espectro de un pulso estrecho o de una onda arbitraria. Es útil para imitar el canal de onda o los pulsos del 12,5/25 % de la Game Boy.
- **Rutas en Windows:** Python bajo Git Bash necesita rutas Windows (`cygpath -w`) para escribir en `%TEMP%`.
- **El VGM en SGDK (XGM2):**
  - **Recurso:** `XGM2 nombre "tema.vgm"` en el `.res`. rescomp lo comprime mucho: en Super Smash Land MD, un VGM de 18 KB quedó en 3 KB y uno de 3,3 KB en 1,25 KB. En PCM a 6.650 Hz, esos dos temas serían más de 30 KB cada uno.
  - **Reproducción:** `XGM2_play(nombre)`. Convive con los efectos de `XGM2_playPCMEx` en los canales PCM, sin pasos extra.
  - **Temas que suenan una vez:** llamar a `XGM2_setLoopNumber(0)` antes de `XGM2_play`. El valor por defecto (-1) repite si el VGM lleva bucle.
  - **Si en el mismo juego hay música PCM y FM:** al cambiar de una a otra hay que parar la anterior, con `XGM2_stopPCM(canal)` o `XGM2_stop()`.

## Referencias cruzadas

- [PSG (Programmable Sound Generator)](psg.md)
- [Mapa de registros del YM2612](ym2612-registers.md)
- [Sonidos chiptune con el YM2612](chiptune-sounds.md)
- [Transcribir chiptune desde audio](transcribir-chiptune-desde-audio.md)
