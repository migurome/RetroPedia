---
titulo: Sonido, parte I — el chip PSG — enfoque Big Evil Corporation
plataforma: [MegaDrive]
categoria: [sonido]
tags: [bigevilcorporation, tanglewood, psg, sn76489, secuenciador, atenuacion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/09/03/sega-megadrive-10-sound-part-i-the-psg-chip/]
---

# Sonido, parte I — el chip PSG — enfoque Big Evil Corporation

## Resumen

Décima entrada de la serie de Matt (Big Evil Corporation, autor de TANGLEWOOD), publicada el 03/09/2012. Primer artículo de sonido de la serie: explica el chip PSG (SN76489, heredado de la Master System) desde el 68000 directamente (sin pasar aún por el Z80), y construye un secuenciador mínimo capaz de reproducir una melodía de prueba variando tono y duración nota a nota.

## Detalle técnico

### Estructura del chip

El PSG ofrece 3 canales de onda cuadrada más 1 canal de ruido blanco. Cada canal de onda cuadrada se controla con dos registros: un registro de atenuación de 4 bits (`$0` = volumen máximo/sin atenuación, `$F` = silencio/atenuación total) y un registro de "contador de reinicio" (reset counter) de 10 bits que determina la frecuencia — el número de ciclos de reloj, divididos entre 16, hasta que la polaridad de la onda cambia.

### Puerto y formato de escritura

Todo el control se hace escribiendo bytes al puerto `$00C00011` (visto desde el 68000). El primer byte de cada comando lleva: bit 7 = bit de latch (1 = primer/único byte), bits 6-5 = ID de canal (0-3), bit 4 = tipo de dato (1 = atenuación, 0 = contador de reinicio), bits 3-0 = los 4 bits bajos del dato. Si el valor necesita más de 4 bits (caso de la frecuencia, de 10 bits), se envía un segundo byte: bit 7 = 0 (indica "no es primer byte"), bits 5-0 = los 6 bits altos restantes del dato.

Fórmula de frecuencia resultante (reloj NTSC 3579545 Hz):

```
Frecuencia = Reloj ÷ (2 × 16 × valor_registro)
Ejemplo: 3579545 ÷ (2 × 16 × 254) = 440.4 Hz
```

### Ejemplos mínimos: fijar tono y volumen

Configurar canal 0 a 440.4 Hz (valor de registro `$FE` = 254):

```
move.b #%10001110, 0x00C00011 ; Latch ON, canal 0, tipo datos contador, 4 bits inferiores
move.b #%00001111, 0x00C00011 ; Latch OFF, 6 bits superiores de datos
```

Poner canal 0 a volumen máximo:

```
move.b #%10010000, 0x00C00011 ; Latch ON, canal 0, tipo datos atenuación, 4 bits
```

### Secuenciador de notas

El ejemplo completo del artículo recorre una tabla de pares (retardo, valor de contador de reinicio) y, para cada nota, escribe el tono al PSG y espera el número de frames indicado antes de pasar a la siguiente:

```
move.b #%10010000, psg_control ; Canal 0 volumen máximo

lea chan0_notes, a0            ; Notas a registro a0
move.l #chan0_notes_count, d1  ; Número de notas a d1
subi.l #0x1, d1                ; -1 para contador

@NextNote:

move.w (a0)+, d0       ; Retraso a d0, incrementar puntero
move.w (a0)+, d2       ; Valor contador reset a d2, incrementar puntero
move.b d2, d3          ; Byte inferior contador reset a d3
and.b #%00001111, d3   ; Limpiar nibble superior (dejar 4 bits inferiores)
or.b #%10000000, d3    ; Bit Latch (7) ON, canal 0 (6-5), bit datos tono (4) OFF
move.b d3, psg_control ; Escribir a puerto PSG

move.w d2, d3          ; Contador reset a d3 nuevamente
ror.w #0x4, d3         ; Desplazar derecha 4 bits
and.b #%00111111, d3   ; Solo bits 5-0 (6 bits superiores del valor de 10 bits)
move.b d3, psg_control ; Escribir a puerto PSG

move.l d1, -(sp)       ; Respaldar d1
jsr WaitFrames         ; Retraso en frames está en d0
move.l (sp)+, d1       ; Restaurar d1

dbra d1, @NextNote     ; Rama hacia atrás para reproducir siguiente nota

move.b #%10011111, psg_control ; Finalizado - silenciar canal 0
```

Datos de la melodía de prueba (tomada de "The Entertainer" de Scott Joplin):

```
chan0_notes:
   dc.w 0x0010, 0x02f8, 0x0010, 0x02cd, 0x0010, 0x02a5, 0x0010, 0x01aa, 0x0008, 0x0000
   dc.w 0x0010, 0x02a5, 0x0010, 0x01aa, 0x0008, 0x0000
   dc.w 0x0010, 0x02a5, 0x0010, 0x01aa, 0x0010, 0x0000
   dc.w 0x0010, 0x01aa, 0x0010, 0x0193, 0x0010, 0x017c, 0x0010, 0x0152, 0x0010, 0x01aa
   dc.w 0x0010, 0x017c, 0x0010, 0x0152, 0x0008, 0x0000
   dc.w 0x0010, 0x01c4, 0x0010, 0x017c, 0x0008, 0x0000
   dc.w 0x0010, 0x01aa

chan0_notes_end
chan0_notes_len equ chan0_notes_end-chan0_notes
chan0_notes_count equ chan0_notes_len/4
```

### El PSG como DAC de 1 bit

El autor apunta que, fijando la salida de un canal de onda en positivo y conmutando muy rápido el registro de atenuación entre sus valores extremos, se puede simular un conversor digital-analógico de 1 bit y reproducir PCM de baja calidad — útil para voces o efectos cortos, aunque la memoria disponible impide usarlo para pistas completas.

## Notas / gotchas

- La atenuación funciona "al revés" de lo intuitivo: `$0` es volumen máximo y `$F` silencia el canal — coincide con lo ya documentado en `psg.md`, pero aquí se ve aplicado en un secuenciador real en vez de solo como fórmula de bits.
- El PSG no puede sonar de forma autónoma: necesita alimentación constante de datos, lo cual el autor da como motivación para pasar a usar el Z80 como coprocesador de sonido dedicado en el siguiente artículo de la serie (no cubierto aquí).
- El canal de ruido blanco se menciona pero se pospone explícitamente a un artículo posterior, tras mejorar el secuenciador.
- El autor reconoce la falta de herramientas de autoría de música propias en ese momento y comenta la intención de crear una.
- Es el mismo chip SN76489 de la Master System, incluido en Mega Drive por compatibilidad — coincide con lo ya documentado en `psg.md`.

## Referencias cruzadas

- [`psg.md`](psg.md) (fuente plutiedev.com) — cubre el mismo chip con foco en la mecánica de bits (atenuación, tono, canal de ruido, ambas CPUs). Esta entrada es el enfoque complementario de un desarrollador con juego comercial real: un secuenciador de notas funcional en 68000 puro, sin pasar aún por el Z80.
- [`smps-sound-driver.md`](smps-sound-driver.md) — driver de sonido SMPS usado en juegos comerciales de Sega, punto de comparación útil frente al secuenciador artesanal de este artículo.
