---
titulo: Registros del PPU ($2100-$213F)
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, registros, referencia, vram, oam, cgram]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/PPU_registers]
---

# Registros del PPU ($2100-$213F)

## Resumen

Referencia completa de los 64 registros de hardware del PPU de la SNES, mapeados en el rango $2100-$213F del bus de la CPU. A través de ellos se controla todo el subsistema gráfico: brillo de pantalla, sprites (OBJ), fondos (BG1-4), scroll, acceso a VRAM/OAM/CGRAM, Modo 7, ventanas de recorte, color math y los registros de solo lectura de estado/contadores. Es la página de referencia central del PPU: el resto de entradas sobre gráficos de SNES remiten aquí para el detalle bit a bit de cada registro.

## Detalle técnico

### Tabla resumen de todos los registros

| Dirección | Nombre | Acceso | Función |
|---|---|---|---|
| $2100 | INIDISP | W8 | Forced blanking, brillo de pantalla |
| $2101 | OBJSEL | W8 | Tamaño de sprite, name select, dirección base |
| $2102-$2103 | OAMADDL/OAMADDH | W16 | Dirección de palabra en OAM |
| $2104 | OAMDATA | W8x2 | Escritura de datos en OAM |
| $2105 | BGMODE | W8 | Modo de BG, tamaño de tiles, prioridad de BG3 |
| $2106 | MOSAIC | W8 | Efecto de mosaico |
| $2107-$210A | BG1SC-BG4SC | W8 | Dirección y tamaño de tilemap por BG |
| $210B | BG12NBA | W8 | Dirección base de CHR de BG1/BG2 |
| $210C | BG34NBA | W8 | Dirección base de CHR de BG3/BG4 |
| $210D, $210F, $2111, $2113 | BGnHOFS | W8x2 | Scroll horizontal por BG |
| $210E, $2110, $2112, $2114 | BGnVOFS | W8x2 | Scroll vertical por BG |
| $2115 | VMAIN | W8 | Incremento y remapeo de dirección VRAM |
| $2116-$2117 | VMADDL/VMADDH | W16 | Dirección de palabra en VRAM |
| $2118-$2119 | VMDATAL/VMDATAH | W16 | Escritura de datos en VRAM |
| $211A | M7SEL | W8 | Ajustes de Modo 7 |
| $211B | M7A | W8x2 | Matriz A de Modo 7 / factor de multiplicación |
| $211C | M7B | W8x2 | Matriz B de Modo 7 / factor de multiplicación |
| $211D | M7C | W8x2 | Matriz C de Modo 7 |
| $211E | M7D | W8x2 | Matriz D de Modo 7 |
| $211F | M7X | W8x2 | Centro X de Modo 7 |
| $2120 | M7Y | W8x2 | Centro Y de Modo 7 |
| $2121 | CGADD | W8 | Dirección de palabra en CGRAM |
| $2122 | CGDATA | W8x2 | Escritura de color en CGRAM (RGB) |
| $2123 | W12SEL | W8 | Configuración de ventanas para BG1/BG2 |
| $2124 | W34SEL | W8 | Configuración de ventanas para BG3/BG4 |
| $2125 | WOBJSEL | W8 | Configuración de ventanas para OBJ/color |
| $2126 | WH0 | W8 | Posición izquierda de ventana 1 |
| $2127 | WH1 | W8 | Posición derecha de ventana 1 |
| $2128 | WH2 | W8 | Posición izquierda de ventana 2 |
| $2129 | WH3 | W8 | Posición derecha de ventana 2 |
| $212A | WBGLOG | W8 | Lógica de máscara de ventanas para BG |
| $212B | WOBJLOG | W8 | Lógica de máscara de ventanas para OBJ/color |
| $212C | TM | W8 | Capas activas en pantalla principal |
| $212D | TS | W8 | Capas activas en subpantalla |
| $212E | TMW | W8 | Ventanas activas en pantalla principal |
| $212F | TSW | W8 | Ventanas activas en subpantalla |
| $2130 | CGWSEL | W8 | Selección de ventana de color y color math |
| $2131 | CGADSUB | W8 | Designación de color math |
| $2132 | COLDATA | W8 | Color fijo para color math |
| $2133 | SETINI | W8 | Modo de pantalla / selección de vídeo |
| $2134-$2136 | MPYL/MPYM/MPYH | R24 | Resultado de multiplicación (Modo 7) |
| $2137 | SLHV | R8 | Latch de contadores H/V por software |
| $2138 | OAMDATAREAD | R8 | Lectura de datos de OAM |
| $2139-$213A | VMDATALREAD/VMDATAHREAD | R16 | Lectura de datos de VRAM |
| $213B | CGDATAREAD | R8x2 | Lectura de color de CGRAM |
| $213C | OPHCT | R8x2 | Contador horizontal |
| $213D | OPVCT | R8x2 | Contador vertical |
| $213E | STAT77 | R8 | Estado/versión de PPU1 |
| $213F | STAT78 | R8 | Estado/versión de PPU2 |

