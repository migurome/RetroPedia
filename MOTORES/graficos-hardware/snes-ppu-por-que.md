---
titulo: Por qué el sistema PPU de la SNES se diseñó así
plataforma: [General]
categoria: [graficos-hardware]
tags: [snes, ppu, vram, sprites, line-buffer, arquitectura-hardware]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/snes_ppus_why/index.html]
---

# Por qué el sistema PPU de la SNES se diseñó así

## Resumen

Primera entrada de la serie de Fabien Sanglard sobre el sistema gráfico de SNES. Su tesis central: prácticamente todas las decisiones de diseño del PPU (dos chips, ausencia de framebuffer, límites de capas/sprites por modo) derivan de una única restricción física — la latencia de acceso a la VRAM frente al tiempo disponible por píxel. Es el ángulo "por qué" de la serie: explica la restricción raíz antes de entrar en el "cómo" (arquitectura interna) y el "diseño" (contexto de ingeniería del sistema de vídeo completo).

## Detalle técnico

### El problema fundamental: latencia de VRAM

El reloj de vídeo de la SNES opera a 5.369.317 Hz, lo que significa que el sistema debe entregar un color cada **186,2 nanosegundos** ("A pixel every 186.2ns"). Los chips de VRAM usados tienen un tiempo de acceso de 100 ns, lo que en la práctica permite **un único acceso a VRAM por píxel**.

Esta única limitación explica, según el artículo:

- Por qué no existe framebuffer (habría sido prohibitivamente costoso).
- Por qué existe una arquitectura de dos PPUs.
- Por qué cada modo de vídeo tiene restricciones específicas de número de capas y profundidad de color.

### Solución: lectura anticipada y buffers internos

En lugar de un único acceso lento por píxel, el PPU2 empieza a leer datos de VRAM **antes** de que se dibuje el primer píxel de una línea, acumulándolos en buffers internos. La estrategia general es procesar varios píxeles a la vez agrupando lecturas:

**Modo 0 (4 capas, 2bpp cada una):**
- 4 lecturas: entradas de tilemap, una por capa.
- 4 lecturas: datos de píxel de los tiles (cada lectura de 16 bits entrega 8 píxeles).
- Total: 8 lecturas → 8 píxeles generados. ("8 read -> Can generate 8 pixels")

**Modo 1 (3 capas: 4bpp, 4bpp, 2bpp — el modo más usado):**
- 3 lecturas: entradas de tilemap.
- 4 lecturas: 2×4 píxeles de las dos capas de 4bpp + 1×8 píxeles de la capa de 2bpp.
- Total: 8 lecturas → 8 píxeles.

**Modo 3 (2 capas: 8bpp, 2bpp):**
- Solo 7 lecturas para generar 8 píxeles — mayor margen que los modos anteriores.

### Modo 7: el caso especial

El Modo 7 es "el más difícil de implementar" porque permite rotación y escalado del fondo. Aquí las líneas de dirección de VRAM se vuelven críticas: el PPU puede direccionar cada chip de VRAM de forma independiente (uno para entradas de tilemap, otro para datos de tile), logrando 1 píxel por ciclo mediante accesos simultáneos:

```
Time -2: Get tilemap for Pixel 0  (VRAM chip1)
Time -1: Get tilemap for Pixel 1  (VRAM chip1)
Time -1: Get tile for Pixel 0 (VRAM chip2)
Time 0: Emit Pixel 0
```

### Renderizado de sprites: el problema del acceso exclusivo

Cita de byuu: "During active screen rendering, the video RAM of the PPUs is completely inaccessible to the SNES CPU" — durante el renderizado activo de pantalla, la VRAM de las PPUs es completamente inaccesible para la CPU de la SNES.

**Solución: line buffer durante HBLANK.**

