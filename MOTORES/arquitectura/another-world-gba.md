---
titulo: Los polígonos de Another World — el port a Game Boy Advance
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, gba, modo4, arm7tdmi, polígonos-2d, dma]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons_GBA/index.html]
---

# Los polígonos de Another World — el port a Game Boy Advance

## Resumen

Analiza el port homebrew de Cyril Cogordan ("Foxy", ~2001) del motor de polígonos de Another World a Game Boy Advance, hecho por ingeniería inversa del binario de Atari ST con IDA Pro. A diferencia de Genesis y SNES, la GBA sí ofrece modos de vídeo en framebuffer directo (bitmap), lo que simplifica el problema de "cómo rellenar un polígono sin blitter" pero introduce sus propios cuellos de botella: un bus de ROM de 16 bits, una jerarquía de memoria con velocidades muy dispares (IWRAM vs EWRAM vs ROM) y el conflicto entre DMA (bloqueante) y audio en tiempo real.

## Detalle técnico

### Contexto histórico

La línea Game Boy (1989-1998) alcanzó 120 millones de ventas acumuladas pese a especificaciones modestas de 8 bits. La Game Boy Advance (2001) representó la primera evolución arquitectónica mayor, convirtiéndose en "la consola de más rápida venta, con 500.000 unidades vendidas en la primera semana", alcanzando 81.51 millones de unidades a finales de 2009.

La GBA llegó en la era de acceso extendido a PC e internet, permitiendo comunidades homebrew robustas con tutoriales y documentación publicados que "perduran hasta hoy".

### Arquitectura

**CPU y componentes principales:** el chip principal integra una CPU ARM7tdmi (16.78 MHz) junto a un PPU con 96 KiB de VRAM, un controlador DMA y 32 KiB de IWRAM (RAM de trabajo interna al chip).

**Jerarquía de memoria y rendimiento de acceso:**

| Tipo  | Bus  | ciclo-8 | ciclo-16 | ciclo-32 |
|-------|------|---------|----------|----------|
| IWRAM | 32   | 1       | 1        | 1        |
| EWRAM | 16   | 3       | 3        | 6        |
| VRAM  | 16   | 1       | 1        | 2        |
| ROM   | 16   | 5       | 5        | 8        |
| SRAM  | 8    | 5       | 5        | 5        |

La relación inversa entre tamaño y velocidad se aprecia en el rendimiento de un ciclo de IWRAM frente a los 5 ciclos constantes de SRAM independientemente del ancho de dato.

**Mapa de espacio de direcciones:**

Memoria interna general:
- 00000000-00003FFF: BIOS System ROM (16 KiB)
- 02000000-0203FFFF: WRAM externa (256 KiB)
- 03000000-03007FFF: WRAM interna al chip (32 KiB)
- 04000000-040003FE: Registros de E/S

Memoria de vídeo interna:
- 05000000-050003FF: Paleta RAM BG/OBJ (1 KiB)
- 06000000-06017FFF: VRAM (96 KiB)
- 07000000-070003FF: OAM, atributos de objeto (1 KiB)

Memoria externa (Game Pak):
- 08000000-09FFFFFF: ROM/FlashROM (máx. 32 MiB, wait state ID0)
- 0A000000-0BFFFFFF: ROM/FlashROM (máx. 32 MiB, wait state ID1)
- 0C000000-0DFFFFFF: ROM/FlashROM (máx. 32 MiB, wait state ID2)
- 0E000000-0E00FFFF: SRAM (máx. 64 KiB)

Las tres regiones de ROM mapean a los mismos 32 MiB físicos con tiempos de acceso ajustables para chips de distinta velocidad.

### El cuello de botella del bus de 16 bits

**El problema:** con un bus de datos de 16 bits conectando la ROM a la CPU, y el ARM esperando instrucciones de 32 bits, la recuperación requeriría en teoría 16+ ciclos por instrucción. El pipeline de tres etapas agrava esta latencia hasta convertirla en "un asesino de rendimiento".

