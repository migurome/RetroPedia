---
titulo: PPU de SNES para desarrolladores de NES
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, comparativa, nes, registros, vram, oam, dma]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/SNES_PPU_for_NES_developers]
---

# PPU de SNES para desarrolladores de NES

## Resumen

Guía de traducción conceptual para quien ya conoce la PPU de NES y quiere entender en qué se parece y en qué difiere radicalmente la PPU de SNES (dos chips, PPU1+PPU2). Recorre VRAM, paletas/CGRAM, DMA/HDMA, modos de fondo, resolución, tilemaps y sprites, y cierra con una tabla de equivalencias registro a registro entre ambas consolas.

## Detalle técnico

### VRAM

La PPU de SNES trae 64KB de VRAM interna y, a diferencia de NES, no admite ampliarla desde el cartucho. Otra diferencia clave de direccionamiento: las direcciones de VRAM son de 16 bits y se manejan como palabras (words), repartidas en dos registros (uno para el byte bajo y otro para el alto en cada dirección), lo que da acceso al rango `$0000`-`$7FFF` en unidades de palabra, en vez de acceder byte a byte como en NES.

### Paletas / CGRAM

SNES ofrece 256 colores RGB de 5 bits por canal (rojo, verde, azul). La paleta se organiza como 8 paletas de fondo y 8 paletas de sprites, cada una con 15 colores más un color de transparencia (los distintos modos de fondo matizan estos detalles). El acceso a VRAM y a CGRAM se hace por rutas de registros completamente separadas.

### DMA

El DMA de SNES es mucho más flexible que el de NES: puede escribir en *cualquier* registro de la PPU, así como en otros registros del mismo rango de direcciones. Las transferencias se hacen entre cartucho/RAM de CPU y VRAM, CGRAM, OAM o RAM de CPU. También soporta copiar en sentido inverso (de VRAM a RAM de CPU) y operaciones de relleno (fill). Para copiar RAM de CPU a RAM de CPU hace falta usar las instrucciones `MVN`/`MVP` de la propia CPU, no el DMA.

#### HDMA

El HDMA (Horizontal-Blanking DMA) transfiere pequeñas cantidades de datos durante cada scanline, en el hueco de HBlank. Se configura mediante una tabla que especifica los valores a escribir y cuántos scanlines mantener cada valor antes de pasar al siguiente, lo que evita tener que hacer timing preciso manualmente para conseguir efectos "raster". El soporte de IRQ por posición H/V sigue estando disponible, con la opción añadida de disparo por coordenada horizontal.

### Modos de fondo

SNES tiene varios modos que cambian cómo se comportan las capas de fondo. Por ejemplo, "el modo 1 ofrece dos capas con 16 colores por tile, y una tercera capa con 4 colores por tile"; esa tercera capa suele reservarse para HUD o barras de estado, dándole prioridad sobre el resto.

**Resolución de pantalla**: por defecto SNES genera una imagen de 240 líneas, con 8 filas de blanking arriba y abajo. El registro `$2133` (SETINI) permite activar un modo "overscan" que se acerca más al área visible de NES. La PPU siempre deja en blanco la primera línea de renderizado: en modo de 240 líneas se ven 239 líneas útiles, y en modo de 224 líneas se ven 224 líneas pero el renderizado empieza una línea más arriba y se muestra una línea extra al final.

**Scroll**: una posición de scroll Y = 0 *no* muestra la primera fila de píxeles de la capa de fondo, precisamente por esa línea en blanco inicial. Los cambios de scroll a mitad de pantalla siguen siendo relativos a la esquina superior izquierda del área activa (a diferencia de cómo se comporta NES). Además, cambiar entre modo de 224 y 239 líneas desplaza la versión de 224 líneas 8 líneas hacia abajo, sin que sea un simple recorte tipo letterbox.

### Tilemaps / nametables

