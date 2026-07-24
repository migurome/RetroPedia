---
titulo: Big Evil Corporation — inicialización de hardware al arranque (serie Tanglewood)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bigevilcorporation, tanglewood, vdp, tmss, z80, boot, inicializacion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/03/09/sega-megadrive-3-awaking-the-beast/]
---

# Big Evil Corporation — inicialización de hardware al arranque (serie Tanglewood)

## Resumen

Tercera entrada de la serie de Matt (Big Evil Corporation, autor de Tanglewood): la secuencia completa de arranque en frío de una Mega Drive, desde el punto de entrada tras el reset hasta dejar el VDP, el Z80 y el PSG en un estado conocido y listo para usarse. A diferencia de `vdp-setup.md` (que solo cubre el VDP), esta entrada documenta la secuencia de boot completa de la máquina, incluida la firma TMSS y la inicialización del Z80.

## Detalle técnico

### 1. Detección de reset

```
EntryPoint:
   tst.w 0x00A10008
   bne Main
   tst.w 0x00A1000C
   bne Main
```

### 2. Limpieza de los 64KB de RAM de trabajo

```
move.l #0x00000000, d0
move.l #0x00000000, a0
move.l #0x00003FFF, d1
@Clear:
move.l d0, -(a0)
dbra d1, @Clear
```

### 3. Firma TMSS

En hardware con el chip TMSS (revisión 1 en adelante) hay que escribir la cadena `'SEGA'` en una dirección concreta para que la BIOS deje ejecutar el código; se comprueba antes el nibble de versión de hardware:

```
move.b 0x00A10001, d0
andi.b #0x0F, d0
beq @Skip
move.l #'SEGA', 0x00A14000
@Skip:
```

### 4. Inicialización del coprocesador Z80

El Z80 recibe un bloque de 42 bytes (0x29 en decimal, usado como contador `dbra`) de código de inicialización. Antes hay que solicitar el bus del Z80 y mantenerlo en reset:

```
move.w #0x0100, 0x00A11100
move.w #0x0100, 0x00A11200
@Wait:
btst #0x0, 0x00A11100
bne @Wait

move.l #Z80Data, a0
move.l #0x00A00000, a1
move.l #0x29, d0
@Copy:
move.b (a0)+, (a1)+
dbra d0, @Copy

move.w #0x0000, 0x00A11200
move.w #0x0000, 0x00A11100
```

Bloque de datos del Z80:

```
Z80Data:
   dc.w 0xaf01, 0xd91f, 0x1127, 0x0021
   dc.w 0x2600, 0xf977, 0xedb0, 0xdde1
   dc.w 0xfde1, 0xed47, 0xed4f, 0xd1e1
   dc.w 0xf108, 0xd9c1, 0xd1e1, 0xf1f9
   dc.w 0xf3ed, 0x5636, 0xe9e9, 0x8104
   dc.w 0x8f01
```

### 5. Inicialización del PSG (Programmable Sound Generator)

```
move.l #PSGData, a0
move.l #0x03, d0
@Copy:
move.b (a0)+, 0x00C00011
dbra d0, @Copy

PSGData:
   dc.w 0x9fbf, 0xdfff
```

### 6. Configuración de los 24 registros del VDP

En vez de escribir cada registro con una instrucción distinta, se recorre una tabla de 24 valores en un bucle, incrementando el número de registro en cada iteración:

```
move.l #VDPRegisters, a0
move.l #0x18, d0
move.l #0x00008000, d1

@Copy:
move.b (a0)+, d1
move.w d1, 0x00C00004
add.w #0x0100, d1
dbra d0, @Copy
```

Tabla completa de valores usados por el autor para este ejemplo de arranque:

```
VDPRegisters:
   dc.b 0x20 ; R0: Horizontal interrupt enable
   dc.b 0x74 ; R1: Vertical interrupt, display, DMA enabled
   dc.b 0x30 ; R2: Scroll Plane A pattern table at 0xC000
   dc.b 0x40 ; R3: Window Plane pattern table at 0x10000
   dc.b 0x05 ; R4: Scroll Plane B pattern table at 0xA000
   dc.b 0x70 ; R5: Sprite attribute table at 0xE000
   dc.b 0x00 ; R6: Unused
   dc.b 0x00 ; R7: Background color palette
   dc.b 0x00 ; R8: Unused
   dc.b 0x00 ; R9: Unused
   dc.b 0x00 ; R10: Horizontal interrupt raster frequency
   dc.b 0x08 ; R11: External interrupts, V/H scrolling enabled
   dc.b 0x81 ; R12: H40 mode (40 cells horizontal)
   dc.b 0x34 ; R13: Horizontal scroll table at 0xD000
   dc.b 0x00 ; R14: Unused
   dc.b 0x00 ; R15: Autoincrement disabled
   dc.b 0x01 ; R16: Vertical scroll 32, Horizontal scroll 64
   dc.b 0x00 ; R17: Window Plane X position 0
   dc.b 0x00 ; R18: Window Plane Y position 0
   dc.b 0x00 ; R19: DMA length low byte
   dc.b 0x00 ; R20: DMA length high byte
   dc.b 0x00 ; R21: DMA source address low byte
   dc.b 0x00 ; R22: DMA source address middle byte
   dc.b 0x00 ; R23: DMA source address high byte
```

### 7. Configuración de los puertos de mando

```
move.b #0x00, 0x000A10009 ; Controller 1 CTRL
move.b #0x00, 0x000A1000B ; Controller 2 CTRL
move.b #0x00, 0x000A1000D ; EXP port CTRL
```

### 8. Limpieza final de registros de la CPU y arranque del programa principal

```
move.l #0x00000000, a0
movem.l (a0), d0-d7/a1-a7

move #0x2700, sr

Main:
   jmp __main
```

### Direcciones de I/O relevantes en esta secuencia

- `0x00A00000` en adelante — áreas de acceso a coprocesadores (Z80, etc.).
- `0x00C00000` — puerto de datos del VDP.
- `0x00C00004` — puerto de control del VDP.

## Notas / gotchas

- TMSS es específico de hardware revisión 1 en adelante: el código comprueba el nibble de versión en `0x00A10001` antes de decidir si hace falta escribir la firma `'SEGA'` en `0x00A14000` — en hardware más antiguo ese paso se puede saltar sin problema.
- El Z80 tiene que quedar en un estado controlado (bus solicitado y en reset) antes de copiarle código de inicialización; el bucle `@Wait` espera activamente a que el bus quede disponible comprobando el bit 0 de `0x00A11100`.
- El orden importa: primero se limpia la RAM de trabajo, luego TMSS, luego Z80/PSG, y solo al final se configuran los 24 registros del VDP y se limpian los registros de la CPU antes de saltar al programa principal.
- Esta es la secuencia de arranque completa de la máquina (TMSS + Z80 + PSG + VDP); si solo interesa la parte de configuración de registros del VDP y limpieza de VRAM/CRAM/VSRAM ya en marcha (sin todo el boot de la consola), ver la referencia cruzada de plutiedev.com más abajo.

## Referencias cruzadas

- [Configuración inicial del VDP](vdp-setup.md) — fuente plutiedev.com; cubre en detalle la limpieza de VRAM/CRAM/VSRAM (incluida la advertencia de no usar `clr` para ello) que este artículo de Big Evil Corporation no desarrolla; ambas entradas son complementarias para el arranque completo.
- [Referencia de registros del VDP](vdp-registers.md)
- [Cabecera de la ROM](../memoria/rom-header.md)
