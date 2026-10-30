---
titulo: Principios de diseño de niveles (teoría base)
plataforma: [General]
categoria: [diseno]
tags: [level-design, kishotenketsu, tutorial, ritmo, riesgo-recompensa]
estado: borrador
fecha: 2026-10-15
fuentes:
  - https://80.lv/articles/gdc-ten-principles-for-good-level-design/   # Dan Taylor, GDC 2013
  - https://www.mcvuk.com/development/video-nintendos-level-design-secrets-in-four-steps   # GMTK sobre Hayashida
  - https://en.wikipedia.org/wiki/World_1-1
  - https://www.mcvuk.com/development-news/video-miyamoto-shares-his-level-design-secrets/
  - https://killscreen.com/mega-man-in-love-and-death   # Egoraptor / Mega Man X
  - https://themmnetwork.com/2015/03/26/a-critical-look-at-mega-man-stages-these-enemies-are-the-pits/
  - https://www.abratabia.com/game-design/level-design-basics.php   # curva de intensidad
---

# Principios de diseño de niveles

## Resumen

Un nivel no es un dibujo: es una **secuencia de decisiones** que el jugador toma en un espacio.
Cada tile tiene que responder a "¿qué hace aquí el jugador, y por qué?". Esta entrada reúne la
teoría que se aplica a cualquier plataformas 2D; las entradas hermanas bajan a guiado,
mundo metroidvania y métricas.

## 1. Los diez principios de Dan Taylor (GDC 2013), resumidos

| # | Principio | Traducción práctica a un plataformas 2D |
|---|---|---|
| 1 | Navegar debe ser divertido | El camino en sí es el contenido: saltos con ritmo, no pasillos vacíos. |
| 2 | No depender de palabras | El espacio cuenta lo que pasa (ruinas, restos, cambios de estilo). Texto solo como último recurso. |
| 3 | Objetivo claro, método no | El jugador ve **adónde** ir (una abertura, un objeto), pero tiene que descubrir **cómo**. |
| 4 | Enseñar siempre algo nuevo | Cada sala introduce, desarrolla o retuerce una idea. Si no aporta nada, sobra. |
| 5 | Sorprender | Romper el patrón de vez en cuando: cambio de escala, sala vertical tras horizontal, vista inesperada. |
| 6 | Empoderar | Dejar que el jugador se sienta hábil: obstáculos que antes costaban, ahora se pasan sin pensar. |
| 7 | Dificultad elegible (riesgo/recompensa) | Ruta fácil obligatoria + ruta difícil opcional con premio. |
| 8 | Eficiencia / modularidad | Reutilizar espacio: bidireccional, recorrible en ambos sentidos (clave en metroidvania). |
| 9 | Crear emoción | Ritmo, contraste, momentos de calma antes de picos. |
| 10 | Guiado por mecánicas | El nivel existe para que las mecánicas brillen; se diseña desde ellas, no desde el decorado. |

## 2. Kishōtenketsu: la estructura de cuatro tiempos de Nintendo

Koichi Hayashida (Super Mario 3D Land/World) estructura cada nivel alrededor de **una sola idea**:

1. **Ki — Introducción.** La mecánica aparece en un sitio **seguro**: si fallas, no mueres
   (o el castigo es mínimo). Ej.: el primer hueco tiene suelo debajo.
2. **Shō — Desarrollo.** La misma idea con algo más de exigencia: más larga, combinada con
   movimiento, encadenada.
3. **Ten — Giro.** La idea se recontextualiza: se usa al revés, en vertical, con tiempo, contra
   un enemigo… Es lo que se recuerda del nivel.
4. **Ketsu — Conclusión.** Una última prueba/celebración breve y se **abandona** la idea
   (no se estira hasta aburrir).

Regla derivada: **una sala o tramo = una idea**. Mezclar tres gimmicks a la vez diluye las tres.
Crítica habitual a Celeste: cuando el "giro" de un capítulo se agota antes que el capítulo, la
dificultad se sube alargando salas, y se vuelve repetitivo. Mejor acortar que estirar.

## 3. Enseñar sin texto (SMB 1-1, Mega Man)

- **Miyamoto (World 1-1):** el nivel contiene todo lo necesario para que el jugador "entienda
  gradual y naturalmente". El primer Goomba avanza despacio, sobre suelo plano, sin nada más
  alrededor: la única respuesta razonable es saltar. El primer champiñón está encajonado para
  que sea casi imposible no cogerlo.
- **Los peligros aparecen después de sus soluciones.** No pongas un hueco que requiere doble
  salto antes de haber dado el doble salto (salvo como *promesa*, ver guiado).
- **Mega Man / Egoraptor:** los enemigos nuevos se presentan primero en un entorno seguro
  (un pasillo, a una distancia donde su primer ataque no alcanza). Introducir un elemento
  nuevo directamente en una situación peligrosa provoca muertes injustas.
- **Celeste:** el moveset completo se presenta en el prólogo y el resto del juego **recontextualiza**
  esas mismas pocas acciones con reglas de cada capítulo. Pocas mecánicas, muchos usos.
- **Patrón "encerrar para enseñar"** (Super Metroid): una sala de la que solo se sale usando
  la mecánica recién aprendida. Funciona si el encierro es seguro y el truco es descubrible
  (un "maestro" que lo demuestra, como los Etecoons con el salto de pared).

## 4. Ritmo: la curva de intensidad

- Puntúa cada tramo de 1 (seguro) a 10 (lo más difícil) y dibuja la curva.
- Forma buena: **diente de sierra ascendente** — sube, baja un poco (descanso), sube más alto,
  pico cerca del final, resolución corta.
- Malo: plana (monótona), pico al principio y luego bajada (anticlimática), todo alto (agota).
- Los tramos tranquilos no son relleno: crean **contraste** y dan tiempo a anticipar.
  Un punto de guardado/checkpoint antes de un pico es una señal ("algo viene").
- Contraste también de **forma**: tras una sala estrecha, una amplia; tras horizontal, vertical.

## 5. Riesgo / recompensa

- La **ruta crítica** (obligatoria) debe ser clara y asequible.
- Las rutas opcionales cuestan más (saltos ajustados, peligro, rodeo) y **siempre** pagan algo
  visible: gema, vida, atajo, vista de una zona futura.
- Sonic: la ruta alta suele ser más rápida y exigente (premia la habilidad con velocidad e items);
  la baja es más lenta y de plataformas. Caer de la alta a la baja es el "castigo" suave, no la muerte.
- El jugador debe **ver la recompensa antes de pagar el riesgo**. Recompensa invisible = nadie
  arriesga.

## Notas / gotchas

- Ningún principio es ley: son plantillas. Pero si se rompe uno, que sea a propósito.
- La teoría de Hayashida y de Taylor llega aquí por resúmenes secundarios (vídeos/artículos);
  la redacción exacta está en la charla de GDC y en el vídeo de Game Maker's Toolkit.

## Referencias cruzadas

- [Guiar al jugador](guiar-al-jugador.md)
- [Diseño de mundo metroidvania](mundo-metroidvania.md)
- [Métricas y cámara](metricas-y-camara.md)
- [Aplicación a Retrovania](aplicacion-retrovania.md)
