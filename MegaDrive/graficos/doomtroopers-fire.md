---
titulo: Efecto de fuego de Doom Troopers
plataforma: [MegaDrive]
categoria: [graficos]
tags: [efecto, sprites, prioridad, tecnica-visual]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/doomtroopers-fire]
---

# Efecto de fuego de Doom Troopers

## Resumen

Explica cómo el juego _Doom Troopers_ logra el efecto de fuego en su pantalla de título, en el que las letras del logo dejan ver "fuego" a través de ellas. El truco no usa ningún generador de fuego dedicado: reutiliza los sprites de explosión ya existentes en el juego, ocultos detrás del logo con huecos transparentes.

## Detalle técnico

Doom Troopers tiene un efecto de fuego llamativo en su pantalla de título: el logo del juego y el menú están escritos sobre una superficie metálica, y las letras que forman el texto "Doom Troopers" son en realidad huecos que dejan ver fuego detrás.

El efecto es sorprendentemente simple: el primer plano es el plano A en alta prioridad (el plano B no se usa), con el logo de _Doom Troopers_ siendo transparente. El fuego son sprites de baja prioridad (que se ven a través de los huecos)... y en concreto, son las explosiones que ya se usan a lo largo del juego, apareciendo en posiciones aleatorias detrás del logo del título. Solo se ven a través de los huecos, lo que dificulta distinguir su forma real, y por eso parecen fuego.

## Notas / gotchas

- No hay ningún algoritmo de generación de fuego: el efecto reutiliza sprites de explosión ya existentes en el juego, spawneados en posiciones aleatorias detrás del logo.
- La ilusión de "fuego" depende de que solo se vea una parte pequeña y recortada de cada sprite de explosión (a través de los huecos de las letras), lo que oculta su forma real y hace difícil reconocerlos como explosiones.
- Requiere que el plano A (con el logo transparente en los huecos de las letras) tenga prioridad alta y los sprites de explosión prioridad baja, para que el "fuego" solo se vea a través de los huecos.
- La página fuente no incluye código en C/ASM, tablas de datos ni valores hexadecimales: es una explicación conceptual acompañada de un GIF y capturas comparativas (una con el plano A normal, otra con el plano A eliminado para revelar los sprites de explosión).

## Referencias cruzadas

- (pendiente de enlazar)