Leyenda de acceso: `W8` escritura de 8 bits; `W16` escritura de 16 bits (o dos de 8); `W8x2` requiere dos escrituras de 8 bits consecutivas; `R8`/`R16`/`R8x2` sus equivalentes de lectura; `R24` resultado de lectura de 24 bits.

### $2100 — INIDISP (Display control)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `F` | `0` | `0` | `0` | `B` | `B` | `B` | `B` |

- **`F`:** forced blanking. A 1, apaga la salida de vídeo (pantalla en negro) sin desactivar el acceso a VRAM/OAM/CGRAM.
- **`B3-0`:** brillo de pantalla, de `0` (apagado) a `$F` (brillo máximo).

### $2101 — OBJSEL (Configuración de objetos/sprites)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `S` | `S` | `S` | `N` | `N` | `b` | `B` | `B` |

- **`S2-0`:** selección de tamaño de sprite (par pequeño/grande, ver tabla siguiente).
- **`N1-0`:** name select — desplaza la segunda página de nombres de sprite en pasos de `(N+1) << 12` palabras.
- **`bBB`:** dirección base de la primera página de nombres de sprite (`base = bBB << 13` en palabras).

Combinaciones de tamaño de sprite (`S2-0`):

| Valor | Tamaño pequeño | Tamaño grande |
|---|---|---|
| 0 | 8×8 | 16×16 |
| 1 | 8×8 | 32×32 |
| 2 | 8×8 | 64×64 |
| 3 | 16×16 | 32×32 |
| 4 | 16×16 | 64×64 |
| 5 | 32×32 | 64×64 |
| 6 | 16×32 | 32×64 |
| 7 | 16×32 | 32×32 |

### $2102-$2103 — OAMADDL/OAMADDH (Dirección OAM)

`$2102` (byte bajo): dirección de palabra en OAM, bits 7-0.

`$2103` (byte alto):

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `P` | `0` | `0` | `0` | `0` | `0` | `0` | `B` |

- **`B`:** bit alto de la dirección de palabra OAM.
- **`P`:** activa la rotación de prioridad de sprites (obscure feature poco usada en homebrew estándar).

La CPU solo puede escribir en OAM durante VBlank o forced blanking; escribir durante el renderizado activo produce corrupción de la tabla.

### $2104 — OAMDATA (Escritura de datos OAM)

Registro de 8 bits, requiere dos escrituras consecutivas (word) para completar una entrada de la tabla baja de OAM. La dirección interna avanza automáticamente tras cada word completo, y también puede escribirse byte a byte para la tabla alta.

### $2105 — BGMODE (Modo de fondo y tamaño de tiles)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `4` | `3` | `2` | `1` | `P` | `M` | `M` | `M` |

