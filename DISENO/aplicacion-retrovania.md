---
titulo: Diseño de salas para Retrovania — métricas reales, procedimiento y checklist
plataforma: [MegaDrive]
categoria: [diseno]
tags: [retrovania, md-engine, metricas, checklist, procedimiento]
estado: borrador
fecha: 2026-10-15
fuentes:
  - retrovania-generator/lib/actors.js   # JUMP 330, DOUBLE_JUMP 300, WALL_JUMP 300/110, GRACE 4
  - retrovania-generator/lib/state.js    # physicsMaxVelocityX 110
  - MegaDrive/herramientas/md-engine/fisica-y-colisiones.md   # physicsGravity 1150
---

# Diseño de salas para Retrovania

## Resumen

Esta es la parte **operativa**: los números del héroe de Retrovania traducidos a tiles, el
procedimiento a seguir antes de escribir un `map:` y la checklist que debe pasar toda sala.
La teoría está en las entradas hermanas.

## 1. Métricas del héroe (en tiles de 8 px)

Valores del generador: salto 330 px/s, doble salto 300, salto de pared 300 vertical + 110
horizontal, gravedad 1150 px/s², velocidad X máx. 110 px/s, hitbox 8×12, coyote 4 frames.
Simulado a 60 fps (Euler); el motor puede diferir unos píxeles → **verificar en partida** y
corregir esta tabla si hace falta.

| Movimiento | Máximo físico | Tiempo en el aire |
|---|---|---|
| Salto simple: altura | ~45 px (5,6 tiles) | 0,57 s |
| Salto simple: avance en llano | ~62 px (7,8 tiles) | |
| Doble salto (en el ápice): altura | ~81 px (10,1 tiles) | 0,93 s |
| Doble salto: avance máx. | ~117 px (14,6 tiles) | |
| Salto de pared: altura por rebote | ~37 px (4,6 tiles) | |
| Caer N px: avance horizontal | 32 px → 26 px · 64 px → 37 px · 96 px → 45 px | |

No hay altura variable (soltar el botón no corta el salto): todos los saltos son completos.

### Bandas de diseño (desnivel = de suelo a suelo; hueco = de borde a borde)

| | Seguro (ruta obligatoria) | Exigente (opcional con premio) | **No usar** (ambiguo) | Imposible claro (cerradura) |
|---|---|---|---|---|
| Subir, salto simple | ≤ 4 tiles | 5 tiles | 6 tiles | ≥ 7 tiles |
| Subir, doble salto | ≤ 7 tiles | 8–9 tiles | 10–11 tiles | ≥ 12 tiles |
| Hueco, salto simple | ≤ 5 tiles | 6 tiles | 7–10 tiles | ≥ 11 tiles |
| Hueco, doble salto | ≤ 10 tiles | 11–12 tiles | 13–16 tiles | ≥ 17 tiles |

**Cerraduras de doble salto** (imposible sin él, seguro con él):
- Saliente a **7 tiles** de altura.
- Hueco de **11 tiles** (exigente con doble salto: ponerle premio o red de seguridad).

**Cerraduras de salto de pared:** pared lisa ≥ 12 tiles (fuera del alcance del doble
salto) o pozo cuya salida esté a ≥ 12 tiles del suelo. Chimenea para subir rebotando:
**hueco interior de 3–5 tiles** (pendiente de verificar en partida).

**Espacio libre:** sobre cualquier sitio desde el que se salta, ≥ 7 tiles de aire (salto +
cuerpo) o el techo corta el salto. Con doble salto disponible, ≥ 12 tiles si se espera que lo use.

**Aterrizajes:** plataforma de ≥ 4 tiles en la ruta obligatoria; 2–3 tiles solo en tramos
de precisión intencionados; 1 tile nunca tras un salto largo.

## 2. Pantalla y cámara

- Pantalla 40×28 tiles (320×224, H40). HUD en las 3 filas superiores; techo de 1 tile; texto
  en filas 1–2. Vertical visible útil ≈ **25 filas**.
- En salas largas (`camera.deadzone`), el héroe ve más por delante que por detrás: los
  peligros de la ida se ven a tiempo, pero los de la **vuelta** (recorrido en reverso) tienen
  menos margen → en la vuelta, nada peligroso pegado al borde izquierdo de pantalla.
- Un salto (5,6 tiles) cabe en la deadzone vertical de pantalla completa: no hay que temer
  movimientos de cámara al saltar en salas de 1 pantalla de alto.
- El primer peligro de un tramo debe entrar en pantalla con ≥ 1 s de antelación
  (≥ ~14 tiles a velocidad máxima).

## 3. Procedimiento para crear una sala (antes de escribir el `map:`)

1. **Función en el mundo.** ¿Qué tipo de sala es? (hub, conector, pozo, reto, premio, sala
   de habilidad, descanso). ¿Qué habilidades tiene el jugador al llegar y cuáles al salir?
   ¿Se recorre en ambos sentidos? Anotarlo en el comentario de cabecera del archivo.
