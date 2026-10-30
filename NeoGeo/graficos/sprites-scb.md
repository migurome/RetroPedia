---
titulo: Sprites en Neo Geo — tablas SCB1-SCB4, cadenas sticky, espejo y orden de dibujo
plataforma: [NeoGeo]
categoria: [graficos]
tags: [sprites, vram, scb1, scb2, scb3, scb4, sticky, metasprite, hflip, profundidad, capa-fix]
estado: revisado
fecha: 2026-10-23
fuentes: [ngdevkit registers.h y ejemplo 03-sprite-animation, proyecto RickAndMorty/neogeo (src/actor.c) probado en GnGeo]
---

# Sprites en Neo Geo (SCB1-SCB4)

## Resumen

Un sprite de Neo Geo es **una columna de 16 px de ancho** y hasta 32 tiles de alto. Los personajes se forman encadenando columnas con el bit *sticky*. La VRAM no está en el mapa de memoria del 68000: se accede con tres registros (dirección, dato y autoincremento).

## Detalle técnico

### Acceso a la VRAM

- `REG_VRAMADDR` (0x3C0000): dirección en palabras.
- `REG_VRAMRW` (0x3C0002): leer o escribir una palabra.
- `REG_VRAMMOD` (0x3C0004): cuánto avanza la dirección tras cada escritura.
  - Con 1 se recorren los tiles de un sprite en SCB1.
  - Con **0x200** se pasa de SCB2 a SCB3 y a SCB4 del mismo sprite con tres escrituras seguidas, porque esas tablas están separadas 0x200 palabras.

### Tablas de cada sprite `s`

| Tabla | Dirección | Contenido |
|---|---|---|
| SCB1 | `s*64` | 32 pares (tile, atributos), de arriba abajo |
| SCB2 | `0x8000+s` | escala: `0x0FFF` = tamaño completo |
| SCB3 | `0x8200+s` | `((496 - y_arriba) & 0x1FF) << 7`, bit 6 sticky y bits 0-5 de altura en tiles |
| SCB4 | `0x8400+s` | `(x & 0x1FF) << 7` |

- Atributo de cada tile en SCB1: paleta en los bits 15-8, bits altos del número de tile en 7-4, autoanimación en 3-2, espejo vertical en el bit 1 y **espejo horizontal en el bit 0**.
- **Altura 0 en SCB3 oculta el sprite.** Al arrancar conviene poner a 0 los 448 SCB3, porque la VRAM puede traer restos.
- La X es de 9 bits: los valores negativos dan la vuelta (−45 se guarda como 467), y un personaje que sale por la izquierda se ve recortado sin hacer nada especial. ✔

### Personaje de varias columnas (cadena sticky)

- El primer sprite lleva su SCB3 completo (Y y altura) y su SCB4 (X).
- Los siguientes solo necesitan SCB2 = `0x0FFF` y SCB3 = `0x40` (sticky): copian Y, altura y escala del anterior y se colocan 16 px a su derecha. Su SCB4 se ignora.
- **Girar el personaje:** se escriben las columnas en orden inverso (`cols-1-c`) con el bit de espejo horizontal de cada tile, y se usa el pivote reflejado: `left = x - (cols*16 - pivot_x)`.
- Para no saturar la VRAM, SCB1 solo se reescribe cuando cambia el fotograma, el giro o el primer sprite asignado. SCB2-4 son tres palabras más una por columna, y se escriben en cada fotograma.

```c
*REG_VRAMMOD = 0x200;
*REG_VRAMADDR = ADDR_SCB2 + s;
*REG_VRAMRW = 0x0FFF;                                   // SCB2
*REG_VRAMRW = (((496 - top) & 0x1FF) << 7) | rows;      // SCB3
*REG_VRAMRW = (left & 0x1FF) << 7;                      // SCB4
for (c = 1; c < cols; c++) {
    *REG_VRAMADDR = ADDR_SCB2 + s + c;
    *REG_VRAMRW = 0x0FFF;
    *REG_VRAMRW = 0x40;                                 // sticky
}
```

### Orden de dibujo

- **El sprite de índice mayor se pinta encima.** ✔
- Para ordenar por profundidad (ver [ordenación de sprites](../../GLOSARIO/sorting-sprites.md)), cada fotograma se reparten los bloques de sprites: el personaje con mayor `y` recibe los índices más altos. Si el orden cambia, hay que reescribir SCB1 de los dos, porque han cambiado de sprites.
- En el proyecto se dejó sin usar el sprite 0 y se empezó en el 1, como en los ejemplos de ngdevkit.

### Capa fix (texto)

- Mapa de 40×32 tiles en la VRAM 0x7000, **por columnas**: `0x7000 + x*32 + y`. Cada palabra lleva la paleta (bits 15-12) y el tile (bits 11-0).
- En pantalla, la fila `r` queda en la línea `8r - 16`: la fila 2 es la primera visible y la 29 la última. En GnGeo se ven las columnas 1–38. ✔
- `ng_text(x, y, paleta, "texto")` escribe el tile igual al código ASCII. `ng_cls()` rellena todo con el tile 255 (vacío).

## Notas / gotchas

- Con `y` medida desde arriba, subir un personaje es **sumar** al valor de SCB3, porque se guarda `496 - y`.
- La caja de tiles de un personaje suele ser bastante más ancha que el dibujo, porque engloba todos los fotogramas: Morty ocupa 9 columnas para unos 70 px de cuerpo. Los límites de pantalla conviene aplicarlos a los pies (el pivote), no a la caja.
- Sin verificar todavía: el límite de 96 sprites por línea. Con muchas columnas en la misma altura (personajes y fondos hechos con sprites) habrá que medirlo.

## Referencias cruzadas

- [Formatos de la ROM C, la ROM S y las paletas](rom-c-s-y-paletas.md)
- [ngdevkit en Windows](../herramientas/ngdevkit-windows.md)
- [Ordenación de sprites](../../GLOSARIO/sorting-sprites.md) · [Punto fijo](../../GLOSARIO/fixed-point.md)