- **`M2-0`:** modo de BG activo (0-7), determina profundidad de color y número de capas de cada BG (ver `backgrounds.md`).
- **`P`:** en el modo 1, activa prioridad alta para BG3 (lo coloca por encima de los sprites de menor prioridad).
- **`1`, `2`, `3`, `4`:** tamaño de carácter de BG1, BG2, BG3 y BG4 respectivamente (`0` = 8×8, `1` = 16×16).

Tabla de modos de fondo (profundidad por BG y orden de prioridad de frente a fondo; `S0-S3` son las cuatro subcapas de prioridad de sprites, `nH`/`nL` son la subcapa alta/baja del BGn):

| Modo | BG1 | BG2 | BG3 | BG4 | Offset-per-tile | Hi-res | Prioridad (frente→fondo) |
|---|---|---|---|---|---|---|---|
| 0 | 2bpp | 2bpp | 2bpp | 2bpp | No | — | S3 1H 2H S2 1L 2L S1 3H 4H S0 3L 4L |
| 1 | 4bpp | 4bpp | 2bpp | — | No | — | S3 1H 2H S2 1L 2L S1 3H S0 3L (bit `P`) |
| 2 | 4bpp | 4bpp | — | — | Sí | — | S3 1H S2 2H S1 1L S0 2L |
| 3 | 8bpp | 4bpp | — | — | No | — | S3 1H S2 2H S1 1L S0 2L |
| 4 | 8bpp | 2bpp | — | — | Sí | — | S3 1H S2 2H S1 1L S0 2L |
| 5 | 4bpp | 2bpp | — | — | No | Sí | S3 1H S2 2H S1 1L S0 2L |
| 6 | 4bpp | — | — | — | Sí | Sí | S3 1H S2 S1 1L S0 |
| 7 | 8bpp | (EXTBG) | — | — | No | — | S3 S2 2H S1 1L S0 2L |

En el modo 0, cada BG usa un bloque distinto de 32 colores de CGRAM (BG1: 0-31, BG2: 32-63, BG3: 64-95, BG4: 96-127). El modo 1 es el más habitual en juegos comerciales (dos capas de 16 colores más una auxiliar de 4). El modo 7 usa un tilemap único de 128×128 con transformación afín (rotación/escala) en vez del sistema estándar de BG1-4.

### $2106 — MOSAIC (Efecto de mosaico)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `S` | `S` | `S` | `S` | `4` | `3` | `2` | `1` |

- **`1-4`:** activa el mosaico en BG1, BG2, BG3 y BG4 respectivamente.
- **`S3-0`:** tamaño del bloque de mosaico, de `0` (1×1, sin efecto) a `$F` (bloques de 16×16 píxeles).

### $2107-$210A — BG1SC/BG2SC/BG3SC/BG4SC (Dirección y tamaño de tilemap)

Mismo formato para los cuatro registros:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `A` | `A` | `A` | `A` | `A` | `A` | `Y` | `X` |

- **`X`:** el tilemap se repite horizontalmente (0 = un solo tilemap de 32 columnas, 1 = dos tilemaps de 32 columnas colocados uno junto a otro).
- **`Y`:** el tilemap se repite verticalmente (mismo criterio en vertical).
- **`A5-0`:** dirección base del tilemap en VRAM, como dirección de palabra `AAAAAA << 10` (páginas de 2 KiB).

Los tilemaps pueden ubicarse en cualquier página de 2 KiB de VRAM; si el desplazamiento se sale del final de VRAM, la dirección da la vuelta (wrap).

### $210B — BG12NBA (Dirección base de caracteres BG1/BG2)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `B` | `B` | `B` | `B` | `A` | `A` | `A` | `A` |

- **`A3-0`:** dirección base de los tiles de BG1 (`AAAA << 12` en palabras).
- **`B3-0`:** dirección base de los tiles de BG2 (`BBBB << 12` en palabras).

### $210C — BG34NBA (Dirección base de caracteres BG3/BG4)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `D` | `D` | `D` | `D` | `C` | `C` | `C` | `C` |

- **`C3-0`:** dirección base de los tiles de BG3 (`CCCC << 12` en palabras).
- **`D3-0`:** dirección base de los tiles de BG4 (`DDDD << 12` en palabras).

