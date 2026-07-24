---
titulo: Notas de hardware de Kabuto (VDP en profundidad)
plataforma: [MegaDrive]
categoria: [memoria]
tags: [vdp, timing, dma, ym2612, debug-register, overdrive2]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/mirror/kabuto-hardware-notes (mirror alojado por plutiedev.com de un documento de terceros; autor original: Kabuto/TiTAN, versión 1.5, escrito durante el desarrollo de la demo Overdrive 2)]
---

# Notas de hardware de Kabuto (VDP en profundidad)

## Resumen

Este documento es un mirror alojado en plutiedev.com de unas notas técnicas de terceros escritas por Kabuto (grupo TiTAN) durante el desarrollo de la demo Overdrive 2, originalmente publicadas como un Google Doc (versión 1.5, 2018-05-25). Cubre en profundidad el timing del bus, el VDP, el YM2612 y un registro de depuración no documentado (`$C0001C`) de la Mega Drive. Al ser una fuente de terceros con contenido muy técnico y de bajo nivel, se preserva casi literalmente.

## Detalle técnico

### Sistema de bus

El Z80 experimenta retrasos de 2-5 ciclos al acceder a la ROM, con un retraso medio de alrededor de 3.3 ciclos de Z80. El 68k sufre un retraso medio de aproximadamente 11 ciclos. Un mecanismo misterioso "roba" 2 de cada 128 ciclos del rendimiento del 68k, haciendo que funcione a aproximadamente 480 ciclos por línea de rasterizado en lugar de los 488.57 ciclos teóricos. Esta ralentización no afecta a las escrituras del VDP y podría estar relacionada con un legado del refresco de DRAM.

### Chip de audio YM2612

Los registros $30-$FF solo existen en un registro de desplazamiento (shift register) rotatorio, lo que limita la velocidad de escritura a "½ de la duración de un ciclo de YM2612, o 33.6 ciclos de Z80" como máximo. El timing del muestreo PCM difiere entre el hardware MD1 y MD2. El MD1 produce un audio más limpio (~3dB mejor relación señal-ruido) comparado con la respuesta apagada y distorsionada del MD2, debido a un filtrado de segundo orden y no linealidades del amplificador.

### Dimensiones de imagen

PAL: 38 borde + 224 imagen + 32 borde + 19 sync/blank = 313 líneas totales
NTSC: 11 borde + 224 imagen + 8 borde + 19 sync/blank = 262 líneas totales

El modo H40 añade 13 píxeles de borde a la izquierda + 14 a la derecha. El VDP reduce la velocidad del reloj en 32 ciclos de reloj de píxel durante el HBlank.

### Comportamiento del puerto de comandos del VDP

"El buffer de dirección se incrementa después de cada lectura o escritura de VRAM/VSRAM/CRAM (sumando el valor del registro $f)."

### Fases del renderizado de sprites

**Fase 1:** comprueba hasta 80 sprites a lo largo de 40 ciclos (2 por ciclo), identificando los primeros 20 sprites visibles por posición Y/tamaño.

**Fase 2:** obtiene la posición X, la información de tile de los sprites visibles; calcula las direcciones de fila de sprite en una caché indexada de 20 slots (34 bits por entrada).

**Fase 3:** durante el HBlank, itera la caché de sprites y obtiene entre 1 y 4 tiles por sprite en un line buffer de 320 píxeles. Bug de renderizado: los sprites se vuelven invisibles cuando X=0 sigue a X≠0; el renderizado se reactiva al inicio de la siguiente línea de rasterizado.

**Fase 4:** el buffer de sprites se envía como plano de prioridad durante la visualización activa.

### Patrón de acceso a memoria (por línea de rasterizado)

H40: 210 accesos (uno cada 2 ciclos)
H32: 171 accesos

Notación del patrón:

- H = Hscroll, AB = tiles de plano, ab = píxeles de plano, S = info de sprite, s = píxeles de sprite, ~ = slot del 68k, r = refresco

Secuencia en H40: `Hssss AsaaBsbb ((A~aaBSbb)*3 AraaBSbb)*5 ~~ s*23 ~ s*11`

### Efectos de desactivar la pantalla (display disable)

Cuando se limpia el bit 6 del registro 1: se activa el borde, se desactiva la salida de sprites, los tiles muestran datos del bus, el refresco continúa, los ciclos de sprite faltantes desactivan a los sprites siguientes.

### Trucos de temporización del modo H40

Registro $81 = H40 estándar (un píxel cada 8 ciclos con ralentización en HBlank)
Registro $01 = H40 rápido (3360 relojes maestros, subreloj del 2%)
Registro $80 = H32 rápido (2736 relojes maestros, subreloj del 20% — incompatible con TV)
Registro $00 = H32 estándar (un píxel cada 10 ciclos)

Alternar rápidamente el bit provoca doblado de líneas, "crunching" de líneas, corrupción gráfica y glitches de sprites.

### Mapeo de direcciones de VRAM

**Modo 64K (estándar):**
Bits de columna: `2 3 4 5 6 7 8 9 | 0 1 A B C D E F`
(el bit 0 se invierte para emular el orden de bytes)

