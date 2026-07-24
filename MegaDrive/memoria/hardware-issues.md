---
titulo: Problemas de hardware conocidos
plataforma: [MegaDrive]
categoria: [memoria]
tags: [bugs-hardware, revisiones, dma, ym2612, emulacion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/hardware-issues]
---

# Problemas de hardware conocidos

## Resumen

Recopilación de bugs, peculiaridades y diferencias de comportamiento entre revisiones de hardware de Mega Drive/Genesis (incluyendo clones) relevantes para el desarrollo: CPU/memoria, vídeo, audio, periféricos y emulación.

## Detalle técnico

### Problemas de CPU y memoria

**El Z80 accediendo al bus del 68000 durante el acceso a puertos de I/O:**
"Si el Z80 intenta acceder al bus del 68000 cuando el 68000 está intentando acceder a los puertos de I/O, el ciclo de bus del Z80 puede acortarse y provocar la lectura de datos inválidos." La solución recomendada implica detener (halt) el Z80 durante las operaciones de puertos de I/O.

**Inicialización de memoria en flashcarts:**
Flashcarts de desarrollo como Everdrive inicializan la RAM a cero, ocultando bugs de variables sin inicializar que aparecerían en cartuchos reales, donde la RAM contiene basura impredecible.

**Instrucción `TAS` del 68000:**
La instrucción `TAS` se comporta de forma inconsistente entre revisiones. La mayoría de consolas no escriben de vuelta el resultado (funcionando como `BTST #7`), pero las revisiones model 3 VA2 sí lo escriben (funcionando como `BSET #7`).

**Instrucción `LD IYH` del Z80:**
El clon Genesis Flashback carece de implementación de `LD IYH,nn` (carga inmediata a IYH). Versiones antiguas de Echo se rompían por esto; las soluciones alternativas sustituían por `LD IY,4000h`.

### Problemas de vídeo

**Colisión de bus entre Z80 y DMA:**
El acceso simultáneo del Z80 al bus del 68000 y el inicio de una DMA puede provocar que la transferencia cargue basura. Las revisiones antiguas de consola muestran glitches severos; todo el hardware está afectado. La mitigación requiere detener el Z80 durante las operaciones de DMA, aunque esto arriesga artefactos de audio.

**Variación de capacidad de VSRAM:**
VSRAM abarca 40 palabras en la mayoría de revisiones, pero se extiende a 64 palabras desde model 2 VA4 en adelante. El comportamiento de scroll vertical en la columna -1 (los 15 píxeles más a la izquierda) cambió de mostrar basura en hardware antiguo a copiar la columna 0 en revisiones posteriores.

**Cruce del límite de 128KB en DMA:**
"Al hacer una transferencia DMA, los datos no pueden cruzar un límite de dirección de 128KB. Si se intenta, la transferencia dará la vuelta de nuevo al principio de ese bloque de 128KB." Esto afecta a la ROM de cartucho y a las transferencias de WORD-RAM de Mega CD, pero no a la RAM.

**Transferencia DMA de longitud cero:**
Fijar la longitud de la DMA a 0 provoca que "el VDP lo interprete como 65536 en su lugar". Esto arriesga corrupción de memoria. Emuladores antiguos como Gens tratan incorrectamente el cero como "sin operación", causando que hacks de ROM de Sonic carguen basura cuando los gráficos descomprimidos son múltiplos de 4KB.

**Márgenes seguros de pantalla:**
"Dos tiles (16px) a los lados y un tile (8px) arriba y abajo son propensos a recortarse dependiendo de los ajustes del televisor." En sistemas de 50Hz, la compresión de la resolución vertical crea bordes gruesos arriba/abajo que requieren elegir con cuidado el color de borde.

### Problemas de audio

**Registro de estado del YM2612:**
"El registro de estado del YM2612 solo debe leerse desde `$A04000` (68000) o `$4000` (Z80), y solo los bits 7, 1 y 0 tienen significado." Leer desde otras direcciones produce resultados inconsistentes entre revisiones. Los juegos que ignoran esta especificación sufren problemas de tempo/música: Hellfire va más lento en revisiones nuevas; Earthworm Jim tiene el tempo roto en revisiones antiguas.

**PSG en el bus del 68000:**
"El PSG en realidad es un componente del VDP, y por lo tanto se accede desde el bus del 68000." Esta consideración afecta a las soluciones alternativas para DMA.

**Capacidades de salida estéreo:**
- Model 1: estéreo por el jack de auriculares; mono por el jack AV
- Model 2: estéreo por el jack AV
- Model 3: mono por el jack AV independientemente del tipo de cable

