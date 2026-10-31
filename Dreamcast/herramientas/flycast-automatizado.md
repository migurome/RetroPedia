---
titulo: Flycast automatizado — buzón en RAM con Lua y capturas
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [flycast, emulador, lua, pruebas, automatizacion, printwindow, benchmark]
estado: revisado
fecha: 2026-10-30
fuentes: [código de Flycast (core/lua/lua.cpp, core/hw/sh4/modules/serial.cpp, core/log/LogManager.cpp), Flycast 2.7 win64, proyecto hyperdbz-dreamcast-port]
---

# Flycast automatizado

## Resumen

[Flycast](https://github.com/flyinghead/flycast) es el emulador de Dreamcast más usado para desarrollar. Para probar sin intervención (lanzar la imagen, recoger mediciones y cerrar) hay un problema en Windows: **la consola serie no sale por stdout**. La solución es un "buzón" en la RAM emulada que lee un script Lua de Flycast. Comprobado con Flycast 2.7.

## Detalle técnico

### Instalación

- La versión para Windows es **portable**: un zip con `flycast.exe` (`flycast-win64-<versión>.zip` en las *releases* de GitHub). No tiene instalador. Ojo: el botón "Download ZIP" del repositorio baja el **código fuente**, no el emulador.
- Si junto al `.exe` hay un `emu.cfg`, Flycast guarda ahí su configuración y lee `flycast.lua` de esa misma carpeta.
- Arranca la imagen que se le pasa como argumento: `flycast.exe C:\ruta\juego.cdi` (con **ruta absoluta**: con una relativa falla con *Cannot get attributes*). No necesita BIOS, porque emula la BIOS en alto nivel.

### Por qué no basta la consola serie

Con `Debug.SerialConsoleEnabled = yes`, en Windows Flycast llama a `AllocConsole()` y escribe en una ventana nueva ("Flycast Serial Output"), no en la salida estándar del proceso. Redirigir la salida no captura nada.

El log propio sí se puede volcar a archivo (sección `[log]` de `emu.cfg`: `LogToFile = yes`, `Verbosity = 4`, y queda en `flycast.log`). Sirve para ver errores de arranque del emulador, no la salida del programa.

### El buzón

1. El programa reserva una estructura **con una firma conocida** y va añadiendo texto:

```c
typedef struct {
    char magic[16];            /* "HDBZ-MAILBOX-01" */
    volatile uint32_t pos;     /* bytes escritos */
    volatile uint32_t done;    /* 1 = terminado */
    uint32_t size;             /* 65536, sirve para confirmar la firma */
    uint32_t pad;
    char buf[65536];
} mailbox_t;
```

   La firma se inicializa carácter a carácter para que no aparezca otra copia en `.rodata`.

2. Flycast ejecuta `flycast.lua` y en cada *vblank* llama a `flycast_callbacks.vblank`. El script busca la firma en la RAM (`0x8C010000`–`0x8D000000`, con `flycast.memory.readTable32`, que devuelve una tabla **indexada por dirección**), copia el texto nuevo a un archivo con la biblioteca `io` de Lua y, cuando `done == 1`, espera unos segundos y llama a `flycast.emulator.exit()`.

3. Para no molestar al usar el emulador a mano, el script solo actúa si existe una variable de entorno (`HDBZ_LOG`) con la ruta del log.

API de Lua usada: `flycast.memory.read32/readTable8/readTable32`, `flycast.emulator.exit`, los eventos `start`, `vblank` y `terminate`, y las bibliotecas estándar de Lua (`io`, `os.getenv`).

### Pulsar botones desde Lua

`flycast.input.pressButtons(jugador, máscara)` y `flycast.input.releaseButtons(jugador, máscara)`, con el
jugador desde 1, mantienen pulsados los botones hasta soltarlos. Así se puede probar un juego con mando sin
la consola:
1. Un archivo con líneas `vblank máscara` dice qué se mantiene pulsado desde cada vblank.
2. En el evento `vblank`, al llegar a cada línea, se suelta la máscara anterior y se pulsa la nueva.

Las líneas `fin+N máscara` cuentan desde que el programa marca el final en el buzón: sirven para probar la
pantalla final, cuyo momento no se conoce de antemano.

Máscara del mando de Dreamcast: C 1, B 2, A 4, START 8, arriba 16, abajo 32, izquierda 64, derecha 128, Z 256,
Y 512, X 1024. El programa lo lee como cualquier mando (`maple_enum_type` + `maple_dev_status`). Para
comprobar el resultado, el juego escribe en el buzón lo que ha visto (botones y estado del jugador) cada
pocos frames.

### Capturas de pantalla

Desde PowerShell, con `PrintWindow(hwnd, hdc, 3)` (1 = solo área cliente, 2 = `PW_RENDERFULLCONTENT`, necesario para ventanas DirectX) se captura la ventana aunque esté tapada o en otra posición. Antes hay que llamar a `SetProcessDpiAwarenessContext(-4)`: si no, con el escalado de Windows las medidas salen mal y la captura queda desplazada. La ventana del juego se reconoce por su título, `Flycast - <nombre>`.

### La VMU como salida

Flycast emula la VMU sobre un archivo (`data/<Game ID>_vmu_save_A1.bin`, ver
[Maple, mando y VMU](../perifericos/maple-mando-y-vmu.md)). Si el programa guarda ahí sus resultados, el
script que lanza el emulador puede copiar ese archivo al terminar y leerlo. Así se recoge el
[registro de partida](registro-de-partida-y-qr.md) en cada ejecución, sin capturas ni buzón. Para saber cuál
se escribió en la ejecución, se busca el `*_vmu_save_A1.bin` más nuevo que un archivo marcado al arrancar.

## Notas / gotchas

- **Tiempos:** Flycast no emula con exactitud la caché ni la latencia de memoria del SH-4, y **lee el disco al instante** (en una prueba, 290 MB/s). Las mediciones de CPU son orientativas y las de carga no valen: hay que confirmarlas en la consola.
- **El reloj se adelanta al vblank (Flycast 2.7):** medido con `timer_us_gettime64()`, un frame de cada ~60 dura 19,18 ms en lugar de 16,68, aunque el PVR solo cuenta un vblank en ese periodo y el frame siguiente vuelve a durar 16,68 ms. Son ~2,5 ms por segundo de desfase del temporizador emulado. En la consola los dos salen del mismo reloj y no pasa. Si se cuentan como lentos los periodos > 17,5 ms, salen ~1 por segundo que no lo son. Los frames perdidos de verdad se cuentan con el contador de vblanks del PVR: `pvr_get_stats(&st)` y la diferencia de `st.vbl_count` entre dos `pvr_wait_ready()` (más de 1 = vblanks perdidos).
- **Un error del script Lua lo desactiva entero, sin aviso fuera de `flycast.log`** (por ejemplo, `io.lines`
  con una ruta relativa: Flycast se ejecuta desde su carpeta). El emulador sigue hasta el tiempo límite sin
  buzón. Las rutas que se pasan por variables de entorno tienen que ser absolutas (`cygpath -aw`).
- El color de fondo de `pvr_set_bg_color()` salió verde en Flycast 2.7 aunque KOS envía bien el color. Pendiente de comprobar en la consola.
- En la consola real con GDEMU no hay serie: el programa debe mostrar los resultados **también en pantalla** (fuente de la BIOS con `bfont_draw_str_ex` sobre `vram_s` tras `pvr_shutdown()`).

## Referencias cruzadas

- [Imagen de disco autoarrancable (CDI)](imagen-cdi.md)
- [ngdevkit en Windows](../../NeoGeo/herramientas/ngdevkit-windows.md) — también usa PrintWindow para probar en GnGeo
