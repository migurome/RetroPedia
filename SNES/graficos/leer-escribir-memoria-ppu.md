---
titulo: Leer y escribir memoria de la PPU (VRAM, CGRAM, OAM)
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, vram, cgram, oam, vmain, dma, registros]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Reading_and_writing_PPU_memory]
---

# Leer y escribir memoria de la PPU (VRAM, CGRAM, OAM)

## Resumen

Referencia de los tres tipos de memoria de la PPU (CGRAM/paletas, OAM/sprites, VRAM) y de los registros que la CPU usa para leerlos y escribirlos: cuándo es seguro acceder a cada uno, cómo funcionan sus modos de escritura/lectura doble, y el "quirk" del buffer de lectura de VRAM que provoca errores off-by-one si no se conoce.

## Detalle técnico

### Ventanas de tiempo de acceso

El acceso a las tres memorias está restringido a determinados periodos: VBlank, HBlank (solo CGRAM) o Force-Blank. Fuera de esas ventanas, los accesos simplemente no tienen efecto (no llegan a leer ni escribir el dato real).

### CGRAM (memoria de paletas)

**Tamaño y estructura**: 256 entradas de paleta de 15 bits cada una.

**Registros de acceso**:
- **CGADD**: fija el índice de color de CGRAM (escritura de 8 bits).
- **CGDATA**: escribe el dato de paleta (registro de escritura doble).
- **CGDATAREAD**: lee el dato de paleta (registro de lectura doble).

**Restricciones clave**:
- La CPU (S-CPU) solo puede acceder a CGRAM durante VBlank, HBlank o Force-Blank.
- CGDATA es de escritura doble: siempre hay que escribir en él un número par de veces.
- El dato de color solo se escribe realmente en CGRAM en la segunda escritura a CGDATA.
- El bit 15 se ignora al escribir, y se enmascara (queda a "open bus") al leer.
- Siempre hay que fijar la dirección de palabra de CGRAM con CGADD antes de leer o escribir.

**Proceso de escritura**: fijar CGADD con el índice de color, y hacer dos escrituras de 8 bits a CGDATA. La dirección se autoincrementa tras la segunda escritura.

**Transferencia por DMA**: usa el patrón "un registro, escritura doble" (patrón DMAP 2).

**Proceso de lectura**: fijar CGADD, y hacer dos lecturas de CGDATAREAD; hay que enmascarar el bit 15 del byte alto leído.

### OAM (memoria de atributos de objetos/sprites)

**Tamaño y estructura**: una tabla baja de 512 bytes más una tabla alta de 32 bytes.

**Registros de acceso**:
- **OAMADDL/OAMADDH**: fijan la dirección de OAM (combinadas forman 16 bits).
- **OAMDATA**: lectura/escritura de datos de sprite.
- **OAMDATAREAD**: lectura de datos de sprite.

**Restricciones clave**:
- La CPU solo puede acceder a OAM durante VBlank o Force-Blank.
- OAMDATA es de escritura doble cuando se escribe en la tabla baja.
- Al escribir en la tabla baja, el dato solo se escribe realmente en OAM en la segunda escritura a OAMDATA.
- Al escribir en la tabla alta, el dato se escribe en OAM en *cada* escritura a OAMDATA (no es doble en ese caso).
- Aun así, se recomienda tratar siempre OAMDATA como si fuera un registro de escritura doble, para simplificar el código y evitar errores de sincronización de la paridad interna.
- La dirección interna de OAM se resetea al último valor escrito en OAMADD en cuanto empieza el VBlank y la pantalla está activada.

**Buena práctica**: mantener un buffer de 544 bytes de OAM en Work-RAM y transferirlo completo por DMA durante VBlank.

### VRAM (memoria de vídeo)

**Tamaño y estructura**: dos chips SRAM de 32K x 8 bits, combinados lógicamente como 32K x 16 bits (64KB en total).