Las entradas de tilemap de SNES son de 16 bits e incluyen selección de paleta, bit de prioridad, flips horizontal/vertical y número de tile, todo en la misma palabra: no existe una tabla de atributos separada como en NES, y un tilemap puede referenciar 1024 tiles distintos en vez de solo 256.

Los tilemaps miden 32x32 tiles (frente a 32x30 en NES). Las capas de fondo pueden usar un único tilemap, dos tilemaps en horizontal, dos en vertical, o cuatro formando un cuadrado 2x2. Los tiles son de 8x8 por defecto, pero también pueden configurarse a 16x16.

### Sprites / OBJ / OAM

**Límites**: hasta 128 sprites en total, con un máximo de 32 sprites visibles por scanline (los que se pasen de ese límite desaparecen), y además un límite adicional de 34 tiles de sprite por scanline.

**Tamaños**: 8x8, 16x16, 32x32 o 64x64. Se eligen dos tamaños disponibles a nivel de PPU, y cada sprite individual usa uno de esos dos. A diferencia de NES, **no existe el tamaño 8x16**.

**Estructura de datos**: parecida a la de NES (X, Y, tile, atributos) pero reordenada y con más propiedades:

- La coordenada X tiene un noveno bit, que permite posicionar parcialmente el sprite saliéndose por el borde izquierdo.
- 8 paletas de 15 colores disponibles para sprites (los tiles de sprite son siempre de 4bpp).
- 4 niveles de prioridad, que permiten colocar un sprite por encima, por debajo o entre distintas capas de fondo.

A la capa de renderizado de sprites se la llama "OBJ". El color math y las ventanas (windows) se aplican a OBJ igual que a las capas de fondo.

**Retraso vertical**: los sprites se retrasan verticalmente 1 scanline, igual que en NES, lo que permite reutilizar sin cambios el código de posicionamiento de scroll/sprites. En el modo por defecto de 224 líneas puede hacer falta subir sprites y fondo 8 líneas. A diferencia de NES, los sprites sí pueden aparecer en la primera línea visible, porque esa primera línea de renderizado real queda oculta.

### Tabla de equivalencias de registros (NES → SNES)

**PPUCTRL ($2000 escritura)** se reparte entre varios registros de SNES: BG1HOFS (`$210D`, escritura doble), BG1VOFS (`$210E`, escritura doble), VMAIN (`$2115`), OBSEL (`$2101`), BG12NBA (`$210B`), SETINI (`$2133`), NMITIMEN (`$4200`).

Desglose de bits de PPUCTRL y su equivalente conceptual en SNES:
- Bit 0: bit alto del scroll horizontal.
- Bit 1: bit alto del scroll vertical.
- Bit 2: modo de incremento de dirección VRAM.
- Bit 3: tabla base de patrones de sprites.
- Bit 4: tabla base de patrones de fondo.
- Bit 5: selección de tamaño de sprite.
- Bit 6: control de EXTBG.
- Bit 7: habilitación de NMI por VBlank.

Se apunta además que la Control Deck original configura este comportamiento para recibir los mismos datos de textura que lee el modo 7, dando control píxel a píxel de la prioridad entre fondo y sprites.

**PPUMASK ($2001 escritura)** se reparte entre TMW (`$212E`), TM (`$212C`), INIDISP (`$2100`), COLDATA (`$2132`).

- Bit 0 (gray/escala de grises): no tiene equivalente directo en la PPU de SNES; para conseguir un efecto de aclarado se usa color math aditivo junto con COLDATA.
- Bits 1-2 (recorte de los 8 píxeles de la izquierda): menos necesario en SNES gracias al bit de signo de la X de los sprites; para lo mismo en fondos se usan ventanas (windows).
- Bits 3-4 (activar renderizado de fondo/sprites): apagar ambos permite que la CPU lea/escriba VRAM libremente; se usa para cargar escenas y para letterboxing.
- Bits 5-7 (selección de color de tinte): mapean al control de tinte de COLDATA.

**PPUSTATUS ($2002 lectura)** se reparte entre STAT77 (`$213E`) y RDNMI (`$4210`).

