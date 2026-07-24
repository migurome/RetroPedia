---
titulo: "Bumbershoot Software — un bug de offset de canal en los registros $B0/$B4 del YM2612, diagnosticado con Gens KGen"
plataforma: [MegaDrive]
categoria: [sonido]
tags: [bumbershoot, ym2612, fm, algoritmo, feedback, panning, debugging, gens-kgen]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2024/10/19/a-musical-interlude/]
---

# Bumbershoot Software — un bug de offset de canal en los registros $B0/$B4 del YM2612, diagnosticado con Gens KGen

## Resumen

"A Musical Interlude" es mayoritariamente un artículo sobre mejoras de sonido en **Apple II/IIgs** (aproximadamente 70-75% del texto); solo la parte final trata sobre Genesis. Esa parte sí tiene contenido técnico concreto y aprovechable: un bug en un script de exportación de música que olvidaba aplicar el offset de canal a los dos últimos registros de configuración de un instrumento del YM2612, y cómo se diagnosticó con el visualizador de estado del YM2612 integrado en el mod Gens KGen. Esta entrada documenta solo esa parte; el resto del artículo (Apple II/IIgs) queda fuera del alcance de esta base de conocimiento centrada en Mega Drive.

## Detalle técnico

### El bug

El script (Python) que compone/exporta la partitura para el driver de sonido genera, para cada nota, un volcado de los registros del YM2612 que definen el instrumento. Dos líneas finales de ese volcado eran las siguientes:

```python
score[time].append((0xb0, instrument[-2]))
score[time].append((0xb4, instrument[-1]))
```

El registro `$B0` es **algoritmo + feedback** del canal, y `$B4` es **panning + AMS/PMS** del canal (ver mapa de registros del YM2612, referencia cruzada); ambos son registros *por canal*, no globales. En el YM2612, el canal 1 usa `$B0`/`$B4`, el canal 2 usa `$B1`/`$B5`, el canal 3 usa `$B2`/`$B6` (y análogamente `$B0+3`... para los canales 4-6 en el segundo banco).

El resto del volcado de instrumento sí sumaba correctamente el offset de voz/canal a cada registro, pero estas dos líneas finales (las últimas del volcado) se escribían siempre en `$B0` y `$B4` a secas, **sin sumarles el offset del canal**.

### Por qué pasó desapercibido en pruebas

Cita textual (parafraseada del análisis): en el canal 1 el offset de voz es 0, así que escribir literalmente en `$B0`/`$B4` daba, por pura coincidencia, el resultado correcto — "el canal 1 tenía esos valores configurados dos veces (al mismo valor, como resulta que pasó, así que su salida era exactamente la que se quería)". El canal 2, en cambio, debía recibir esos valores en `$B1`/`$B5`, pero como el bug seguía apuntando a `$B0`/`$B4`, el canal 2 se quedó con **lo que hubiera en esos registros por defecto en el hardware** (valores de algoritmo/feedback y panning obsoletos o incorrectos), produciendo un timbre equivocado.

### Diagnóstico con Gens KGen

El mod Gens KGen incluye herramientas de visualización del estado interno del YM2612. Comparando el estado del canal 1 frente al del canal 2 con esa herramienta, el autor pudo ver directamente que los valores de algoritmo/feedback y panning no coincidían entre canales cuando deberían haber sido consistentes con lo que el instrumento pedía, revelando que el timbre del canal 2 estaba siendo alterado por el bug.

### La corrección

El fix fue un cambio de dos bytes en el binario resultante de la partitura compilada: aplicar el mismo offset de voz/canal a esas dos últimas escrituras de registro que ya se aplicaba correctamente al resto del volcado de instrumento.

## Notas / gotchas

- Patrón de bug generalizable: al escribir herramientas de conversión/exportación que generan, a partir de una plantilla, un volcado de registros por canal aplicando un offset de voz a cada dirección de registro, es fácil aplicar ese offset a la mayoría de los campos y olvidarlo en los **últimos** campos de la tabla (los que se añaden o gestionan aparte del bucle principal de generación).
- Este tipo de bug puede ser invisible si solo se prueba en el canal/voz 0 (offset 0 no cambia nada), y solo se manifiesta al usar canales posteriores — conviene probar siempre con instrumentos asignados a canales distintos de 1, no solo el primero.
- El visualizador de estado del YM2612 de Gens KGen es una herramienta de diagnóstico útil específicamente para comparar registro a registro entre canales cuando se sospecha de un problema de timbre o configuración, más allá de simplemente escuchar el resultado.
- Esta entrada no cubre el resto del artículo original (mejoras de sonido en Apple II/IIgs mediante un contador interno de 17 bits derivado de una técnica del Ensoniq 5503 de la IIgs), por quedar fuera del alcance de plataforma de esta base de conocimiento.

## Referencias cruzadas

- [Mapa de registros del YM2612](ym2612-registers.md) — formato exacto de `$B0+` (algoritmo/feedback) y `$B4+` (panning/AMS/PMS) y la tabla de asignación de registro por canal que este bug vulneraba.
- [Operaciones comunes del YM2612](ym2612-operations.md) — procedimiento estándar de "cargar un instrumento", que es precisamente el tipo de volcado de registros donde ocurrió este bug.
