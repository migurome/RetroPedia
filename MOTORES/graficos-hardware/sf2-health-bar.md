---
titulo: "Street Fighter II: Health Bar"
plataforma: [General]
categoria: [graficos-hardware]
tags: [cps1, arcade, street-fighter-2, sprites, ui, health-bar]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/sf2_health_bar/index.html]
---

# Street Fighter II: Health Bar

## Resumen

El artículo (título original "Street Fighter II, subtile accurate animation") explica cómo Street Fighter II logra que la barra de vida (stamina) de los oponentes se anime de forma suave, a nivel de píxel, a pesar de que el hardware de la CPS-1 solo puede manipular tiles de 16x16 píxeles. Es relevante como caso de estudio de un problema muy común en motores de tiles: cómo dar granularidad fina a un elemento de UI cuando la unidad mínima de dibujado es mucho más gruesa.

## Detalle técnico

### El problema

Como queda claro en las entradas anteriores de la serie, la máquina que ejecuta Street Fighter II solo puede manipular tiles de **16x16 píxeles**. Sin embargo, la barra de vida ("health bar") de los oponentes se anima suavemente a nivel de píxel individual. ¿Cómo es esto posible?

Si el desarrollador hubiera trabajado con tres tiles (uno para el "borde", uno para "lleno" y uno para "vacío"), una barra de vida de doce tiles solo habría podido cambiar en incrementos de 16 píxeles. El artículo incluye una demostración interactiva que reproduce justamente esa hipótesis "ingenua": con un slider de 0 a 99, se recorre una secuencia de diez tiles centrales (más dos tiles laterales de borde, `side.png`, uno de ellos reflejado horizontalmente) que solo alternan entre imagen "llena" (`f.png`) y "vacía" (`e.png`) en bloques de a uno, produciendo una animación con saltos de 16 en 16 píxeles.

Esta implementación no solo genera una animación entrecortada, sino que transmite una sensación de imprecisión que presenta la mecánica del juego bajo una luz desfavorable ("presents the mechanics of the game in a poor light"). Hacía falta un sistema mejor.

### Solución: seguimiento fino de la stamina

Explorando las hojas de sprites del juego se revela cómo los programadores de Capcom lo lograron funcionar con fluidez. **Los tiles de la barra de vida están ubicados en la sheet `0x81`.**

Observando esa hoja se descubre que no se usan tres tiles, sino **veinte tiles**: una muestra de que "muchos problemas pueden resolverse tirando memoria o almacenamiento" ("a lot of problems can be solved by throwing memory or storage at them").

Los **estados intermedios de un tile están todos pre-renderizados en la fila `0xF0`**. Estos tiles adicionales permiten una animación de barra de vida perfecta a nivel de píxel.

El artículo incluye una segunda demostración interactiva, esta vez con un slider de 0 a 160 (representando 10 tiles de 16 píxeles cada uno = 160 píxeles de resolución total de la barra), cuya lógica de mapeo a sprites es la siguiente (código de la demo, tal como aparece en la página):

```javascript
function sslide_coarse(e) {
  var v = parseInt(e.value, 10);
  var fv = Math.floor(v / 16);

  for (let i = 0; i < fv; i++) {
    var img = document.getElementById("s" + i.toString());
    img.src = "16.png";
  }

  if (v == 160) {
    return;
  }

  if (v % 16 != 0 && v != 0) {
    var img = document.getElementById("s" + fv.toString());
    var id =  (v - Math.floor(v / 16 ) * 16).toString(16);
    img.src = id + "_15.png";
  }

  if (v == 0) {
    fv = fv -1;
  }

  for (let i = fv + 1; i <=9; i++) {
    var img = document.getElementById("s" + i.toString());
    img.src = "0_15.png";
  }
}
```

Esta lógica ilustra el esquema de tiles subyacente: para un valor de vida `v` (0 a 160, sobre 10 posiciones de tile de 16 píxeles cada una):
- Los tiles totalmente cubiertos (índice `0` hasta `fv - 1`, donde `fv = floor(v / 16)`) muestran el tile "lleno" (`16.png`).
- El tile en la posición `fv` (el que está a medio llenar) muestra un tile **pre-renderizado según el resto de la división** — el remanente `v - fv*16` convertido a hexadecimal (`id`) selecciona la imagen `id_15.png` (es decir, existe un tile pre-dibujado por cada uno de los 16 posibles niveles de llenado parcial, de `0_15.png` a `f_15.png`).
- Los tiles restantes (posiciones `fv+1` a `9`) muestran el tile "vacío" (`0_15.png`).

Esto coincide con lo indicado en prosa: en vez de tres estados, la sheet `0x81` contiene un juego de tiles con 16 variantes de llenado parcial (fila `0xF0`) más los estados lleno/vacío/borde, sumando las "veinte" tiles mencionadas por el autor.

### Conclusión de diseño

El método refleja una decisión pragmática y muy propia del hardware de la época: en vez de calcular dinámicamente un degradado de llenado en tiempo real (algo que el motor de tiles de 16x16 no puede hacer de forma nativa), Capcom **pre-renderizó todos los estados intermedios posibles como tiles individuales** y los seleccionó en tiempo de ejecución según el valor exacto de vida restante. Complejidad visual mejorada mediante redundancia de datos precalculados, en lugar de cómputo dinámico.

## Notas / gotchas

- La "resolución de píxel" de la barra de vida es una ilusión de selección de assets: no hay ningún tipo de sub-tile rendering real en el hardware; lo que hay es un tile completo distinto para cada uno de los 16 niveles de llenado parcial posibles dentro de una celda de 16 píxeles.
- Este enfoque cuesta memoria de ROM (17 variantes de un mismo tile en vez de 1) a cambio de fluidez visual — un trade-off directamente aplicable a barras de vida/energía en un motor con tiles de tamaño fijo como Mega Drive.
- El autor señala explícitamente el paralelismo con el efecto "fizzlefade" de Carrier Air Wing (ver `cps1-gfx-system.md`), que usa una filosofía similar de patrones pre-calculados en tiles OBJ para simular granularidad por debajo del tile.

## Referencias cruzadas

- (pendiente de enlazar)
