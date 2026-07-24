---
titulo: Efecto de agua translúcida de Sonic
plataforma: [MegaDrive]
categoria: [graficos]
tags: [sonic, efecto, raster, cram, paletas, tecnica-visual]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/sonic-water]
---

# Efecto de agua translúcida de Sonic

## Resumen

Probablemente uno de los efectos más conocidos de la consola. Algunas zonas de los juegos de Sonic tienen agua translúcida que cubre todo el ancho de la pantalla (por ejemplo, Labyrinth Zone acto 1 en Sonic 1, donde los gráficos se ven mayormente naranjas por encima del agua y se vuelven verdes por debajo de ella, con la superficie del agua cubierta de ondas hechas con sprites).

## Detalle técnico

El efecto se consigue mediante un efecto raster que reescribe todas las paletas a la altura del agua. Es decir, durante el barrido de la pantalla, en la línea donde está la superficie del agua, el código reescribe las paletas activas en CRAM para que todo lo que se dibuje por debajo de esa línea use los colores "de debajo del agua" (verdes) en vez de los colores normales (naranjas).

Este cambio de paleta a mitad de pantalla deja un rastro visible: los llamados "CRAM dots" (puntos de CRAM), un artefacto que aparece allí donde se escribe un color en la paleta mientras esa zona de la pantalla ya es visible (es decir, escribir en CRAM durante el área visible, no durante el blanking). El juego tapa esto usando los sprites de las ondas de agua, que son lo bastante ruidosos visualmente como para que los CRAM dots se pierdan dentro de ellos.

## Notas / gotchas

- El efecto depende de reescribir la CRAM en mitad del área visible de la pantalla (raster effect), lo cual introduce artefactos visuales ("CRAM dots") en el punto exacto donde se escribe.
- La solución del juego a los CRAM dots no es evitarlos, sino ocultarlos: coloca sprites de ondas ruidosas justo encima de la línea de agua para camuflar los puntos.
- La página fuente no incluye código en C/ASM, tablas de valores de color/paleta concretos, ni detalles de timing de H-blank: es una explicación conceptual de alto nivel acompañada de capturas de pantalla.

## Referencias cruzadas

- (pendiente de enlazar)
