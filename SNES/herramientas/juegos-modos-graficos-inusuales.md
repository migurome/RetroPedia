---
titulo: Juegos de SNES con modos gráficos inusuales
plataforma: [SNES]
categoria: [herramientas]
tags: [bg-modes, modo-7, hi-res, interlace, offset-per-tile, overscan]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Uncommon_graphics_mode_games]
---

# Juegos de SNES con modos gráficos inusuales

## Resumen

Listado de la SNESdev Wiki con ejemplos reales de juegos comerciales que usan modos gráficos poco habituales de la PPU de SNES: Modo 0, Modo 3 y Modo 4 en 8bpp, EXTBG de Modo 7, color directo, alta resolución/entrelazado, offset-per-tile horizontal y vertical, y overscan (239 líneas). Sirve como catálogo de referencias jugables para cada característica poco común del hardware.

## Detalle técnico

### BG Modo 0

Ofrece 4 capas independientes de gráficos en 2bpp.

| Juego | Uso |
|-------|-----|
| Dragon Quest V | Pantalla de título en la secuencia de nueva partida |
| Final Fantasy IV | Pantallas de menú y carga de partida |
| Final Fantasy V | Pantallas de menú y carga de partida |
| Kingyo Chuuihou! Tobidase! Game Gakuen | Animación del logo de Jaleco al arrancar |
| The Simpsons: Bart's Nightmare | Durante el juego |
| Super Mario Kart | Pantalla de selección de piloto con capas separadas de texto y suelo con scroll independiente |
| Star Ocean | Pantallas de menú, incluyendo partidas guardadas y tiendas |
| Tales of Phantasia | Pantallas de menú, incluyendo partidas guardadas y tiendas |
| Yoshi's Island | Parte superior de la pantalla de título; en 6-8 "King Bowser's Castle" hay pilares con parallax de 4 capas |

### BG Modo 3, 8bpp

Modo de gráficos en 8bpp; los casos de color directo se listan aparte.

| Juego | Uso |
|-------|-----|
| Donkey Kong Country | Líneas verdes del logo de Rareware al arrancar |
| Kingyo Chuuihou! Tobidase! Game Gakuen | Uso extensivo para escenas de CG |
| Ms. Pac-Man | Pantalla de título |
| Secret of Mana | Pantalla de título, usando compresión con pérdida similar a JPEG |
| Sim City 2000 | Visualización de la ciudad (la interfaz usa 4bpp) |
| Toy Story | Numerosas pantallas de alto colorido |
| Yam Yam | Pantalla de título |
| Zoop | Una única capa en 8bpp durante el juego |

### BG Modo 4, 8bpp

8bpp en BG1 combinado con offset-per-tile.

| Juego | Uso |
|-------|-----|
| Bust-a-Move/Puzzle Bobble | 8bpp para las combinaciones de color de las burbujas; offset-per-tile vertical para desplazar el área de juego |
| Rock'n'Roll Racing | Pantalla de compra de equipo, sin offset-per-tile |

### BG Modo 7 EXTBG

Variante poco común del Modo 7.

| Juego | Uso |
|-------|-----|
| Contra 3 | Segundo nivel, permite al jugador pasar por debajo de puentes |
| Super Ghouls 'n Ghosts | Nivel giratorio con una capa superpuesta sobre los pies del jugador |
| Super Turrican 2 | Combate contra jefe |
| Tiny Toon Adventures | Pantalla de título |
| extbgtest | ROM de test para las variantes de EXTBG de Modo 7 |

### Color directo

Usa color directo de Modo 7 o de Modo 3.

| Juego | Uso |
|-------|-----|
| Actraiser 2 | Mapa del mundo en Modo 7 |
| Aerobiz | Pantalla de título en Modo 7 |
| Romance of the Three Kingdoms II | Título e introducción en Modo 7 |
| Secret of Mana | Mapa del mundo esférico en Modo 7; mapa del mundo plano en Modo 3 (intercambiable con Start + R) |

### Alta resolución y entrelazado

#### Hi-res general (no texto)

| Juego | Uso |
|-------|-----|
| The Atlas: Renaissance Voyager | Área de estado en alta resolución (Modo 5) en la pantalla de mapa |
| Bishoujo Janshi Suchi-Pie | Modo 5 y entrelazado para los gráficos de entrada de localización |
| Breath of Fire II | Pseudo-hi-res en la intro del pueblo, para sombreado de árboles |
| Chrono Trigger | Entrelazado en la vista de cabina del Epoch al estrellarse contra Lavos (Modos 7 y 1); podría ser jitter intencional |
| Crayon Shin-Chan: Arashi o Yobu Enji | Efecto de neblina gris en pseudo-hi-res en la Fase 2 |
| Donkey Kong Country | Modo 5 para el pequeño logo de Rareware en la esquina inferior derecha |
| Jurassic Park | Pseudo-hi-res en la superposición del HUD y en la caja de notificaciones, para transparencia |
| Kirby's Dream Land 3 | Pseudo-hi-res para transparencia del primer plano |
| Maka Maka | Entrelazado activado durante el juego |
| Porky Pig's Haunted Holiday | Modo 5 para las escenas introductorias en alta resolución |
| Power Drive | Modo 5 y entrelazado para el logo de U.S. Gold y las pantallas de texto |
| Radical Psycho Machine Racing (RPM Racing) | Modo 5 y entrelazado durante todo el juego (la versión japonesa usa baja resolución en su lugar) |
| Street Combat / Ranma ½: Chounai Gekitou Hen | Entrelazado activado (probablemente accidental) |
| PPU Bus Activity Demo | Incluye Modo 6 |
| Two Ship Demo | Comparación de Modo 5 + entrelazado frente a Modo 1 |

