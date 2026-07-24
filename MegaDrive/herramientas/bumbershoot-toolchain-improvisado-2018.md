---
titulo: Bumbershoot Software — el toolchain improvisado de 2018 para Genesis (ASMX + conversión manual)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [bumbershoot, asmx, dgen, gens-kmod, objcopy, imagemagick, s-record, toolchain, build]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/03/04/an-improvised-sega-genesis-toolchain/]
---

# Bumbershoot Software — el toolchain improvisado de 2018 para Genesis (ASMX + conversión manual)

## Resumen

Continuación directa de la primera entrada de la serie (ver `bumbershoot-preparando-entorno-genesis.md`, donde el autor elige ASMX como ensamblador): este post de marzo de 2018 detalla el pipeline concreto —bastante artesanal— que montó para pasar de fuentes `.s` a una ROM cargable, incluyendo varios scripts propios para compensar carencias de ASMX en aquel momento (sin `INCBIN`, sin salida binaria directa).

## Detalle técnico

### Limitaciones de ASMX que obligan a improvisar

En 2018 ASMX no tenía directiva `INCBIN` (incluir un fichero binario tal cual) ni podía volcar directamente un binario plano: solo generaba **Intel Hex** o **Motorola S-Record**. Además, la salida S-Record en formato S28 tenía un bug para ROMs de más de 64 KB (recomendación del propio ecosistema: usar S38 o Intel Hex en su lugar para ROMs grandes).

Nota de nomenclatura: GameHut usa en sus vídeos un ensamblador que también se llama "asm68k", pero es el de Psygnosis (Psy-Q devkit, distribuido vía sonicretro.org) — un dialecto distinto al ASMX de esta serie, no confundir.

### Truco para incluir binarios (gráficos, datos, Z80) sin INCBIN

Como ASMX no podía tragarse un binario directamente, el autor convierte cualquier blob a una lista de directivas `HEX` de texto, que sí es válida como fuente ASMX. Dos formas de hacerlo:

- Con una utilidad propia en C, `bin2asmx.c`, pensada para integrarse en el flujo de build.
- Con un one-liner equivalente usando herramientas Unix estándar:

```
od -An -t x1 -v | sed 's/^/        HEX     /' < blob.bin > blob.s
```

(`od -An` oculta la columna de direcciones, `-t x1` vuelca en bytes hexadecimales, `-v` desactiva el resumen de líneas repetidas con `*`; `sed` antepone la directiva `HEX` con la indentación esperada por ASMX).

Directivas ASMX relevantes usadas en el flujo:

```
dc.b    $01,$02,$ca,$fe
dc.l    $0102cafe
hex     01 02 ca fe
```

### De la salida de ASMX a una ROM cargable

1. ASMX ensambla a S-Record (p. ej. `rom.s19`).
2. Se convierte a binario plano con `objcopy`:

```
objcopy -I srec -O binary --gap-fill=0xff --pad-to=0x8000 rom.s19 rom.bin
```

3. Un script propio en Python, `srec2smd.py`, se encarga de lo que `objcopy` por sí solo no resuelve: expandir/rellenar el binario al tamaño esperado, calcular el checksum de cabecera y escribir el campo de tamaño de ROM en los metadatos Sega — produciendo un `.bin`/`.smd` ya cargable en emulador.

### Gráficos: de PNG a formato VDP

El logo de arranque se prepara reduciendo color con ImageMagick:

```
convert logo.png -colors 65536 -resize 200x200 -colors 16 logo.xpm
```

(reduce primero a 65536 colores, redimensiona, y luego cuantiza a 16 colores, exportando a XPM para poder editar píxel a píxel). Un script propio, `logo.py`, convierte después ese XPM a los formatos de tiles/paleta que espera el VDP.

### Entorno y emuladores de prueba

- Desarrollo en Fedora Linux; `gcc` para las utilidades en C (`bin2asmx.c`) y Python 2 para los scripts (`srec2smd.py`, `logo.py`).
- **DGen** — el mejor depurador de los dos, aunque en Fedora requiere instalar `wine`, `SDL-devel`, `SDL_image-devel` y `mesa-libGL-devel` para compilarlo.
- **Gens KGen mod** — mejor visualizador (VRAM, paletas) que DGen, usado en paralelo.
- El código de ejemplo de esta y las entradas anteriores de la serie se publicó en `https://github.com/michaelcmartin/bumbershoot/tree/master/genesis`.

## Notas / gotchas

- La reducción de color de ImageMagick introduce dithering que difumina/corrompe líneas nítidas del arte original — el autor corrige a mano a nivel de píxel en el XPM antes de convertir, no es un proceso 100% automático.
- Este pipeline de 2018 (ASMX 1.8.2 + `od`/`sed` + `objcopy` + `srec2smd.py`) queda completamente reescrito 7 años después; ver `bumbershoot-modernizando-toolchain-asmx-2025.md` para el toolchain sustituto (ASMX 2.0+, `INCBIN` nativo, `smdfix`) y qué problema concreto de cada pieza de este flujo motivó el cambio.
- Repárese en que el propio autor ya había elegido ASMX por motivos de estilo (ver `bumbershoot-preparando-entorno-genesis.md`); esta entrada no reabre esa decisión, solo documenta la mecánica de build que construyó alrededor de ella.

## Referencias cruzadas

- [Bumbershoot Software — panorama de hardware y elección de herramientas para Genesis/Mega Drive](bumbershoot-preparando-entorno-genesis.md) — la entrada previa de la misma serie donde se elige ASMX; esta la complementa con el pipeline de build concreto.
- [Bumbershoot Software — modernizando el toolchain de ASMX en 2025](bumbershoot-modernizando-toolchain-asmx-2025.md) — revisión de este mismo toolchain siete años después.
- [Build y debug de ASM puro para Mega Drive (sin SGDK)](build-debug-asm-puro.md) — comparativa moderna de ensambladores y depuradores (incluye BlastEm, ausente en esta serie de 2018).
- [Comparativa de toolchains de ensamblador para Mega Drive](bumbershoot-comparativa-toolchains.md) — panorama general donde ASMX aparece descrito como "primitivo en algunos aspectos", justo la carencia que este pipeline de 2018 rodea con scripts propios.
