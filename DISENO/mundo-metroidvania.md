---
titulo: Diseño de mundo metroidvania — llaves, cerraduras, bucles y backtracking
plataforma: [General]
categoria: [diseno]
tags: [metroidvania, gating, lock-and-key, dependency-graph, atajos, backtracking]
estado: borrador
fecha: 2026-10-15
fuentes:
  - https://bugnet.io/blog/how-to-design-a-metroidvania-map
  - https://ludonauta.itch.io/platformer-essentials/devlog/1084669/the-hidden-genius-behind-hollow-knights-terrain-design
  - https://blog.hslu.ch/games/files/2021/11/Metroidvania_Analysis.pdf   # Hollow Knight locks/keys
  - https://gamedeveloper.com/design/the-invisible-hand-of-super-metroid
  - https://omegametroid.com/check-out-this-in-depth-world-design-analysis-of-the-metroid-series/   # GMTK Boss Keys
  - https://spawningpoint.com/article/the-metroidvania-design-lineage-symphony-of-the-night-to-animal-well
---

# Diseño de mundo metroidvania

## Resumen

Un metroidvania es **un único espacio que se pliega sobre sí mismo**, cerrado con cerraduras
que se abren con habilidades. Bucle central: *explorar → toparse con un bloqueo → conseguir
una llave → volver y abrirlo*. El mapa es el motor del género: si el mapa es legible, volver
es un placer; si no, es una caminata tediosa.

## 1. Llaves y cerraduras

**Tipos de llave** (Hollow Knight): habilidad (la más característica del género), enemigo/jefe
derrotado, objeto, vida/recurso, palancas. Las llaves-objeto literales (puerta + llave) son
las menos interesantes; preferir **habilidades** porque cambian cómo se juega todo el mapa.

**Cerradura = terreno.** La mejor cerradura es geometría que la habilidad resuelve:

| Habilidad | Cerradura típica |
|---|---|
| Doble salto | Saliente alto, hueco ancho |
| Salto de pared | Pozo/chimenea vertical, pared alta |
| Dash | Hueco muy ancho, paso estrecho bajo el techo |
| Inmunidad | Líquido/zona dañina |
| Rebote hacia abajo | Pinchos, enemigos en fila |
| Romper bloques | Pared/suelo agrietado |

Reglas:
- La superficie debe ser **predecible**: que nunca haya dudas de si un saliente se alcanza
  o no (ver [métricas](metricas-y-camara.md)). Hollow Knight evita plataformas atravesables
  justo para que el terreno funcione como cerradura sin ambigüedad.
- **Cada habilidad abre varios caminos**, no uno. Si solo abre una puerta, es una llave, no
  una habilidad.
- La habilidad debe **abrir espacio nuevo**, no solo facilitar el ya accesible.
- **Cerraduras de conocimiento** (Animal Well): el jugador vuelve porque ahora *entiende*
  algo, no porque tenga un objeto nuevo. Muy potentes y baratas.

**Cerraduras blandas** (*soft locks*): no impiden pasar, pero lo hacen improbable (camino
difícil, enemigo fuerte, objeto caro). Permiten variar ligeramente el orden sin romper el juego.

## 2. Grafo de dependencias

Mark Brown (Boss Keys) dibuja qué habilidad abre qué zonas:
- **Ancho** (Metroid NES): mucha libertad, pero fácil perderse.
- **Alto y estrecho** (Zero Mission): lineal, guiado, poca exploración.
- Objetivo habitual: **estrecho al principio** (enseñar), **más ancho en el centro**
  (libertad), y converger hacia el final.

Diseñar primero el grafo (habilidades y bloqueos), **después** las salas.

## 3. Herramientas de flujo

- **Puertas de un sentido / punto de no retorno:** una caída, un hueco por el que cabes pero
  no vuelves (Metroid Dread), una puerta que solo abre desde el otro lado. Enfocan al jugador
  en una región y evitan que se pierda.
- **Puertas que se abren desde el otro lado** (Hollow Knight): el jugador ve la palanca tras
  la puerta → sabe que dará la vuelta y volverá por ahí. Se convierten en atajo permanente.
- **Bloqueo del camino directo:** si el jefe está bloqueado, el rodeo obligatorio pasa por
  contenido importante (el cartógrafo, una tienda) y no obliga a desandar lo mismo.
- **Trampa de habilidad:** entras a una zona (caída) y solo sales con la habilidad que hay
  dentro. Garantiza que el jugador la encuentra (Mantis Claw en Hollow Knight, el pozo del
  salto de pared en Super Metroid).
- **Atajos y bucles:** el mapa debe ser una **red densamente conectada** que vuelve sobre sí
  misma. A medida que crece, los atajos mantienen corto el regreso. Diseñar los atajos como
  parte de la red, no como parche.
- **Recorrido en ambos sentidos:** cada sala se diseña para recorrerse en los dos sentidos
  (y a veces con habilidades distintas): ida de una forma, vuelta de otra.

## 4. Backtracking que no aburre

- Volver debe ser **revisitar con poder nuevo**: la sala vieja se juega distinta.
- Colocar en las zonas viejas **promesas** (items visibles e inalcanzables) que la habilidad
  nueva desbloquea → recompensa inmediata al volver.
- Señuelos: un saliente ahora alcanzable en el camino de vuelta desvía hacia contenido nuevo.
- Recorrer en reverso una sala larga debe ser **más corto o más interesante** que la ida.
- Que el jugador pueda **recordar** dónde estaban las cerraduras: hitos visuales únicos junto
  a cada bloqueo; el mapa del HUD marca puertas/huecos.

## 5. Salas: tamaños y funciones

Super Metroid varía tamaño y propósito: salas grandes de exploración, pasillos de conexión,
pozos verticales, salas pequeñas de premio, salas de guardado. Tipos útiles:

| Tipo | Función |
|---|---|
| Hub / nodo | Varias salidas visibles; se vuelve a él a menudo. |
| Pasillo/conector | Ritmo, pocos elementos, a veces un reto corto. |
| Pozo vertical | Contraste con lo horizontal; suele ser cerradura (salto de pared). |
| Sala de reto | Una idea desarrollada (kishōtenketsu). |
| Sala de premio | Pequeña, al final de una rama; contiene la recompensa. |
| Sala de habilidad | Uso obligatorio inmediato de la habilidad para salir. |
| Sala de descanso/guardado | Calma antes de un pico; señal de peligro cercano. |

## 6. Secuencia y libertad

- Ruptura de secuencia (*sequence break*): se tolera o se premia en juegos con fans
  avanzados, pero **la ruta normal debe funcionar sin ella**. Comprobar que ninguna
  combinación de habilidades deja al jugador atascado sin salida (softlock real).
- Elecciones binarias acotadas (Brinstar): dar opciones, pero pocas, para que no se pierda.

## Referencias cruzadas

- [Guiar al jugador](guiar-al-jugador.md)
- [Métricas y cámara](metricas-y-camara.md)
- [Aplicación a Retrovania](aplicacion-retrovania.md)
