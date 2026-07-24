---
titulo: Offset-per-tile
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, offset-per-tile, scroll, bg3, modos-2-4-6]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Offset-per-tile]
---

# Offset-per-tile

## Resumen

Offset-per-tile es una característica disponible en los modos de BG 2, 4 y 6 que permite sobrescribir el scroll horizontal y/o vertical de una capa de fondo de forma independiente para cada columna de tiles, en vez de aplicar un único desplazamiento global. Se implementa reutilizando el tilemap de BG3 (BG2 en el modo 6) como una tabla de valores de desplazamiento en lugar de como un tilemap normal.

## Detalle técnico

### Reutilización del tilemap de BG3

En los modos 2, 4 y 6, el PPU deja de interpretar el tilemap de BG3 como referencias a tiles y en su lugar lee de él dos filas de valores: una con los desplazamientos horizontales y otra con los verticales. En el modo 4 solo se lee una fila, y cada entrada indica explícitamente si es un valor horizontal o vertical mediante un bit de dirección.

### Formato de cada entrada (16 bits)

```
15  bit  8   7  bit  0
---- ----   ---- ----
V21. ..SS   SSSS Ssss
```

- **`S...s` (bits 0-9):** nuevo valor de desplazamiento de scroll.
- **`1`:** si está activo, esta entrada sustituye el scroll de BG1 en esa columna.
- **`2`:** si está activo, esta entrada sustituye el scroll de BG2 en esa columna.
- **`V`:** solo en modo 4 — indica si el valor de esta entrada es horizontal (0) o vertical (1).

Para los valores horizontales, los 3 bits más bajos del scroll fino (desplazamiento de 0 a 7 píxeles) no se sustituyen: el offset-per-tile solo reemplaza la parte "gruesa" del desplazamiento horizontal, dejando intacto el ajuste fino ya configurado en el registro BGnHOFS correspondiente. El desplazamiento vertical, en cambio, se sustituye por completo.

### Alcance espacial del efecto

La primera columna de tiles de la pantalla no se ve afectada por esta característica: siempre usa el valor de scroll estándar de HOFS/VOFS. Las 32 entradas de la tabla de offsets controlan las columnas siguientes (columnas 2 a 33), lo que deja la columna inicial bajo control exclusivo de los registros de scroll normales.

## Notas / gotchas

- Al activar offset-per-tile, el tilemap de BG3 (o BG2 en modo 6) deja de estar disponible como capa de fondo dibujable: su VRAM pasa a usarse íntegramente como tabla de desplazamientos.
- Solo el scroll horizontal grueso se sustituye; el scroll fino de 0-7 píxeles sigue viniendo del registro BGnHOFS estándar.
- En modo 4, cada entrada de la tabla sirve para horizontal O vertical (nunca ambos a la vez), señalado por el bit de dirección; en modos 2 y 6 se leen dos filas separadas (una por eje).
- La primera columna de la pantalla nunca se ve afectada por esta técnica, a diferencia de las columnas siguientes.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Fondos y modos de BG](backgrounds.md)
- [Tilemaps](tilemaps.md)
