---
titulo: Bumbershoot Software — pipeline de cuantización y compresión para un logo en Genesis
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bumbershoot, paleta, dithering, floyd-steinberg, cuantizacion, lz4, compresion, cram]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2025/07/05/genesis-refining-the-logo-graphics/]
---

# Bumbershoot Software — pipeline de cuantización y compresión para un logo en Genesis

## Resumen

Artículo de 2025 (mucho más tardío que el resto de la serie, que es de 2018) donde el autor documenta el proceso completo de convertir una imagen de logo a un formato válido para el VDP de Genesis: cuantización de color respetando el espacio de color real del hardware, uso combinado de varias capas de vídeo para superar el límite de 16 colores por tile, deduplicación de tiles, y compresión final con una variante simplificada de LZ4. No incluye código; es un artículo de proceso/diseño de arte con cifras concretas.

## Detalle técnico

### Límites de color reales del hardware

CRAM tiene 64 words, valor de color de 9 bits en formato `$0BGR` (512 colores base). El modo shadow/highlight (ver `shadow-highlight.md`) multiplica el rango a 1.408 combinaciones posibles, pero en la práctica, contando la transparencia y las restricciones de uso, el autor cifra el margen de trabajo real en 61 colores simultáneos en pantalla. Las 64 entradas de CRAM se organizan en 4 paletas de 16 (ver `tiles-and-palettes.md`); cada sprite o tile solo puede usar una de esas 4 paletas a la vez.

### Resolución y capas disponibles

Pantalla de 40×28 tiles, planos de scroll de hasta 32/64/128 celdas de lado, dos planos de scroll independientes (A y B) más capa de sprites y ventana (window) de posición fija. El nametable está limitado a 8 KB, lo que en la práctica acota los tamaños de plano útiles a 64×64 o 32×128 celdas.

Detalle citado sobre la señal de vídeo: la resolución de 320 píxeles de la Genesis es "exactamente el doble de la resolución que el NTSC puede manejar con su color clock", lo que hace que colores adyacentes se mezclen visualmente en un televisor por composite — una propiedad que el autor aprovecha deliberadamente en el dithering (ver más abajo).

### Cuantización de color en tres pasos

1. **Redondeo directo (posterizado)** de cada píxel al color de 9 bits más cercano: resultado feo, con bandas de color muy visibles, pero solo 34 colores usados en total.
2. **Dithering de Floyd-Steinberg** aplicado sobre ese mismo espacio de color de 9 bits: mejora la impresión visual, pero el ruido de difusión de error introduce colores nuevos (76 en total), superando el límite de 64 entradas de CRAM.
   - **Solución adoptada:** restringir la paleta candidata del dithering exactamente a los 34 colores obtenidos en el paso 1 (en vez de a todo el espacio de 512), de modo que el error se difunde pero el resultado nunca sale de ese conjunto cerrado de colores.
3. **Retoque manual** de detalles críticos (texto y sombras) con Aseprite, tras la cuantización automática, para pulir zonas donde el resultado algorítmico no es aceptable.

Para elegir qué colores conservar, el autor usa el **método de población** (histograma de frecuencia de color) en vez de median cut, razonando que unos pocos colores atípicos pero muy raros no deben dominar la selección de paleta.

### Aprovechar varias capas para superar el límite de 16 colores por tile

Dado que el logo es el único contenido en pantalla, el autor reparte la misma zona entre plano A, plano B y sprites superpuestos, alcanzando hasta 46 colores distintos combinados en un mismo tile aunque cada capa individual siga limitada a 16. Los 3 colores menos frecuentes de la imagen (solo 12 píxeles en total sobre una imagen de 192×192) se reservan para la capa de sprites; los 31 colores restantes se reparten en dos paletas, una para cada plano de scroll.

### Optimización de almacenamiento en VRAM

- Logo de 24×24 tiles en dos capas: coste bruto de patrones = 24×24 tiles × 32 bytes/tile × 2 capas = 36.864 bytes de VRAM.
- **Deduplicación de tiles:** de 1.152 tiles teóricos a solo 512 únicos tras eliminar repetidos.
- Coste del nametable (los índices que referencian esos tiles, no los propios píxeles): 24×24×2×2 = 2.304 bytes. Añadir la fuente de texto cuesta ~2 KB adicionales.
- **Compresión:** LZ4 con ajustes de máxima compresión, descartando el formato de framing estándar de LZ4 — el autor razona que, al tratarse de datos fijos horneados en el cartucho (no un stream), la información de framing del contenedor estándar es peso muerto que se puede omitir en favor de un formato propio mínimo.
- Tamaño final tras compresión (gráficos + fuente): **11.437 bytes**, frente a 16.000 bytes sin comprimir.
- Se probó también compresión delta antes de LZ4, pero empeoró el resultado (12.733 bytes en vez de 11.437), así que se descartó.
- El búfer de descompresión en RAM necesario para reconstruir los datos ronda "un pelín más de 30 KB".

## Notas / gotchas

- El truco de "más colores combinando capas" solo es aplicable tal cual porque el logo es el único contenido en pantalla; en una escena de juego normal, plano B y los sprites suelen estar ocupados por fondo/personajes y no están libres para robarles paleta a un elemento puntual.
- Restringir el dithering a la paleta ya cuantizada (en vez de dejar que Floyd-Steinberg elija libremente del espacio de 512 colores) es el paso clave que evita salirse del límite de 64 entradas de CRAM — un error común es aplicar dithering "genérico" sin esta restricción y descubrir después que la imagen resultante necesita más colores de los que caben.
- El formato "LZ4 sin framing estándar" descrito aquí es una variante casera del autor, no compatible con herramientas LZ4 genéricas — cualquier decodificador debe escribirse a medida (no vale un `lz4 -d` estándar).
- Artículo publicado en 2025, muy posterior al resto de la serie (2018); su nivel de detalle en el pipeline de arte es mayor que en las entradas tempranas, centradas más en mecánica de VDP que en autoría de contenido gráfico.

## Referencias cruzadas

- [Modo Shadow/Highlight](shadow-highlight.md) — mecanismo que expande el rango de color citado aquí (512 → 1.408 combinaciones).
- [Rampa de color del VDP](vdp-color-ramp.md) — curva real de salida de color relevante para cualquier cuantización que aspire a igualar el resultado en hardware real, no solo en emulador.
- [Tiles y paletas](tiles-and-palettes.md) — formato base de las 4 paletas de 16 colores que este artículo reparte entre plano A y plano B.
