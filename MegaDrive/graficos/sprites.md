---
titulo: Sprites en el VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, sprites, sprite-table, vram, dma]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/sprites]
---

# Sprites en el VDP

## Resumen

Cubre el formato de almacenamiento de sprites en la Mega Drive, la configuración de la tabla de sprites (Sprite Attribute Table) en el VDP, el formato de cada entrada de 8 bytes, el proceso de actualización por frame y los límites de hardware (número de sprites y sprites por línea).

## Detalle técnico

### Formato de almacenamiento de sprites

Los sprites están constituidos por tiles organizados de forma específica: primero verticalmente, luego horizontalmente. El tamaño puede variar desde 1×1 hasta 4×4 tiles (8×8 a 32×32 píxeles), permitiendo proporciones rectangulares.

### Configuración del VDP

La dirección de la tabla de sprites debe ser múltiplo de `$200` (pantalla 256px) o `$400` (pantalla 320px). Se utiliza el registro VDP `$85xx` donde "bits 15-9 de la dirección se desplazan 9 posiciones a la derecha para los bits 6-0 del registro".

Ejemplo de configuración:

```
SPRITE_ADDR: equ $F000
SetSpriteAddr macro addr
    move.w  #VDPREG_SPRITE|((addr)>>9), (VdpCtrl)
endm
```

### Estructura de la tabla de sprites en RAM

Se requieren:
- Buffer de 8 bytes × máximo número de sprites (640 bytes máximo).
- Variable byte para contar sprites insertados.

### Proceso por frame

1. Limpiar tabla (establecer conteo en 0, llenar primeros 8 bytes con ceros).
2. Agregar cada sprite mediante subrutina.
3. Copiar tabla a memoria de vídeo durante vblank.

### Formato de entrada de sprite (8 bytes)

| Offset | Tamaño | Descripción |
|--------|--------|-------------|
| +0 | 2 bytes | Coordenada Y |
| +2 | 1 byte | Tamaño de sprite |
| +3 | 1 byte | Enlace a siguiente sprite |
| +4 | 2 bytes | ID de tile + flags |
| +6 | 2 bytes | Coordenada X |

#### Coordenadas X e Y

"Posición de la esquina superior-izquierda del sprite en pantalla. Necesita agregar 128 a ambas" para obtener el valor a escribir (ejemplo: posición 40px requiere escribir 168).

#### ID de tile y flags

Rango de 0 a 2047 para el tile base. Se pueden aplicar flags especiales mediante operación OR.

#### Tamaño de sprite

Bits 3-2 definen ancho, bits 1-0 definen alto. Se resta uno al tamaño en tiles. Constantes predefinidas:

```
SPR_1x1:  equ %0000    SPR_4x4:  equ %1111
SPR_2x2:  equ %0101    SPR_3x2:  equ %1001
```

(Completo: 16 combinaciones desde 1×1 hasta 4×4)

#### Enlace de sprites

Cada entrada contiene el número del siguiente sprite más uno, excepto la última que contiene 0. "Slap the sprite number plus one (i.e. 1 for first sprite, 2 for second sprite, etc.), except for the last sprite, where you'll put 0 in it".

### Código de ejemplo: AddSprite

La subrutina acepta parámetros en registros (d0=X, d1=Y, d2=tile+flags, d3=tamaño) y realiza:

- Validación de límites de pantalla.
- Verificación de límite de sprites.
- Cálculo de dirección: `SpriteTable + (NumSprites << 3)`
- Escritura de 8 bytes en el orden especificado.
- Actualización del enlace del sprite anterior.

### Carga de tabla a VRAM

Longitud a copiar:
- 8 bytes si no hay sprites.
- NumSprites × 8 bytes si hay sprites.

Requisito del autor: "Make sure to do this during vblank though (like any large writes you write to video memory)" (asegurarse de hacer esto durante vblank, como cualquier escritura grande a memoria de vídeo).

### Límites de hardware

**Para 320px de ancho:**
- Hasta 80 sprites en pantalla.
- Hasta 20 sprites por línea.
- Hasta 320 píxeles de sprite por línea.

**Para 256px de ancho:**
- Hasta 64 sprites en pantalla.
- Hasta 16 sprites por línea.
- Hasta 256 píxeles de sprite por línea.

### Sprites grandes

La limitación de 32×32 píxeles se supera dividiendo gráficos grandes en múltiples sprites menores, reutilizando gráficos cuando sea posible.

## Notas / gotchas

- Advertencia sobre la caché de sprites: "Beware that there's a sprite cache. This cache holds the Y coordinate as well as sprite sizes and their order, but not the X coordinate and tile number + flags" (la caché de sprites guarda la coordenada Y, el tamaño y el orden, pero no la coordenada X ni el tile+flags). Esta caché se vacía al escribir en la tabla de sprites. Cambiar la dirección de la tabla sin reescribir los datos causa mezcla de datos antiguos y nuevos.
- Algunos juegos explotan este comportamiento de la caché deliberadamente (se menciona Castlevania Bloodlines para efectos de reflexión).
- Los sprites deben copiarse a VRAM durante el vblank, igual que cualquier escritura grande a memoria de vídeo.

## Referencias cruzadas

- (pendiente de enlazar)