### $210D/$210F/$2111/$2113 — BGnHOFS (Scroll horizontal) y $210E/$2110/$2112/$2114 — BGnVOFS (Scroll vertical)

Cada registro se escribe dos veces (byte bajo, luego byte alto) para formar un valor de 10-13 bits:

```
escritura 1 (byte bajo): XXXX XXXX
escritura 2 (byte alto): .... ..XX
```

El valor final es relativo a la esquina superior izquierda de la pantalla, incluso si se actualiza a mitad de frame mediante HDMA. `$210D`/`$210E` son compartidos por Modo 7 como `M7HOFS`/`M7VOFS` cuando BGMODE = 7 (ver más abajo).

### $2115 — VMAIN (Control de puerto VRAM)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `M` | `0` | `0` | `0` | `R` | `R` | `I` | `I` |

- **`I1-0`:** cantidad de autoincremento de dirección tras cada acceso (`00`=1, `01`=32, `10`=128, `11`=128).
- **`R1-0`:** modo de remapeo de dirección (para acelerar el acceso a tiles como si fueran una textura lineal):
  - `00`: sin remapeo
  - `01`: tiling 2bpp (`rrrrrrrr YYccccc` → `rrrrrrrr cccccYYY`)
  - `10`: tiling 4bpp (`rrrrrrrY YYcccccP` → `rrrrrrcc cccPYYY`)
  - `11`: tiling 8bpp (`rrrrrrYY YcccccPP` → `rrrrrrcc cccPPYYY`)
- **`M`:** momento del autoincremento — `0` tras escribir/leer `$2118`/`$2139` (byte bajo), `1` tras `$2119`/`$213A` (byte alto).

### $2116-$2117 — VMADDL/VMADDH (Dirección VRAM)

Dirección de palabra de 16 bits (bit 15 sin efecto — VRAM tiene 64 KiB de palabras direccionables). El acceso a VRAM solo es seguro durante VBlank o forced blanking; las escrituras durante HBlank o pantalla activa se ignoran.

### $2118-$2119 — VMDATAL/VMDATAH (Escritura de datos VRAM)

Escribir incrementa automáticamente `VMADD` según lo configurado en VMAIN. Igual que la dirección, las escrituras fuera de VBlank/forced blanking no tienen efecto.

### $2139-$213A — VMDATALREAD/VMDATAHREAD (Lectura de datos VRAM)

La lectura usa un buffer interno (`vram_latch`) que se carga inmediatamente al fijar la dirección con VMADD. Al leer varias posiciones consecutivas con autoincremento, hay que tener en cuenta que el valor disponible corresponde al leído previamente al latch, por lo que conviene descartar la primera lectura o ajustarla según el patrón de acceso.

### Registros de Modo 7 ($211A-$2120)

#### $211A — M7SEL (Ajustes de Modo 7)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `R` | `F` | `0` | `0` | `0` | `0` | `Y` | `X` |

- **`X`/`Y`:** activan el volteo horizontal/vertical de toda la pantalla de Modo 7.
- **`F`:** comportamiento fuera del área del tilemap — `0` transparente, `1` usa el carácter 0.
- **`R`:** repetición del tilemap — `0` repite el tilemap 128×128 infinitamente, `1` lo rellena según `F` fuera de sus límites.

#### $211B — M7A / #$211C — M7B / $211D — M7C / $211E — M7D (Matriz de transformación)

Cuatro registros de 16 bits (dos escrituras cada uno) en formato de punto fijo 8.8, que forman la matriz 2×2 de rotación/escala de Modo 7:

```
escritura 1 (byte bajo): dddd dddd
escritura 2 (byte alto): DDDD DDDD
```

Adicionalmente, `M7A` y `M7B` se reutilizan como entradas de un multiplicador con signo de 16×8 bits cuyo resultado aparece en `MPYL/MPYM/MPYH` ($2134-$2136).

