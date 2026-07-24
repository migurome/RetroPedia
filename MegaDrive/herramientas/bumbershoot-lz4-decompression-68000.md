---
titulo: "Bumbershoot Software — descompresor LZ4 en ensamblador 68000"
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [bumbershoot, lz4, compresion, descompresion, 68000, asm, vdp, dma]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2025/07/12/lz4-decompression-on-the-68000/]
---

# Bumbershoot Software — descompresor LZ4 en ensamblador 68000

## Resumen

Artículo de 2025 (una semana después del pipeline de compresión del logo del proyecto CCA, ver referencia cruzada) donde el autor implementa el lado de **descompresión** de su variante casera de LZ4 directamente en ensamblador 68000, para poder desempaquetar en tiempo real dentro de una Genesis real los datos que había comprimido en el PC. El resultado cabe en "bastante menos de cien bytes de código". Incluye el listado completo y un par de sorpresas de la arquitectura 68000 (acceso desalineado y una peculiaridad del ensamblador ASMX con `SUB`/`SUBA`).

## Detalle técnico

### Formato LZ4: literales + backreference = "sequence"

LZ4 se basa en dos ideas: **backreferences** (en vez de repetir datos, indicar un offset y una longitud desde donde copiarlos) y que una copia puede seguir escribiendo más allá del final original de los datos fuente, repitiendo valores de forma natural (permite compresión tipo run-length "gratis").

La unidad fundamental de descompresión es una **sequence**: una cadena (posiblemente de longitud cero) de literales, seguida de una backreference. Estructura de una sequence:

- **Byte de longitudes:** nibble alto = longitud de la cadena de literales; nibble bajo = longitud de la backreference **menos 4**.
- **Bytes literales:** se copian tal cual.
- **Word de offset (2 bytes, little-endian):** distancia hacia atrás de la backreference.

Si una longitud (literales o backreference) alcanza 15 en su nibble, se extiende con bytes adicionales: cada byte extra vale 0-255, y el valor 255 señala "sigue habiendo más" (hay que seguir sumando bytes hasta encontrar uno menor que 255).

Garantía del formato que simplifica el detector de fin de stream: el último bloque contiene solo literales, y los datos comprimidos terminan justo antes de lo que sería un word de offset — así que basta un word de ceros como terminador.

### Listado completo (verbatim, tal como se extrajo del artículo)

Convención de registros: `A0` = puntero a los datos comprimidos (fuente), `A1` = puntero de escritura en destino, `D0` = valor de retorno (bytes descomprimidos), se preservan `D2`-`D3`/`A2`-`A3`.

```asm
.loop:  move.b  (a0)+,d0                ; Load lengths byte
        moveq   #0,d1
        move.b  d0,d1
        lsr.w   #4,d1                   ; Extract literal length
        beq.s   .bkref
        bsr.s   .rdlen                  ; Read supplemental bytes
.copy:  move.b  (a0)+,(a1)+             ; Copy literals
        dbra    d1,.copy

.bkref: moveq   #0,d2
        moveq   #0,d3
        move.b  (a0)+,d3                ; Load little-endian offset
        move.b  (a0)+,d2
        lsl.w   #8,d2
        or.w    d3,d2
        beq.s   .done                   ; Zero offset = terminator
.bkok:  move.l  a1,a2
        sub.l   d2,a2                   ; Calculate source address
        moveq   #0,d1
        move.b  d0,d1
        and.b   #$0f,d1
        bsr.s   .rdlen
        addq    #4,d1                   ; Add 4 for true length
.copy2: move.b  (a2)+,(a1)+             ; Copy backreference
        dbra    d1,.copy2
        bra.s   .loop

.rdlen: cmp.w   #15,d1
        bne.s   .rdone
        moveq   #0,d2
.rloop: move.b  (a0)+,d2
        add.w   d2,d1
        cmp     #$ff,d2
        beq.s   .rloop
.rdone: subq    #1,d1                   ; Adjust for DBRA
        rts

.done:  suba.l  a3,a1                   ; Compute length
        move.l  a1,d0
        movem.l (a7)+,d2-d3/a2-a3
        rts
```

Tamaño total del código: "bastante menos de cien bytes".

### Problema de alineación de memoria del 68000

