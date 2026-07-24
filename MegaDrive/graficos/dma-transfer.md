---
titulo: Transferencias DMA
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, dma, vram, z80, rendimiento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/dma-transfer, board.mddc.dev (vía CLAUDE.md del proyecto motor-cenital)]
---

# Transferencias DMA

## Resumen

Explica cómo configurar el VDP para que copie datos directamente desde ROM o RAM hacia la memoria de vídeo mediante DMA, evitando que el 68000 tenga que mover los datos manualmente, mejorando así el rendimiento. Cubre los registros involucrados, el cálculo de direcciones/longitudes y la limitación de 128KB.

## Detalle técnico

### Concepto fundamental

Las transferencias DMA permiten que el VDP copie datos directamente desde ROM o RAM hacia la memoria de vídeo, eliminando la necesidad de que el procesador 68000 maneje esta operación manualmente, lo que mejora significativamente el rendimiento.

### Registros VDP para DMA

El sistema requiere cinco registros VDP específicos para configurar una transferencia:

- **VDPREG_DMALEN_L** (dirección $9300): byte bajo de la longitud.
- **VDPREG_DMALEN_H** (dirección $9400): byte alto de la longitud.
- **VDPREG_DMASRC_L** (dirección $9500): byte bajo de la dirección fuente.
- **VDPREG_DMASRC_M** (dirección $9600): byte medio de la dirección fuente.
- **VDPREG_DMASRC_H** (dirección $9700): byte alto de la dirección fuente.

### Información crítica sobre direcciones y longitudes

"Source address and length are measured in _words_, not bytes." (La dirección fuente y la longitud se miden en words, no en bytes). Esto significa que ambos valores deben desplazarse a la derecha en una posición para convertir de bytes a palabras. La dirección fuente es de 23 bits solamente; el bit 24 se reserva para otras operaciones DMA, por lo que debe enmascararse: `and.l #$7FFFFF, d7`.

### Proceso de configuración

#### Preparación de valores

```
lsr.w   #1, d0          ; convertir longitud a palabras
move.l  a0, d7          ; copiar dirección fuente
lsr.l   #1, d7          ; convertir a palabras
and.l   #$7FFFFF, d7    ; limpiar bit 24 y superiores
```

#### Escritura en registros VDP

Se escriben cinco bytes individuales en los registros VDP, desplazando cada byte superior 8 posiciones. El documento muestra dos enfoques: el método de desplazamiento iterativo o la técnica `movep` optimizada.

### Ejecución de la transferencia

La transferencia se inicia escribiendo la dirección de destino en memoria de vídeo, utilizando comandos especiales que incluyen el flag DMA:

- **VRAM_DMA_CMD**: `$40000080`
- **CRAM_DMA_CMD**: `$C0000080`
- **VSRAM_DMA_CMD**: `$40000090`

La dirección de destino debe procesarse así:
- Enmascarar a 16 bits: `and.l #$FFFF, d1`
- Desplazar izquierda 2: `lsl.l #2, d1`
- Desplazar derecha 2: `lsr.w #2, d1`
- Intercambiar palabras: `swap d1`
- Aplicar comando OR: `or.l #VRAM_DMA_CMD, d1`

### Consideración del Z80

"If the Z80 accesses the 68000 memory at the same time as DMA begins, there's risk that the data will not be transferred properly." (Si el Z80 accede a la memoria del 68000 al mismo tiempo que comienza el DMA, hay riesgo de que los datos no se transfieran correctamente). La solución recomendada es pausar el Z80 durante la transferencia DMA usando macros `FastPauseZ80` y `ResumeZ80`.

### Limitación de 128KB

Esta es la restricción más significativa: "the source address can't cross a 128KB boundary (e.g. cross from $01FFFF to $020000, from $03FFFF to $040000, etc.)" (la dirección fuente no puede cruzar un límite de 128KB). Esto afecta principalmente a ROM; RAM no suele presentar este problema. Las soluciones son: organizar gráficos sin cruzar límites de 128KB, o dividir transferencias que crucen estos límites en múltiples operaciones.

### Cola de transferencias DMA

Para optimizar el rendimiento, se recomienda implementar un sistema de cola que acumule comandos DMA durante el frame y los procese completamente durante el blanking vertical, evitando desperdiciar este período crítico en cálculos.

### Longitud 0 no es "no transferir nada"

Un DMA configurado con longitud `0` no se interpreta como "no transferir" — el hardware lo trata como **65536** (el registro de longitud es de 16 bits y no distingue "cero" de "vuelta completa"). Cualquier código que calcule dinámicamente cuántas words volcar por DMA (p. ej. "cuántos tiles sucios hay que refrescar este frame" en un esquema de streaming por DMA) debe cortocircuitar explícitamente el caso `0` antes de lanzar la transferencia — no asumir que un tamaño 0 es un no-op seguro. Fuente: síntesis de la comunidad (board.mddc.dev) recogida por el proyecto motor-cenital, sin contrastar todavía contra un test ROM propio de esta base.

## Notas / gotchas

- La dirección fuente y la longitud se miden en words, no en bytes: hay que desplazarlas antes de escribirlas en los registros DMA.
- La dirección fuente solo tiene 23 bits útiles; el bit 24 está reservado y debe enmascararse con `and.l #$7FFFFF, d7`.
- Riesgo de corrupción de datos si el Z80 accede a la memoria del 68000 justo cuando empieza el DMA — se recomienda pausar el Z80 durante la transferencia.
- Limitación de 128KB: la dirección fuente no puede cruzar un límite de 128KB (p. ej. de $01FFFF a $020000). Afecta sobre todo a datos en ROM.
- Conviene encolar los comandos DMA durante el frame y procesarlos todos durante el vblank, para no desperdiciar ese tiempo crítico en cálculos.
- Longitud `0` en un DMA se interpreta como `65536`, no como "nada que transferir" — cortocircuitar el caso 0 a mano en cualquier cálculo dinámico de longitud.

## Referencias cruzadas

- (pendiente de enlazar)
