---
titulo: SpritesMind.Net — foro de referencia para desarrollo en Mega Drive
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [spritesmind, foro, comunidad, referencia, hardware]
estado: borrador
fecha: 2026-07-21
fuentes: [https://www.spritesmind.net/, https://gendev.spritesmind.net/forum/]
---

# SpritesMind.Net — foro de referencia para desarrollo en Mega Drive

## Resumen

SpritesMind.Net es el portal/comunidad histórica de referencia para desarrollo homebrew y reverse-engineering de Sega Mega Drive/Genesis (y otros sistemas Sega). Su foro técnico (`gendev.spritesmind.net/forum`) contiene, desde hace más de 20 años, las discusiones más profundas sobre el hardware real: timing exacto de bus, comportamiento del FIFO del VDP, registros indocumentados del YM2612, diferencias entre revisiones de la consola, etc. — información que a menudo no está en plutiedev.com ni en la documentación de SGDK porque procede de ingeniería inversa con analizador lógico sobre hardware real (destaca especialmente el usuario **Nemesis**).

## Detalle técnico

### Estructura del sitio

- `www.spritesmind.net` — portal, enlaza a varias secciones (foro de Genesis, "Retro Fun", "Ar'Hack'De" de arcade, traducción de doujin, etc.)
- `gendev.spritesmind.net` — el subdominio dedicado a "Sega Genesis/Megadrive homebrew development", con el foro en `/forum/` y una carpeta `/mirrors/` con documentos y PDFs de terceros (p. ej. notas de hardware de Master System, documentación de lightguns de Eke).
- `gendev.spritesmind.net/page-doc.html` — página que actúa de índice de documentación del sitio.

### Limitación importante para esta base de conocimiento

**No ha sido posible hacer scraping/fetch directo del foro ni de `page-doc.html`** — tanto la portada del foro como los hilos individuales (`viewtopic.php?t=...`) devuelven un bucle de redirecciones con las herramientas de fetch disponibles (posiblemente protección anti-bot en ese subdominio). Solo la portada `www.spritesmind.net` es accesible directamente.

Por eso, las entradas de esta base que referencian hilos de SpritesMind (ver "Referencias cruzadas") son **índices curados construidos a partir de fragmentos de resultados de búsqueda**, no transcripciones completas de los hilos. Contienen el enlace directo a cada hilo y un resumen breve de lo que aporta, pero **hay que abrir el hilo en un navegador para leer el contenido completo** antes de dar por buena cualquier afirmación técnica fina (fórmulas, timings exactos, valores de registro).

### Hilo meta-índice recomendado

- ["Aggregating Community Research"](https://gendev.spritesmind.net/forum/viewtopic.php?t=2227) — hilo pensado explícitamente como punto de partida para quien quiera escribir documentación, para no tener que rebuscar entre cientos de hilos. Buen primer sitio para explorar manualmente.

## Notas / gotchas

- El foro usa phpBB con URLs tipo `viewtopic.php?t=<id>` — son estables en el tiempo, así que los enlaces de esta base deberían seguir funcionando aunque el fetch automático falle.
- Si en el futuro se consigue acceder al contenido completo (navegador manual, copiar/pegar el texto, u otra vía), estas entradas-índice deberían actualizarse con el contenido real y subir de `estado: borrador` a `revisado`.

## Referencias cruzadas

- [Hilos de VDP en SpritesMind](../graficos/spritesmind-vdp-hilos.md)
- [Hilos de YM2612 en SpritesMind](../sonido/spritesmind-ym2612-hilos.md)
- [Hilos de timing de bus/DMA/Z80 en SpritesMind](../cpu/spritesmind-bus-timing-hilos.md)
- [Hilos sobre revisiones de hardware en SpritesMind](../memoria/spritesmind-hardware-revisiones-hilos.md)
