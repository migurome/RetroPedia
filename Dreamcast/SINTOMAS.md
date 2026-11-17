---
titulo: Dreamcast — síntomas y causas
plataforma: [Dreamcast]
categoria: [referencia]
tags: [depuracion, emuladores, kos]
estado: borrador
comprobado: emulador
comprobado_en: "parcial: Flycast 2.7 y DreamSDK R4, hyperdbz-dreamcast-port; solo las filas sin marca"
fecha: 2026-11-06
fuentes: [Dreamcast/herramientas/dreamsdk-windows.md, Dreamcast/avanzado/lectura-del-cd-kos.md, Dreamcast/graficos/cache-sprites-vram.md, Dreamcast/memoria/mapa-de-memoria.md, Dreamcast/herramientas/flycast-automatizado.md]
---

# Dreamcast — síntomas y causas

Las filas sin marca pasaron en `hyperdbz-dreamcast-port`, en Flycast o al compilar. **(doc)** significa que viene de la
documentación y no lo hemos visto. Nada de esto está comprobado todavía en la consola.

## Se cuelga o no carga

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Se queda mudo en una carga, sin error; empieza tras crecer un archivo | Cuelgue del modo stream de `fs_iso9660` con un último sector de 2017–2047 bytes no múltiplo de 32 | Mirar `tamaño % 2048`; leer a trozos de 64 KB y luego de 2048 con trazas | [lectura del CD](avanzado/lectura-del-cd-kos.md) |
| Excepción al leer una estructura binaria del disco | Acceso desalineado de 16/32 bits | Revisar la alineación de los campos | [overview](README.md) |
| Un script de Flycast deja de hacer efecto y el emulador sigue hasta el tiempo límite | Error de Lua, que lo desactiva sin aviso; a menudo una ruta relativa | Mirar `flycast.log`; usar rutas absolutas | [Flycast automatizado](herramientas/flycast-automatizado.md) |

## Se ve mal

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Al pintar en VRAM salen unos valores y otros no (el blanco sí, el negro no) | GCC convierte el bucle en `memset`, que escribe byte a byte, y esas escrituras se pierden | Usar `volatile uint16_t *` y comparar | [mapa de memoria](memoria/mapa-de-memoria.md) |
| Un polígono transparente no aparece | Se envió a la lista OP **(doc)** | Revisar la lista y el orden OP, TR y PT | [PowerVR](graficos/powervr-pipeline.md) |
| Colores siempre a 0 o a 1 | División entera `a / 255` sobre `uint8` **(doc)** | Usar `255.0f` | [PowerVR](graficos/powervr-pipeline.md) |
| Sprite deformado en modo sprite | UV de 16 bits sin alinear **(doc)** | Alinear posiciones o usar polígonos | [texturas](graficos/texturas-paletas-y-sprites.md) |
| Texturas corruptas tras liberar | Se libera una textura usada en el frame anterior; hay 2 de latencia **(doc)** | Retrasar la liberación 2 frames | [PowerVR](graficos/powervr-pipeline.md) |
| Datos viejos tras un DMA | Caché sin vaciar antes del DMA, o sin invalidar después **(doc)** | Vaciar o invalidar (`ocbi`) y comparar | [mapa de memoria](memoria/mapa-de-memoria.md) |
| `pvr_set_bg_color()` sale verde | Sin aclarar; visto en Flycast 2.7, aunque KOS envía bien el color | Probar en la consola | [Flycast automatizado](herramientas/flycast-automatizado.md) |

## Va lento

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Picos de cientos de ms al cargar sprites | Se espera a que `pvr_mem_malloc` falle, y cada fallo escribe un aviso por serie | Llevar presupuesto y expulsar antes de reservar | [caché de sprites](graficos/cache-sprites-vram.md) |
| Frames de cientos de ms con VRAM libre | Fragmentación del asignador de KOS con tamaños variados | Contar reservas fallidas; usar un buddy propio | [caché de sprites](graficos/cache-sprites-vram.md) |
| Falta VRAM para los sprites tras cargar el escenario | Escenario subido en trozos o después de la caché | Subirlo antes, en un solo bloque | [escenarios 2D](graficos/escenarios-2d-paletas-y-baldosas.md) |
| Frames lentos con pocos polígonos | Demasiada área en lista TR **(doc)** | Sumar el área dibujada; pasar fondos a OP y sprites a PT | [PowerVR](graficos/powervr-pipeline.md) |

