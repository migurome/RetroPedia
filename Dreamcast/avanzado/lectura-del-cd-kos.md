---
titulo: Leer archivos del CD con KOS — el cuelgue del modo stream de fs_iso9660
plataforma: [Dreamcast]
categoria: [avanzado]
tags: [kos, fs_iso9660, fs_read, cdrom, stream, dma, gd-rom, flycast]
estado: revisado
fecha: 2026-10-31
fuentes: [kernel/arch/dreamcast/fs/fs_iso9660.c y hardware/cdrom.c de KOS (DreamSDK R4), proyecto hyperdbz-dreamcast-port (build 2.020)]
---

# Leer archivos del CD con KOS — el cuelgue del modo stream

## Resumen

`fs_read` sobre `/cd` puede **quedarse colgado para siempre** según el tamaño exacto del archivo. Pasa cuando
el destino está alineado a 32 bytes (lo normal con `memalign(32, ...)`) y el último sector del archivo tiene
**más de 2016 bytes** (y su tamaño no es múltiplo de 32). No hay ningún error ni mensaje. Se evita leyendo el
final del archivo por la caché de sectores de KOS en vez de por el stream.

## Detalle técnico

### Cómo lee `fs_iso9660` (`iso_read`)

- **Destino alineado a 32 y al menos 32 bytes por leer:** empieza un *stream* del CD (`cdrom_stream_start`)
  desde el sector actual hasta el final del archivo, redondeado a sectores enteros. Pide los datos con
  `cdrom_stream_request` en múltiplos de 32 bytes (DMA).
- **Si quedan menos de 32 bytes** con el stream en marcha, pide 32 a un buffer interno y espera con
  `thd_poll(iso_stream_done)`.
- **Si no** (destino sin alinear, o menos de 32 bytes sin stream), lee sector a sector por su caché
  (`bdread`, `cdrom_read_sectors_ex`) y copia.

Si esa última petición de 32 bytes acaba justo en el final del último sector del stream, la espera no termina.
Pasa con un último sector de entre 2017 y 2047 bytes, que no sea múltiplo de 32. Visto en Flycast 2.7 con un
archivo de 882 668 bytes (430 sectores y 2028): `fs_read` del archivo entero, o del último trozo, no volvía.
Con 1772, 1996 o 1088 bytes en el último sector no pasaba. Si también pasa en la consola, aún no se sabe.

### Lo que no era

- **No es el final de la imagen.** Con un archivo de relleno detrás (que se ordena después en la ISO) seguía
  igual. `mkisofs -pad` no cambia nada: la versión de DreamSDK ya deja 150 sectores al final.
- **No es memoria:** el heap estaba bien antes de leer, y un buffer más grande no cambiaba nada.

### Lectura segura

Usar el stream (rápido, por DMA) solo hasta un sector entero que deje al menos 64 bytes detrás. El resto, sector
a sector, a un buffer intermedio **sin alinear a 32**, para que KOS use la caché:

```c
static inline ssize_t cd_read(file_t f, void *buf, size_t size) {
    static uint8_t bounce[2048 + 32] __attribute__((aligned(32)));
    size_t pos = fs_tell(f), total = fs_total(f);
    size_t end = pos + size < total ? pos + size : total;
    size_t safe = total >= 2048 + 64 ? (total - 64) & ~(size_t)2047 : 0;
    uint8_t *out = buf;
    ssize_t got = 0;
    if(pos < safe) {                                  /* el grueso, por el stream */
        size_t n = (end < safe ? end : safe) - pos;
        ssize_t r = fs_read(f, out, n);
        if(r < 0) return r;
        got += r;
        if((size_t)r != n) return got;
        out += r; pos += r;
    }
    while(pos < end) {                                /* el final, sector a sector, sin alinear */
        size_t n = end - pos, left = 2048 - pos % 2048;
        n = n < left ? n : left;
        ssize_t r = fs_read(f, bounce + 1, n);
        if(r <= 0) return got ? got : r;
        memcpy(out, bounce + 1, r);
        got += r; out += r; pos += r;
    }
    return got;
}
```

Detalles:
- **Se lee hasta el final del sector en cada vuelta:** al cruzar de sector el destino seguiría sin alinear
  (`bounce + 1 + 2048`), pero así no depende de eso.
- **Cuesta muy poco:** a lo sumo dos sectores por la caché.

## Notas / gotchas

- **Síntoma:** el programa se queda mudo en una carga, sin error, y depende del tamaño del archivo. Un `.hdl`
  que crece unos pocos KB en una build nueva puede empezar a colgarse. Si pasa, lo primero es mirar
  `tamaño % 2048`.
- **Cómo encontrarlo:** con trazas por el buzón alrededor de cada `fs_read`, y leyendo el archivo a trozos de
  64 KB y luego de 2048 bytes, para ver en qué trozo se para.
- **Los archivos que se leen por partes también:** con `fs_seek` y `fs_read` (por ejemplo, los sonidos uno a
  uno), la lectura que llega al final del archivo tiene el mismo riesgo.

## Referencias cruzadas

- [Imagen de disco autoarrancable (CDI)](../herramientas/imagen-cdi.md)
- [Flycast automatizado — buzón en RAM con Lua](../herramientas/flycast-automatizado.md)
- [Mapa de memoria](../memoria/mapa-de-memoria.md)
