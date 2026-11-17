---
titulo: Registro de decisiones de proyectos — qué se eligió, qué se descartó y por qué
plataforma: [General]
categoria: [general]
tags: [proceso, port, arquitectura, referencia]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [proyecto hyperdbz-dreamcast-port (PLAN_PORT_DREAMCAST.md, DIARIO.md), proyecto SSB Port (PLAN_PORT_MEGADRIVE.md, CLAUDE.md)]
---

# Registro de decisiones de proyectos

## Resumen

Decisiones que condicionan otros proyectos, con la alternativa descartada y el motivo. Evita repetir una discusión ya
resuelta. El detalle sigue en el diario de cada proyecto. `comprobado` no aplica: esto recoge razones, no técnicas.

**Al añadir una:** una fila por decisión que valga fuera de su proyecto, al cerrar una fase. Si se revoca, se tacha y se
dice por qué, sin borrarla.

## Detalle técnico

### Hyper DBZ en Dreamcast

| Fecha | Decisión | Descartado | Por qué |
|---|---|---|---|
| Fase 0 | Dreamcast | PS2 | El doble de RAM no cambia el planteamiento |
| Fase 0 | Motor propio con CNS compilado, sprites comprimidos en RAM y caché en VRAM | Un intérprete ya hecho, como Dolmexica | El análisis de viabilidad y las medidas de la Fase 0. Dolmexica [recorta funciones](../MOTORES/mugen/dolmexica-infinite.md) |
| 2026-10-30 | Implementar ya los controladores de golpe que faltaban | Esperar a la Fase 2 | El paso prometía velocidades y caída, y no se cumplía |
| 2026-10-30 | Efectos enormes a resolución completa (usuario, tras ver un vídeo comparativo) | Media resolución | Los rayos finos del aura quedan escalonados |
| 2026-10-30 | Si MUGEN crea 56 helpers, el port también | «Arreglar» al personaje | La fidelidad va primero |
| 2026-10-31 | Registro en la VMU en emulador y por QR en la consola | Una sola vía | En cada sitio se saca por donde se puede |
| 2026-11-01 | No comparar con Ikemen GO tick a tick | Comparación completa | La IA y el azar separan los dos motores |
| 2026-11-05 | Retirar el camino rápido del intérprete | Conservarlo | Sin ganancia en Flycast; sobran 40 líneas, y el perfil del PC engañó |
| 2026-11-06 | Objetivo de 8 ms medido en combate real (usuario) | Medirlo en el modo pesimista | El caso pesimista no se da en MUGEN. No sustituye a medir en la consola |
| 2026-11-06 | Dolmexica solo como consulta del PVR | Usarlo como referencia de fidelidad | Prioriza contenido antiguo |

### Super Smash Land en Mega Drive

| Fecha | Decisión | Descartado | Por qué |
|---|---|---|---|
| Fase 0 | El juego de PC solo es referencia | Recompilar el `.gmk` | No se va a modificar, y Windows bloquea los binarios sin firma |
| 2026-10-22 | No guardar datos; todo disponible desde el principio (usuario) | Guardado del progreso | El plan no da el motivo; como efecto, MD Engine no reserva flash |
| 2026-10-22 | Música completa en PCM, aunque sean menos temas (usuario) | Más temas | El plan no da el motivo; el proyecto tiene después músicas FM: revisarlo antes de copiarla |
| 2026-10-23 | 4 luchadores con color: dos personajes por paleta, en índices repartidos, y hojas duplicadas en LZ4W (usuario) | Sombra y resalte; cambiar paleta a mitad de pantalla | Sombra y resalte solo aclaran u oscurecen; los luchadores se mueven libres y quedan puntos en la línea del cambio |
| 2026-10-23 | Arcade espera a los 4 jugadores (usuario) | Hacerlo antes | La ronda 3 son tres CPU a la vez |
| B047 | ROM propia de SGDK como vía de publicación (usuario, tras probar la B046 en BlastEm) | Seguir publicando con MD Engine | «Esta va a ser la manera de sacar el juego a partir de ahora» |

## Notas / gotchas

- **Lo revocado también enseña:** el plan de SSB partió de MD Engine y acabó en SGDK.
- Las fechas `Fase 0` y `B047` son las que dan los documentos; no constan como día concreto en lo revisado.

## Referencias cruzadas

- [Reglas comunes de proyecto](reglas-comunes-de-proyecto.md)
- [Imitar paletas de otras consolas](../MegaDrive/graficos/imitar-paletas-otras-consolas.md)
- [Dolmexica Infinite](../MOTORES/mugen/dolmexica-infinite.md)