**La solución: buffer de prefetch.** Nintendo implementó un buffer de prefetch de 8 × 16 bits que se rellena durante los ciclos ociosos de la CPU, elimina por completo los wait states, y da latencia de 1 ciclo en lecturas de 16 bits.

**Limitación:** al no haber predicción de saltos, "cualquier instrucción de salto garantiza un fallo de prefetch y un pipeline ARM vacío".

**Compromiso del juego de instrucciones — modo Thumb.** El ARM7tdmi soporta instrucciones Thumb de 16 bits (frente a las de 32 bits del modo ARM). Según la documentación de ARM: "el código Thumb puede llegar a ocupar hasta un 65% del tamaño del código ARM, y dar un 160% del rendimiento de un procesador ARM equivalente conectado a un sistema de memoria de 16 bits." Para aplicaciones sensibles al tamaño: "el modo Thumb de 16 bits alternativo reduce el código en más de un 30% con una penalización de rendimiento mínima."

**Ejemplo de código — función abs():**

Versión ARM (12 bytes, 3 instrucciones):
```
abs_arm(int):
    cmp     r0, #0
    rsblt   r0, r0, #0
    bx      lr
```

Versión Thumb (8 bytes, 4 instrucciones — 25% más instrucciones pero 33% más pequeña):
```
abs_thumb(int):
    asrs    r3, r0, #31
    adds    r0, r0, r3
    eors    r0, r3
    bx      lr
```

### Especificación del sistema de vídeo

**Estándar de pantalla:** resolución fija de 240 × 160 píxeles a 60Hz (sin variante PAL/NTSC).

**Arquitectura del PPU:** al igual que en SNES, el PPU organiza el renderizado en torno a Backgrounds y Sprites. De forma crítica para Another World, la GBA incluye tres modos bitmap que permiten manipulación directa por píxel sin necesidad de simular con tiles.

**Modos de vídeo:**

| Modo | Tipo    | Resolución | Backgrounds | Colores            |
|------|---------|------------|-------------|---------------------|
| 0    | Tile    | 240 × 160  | 4           | 8bpp (indexado)      |
| 1    | Tile    | 240 × 160  | 3           | 8bpp (indexado)      |
| 2    | Tile    | 240 × 160  | 2           | 8bpp (indexado)      |
| 3    | Bitmap  | 240 × 160  | 1           | 16bpp (directo)      |
| 4    | Bitmap  | 240 × 160  | 2           | 8bpp (indexado)      |
| 5    | Bitmap  | 160 × 128  | 2           | 16bpp (directo)      |

**Codificación de color:** el color de 16 bits usa RGB de 5 bits (formato 555) con 1 bit sin usar. El modo 3 permite escritura directa al framebuffer sin lookup de paleta, pero consume tanta VRAM que el doble buffer se hace imposible.

**Timing de pantalla:**

| Métrica    | Longitud       | Ciclos |
|------------|----------------|--------|
| Píxel      | 1              | 4      |
| HDraw      | 240            | 960    |
| HBlank     | 68             | 272    |
| Scanline   | HDraw + HBlank | 1232   |
| VDraw      | 160 × Scanline | 197120 |
| VBlank     | 68 × Scanline  | 83776  |
| Refresco   | VDraw + VBlank | 280896 |

**Direccionamiento de framebuffer (Modo 4):**
```c
u16*  VideoFrontBuffer = (u16*) 0x6000000;
u16*  VideoBackBuffer  = (u16*) 0x600a000;
u16*  Palette          = (u16*) 0x5000000;
```

**Métodos de sincronización con VBLANK:**

Método 1 — sondeo de REG_VCOUNT:
```c
#define REG_VCOUNT *(vu16*)0x04000006

void vid_vsync() {
    while(REG_VCOUNT >= 160);
    while(REG_VCOUNT < 160); 
}
```