## La medida engaña

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Un frame de cada ~60 parece durar 19 ms | En Flycast 2.7 el reloj se adelanta al vblank | Contar con los vblanks del PVR | [Flycast automatizado](herramientas/flycast-automatizado.md) |
| Las cargas parecen instantáneas | Flycast lee el disco al instante (290 MB/s en una prueba) | Medir en la consola | [Flycast automatizado](herramientas/flycast-automatizado.md) |
| Un frame de 16 ms pasa a 24 al activar trazas | `printf` por consola o buzón dentro del frame | Quitar las trazas o acumularlas | [caché de sprites](graficos/cache-sprites-vram.md) |
| El peor tick de una prueba cambia sin tocar su código | Una escritura en VMU anterior cambia el estado de la memoria | Mover la escritura al final o fuera de la medida | [registro de partida](herramientas/registro-de-partida-y-qr.md) |
| La huella del replay no coincide entre PC y Dreamcast | Libm, `fsin`/`fcos` o `(int)` fuera de rango en la lógica | Comparar la huella por tramos | [coma flotante](cpu/coma-flotante-determinista.md) |
| La captura es de otro juego | Hay otro Flycast abierto, y la ventana se buscó por título | Buscar por el PID lanzado; `Get-Process flycast` antes de empezar | [Flycast automatizado](herramientas/flycast-automatizado.md) |

## Sonido y mandos

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Aviso en `snd_sfx_load_raw_buf` | Efecto de más de 65.534 muestras | Comprobar la longitud; usar `snd_stream_*` | [ADPCM](sonido/aica-adpcm.md) |
| No caben los sonidos en la RAM de audio | Se cargan todos | Sumar tamaños; cargar bajo demanda | [ADPCM](sonido/aica-adpcm.md) |
| El mando del jugador 2 lee el del jugador 1 | `maple_enum_type(0, ...)` da el primero que encuentra **(doc)** | Usar `maple_enum_dev(puerto, unidad)` | [Maple y VMU](perifericos/maple-mando-y-vmu.md) |

## Al compilar (DreamSDK en Windows)

| Síntoma | Causa | Solución | Entrada |
|---|---|---|---|
| `make` falla con `Error 203` | `kos-cc.exe` exige `DREAMSDK_HOME` con ruta de Windows | Exportarla después de `environ.sh` | [DreamSDK](herramientas/dreamsdk-windows.md) |
| `Cannot create temporary file in C:\Windows\` | Falta `TMP`/`TEMP` | Apuntarlas a `C:\DreamSDK\tmp` | [DreamSDK](herramientas/dreamsdk-windows.md) |
| Un port no encuentra su propia cabecera | Los lanzadores `kos-*.exe` pierden los `KOS_CFLAGS` | Poner antes en el `PATH` los scripts originales | [DreamSDK](herramientas/dreamsdk-windows.md) |
| `makeip.exe` no arranca o se queda colgado | Falta una DLL | Generar `IP.BIN` con un script propio | [imagen CDI](herramientas/imagen-cdi.md) |
| La compilación no empieza, sin errores | `kos-cc -dumpversion` se cuelga al cargar `environ.sh` | Ver la solución en la entrada | [DreamSDK](herramientas/dreamsdk-windows.md) |
| Un cambio en un `.h` generado no surte efecto | Los makefiles de KOS no siguen las cabeceras | Borrar los `.o` que lo usan | [DreamSDK](herramientas/dreamsdk-windows.md) |

MUGEN tiene sus propias trampas: ver [compilar el CNS](../MOTORES/mugen/cns-compilacion.md) y [física y golpes](../MOTORES/mugen/fisica-y-golpes-del-motor.md).