#### $211F — M7X / $2120 — M7Y (Centro de rotación)

Dos registros de 13 bits con signo (dos escrituras cada uno) que fijan el punto de pantalla alrededor del cual pivota la transformación de Modo 7.

### $2121 — CGADD (Dirección CGRAM)

Registro de 8 bits: selecciona una de las 256 entradas de color de CGRAM. Al escribir CGADD, el contador interno de byte de CGDATA se reinicia a 0 (el siguiente byte escrito en `$2122` se interpreta como byte bajo).

### $2122 — CGDATA (Escritura de color CGRAM)

Color de 15 bits en formato BGR555, se completa con dos escrituras de 8 bits:

```
escritura 1 (byte bajo):  GGGR RRRR
escritura 2 (byte alto): .BBB BBGG
```

- **`R4-0`:** componente rojo (0-31)
- **`G4-0`:** componente verde (0-31)
- **`B4-0`:** componente azul (0-31)

Tras completar las dos escrituras, CGADD se incrementa automáticamente a la siguiente entrada. El acceso a CGRAM solo es válido durante VBlank, HBlank o forced blanking; escribir durante el renderizado activo apunta a una dirección incorrecta de CGRAM.

### $2123 — W12SEL (Ventanas para BG1/BG2)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `D` | `d` | `C` | `c` | `B` | `b` | `A` | `a` |

- **`a`/`A`:** habilita/invierte la ventana 1 sobre BG1.
- **`b`/`B`:** habilita/invierte la ventana 2 sobre BG1.
- **`c`/`C`:** habilita/invierte la ventana 1 sobre BG2.
- **`d`/`D`:** habilita/invierte la ventana 2 sobre BG2.

(Nota: minúscula = bit de habilitación, mayúscula = bit de inversión, agrupados en pares por capa; ver `windows.md` para el detalle de la lógica de ventanas.)

### $2124 — W34SEL (Ventanas para BG3/BG4)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `H` | `h` | `G` | `g` | `F` | `f` | `E` | `e` |

Mismo esquema que W12SEL, aplicado a BG3 (`e`/`E`, `f`/`F`) y BG4 (`g`/`G`, `h`/`H`).

### $2125 — WOBJSEL (Ventanas para OBJ y color)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `L` | `l` | `K` | `k` | `J` | `j` | `I` | `i` |

Mismo esquema aplicado a la capa OBJ (`i`/`I`, `j`/`J`) y a la ventana de color usada por color math (`k`/`K`, `l`/`L`).

### $2126-$2129 — WH0/WH1/WH2/WH3 (Posiciones de ventana)

| Registro | Dirección | Función |
|---|---|---|
| WH0 | $2126 | Borde izquierdo de la ventana 1 |
| WH1 | $2127 | Borde derecho de la ventana 1 |
| WH2 | $2128 | Borde izquierdo de la ventana 2 |
| WH3 | $2129 | Borde derecho de la ventana 2 |

Cada registro es un valor de 8 bits (coordenada de columna 0-255). Una ventana enmascara la región donde `izquierda <= X <= derecha`; si izquierda es mayor que derecha, la ventana no cubre ninguna columna.

### $212A — WBGLOG (Lógica de ventanas para BG)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `4` | `4` | `3` | `3` | `2` | `2` | `1` | `1` |

Dos bits por BG (BG1, BG2, BG3, BG4) que seleccionan cómo se combinan la ventana 1 y la ventana 2 sobre esa capa: `00`=OR, `01`=AND, `10`=XOR, `11`=XNOR.

### $212B — WOBJLOG (Lógica de ventanas para OBJ/color)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `0` | `C` | `C` | `O` | `O` |

- **`O1-0`:** lógica de combinación de ventanas para la capa OBJ (mismos códigos que arriba).
- **`C1-0`:** lógica de combinación de ventanas para la ventana de color.

### $212C — TM (Capas en pantalla principal)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `O` | `4` | `3` | `2` | `1` |

