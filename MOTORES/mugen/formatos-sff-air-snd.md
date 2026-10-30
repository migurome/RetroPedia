---
titulo: Formatos de MUGEN — SFF (v1/v2), AIR y SND
plataforma: [General]
categoria: [formatos, graficos, sonido]
tags: [mugen, sff, pcx, rle8, lz5, air, snd, act, paletas, sprites, ikemen]
estado: revisado
fecha: 2026-10-30
fuentes: [implementación de Ikemen GO (decodificador LZ5), documentación de formatos de Elecbyte, proyecto hyperdbz-dreamcast-port (tools/mugen/*.py, probado con los 22 personajes de Hyper DBZ 6.0c)]
---

# Formatos de MUGEN — SFF, AIR y SND

## Resumen

Un personaje de MUGEN es un `.def` que apunta a sprites (`.sff`), animaciones (`.air`), sonidos (`.snd`), comandos (`.cmd`), estados (`.cns`) y paletas (`.act`). Esta entrada describe los tres formatos binarios o de datos, comprobados leyendo los 22 personajes y 47 escenarios de Hyper DBZ: Wicked Ambition 6.0c. La lógica (CNS) está en [compilar CNS](cns-compilacion.md).

## Detalle técnico

### SFF v2 (MUGEN 1.0+)

Firma `ElecbyteSpr\0`; el byte 15 vale 2 (versión 2.x). Todo en little endian.

| Offset | Campo |
|---|---|
| 36 | offset de la lista de sprites |
| 40 | número de sprites |
| 44 | offset de la lista de paletas |
| 48 | número de paletas |
| 52 / 56 | offset / tamaño de *ldata* |
| 60 / 64 | offset / tamaño de *tdata* |

- **Sprite** (28 bytes): grupo u16, número u16, ancho u16, alto u16, eje x s16, eje y s16, enlace u16, formato u8, profundidad u8, offset u32, tamaño u32, índice de paleta u16 y flags u16. Si el tamaño es 0, es un **enlace** a otro sprite. Si `flags & 1`, los datos están en *tdata*; si no, en *ldata*.
- **Paleta** (16 bytes): grupo, número, nº de colores, enlace, offset y tamaño. Los colores son RGBA de 4 bytes.
- Formatos: 0 raw, 2 RLE8, 3 RLE5, 4 LZ5, 10–12 PNG. Los datos RLE/LZ5 empiezan con **4 bytes del tamaño descomprimido**.
- **RLE8:** si `(b & 0xC0) == 0x40`, repetir el byte siguiente `b & 0x3F` veces; si no, `b` es un píxel.
- **LZ5:** un byte de control da 8 flags (empezando por el bit bajo). Flag 0 = RLE: color `d & 0x1F` repetido `d >> 5` veces, y si ese contador es 0, el siguiente byte + 8. Flag 1 = LZ: si `d & 0x3F == 0`, LZ largo con offset `((d << 2) | siguiente) + 1` y longitud `siguiente + 3`; si no, LZ corto con longitud `(d & 0x3F) + 1`, y los 2 bits altos se van acumulando: cada 4 paquetes cortos forman un byte de offset, y mientras tanto se lee un byte de offset aparte.
- El índice 0 siempre es transparente.

### SFF v1

Firma igual con versión 1. En el offset 20: nº de imágenes y offset del primer subarchivo. Cada subarchivo (cabecera de 32 bytes) da: siguiente offset, tamaño, eje x/y, grupo, número, enlace y "misma paleta que el anterior", y después un **PCX** de 8 bits RLE con la paleta de 768 bytes al final (precedida de `0x0C`). Los escenarios de Hyper DBZ todavía usan v1.

### Paletas .act

768 bytes, 256 colores RGB **en orden inverso** (el color 0 es la última entrada). En un SFF v2 de personaje, la paleta elegida (`pal1`...`pal12` del `.def`) sustituye a la paleta del grupo 1,1.

### AIR

Texto. `[Begin Action N]` y luego frames `grupo, número, x, y, tiempo[, flip][, trans]`:
- tiempo -1 = infinito;
- flip `H`, `V` o `HV`;
- trans `A` (suma), `A1` (suma a la mitad), `S` (resta) o `AS<n>D<m>` (alfa).
- `Loopstart` marca el frame de vuelta del bucle.
- Cajas: `Clsn2Default: n` (valen para todos los frames siguientes), `Clsn1: n` (solo para el siguiente frame), y después `Clsn1[i] = x1, y1, x2, y2`. Clsn1 son las de ataque y Clsn2 las de cuerpo.

### SND

Firma `ElecbyteSnd\0`. En el offset 16: nº de sonidos y offset del primero. Cada subarchivo: siguiente offset u32, tamaño u32, grupo u32, número u32, y un **WAV completo** (RIFF) como dato.

## Notas / gotchas

- **Cifras reales (Hyper DBZ):** un personaje tiene 160–5.000 sprites. Hay sprites de efectos de hasta 2.000×2.000 casi transparentes: recortar al área opaca reduce mucho la memoria. Comprimido con zlib tras recortar, ocupa menos de la mitad que el SFF original (Goku: de 8,8 a 3,6 MB).
- Los SND de personaje llegan a 100 MB de WAV a 44/48 kHz, a menudo en estéreo y con varios idiomas.
- Usar los sprites de las animaciones (más el grupo 9000 de retratos) permite descartar un 10–25 % de sprites que no se usan.

## Referencias cruzadas

- [Compilar la lógica CNS de MUGEN](cns-compilacion.md)
- [Caché de sprites en VRAM (Dreamcast)](../../Dreamcast/graficos/cache-sprites-vram.md)
