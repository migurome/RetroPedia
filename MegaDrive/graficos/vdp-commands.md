---
titulo: Referencia de comandos del VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, referencia, registros, vram, cram, vsram, dma]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/vdp-commands]
---

# Referencia de comandos del VDP

## Resumen

Esta entrada documenta el formato exacto de los valores que se pueden escribir en el puerto de control del VDP (`$C00004`) para cuatro operaciones básicas: configurar un registro, establecer una dirección de escritura en memoria de vídeo, establecer una dirección de lectura, e iniciar una transferencia DMA. Es la referencia base para cualquier código que toque VRAM, CRAM o VSRAM.

## Detalle técnico

### Set register (configurar un registro)

Escribir en un registro del VDP se hace escribiendo un valor de 16 bits (una sola palabra) en `$C00004` (`VdpCtrl`) con este formato:

```
$8000 OR (register << 8) OR value
```

Para la lista completa de registros y sus valores, ver la referencia de registros del VDP (`vdp-registers`).

**Advertencia del autor:** "Setting a VDP register will _unset_ the current address, so if you need to keep writing or reading video memory then you'll need to set the address again." (Configurar un registro del VDP anula la dirección actual, así que si necesitas seguir escribiendo o leyendo memoria de vídeo tendrás que volver a establecer la dirección).

### Set write address (establecer dirección de escritura)

Para escribir en memoria de vídeo hay que escribir un valor de 32 bits en `$C00004` (`VdpCtrl`), ya sea como longword o como dos palabras separadas. Los bits 13-10 de la dirección van en la palabra alta, los bits 15-14 van en los bits 1-0 de la palabra baja:

```
((address AND $3FFF) << 16) OR
((address AND $C000) >> 14) OR
command
```

El `command` es uno de los siguientes (tabla "Commands to set a video address for writing"):

| Memory space | OR with |
|---|---|
| Write VRAM address | `$40000000` |
| Write CRAM address | `$C0000000` |
| Write VSRAM address | `$40000010` |

Tras esto, los datos se escriben en `$C00000` (`VdpData`), en palabras o longwords (estos últimos se tratan como dos palabras). Tras cada palabra escrita la dirección se incrementa automáticamente según la cantidad configurada en el autoincremento (registro `$8Fxx`, ver `vdp-registers#8Fxx`).

**Nota del autor (Iwis):** en el caso del modo de 128KB de VRAM (en máquinas que lo soportan, p. ej. Teradrive) los bits 16-14 de la dirección van en los bits 2-0 de la palabra baja (como cabría esperar). Todo lo demás es idéntico.

### Set read address (establecer dirección de lectura)

Leer de memoria de vídeo es similar a escribir; la diferencia principal es el `command` (tabla "Commands to set a video address for reading"):

| Memory space | OR with |
|---|---|
| Read VRAM address | `$00000000` |
| Read CRAM address | `$00000020` |
| Read VSRAM address | `$00000010` |

Tras esto, los datos se leen de `$C00000` (`VdpData`), en palabras o longwords (estos últimos vistos como dos palabras). Tras cada palabra leída la dirección se incrementa automáticamente según el autoincremento.

### Start DMA (iniciar DMA)

Configurar la dirección de destino de DMA es similar a establecer una dirección de escritura, con la única diferencia siendo el `command` usado (de hecho son el mismo valor salvo por el bit 7 de la segunda palabra del comando) (tabla "Commands to set DMA destination address"):

| Memory space | OR with |
|---|---|
| DMA to VRAM address | `$40000080` |
| DMA to CRAM address | `$C0000080` |
| DMA to VSRAM address | `$40000090` |

En el caso de DMA transfer o DMA copy, la operación empieza inmediatamente tras escribir este valor. En el caso de DMA fill, hay que escribir el valor de relleno en `$C00000` (`VdpData`) después de esto para iniciar el DMA fill.

## Notas / gotchas

- Configurar un registro (`Set register`) anula la dirección de VRAM/CRAM/VSRAM actualmente establecida; hay que volver a fijarla si se necesita seguir leyendo/escribiendo.
- El modo de 128KB de VRAM (Teradrive) cambia la posición de los bits altos de la dirección (van en bits 2-0 de la palabra baja en vez de bits 1-0).
- DMA fill requiere un paso adicional: escribir el valor de relleno en `VdpData` tras configurar la dirección de destino; DMA transfer/copy arrancan de inmediato.

## Referencias cruzadas

- (pendiente de enlazar)