- **`1-4`:** activa BG1-BG4 en la pantalla principal.
- **`O`:** activa la capa de sprites (OBJ) en la pantalla principal.

### $212D — TS (Capas en subpantalla)

Mismo formato que TM, pero controla qué capas se dibujan en la subpantalla (usada como segunda fuente en color math).

### $212E — TMW (Ventanas sobre pantalla principal) y $212F — TSW (Ventanas sobre subpantalla)

Mismo formato que TM/TS: cada bit indica si la ventana correspondiente (según W12SEL/W34SEL/WOBJSEL) se aplica a esa capa en pantalla principal (TMW) o subpantalla (TSW).

### $2130 — CGWSEL (Ventana de color y color math)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `M` | `M` | `S` | `S` | `0` | `0` | `A` | `D` |

- **`D`:** activa el modo de color directo (bypass de CGRAM) para BG con 8bpp y Modo 7.
- **`A`:** fuente del sumando en color math — `0` color fijo (COLDATA), `1` subpantalla.
- **`S1-0`:** región de la subpantalla donde la ventana de color deja pasar el color math.
- **`M1-0`:** región de la pantalla principal donde la ventana de color fuerza negro.

Códigos de región (`M1-0`/`S1-0`): `00`=ninguna parte, `01`=fuera de la ventana, `10`=dentro de la ventana, `11`=toda la pantalla.

### $2131 — CGADSUB (Designación de color math)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `M` | `H` | `B` | `O` | `4` | `3` | `2` | `1` |

- **`1-4`:** activa color math sobre BG1-BG4.
- **`O`:** activa color math sobre sprites con paleta 4-7 (las paletas 0-3 de sprites quedan siempre opacas al color math).
- **`B`:** activa color math sobre el color de fondo (backdrop).
- **`H`:** activa "half color" (divide el resultado entre 2).
- **`M`:** operación — `0` suma, `1` resta.

### $2132 — COLDATA (Color fijo)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `B` | `G` | `R` | `C` | `C` | `C` | `C` | `C` |

- **`C4-0`:** valor de intensidad (0-31) a escribir.
- **`R`/`G`/`B`:** a qué canal(es) del color fijo se aplica ese valor (pueden combinarse varias escrituras para fijar los tres canales).

Ejemplos de secuencias de escritura: negro = una escritura `%111_00000`; blanco = una escritura `%111_11111`; azul oscuro = dos escrituras `%100_10010`, `%011_00000`.

### $2133 — SETINI (Modo de pantalla)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `E` | `X` | `0` | `0` | `H` | `O` | `i` | `I` |

- **`I`:** entrelazado de pantalla (produce salida 480i en dos campos).
- **`i`:** entrelazado de OBJ (para sprites de doble altura en modo entrelazado).
- **`O`:** modo overscan — activa las 239 líneas visibles en vez de 224 (reduce el VBlank en 16 líneas).
- **`H`:** modo de alta resolución — duplica la salida horizontal de 256 a 512 píxeles.
- **`X`:** activa EXTBG (permite usar BG2 partido en dos niveles de prioridad en modo 7 extendido).
- **`E`:** sincronismo externo.

En los modos 5 y 6, los BG se auto-entrelazan para duplicar la resolución vertical percibida.

### $2134-$2136 — MPYL/MPYM/MPYH (Resultado de multiplicación)

Resultado de 24 bits con signo de `M7A × M7B`, repartido en tres registros de lectura:

```
$2134 (byte bajo)   $2135 (byte medio)   $2136 (byte alto)
```

El resultado está disponible inmediatamente tras completar la escritura de M7A (16 bits) y M7B (8 bits efectivos). Si una interrupción o HDMA escribe sobre el scroll de BG1 o la matriz de Modo 7 entre medias, el valor interno se corrompe y MPY devuelve un resultado incorrecto.

### $2137 — SLHV (Latch de contadores H/V)

Registro de solo lectura sin datos útiles: el simple acto de leerlo activa el latch por software de los contadores horizontal/vertical (`counter_latch = 1`), cuyo valor puede leerse después en OPHCT/OPVCT.

