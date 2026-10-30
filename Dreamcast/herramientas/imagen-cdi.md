---
titulo: Imagen de disco autoarrancable (CDI) — 1ST_READ.BIN, IP.BIN, mkisofs y cdi4dc
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [cdi, ip.bin, 1st_read.bin, scramble, mkisofs, cdi4dc, gdemu, selfboot]
estado: revisado
fecha: 2026-10-30
fuentes: [KOS utils/makeip (field.c, crc.c, rsrc/templates/ip.tmpl), KOS utils/scramble, cdi4dc 0.3b, proyecto hyperdbz-dreamcast-port]
---

# Imagen de disco autoarrancable (CDI)

## Resumen

Para que un programa de KOS lea datos de `/cd` (como hará en la consola) hay que meterlo en una imagen de disco. Cargar el `.elf` directamente en el emulador no sirve: no hay disco y un *romdisk* se carga entero en RAM, lo que falsea las mediciones de memoria. El proceso tiene cuatro pasos y todas las herramientas vienen con DreamSDK. La misma imagen CDI vale para Flycast, para un CD-R y para GDEMU.

## Detalle técnico

```sh
sh-elf-objcopy -R .stack -O binary juego.elf juego.bin           # 1. ELF -> binario plano
scramble.exe juego.bin cd/1ST_READ.BIN                            # 2. "desordenar" para el arranque desde CD
python make_ip.py ip.tmpl IP.BIN "TITULO" "V0.001"                # 3. bootstrap con la cabecera
mkisofs -C 0,11702 -V JUEGO -G IP.BIN -r -J -l -quiet -o juego.iso cd/   # 4a. ISO de la 2.ª sesión
cdi4dc juego.iso juego.cdi                                        # 4b. CDI audio+datos
```

1. **objcopy**: el `.elf` se convierte en binario plano sin la sección `.stack`.
2. **scramble**: la BIOS de la Dreamcast, al arrancar desde un CD normal (no GD-ROM), espera el `1ST_READ.BIN` "desordenado". Se usa `utils/scramble` de KOS.
3. **IP.BIN** (32 KB): una cabecera de texto de 256 bytes seguida del código de arranque. KOS trae la plantilla libre `utils/makeip/rsrc/templates/ip.tmpl`.
4. **mkisofs -C 0,11702**: crea la sesión de datos empezando en el sector 11702, el estándar para CD-R audio+datos (*selfboot*). `-G` mete el `IP.BIN` en los primeros sectores. El aviso `-C specified without -M` es normal. Después, **cdi4dc** lo empaqueta en formato DiscJuggler.

### Campos de IP.BIN

`makeip.exe` de DreamSDK R4 no arranca, pero es fácil generarlo en Python a partir de la plantilla, con los mismos campos que `makeip` (`src/field.c`):

| Offset | Long. | Campo | Ejemplo |
|---|---|---|---|
| 0x00 | 16 | Hardware ID (fijo) | `SEGA SEGAKATANA ` |
| 0x10 | 16 | Maker ID (fijo) | `SEGA ENTERPRISES` |
| 0x20 | 16 | Device info: CRC + medio | `XXXX CD-ROM1/1` |
| 0x30 | 8 | Áreas | `JUE` |
| 0x38 | 8 | Periféricos | `E000F10` |
| 0x40 | 10 | Nº de producto | `T-00000` |
| 0x4A | 6 | Versión | `V0.001` |
| 0x50 | 16 | Fecha | `20261030` |
| 0x60 | 16 | Ejecutable de arranque | `1ST_READ.BIN` |
| 0x70 | 16 | Compañía | `KallistiOS` |
| 0x80 | 128 | Título | `HYPER DBZ 0.001` |

Los campos se rellenan con espacios. Los 4 primeros caracteres de *Device info* son el CRC en hexadecimal (mayúsculas) de los 16 bytes desde 0x40 (producto + versión):

```python
def crc(buf):                       # igual que utils/makeip/src/crc.c
    n = 0xFFFF
    for b in buf:
        n ^= b << 8
        for _ in range(8):
            n = ((n << 1) ^ 4129) if n & 0x8000 else (n << 1)
    return n & 0xFFFF

ip[0x20:0x24] = f'{crc(ip[0x40:0x50]):04X}'.encode()
```

## Notas / gotchas

- KOS lee la ISO9660 sin distinguir mayúsculas: `fs_open("/cd/gokuz2.hdb")` encuentra `GOKUZ2.HDB`.
- Para versionar builds conviene poner la versión en el campo 0x4A, en el título y en el nombre del archivo (`juego.0.001.cdi`), además de dentro del propio programa.
- La imagen de 25 MB con ~7 MB de datos tarda unos segundos en generarse; `mkisofs` y `cdi4dc` no son el cuello de botella.

## Referencias cruzadas

- [DreamSDK R4 en Windows](dreamsdk-windows.md)
- [Flycast automatizado](flycast-automatizado.md)
