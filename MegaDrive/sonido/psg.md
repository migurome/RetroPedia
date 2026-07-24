---
titulo: PSG (Programmable Sound Generator)
plataforma: [MegaDrive]
categoria: [sonido]
tags: [psg, sn76489, atenuacion, ruido, canales, z80]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/psg]
---

# PSG (Programmable Sound Generator)

## Resumen

El PSG (Programmable Sound Generator) es una de las dos piezas de hardware de sonido de la Mega Drive, heredado de la SG-1000 y la Master System. Solo produce sonidos simples, pero es fácil de programar y hace de buen acompañante para los canales FM del YM2612.

## Detalle técnico

### Overview

El PSG proporciona:

- 3x canales de onda cuadrada (canales 0 a 2)
- 1x canal de ruido (canal 3)

Se usa generalmente para efectos de sonido e instrumentos de fondo, y el canal de ruido se usa a menudo para hi-hats. Se puede aprovechar mejor si se desea ("Door into Summer" de *Knuckles Chaotix* es un buen ejemplo de esto).

### 68000 y Z80

El PSG puede ser accedido tanto por el 68000 como por el Z80. Todo lo que se hace implica escribir bytes al puerto del PSG, así que primero hay que conocer su dirección. El puerto del PSG está en las siguientes ubicaciones para cada CPU:

- **68000:** en `$C00011`
- **Z80:** en `$7F11`

Constante de ensamblador para 68000:
```
PsgPort: equ $C00011
```

Constante de ensamblador para Z80:
```
PsgPort: equ $7F11
```

### Cambiar el volumen

Para obtener sonido de un canal del PSG hay que fijar dos cosas: su volumen y su tono. Empecemos con el volumen.

Los canales del PSG no tienen volumen sino "atenuación" (por así decirlo): cada vez que se incrementa, el canal se vuelve más silencioso. La atenuación va de 0 a 15, donde cada paso es -2dB: 0 es volumen máximo, 1 es -2dB, 2 es -4dB, y así hasta 14 en -28dB (15 silenciará el canal).

Cambiar la atenuación de un canal se hace escribiendo un único byte al puerto del PSG. El byte a escribir es el siguiente:

```
$90 OR (channel << 5) OR attenuation
```

Ejemplo en ensamblador 68000:
```
; d0 = channel (0..3)
; d1 = attenuation (0..15)

    ror.b   #3, d0
    or.b    d1, d0
    or.b    #$90, d0

    move.b  d0, (PsgPort)
```

Ejemplo en ensamblador Z80:
```
; a = channel (0..3)
; b = attenuation (0..15)

    rrca
    rrca
    rrca
    or b
    or $90

    ld (PsgPort), a
```

### Cambiar el tono (pitch)

Los canales 0 a 2 son ondas cuadradas y tienen un rango de tono razonable (aunque flojo en el lado grave). Tienen una frecuencia que va de 0 a 1023, aunque, al igual que con el volumen, funciona *al revés* (es decir, valor más bajo = tono más agudo).

La siguiente es una tabla de frecuencias del PSG a semitonos para la octava completa más baja que soporta. Para obtener tonos más agudos, dividir el valor entre 2 por cada octava que se suba:

| Semitono | Frecuencia |
|----------|-----------|
| C-3 | 851 |
| C#3 | 803 |
| D-3 | 758 |
| D#3 | 715 |
| E-3 | 675 |
| F-3 | 637 |
| F#3 | 601 |
| G-3 | 568 |
| G#3 | 536 |
| A-3 | 506 |
| A#3 | 477 |
| B-3 | 450 |

Cambiar el tono de un canal de onda cuadrada requiere escribir dos bytes al puerto del PSG. El primer byte es:

```
$80 OR (channel << 5) OR (frequency AND 0x0F)
```

El segundo byte es simplemente:

```
frequency >> 4
```

Ejemplo en ensamblador 68000:
```
; d0 = channel (0..3)
; d1 = frequency (0..1023)
; d2 = scratch

    ; Split the frequency into
    ; its two parts
    move.w  d1, d2
    lsr.w   #4, d2
    and.b   #$0F, d1
    and.b   #$3F, d2

    ; Prepare the first byte (the
    ; second one is d2 as-is)
    ror.b   #3, d0
    or.b    d1, d0
    or.b    #$80, d0

    ; Send the bytes
    move.b  d0, (PsgPort)
    move.b  d2, (PsgPort)
```

Ejemplo en ensamblador Z80:
```
; c = channel (0..3)
; de = frequency (0..1023)
; a,b = scratch

    ; We need the low 4 bits
    ; of the frequency first
    ld a, e
    and $0F
    ld b, a

    ; Build the 1st byte
    ld a, c
    rrca
    rrca
    rrca
    or b
    or $80

    ; Send 1st byte
    ld (PsgPort), a

    ; We need the upper six
    ; bits of frequency now
    ld a, e
    and $F0
    rrca
    rrca
    rrca
    rrca
    ld b, a
    ld a, d
    rlca
    rlca
    rlca
    rlca
    or a, b

    ; Send 2nd byte
    ld (PsgPort), a
```

### Canal de ruido

El canal de ruido funciona de forma diferente. Puede producir dos tipos de sonido:

- Ruido periódico (en realidad, un ciclo de trabajo de 1/16)
- Ruido blanco (ruido real)

El rango de tonos es más limitado:

- Tono alto
- Tono medio
- Tono bajo
- La frecuencia del canal 2

El canal de ruido no tiene su propia frecuencia, sino que ofrece tres tonos fijos distintos. Alternativamente, se puede tomar prestada la frecuencia del canal 2 para un control más fino del tono del ruido, pero en ese caso probablemente se quiera silenciar dicho canal.

Cambiar el tono del ruido implica escribir un único byte al puerto del PSG. Se usan los siguientes valores para cada una de las combinaciones posibles:

```
PSG_PERIODIC_HIGH:  equ $E0
PSG_PERIODIC_MID:   equ $E1
PSG_PERIODIC_LOW:   equ $E2
PSG_PERIODIC_CH2:   equ $E3
PSG_WHITE_HIGH:     equ $E4
PSG_WHITE_MID:      equ $E5
PSG_WHITE_LOW:      equ $E6
PSG_WHITE_CH2:      equ $E7
```

Si se usa la frecuencia del canal 2, también habrá que manipular ese canal por separado.

### Envolventes del PSG

El PSG no tiene soporte de hardware para envolventes; hay que simularlas por software manipulando los registros de volumen y frecuencia regularmente (p. ej. cada frame).

Echo tiene el formato EEF para almacenar envolventes del PSG, si no se sabe qué usar. Maneja tanto cambios de volumen a lo largo del tiempo como de tono (útil para "instrumentos" más complejos como los silbidos).

## Notas / gotchas

- La atenuación funciona al revés que un volumen normal: 0 = máximo, 15 = silencio, cada paso son -2dB.
- La frecuencia de los canales de tono también funciona al revés: valores más bajos producen tonos más agudos.
- El canal de ruido no tiene frecuencia propia salvo que se tome prestada la del canal 2 (en cuyo caso conviene silenciar el canal 2 si no se quiere oír también su tono).
- No hay envolventes por hardware: hay que simularlas actualizando volumen/frecuencia periódicamente (por ejemplo, formato EEF de Echo).

## Referencias cruzadas

- (pendiente de enlazar)