2. **Una idea** (kishōtenketsu). Escribir las 4 fases con pantallas concretas:
   `Ki` pantalla 0: presentación segura · `Shō` 1–2: desarrollo · `Ten` 3: giro · `Ketsu` 4: cierre.
3. **Grafo local.** Entradas, salidas, cerraduras (con la habilidad que las abre) y
   promesas (lo que se ve y no se alcanza todavía).
4. **Curva de intensidad.** Una cifra 1–10 por pantalla; diente de sierra, pico cerca del final.
5. **Esbozo con métricas.** Colocar suelos y desniveles usando solo las bandas de la tabla.
   Ruta obligatoria = banda "seguro". Opcional = "exigente". Cerradura = "imposible claro".
6. **Colocar elementos** con las reglas de [guiar al jugador](guiar-al-jugador.md):
   salida u objetivo visible al entrar, una cosa interesante por pantalla, premios al final
   de desvíos, bloqueo visto antes que la llave, uso obligatorio justo tras la llave,
   un hito por sala.
7. **Recorrer mentalmente** la sala en los dos sentidos y con cada conjunto de habilidades
   posible (sin nada, +doble, +pared, ambas): ¿se puede quedar atascado? ¿se puede llegar
   a algo antes de tiempo?
8. **Escribir el archivo** y pasar la checklist.

## 4. Checklist de toda sala

**Lectura**
- [ ] Al entrar se ve una salida, un objetivo o una dirección clara.
- [ ] Ninguna medida cae en la columna "no usar".
- [ ] Lo que parece suelo es suelo; lo inalcanzable es promesa a propósito.
- [ ] Hay al menos un hito reconocible (forma única) en cada sala grande.

**Ritmo**
- [ ] La sala tiene una idea y sus 4 fases; no hay pantallas sin propósito (salvo calma intencionada).
- [ ] La curva de intensidad sube en diente de sierra; checkpoint antes de cada pico.
- [ ] Ningún elemento nuevo aparece por primera vez en una situación peligrosa.

**Exploración**
- [ ] Todo callejón sin salida tiene premio.
- [ ] Cada premio opcional se ve antes de arriesgarse por él.
- [ ] Cada cerradura se ve antes de tener su llave, y está junto a un hito memorable.
- [ ] Tras cada habilidad, hay un uso obligatorio inmediato y al menos una promesa antigua
      que ahora se abre (regla del usuario: gema solo alcanzable con el nuevo poder).
- [ ] No hay atajo de vuelta tras un power-up (regla del usuario): la vuelta recorre la
      sala en reverso, y el reverso es interesante (otros saltos, la gema, otra vista).

**Seguridad**
- [ ] Ninguna combinación de habilidades deja al jugador atrapado sin salida.
- [ ] Spawns de llegada lejos de las puertas (si no, el héroe rebota de vuelta).
- [ ] Caídas a fosos con checkpoint cercano; no hay caídas a ciegas en la ruta obligatoria.
- [ ] Aire libre suficiente sobre los puntos de salto (sin golpes de cabeza involuntarios).

## 5. Patrones listos para usar (gramática del `map:`)

`.` aire · `#` sólido · `X` exterior/vacío. Cada carácter = 8 px.

**Escalón seguro (4 tiles) — ruta obligatoria**
```
..........
......####
......####
......####
......####
##########
```

**Cerradura de doble salto (saliente a 7 tiles) con promesa visible**
```
.....G....     G = gema/objeto encima, visible desde abajo
...#######
...#######
..........
..........
..........
..........
..........
##########
```

**Introducción segura de un hueco (Ki): el foso tiene fondo a 2 tiles; fallar no mata**
```
................
................
#####.....######
#####.....######
################
```
Después (Shō), el mismo hueco sin fondo; luego (Ten), el hueco con un saliente arriba.

**Chimenea de salto de pared (interior 4 tiles), salida a ≥ 12 tiles**
```
##....##
##....##
##....##
##....##   (repetir hasta ≥ 12 filas)
##....##
```

## Notas / gotchas

- Si se cambian `JUMP`, `DOUBLE_JUMP`, la gravedad o `physicsMaxVelocityX`, **recalcular la
  tabla** (script de simulación: integrar `vy += g·dt; y += vy·dt` a 60 fps).
- Las salas existentes (S1) se diseñaron antes de esta guía: revisarlas contra la checklist
  antes de ampliarlas.

## Referencias cruzadas

- [Principios](principios-diseno-niveles.md) · [Guiar al jugador](guiar-al-jugador.md) ·
  [Mundo metroidvania](mundo-metroidvania.md) · [Métricas y cámara](metricas-y-camara.md)
- `retrovania-generator/README.md` — formato de sala y enlaces