#### Hi-res para texto en japonés

| Juego | Uso |
|-------|-----|
| Dark Law: Meaning of Death | Modo 5 para cajas de texto, menús y barra de estado |
| Desert Fighter / Air Strike Patrol | Modo 5 y entrelazado para el texto de brífing de misión |
| Doukyuusei 2 | Modo 5 para cajas de texto y menús |
| Dragon Knight 4 | Modo 5 para cajas de texto y menús |
| Marvelous: Mouhitotsu no Takarajima | Modo 5 para cajas de texto y menús |
| Moryou Senki Madara 2 | Modo 5 para cajas de texto y menús |
| Rudra no Hihou | Modo 5 para cajas de texto y menús |
| Secret of Mana | Modo 5 para la pantalla de menú |
| Seiken Densetsu 3 (Trials of Mana) | Modo 5 para cajas de texto y menús |
| Shinseiki Odysselya II | Modo 5 para cajas de texto |
| Stable Star: Kyuusha Monogatari | Modo 5 y entrelazado en la pantalla de estadísticas del caballo |
| Syvalion | Modo 5 y entrelazado en las pantallas de texto iniciales |
| Tokimeki Memorial: Densetsu no Ki no Shita de | Modo 5 para cajas de texto y menús |

### Offset-per-tile horizontal

Señalado como "especialmente raro", porque los cambios de scroll horizontal por scanline ya permiten lograr un desplazamiento horizontal suave sin necesidad de esta técnica.

| Juego | Uso |
|-------|-----|
| Chrono Trigger | Efecto de brillo tembloroso en la nave Black Omen durante la introducción |
| Super Genjin 2 | Transición de pantalla en la pantalla de título; usa Modo 4 |
| Super Mario All-Stars | Transición de pantalla entre el título y la selección de personaje en Super Mario Bros 2 |

### Offset-per-tile vertical

| Juego | Uso |
|-------|-----|
| Aladdin | Combate final contra el jefe |
| Axelay | Grandes obstáculos con movimiento vertical en el segundo nivel |
| Battletoads in Battlemaniacs | Efecto de bandera ondeante en la intro |
| Bust-a-Move/Puzzle Bobble | Desplazamiento hacia abajo del área de juego sin afectar al marco fijo; también usa Modo 4 |
| Chrono Trigger | Ondulación vertical en el texto de título "Trigger"; texto con scroll en el menú cuando el grupo supera 3 miembros |
| Dragon Quest V | El barco se mueve de forma independiente respecto a la tierra al principio |
| GT Racing | Bandera ondeante en Modo 2 en la pantalla de título |
| Kingyo Chuuihou! Tobidase! Game Gakuen | Bandera ondeante en Modo 2 durante los créditos de copyright y la intro/título |
| Prehistorik Man | Icebergs en la Fase 21, para simular rotación sobre plataformas de hielo |
| Star Fox | Ligera rotación en el fondo de Super FX |
| Super Double Dragon | Ascensor en la segunda área del primer nivel |
| Super Mario RPG | Segmentos de vagoneta en Moleville, para las vías en pendiente |
| Super Turrican 2 | Cuarta área del primer nivel |
| Tetris Attack | Columnas de juego que se desplazan de forma independiente |
| Timecop | Bamboleo de la máquina del tiempo durante la introducción |
| Yoshi's Island | Efecto de la fase 1-7 "Touch Fuzzy Get Dizzy"; plataformas móviles en 6-4 "Tap-Tap The Red Nose's Fort" |

### Overscan (239 líneas)

Alternativa a las 224 líneas estándar; ofrece un periodo de VBLANK significativamente más largo, pero las líneas extra suelen ser invisibles en televisores NTSC de la época.

| Juego | Región / notas |
|-------|-----------------|
| Dragon Quest I & II | NTSC (Japón) |
| Dragon Quest V | NTSC (Japón) |
| Rendering Ranger R2 | NTSC (Japón); a menudo blanquea líneas manualmente; rara vez usa el tiempo en blanco para subidas a la PPU |
| SNES Test Program | Cambia a overscan incluso en consolas NTSC |
| Super Mario All-Stars | Solo versión PAL |
| Super Mario Kart | Solo versión PAL, con letterboxing manual |
| Super Mario World | Revisión PAL tardía (no la PAL de lanzamiento) |
| Super Tetris 2 + Bombliss | NTSC (Japón) |
| Super Tetris 3 | NTSC (Japón) |
| Tetris & Dr. Mario | Versiones NTSC y PAL |
| Tom & Jerry | NTSC y PAL |
| Yoshi's Cookie | NTSC y PAL |

## Notas / gotchas

- El offset-per-tile horizontal es deliberadamente raro porque el scroll horizontal por scanline ya cubre la mayoría de los casos de uso — su presencia en un juego suele indicar una necesidad muy concreta (transiciones de pantalla).
- El overscan (239 líneas) amplía el tiempo de vblank disponible pero a costa de líneas que muchos televisores NTSC de época no mostraban — ver la tabla de presupuesto de vblank en [Timing del sistema SNES](timing.md) para los números exactos de clocks/DMA disponibles según altura de pantalla.
- Varios juegos (Chrono Trigger, Yoshi's Island, Secret of Mana, Kingyo Chuuihou!) aparecen en más de una categoría de esta lista, combinando distintas técnicas gráficas poco comunes.

## Referencias cruzadas

- [Timing del sistema SNES](timing.md)
- [Juegos difíciles de emular](juegos-dificiles-de-emular.md)
- (pendiente de enlazar)
