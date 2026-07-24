---
titulo: Erratas y quirks conocidos del hardware SNES
plataforma: [SNES]
categoria: [herramientas]
tags: [errata, quirks, bugs-hardware, ppu, dsp, dma, cpu]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Errata]
---

# Erratas y quirks conocidos del hardware SNES

## Resumen

Catálogo de comportamientos peculiares y bugs de hardware de la SNES que un programador debe tener en cuenta, recopilados a partir del análisis de hardware real y del desarrollo de emuladores (no son erratas de un manual concreto de Nintendo, sino quirks del silicio). Cubre PPU, sistema de audio (S-SMP/S-DSP/SPC-700), el multiplicador de Modo 7, la CPU 65c816, el controlador S-CPU/DMA, formatos de archivo y una limitación de hardware del cartucho.

## Detalle técnico

### Subsistema de vídeo (PPU)

- **Offset-per-tile y el primer tile**: el offset-per-tile nunca afecta al tile más a la izquierda (el primero de la fila), pensado para compensar el scroll horizontal cuando hay columnas parciales.
- **Color math y sprites**: el color math que afecta a sprites solo tiene efecto sobre los sprites que usan las últimas cuatro paletas, no sobre todas las paletas de sprite.
- **NMI al activar NMITIMEN con RDNMI ya activo**: si se activa el bit 7 de NMITIMEN (NMI de vblank) mientras el bit 7 de RDNMI ya está puesto, se dispara un NMI inmediato, lo que puede provocar varios NMIs en un mismo vblank si no se ha leído antes RDNMI. Solución: leer RDNMI antes de activar el NMI.
- **Prioridad de recorte de sprites por scanline**: cuando se supera el límite de sprites/slivers por scanline, la SNES descarta los slivers de mayor prioridad en vez de los de menor prioridad — al revés de lo esperable.
- **Sprites en X=-256**: los sprites colocados en X=$100 (-256) cuentan erróneamente para el límite de 32 sprites por scanline, y hacen que todos sus slivers de tile cuenten para el límite de 34 slivers por scanline.
- **Retardo de brillo en INIDISP ($2100)**: en la SNES de 1 chip, los cambios de brillo producen un fundido gradual que tarda 72 píxeles o más; en la SNES de 3 chips, el cambio ocurre en cuestión de píxeles.
- **Lectura anticipada de INIDISP**: la PPU no espera a que el valor esté puesto en el bus antes de intentar leerlo, lo que renderiza un píxel con un valor de INIDISP incorrecto. Solución: usar direccionamiento largo, por ejemplo `STA $8F2100`, para controlar el estado del bus.
- **Sprites 16x32 entrelazados rotos**: se tratan como si fueran 16x16 — se ignora la mitad inferior de 16x16 y la mitad superior de 16x16 queda comprimida a 16x8.
- **Flip vertical erróneo en 16x32/32x64**: el flip vertical en estos tamaños no oficiales hace que la mitad superior y la mitad inferior se volteen de forma independiente.

### Sistema de audio

#### S-SMP (interfaz de la CPU de sonido)

- **Cruce de escritura de 16 bits**: escribir en $2140-$2141 como operación de 16 bits puede también escribir en $2143, aunque puede ser difícil de provocar o incluso no existir realmente.
- **Carrera en lectura/escritura de registros**: lecturas simultáneas del SPC700 y escrituras de la S-CPU en los registros de comunicación ($2140-$2143) producen datos incorrectos. Solución: leer dos veces y comprobar que el valor es consistente.
- **Peligros del registro TEST ($F0)**: puede colapsar fácilmente el SPC700, con capacidad de ralentizar el reloj, deshabilitar el acceso a Audio-RAM o detener los timers.

#### S-DSP (procesador de sonido)