Método 2 — dirigido por interrupción con sondeo:
```c
volatile u16 num_vbl=0;
Set_Interrupt(IT_VBLANK, &it_vbl);
void it_vbl() { num_vbl++; REG_IF=...; }

void vid_vsync() {
  u16 nv = num_vbl;
  while(nv==num_vbl);
}
```

Método 3 — VBlankIntrWait de BIOS (preferido): "evita un busy loop y reduce el consumo de batería."

**Intercambio de framebuffer:**
```c
#define DISP_CR     *(u16*)0x4000000
#define BACKBUFFER    0x10

void flip() {
  vid_vsync()

  if (DISP_CR & BACKBUFFER){
    DISP_CR &= ~BACKBUFFER;
    VideoBuffer = VideoBackBuffer;
  } else {
    DISP_CR |= BACKBUFFER;
    VideoBuffer = VideoFrontBuffer;
  }
}
```

**Curiosidad sobre el comportamiento de la VRAM:** "la VRAM es peculiar en el sentido de que se puede leer libremente pero solo acepta escrituras alineadas de 16 bits. Intentar escribir un solo byte también modifica el byte adyacente."

### Toolchain y proceso de build de GBA

**Stack de desarrollo:** Devkitpro (basado en GNU), soportando compilación de C++/C/ASM.

**Pipeline de build en cinco pasos:**

```bash
1$ arm-gcc -c main.c -mthumb-interwork -mthumb -O2 -o main.o
2$ arm-thumb-elf-as crt0.s -o crt0.o
3$ arm-gcc main.o crt0.o -mthumb-interwork -mthumb -specs=gba.specs -o main.elf
4$ arm-objcopy -v -O binary main.elf main.gba
5$ gbafix main.gba
```

Los pasos 1-3 producen ejecutables ELF estándar con instrucciones Thumb. El paso 4 elimina todos los metadatos ELF, dejando binario en crudo. El paso 5 añade la cabecera GBA.

**Mecanismo de optimización de IWRAM:** el paso 2 (crt0.s) habilita la población automática de IWRAM. No existe bootloader — la ROM se mapea directamente en 0x08000000. Los desarrolladores declaran el código crítico así:

```c
#define CODE_IN_IWRAM __attribute__ ((section(".iwram"),long_call))

void CODE_IN_IWRAM MyCriticalFunctionWhichNeedsSpeed(...) {
  ...
}
```

El script del linker (gba.specs → gba_cart.ld, líneas 158-167) define los identificadores de origen de copia (__iwram_start__, __iwram_end__). El runtime de crt0.s (línea 137) copia estas secciones a 0x03000000 antes de ejecutar main(). El mismo runtime establece la pila de la CPU en IWRAM "por razones de rendimiento".

El runtime también gestiona de forma transparente los saltos "largos" de PC entre ROM e IWRAM.

**Asignación de memoria:** "¿Adivinas dónde se asigna memoria en malloc? Si has dicho EWRAM, aciertas, ¡ahí es donde está el heap!"

### Implementación de Another World en GBA

**Desarrollador:** el ingeniero inverso francés Cyril Cogordan (alias "Foxy") implementó Another World en GBA como proyecto personal hacia 2001.

**Contexto de desarrollo:** Cogordan había hecho ingeniería inversa completa de Another World a partir del binario de Atari ST usando IDA Pro, lo que le dio conocimiento total de los algoritmos. Persiguió un "port pixel-perfect" a GBA: "hice ingeniería inversa de todo el juego a partir de mi ejecutable m68k de Atari ST usando IDA Pro y luego reimplementé cada función en C. Eso no fue muy difícil. Lo desafiante después fue optimizar para GBA y conseguir un buen framerate."

**Entorno de desarrollo:** SO Windows XP; IDE Visual Studio; compilador Devkitpro; emulación con No$GBA; pruebas en hardware real con cartucho linker; referencia técnica "GBATek" (documentación de Martin Korth).

