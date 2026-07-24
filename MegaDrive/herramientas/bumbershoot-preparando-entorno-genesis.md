---
titulo: Bumbershoot Software — panorama de hardware y elección de herramientas para Genesis/Mega Drive
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [bumbershoot, 68000, z80, asmx, sgdk, gendev, toolchain, hardware-overview]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/02/24/getting-ready-for-genesis-development/]
---

# Bumbershoot Software — panorama de hardware y elección de herramientas para Genesis/Mega Drive

## Resumen

Primera entrada de la serie de desarrollo para Genesis/Mega Drive del blog Bumbershoot Software (2018). Repasa el hardware de la consola a alto nivel, la compara brevemente con la Super Nintendo, y documenta el panorama de herramientas de desarrollo disponibles en la época (SGDK, gendev, asmx, GCC), explicando por qué el autor elige finalmente programar en ensamblador 68000 puro en vez de usar SGDK/C.

## Detalle técnico

### Hardware a grandes rasgos

- CPU principal: Motorola 68000 a 8 MHz, descrito por el autor como "un procesador de 32 bits con bus de datos de 16 bits" — la misma familia que impulsaba Macintosh clásicos, Amiga y Atari ST.
- Coprocesador de sonido: Zilog Z80.
- Espacio de direcciones de 16 MB; los programas pueden llegar a 4 MB sin necesidad de bankswitching.
- Chip gráfico propio (VDP) con varias capas de fondo redimensionables y desplazables de forma independiente, interrupciones raster programables, y soporte de scroll dividido en pantalla (secciones izquierda/derecha independientes).
- Chip de sonido comparado por el autor con el AdLib de DOS, orientado a síntesis (no a audio digital como el de la SNES), lo que permite música de fondo "fire-and-forget" sin intervención continua de la CPU principal.

### Comparación con la competencia

El autor sitúa la Genesis como una máquina con "una cantidad de potencia de cálculo bruta impresionante" para su generación, aunque la describe con la metáfora de "un cortacésped con motor de Lamborghini" (mucha CPU, periféricos gráficos más limitados que otros). Frente a la Super Nintendo destaca que esta última ofrece el doble de RAM y hasta 64 veces más colores simultáneos disponibles.

### Panorama de herramientas de desarrollo

- **SGDK** — kit de desarrollo en C mantenido activamente; opción recomendada para la mayoría de proyectos modernos (https://github.com/Stephane-D/SGDK).
- **gendev** — empaqueta el toolchain de SGDK (GCC + herramientas) para sistemas no-Windows (https://github.com/kubilus1/gendev/).
- **GCC con ensamblador m68k** — alternativa de bajo nivel que el autor descarta señalando la sintaxis de ensamblador "poco agraciada" que expone.
- **asmx** — ensamblador multi-CPU de línea de comandos (http://xi6.com/projects/asmx/), el que finalmente elige el autor.
- **gendev.spritesmind.net** — mencionado como el foro/hub homebrew más cercano a un centro de referencia, aunque el autor lo califica de "escaso, casi al nivel de una página personal" (ver también `spritesmind-overview.md`).
- La documentación oficial de la época se describe como una fuente que "se contradice activamente en varios puntos importantes".
- Se cita la serie de YouTube *Coding Secrets* de GameHut como referencia de técnicas de desarrollo period-accurate.

### Por qué ensamblador puro en vez de SGDK

El autor explica que en proyectos anteriores en otras plataformas mantuvo el principio de que "cada byte de la salida está ahí por orden mía", y decide aplicar el mismo criterio aquí: en vez de apoyarse en las abstracciones de SGDK, opta por escribir 68000 a mano con asmx, asumiendo el coste de resolver él mismo problemas que SGDK ya resuelve (drivers de VDP, gestión de recursos, etc.).

### Programa mínimo y flujo de compilación

El arranque se basa en `sega.s`, descrito como "una versión modificada del código de arranque del compilador Sozobon C". El primer programa de prueba del autor es un bucle infinito trivial:

```
main:   bra.s   main
```

Esta instrucción realiza un salto incondicional a cualquier punto dentro de unos 127 bytes de rango. asmx produce volcados de ROM en formato de texto (Motorola S-Record) que hay que convertir a binario antes de poder cargarlos en un emulador.

Como contraste —mostrado por el propio autor solo como referencia de lo que NO va a usar en el resto de la serie— el mismo "Hello World" con SGDK se reduce a:

```c
#include <genesis.h>

int main()
{
    VDP_drawText("Hello Genesis World!", 9, 13);

    while (1) {
        VDP_waitVSync();
    }
    return 0;
}
```

que compila a un binario de 128 KB.

## Notas / gotchas

- Esta serie de Bumbershoot Software NO usa SGDK: todo el desarrollo posterior es en ensamblador 68000 puro con asmx, a diferencia de la mayoría de tutoriales modernos que recomiendan SGDK como punto de entrada.
- El flujo de asmx (texto → S-Record → binario) añade un paso de conversión que no existe con ensambladores orientados a binario plano como vasm (ver `build-debug-asm-puro.md`).
- La cifra de "64 veces más colores" en la SNES es una simplificación de blog; compárese con el detalle real de paletas y CRAM en `../graficos/tiles-and-palettes.md` y `../graficos/vdp-color-ramp.md` antes de citarla como dato de hardware preciso.

## Referencias cruzadas

- [Big Evil Corporation — puesta en marcha del toolchain](bec-getting-started.md) — misma decisión (ASM puro sin SGDK) tomada en 2012 con un toolchain distinto (ASM68K + MESS).
- [Build y debug de ASM puro para Mega Drive (sin SGDK)](build-debug-asm-puro.md) — comparativa de ensambladores modernos (vasm, ASMX, WLA-DX...) que cita esta misma serie de Bumbershoot Software como fuente indirecta.
- [SpritesMind — resumen](spritesmind-overview.md) — más detalle sobre el foro que este artículo menciona de pasada.
