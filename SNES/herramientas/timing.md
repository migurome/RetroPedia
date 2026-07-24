---
titulo: Timing general del sistema SNES
plataforma: [SNES]
categoria: [herramientas]
tags: [timing, ciclos, scanlines, ntsc, pal, vblank, reloj-maestro]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Timing]
---

# Timing general del sistema SNES

## Resumen

Referencia técnica sobre los tiempos del sistema SNES: frecuencia del reloj maestro (distinta en NTSC y PAL), duración de ciclos de CPU según la zona de memoria accedida, estructura de scanlines y de la imagen visible, particularidades del entrelazado, presupuesto de tiempo disponible durante el vblank, y el reloj interno del subsistema de audio. Es la referencia base para calcular cuánto trabajo (DMA, cómputo) cabe en cada frame.

## Detalle técnico

### Reloj maestro

- NTSC: 945/44 MHz ≈ 21.4773 MHz (6 veces la frecuencia de crominancia/chroma).
- PAL: 21.281370 MHz (4.8 veces chroma). Se sintetiza multiplicando por 6/5 un cristal de 17.734475 MHz (4 veces chroma).

### Duración de los ciclos de CPU

El 65816 ejecuta ciclos que duran 6, 8 o 12 periodos del reloj maestro, según la región de memoria accedida y la configuración de MEMSEL. Las velocidades habitualmente citadas son:

- 3.58 MHz — acceso a Fast-ROM (ciclos de 6 clocks)
- 2.68 MHz — acceso a Slow-ROM (ciclos de 8 clocks)
- 1.79 MHz — resto de memoria (ciclos de 12 clocks)

Duración de acceso por zona de memoria:

- Fast-ROM ($800000+): 6 clocks
- Slow-ROM: 8 clocks
- S-WRAM interna: 8 clocks
- Mayoría de registros MMIO: 6 clocks
- JOYSER0/JOYSER1: 12 clocks
- Ciclos internos (que no acceden a memoria): 6 clocks

### Estructura de la scanline

Cada scanline dura 1364 clocks del reloj maestro, equivalentes a 341 ciclos de dot (dot cycles). La CPU se pausa 40 clocks maestros a mitad de cada línea para el refresco de la DRAM, dejando 1324 clocks activos por línea. La región de imagen activa va desde el dot/clock 88 hasta el 1112 de la línea.

Reparto de scanlines por frame:

- Scanline 0: oculta/en blanco (fin del vblank / inicio del renderizado)
- Scanlines 1 a 224 o 239 (según overscan): renderizado de imagen visible
- Scanline 261 (NTSC) o 311 (PAL): última línea del vblank

### Scanlines cortas y largas (entrelazado)

En condiciones especiales de entrelazado aparecen líneas de duración distinta:

- **Scanline corta**: NTSC, entrelazado desactivado, field=1, V=240
- **Scanline larga**: PAL, entrelazado activado, field=1, V=311

Comparación de duraciones:

| Tipo | Clocks totales | Dots | Composición |
|------|----------------|------|-------------|
| Normal | 1364 | 340 | 336 dots de 4 clocks + 4 dots de 5 clocks |
| Larga | 1368 | 341 | 337 dots de 4 clocks + 4 dots de 5 clocks |
| Corta | 1360 | 340 | todos los dots de 4 clocks |

### Vertical blank (vblank)

El vblank comienza en V=225 o V=240 según la configuración de overscan, y termina tras la línea V=261 (NTSC) o V=311 (PAL).

Presupuesto de tiempo/bandwidth disponible durante el vblank, según altura de pantalla y región:

| Región | Altura | Líneas de vblank | Clocks de vblank | Bytes de DMA | Tiles 4bpp | Tiles 8bpp |
|--------|--------|-------------------|-------------------|---------------|------------|------------|
| NTSC | 224 | 37 | 48,988 | 6,123 | 191 | 95 |
| NTSC | 239 | 22 | 29,128 | 3,641 | 113 | 56 |
| PAL | 224 | 87 | 115,188 | 14,398 | 449 | 224 |
| PAL | 239 | 72 | 95,328 | 11,916 | 372 | 186 |

### Totales por frame

| Región | Scanlines | Clocks maestros totales | Clocks disponibles |
|--------|-----------|--------------------------|----------------------|
| NTSC | 262 | 357,368 | 346,888 |
| PAL | 312 | 425,568 | 413,088 |

### Reloj del subsistema de audio

El S-SMP tiene su propia fuente de reloj, independiente de la CPU principal:

- Reloj del S-DSP: 24.576 MHz (resonador cerámico)
- Reloj interno del S-DSP: 3.072 MHz (÷8)
- SPC-700: 1.024 MHz (÷24)
- Frecuencia de muestreo del DAC: 32000 Hz según especificación (÷(24×32)), aunque varía de consola a consola entre 32000 Hz y 32160 Hz

## Notas / gotchas

- La frecuencia real de muestreo de audio no es idéntica en todas las consolas físicas (32000–32160 Hz), algo a tener en cuenta si se sincroniza audio con precisión.
- Las scanlines cortas y largas solo aparecen bajo combinaciones concretas de región/entrelazado/field/línea — no son el caso general y hay que tenerlas en cuenta al calcular timing crítico ciclo a ciclo.
- El presupuesto de vblank cambia drásticamente entre NTSC y PAL (PAL tiene muchas más líneas de vblank disponibles), lo que afecta cuánta VRAM/CGRAM/OAM se puede actualizar por frame vía DMA.

## Referencias cruzadas

- [Herramientas recomendadas para desarrollo en SNES](tools.md)
- [Glosario SNES](glosario.md)
- (pendiente de enlazar)
