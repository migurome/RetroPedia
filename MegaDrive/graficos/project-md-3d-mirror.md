---
titulo: Efecto de espejo 3D de Project MD
plataforma: [MegaDrive]
categoria: [graficos]
tags: [efecto, pseudo-3d, prioridad, sprites, scroll, tecnica-visual]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/mirror/project-md-3d-mirror (mirror de un artículo original de Sik en Gamasutra, publicado el 2016-02-02: https://www.gamasutra.com/blogs/JavierDegirolmo/20160202/264791/Project_MDs_3D_mirror_effect.php)]
---

# Efecto de espejo 3D de Project MD

## Resumen

Esta página es un mirror alojado en Plutiedev de un artículo de terceros ("Sik", pseudónimo de Javier Degirolmo) publicado originalmente en Gamasutra en 2016. Diseca la técnica de pseudo-perspectiva 3D con reflejo en el suelo del primer nivel ("Infinite Field") de _Project MD_, un plataformero homebrew inacabado para Mega Drive con una estética similar a Pulseman. El efecto combina scroll por línea (para simular perspectiva) con una composición muy enrevesada de prioridades de plano A/B y sprites para lograr los reflejos.

## Detalle técnico

### Resumen del hardware usado

El nivel es notorio porque tiene un reflejo sobre un suelo 3D. Mucha gente lo considera impresionante, pero el autor original resta importancia al coste de CPU: lo interesante es lo enrevesado del montaje de capas.

La Mega Drive cuenta con cuatro capas además del color de fondo:

- Scroll plane A
- Scroll plane B
- Window plane
- Sprites

(el plano window no se usa en el juego, así que el autor no lo vuelve a mencionar)

Además, cada tile de los planos de scroll y cada sprite tiene un bit de prioridad. Las capas de alta prioridad aparecen por encima de las de baja prioridad. Cuando dos capas con la misma prioridad se superponen, se ordenan así: background < plane B < plane A < sprites. Además, los sprites se ordenan según su posición en la tabla de sprites (los sprites anteriores aparecen sobre los posteriores).

### El HUD

El HUD son simplemente sprites de alta prioridad y los primeros de la lista, por lo que aparece por encima de todo lo demás, sin más complicación.

### El suelo 3D

El suelo 3D es solo un gráfico que ya tiene la perspectiva dibujada, y el efecto 3D se logra cambiando su valor de scroll en cada línea (el hardware ofrece una tabla pensada justamente para esto), de modo que las líneas de arriba se desplazan más despacio que las de abajo.

Además, para dejarlo claro, el cálculo es absurdamente simple porque la deformación es lineal: para cada línea simplemente se suma un valor delta fijo (calculado internamente en punto fijo, ya que necesita precisión de subpíxel, pero la idea básica es esa). Esto es trivial para el 68000.

### El reflejo en primer plano

"Aquí es donde empieza el desastre" (cita del autor original).

Los objetos y los elementos de decorado del escenario (bloques, etc.) son sprites de alta prioridad y plano A respectivamente. Sus reflejos se hacen dibujándolos dos veces, pero el reflejo va en baja prioridad, volteado verticalmente, y usando paletas más oscuras (bueno, en el caso del decorado del escenario el reflejo forma parte del propio decorado; solo los sprites se dibujan realmente dos veces).

El suelo, en la parte donde ocurre el reflejo, es plano B de alta prioridad. Esto significa que los objetos y el decorado aparecerán por encima de él, pero los reflejos aparecerán por debajo. Los reflejos se ven a través de los píxeles transparentes del suelo. El color de fondo resulta ser negro, lo que junto con los reflejos oscurecidos da la sensación de que el suelo es como un cristal polarizado.

Un truco adicional: la rejilla del suelo está diseñada para que las columnas coincidan (intersecten) con los tiles del decorado. Combinado con los reflejos, esto da la sensación de que el decorado atraviesa la rejilla, lo que refuerza el efecto 3D. Es un truco sutil, fácil de pasar por alto.

### El reflejo en segundo plano

"Pero espera, ¿qué pasa con los árboles del fondo?"

Los árboles también son sprites, pero de baja prioridad (tanto el árbol como su reflejo, que también va volteado verticalmente y oscurecido). Esto es necesario para que aparezcan por debajo del primer plano, pero entonces, ¿cómo aparecen los árboles por encima del suelo?

La prioridad en los tilemaps es por-tile. En esta zona, el suelo tiene baja prioridad por encima de la base del árbol, y alta prioridad por debajo de la base del árbol. Esto hace que el reflejo quede por debajo del suelo, pero el árbol en sí no (ya que los sprites aparecen por encima del plano B por defecto). Esto implica que la base de los árboles debe estar alineada a un límite de tile, ya que esa es la granularidad de la prioridad en los planos de scroll.

¿Cómo se calcula su posición, si el 68000 es malo dividiendo y ese factor de escala probablemente no es una potencia de dos? El autor no recuerda los detalles exactos, pero recuerda que el valor estaba relacionado con 14. Eso suena mal... hasta que te das cuenta de que se puede calcular como 16 - 2, y eso es exactamente lo que hace el juego: usa desplazamiento de bits (bit shifting) para obtener 16 y 2, y luego usa esos subtotales para obtener el valor final (eso sí, el autor reconoce que tuvo suerte de que fuera un valor fácil de calcular).

### Edificios

Por completitud: los edificios están simplemente en el plano B (por encima del suelo), y esta zona hace scroll mucho más despacio que el resto. Eso es todo.

### Conclusión

Desde el punto de vista de la CPU, este efecto no es realmente impresionante, y tampoco exige mucho al hardware de vídeo. Sin embargo, requiere una configuración de capas realmente enrevesada, lo bastante compleja como para dedicarle un artículo entero.

## Notas / gotchas

- El efecto de perspectiva del suelo es puramente lineal (delta de scroll fijo por línea, en punto fijo para precisión de subpíxel); no hay ninguna matemática de perspectiva "real" involucrada, y por eso el coste de CPU es trivial para el 68000.
- El reflejo de primer plano depende de que el suelo (plano B) tenga alta prioridad y sea transparente donde debe verse el reflejo: si el suelo no fuera plano B de alta prioridad con huecos, el truco de "ver el reflejo a través del cristal" no funcionaría.
- El reflejo de fondo (árboles) depende de la granularidad por-tile de la prioridad en los planos de scroll: la base del árbol debe estar alineada a un tile exacto para que el cambio de prioridad del suelo (bajo por encima de la base, alto por debajo) funcione correctamente.
- El cálculo de posición de los árboles de fondo evita la división lenta del 68000 aprovechando que el factor de escala equivale a 16 - 2, resuelto con desplazamientos de bits; el propio autor admite que fue una casualidad afortunada tener un valor tan fácil de descomponer así.
- Es un mirror de contenido de terceros (artículo original de Sik/Javier Degirolmo en Gamasutra, 2016-02-02); el propio autor no proporciona código fuente ni fórmulas exactas más allá de la descripción conceptual anterior.

## Referencias cruzadas

- (pendiente de enlazar)
