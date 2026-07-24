---
titulo: Taller de ensamblador Mega Drive y ejemplo "hello world" (2018) — Big Evil Corporation
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [bigevilcorporation, tanglewood, taller, hello-world, asm68k, recursos-educativos]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2018/03/03/mega-drive-assembly-workshop-and-hello-world-sample/]
---

# Taller de ensamblador Mega Drive y ejemplo "hello world" (2018) — Big Evil Corporation

## Resumen

Entrada breve de Matt (Big Evil Corporation, autor de TANGLEWOOD), publicada el 03/03/2018, anunciando un taller presencial de programación en ensamblador para Mega Drive impartido en el National Videogame Arcade, junto con un paquete de recursos descargable y un ejemplo mínimo de "hello world" en un repositorio público. A diferencia del resto de artículos de esta lista, esta página no contiene código incrustado ni desarrollo técnico propio: es un anuncio con enlaces a material externo.

## Detalle técnico

### Contenido del taller

Diapositivas (alojadas en Google Slides) de un taller práctico centrado en llevar paso a paso desde cero hasta mostrar "Hello World" en pantalla, con extensiones sobre gestión de memoria y sprites. Acompañado de un paquete de recursos descargable (`md_workshop.zip`, alojado en MediaFire) pensado para complementar esas diapositivas durante la sesión presencial.

### Ejemplo hello world

Repositorio GitHub `BigEvilCorporation/megadrive_samples`, carpeta `hello_world`: un ejemplo de un único archivo fuente, descrito por el autor como revisado por pares y probado en hardware real. Se compila con el mismo ensamblador que el resto de la serie:

```
asm68k.exe /p hello.asm,hello.bin
```

### Público objetivo

Programadores que quieran arrancar rápido en desarrollo para Mega Drive sin necesidad de asistir al taller presencial.

## Notas / gotchas

- **Contenido mayormente redundante**: esta página no aporta código ni explicación técnica nueva respecto a lo que ya cubre la serie numerada del propio autor (en particular la primera entrada de la serie, "Sega Megadrive – 1: Getting Started", que sienta las bases de toolchain y hello world) ni respecto a lo ya documentado en esta base en [`build-debug-asm-puro.md`](build-debug-asm-puro.md) (ensambladores, proceso de build) y en el tutorial equivalente de SGDK ([`sgdk/tuto-hello-world.md`](sgdk/tuto-hello-world.md), con enfoque en C en vez de ASM puro). Su valor real está en los enlaces a recursos externos (diapositivas, zip de recursos, repositorio de ejemplo), no en contenido técnico propio de la página.
- El propio autor admite en el artículo estar centrado en terminar su juego (TANGLEWOOD) y promete tutoriales más completos "en el futuro" — es una entrada de mantenimiento/comunidad, no un artículo técnico de la serie principal.
- No se pudo extraer ningún bloque de código propio de esta página más allá del comando de compilación del ejemplo; el código real vive en el repositorio de GitHub enlazado, fuera del alcance de este fetch.

## Referencias cruzadas

- [`build-debug-asm-puro.md`](build-debug-asm-puro.md) — cubre en detalle los ensambladores y el flujo de build para ASM puro sin SGDK; esta entrada solo lo menciona de pasada (mismo ensamblador, `asm68k.exe`).
- [`sgdk/tuto-hello-world.md`](sgdk/tuto-hello-world.md) — el "hello world" equivalente pero en C con SGDK, útil como contraste del mismo objetivo con dos toolchains distintas.
