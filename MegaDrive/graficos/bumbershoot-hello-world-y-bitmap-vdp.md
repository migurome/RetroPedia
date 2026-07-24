---
titulo: Bumbershoot Software — VDP desde cero: "Hello World" y bitmap con ASM puro
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bumbershoot, vdp, tiles, paletas, dma, blast-processing, texto, fuente, asm68000]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/02/27/genesis-graphical-basics/]
---

# Bumbershoot Software — VDP desde cero: "Hello World" y bitmap con ASM puro

## Resumen

Segunda entrada de la serie Bumbershoot Software (2018): construye, en ensamblador 68000 puro (sin SGDK), un programa que inicializa el VDP, carga una paleta y un tileset (un logo) en VRAM, dibuja ese logo como mapa de tiles en el plano A, imprime texto reutilizando una fuente expandida en tiempo de ejecución, y finalmente sustituye la carga manual de tiles por una transferencia DMA.

## Detalle técnico

### Resolución y organización de VRAM asumidas

- Resolución 320×224 (rejilla de 40×28 tiles) o 256×224 (32×28), tiles de 8×8 px, 4 bits por píxel (16 colores por tile), hasta 2048 patrones direccionables en VRAM (32 bytes cada uno, alineados).
- 4 paletas de 16 colores en CRAM, color de 9 bits.
- Planos de scroll de 32×32 o 64×64 celdas (el ejemplo usa un plano de 64×32, el tamaño utilizable más pequeño).
- Direcciones VRAM usadas en el ejemplo: carácter 0 dejado en blanco en `$0000`; fuente ASCII a partir de `$0020` (byte `$0400`); patrones del logo a partir de `$0200` (byte `$4000`). Plano A en `$C000`, plano B en `$E000`.
- Puertos del VDP: datos en `$C00000` (alias en `$C00002` para permitir `move.l`), control en `$C00004` (alias `$C00006`).

### Inicialización de acceso al VDP

El autor guarda las direcciones de los puertos en registros de direcciones para reutilizarlas, y activa la pantalla escribiendo el registro 1 del VDP:

```
movea.l #$c00000, a0
movea.l a0, a1
addq    #4, a1
;; ...
move.w  #$8411, (a1)
```

Existe una vía más corta para lo mismo, escribiendo directamente sobre la dirección absoluta del puerto de control:

```
move.w  #$8144, $c00004
```

### Carga de patrones (tiles) del logo en VRAM

Selecciona el registro de autoincremento a 2 bytes (`$8f02`) y apunta el comando de escritura en VRAM a la dirección `$0200` del logo (comando `$40200000`); copia los datos longword a longword con `dbra`:

```
move.w  #$8f02, (a1)
move.l  #$40200000, (a1)
movea.l #logo, a2
move.w  #((logoend-logo) / 4)-1,d0
@lp:    move.l  (a2)+, (a0)
        dbra    d0, @lp
```

### Carga de la paleta en CRAM

Comando de escritura en CRAM en la dirección 0 (`$c0000000`), copiando 8 longwords (16 colores de 16 bits) desde la tabla `pal`:

```
move.l  #$c0000000, (a1)
movea.l #pal, a2
moveq   #7, d0
@lp2:   move.l  (a2)+, (a0)
        dbra    d0, @lp2
```

### Trazado del logo como mapa de tiles (plano A)

Sitúa el cursor de escritura en el plano A (comando `$40940003`) y recorre una rejilla de 25 filas × 20 columnas escribiendo IDs de patrón consecutivos, dejando 44 celdas en blanco (`$0000`) al final de cada fila hasta completar el ancho de 64 celdas del plano:

```
move.l  #$40940003, (a1)
moveq   #$1, d2         ; Tile counter
moveq   #$18, d0        ; 25 rows
@row:   moveq   #$13, d1        ; 20 columns
@rlogo: move.w  d2, (a0)
        addq    #1, d2
        dbra    d1, @rlogo
        moveq   #$2b, d1        ; 44 blanks to start of next row
@rblnk: move.w  #$0000, (a0)
        dbra    d1, @rblnk
        dbra    d0, @row
```

### Rutina genérica de impresión de texto

Recibe en `d0` el comando de dirección VDP ya construido para el destino (con el bit de escritura de plano activado vía `ori #$4000, d0`) y en `a2` un puntero a una cadena terminada en byte 0; escribe cada carácter como word (ID de patrón) hasta encontrar el terminador:

