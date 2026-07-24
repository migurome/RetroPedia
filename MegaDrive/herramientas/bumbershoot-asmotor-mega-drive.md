---
titulo: Bumbershoot Software — probando ASMotor para Mega Drive (2026)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [bumbershoot, asmotor, xlink, wla-dx, asmx, seccion, linker, toolchain]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2026/04/11/mega-drive-development-with-asmotor/]
---

# Bumbershoot Software — probando ASMotor para Mega Drive (2026)

## Resumen

Un año después de modernizar su toolchain de ASMX (ver `bumbershoot-modernizando-toolchain-asmx-2025.md`), el autor prueba **ASMotor** como alternativa: un ensamblador multi-target pensado desde el diseño para producir binarios que mezclan código de varias CPUs a través de su propio linker, en vez de ensamblar cada CPU por separado e incrustar el resultado como un blob binario opaco (justo el enfoque que documentó en la entrada de 2018). Tras probarlo a fondo, concluye que se queda con ASMX para su propio trabajo.

## Detalle técnico

### Pipeline de build con ASMotor

Tres pasos: ensamblar cada CPU con su propio ensamblador (sin mezclar CPUs en el mismo fichero fuente, a diferencia de ASMX 2.0) y enlazar con `xlink`:

```
motor68k -omain.obj main.asm
motorz80 -osnddrv.obj snddrv.asm
xlink -csmd -fsmd -oprogram.bin main.obj snddrv.obj
```

Los flags `-csmd -fsmd` de `xlink` declaran la configuración de memoria y el formato de salida como "Sega Mega Drive", y con eso el linker gestiona automáticamente el tamaño de ROM y el checksum de cabecera (el mismo trabajo que en los toolchains anteriores hacían a mano `srec2smd.py` o `smdfix`).

### Secciones con dirección fija (offset assembly)

```asm
EXPORT driver, driver_size

SECTION "SoundDriver",CODE
driver:
    ORG     $0038
driver_main:
    ;; Code goes here...
driver_end:
driver_size     EQU     driver_end-driver_main
```

`SECTION ... CODE` declara una sección de código; el `ORG` interno fija una dirección concreta para ensamblado con offset. Las etiquetas colocadas antes del `ORG` (como `driver`) resuelven a la dirección real en ROM; las colocadas después resuelven a direcciones virtuales que el linker termina de resolver en la fase de relocación.

### Direcciones de 24 bits cruzando de 68000 a Z80

Ejemplo de driver Z80 anclado a un offset fijo de ROM (80 KB, `$014000`), donde el código Z80 (direccionamiento de solo 16 bits) necesita importar una dirección completa de 24 bits calculada por el linker en el lado 68000:

```asm
EXPORT z80drv, z80drv_size

SECTION "Z80Driver",CODE[$14000]
z80drv:
    ORG $0000
.start:
    ld      hl,z80drv & $ffff
    ld      e,z80drv >> 16
    ld      d,z80drv_size
.lp:    jp      .lp
.end:
z80drv_size EQU z80drv.end-z80drv.start
```

La dirección de 24 bits se parte a mano con operadores de máscara/desplazamiento (`& $ffff` para los 16 bits bajos, `>> 16` para el byte alto), porque el Z80 no tiene forma nativa de manejar un puntero de ese tamaño.

### Comparativa ASMotor vs ASMX vs WLA-DX (según la experiencia del autor)

| Aspecto | ASMX (2.0+) | ASMotor | WLA-DX |
|---|---|---|---|
| Mezclar CPUs en un fichero | Sí, con `CPU` + `RORG`/`REND` en el mismo `.s` (ver `bumbershoot-modernizando-toolchain-asmx-2025.md`) | No directamente: cada CPU va en su propio fichero, con su propio ensamblador (`motor68k`/`motorz80`) | No: exige enlazar objetos de la misma CPU; para mezclar CPUs obliga a "una cadena de builds que se incluyen unos a otros como binarios" |
| Linker | No tiene (composición a nivel de fuente con `INCLUDE`/`INCBIN`) | Linker propio (`xlink`) que resuelve etiquetas entre CPUs guiándose por el formato de salida pedido, no por conocimiento de cada CPU | Requiere encadenar builds propios para lograr algo similar |
| Otras notas | Prefijo `$` para hex en Z80 desactivado por defecto (ver migración de 2025) | — | Soporte extra para SPC700 (relevante para audio en SNES, fuera del alcance de Mega Drive) |

Quirk de sintaxis compartido entre ASMotor y WLA-DX: ninguno de los dos permite el sufijo `.s` para saltos cortos (`bra.s`, etc.), hay que usar `.b` en su lugar — a tener en cuenta si se porta código 68000 escrito para vasm/ASMX.

### Veredicto del autor

Cita textual resumida: "ASMX is still a somewhat better fit for me than ASMotor", aunque reconoce que "ASMotor worked better than WLA-DX". La razón declarada es una preferencia personal por sistemas de ensamblado más simples y de estilo libre ("simpler and more freeform assembly language systems") frente al linker más estructurado de ASMotor.

## Notas / gotchas

- A pesar de probar ASMotor específicamente por su capacidad de mezclar CPUs vía linker, el autor termina prefiriendo el enfoque de ASMX 2.0 (mezcla en un mismo fichero fuente, sin linker) — esta entrada documenta la evaluación, no una migración real del proyecto.
- El manejo manual de direcciones de 24 bits partidas en `& $ffff` / `>> 16` para el lado Z80 es un patrón a recordar si se trabaja con cualquier ensamblador que no ofrezca ayuda automática para direccionamiento cruzado entre CPUs de distinto ancho de bus.
- Esta entrada da por conocido el contexto de las dos anteriores de la serie (`bumbershoot-toolchain-improvisado-2018.md` y `bumbershoot-modernizando-toolchain-asmx-2025.md`); el interés principal aquí es comparativo, no un tutorial de ASMotor desde cero.
- Se solapa parcialmente con la fila de ASMotor ya existente en `bumbershoot-comparativa-toolchains.md` (mismo dato de "linker completo propio" y "mezcla Motorola/Zilog"); esta entrada añade el detalle concreto de sintaxis (`SECTION`/`ORG`/`EXPORT`) y el veredicto práctico tras probarlo, ausentes en aquella comparativa general.

## Referencias cruzadas

- [Bumbershoot Software — modernizando el toolchain de ASMX en 2025 (INCBIN, RORG/REND, smdfix)](bumbershoot-modernizando-toolchain-asmx-2025.md) — el toolchain que el autor decide mantener tras esta evaluación de ASMotor.
- [Bumbershoot Software — el toolchain improvisado de 2018 para Genesis (ASMX + conversión manual)](bumbershoot-toolchain-improvisado-2018.md) — el enfoque original de "CPUs por separado + blob binario opaco" que ASMotor buscaba mejorar.
- [Comparativa de toolchains de ensamblador para Mega Drive](bumbershoot-comparativa-toolchains.md) — ya incluye una fila general sobre ASMotor; esta entrada la complementa con sintaxis concreta y la experiencia práctica del autor.
- [Build y debug de ASM puro para Mega Drive (sin SGDK)](build-debug-asm-puro.md) — menciona ASMotor entre los ensambladores disponibles, sin el detalle de `SECTION`/`xlink` que aporta esta entrada.
