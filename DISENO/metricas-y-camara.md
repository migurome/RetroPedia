---
titulo: Métricas del personaje y cámara en plataformas 2D
plataforma: [General]
categoria: [diseno]
tags: [metricas, salto, huecos, camara, deadzone, legibilidad]
estado: borrador
fecha: 2026-10-15
fuentes:
  - https://www.gamedeveloper.com/design/lessons-from-suzy-cube-measuring-up
  - https://forum.defold.com/t/guide-on-jumping-in-platformers/71882
  - https://www.gamedeveloper.com/design/scroll-back-the-theory-and-practice-of-cameras-in-side-scrollers   # Itay Keren
---

# Métricas y cámara

## Resumen

Antes de dibujar una sala hay que conocer los **números del personaje**: cuánto sube, cuánto
avanza en un salto, qué cabe por un hueco. Todo hueco y saliente se mide en esas unidades.
Y la cámara decide **qué ve** el jugador al tomar cada decisión.

## 1. Métricas: reglas de Suzy Cube (y generales)

- Medir: altura máxima de salto, distancia máxima, velocidad de carrera, tamaño del hitbox.
- **Nada justo en el máximo.** Un saliente a la altura máxima o un hueco a la distancia
  máxima exige píxel-perfección: frustra y no se distingue de lo imposible.
- **Tres bandas, sin zona gris:**
  - **Seguro** (claramente alcanzable): ~≤ 75 % del máximo. Para la ruta obligatoria.
  - **Exigente** (alcanzable con buena ejecución): 75–90 %. Solo en rutas opcionales con premio.
  - **Imposible claro**: ≥ ~110 % del máximo. Para cerraduras: el jugador debe entender al
    primer intento que **no** llega.
  - Lo que quede entre 90 % y 110 % **no se usa**: es ambiguo.
- Elegir una **unidad mínima de rasgo** (en Suzy Cube, ~altura del personaje) y que los
  desniveles sean múltiplos claros: "medio cuerpo" o "a la cintura", nunca algo intermedio.
- Una cerradura de habilidad debe estar en la banda "imposible claro" sin la habilidad y en
  la banda "seguro" con ella. Si no cabe en ambas a la vez, cambiar la geometría.
- Geometría en bloques (tiles) facilita medir; con formas orgánicas, la disciplina es la misma.
- Si un valor máximo aparece en el mapa, es casi seguro un error de medida.

## 2. Otras métricas de espacio

- **Altura libre** sobre una plataforma: al menos la altura del salto + el sprite, o el techo
  cortará el salto (golpe de cabeza = frustración si no es intencionado).
- **Anchura de pasillo**: holgura de varios píxeles sobre el hitbox.
- **Plataforma de aterrizaje**: más ancha cuanto más largo/rápido el salto que llega a ella
  (aterrizar en 1 tile tras un salto largo es una prueba de precisión, no un paso normal).
- **Caídas**: una caída larga sin ver el fondo es un salto de fe; mostrar el suelo o usar
  cámara/espacio para que se vea.

## 3. Cámara (Itay Keren, "Scroll Back")

Técnicas relevantes para un plataformas:

| Técnica | Qué hace | Ejemplos |
|---|---|---|
| Ventana de cámara (deadzone) | La cámara se mueve solo cuando el jugador empuja el borde de la ventana | Sonic (ventana estrecha: siempre ves lo que viene) |
| Platform-snapping | La cámara no sigue verticalmente durante el salto; ajusta al aterrizar | Super Mario World, Rayman, Sonic |
| Foco adelantado (forward focus) | Más espacio visible en la dirección de avance (~25 % del ancho) | SMB, Defender |
| Doble foco adelantado | Cambia el adelanto según la dirección del jugador, con umbral para no oscilar | Super Mario World, Cave Story |
| Lerp | La cámara se acerca a su objetivo suavizando | DKC, Super Meat Boy |
| Atractores (cue focus) | Objetos importantes tiran de la cámara | Limbo |

Implicaciones para la colocación:
- La **ventana vertical** debe ser al menos tan alta como un salto normal (Rayman), para que
  saltar no mueva la cámara.
- En salas que avanzan en un sentido, el jugador necesita ver **lo que viene antes de
  decidir**: un obstáculo no debe aparecer en pantalla cuando ya es tarde para reaccionar.
  Regla práctica: el primer peligro de un tramo debe verse con al menos ~1 s de antelación.
- Con mucho adelanto, un enemigo que viene **por detrás** o **desde arriba** es injusto:
  ventanas estrechas (Rastan) dejan poco margen arriba.
- Lo importante (salida, item) debe poder **encuadrarse** en pantalla junto con el jugador en
  algún momento: si nunca están juntos en pantalla, el jugador no los relaciona.

## Referencias cruzadas

- [Velocidad y gravedad en plataformas](../GLOSARIO/speed-and-gravity.md)
- [Física y colisiones en MD Engine](../MegaDrive/herramientas/md-engine/fisica-y-colisiones.md)
- [Aplicación a Retrovania](aplicacion-retrovania.md) — números reales del héroe
