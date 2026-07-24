---
titulo: Comparativa de toolchains de ensamblador para Mega Drive (SGDK, ASMX, Vasm, WLA-DX, ASMotor)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, asmx, vasm, wla-dx, asmotor, ensamblador, toolchain, referencia-indirecta]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/platform-guide-sega-genesis-mega-drive/]
---

# Comparativa de toolchains de ensamblador para Mega Drive (SGDK, ASMX, Vasm, WLA-DX, ASMotor)

## Resumen

Segunda mitad de la sección de configuración de entorno de la guía de plataforma de Bumbershoot Software: un repaso de las opciones para programar en 68000/Z80 (más SGDK como opción en C), con las preferencias personales del autor y por qué eligió Vasm para los ejemplos de su propia guía.

## Detalle técnico

| Herramienta | Qué es | Observaciones del autor |
|---|---|---|
| **SGDK** | Cross-compilador basado en GCC más una biblioteca estándar muy completa para interactuar con el hardware. | Incluye Sjasm para programar el Z80 directamente (un ensamblador que merece la pena incluso combinado con otras toolchains). Es Windows-only; el proyecto GenDev ofrece procedimientos para montar un entorno equivalente en otras plataformas. |
| **ASMX** | Ensamblador Motorola que el autor usó en su trabajo inicial sobre la plataforma. | Descrito como "primitivo en algunos aspectos" frente a las alternativas, pero el proyecto ha sido adoptado por otros y sigue activo en GitHub; la continuación moderna permite mezclar código m68k y Z80 en el mismo fichero fuente. |
| **Vasm** | Ensamblador del toolkit VBCC, el mismo que el autor recomienda para desarrollo en Amiga y Atari ST. | Incluye ensamblador Z80 además del Motorola, por lo que puede servir como toolkit casi completo por sí solo. Lo único que le falta es calcular automáticamente el checksum de los metadatos del cartucho. Aviso de licencia: como en el caso de Atari ST, la licencia de Vasm solo permite uso no comercial por defecto. Es el ensamblador que el autor usa para los ejemplos de su guía (paquete "multi-target binaries of vbcc/vasm/vlink", añadiendo su `bin/` al PATH). |
| **WLA-DX** | Incluye ensambladores Motorola y Z80 con directivas adicionales específicas para Genesis. | Su dialecto de ensamblador m68k difiere ligeramente de lo que el autor está acostumbrado a usar, y combinado con particularidades en las directivas de linkado del fichero final, es la opción que menos le gusta. Aun así, la considera una solución completa y potente. |
| **ASMotor** | Ensamblador multi-target del creador del Rednex Game Boy Development System; incluye salidas específicas para Genesis. | Como ASMX, permite mezclar ensamblador Motorola y Zilog en el mismo proyecto; a diferencia de ASMX, lo gestiona con un linker completo propio. |

### Comando de ensamblado mínimo con Vasm

```
vasmm68k_mot -Fbin -o hello.bin hello.asm
```

Genera un binario plano ejecutable directamente en cualquiera de los emuladores mencionados en la comparativa de emuladores (ver referencia cruzada), sin pasos de linkado adicionales para un programa tan simple como el "Hello World" de la propia guía.

## Notas / gotchas

- Esta comparativa se solapa parcialmente con la ya documentada en `build-debug-asm-puro.md` (misma fuente: la guía de plataforma de Bumbershoot Software). Esa entrada ya cubre vasm, ASMX, ASMotor, WLA-DX y ASM68K/AS con foco en el flujo de build completo (estructura de ROM, checksum, herramientas de debug). Esta entrada aporta el matiz que falta allí: **por qué** el autor prefiere Vasm sobre las demás (experiencia personal con cada una) y el detalle de que SGDK incluye Sjasm como ensamblador Z80 aprovechable incluso fuera de un proyecto SGDK puro.
- Aviso de licencia de Vasm: uso no comercial por defecto (igual que para Atari ST) — revisar los términos si el proyecto es comercial.
- Vasm no calcula automáticamente el checksum de cabecera; hay que generarlo aparte (ver `../memoria/rom-header.md` para el formato del campo de checksum).

## Referencias cruzadas

- [Build y debug de ASM puro para Mega Drive (sin SGDK)](build-debug-asm-puro.md) — comparativa de ensambladores y herramientas de debug ya existente sobre la misma fuente; esta entrada la complementa, no la sustituye.
- [Comparativa de emuladores para desarrollo en Mega Drive/Genesis](bumbershoot-emuladores-desarrollo.md)
- [Hello World anotado (código máquina completo)](../cpu/bumbershoot-hello-world-anotado.md) — el programa que se ensambla con el comando de Vasm mostrado arriba.
- [Cabecera de la ROM](../memoria/rom-header.md)