- Bit 0: reservado.
- Bit 5: range/time over (límite de sprites/tiles por scanline excedido).
- Bit 6 (sprite 0 hit): no tiene equivalente directo en la PPU de SNES.
- Bit 7: flag de NMI de VBlank sin reconocer.

Para timing por raster se recomienda usar la IRQ H/V a través de NMITIMEN (`$4200` escritura), HTIMEL/HTIMEH (`$4207`-`$4208` escritura) y VTIMEL/VTIMEH (`$4209`/`$420A`), o bien usar HDMA para escribir directamente en registros de la PPU.

**OAMADDR ($2003 escritura)** equivale a OAMADDL/OAMADDH (`$2102`) en SNES, direccionado por palabra (a diferencia de NES). Las entradas de OAM tienen un layout distinto, con la X como primer byte, más una tabla adicional al final que guarda el bit de signo de X y los tamaños de sprite. Se señala que mover la dirección manualmente es mucho menos delicado que la interfaz DRAM de OAM de la PPU de NES.

**OAMDATA ($2004 escritura)** equivale a OAMDATA (`$2104`) en SNES. Algunas revisiones de la PPU de NES tienen lectura poco fiable de este registro; en SNES conviene usar OAMDATAREAD (`$2138` lectura) en su lugar.

**PPUSCROLL ($2005 escritura)** fija el scroll horizontal fino y los bits altos de coarse X/Y de la dirección VRAM; equivale a BG1HOFS (`$210D`, escritura doble) y BG1VOFS (`$210E`, escritura doble).

**PPUADDR ($2006 escritura)** equivale a VMADDL/VMADDH (`$2116`-`$2117`), direccionado por palabra (unidades de 2 bytes). Las direcciones `$3F00`-`$3FFF` de NES (paleta) se corresponden con CGADD (`$2121`) en SNES.

**PPUDATA ($2007 lectura/escritura)** equivale a VMDATAL/VMDATAH (`$2118`-`$2119` escritura) y VMDATALREAD/VMDATAHREAD (`$2139`-`$213A` lectura). La dirección solo se incrementa al escribir/leer el byte bajo o el alto, según el estado de VMAIN.

Las escrituras a `$3F00`-`$3FFF` en NES (paleta) equivalen a CGDATA (`$2122`, escritura doble) en SNES. Las revisiones más recientes de la PPU de NES añaden soporte de lectura de paleta; en SNES el equivalente es CGDATAREAD (`$213B`, lectura doble).

## Notas / gotchas

- No confundir el direccionamiento: en SNES casi todo (VRAM, tilemap, sprites) se maneja en unidades de *palabra* (16 bits), no de byte como en NES.
- Y = 0 de scroll no muestra la primera fila de píxeles reales de la capa, por la línea de blanking inicial que la PPU siempre descarta — hay que tenerlo en cuenta al portar lógica de scroll desde NES.
- No existe sprite 0 hit ni tamaño de sprite 8x16 en SNES; hay que rediseñar cualquier técnica de NES que dependa de ellos.
- El límite de sprites por scanline en SNES es 32 (frente a 8 en NES), pero además hay un límite independiente de 34 tiles de sprite por scanline que también puede recortar sprites.
- Cambiar entre modo de 224 y 239 líneas no es un simple letterbox: desplaza la imagen 8 líneas, hay que compensar el scroll.
- El "gray" de PPUMASK no tiene registro directo en SNES: para atenuar la imagen hay que usar color math con COLDATA.

## Referencias cruzadas

- Ver `leer-escribir-memoria-ppu.md` (detalle de VMAIN, VMADD, VMDATA, CGADD, CGDATA, OAMADD, OAMDATA mencionados en la tabla de equivalencias).
- Ver `vblank-interrupts.md` (detalle de NMITIMEN y RDNMI).
- Ver `hdma-ejemplos.md` (detalle práctico del HDMA mencionado aquí de forma general).
