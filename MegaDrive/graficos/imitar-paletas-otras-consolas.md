---
titulo: Imitar la paleta de otras consolas en Mega Drive (Game Boy, Virtual Boy, NES…)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [paletas, cuantizacion, game-boy, virtual-boy, nes, snes, estilo-retro, cram]
estado: borrador
fecha: 2026-10-15
fuentes: [https://www.copetti.org/writings/consoles/game-boy/, https://www.copetti.org/writings/consoles/virtual-boy/, https://www.copetti.org/writings/consoles/nes/, https://www.copetti.org/writings/consoles/super-nintendo/, https://www.copetti.org/writings/consoles/mega-drive-genesis/]
---

# Imitar la paleta de otras consolas en Mega Drive

## Resumen

La CRAM de la Mega Drive guarda colores de 9 bits (3 bits por canal, 512 colores), así que los colores de otras consolas no se pueden reproducir exactos: se ajustan al nivel más cercano de cada canal. A veces dos colores originales caen en el mismo color de MD y hay que elegir el siguiente más cercano para mantenerlos distintos.

## Detalle técnico

**Niveles por canal** (los que usan las plantillas de MD Engine): `00 34 57 74 90 AC CE FF`. Para cuantizar, cada canal R, G y B se ajusta al nivel más cercano.

**Game Boy (DMG), 4 verdes:**

| Original | Ajuste directo | Usado | Nota |
|---|---|---|---|
| `0F380F` | `003400` | `003400` | |
| `306230` | `345734` | `345734` | |
| `8BAC0F` | `90AC00` | `90AC00` | |
| `9BBC0F` | `90AC00` | **`90CE00`** | colisiona con el anterior; el siguiente más cercano en distancia RGB² es `90CE00` (670), por delante de `ACAC00` (770) |

**Otras referencias de hardware** (Copetti):

| Consola | Color | Límites a imitar |
|---|---|---|
| Game Boy | 4 tonos; sprites de 3 tonos + transparente | 160×144; 10 sprites por línea, 40 por fotograma; 4 canales de sonido (2 pulsos, onda, ruido) → PSG de la MD |
| Virtual Boy | Monocromo rojo: 2 rojos definibles + 1 derivado + transparente; 8 paletas | 384×224; profundidad por paralaje estereoscópico; 5 canales de wavetable + ruido |
| NES | Paleta maestra de 64; paletas de 3 colores + transparente (4 de fondo y 4 de sprites) | 256×240 (usar H32); 8 sprites por línea, 64 en total; 2 pulsos + triangular + ruido + DPCM |
| SNES | Paletas de 256 entradas (CGRAM) | 256×224; 128 sprites (32 por línea); Mode 7; 8 canales ADPCM |
| Mega Drive | 4 paletas × 16 de 512 | 320×224 / 256×224; 20 sprites por línea, 80 por pantalla; YM2612 (6 FM) + PSG (3 cuadradas + ruido) |

**Técnica de "re-skin" por zona:** dibujar todos los sprites con una convención fija de índices (0 transparente, 1 contorno, 2 detalle, 3 medio, 15 cuerpo) y generar un PNG por zona que solo cambie la PLTE. El mismo arte sirve para Pong (blanco y negro), Game Boy (4 verdes) o Virtual Boy (rojos).

## Notas / gotchas

- Los fundidos estándar (`FADE_OUT/IN`) pasan por negro: rompen una zona Game Boy estricta, donde no hay negro puro. Usa cortes directos o fundidos entre los tonos de la propia paleta.
- La fuente por defecto de MD Engine parece usar el índice 15 de la paleta de texto (todas las plantillas ponen blanco ahí; **por verificar**). En una zona de paleta restringida conviene que **los 16 índices** sean colores de la zona, para que el texto no se salga de la paleta use el índice que use.
- El cuerpo del héroe en el tono más claro de Game Boy coincide con el fondo más claro: el contorno (índice 1) es lo que lo hace legible.

## Referencias cruzadas

- [tiles-and-palettes.md](tiles-and-palettes.md) · [vdp-color-ramp.md](vdp-color-ramp.md) · [shadow-highlight.md](shadow-highlight.md)
- [../herramientas/md-engine/overview.md](../herramientas/md-engine/overview.md)
