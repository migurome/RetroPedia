---
titulo: Formatos de archivo de guardado (SRM y savestates)
plataforma: [SNES]
categoria: [memoria]
tags: [save, srm, sram, savestate, emuladores]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Save_file_formats]
---

# Formatos de archivo de guardado (SRM y savestates)

## Resumen

Los emuladores de SNES manejan dos tipos de archivo de "guardado" bien distintos: el volcado de la RAM con batería del cartucho (formato `.srm`, estandarizado de facto entre emuladores) y los savestates, que capturan el estado completo del sistema en un instante dado pero cuyo formato interno no está estandarizado y depende de cada emulador. Esta página de la wiki es breve y se limita a estas dos secciones.

## Detalle técnico

### Save RAM (archivos `.srm`)

El formato `.srm` es, literalmente, un volcado simple de la RAM con respaldo de batería (battery-backed SRAM) del cartucho. El tamaño de esa RAM lo indica la propia cabecera de la ROM (ver `rom-header`, campo de tamaño de RAM en `$FFD8`). El archivo consiste en un volcado lineal de la SRAM empezando por su dirección más baja según el mapa de memoria del cartucho correspondiente.

Características principales:

- Compatibilidad prácticamente universal entre emuladores (es el formato de facto para guardado de partida).
- Se puede transferir hacia/desde un cartucho original con hardware apropiado (programadores/copiadores de flashcart).
- Los emuladores suelen guardarlo automáticamente al cerrar el juego (si la cabecera de la ROM indica que existe RAM con batería), y muchos también guardan periódicamente durante la partida si detectan cambios en los datos.

### Savestates

Un savestate captura el estado completo de la SNES en un momento dado, permitiendo reanudar la ejecución instantáneamente desde ese punto. Lo soportan muchos emuladores y algunas flashcarts.

- **No existe un formato estandarizado** para savestates: cada emulador define el suyo, y en la práctica suele ser, simplemente, un volcado del estado interno de ese emulador concreto (registros de CPU/PPU/APU, contenido de WRAM/VRAM/OAM/CGRAM, etc., en el layout que le resulte cómodo a esa implementación).
- Como referencia de un formato concreto y decodificable, la wiki menciona el código fuente `zstate.c` del emulador ZSNES.

## Notas / gotchas

- No asumir compatibilidad de savestates entre emuladores distintos (ni siquiera entre versiones del mismo emulador) — al no haber formato estándar, cada uno serializa su propio estado interno a su manera.
- El tamaño esperado de un archivo `.srm` depende directamente del campo de tamaño de RAM de la cabecera de la ROM; un archivo `.srm` de tamaño incorrecto (por ejemplo, de una ROM sin cabecera parcheada) puede no cargar bien en todos los emuladores.
- Esta página de la wiki es deliberadamente escueta (solo dos secciones, Save RAM y Savestates); no documenta variantes de `.srm`, datos de RTC anexados al archivo de guardado, ni formatos de guardado específicos de coprocesadores (DSP, Cx4, Super FX, etc.).

## Referencias cruzadas

- (pendiente de enlazar)