```
strout: ori     #$4000, d0
        move.w  d0, (a1)
        move.w  #$0003, (a1)
        clr.l   d0
@lp:    move.b  (a2)+, d0
        beq.s   @done
        move.w  d0, (a0)
        bra.s   @lp
@done:  rts
```

### Expansión de fuente de 1 a 4 bits por píxel

Convierte una fuente monocroma (1 bit/píxel, empaquetada 8 píxeles por byte) al formato nativo del VDP (4 bits/píxel; un tile de 8×8 ocupa 8 longwords). Por cada byte de entrada genera un longword de salida desplazando cada bit a un nibble:

```
move.l  #$44000000, (a1)
movea.l #font, a2
move.w  #(fontend-font)-1,d0
@lp:    move.b  (a2)+, d1
        moveq   #7, d2
@lp2:   asl.l   #4, d3
        asl.b   #1, d1
        bcc.s   @nobit
        ori.b   #1, d3
@nobit: dbra    d2, @lp2
        move.l  d3, (a0)
        dbra    d0, @lp
```

### Sustitución por DMA ("blast processing")

El autor reemplaza el bucle manual de copia de tiles por una transferencia DMA, escribiendo longitud y dirección fuente en los registros de configuración DMA del VDP (accedidos aquí por su valor de comando directo, no por nombre simbólico) antes de lanzar la transferencia con el bit DMA activo en el comando de escritura a VRAM:

```
move.w  #$8114, (a1)    ; Enable DMA
move.w  #$8f02, (a1)    ; Word writes
move.w  #$9300 + (((logoend-logo) >> 1) & $ff), (a1)
move.w  #$9400 + ((logoend-logo) >> 9), (a1)
move.w  #$9500 + ((logo >> 1) & $ff), (a1)
move.w  #$9600 + ((logo >> 9) & $ff), (a1)
move.w  #$9700 + ((logo >> 17) & $ff), (a1)
move.w  #$4020, (a1)
move.w  #$0080, $ff0000
move.w  $ff0000, (a1)
```

El propio autor la sitúa como "aproximadamente ocho veces" más rápida que el bucle de copia por CPU equivalente.

### Tabla de direcciones y valores clave

| Elemento | Valor |
|---|---|
| Puerto de datos VDP | `$C00000` (alias `$C00002`) |
| Puerto de control VDP | `$C00004` (alias `$C00006`) |
| Plano A (VRAM) | `$C000` |
| Plano B (VRAM) | `$E000` |
| Carácter 0 (en blanco) | `$0000` |
| Fuente ASCII (VRAM) | `$0020` (byte `$0400`) |
| Patrones del logo (VRAM) | `$0200` (byte `$4000`) |
| RAM principal (intermedio usado antes del DMA) | `$FF0000` |

## Notas / gotchas

- Esta entrada usa **ensamblador 68000 puro**, no SGDK — es el enfoque "cada byte por mi orden" descrito en la entrada previa de la serie (`../herramientas/bumbershoot-preparando-entorno-genesis.md`).
- La rutina de expansión de fuente 1bpp→4bpp es reutilizable para cualquier fuente monocroma importada desde otro formato; reconstruye el nibble usando el carry de `asl.b` para ir empujando bits al longword de salida — conviene verificar el orden de bits contra la fuente de origen real antes de reutilizarla literalmente.
- El truco de disparar un DMA escribiendo los registros `$93`-`$97` con el valor de comando ya combinado in situ (en vez de nombrar constantes como `VDPREG_DMALEN_L`, como hace `dma-transfer.md`) es funcionalmente equivalente pero mucho menos legible; útil como referencia de "a qué se reduce todo por debajo" de las macros de un motor.
- El propio autor presenta este código como demostración didáctica, no como biblioteca reutilizable — carece de comprobación de límites (p. ej. no valida que el logo quepa en las 2048 entradas de patrón disponibles).

## Referencias cruzadas

- [Big Evil Corporation — primer "Hello World" en pantalla](bec-hello-world.md) — mismo objetivo (texto en pantalla desde cero en ASM puro) con un enfoque de macros distinto; buena comparación de dos estilos de código para el mismo problema.
- [Transferencias DMA](dma-transfer.md) — documenta con nombres simbólicos los mismos registros (`$9300`-`$9700`) que aquí aparecen escritos con valores de comando en crudo.
- [Referencia de comandos del VDP](vdp-commands.md), [Tiles y paletas](tiles-and-palettes.md) — formato de comando de dirección y de paletas usado en este artículo.
- [Bumbershoot Software — panorama de hardware y herramientas](../herramientas/bumbershoot-preparando-entorno-genesis.md) — artículo anterior de la misma serie.