- **Desequilibrio del DAC NEC µPD6376**: en las consolas con este DAC hay un desequilibrio de audio, porque Nintendo conectó ambos pines Vref a un único condensador de 47 μF en vez de usar un condensador por pin como indica la hoja de datos.
- **Tasa de release fija**: la tasa de release del S-DSP es fija. Solución: cambiar a los modos de GAIN de pendiente lineal o exponencial a mitad de nota para simular un release.
- **Carrera entre ADSR/GAIN**: escribir primero ADSR1 y luego ADSR2/GAIN permite que el S-DSP lea valores obsoletos de ADSR2/GAIN. Solución: escribir ADSR2/GAIN antes que ADSR1, o cambiar de modo solo durante el estado de release.
- **Correlación del ruido LFSR**: el hardware interpreta los 15 bits completos del estado del LFSR como una muestra de ruido con signo, produciendo un ruido filtrado en paso alto distinto del típico de chips con LFSR de 1 bit.
- **Desbordamiento del delay de eco**: poner el registro de delay de eco (EDL) a 0 sobrescribe continuamente 4 bytes de ARAM al inicio de la página del buffer de eco. Con EDL=0 y ESA=$00 se corrompe la página cero ($0000-$0003). Prevención: activar el bit 5 del registro FLG para proteger las escrituras de eco.
- **Retardo al escribir EDL/ESA**: las escrituras a EDL solo surten efecto cuando el S-DSP llega al final del buffer de eco (hasta 7680 muestras / 240 ms a 32 kHz). Las escrituras a ESA pueden retrasarse una muestra.
- **Sondeo de KON/KOFF**: KON y KOFF se comprueban (poll) cada dos muestras, por lo que limpiar esos bits demasiado pronto puede hacer fallar el key-on/key-off de un canal.
- **Desbordamiento del decodificador BRR**: el decodificador satura la salida a un valor de 16 bits con signo y luego la recorta a 15 bits con signo, lo que puede corromper la salida si los codificadores no replican ese mismo comportamiento con muestras de amplitud grande.
- **Desbordamiento del filtro FIR**: el filtro usa suma saturada (clipped-sum) para los primeros 7 taps y suma con clamp solo en el último tap, lo que provoca clics si la ganancia del filtro supera 0dB. Prevención: mantener la suma absoluta de los registros FIR0-FIR7 ≤ 128.
- **Bug de interpolación gaussiana**: la tabla contiene un bug que puede causar un desbordamiento (pop audible) si la muestra BRR contiene tres valores negativos máximos seguidos.

#### SPC-700 (CPU de sonido)

- **Lógica de TSET1/TCLR1**: estas instrucciones realizan una comprobación de igualdad (flags z/n = ALU(A - valor_anterior)), no una comprobación de bit real.
- **Comportamiento de flags en MUL**: los flags reflejan solo el valor del registro Y (byte alto), no el resultado completo de 16 bits.
- **Límite de validez de DIV**: el resultado solo es válido si el cociente es ≤ 511 (resultado de 9 bits).
- **Comportamiento de flags en DIV**: los flags Z/N se basan solo en el valor del registro A (bits 0-7 del cociente).

### Multiplicador de Modo 7 (registro MPY)

- **Interferencia en la matriz de Modo 7**: el resultado del multiplicador de Modo 7 puede corromperse si una interrupción o una transferencia HDMA escribe en un registro de scroll de BG1 o en un registro de la matriz de Modo 7 entre las dos escrituras de M7A, ya que ambos comparten el mismo latch de doble escritura.

### CPU 65c816

- **Limpieza del byte alto al pasar a índice de 8 bits**: poner los registros de índice en modo de 8 bits (vía SEP, PLP o XCE) limpia el byte alto de X e Y.
- **Alcance de direccionamiento de JMP**: `JMP ($1234)` lee 2 bytes de $00:1234 (siempre en el banco 0), mientras que `JSR ($1234,x)` lee 2 bytes de PB:{$1234 + X} (en el banco de programa actual).
- **Efecto colateral de MVN/MVP**: las instrucciones MVN y MVP cambian el Data Bank (DB) al banco de destino de la transferencia.

### S-CPU (5A22)

- **Colisión de multiplicación/división**: iniciar una nueva operación escribiendo en $4203 (WRMPYB) o $4206 (WRDIVB) mientras la operación anterior todavía se está procesando puede hacer que el 5A22 escriba valores erróneos en RDDIV y/o RDMPY.

