---
titulo: Estudio del sistema gráfico de la Capcom CPS-1
plataforma: [General]
categoria: [graficos-hardware]
tags: [cps1, arcade, street-fighter-2, capcom, sprites, tilemap]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/cps1_gfx/index.html]
---

# Estudio del sistema gráfico de la Capcom CPS-1

## Resumen

El artículo analiza la arquitectura de composición gráfica de la placa arcade CPS-1 de Capcom (1989) a través de una serie de juegos representativos (Forgotten Worlds, Ghouls'n Ghosts, Strider, Final Fight, Carrier Air Wing, Street Fighter II y sus revisiones). El autor modificó el driver `video/cps1.cpp` de MAME para colorear cada capa por separado y así poder inferir, juego a juego, cómo los equipos de arte de Capcom repartían sus seis capas de hardware. Es relevante como referencia de diseño porque documenta decisiones reales de producción (presupuesto de tiles, prioridades, trucos de capas) tomadas bajo las restricciones de un motor de tiles de finales de los 80.

## Detalle técnico

### Las seis capas de la CPS-1

La CPS-1 era una máquina muy potente para su época: podía componer hasta **seis capas, apilables en cualquier orden**. Los desarrolladores disponían de:

- Tres capas de tilemap de "fondo": `SCROLL1`, `SCROLL2`, `SCROLL3`.
- Una capa `OBJ`, donde los tiles se agregan para formar sprites.
- Dos campos de estrellas: `STAR1`, `STAR2`.

Código de color usado por el autor para visualizar las capas en las capturas del artículo:
- Rojo = SCROLL1
- Verde = SCROLL2
- Azul = SCROLL3
- Negro = OBJ
- Cian = STAR1
- Amarillo = STAR2

**Tamaño de tile por capa** (granularidad distinta por diseño):
- SCROLL1: tiles de **8x8** píxeles.
- SCROLL2: tiles de **16x16** píxeles.
- SCROLL3: tiles de **32x32** píxeles.

**Partición física de la ROM gráfica**: una ROM de GFX se divide en **cuatro áreas a nivel de hardware**: una por cada capa SCROLL más una para la capa OBJ. El tamaño de estas áreas se decide al comienzo del desarrollo y queda fijado en EPROM (cita textual del autor: "set in stone").

**Límite de la capa OBJ**: la máquina está limitada a **256 tiles** en su capa OBJ, por lo que los tiles de OBJ deben usarse con cuidado ("OBJ tiles must be used wisely").

Todas las capas tienen una prioridad de renderizado que puede configurarse libremente (orden de apilado arbitrario).

### Casos de estudio por juego

**Forgotten Worlds (mayo 1988)** — primer título de la plataforma. La escena inicial usa las seis capas simultáneamente, posiblemente como demostración de potencia. Los elementos de GUI se colocan en SCROLL1 (rojo). Esto podría parecer contraintuitivo porque un tilemap obliga a alinear los elementos en una grilla, mientras que la capa OBJ permitiría posicionar la GUI en cualquier parte de la pantalla. Sin embargo, es una decisión favorable porque los elementos de GUI suelen estar alineados de todas formas, y sobre todo porque la máquina está limitada a 256 tiles en OBJ: hay que reservarlos para otros usos.

**Ghouls'n Ghosts (diciembre 1988)** — secuela de Ghosts'n Goblins (1985). De nuevo SCROLL1 (rojo) se usa para GUI, decisión no aleatoria: dado que SCROLL1 tiene la granularidad más fina (8x8) frente a SCROLL2 (16x16) y SCROLL3 (32x32), los diseñadores usan lógicamente la capa de grano más fino para la interfaz.

Poco después del inicio del Nivel 1 empieza a llover. Dado que casi todas las capas ya están en uso y STAR2 solo puede generar puntos diminutos (inútil para lluvia), el truco usado es: **cada cuatro fotogramas, la GUI deja de mostrarse en favor de tiles de lluvia**. El ojo humano y los CRT de la época funden ambos estados visualmente.

**Strider (marzo 1989)** — proyecto ambicioso que combina ocasionalmente tiles OBJ no para formar varios enemigos pequeños, sino para construir un único oponente gigantesco (efecto impresionante para la época). Strider también presenta superficies no alineadas con las que el héroe interactúa: Hiryu interactúa tanto con SCROLL3 (azul) como con la capa OBJ (negro).

**Final Fight (diciembre 1989)** — desarrollado con un presupuesto gráfico ajustado: en esa época el precio de la ROM era tan alto que a los artistas solo se les asignaron **2MiB**. Pese a esa limitación, el juego fue uno de los mayores éxitos de la plataforma.

Orden de prioridad de capas observado: SCROLL3 (azul) tiene la prioridad más baja, sirviendo de skyline/horizonte; después SCROLL2 (verde) para el área de combate; después OBJ (negro), donde están los personajes, barriles y neumáticos; finalmente la GUI en SCROLL1 (rojo) tiene la prioridad más alta.

Excepción notable: Mike Haggar aparece a la vez delante y detrás de SCROLL2 (verde). Esto se logra mediante una **"priority mask"** (máscara de prioridad), una propiedad especial que se puede otorgar a la capa situada detrás de OBJ. A ciertos tiles se les puede asignar esta máscara para que algunos de sus colores aparezcan por encima de OBJ en vez de ser ocluidos. En este caso los colores marcados con la máscara producen una especie de "escalera" que envuelve/sandwichea a los personajes.

**Carrier Air Wing (octubre 1990)** — la intro presenta efectos que en teoría un motor de tiles no debería poder producir: una estela horizontal dejada por el F-14 que se expande de a una línea por vez (una granularidad que la máquina no puede alcanzar de forma nativa), y un efecto de "fizzlefade" a nivel de píxel. Según la vista de capas, todo esto se hace mediante tiles OBJ, de forma análoga a la barra de energía de Street Fighter II (ver referencias cruzadas). El fizzlefade es en realidad un **patrón precalculado repartido en varios tiles, repetido una y otra vez**; como suele pasar cuando las cosas se mueven muy rápido, el ojo humano tiende a "rellenar los huecos".

**Street Fighter II (febrero 1991)** — título peculiar: los desarrolladores decidieron usar SCROLL1 (rojo) para mejorar los efectos de parallax en vez de usarla para GUI. Como resultado, la GUI está implementada íntegramente con tiles OBJ (negro).

Caso curioso: en el escenario de Guile, la cabina y la cola del F-16 están parcialmente hechas con OBJ (negro). Teoría del autor: los artistas se quedaron sin espacio en la capa SCROLL2 (verde) y terminaron el fondo con tiles OBJ, de los que tenían un pequeño excedente.

SCROLL2 (verde) tiene un "poder especial": originalmente no era una característica muy útil hasta que Street Fighter II la aprovechó para implementar un **efecto de parallax de suelo por línea (per-line floor parallax)**. Funciona haciendo scroll horizontal de los tiles **a nivel de línea individual**.

**Street Fighter II Champion Edition (marzo 1992)** — cita del productor Akira Nishitani (Capcom): "En la época de SF2, las especificaciones de las placas arcade no eran tan buenas como hoy, y había un límite de cuántos sprites podían mostrarse en pantalla a la vez. Planeamos cuidadosamente las cosas para que el personaje más grande y el segundo más grande pudieran caber apenas en pantalla al mismo tiempo. Pero cuando los combates espejo (mismo personaje) se hicieron posibles en Champion Edition, eso significaba que había que poder mostrar dos copias del personaje más grande en pantalla. Terminamos teniendo que eliminar elementos de fondo y similares."

En el escenario original de Ryu, el equipo de arte usó muchos sprites para embellecerlo: esta escena representa **casi 200 tiles en la capa OBJ**. Para permitir que los personajes grandes (p. ej. Zangief u Honda) se enfrenten a su espejo en este escenario, se eliminó el cartel "viento, bosque, fuego, montaña" (風林火山). Para compensar, se rehicieron también las paletas de scroll, resultando en un combate que ocurre de noche en vez de al atardecer.

**Street Fighter II' Turbo: Hyper Fighting (diciembre 1992)** — uno de los últimos juegos de CPS-1. La pantalla de título es mayoritariamente negra excepto por el logo. Este efecto se logra dando a STAR1 (cian) **máxima prioridad** sobre las demás capas SCROLL y configurando todas sus paletas en negro, de modo que las estrellas no son visibles. Nota del autor: "quizás sea el aspecto más poético de la máquina: la mayoría de las veces que ves una gran porción de negro, en realidad estás mirando el cielo nocturno".

## Notas / gotchas

- El artículo es un estudio visual/de casos de uso de capas (mediante recoloreado del driver de MAME), no una especificación de registros de hardware a bajo nivel: no incluye direcciones de memoria, mapas de registros, resoluciones de pantalla en píxeles, bits por píxel ni frecuencias de reloj del subsistema gráfico. Para esos detalles hacen falta otras fuentes (p. ej. manuales de MAME, esquemas originales de Capcom).
- El límite de 256 tiles en OBJ condiciona decisiones de diseño recurrentes (dónde poner la GUI, cuántos personajes grandes caben en pantalla a la vez).
- Varias técnicas documentadas son trucos de percepción/temporización (parpadeo de tiles, patrones precalculados) más que capacidades reales de hardware — relevante para un motor con restricciones similares (Mega Drive).
- El reparto de las 4 áreas de ROM (una por SCROLL + una por OBJ) se fija en tiempo de compilación/grabado de EPROM, no es reconfigurable en tiempo de ejecución.

## Referencias cruzadas

- (pendiente de enlazar)
