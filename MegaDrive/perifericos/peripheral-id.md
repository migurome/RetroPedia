---
titulo: Identificación de periféricos (Peripheral ID) en Mega Drive
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [peripheral-id, io, deteccion, protocolo]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/peripheral-id]
---

# Identificación de periféricos (Peripheral ID) en Mega Drive

## Resumen

La mayoría de los dispositivos conectables a la Mega Drive poseen un "ID" de cuatro bits que permite identificarlos antes de usarlos. Este identificador es una consecuencia del protocolo de comunicación, por lo que no todos los periféricos lo incluyen. Esta entrada documenta el procedimiento de lectura del ID y la tabla de IDs conocidos.

## Detalle técnico

### Protocolo de lectura

Configuración inicial: primero debe configurarse el puerto de control I/O de modo que el bit 6 sea salida y los demás entrada (similar a la configuración de los mandos). Es necesario pausar el Z80 durante el acceso al puerto.

Procedimiento de lectura: la extracción del ID requiere dos lecturas.

| ID bit | Valor a escribir | Lectura del puerto |
|--------|------------------|---------------------|
| 3 | `$40` | bit 3 OR bit 2 |
| 2 | `$40` | bit 1 OR bit 0 |
| 1 | `$00` | bit 3 OR bit 2 |
| 0 | `$00` | bit 1 OR bit 0 |

### Rutina en código

```
GetPeripheralId:
    FastPauseZ80
    move.b  #$40, 6(a0)
    move.b  #$40, (a0)
    nop
    nop
    lea     @Table1(pc), a1
    moveq   #$0F, d1
    and.b   (a0), d1
    move.b  (a1,d1.w), d1

    move.b  #$00, (a0)
    nop
    nop
    lea     @Table2(pc), a1
    moveq   #$0F, d0
    and.b   (a0), d0
    move.b  (a1,d0.w), d0

    move.b  #$40, (a0)
    ResumeZ80
    or.b    d1, d0
    rts

@Table1:
    dc.b  %0000,%0100,%0100,%0100
    dc.b  %1000,%1100,%1100,%1100
    dc.b  %1000,%1100,%1100,%1100
    dc.b  %1000,%1100,%1100,%1100

@Table2:
    dc.b  %0000,%0001,%0001,%0001
    dc.b  %0010,%0011,%0011,%0011
    dc.b  %0010,%0011,%0011,%0011
    dc.b  %0010,%0011,%0011,%0011
```

### IDs de periféricos conocidos

| ID | Periférico |
|-------|-----------|
| `1111` (`$0F`) | (indetectable) |
| `1101` (`$0D`) | Controlador Mega Drive |
| `1100` (`$0C`) | Controlador Mega Drive (nota especial) |
| `1011` (`$0B`) | Controlador Saturn |
| `1010` (`$0A`) | Impresora |
| `0111` (`$07`) | Multitap Sega |
| `0101` (`$05`) | Otros periféricos Saturn |
| `0011` (`$03`) | Ratón |
| `0001` (`$01`) | Justifier |
| `0000` (`$00`) | Menacer |

### Periféricos no detectables

Estos requieren interacción directa (no exponen ID):
- Controlador Master System
- Trackball Master System
- Paddle Master System
- Multitap EA
- Ten Key Pad
- Activator
- Teclado Miracle Piano
- Teclado XBAND

## Notas / gotchas

- "Beware! Since we messed with the data port the peripheral now may need some time before returning correct data." — al haber modificado el puerto de datos, el periférico puede necesitar tiempo antes de devolver datos correctos; se recomienda guardar el ID y esperar al siguiente fotograma antes de usar el periférico.
- El ID `1100` puede ocurrir si la consola se reinicia durante la lectura de un mando de 6 botones.

## Referencias cruzadas

- (pendiente de enlazar)