**Arquitectura de renderizado:** idéntica en concepto a los ports de SNES y Genesis — doble buffer en VRAM con tres framebuffers de trabajo en EWRAM. Al completarse un frame, la CPU espera al VBLANK, copia el frame al buffer libre, e intercambia. Un sexto framebuffer gestiona los ajustes de luminosidad/contraste sin corromper el estado del juego.

**Selección de modo de vídeo:** la GBA se configuró en modo 4 (bitmap indexado de 8 bits) usando solo el Background 2:

```c
#define MODE4     0x4
#define BG2       0x400
#define SetVideoMode(mode) DISP_CR=(mode)

SetVideoMode(MODE4 | BG2);
```

"Este modo da 8 bits por píxel, lo que significa que se desperdicia un nibble por byte, pero no había forma de evitarlo."

**Compromiso de resolución:** para mantener la relación de aspecto original, se dejan diez scanlines en blanco: "resolución: 240 × 150. Diez líneas de píxeles se dejan en blanco para igualar la relación de aspecto original."

### Retos críticos de optimización

**Limitaciones del DMA:** Cogordan intentó inicialmente usar el DMA de la GBA para las operaciones COPY y FILL:

```c
void DMACopyCH3(void* source, void* dest, u32 WordCount, u32 mode);
void DMAFillMemory16(void* dest, u32 WordCount, u16 val_fill);
```

Sin embargo: "tuve problemas usando DMA3 porque son llamadas bloqueantes. No se ejecuta ningún código hasta que la petición DMA se completa. Esto causaba cortes en la reproducción de audio, lo cual era inaceptable."

**Solución: COPY/FILL por CPU en IWRAM.** Cogordan reubicó COPY y FILL en IWRAM, compilados en ARM (frente a Thumb para el resto del juego):

```c
void CODE_IN_IWRAM DMACopyCH3(void* source, void* dest, u32 WordCount, u32 mode) {
  for (u32 i=0; i < WordCount; i++)
    *((u16 *)dest)++ = *((u16 *)source)++;
}

void CODE_IN_IWRAM DMAFillMemory16(void* dest, u32 WordCount, u16 val_fill) {
  for (u32 i=0; i < WordCount; i++)
    *((u16 *)dest)++ = val_fill;
}
```

Las operaciones usan base de 16 bits "porque el bus habría partido las operaciones de 32 bits en dos de todas formas".

**Optimización del dibujado de polígonos.** La misma estrategia IWRAM/ARM se aplicó a DRAW y TEXTAT:

```c
void CODE_IN_IWRAM draw_shape(u8 *pt_shape, s32 x, s32 y, u16 scale, u16 color);
void CODE_IN_IWRAM printat(u16 ntxt, u16 x, u16 y, u8 col);
```

La función draw_shape emplea la optimización de "múltiples píxeles por escritura" vista en todos los ports de Another World:

```c
u16 col = (color<<8)|color;

if (x1 & 1) {
  *pt_scr++=(u8)color;
  lg--;
}

for (; lg>1; lg-=2)
  *((u16 *)pt_scr) ++= col;
```

Esto escribe pares de píxeles en escrituras únicas de 16 bits a lo largo de las scanlines horizontales.

### Resultados de rendimiento y limitaciones

**Framerate alcanzado:** medido por Felix Jones en hardware real, el port logró "un framerate que varía entre 12 y 20 fps".

**Valoración del desarrollador:** Cogordan afirmó: "al final creo que encontré el equilibrio adecuado entre el código original, la velocidad, y la fidelidad de vídeo/audio. Probablemente sería posible optimizar más el código pero no creo que aportara mucho al juego." La filosofía de diseño central fue "rendir homenaje a Another World y ser lo más fiel posible al original".

### Eventos posteriores al lanzamiento y recepción

Cerca de completarse como lanzamiento homebrew gratuito, Éric Chahi (el diseñador original de Another World) contactó a Cogordan. Chahi estaba adquiriendo los derechos para el 20º aniversario. Cogordan aceptó retrasar el lanzamiento hasta que Chahi resolviera el tema comercial. Posteriormente: "más adelante, él me ayudó a completar el proyecto proporcionándome los fondos remasterizados a 256 colores. Ya estaba encantado de haber completado el juego, pero poder lanzarlo con la bendición de Éric fue increíble."