El 68000 no puede leer directamente un valor de 16 bits desalineado (dirección impar), y LZ4 no garantiza ninguna alineación para sus words de offset. Por eso el offset se lee **byte a byte** y se reconstruye a mano con desplazamientos y OR (`move.b` × 2 + `lsl.w #8,d2` + `or.w d3,d2`) en vez de un simple `move.w (a0)+,d2`.

### Detalle de ensamblador: `SUB` vs `SUBA` con dos registros de dirección

Al calcular la longitud final descomprimida, el autor necesita restar dos registros de dirección (`A3` de `A1`). Según el manual oficial del MC68000, `SUB.L D2,A2` es una instrucción perfectamente legítima, pero **`SUB.L A3,A1` no lo es** — hace falta `SUBA.L A3,A1` explícitamente. Muchos ensambladores corrigen esto de forma automática y aceptan `SUB` como alias aunque el resultado deba codificarse como `SUBA`, pero **ASMX no lo hace**, así que hay que escribir `SUBA.L` a mano cuando ambos operandos son registros de dirección.

### Integración con el VDP tras descomprimir

Una vez descomprimidos los datos a RAM (en el ejemplo, a partir de `$FF0000`), se vuelcan al VDP con DMA/copia directa:

- **Paleta** (64 bytes a CRAM, dirección 0): comando `$c0000000`, bucle de 16 longwords (`moveq #15,d1`).
- **Patrones** (VRAM `$0000`): comando `$40000000`.
- **Nametables** (VRAM `$C000` y `$E000`): comando `$40000003`, bucle de `3071` longwords (`move.w #3071,d0`).
- **Activar pantalla:** `MOVE.W #$8144,(a3)` (registro 1 del VDP, activa display y modo Genesis).

Estas direcciones de VRAM (`$C000`/`$E000` para los planos, comandos de escritura con el patrón `$4xxxxxxx`/`$Cxxxxxxx`) coinciden con las ya vistas en el "Hello World" con bitmap de esta misma serie (ver referencia cruzada).

## Notas / gotchas

- El formato descrito aquí es la contraparte de descompresión de la variante "LZ4 sin framing estándar" ya documentada en `../graficos/bumbershoot-refinando-graficos-logo.md` (donde se comprimió el logo del proyecto CCA de 16.000 a 11.437 bytes); no es LZ4 estándar y **no** se puede descomprimir con un `lz4 -d` genérico — el decodificador tiene que ser este mismo código a medida.
- El código asume que el buffer de descompresión en RAM tiene sitio suficiente; el propio artículo de compresión citaba una necesidad de "un pelín más de 30 KB" de RAM para el caso del logo.
- Gotcha de portabilidad entre ensambladores: si se reutiliza este listado con un ensamblador distinto de ASMX (p. ej. Vasm o ASMotor, ver comparativa de toolchains), probablemente `SUB.L A3,A1` sí ensamble sin problema por alias automático a `SUBA`; con ASMX hay que escribirlo explícitamente como `SUBA.L`.
- El truco de reconstruir un word desalineado byte a byte con `lsl.w`/`or.w` es reutilizable en cualquier formato de datos que, como LZ4, no garantice alineación de sus campos multi-byte al portarlo al 68000.

## Referencias cruzadas

- [Bumbershoot Software — pipeline de cuantización y compresión para un logo en Genesis](../graficos/bumbershoot-refinando-graficos-logo.md) — el lado de compresión (PC) de este mismo formato LZ4 casero; esta entrada documenta el lado de descompresión (68000).
- [Formatos de compresión clásicos de Sega — Nemesis, Kosinski, Enigma](compresion-nemesis-kosinski-enigma.md) — otros formatos de compresión usados históricamente en Mega Drive, con objetivos distintos (velocidad de descompresión a VRAM vs. tasa de compresión).
- [Formato SLZ](format-slz.md), [Formato UFTC](format-uftc.md) — otros formatos de compresión ya documentados en esta carpeta, de origen homebrew moderno (plutiedev.com).
- [Bumbershoot Software — VDP desde cero: "Hello World" y bitmap con ASM puro](../graficos/bumbershoot-hello-world-y-bitmap-vdp.md) — mismas direcciones de VRAM/comandos de escritura reutilizados aquí para volcar los datos ya descomprimidos.