Una línea de la SNES tiene 341 dots (256 activos + 85 en blanco). El PPU1 aprovecha ese período de HBLANK (85 dots = 15.827 ns) para pre-cargar (prefetch) datos de sprites, construyendo un "sprite line buffer" dividido en unidades de 8 píxeles llamadas **"slivers"** (4 bytes = 2 accesos a VRAM cada una).

- Capacidad: 34 slivers = 272 píxeles de sprite como máximo por línea.
- Límite real: 32 sprites **o** 272 píxeles por línea — lo que se alcance primero.
- Overhead: el prefetch de sprites solo consume 6.800 ns de los 15.827 ns disponibles en HBLANK, dejando 9.027 ns libres para otros efectos.

### Tabla comparativa de arquitecturas de sprites de la época

| Máquina | Técnica | Capacidad | Límite por línea |
|---------|---------|-----------|-------------------|
| C64 | Unidades de sprite (8) | No reutilizable en la misma línea | 8 sprites |
| Amiga | Unidades de sprite (8) | Reutilizable en la línea | Más de 8 (mediante multiplexado) |
| SNES | Line buffer | 128 sprites | 32 sprites / 272 píxeles |
| Genesis | Line buffer | 80 sprites | 20 sprites / 320 píxeles |
| Neo-Geo | Double line buffer | 381 sprites | 96 por línea |
| CPS-1 | Framebuffer de sprites | 256 sprites | Sin límite por línea |

### Decisiones de arquitectura comparadas

- **SNES (line buffer)**: equilibrio entre coste y capacidad. El prefetch durante HBLANK permite rasterizar la línea actual mientras se construye el buffer de la siguiente.
- **CPS-1 (framebuffer de sprites)**: "Capcom no escatimó en gastos" — usa SRAM cara para mantener dos buffers completos de pantalla (bus de 32 bits), permitiendo 256 sprites de 16×16 píxeles sin ninguna limitación por línea.
- **Neo-Geo (double line buffer)**: permite al PPU recuperar los sprites de la siguiente línea mientras aún procesa la actual, logrando el triple de capacidad que la SNES (96 vs 32 sprites por línea). La contrapartida: el sistema "no tiene fondo, todo es sprite".

### Organización de datos en VRAM

La arquitectura impone restricciones estrictas sobre cómo deben almacenarse los datos:

- **Tiles 2bpp**: formato planar — cada chip de VRAM contiene los bytes altos/bajos por separado.
- **Tiles 4bpp**: formato planar modificado — dos planos empaquetados en un byte, distribuidos entre los chips.
- **Entradas de tilemap**: el entrelazado (interleave) de VRAM garantiza que una entrada completa se pueda leer en un único acceso, usando ambos buses de dirección disponibles.

## Notas / gotchas

- La idea central del artículo es que casi ninguna limitación de la SNES (número de capas, profundidad de bits, sprites por línea, modos de vídeo) es arbitraria: todas derivan de un único cuello de botella — la latencia de un componente concreto, el acceso a VRAM de 100 ns frente a un presupuesto de 186,2 ns por píxel.
- La arquitectura de dos PPUs (PPU1 como "cerebro" que decide direcciones, PPU2 como "compositor" que solo consume datos) es la solución arquitectónica a ese cuello de botella, no una elección de diseño independiente.
- Comparar el enfoque de "line buffer" (SNES, Genesis) frente al de "framebuffer de sprites completo" (CPS-1) o "double line buffer" (Neo-Geo) es directamente relevante para decisiones de motor: más memoria dedicada a sprites compra más capacidad y elimina límites por línea, a cambio de coste de silicio.
- El límite de "32 sprites o 272 píxeles, lo que ocurra primero" es un patrón de diseño común en hardware de la época (comparar con el límite de Mega Drive de 20 sprites/320 píxeles por línea en la tabla) — ambos límites son consecuencia de cuánto tiempo de HBLANK hay disponible para prefetch, no límites "porque sí".

## Referencias cruzadas

- (pendiente de enlazar)