**Modo 128K:**
Bits de columna: `2 3 4 5 6 7 8 9 | 1 A B C D E F G`

**Modo SMS:**
Bits de columna: `1 2 3 4 5 6 7 8 | 0 9 A B C D E F`

### DMA byte-a-byte en modo 128K

Permite escribir bytes individuales de VRAM en lugar de palabras de 16 bits. Fórmula de dirección destino:

`(((a & 2) >> 1) ^ 1) | ((a & $400) >> 9) | a & $3FC | ((a & $1F800) >> 1)`

Solo se escribe el byte bajo; el byte alto se pierde. Genesis 3 usa una fórmula más simple: `a^1`.

### Velocidad de DMA

Transferencias rápidas (CRAM, VSRAM, VRAM en 128K): `words*2.4+5.6` ciclos
Transferencias lentas (VRAM en 64K): `max(words*2.4+5.6, words*4.7-6)` ciclos

### Operación de sombra/realce (Shadow/Highlight, S/H)

Los colores de sprite 62-63 controlan la semitransparencia: el color 63 oscurece los píxeles subyacentes; el color 62 los aclara. El color de sprite 14 (en las paletas 0-2) crea un estado normal no intencionado en ciertas condiciones. Cuando ambos planos tienen prioridad baja se dispara la sombra; los sprites de prioridad alta evitan el oscurecimiento.

### Registro de depuración ($C0001C)

**Funciones de los bits:**

- Bit 1: efectos de hardware (impide el arranque de Overdrive 1)
- Bit 3: provoca cuelgues de DMA, cuelgues de VSRAM
- Bit 5: distorsiona el direccionamiento (patrón repetitivo de 256 bytes)
- Bit 6: desactiva la pantalla (solo planos, sin DMA rápida)
- Bits 7-8: fuerza la visibilidad de un único plano (01=sprites, 10=A, 11=B)
- Bits 9-11: modo de sustitución de volumen del PSG

**Bits 12-14 (bloqueos de buffer):**

- Bit 12: bloquea la caché de atributos de sprite (buffer de 20 slots)
- Bit 13: bloquea el line buffer; impide escrituras de fase 3 o lecturas de fase 4
- Bit 14: bloquea el registro de desplazamiento de sprites de fase 1

### Modos de visualización de depuración (bits 8,7,6)

`000`: normal
`001`: solo fondo
`010`: sprites enmascarados (con AND)
`011`: solo sprites
`100`: plano A enmascarado
`101`: solo plano A
`110`: plano B enmascarado
`111`: solo plano B

### Modo 4 (compatibilidad SMS)

Habilita el acceso de palabra de 16 bits (a diferencia de los 8 bits reales de la SMS). El plano B está disponible pero es defectuoso. El H40 roto muestra basura en los 8 tiles más a la derecha; los sprites 1-2 muestran artefactos de duplicación de píxeles.

El plano B roto interpreta datos desde direcciones con offset +$200 usando decodificación estilo modo 5; efectivamente crea tiles de 16×8 píxeles con huecos.

### Glitches de escritura en el registro de depuración

El muestreo ocurre demasiado pronto; el VDP obtiene brevemente datos obsoletos del bus antes del valor pretendido, causando:

- corrupción de VRAM (glitches de direccionamiento del bit 5 confunden la selección de fila de la DRAM)
- destrucción del buffer de sprites (se interrumpen las señales del line buffer)
- corrupción de lectura (glitches de dirección durante la obtención de datos)
- glitches de píxel aislados (artefactos tipo "CRAM-dot")

### Inestabilidad del enmascarado de planos

Los chips VDP 315-5313 originales (especialmente los fabricados por Hitachi, VA4 y anteriores) muestran "mezcla en lugar de lógica AND apropiada" cuando se fuerzan varios planos activos simultáneamente. Esto resulta en ruido, metaestabilidad y corrupción de paleta. Afecta a ~30% de los sistemas Model 1; muy pocos sistemas Model 2.

"Mezclar 0 y 1 no da un 0 limpio sino a menudo algo intermedio", filtrando bits de dirección de VRAM hacia la etapa de lookup de paleta, potencialmente escribiendo entradas de paleta combinadas de vuelta a la CRAM.

## Notas / gotchas

1. El timing de retraso del Z80 asume acceso a ROM; el comportamiento con acceso a RAM es diferente.
2. Fijar el bit 16 para la tabla de sprites sin que coincida el bit 16 del objetivo de escritura provoca desincronización entre la caché y la memoria.
3. Cambiar de modo 64K a 128K con datos de FIFO sin escribir provoca comportamiento errático.
4. Las lecturas de VRAM por DMA pueden colgar el sistema bajo ciertas condiciones.
5. Desactivar la pantalla a mitad de línea de rasterizado rompe de forma fiable tanto los tiles como los sprites.
6. Alternar rápidamente los bits H40/H32 causa inestabilidades de temporización.
7. Cambiar el registro de depuración a mitad de línea de rasterizado corrompe el line buffer con alta probabilidad.

## Referencias cruzadas

- (pendiente de enlazar)
