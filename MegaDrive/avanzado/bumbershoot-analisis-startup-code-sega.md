---
titulo: "Análisis del startup code oficial de Sega (arqueología del boot code de ROMs comerciales)"
plataforma: [MegaDrive]
categoria: [avanzado]
tags: [startup-code, boot, tmss, z80, vdp, sgdk, reverse-engineering, referencia-indirecta]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/03/02/the-sega-genesis-startup-code/]
---

# Análisis del startup code oficial de Sega (arqueología del boot code de ROMs comerciales)

## Resumen

El autor diseca, instrucción por instrucción, el bloque de código de arranque que aparece —con variaciones menores— en un gran número de ROMs comerciales de Genesis/Mega Drive, y que según su investigación desciende de código de referencia distribuido por Sega a desarrolladores terceros. A diferencia del "Hello World" didáctico (ver referencia cruzada), este es el startup "real" tal y como circulaba en la época: limpia RAM de trabajo, arranca el Z80 con su propio programa de inicialización, y respeta un caso que el ejemplo simplificado ignora — que el botón RESET se pulse con la consola ya encendida y un juego en curso.

## Detalle técnico

### Origen del código

Según el autor, este bloque de arranque coincide (con variaciones) con el que usa **SGDK**, y este a su vez deriva de código original de **Paul W. Lee**, con ediciones posteriores de **Charles Coty** y **Stephane Dallongeville**. La hipótesis del autor es que en última instancia desciende de código de arranque "de fábrica" distribuido por Sega a los desarrolladores licenciados, lo que explicaría por qué variantes muy similares aparecen en tantos juegos distintos de la época.

### 1. Vectores y cabecera (primeros 512 bytes)

Los primeros 4 bytes son el valor inicial del stack pointer (puesto a cero), los 4 siguientes son el program counter inicial (apunta a `RESET`), y los 248 bytes restantes son la tabla de vectores de interrupción — normalmente todos apuntando a una rutina que ignora la interrupción, para evitar que una IRQ inesperada cuelgue el programa antes de completar la inicialización.

### 2. Detección de reset "en caliente"

```asm
RESET:  tst.l   $a10008
	bne.s   @SkipJoyDetect
        tst.w   $a1000c
@SkipJoyDetect:
	bne.s   @SkipSetup
```

A diferencia del ejemplo didáctico, este startup comprueba primero si el botón RESET se pulsó con el juego ya en marcha (probando los puertos de datos de los controladores). Si es así, salta toda la secuencia de reinicialización de hardware (`@SkipSetup`) y preserva el contenido actual de la RAM de trabajo — necesario para que un "soft reset" no destruya el estado del juego en curso.

### 3. Carga de la tabla de inicialización

```asm
lea     @Table,a5
movem.w (a5)+,d5-d7
movem.l (a5)+,a0-a4

@Table:
        dc.w    $8000, $3fff, $0100
        dc.l    $00a00000, $00a11100, $00a11200, $00c00000, $00c00004
```

Mismo patrón de "tabla compacta + `movem`" que en el ejemplo didáctico: d5=`$8000`, d6=`$3fff`, d7=`$0100`; a0-a4 quedan apuntando a Z80 RAM, control de bus del Z80 (dos direcciones) y los puertos de datos/control del VDP.

### 4. Comprobación TMSS

```asm
move.b  -$10ff(a1),d0
andi.b  #$0f,d0
beq.s   @VersionOK
move.l  #$53454741,$2f00(a1)
@VersionOK:
```

Se lee el nibble de versión de hardware; si es distinto de cero (hardware con TMSS), se escribe el longword `$53454741` — que interpretado como cuatro caracteres ASCII en vez de como número es la palabra **"SEGA"** — en el puerto correspondiente, paso obligatorio para que la BIOS deje ejecutar el programa.

### 5. Reseteo de registros de conveniencia

```asm
move.w  (a4),d0
moveq   #$00,d0
movea.l d0,a6
move    a6,usp
```

Se lee el registro de estado del VDP (para resetearlo), se pone d0 a cero, y ese cero se usa para inicializar tanto a6 como el **USP** (User Stack Pointer, el stack del modo no supervisor del 68000).

### 6. Carga de los 24 registros del VDP

```asm
moveq   #$17,d1
@FillLoop:
        move.b  (a5)+,d5
        move.w  d5,(a4)
        add.w   d7,d5
        dbra    d1,@FillLoop

        move.l  (a5)+,(a4)
        move.w  d0,(a3)
```

Datos de la tabla de registros VDP usados en esta secuencia:

```asm
dc.b    $04, $14, $30, $2c, $07, $54, $00, $00
dc.b    $00, $00, $00, $00, $81, $2b, $00, $01
dc.b    $01, $00, $00, $ff, $ff, $00, $00, $80
dc.l    $40000080
```

Con estos valores, la disposición de memoria de vídeo resultante queda: Scroll Plane A en `$C000`, Scroll Plane B en `$E000`, Window (invisible por defecto) en `$B000`, tabla de atributos de sprites en `$A800` y tabla de scroll horizontal en `$AC00`. El longword final (`$40000080`) es un comando DMA que se usa más adelante para el clear de VRAM.

### 7. Inicialización del Z80 con programa propio

```asm
move.w  d7,(a1)
move.w  d7,(a2)
@ZWait: btst    d0,(a1)
	bne     @Zwait

moveq   #$25,d2
@ZFill: move.b  (a5)+,(a0)+
        dbra    d2,@ZFill

move.w  d0,(a2)
move.w  d0,(a1)
move.w  d7,(a2)
```

