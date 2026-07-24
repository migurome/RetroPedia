---
titulo: SGDK — Tutorial Hello World
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, tutorial, hello-world, vdp, vblank]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/Tuto-Hello-World]
---

# SGDK — Tutorial Hello World

## Resumen

Primer programa funcional en SGDK: dibuja un texto en pantalla y establece la estructura de bucle principal (main loop) que se repetirá en cualquier proyecto SGDK.

## Detalle técnico

### Código completo final

```c
#include <genesis.h>

int main(u16 hard)
{
    VDP_drawText("Hello Genny World!", 10, 13);

    while(TRUE)
    {
        // read input
        // move sprite
        // update score
        // draw current screen (logo, start screen, settings, game, gameover, credits...)

        // wait for screen refresh and do all SGDK VBlank tasks
        SYS_doVBlankProcess();
    }

    return 0;
}
```

### Explicación de las funciones clave

- **`VDP_drawText(<texto>, <x>, <y>)`** — dibuja texto en pantalla. Las coordenadas `x`/`y` están en unidades de tile, donde cada tile equivale a 8 píxeles.
- **`SYS_doVBlankProcess()`** — sincroniza el programa con el ciclo de refresco de pantalla (50 Hz en PAL, 60 Hz en NTSC). Internamente espera al vertical sync antes de ejecutar las tareas de vblank propias de SGDK.
- **`main(u16 hard)`** — punto de entrada del programa; recibe un parámetro de "hard reset" (1 = sistema recién encendido, 0 = pulsación del botón de reset).

### Build y ejecución

El proceso de compilación depende del IDE elegido (Code::Blocks, Eclipse o QtCreator) — ver las guías de integración específicas en la wiki de SGDK (no cubiertas en detalle en esta base). Para el build por línea de comandos, ver [Uso de SGDK](usage.md).

## Notas / gotchas

- El bucle `while(TRUE)` con `SYS_doVBlankProcess()` al final es el patrón estándar de cualquier programa SGDK — los comentarios del ejemplo (leer input, mover sprite, actualizar marcador, dibujar pantalla actual) marcan dónde va la lógica real del juego en proyectos posteriores.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Introducción](tuto-introduction.md)
- [Tutorial: Backgrounds](tuto-background.md)
