---
titulo: Copiers de cartuchos SNES
plataforma: [SNES]
categoria: [perifericos]
tags: [copier, roms, dumping, backup-unit, smc, swc]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Copier]
---

# Copiers de cartuchos SNES

## Resumen

Durante la vida comercial de SNES existió una amplia variedad de dispositivos "Copier": aparatos de terceros que permitían volcar (dump) el contenido de cartuchos a un ordenador y, a la inversa, ejecutar esos volcados desde una unidad conectada a la consola simulando un cartucho. Estos dispositivos son el origen histórico de los formatos de archivo ROM de SNES que siguen usándose hoy en emuladores.

## Detalle técnico

### Formatos de archivo asociados

La página de referencia asocia explícitamente estas extensiones a copiers concretos:

- `.SMC` — Super Magicom
- `.SWC` — Super Wild Card
- `.FIG` — Super Wild Card

### Origen del header de 512 bytes

Estos dispositivos son responsables de los headers adicionales de 512 bytes que aparecen en muchos archivos ROM de SNES: dicho header no forma parte del cartucho original, sino que lo añadía el propio copier al volcar la ROM, y hoy sigue apareciendo en muchos dumps en circulación.

### Parches "trainer"

Algunos de estos dispositivos empleaban parches de tipo "trainer" aplicados a la ROM, ya fuera para hacer compatible un juego concreto con las limitaciones del copier, o para añadir trucos u otras modificaciones pensadas para el jugador.

## Notas / gotchas

- La página fuente en sí es breve y no detalla especificaciones de hardware, protocolos de comunicación con el PC, ni una lista exhaustiva de modelos (menciona como enlace externo "UCon64 Copier Hardware" para quien necesite ese nivel de detalle); esta entrada refleja fielmente ese alcance limitado y no inventa datos adicionales.
- Al procesar ROMs antiguas de origen incierto, conviene comprobar si tienen el header de 512 bytes heredado de estos copiers, ya que puede desalinear el mapeo de memoria si no se descarta correctamente.

## Referencias cruzadas

- (pendiente de enlazar)