A diferencia del ejemplo didáctico (que solo detiene el Z80 y lo deja así), este startup **copia 38 bytes de código Z80** a la RAM del coprocesador antes de sacarlo de reset. El programa Z80 embebido, una vez desensamblado, es:

```asm
xor     a
ld      bc, $1fd9
ld      de, $0027
ld      hl, $0026
ld      sp, hl
ld      (hl), a
ldir
pop     ix
pop     iy
ld      i, a
ld      r, a
pop     de
pop     hl
pop     af
ex      af, af'
exx
pop     bc
pop     de
pop     hl
pop     af
ld      sp, hl
di
im      1
ld      (hl), $e9
jp      (hl)
```

Este pequeño programa deja limpios los registros del Z80 (incluidos el juego alterno `af'`/`exx` y los registros `i`/`r`), configura el stack e interrupciones del Z80, y finalmente coloca un opcode `$e9` (`jp (hl)`) en memoria antes de saltar a él — un patrón de auto-modificación mínimo característico del código de arranque de la época.

### 8. Limpieza de la Work RAM del 68000

```asm
@Clear: move.l  d0,-(a6)
        dbra    d6,@Clear
```

Escribe longwords de cero en direccionamiento predecremento, recorriendo los 64KB superiores de RAM de trabajo (16K longwords, coherente con el valor `$3fff` cargado en d6 al principio). Este paso **no existe** en el ejemplo didáctico simplificado.

### 9. Limpieza de VRAM, CRAM y VSRAM

```asm
move.l  (a5)+,(a4)
move.l  (a5)+,(a4)

moveq   #$1f,d3
@ClrC:  move.l  d0,(a3)
        dbra    d3,@ClrC

move.l  (a5)+,(a4)

moveq   #$13,d4
@ClrVS: move.l  d0,(a3)
        dbra    d4,@ClrVS
```

Valores de control VDP asociados a este bloque: `dc.w $8104, $8f01, $c000, $0000, $4000, $0010`. Primero termina de configurar el modo de auto-incremento en VRAM (ya limpiada por DMA previamente, igual que en el ejemplo didáctico), luego limpia los 32 longwords de CRAM (todas las paletas) y por último los 20 longwords de VSRAM.

### 10. Silenciar el PSG

```asm
moveq   #$03,d5
@PSGLp: move.b  (a5)+,$0011(a3)
        dbra    d5,@PSGLp

dc.b    $9f, $bf, $df, $ff
```

Mismos cuatro valores de silencio (`$9f`, `$bf`, `$df`, `$ff`) que en el ejemplo didáctico, pero aplicados en bucle desde una tabla en vez de cuatro instrucciones `move.b` sueltas.

### 11. Cierre: restaurar registros y saltar a main

```asm
move.w  d0,(a2)
movem.l (a6),d0-d7/a0-a6
move    #$2700,sr
@SkipSetup:
	bra.s   main
```

El registro de estado (`sr`) se fija a `$2700`, deshabilitando todas las interrupciones enmascarables (idéntico al ejemplo didáctico). Nótese que la etiqueta `@SkipSetup` —el destino del salto cuando se detectó un reset en caliente en el paso 2— cae justo aquí, justo antes de saltar a `main`: un reset en caliente se salta toda la reinicialización de hardware pero llega al mismo punto de entrada del programa principal.

## Notas / gotchas

- El autor documenta varias **ineficiencias e imperfecciones** de este código, pese a ser (presuntamente) el estándar de facto de la época:
  - Las operaciones sobre VRAM usan incrementos de dirección subóptimos (podrían usar paso 2 en vez de 1 en algunos tramos).
  - El stack pointer del Z80 queda establecido incorrectamente a `$0000` en algún punto de la secuencia embebida.
  - El chip de sonido "no legacy" (es decir, hardware más allá del PSG/YM2612 básico en ciertas revisiones/clones) no queda reinicializado por este código.
  - El Z80 se saca de reset ("se libera") demasiado rápido, antes de que su propia rutina de limpieza de estado haya terminado de ejecutarse por completo.
- Estas imperfecciones sugieren que el código nunca se pulió del todo pese a su altísima reutilización — es plausible que generaciones de desarrolladores lo copiaran de un juego a otro sin revisarlo línea por línea, transmitiendo también sus defectos.
- A diferencia del "Hello World" de la entrada de referencia cruzada (pensado para claridad didáctica), este startup real prioriza robustez frente a: (a) reset en caliente con juego en curso, (b) coprocesador Z80 dejado en un estado limpio y ejecutando su propio programa en vez de simplemente detenido, y (c) limpieza completa de la Work RAM del 68000 — tres cosas que el ejemplo simplificado omite deliberadamente por brevedad.
- Comparar con `../graficos/bec-vdp-init.md` (implementación de Big Evil Corporation/Tanglewood): ese código también hace TMSS + Z80 + PSG + VDP, pero con una tabla de registros VDP distinta y sin el paso de detección de reset en caliente ni la limpieza completa de RAM — es decir, se sitúa a medio camino entre el "Hello World" mínimo y este startup "de producción".

## Referencias cruzadas

- [Hello World en código máquina puro para Mega Drive (anotado)](../cpu/bumbershoot-hello-world-anotado.md) — versión simplificada del mismo tipo de startup, sin manejo de reset en caliente ni inicialización real del Z80; útil para contrastar qué se sacrifica al simplificar.
- [Big Evil Corporation — inicialización de hardware al arranque](../graficos/bec-vdp-init.md) — otra variante intermedia de la misma secuencia de boot.
- [Cabecera de la ROM](../memoria/rom-header.md) — formato de los 512 bytes iniciales que preceden a este código.
- [SGDK — visión general](../herramientas/sgdk/overview.md) — SGDK usa una variante de este mismo startup code como parte de su runtime.