Recepción de la comunidad: "el juego fue muy bien recibido en la escena homebrew. El lanzamiento ocurrió en playerAdvance.org, un sitio dedicado a juegos homebrew para consolas. En una semana el juego se había descargado 10.000 veces. Incluso apareció en la revista Joypad."

### Disponibilidad del código fuente

Cogordan compartió el código fuente para el artículo, con la salvedad de que se eliminó el fichero files.c (que contiene el bytecode de Éric Chahi). Su redistribución requiere consultar con "Foxy" sobre la licencia adecuada.

### Comparación con los otros ports

A diferencia de Genesis y SNES, con motores de tiles dedicados, los modos bitmap de la GBA (3-5) son más primitivos — el formato indexado de 8 bits del modo 4 desperdicia un nibble por byte, pero fue la única vía viable para mantener la fidelidad visual del juego dentro de las restricciones de VRAM (96 KiB en total). El rasterizado y relleno de polígonos se mantuvo algorítmicamente idéntico al original, pero la severa jerarquía de memoria de la GBA (diferencial de velocidad IWRAM vs EWRAM) y la potencia de CPU limitada (16.78 MHz) forzaron la reubicación de rutas críticas y la selección de juego de instrucciones (ARM vs Thumb). El resultado de 12-20 fps representa un compromiso sustancial frente a los 24+ fps del PC de la época, pero mantiene la integridad del audio — un trade-off que el bloqueo del DMA habría violado.

## Notas / gotchas

- A diferencia de Genesis/SNES, la GBA sí tiene modos de framebuffer directo (modo 4), lo que resuelve de raíz el problema de "rasterizar un polígono sin blitter" — pero el precio es la limitación de VRAM (96 KiB total) y el desperdicio de medio byte por píxel al usar 8bpp indexado en vez de 4bpp.
- El DMA de la GBA es **bloqueante**: usarlo para COPY/FILL detiene toda ejecución de CPU hasta terminar, lo cual rompía el audio en tiempo real. La solución fue implementar COPY/FILL por software en IWRAM compilado en modo ARM (no Thumb) — un ejemplo claro de "el hardware ofrece una vía rápida pero incompatible con otro requisito del motor (audio sin cortes), así que se descarta a favor de una solución por software más lenta pero cooperativa".
- La jerarquía de memoria (IWRAM 1 ciclo vs EWRAM 3 ciclos vs ROM 5 ciclos) obliga a reubicar manualmente las rutinas críticas (draw_shape, printat, DMACopyCH3, DMAFillMemory16) a IWRAM vía atributos de sección del compilador (`__attribute__((section(".iwram"),long_call))`), un patrón de "code placement" análogo al de cualquier motor con múltiples niveles de memoria de distinta velocidad.
- El compromiso ARM vs Thumb (Thumb: ~65% del tamaño de código, ~160% del rendimiento en bus de 16 bits, pero más instrucciones por operación) es una decisión de motor por-función: código general en Thumb por densidad, hot paths en ARM dentro de IWRAM por velocidad.
- La resolución se recortó a 240×150 (dejando 10 scanlines en blanco) solo para preservar la relación de aspecto original del juego, no por una limitación de ancho de banda como en Genesis/SNES — el cuello de botella aquí era más de CPU/DMA que de transferencia de framebuffer.
- Sin predicción de saltos y sin wait states gracias al buffer de prefetch de 8×16 bits: cualquier salto (jump/branch) invalida el prefetch y vacía el pipeline — relevante para cualquier bucle interno de rasterizado con saltos condicionales frecuentes por scanline.

## Referencias cruzadas

- MOTORES/arquitectura/another-world-pc-dos.md
- MOTORES/arquitectura/another-world-genesis.md
- MOTORES/arquitectura/another-world-snes.md
- MOTORES/arquitectura/another-world-jaguar.md
