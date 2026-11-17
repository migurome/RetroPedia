---
titulo: MUGEN — mapa de la documentación oficial y de esta sección
plataforma: [General]
categoria: [motores]
tags: [mugen, referencia, port, formatos, animacion]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [https://network.mugenguild.com/justnopoint/1.0docs/mugen.html, https://network.mugenguild.com/justnopoint/1.0docs/readme.txt]
---

# MUGEN

## Resumen

Punto de entrada para llevar juegos de MUGEN a otras máquinas. Reúne lo aprendido en `hyperdbz-dreamcast-port` y enlaza
cada documento oficial de Elecbyte con la entrada de RetroPedia que lo trata. La documentación es la de **MUGEN 1.0**
(1.1b solo aparece como descarga), publicada en MUGEN Fighters Guild.

## Por dónde empezar

1. [Migrar contenido de 2002 a 1.0](migrar-contenido-2002-a-1.0.md): qué cambia según `mugenversion` y `localcoord`.
2. [Formatos SFF, AIR y SND](formatos-sff-air-snd.md): leer los ficheros.
3. [Compilar el CNS](cns-compilacion.md), [física y golpes](fisica-y-golpes-del-motor.md) y
   [comandos](comandos-y-entrada.md): la lógica del combate.
4. [Escenarios](escenarios-def.md), [barra de vida y asaltos](barra-de-vida-y-asaltos.md) y
   [efectos en el PVR](efectos-visuales-en-dreamcast.md): lo que se ve.
5. [Dolmexica Infinite](dolmexica-infinite.md): la alternativa de usar un intérprete ya hecho.

Para ver la cadena entera: [de un personaje de MUGEN al disco](receta-personaje-a-disco.md).

## Documentación oficial y dónde se trata aquí

Base de las URL: `https://network.mugenguild.com/justnopoint/1.0docs/`.

| Documento | Qué contiene | En RetroPedia |
|---|---|---|
| [cns.html](https://network.mugenguild.com/justnopoint/1.0docs/cns.html) | Variables, `StateDef`, triggers, `common1.cns`, estados especiales y **expresiones** | [CNS](cns-compilacion.md), [física](fisica-y-golpes-del-motor.md) |
| [sctrls.html](https://network.mugenguild.com/justnopoint/1.0docs/sctrls.html) | Referencia de los controladores de estado | [física](fisica-y-golpes-del-motor.md), [efectos](efectos-visuales-en-dreamcast.md) |
| [trigger.html](https://network.mugenguild.com/justnopoint/1.0docs/trigger.html) | Referencia de los triggers | [CNS](cns-compilacion.md) |
| [air.html](https://network.mugenguild.com/justnopoint/1.0docs/air.html) | Formato AIR y números de acción reservados | [formatos](formatos-sff-air-snd.md) |
| [bgs.html](https://network.mugenguild.com/justnopoint/1.0docs/bgs.html) | Escenarios, elementos y controladores de fondo | [escenarios](escenarios-def.md) |
| [coordspace.html](https://network.mugenguild.com/justnopoint/1.0docs/coordspace.html) | Espacios de coordenadas y `localcoord` | [migrar](migrar-contenido-2002-a-1.0.md) |
| [upgrading.html](https://network.mugenguild.com/justnopoint/1.0docs/upgrading.html) | Cambios de 2002.04.14 a 1.0 | [migrar](migrar-contenido-2002-a-1.0.md) |
| [history.html](https://network.mugenguild.com/justnopoint/1.0docs/history.html) | Historial, con los cambios de compatibilidad | [migrar](migrar-contenido-2002-a-1.0.md) |
| [fonts.html](https://network.mugenguild.com/justnopoint/1.0docs/fonts.html) | Fuentes FNT v2: de mapa de bits y TrueType | [barra de vida](barra-de-vida-y-asaltos.md) |
| [sprmake2.html](https://network.mugenguild.com/justnopoint/1.0docs/sprmake2.html) | Generar SFF v2 desde PNG; compresión `rle8`, `rle5` y `lz5` | [formatos](formatos-sff-air-snd.md) |
| [spr.html](https://network.mugenguild.com/justnopoint/1.0docs/spr.html) | Sprites obligatorios y grupos recomendados | [sprites](sprites-obligatorios-y-grupos.md) |
| [storyboard.html](https://network.mugenguild.com/justnopoint/1.0docs/storyboard.html) | Cinemáticas: logo, intro, finales, créditos, game over | [storyboards](storyboards.md) |
| [localization.html](https://network.mugenguild.com/justnopoint/1.0docs/localization.html) | Idiomas | [localización](localizacion.md) |
| [loopguide.html](https://network.mugenguild.com/justnopoint/1.0docs/loopguide.html) | Puntos de bucle de la música | [migrar](migrar-contenido-2002-a-1.0.md) |
| [readme.txt](https://network.mugenguild.com/justnopoint/1.0docs/readme.txt) | Uso general y licencia | Ver abajo |
| [tutorial1](https://network.mugenguild.com/justnopoint/1.0docs/tutorial1.html) a [tutorial4](https://network.mugenguild.com/justnopoint/1.0docs/tutorial4.html), [bg-tut](https://network.mugenguild.com/justnopoint/1.0docs/bg-tut.html) | Crear un personaje y un escenario paso a paso | No hace falta extraerlos: son para autores de contenido |

## Notas / gotchas

- **No existe `exp.html` en esta copia (404):** las expresiones están en `cns.html`, sección *Expressions*.
- **`bgs.html` está desfasado en dos puntos:** sigue documentando `bgvolume` y no incluye `overdrawhigh`, `overdrawlow`,
  `cuthigh`, `cutlow` ni `bgmvolume`. Estos están en `upgrading.html`.
- **Los enlaces "MFG Thread Link" llevan a hilos del foro:** no los he revisado.

### Licencia

El `readme.txt` aplica la licencia al ejecutable y a "otros ficheros de datos asociados" de Elecbyte:

- Solo se permite el uso no comercial, salvo permiso escrito de Elecbyte.
- Se puede redistribuir en obras gratuitas, con copia de la licencia y marcadas como tales.
- El ejecutable no se puede modificar.
- Elecbyte no restringe distribuir personajes o escenarios que no incluyan una parte significativa del entorno.
- El contenido de ejemplo va con licencia Creative Commons no comercial.

Un port que no use el ejecutable aún debe mirar si incluye datos de Elecbyte, como `common1.cns` o los motifs. Los derechos
de personajes y juegos de terceros son aparte; esta licencia no los concede.

## Referencias cruzadas

- [Bibliotecas, motores y recursos de Dreamcast](../../Dreamcast/herramientas/bibliotecas-motores-y-recursos.md)
- [Escenarios 2D en el PVR](../../Dreamcast/graficos/escenarios-2d-paletas-y-baldosas.md)
