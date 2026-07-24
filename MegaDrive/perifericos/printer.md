---
titulo: Interfaz de impresora en Mega Drive
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [impresora, printer, esc-p, mega-anser, io, cmyk]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/printer]
---

# Interfaz de impresora en Mega Drive

## Resumen

Esta entrada documenta el protocolo de comunicación con la impresora usada junto al Mega Anser, incluyendo detección, transferencia de bytes, comandos ESC/P soportados e impresión de imágenes monocromáticas y en color (CMYK). El propio autor advierte que la información proviene de ingeniería inversa del Mega Anser y no ha podido verificarse con el adaptador de impresora real.

## Detalle técnico

### Disclaimer del autor

"the information below comes from reverse engineering Mega Anser and hasn't been checked with the printer adapter." La verificación con hardware real es actualmente imposible.

### Detección y configuración

ID de periférico: la impresora tiene ID 10 (`%1010`). Se detecta mediante verificación del ID de periférico.

Configuración de pines:
```
FastPauseZ80
move.b  #$07, (IoCtrl2)
move.b  #$07, (IoData2)
ResumeZ80
```
Establece los bits 2-0 como salida, el resto como entrada. La impresora se conecta al puerto dos del controlador.

### Protocolo de comunicación

Verificación de disponibilidad:
```
btst    #3, (a0)
bne     @NotReady
```
Si el bit 3 = 1, la impresora no puede recibir datos.

Escritura de byte (transferencia bit a bit):
```
moveq   #8-1, d7
@Loop:
add.b   d0, d0
addx.b  d1, d1
and.b   #$01, d1
or.b    #$FC, d1
move.b  d1, (a0)
nop
nop
or.b    #$FE, d1
move.b  d1, (a0)
nop
nop
dbf     d7, @Loop
```

Confirmación de byte:
```
move.b  #$FB, (a0)
nop
nop
move.b  #$FF, (a0)
nop
nop
ResumeZ80
```

### Comandos ESC/P soportados

| Comando | Código Hex | Función |
|---------|-----------|---------|
| NUL | 00 | Ignorar |
| CR | 0D | Retorno de línea |
| LF | 0A | Avance de línea |
| ESC A | 1B 41 nn | Altura de línea (nn = 1/60 pulgadas) |
| ESC L | 1B 4C nn mm ... | Gráficos de mapa de bits (120x60 dpi, 8px de altura) |

### Impresión de imagen monocromática

Requisitos:
- Monocromática obligatoriamente.
- Ancho: máximo 640px.
- Altura: múltiplo de 8px.
- Píxeles rectangulares (ancho = mitad de la altura).

Secuencia:
```
print 1B 41 08 0D 0A
for every 8px slice
   print 1B 4C
   print (width & 0xFF)
   print (width >> 8)
   for every column
      byte = (row[0] << 7) |
             (row[1] << 6) |
             (row[2] << 5) |
             (row[3] << 4) |
             (row[4] << 3) |
             (row[5] << 2) |
             (row[6] << 1) |
             (row[7] << 0)
      print byte
   end
   print 0D 0A
end
print 0C
```

### Impresión en color (CMYK)

Códigos ESC r:
| Valor | Color |
|-------|-------|
| 0 | Negro |
| 1 | Magenta |
| 2 | Cian |
| 3 | Violeta |
| 4 | Amarillo |
| 5 | Rojo |
| 6 | Verde |

Orden de impresión por slice (crítico contra manchas):
1. Amarillo: `1B 72 04`
2. CR: `0D`
3. Cian: `1B 72 02`
4. CR: `0D`
5. Magenta: `1B 72 01`
6. CR: `0D`
7. Negro: `1B 72 00`
8. CR: `0D`
9. LF: `0A`

### Conversión RGB a CMYK

RGB a CMY:
```
cyan = 100% - red
magenta = 100% - green
yellow = 100% - blue
```

Agregar K (negro):
```
1. Convertir RGB a CMY
2. k = mín(cyan, magenta, yellow)
3. cyan -= k; magenta -= k; yellow -= k
4. black += k
```

## Notas / gotchas

- La información de esta página proviene de ingeniería inversa del Mega Anser y no ha sido verificada contra el adaptador de impresora real, según el propio autor.
- El orden de impresión de colores en modo CMYK es crítico para evitar manchas (amarillo, cian, magenta, negro, en ese orden).

## Referencias cruzadas

- (pendiente de enlazar)