#### Restricciones de DMA

- **Direcciones de A-Bus inválidas para DMA**: el DMA no puede acceder a $21xx (espacio del B-Bus), $4000-$41ff, $4200-$421f, $4300-$437f (registros de MMIO/DMA), ni a la Work-RAM cuando el B-Bus apunta a WMDATA ($2180) — esto impide copias RAM a RAM directas.
- **Cuelgue en la versión 1 del chip (S-CPU)**: el chip puede colgarse si un DMA termina justo antes de que empiece un HDMA, algo especialmente problemático cuando un DMA está limpiando la WRAM durante el renderizado.
- **Conflicto HDMA/DMA en la versión 2 (S-CPU-A)**: una transferencia HDMA reciente hacia/desde INIDISP puede hacer que una transferencia DMA falle — los registros de tamaño del DMA quedan sin cambios en vez de ponerse a cero. Solución: fijar BBADn=$ff con patrón de transferencia 1 para escribir $21ff y luego $2100, usando valores idénticos para evitar el glitch de lectura anticipada de INIDISP.
- **Fallo al iniciar HDMA**: el HDMA puede fallar si una transferencia DMA termina justo cuando empieza el HDMA y el último valor leído por el DMA es 0, provocando que el canal HDMA se detenga en la scanline 0 durante todo el frame.
- **Timing de activación de canal HDMA**: activar un canal HDMA fuera del periodo de vertical blanking puede causar escrituras erróneas no deseadas en la PPU, debido a registros de estado del HDMA sin inicializar.

#### Entrada (mando)

- **Ventana de timing del auto-read**: la lectura automática del mando empieza entre H=32.5 y H=95.5 de la primera scanline del vblank, por lo que comprobar HVBJOY ($4212) justo después del vblank no garantiza por sí solo que la lectura ya haya terminado.
- **Estado variable en frames de lag**: el resultado del auto-read puede cambiar inesperadamente durante un frame de lag — conviene copiarlo a una variable propia o desactivar el auto-read mientras se ejecuta la lógica del juego.

### Problemas de formato de archivo

- **Detección de cabecera de ROM**: LoROM y HiROM colocan la cabecera de la ROM en offsets distintos, obligando a emuladores y flashcarts a adivinar el formato mediante heurísticas. Algunas Super Everdrive requieren que el checksum de la cabecera de la ROM sea correcto.
- **Limitaciones del formato SPC**: el formato SPC no captura el estado completo de la APU — está pensado para el estado previo a la canción, no para snapshots a mitad de reproducción. El formato omite las posiciones de los timers del S-SMP, la posición del buffer de eco, las posiciones del decodificador BRR, las posiciones del interpolador gaussiano, las envolventes de voz y las comunicaciones por APUIO durante la reproducción.
- **Ambigüedad de la etiqueta ID666**: el formato SPC tiene dos formatos distintos de etiqueta ID666 (texto y binario) y no existe un campo de versión para diferenciarlos.

### Hardware

- **Limitación del /RESET del cartucho**: un cartucho que fuerza a nivel bajo el pin /RESET del slot no resetea la PPU — solo resetea la S-CPU, la APU y la S-WRAM.

## Notas / gotchas

- Varios de estos quirks (HDMA ignorando flags de "fixed transfer", "decrement" o "do transfer"; timing de BRK/COP; timing del SPC ciclo a ciclo) son la causa raíz de bugs de emulación documentados en juegos concretos — ver [Juegos difíciles de emular](juegos-dificiles-de-emular.md) para casos reales asociados a cada uno.
- Al escribir en registros sensibles a timing (INIDISP, DMA, registros de Modo 7) conviene usar direccionamiento largo o secuencias de escritura específicas para evitar los glitches de lectura anticipada descritos arriba.

## Referencias cruzadas

- [Juegos difíciles de emular](juegos-dificiles-de-emular.md)
- [Glosario SNES](glosario.md)
- (pendiente de enlazar)
