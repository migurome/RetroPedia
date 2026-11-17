---
titulo: Localización de MUGEN — idioma y grupos con prefijo
plataforma: [General]
categoria: [motores]
tags: [mugen, formatos, referencia]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [https://network.mugenguild.com/justnopoint/1.0docs/localization.html]
---

# Localización de MUGEN

## Resumen

El idioma sale de `Language` en la sección `[Config]` de `mugen.cfg`, con un código ISO 639-1 (`en`, `ja`), y se lee al
arrancar. El contenido se traduce por grupos: si existe `[xx.Nombre]` para el idioma elegido, se usa ese; si no, `[Nombre]`,
que debe estar en inglés.

## Detalle técnico

| Se puede localizar | Ejemplo |
|---|---|
| `[Files]` de `system.def` | Otra fuente para las frases de victoria |
| Escenas de storyboard | `[es.Scene 3 hello]`; el resto del nombre debe coincidir con la versión inglesa |
| Frases de victoria y logo del título | — |

| No se puede localizar |
|---|
| Todos los grupos de `mugen.cfg` |
| `[Command]`, `[State ...]` y `[Statedef ...]` |
| Elementos individuales de los fondos |

## Notas / gotchas

- **Caracteres no ASCII:** necesitan fuentes TrueType: las de mapa de bits solo admiten de 32 a 126
  ([FNT v2](barra-de-vida-y-asaltos.md)). Una consola sin TrueType tendría que convertir esos glifos a sprites.
- **Nombres de personajes y escenarios:** se recomienda ASCII, para que se vean en cualquier motif.
- **Para un port:** un lector de `.def` debe descartar los grupos con prefijo de otro idioma. Si no, un `[ja.Scene 0]` podría
  pisar o duplicar `[Scene 0]`.

## Referencias cruzadas

- [Storyboards](storyboards.md)
- [Barra de vida y fuentes](barra-de-vida-y-asaltos.md)