**Pines de entrada de audio del cartucho:**
Los pines de audio-in del conector de cartucho quedan sin conectar en el Nomad y en model 3 (además de en la mayoría de clones). La mayoría de revisiones incluyen resistencias en la línea de audio, pero algunas revisiones seleccionadas las omiten, requiriendo una impedancia de cartucho de 16 ohmios.

**Soporte de SSG-EG:**
"La función SSG-EG del YM2612 no está correctamente implementada en los clones vendidos por AtGames... y provocará un ruido ensordecedor si se usa." Las soluciones alternativas implican evitar SSG-EG o detectar el hardware incompatible para desactivar la función.

### Problemas de periféricos

**Temporización de la línea TH del mando:**
Existe un requisito de retraso de 2 microsegundos entre alternar la línea TH (bit 6) y leer el estado de los botones, aunque la mayoría de mandos no lo exigen estrictamente.

**Compatibilidad de mandos de 6 botones:**
Los mandos de 6 botones requieren alternancias rápidas de TH para habilitar los botones extra. Algunos juegos que alternan la línea en exceso provocan un mal funcionamiento. El botón Mode permite recuperarse: mantenerlo pulsado durante el encendido (o Mode+X+C para el inalámbrico infrarrojo).

**Ausencia de botón Reset en el Nomad:**
El Nomad carece de botón Reset, lo que rompe funcionalidades que dependen de él (notablemente hace que X-Men sea imposible de terminar en hardware Nomad).

**Discontinuación del puerto de módem:**
Las consolas antiguas incluían puerto de módem; el hardware posterior lo eliminó. Los periféricos no deberían depender de la disponibilidad del puerto de módem.

**Retraso en la transición de pines de I/O de pull-up:**
Al cambiar pines de salida a entrada, "el valor antiguo permanece ya que el pull-up es lento — excepto en el Nomad, donde la transición ocurre más rápido". Esto rompe Decap Attack, que alterna la dirección del pin TL y espera leer el valor del botón inmediatamente.

**Compatibilidad con juegos de Master System:**
Los mandos de 3 botones requieren que TH se mantenga a nivel alto o flotante para compatibilidad retroactiva con juegos de Master System (los botones B/C se mapean a 1/2). Juegos que ponen accidentalmente TH como salida a nivel bajo rompen esta compatibilidad. La Mark III cablea TH como entrada de forma fija, impidiendo el uso de botones extra pero garantizando la compatibilidad retroactiva.

**Sports Pad en el Nomad:**
El Sports Pad de Master System funciona mal en el Nomad incluso en modo digital, aparentemente debido a rieles de alimentación ruidosos que interfieren con la detección de movimiento del trackball.

### Problemas de emulación

**Diferencias en la inicialización de memoria:**
Los emuladores típicamente inicializan la RAM a cero, a diferencia de la basura impredecible del hardware real. BlastEm permite configurarlo vía Settings → System → Initial RAM Value, poniéndolo en "random".

**Manejo de errores de dirección:**
"El 68000 no puede realizar accesos de palabra o palabra larga a direcciones impares; intentarlo provoca un error de dirección (address error)." Muchos emuladores antiguos omiten esta validación por rendimiento, permitiendo que bugs pasen desapercibidos.

**Retrasos de temporización de hardware:**
Escribir en los registros del YM2612 requiere retrasos entre escrituras sucesivas; el reset del Z80+YM2612 requiere un mínimo de 192 ciclos de reloj del 68000. Los emuladores típicamente omiten estos retrasos, permitiendo que código que falla en hardware real funcione en ellos.

**Detección de tipo de archivo por padding de ROM:**
Las ROMs sin padding pueden provocar una autodetección de formato incorrecta en emuladores, mostrando títulos con basura en lugar del contenido de la cabecera.

## Notas / gotchas

- Muchos de estos problemas solo se manifiestan en hardware real o en emuladores muy precisos (p. ej. BlastEm), no en emuladores permisivos como los antiguos Gens.
- El comportamiento de `TAS` cambia entre revisiones del 68000: puede o no escribir de vuelta el resultado.
- El registro de estado del YM2612 solo debe leerse desde las direcciones oficiales (`$A04000`/`$4000`); leer de otras direcciones da resultados inconsistentes según la revisión.
- El límite de 128KB en DMA y el caso de longitud cero (interpretado como 65536) son fuentes comunes de corrupción de memoria si no se manejan explícitamente.

## Referencias cruzadas

- (pendiente de enlazar)
