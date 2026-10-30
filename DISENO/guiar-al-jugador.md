---
titulo: Guiar al jugador — señalización, landmarks y colocación de elementos
plataforma: [General]
categoria: [diseno]
tags: [level-design, signposting, breadcrumbs, landmarks, affordance, lynch]
estado: borrador
fecha: 2026-10-15
fuentes:
  - https://gamedev.net/blogs/entry/2277800-should-players-be-hand-held
  - https://www.gamedeveloper.com/design/level-design-tips-and-tricks
  - https://gamedeveloper.com/design/the-invisible-hand-of-super-metroid
  - https://blog.hslu.ch/games/files/2021/11/Metroidvania_Analysis.pdf   # Hollow Knight, Lena Schütz
  - https://betterworld.mit.edu/images-of-the-city   # Kevin Lynch
  - https://tvtropes.org/pmwiki/pmwiki.php/Main/BreakableWallHint
---

# Guiar al jugador

## Resumen

El jugador explora bien cuando **siempre tiene algo que le llama la atención** y **puede
leer el espacio** (qué es suelo, qué se alcanza, qué está bloqueado). Guiar no es poner flechas:
es colocar cada elemento donde la mirada y la curiosidad del jugador lo encuentren.

## 1. Vocabulario de herramientas

| Herramienta | Qué es | Cómo se hace en 2D/tiles |
|---|---|---|
| **Affordance** | La forma dice qué se puede hacer | Un borde plano = se puede pisar; una pared lisa y alta junto a otra = chimenea de salto de pared. Mantener las reglas **consistentes** (romperlas confunde). |
| **Líneas guía** (*leading lines*) | Formas que llevan la vista | Escalones en diagonal, una fila de plataformas, tuberías, el borde del techo que sube hacia la salida. |
| **Migas de pan** (*breadcrumbs*) | Coleccionables en hilera | Una fila de gemas/monedas marca el arco de un salto o el camino a un secreto. El jugador persigue coleccionables "como migas de pan". |
| **Weenie / landmark** | Objeto grande y visible a lo lejos (término de Walt Disney) | Una torre en el fondo/parallax, una abertura en el techo visible desde el inicio, un objeto que brilla. Se puede "esconder y revelar" desde distintos puntos. |
| **Luz / contraste** | Lo brillante atrae | Con paletas limitadas: el color más claro de la zona = "importante" (salidas, items). No gastarlo en decorado. |
| **Encuadre** (*framing*) | Rodear lo importante | Una abertura en un muro, un hueco en el techo que enmarca la plataforma superior. |
| **Visible pero inalcanzable** | La promesa | Un item a la vista sobre un saliente demasiado alto: el jugador recordará volver. |
| **Enemigos como guía** | Huimos de unos, perseguimos otros | Enemigos que vienen de una dirección "empujan"; un enemigo inofensivo puede demostrar una mecánica. |

## 2. Reglas de colocación (qué poner y dónde)

1. **Primera pantalla de cada sala = lectura clara.** Al entrar, el jugador debe ver al menos
   una salida u objetivo. Si la salida no se ve, que se vea la **dirección** (líneas guía).
2. **Una cosa interesante por pantalla, mínimo.** Si una pantalla no tiene salto, item, vista
   o peligro, el jugador la cruza sin mirar (y eso está bien solo si es un tramo de calma
   intencionado).
