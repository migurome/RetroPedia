---
titulo: Ten Key Pad (Mega Anser)
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [ten-key-pad, mega-anser, teclado, japon, io]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/ten-key-pad]
---

# Ten Key Pad (Mega Anser)

## Resumen

El Ten Key Pad es el periférico de entrada principal para el Mega Anser, e incluye un teclado tipo teléfono, flechas y botones de comando. Fue lanzado únicamente en Japón, con etiquetas en japonés.

## Detalle técnico

### Configuración inicial

Para configurar el teclado, se escribe `$60` en los puertos de control y datos:

```
; a0 = IoData1 o IoData2
FastPauseZ80
move.b  #$60, 6(a0)  ; Control port
move.b  #$60, (a0)   ; Data port
ResumeZ80
```

### Lectura del teclado

Se ejecuta el siguiente procedimiento 10 veces:

1. Escribir `$20` en el puerto de datos y esperar.
2. Leer los bits 3-0 (nibble inferior).
3. Escribir `$00` en el puerto de datos y esperar.
4. Leer los bits 3-0 (nibble superior).
5. Escribir `$60` al finalizar.

```
lea     (Buffer), a1
moveq   #10-1, d0
@Loop:
FastPauseZ80
move.b  #$20, (a0)
nop / nop / nop
move.b  (a0), d1
move.b  #$00, (a0)
nop / nop / nop
move.b  (a0), d2
ResumeZ80
and.b   #$0F, d1
lsl.b   #4, d2
or.b    d2, d1
move.b  d1, (a1)+
dbf     d0, @Loop
FastPauseZ80
move.b  #$60, (a0)
ResumeZ80
```

### Mapeo de bits (10 bytes)

| Byte | Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|------|-------|-------|-------|-------|-------|-------|-------|-------|
| 1 | d | c | — | — | — | — | — | — |
| 2 | — | — | — | — | — | — | — | — |
| 3 | b | — | x | — | — | — | — | — |
| 4 | r | y | — | s | — | — | — | — |
| 5 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| 6 | # | — | — | — | — | — | 9 | 8 |
| 7 | — | — | — | — | n | — | — | * |
| 8 | — | — | — | — | — | U | R | — |
| 9 | — | — | — | — | — | — | — | — |
| 10 | D | — | — | — | — | — | — | L |

### Nomenclatura de teclas especiales

- **n** (次頁): Next page (página siguiente)
- **b** (前頁): Previous page (página anterior)
- **r** (実行): Execute (ejecutar)
- **x** (漢字変換): Kanji conversion (conversión de kanji)
- **y** (カナ漢字): Kana/Kanji
- **c** (キャンセル): Cancel (cancelar)
- **d** (削除): Delete (borrar)
- **s** (通信終了): Disconnect (desconectar)

## Notas / gotchas

- El autor advierte incertidumbre en la verificación de las teclas `x` e `y` sin acceso físico al dispositivo.

## Referencias cruzadas

- (pendiente de enlazar)
