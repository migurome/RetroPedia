---
titulo: Sega Retro — wiki de documentación técnica (overview y limitaciones)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [segaretro, wiki, referencia, hardware, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://segaretro.org/Category:Mega_Drive_technical_information]
---

# Sega Retro — wiki de documentación técnica (overview y limitaciones)

## Resumen

Sega Retro (`segaretro.org`) es una wiki enciclopédica sobre todo el catálogo y hardware de Sega, con una categoría dedicada, ["Mega Drive technical information"](https://segaretro.org/Category:Mega_Drive_technical_information), que documenta 14 aspectos concretos del hardware de la consola. Es un recurso más "enciclopédico/curado" que plutiedev.com (tutorial) o SpritesMind (foro de discusión cruda).

## Detalle técnico

### Limitación de acceso (mismo patrón que otras wikis de esta base)

`segaretro.org` devuelve **403 Forbidden** con las herramientas de fetch disponibles — ya confirmado en la sesión anterior (ver memoria del proyecto) y confirmado de nuevo hoy. Un intento de usar un mirror/proxy encontrado por búsqueda (`mokles.hedbergandson.com`) falló por DNS inexistente (dominio caído). Por tanto, igual que con SpritesMind y la wiki de Sonic Retro, el contenido de abajo se reconstruyó vía WebSearch, no por fetch directo.

### Las 14 páginas de la categoría "Mega Drive technical information"

1. Control pad inputs
2. DMA
3. Interrupts — ver [`../graficos/segaretro-interrupts.md`](../graficos/segaretro-interrupts.md) (contenido extraído)
4. Memory map
5. Mode 4 (modo de compatibilidad con Master System)
6. Palettes and CRAM
7. Planes
8. Priority
9. Quirks — ver [`../memoria/segaretro-quirks.md`](../memoria/segaretro-quirks.md) (contenido extraído)
10. Scrolling
11. Shadow and highlight
12. Sprites
13. VDP general usage
14. VDP registers

De estas 14, la mayoría se solapan temáticamente con entradas ya existentes en esta base (fuente plutiedev.com): DMA, memoria/mapa, paletas/CRAM, planos, prioridad, scroll, shadow/highlight, sprites y registros del VDP. Para esos temas, WebSearch no devolvió contenido técnico sustancial más allá de confirmar que la página existe — solo se ha podido extraer contenido real y genuinamente nuevo para **Interrupts** y **Quirks** (ver enlaces arriba). El resto queda como referencia pendiente: si se necesita contrastar/ampliar un tema concreto de los de la lista, visitar la página directamente en el navegador.

### Hallazgo adicional (no procesado todavía)

Durante la búsqueda apareció `genvdp.txt` (http://jiggawatt.org/genvdp.txt), documentación no oficial del VDP escrita por Charles MacDonald — muy citada en la escena de desarrollo/emulación de Mega Drive. No se ha procesado en esta sesión; queda como candidato para una futura ronda de esta base.

## Notas / gotchas

- Como con SpritesMind, si en el futuro se logra acceso directo a segaretro.org, revisar y ampliar `segaretro-interrupts.md` y `segaretro-quirks.md`, y considerar completar las 12 páginas restantes de la categoría.

## Referencias cruzadas

- [SpritesMind — overview y limitaciones de acceso](spritesmind-overview.md)
- [Disassemblies de Sonic Retro](sonic-retro-disassemblies.md)
- [`../graficos/segaretro-interrupts.md`](../graficos/segaretro-interrupts.md)
- [`../memoria/segaretro-quirks.md`](../memoria/segaretro-quirks.md)