3. **El objetivo a la vista, el camino oculto** (Taylor #3). Mostrar la meta desde abajo y
   que el camino serpentee hasta ella.
4. **Items en el borde del espacio conocido.** Coloca recompensas al final de un desvío
   (callejón sin salida = siempre debe premiar), nunca en mitad de la ruta obligatoria donde
   se cogen sin decidir.
5. **Callejón sin salida sin premio = frustración.** Toda rama opcional termina en algo: gema,
   vista, atajo, pista.
6. **Lo que se ve, se puede alcanzar (algún día).** No dibujes plataformas inalcanzables
   "porque sí": el jugador las anotará como promesa. Si es decoración, que no parezca suelo.
7. **Presenta el bloqueo antes que la llave.** El jugador debe ver el saliente alto **antes**
   de tener el doble salto, para que al obtenerlo piense "¡ya sé dónde usarlo!".
8. **Justo después de la llave, un uso obligatorio.** Hollow Knight: tras la habilidad, el
   jugador encuentra enseguida obstáculos que solo se pasan con ella (experiencia de primera
   mano, inmediata). Con la primera habilidad, le pone delante el mismo tipo de bloqueo que
   vio antes en otro sitio.
9. **Símbolos asociados.** Si un elemento visual va ligado a una habilidad (cristales ↔
   Crystal Heart), ponerlo en otros sitios indica "usa la habilidad aquí". Solo funciona si
   el símbolo se distingue bien del decorado.
10. **Secretos con pista.** Paredes rompibles o pasos ocultos siempre con una señal sutil y
    consistente (grieta, tile distinto, un hueco en el techo de la sala contigua). Si no hay
    pista, el jugador acaba golpeando todas las paredes. Y no abusar: si todo puede ser secreto,
    nada lo parece.
11. **Distancia y ángulo.** Mostrar un objetivo desde lejos y desde abajo genera curiosidad;
    desde arriba da información (planificación). Alterna ambos.

## 3. Técnicas de Super Metroid (el "invisible hand")

- **Forzar la primera dirección** con un muro (el inicio en Crateria obliga a ir a la izquierda).
- **Dos puertas, una ya abrible con lo que acabas de conseguir:** el jugador elige la nueva
  por curiosidad.
- **Bloqueos que se entienden y se aplazan:** un pasillo de pinchos con la puerta abierta,
  pero que requiere el gancho: se ve, se recuerda, se vuelve.
- **Bloques fuera de sitio** + suelo desconectado + un trozo de techo de otra sala visible =
  "aquí hay algo, dispárame".
- **Reutilizar enemigos/salas** para enseñar: el bicho invencible de antes vuelve donde
  congelarlo sirve de plataforma.
- **Punto de guardado como aviso** antes de un peligro.
- **Encerrar para enseñar** (salto de pared, carrera): sala segura de la que solo se sale
  usando la técnica, con un "maestro" que la demuestra.
- **Puertas traseras:** el juego no depende de que el jugador coopere; siempre hay una salida
  alternativa si no descubre el truco "oficial".
- **Señuelo de vuelta:** al regresar a una zona vieja, un saliente ahora alcanzable distrae
  al jugador hacia contenido nuevo.

## 4. Imageabilidad: Kevin Lynch aplicado a mapas

Lynch (The Image of the City, 1960) descubrió que la gente construye su mapa mental de una
ciudad con cinco elementos. En un mundo de juego:

| Lynch | En el juego | Regla |
|---|---|---|
| **Caminos** | Ruta crítica, pasillos | Que la ruta principal sea continua y reconocible (mismo suelo, misma altura base). |
| **Bordes** | Paredes, fosos, cambios de zona | Bordes claros delimitan zonas; un borde de agua/vacío "dice" que ahí acaba algo. |
| **Distritos** | Zonas con identidad (aquí: cada consola) | Cada zona con estilo, paleta y forma propios: el jugador sabe dónde está con un vistazo. |
| **Nodos** | Cruces, salas hub, salas de guardado | Pocos y memorables; desde un nodo se ven varias salidas. |
| **Hitos** (*landmarks*) | Objetos únicos | Al menos uno por sala grande: una columna colgante, una estatua, un hueco con forma rara. Sirven para orientarse y para recordar "el bloqueo que estaba junto a X". |

## 5. Mapa y HUD como guía

- Hollow Knight solo dibuja en el mapa lo visitado: muestra qué queda por explorar sin
  revelar los secretos.
- Marcar en el mapa salas importantes (jefe, objetivo) informa sin obligar.
- Hollow Knight usa caminos iluminados con antorchas en una bifurcación: el camino nuevo
  destaca frente al ya conocido.

## Notas / gotchas

- **Demasiada guía** (todo brillante, flechas) se siente como llevar de la mano. Usar la
  señal más débil que funcione.
- Con paleta de 2 colores (Pong) no hay "luz": se guía con **forma** (huecos, encuadres,
  hileras) y **movimiento** (objetos que parpadean/se mueven).

## Referencias cruzadas

- [Principios de diseño de niveles](principios-diseno-niveles.md)
- [Diseño de mundo metroidvania](mundo-metroidvania.md)
- [Aplicación a Retrovania](aplicacion-retrovania.md)