**Registros de acceso**:
- **VMAIN**: controla el modo de acceso (bits 0-1: incremento; bits 2-3: remapeo; bit 7: modo de incremento de dirección).
- **VMADD** (VMADDL/VMADDH): fija la dirección de palabra en VRAM (16 bits).
- **VMDATA** (VMDATAL/VMDATAH): lectura/escritura de VRAM.
- **VMDATAREAD** (VMDATALREAD/VMDATAHREAD): lectura de VRAM.

**Restricciones clave**:
- La CPU solo puede acceder a VRAM durante VBlank o Force-Blank.
- Si se accede a VMAIN, VMDATA o VMDATAREAD durante HBlank o con la pantalla activa, VRAM simplemente no se lee ni se escribe.
- VMDATA y VMDATAREAD no son registros de palabra completa: el byte bajo y el alto se acceden por separado.
- Siempre hay que escribir en VMAIN antes de realizar una transferencia de VRAM.

**Detalle del registro VMAIN**:

*Modo de incremento de dirección (bit 7)*:
- Con el bit a 0, la dirección interna de VRAM se incrementa al escribir en VMDATAL o al leer de VMDATALREAD.
- Con el bit a 1, se incrementa al escribir en VMDATAH o al leer de VMDATAHREAD.

*Bits de incremento (bits 0-1)*:
- `0b00`: incremento de 1.
- `0b01`: incremento de 32 (columnas de tilemap normal).
- `0b10`/`0b11`: incremento de 128 (columnas de tilemap de modo 7).

*Bits de remapeo de dirección (bits 2-3)*: normalmente en 0 (sin remapeo).

**Valores habituales de VMAIN**:
- `$80`: acceso por palabra completa (16 bits), incremento 1 (para tiles, tilemaps normales, offset-per-tile).
- `$00`: solo byte bajo, incremento 1 (tilemap de modo 7, tiles de 1bpp).
- `$80` (variante de solo byte alto): escritura de 8 bits a VMDATAH, incremento 1 (tiles de modo 7, tilemap partido).
- `$81`: columna de tilemap, incremento 32 (columnas de 32 palabras).
- `$02`: columna de tilemap de modo 7, incremento 128.

**Comportamiento de VMADD**: escribir en VMADDL o VMADDH hace que la PPU realice internamente una lectura de VRAM hacia un latch de lectura (esto rellena el buffer de lectura, ver el "quirk" más abajo).

**Quirk crítico al leer VRAM**: leer de VMDATAREAD devuelve *inmediatamente* el valor que ya estaba en el latch de VRAM, y solo *después* realiza la lectura real de VRAM e incrementa la dirección interna. Esto significa que para leer varios bytes/palabras seguidas de VRAM hace falta hacer antes una lectura "dummy" (de descarte) de VMDATAxREAD.

- Para una lectura única: basta con fijar VMADD y leer VMDATAREAD directamente (no hace falta lectura dummy).
- Para una lectura en bloque: fijar VMADD, hacer una lectura dummy, y luego leer repetidamente de VMDATAREAD.
- Si se omite la lectura dummy en una lectura en bloque, se produce un error de desfase de uno (off-by-one): los dos primeros bytes/palabras leídos contienen datos duplicados de la misma dirección de VRAM.

**Transferencias por DMA**:
- Para escribir: usar el patrón "dos registros" (DMAP patrón 1) hacia VMDATA.
- Para leer: usar el patrón `$81` (dos registros, en dirección PPU→CPU) desde VMDATAREAD.

### Ejemplos de código (verbatim)

Escribir un único color en CGRAM:

```
.a8
.i16
    lda     #COLOR_INDEX
    sta     CGADD
    lda     #.lobyte(COLOR_VALUE)
    sta     CGDATA
    lda     #.hibyte(COLOR_VALUE)
    sta     CGDATA
```

Escribir un bloque de CGRAM por DMA:

```
subroutine TransferBufferToCgram:
    CGADD = 0
    DMAP0 = 2
    BBAD0 = .lobyte(CGDATA)
    A1T0  = .loword(cgramBuffer)
    A1B0  = .bankbyte(cgramBuffer)
    DAS0  = .sizeof(cgramBuffer)
    MDMAEN = 1 << 0
```

Leer un único color de CGRAM:

```
.proc ReadCgramColor
    sta     CGADD
    lda     CGDATAREAD
    sta     zpTmpWord
    lda     CGDATAREAD
    and     #$7f
    sta     zpTmpWord + 1
    rts
.endproc
```

Escribir datos de palabra a VRAM (desde CPU, sin DMA):

```
    lda     #$80
    sta     VMAIN
    ldx     #VRAM_BG1_TILES_WADDR
    stx     VMADD
    rep     #$30
.a16
    ldx     #0
    Loop:
        lda     f:TileData,x
        sta     VMDATA
        inx
        inx
        cpx     #TILE_DATA_SIZE
        bcc     Loop
```

Escribir a VRAM por DMA:

```
subroutine WriteTileDataToVram:
    VMAIN = $80
    VMADD = vram_waddr
    DMAP0 = 1
    BBAD0 = .lobyte(VMDATA)
    A1T0  = .loword(data)
    A1B0  = .bankbyte(data)
    DAS0  = data_size
    MDMAEN = 1 << 0
```

Leer una única palabra de VRAM:

```
.proc ReadOneVramWord
    lda     #$80
    sta     VMAIN
    stx     VMADD
    ldy     VMDATAREAD
    rts
.endproc
```

Leer un bloque de VRAM (demuestra la lectura dummy):

```
    lda     #$80
    sta     VMAIN
    ldx     #$6000
    stx     VMADD
    ldy     VMDATAREAD      // lectura dummy (rellena el latch)
    ldy     VMDATAREAD      // dato real de $6000
    ldy     VMDATAREAD      // dato de $6001
    ldy     VMDATAREAD      // dato de $6002
```

Leer un bloque de VRAM por DMA:

```
.proc ReadVramWordData
    lda     #$80
    sta     VMAIN
    stx     VMADD
    ldx     VMDATAREAD      // lectura dummy
    lda     #$81
    sta     DMAP0
    lda     #.lobyte(VMDATAREAD)
    sta     BBAD0
    ldx     zpFarPtr
    stx     A1T0
    lda     zpFarPtr + 2
    sta     A1B0
    sty     DAS0
    lda     #1 << 0
    sta     MDMAEN
.endproc
```

### Tabla resumen de ventanas de tiempo

| Memoria | Escritura | Lectura |
|---------|-----------|---------|
| CGRAM | VBlank, HBlank, Force-Blank | VBlank, Force-Blank |
| OAM | VBlank, Force-Blank | VBlank, Force-Blank |
| VRAM | Solo VBlank, Force-Blank | Solo VBlank, Force-Blank |

## Notas / gotchas

- Fuera de las ventanas de tiempo permitidas, los accesos a estas memorias no fallan de forma ruidosa: simplemente no tienen efecto (no leen ni escriben), lo que puede pasar desapercibido y producir bugs silenciosos.
- CGDATA y OAMDATA (tabla baja) son de escritura doble: el dato solo se confirma en la segunda escritura. Escribir un número impar de veces deja el estado interno desincronizado para la siguiente operación.
- CGRAM es la única de las tres que también admite acceso durante HBlank, no solo VBlank/Force-Blank.
- El "quirk" de VMDATAREAD (necesidad de lectura dummy antes de leer un bloque) es la trampa más habitual al implementar lectura de VRAM: omitirla desplaza todos los datos leídos una posición.
- Siempre hay que escribir VMAIN antes de cada transferencia de VRAM; no asumir que conserva un valor "de fábrica" útil.
- La dirección interna de OAM se resetea automáticamente al valor de OAMADD en cada VBlank con pantalla activa — no hace falta (ni conviene) resetearla manualmente si ya se fija OAMADD cada frame.

## Referencias cruzadas

- Ver `vblank-routine.md` (uso práctico de estos registros dentro de buffers de OAM/CGRAM/tilemap transferidos en VBlank).
- Ver `scroll-mapa-grande.md` (uso de VMAIN en modo incremento de 32 para streaming de columnas de tilemap).
- Ver `hdma-ejemplos.md` (patrones DMAP 2/3 aplicados a CGDATA en el contexto de HDMA).