### $2138 — OAMDATAREAD (Lectura de datos OAM)

Lee un byte de OAM en la dirección actual e incrementa automáticamente OAMADD. Solo es fiable durante VBlank o forced blanking.

### $213B — CGDATAREAD (Lectura de color CGRAM)

Mismo formato BGR555 que CGDATA, leído en dos accesos de 8 bits; el bit más alto (bit 15) no está implementado y devuelve un valor no fiable que debe ignorarse. Solo es fiable durante VBlank, HBlank o forced blanking.

### $213C — OPHCT (Contador horizontal) y $213D — OPVCT (Contador vertical)

Registros de 9 bits leídos en dos accesos de 8 bits, que reflejan el valor de los contadores de posición de haz en el momento del último latch (por lectura de SLHV o por la señal externa `/EXTLATCH`). Los bits altos no usados devuelven "open bus" de PPU2.

### $213E — STAT77 (Estado de PPU1)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `T` | `R` | `M` | `x` | `V` | `V` | `V` | `V` |

- **`V3-0`:** número de versión de PPU1.
- **`x`:** open bus de PPU1.
- **`M`:** modo maestro/esclavo (línea física del pin 25 de PPU1).
- **`R`:** range over — se activó el límite de 34 slivers de sprite por línea.
- **`T`:** time over — se activó el límite de 32 sprites por línea.

### $213F — STAT78 (Estado de PPU2)

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `F` | `L` | `x` | `M` | `V` | `V` | `V` | `V` |

- **`V3-0`:** número de versión de PPU2.
- **`M`:** región — `0` NTSC/60 Hz, `1` PAL/50 Hz (línea física del pin 30 de PPU2).
- **`x`:** open bus de PPU2.
- **`L`:** valor actual del latch de contadores.
- **`F`:** campo de entrelazado (par/impar) del frame actual.

Al leer STAT78 se reinician `counter_latch`, el byte interno de OPHCT y el de OPVCT. Como curiosidad de errata: Nintendo dejó de incrementar el número de versión a partir de 3 (los chips S-PPU2 revisión B/C y S-CPUN revisión A reportan todos versión 3).

## Notas / gotchas

- Bajar `INIDISP` (forced blanking) fuera del VBlank produce artefactos gráficos; los cambios de brillo tampoco son instantáneos (pueden tardar más de 72 píxeles en el PPU de un solo chip).
- El acceso a OAM (`$2102-$2104`, `$2138`) y a CGRAM (`$2121-$2122`, `$213B`) solo es seguro durante VBlank/HBlank/forced blanking; escribir en pantalla activa corrompe la dirección interna.
- Las escrituras a VRAM (`$2118-$2119`) fuera de VBlank o forced blanking se ignoran silenciosamente.
- En modo alta resolución/entrelazado (`SETINI`), el modo overscan reduce el VBlank efectivo en 16 líneas — hay que tenerlo en cuenta si el código de VBlank está muy ajustado en tiempo.
- MPYL/MPYM/MPYH puede devolver un resultado corrupto si HDMA o una interrupción escribe sobre el scroll de BG1 o la matriz de Modo 7 mientras se calcula la multiplicación.
- `$210D`/`$210E` cumplen doble función: son BG1HOFS/BG1VOFS en modos normales y M7HOFS/M7VOFS en Modo 7 — no existen registros de scroll separados para Modo 7.
- El bit alto de CGDATAREAD (`$213B`) no está implementado en hardware; su valor no debe usarse.

## Referencias cruzadas

- [Fondos y modos de BG](backgrounds.md)
- [Tilemaps](tilemaps.md)
- [Tiles y formatos de bitplane](tiles.md)
- [Sprites (OAM)](sprites.md)
- [Paletas y CGRAM](palettes.md)
- [Ventanas de recorte](windows.md)
- [Offset-per-tile](offset-per-tile.md)
- [Color math](color-math.md)
