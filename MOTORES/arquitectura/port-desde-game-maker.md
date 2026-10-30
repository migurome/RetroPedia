---
titulo: Portar un juego de Game Maker a una consola retro (caso Super Smash Land → Mega Drive)
plataforma: [General, MegaDrive]
categoria: [arquitectura]
tags: [port, game-maker, gml, punto-fijo, sprites, escenarios, sonido, pruebas, md-engine, sgdk, ia]
estado: borrador
fecha: 2026-10-23
fuentes: [port de Super Smash Land (Game Maker 8, 4x sobre 160x144) a Mega Drive con SGDK y MD Engine 1.1.0, builds B001-B040; PLAN_PORT_MEGADRIVE.md del proyecto]
---

# Portar un juego de Game Maker a una consola retro

## Resumen

Un juego de Game Maker (GM) guarda su lógica en scripts GML y eventos de objeto, y sus datos en sprites, rooms y `ds_grid`. Para llevarlo a una consola:
- La lógica se reescribe en C con **las mismas unidades y los mismos pasos** que el original.
- Los datos se convierten con una herramienta que **lee el proyecto extraído**, sin pasarlos a mano.

Esta entrada recoge lo que hizo falta (y lo que falló) en el port de Super Smash Land a Mega Drive.

## Detalle técnico

### Arquitectura que funcionó

- **Núcleo en C independiente de la plataforma** (`core/ssb_core.c`) más **adaptadores** finos: un banco de pruebas en SGDK puro y un plugin de MD Engine. El núcleo llama a `ssb_platform_draw / _text / _sfx / _music / _set_stage…`.
  - El banco compila en segundos y sirve para probar sin el editor.
  - MD Engine aporta escenas, recursos y el driver de sonido.
- **Los datos los genera una herramienta** (`tools/build_proto.py`) a partir del proyecto extraído: hojas de sprites, tablas de ataque, colisión de escenarios, sonidos y el `gen_data.c`. Valida que sus listas coinciden con los `enum` del C.

### Unidades y tiempo

- **Se conservan las unidades del original.** GM dibujaba a 4x (160x144 escalado), así que las posiciones se guardan en píxeles GM con punto fijo 8.8 y solo se divide entre 4 al dibujar. Las constantes del GML (gravedad, knockback, `x-170` para el KO…) se copian tal cual.
- **Un paso de GM es un fotograma** (`room_speed` 60):
  - `alarm[n] = 30` son 30 fotogramas.
  - `image_speed = .031` avanza 0,031 fotogramas de animación por paso; una animación de 4 fotogramas dura 4 / 0,031 ≈ 129 pasos.
  - **`room_speed = 15`** (cámara lenta del final de partida) se imita actualizando la lógica uno de cada cuatro fotogramas.
- Las animaciones que terminan algo (`other_animation_end`) y las alarmas están en los **eventos del objeto**, no en los scripts. Hay que leerlas: la cuenta atrás «3-2-1 GO» arranca la música en el evento de fin de animación.

### Sprites

- **Bajar de 4x a 1x** solo es exacto si la rejilla de 4x4 del dibujo empieza en (0,0). Algunos sprites están desfasados (`link_uground2`, `pika_hurt`, suelos de Clock Town): hay que detectar la fase de la rejilla antes de muestrear.
- **Un origen que no es múltiplo de 4** deja un resto que desplaza la fila 1 píxel. Se guarda ese resto por fila (`row_dy`) y se descuenta al dibujar.
- **Cada estado tiene su `sprite_index`.** Si se olvida uno, el error se nota en el juego, no al compilar. Ejemplo: al reaparecer, el original usa `<pj>_spawn` (el personaje sobre un halo flotante). Sin él, el luchador parecía flotar en el aire (fallo reportado en Lavender).
- **La animación se decide como en el original, no por intuición.** Aquí la de quieto exige velocidad 0 **y** no pulsar izquierda ni derecha (`<pj>Step.gml`). El port la ponía con solo velocidad 0. Empujando una pared, la velocidad vuelve a 0 cada fotograma y el sprite alternaba entre quieto y andar (Kirby en Peach Castle, B017). En el original se sigue andando en el sitio.
- **Paletas:** en Mega Drive hay 4 paletas de 16 colores. Todas las hojas comparten la paleta base, y el color de cada luchador se carga en PAL1/PAL2 según el personaje elegido. Así caben 6 personajes.
- **Cuatro luchadores con 3 paletas libres** (Super Smash Land MD, B043). PAL0 es del escenario; cada personaje solo usa 5 índices (los 4 tonos de Game Boy y uno extra).
  - Dos luchadores comparten PAL3: el tercero en los índices 1-5 y el cuarto en los 6-10.
  - El cuarto usa **copias de sus hojas con los índices +5**, generadas al exportar: personajes, proyectiles, retratos y la foto de la selección.
  - Las copias van comprimidas (LZ4W): ~190 KB sin comprimir, que no cabían en la ROM. Solo el cuarto paga la descompresión al cambiar de fotograma.
  - Cada luchador carga solo sus 5 colores (`PAL_setColors` / `TBCE_PaletteSetColors` desde el índice de su mitad), para no pisar al otro.
  - Descartados: sombra y resalte, que solo aclara u oscurece lo que hay, y cambiar la paleta a mitad de pantalla, que no sirve con luchadores que se mueven libremente.

### Escenarios (rooms)

- **Imagen:** se compone en el orden de dibujo de GM: fondos, instancias y tiles ordenados por `depth` (mayor depth, más al fondo) y primeros planos. Los objetos móviles se hornean en su posición inicial; es una limitación conocida.
- **Primer plano:** lo que tiene `depth` menor que la del jugador (`obj_player` tiene 0) y los fondos `foreground` tapan a los personajes. Se saca a una segunda imagen con transparencia y va en el plano A con prioridad alta, con los personajes en prioridad baja. La imagen completa del escenario puede seguir incluyéndolo, porque el plano A lo vuelve a pintar encima.
- **Caminos (paths) de GM:** `path_start(camino, velocidad, ...)` mueve la instancia por la línea (o, si el camino es suavizado, por la curva) a velocidad constante. GM 8 suaviza con una curva cuadrática por cada punto interior, que va del punto medio de un tramo al del siguiente con el punto como control, partida en 2^precisión trozos. Lo más barato en consola es muestrear el camino en el generador a un punto por unidad de recorrido; el juego solo suma la velocidad a una distancia y lee el punto.
- **Quién mueve a quién:** en el motor de plataformas de Jenner, las plataformas no arrastran a nadie; es el jugador quien, al comprobar el suelo, se engancha a la plataforma móvil (`on_move`) y suma su velocidad. Sin eso, en una plataforma que baja 1 por paso el jugador queda en el aire cada paso y cae a tirones. El bloque sólido que se desliza (`solidMove`) se trata aparte, al final de `movement_scr`: lleva, empuja y saca.
- **Partes móviles:** los objetos con `step` propio (plataformas que se deslizan, suben o giran) se sacan de la imagen fija y de la colisión estática, y van como sprites con su lógica en C.
  - Sus límites suelen ser **marcadores invisibles** de la sala (`obj_marker`): `place_meeting` con ellos les da la vuelta. El generador puede calcular los topes de antemano moviendo la máscara contra los marcadores, y el juego solo compara con dos números.
  - Las velocidades de cada instancia están en su **código de creación** en la sala (`vsp = 1;`, `dir = 90;`), no en el objeto.
  - Una **lista única de plataformas activas** (fijas, móviles y temporales) evita repetir el cambio en cada consulta.
  - En consola los sprites añadidos antes se pintan delante: hay que crear los de las piezas después de los personajes.
  - Los índices de actor de MD Engine tienen que ser iguales en todas las escenas, así que se reserva siempre el máximo de piezas.
- **Objetos que cambian de estado sin moverse** (ladrillos rompibles): en GM cambian `sprite_index` a uno con máscara vacía (`clear_hitbox`) y una alarma los repone. En el port son una pieza más con un indicador de sólido; al quitarla de la lista de sólidos deja de chocar y de dibujarse.
  - Al añadir sólidos dinámicos hay que medir otra vez: con 8 ladrillos comprobados en cada paso de 1 píxel, Underground pasó de caber a gastar 209 líneas. Una caja envolvente de todos los sólidos móviles lo devuelve al presupuesto.
  - Las partículas de adorno (trozos que caen) pueden reutilizar los huecos de proyectil con un tipo que no golpea, en lugar de reservar sprites propios.
- **Una pieza entre dos capas del fondo** (la luna de Clock Town, depth 40, entre el cielo y la torre de depth 20): en Mega Drive un sprite va delante o detrás de todo un plano, no en medio. Si se mueve por un recorrido fijo, el generador puede recortar de antemano lo que la tapa en cada posición de píxel donde se solapan (un mapa de profundidades al componer la sala dice qué tapa cada píxel). Solo hacen falta fotogramas donde hay solape: 34 de 88x88 en lugar de uno por posición.
- **Recortar a la vista del original:** GM solo pinta lo que está dentro de la vista; en consola, un sprite fuera del campo de 160x144 se ve sobre el marco. Rellenar el plano A fuera del campo con un tile opaco del color del fondo y prioridad alta lo tapa sin gastar sprites. SGDK trae 16 tiles de sistema de un solo color (`TILE_SYSTEM_INDEX + i` = color i) que sirven para esto.
- **Temblor de pantalla** (`screenShake_obj`): mueve `view_xview` al azar ±x durante y pasos. A 4x, los 5 píxeles del KO son ~1 píxel de consola.
- **Colisión:** las máscaras de los `par_block` de la sala se unen y se pasan a trapecios exactos. Hay que portar la lógica de rampas: ver [Colisión con máscaras de píxel: trapecios y rampas](../../GLOSARIO/mask-collision-slopes.md).
- **Plataformas atravesables** (`par_jumpthrough`): el original usa `collision_line` en la y del personaje con la caja del sprite. Su borde superior es la `y` de la instancia, no la de la caja.
- **Salidas:** las instancias `obj_player` con `player = N` en su código de creación.

### IA de la CPU

- **La IA de GM suele sustituir a las teclas por variables propias.** En SSB, `obj_player` llama a `movement_scr(ai_walk = -1, ai_walk = 1, ai_jump, ai_djump, ...)` en lugar de `movement_scr(leftKey, rightKey, jumpKey, jumpPressed, ...)`. Algunas acciones, como cargar un ataque, las hace poniendo `attackKey` a mano.
  - En el port, la CPU **fabrica un mando virtual** cada paso: botones mantenidos y botones pulsados en ese paso. Ese mando entra al mismo código que el de una persona.
  - Así, todo lo que cada personaje hace con «ataque mantenido» o «ataque pulsado» funciona igual para la CPU, sin código aparte.
  - Lo que el GML hace distinto se respeta. El ataque lo elige la CPU (`cpuAttack.gml`) y se pide con la misma función que valida los de las personas. «Abajo» solo cuenta para la DI cuando sube rápido (`vsp < -2`).
- **Orden de eventos en GM 8:** las alarmas van antes del Step. La decisión de atacar (`alarm[3]`) se evalúa antes de `ai_step`.
- **El orden dentro del Step también cuenta.** En SSB, `cpuAttack` (dentro de `attackScript`) va **después** de `movement_scr`: si la CPU salta en ese paso, ya está en el aire al elegir y escoge un ataque aéreo.
  - El port la llamaba antes de moverse, donde empiezan los ataques de las personas. Elegía uno de suelo y luego saltaba, así que hacía un ataque de suelo en el aire.
  - Pikachu soltaba el rayo saltando, pasaba 70 pasos sin control y se caía. Solo se vio en MD Engine, con otras tiradas al azar.
  - Copiar el orden de llamadas del Step tal cual, aunque el código de las personas vaya en otro sitio.
- **`floor(random(n)) = 0`** es una probabilidad de 1/n por paso. Con `ai_diff` 30, 15 y 5 (niveles 1-3), «saltar si el rival está más alto» va del 3 % al 20 % de los pasos.
- **La IA lee estados del jugador que el port puede no tener.** Antes de portarla, comprobar que existe cada variable que consulta.
  - En SSB faltaba `fall_state`: la caída indefensa tras el ataque de recuperación, sin atacar ni doble salto hasta tocar suelo y con aterrizaje lento.
  - Sin ella, una persona podía encadenar supersaltos sin límite, y la IA no habría sabido cuándo dejar de intentarlo.
- **Probar la IA:** es aleatoria, así que las pruebas comprueban hechos con margen: se mueve, ataca, hace daño y no se cae sola en 30 s.
  - Para la recuperación conviene **teletransportar** al luchador escribiendo su `x, y` en la RAM en un fotograma concreto, y mirar si vuelve desde un punto fijo con cada personaje.

### Sonido

- GM 8 usaba una DLL de sonido (`SS_PlaySound`) con dos canales de efectos, y un efecto nuevo corta al más antiguo. Se imita con los dos canales PCM libres de XGM2.
- La música en PCM ocupa mucho: 6 temas a 6,65 kHz son 2,3 MB, y 13 temas son 2,8 MB. Conviene decidir pronto si va PCM completo (menos temas) o se recompone para FM.

### Pruebas

- **Batería automática que lee la RAM** (ver [depuración de MD Engine](../../MegaDrive/herramientas/md-engine/depuracion-y-generacion.md)). Cada fallo reportado por el jugador se convierte en una prueba: reaparición, rampa, borde, resultados.
- **El usuario prueba en BlastEm:** el emulador WASM tolera errores que el hardware no.

## Notas / gotchas

- **Acciones D&D convertidas a texto:** en GM8 un evento es una lista de acciones, y una acción de código es *una* acción. Detrás de una condición D&D, de un `Else` o de un `Repeat`, todo su código depende de ellos. Si el exportador pega el código sin llaves, parece que solo la primera sentencia está dentro. Aquí hizo creer que el cohete de Wily anulaba su velocidad en cada paso (caía 0,2 por paso dentro de la torre) cuando eso solo pasa al explotar. Si un comportamiento leído parece absurdo en el juego, revisar primero cómo se extrajo el código.
- **No inventar comportamiento: leer el GML.** En SSB se descubrió así que durante la cuenta atrás ya se puede jugar (solo se para el reloj), y que el KO cuenta para el último que te golpeó, que se borra al tocar suelo (`playerHurt`).
- **Quirks del original que conviene conservar:** la variable de carga del ataque abajo de Link no se reinicia nunca en el original. Si se corrige, hay que documentarlo como decisión propia.
- **Rendimiento:** el motor de plataformas de GM se mueve de píxel en píxel y consulta todos los sólidos en cada paso. En un 68000 hay que vigilar cuántos sólidos hay y medir cada escenario.
- **`%` y productos de 32 bits en el 68000:** gcc los convierte en llamadas a `__modsi3` / `__udivsi3` / `__mulsi3`, de cientos de ciclos cada una. Las 4 piezas del reloj de Clock Town (`(dir + 719) % 720`, `SIN_TABLE[a % 360]`, `s32 * 114`) gastaban 37 líneas de pantalla por fotograma. Con resta condicional y `(s32)s16 * (s16)k`, que sale como un `muls.w`, bajó a ~0. Conviene buscar `jsr __` en el `-S` de las funciones que corren cada fotograma.
- **Generalizar de 2 a N jugadores sin perder rendimiento** (SSB B042):
  - **Restar punteros es dividir:** `p - array` divide entre el tamaño del struct. Si no es potencia de 2, en el 68000 es una llamada de cientos de ciclos. Guardar el índice en el propio struct (`p->index`).
  - **Indexar multiplica:** `&array[i]` en un bucle multiplica por el tamaño. Es más barato recorrer con un puntero que avanza y una marca en el struct (`p->in_fight`).
  - **Saltar el bucle de rivales** cuando el jugador no ataca (ni lleva escudo): casi siempre.
  - **Ocultar una sola vez** lo que no se usa (luchadores apagados, retratos), no en cada fotograma.
  - **Buscar lo que es común a todos una vez por paso**, no por cada jugador. Ejemplo: los cohetes entre los proyectiles.
  - Con eso, el núcleo para 4 jugadores costó lo mismo que el de 2 jugando a 2 (Underground: 122,8 líneas de media en los dos). Sin ello había subido a 132.
  - Para comparar, compilar el banco con el núcleo anterior y perfilar los dos con el mismo guion.
- **Sonido en MD Engine:** `XGM2_isPlayingPCM` pide el bus del Z80 en cada llamada. Consultar dos canales por efecto hacía que un golpe costara ~30 líneas. Calcular cuándo acaba cada efecto a partir de su tamaño (13 300 Hz, ~222 bytes por fotograma) evita la consulta.
- **Coste de llamar en el 68000:** una función pequeña llamada 16 veces por fotograma, aunque no haga nada, puede costar ~20 líneas (guarda y restaura registros, pasa argumentos por la pila). Antes de recorrer todos los huecos fijos (16 piezas), recorrer solo los que el escenario usa; las comprobaciones de caché, en línea.
- **Precalcular en el generador lo que es fijo:** los bordes de un trapecio inclinado fila a fila (una tabla en ROM) en vez de `e + d*k >> 8` con `muls` en cada consulta; Air Ride bajó ~40 líneas en las carreras por las rampas. Las divisiones por constantes pequeñas, con recíproco y corrección (exacto para |v| < 65536).
- **Consultas que nunca pueden dar positivo:** una plataforma que solo sube y baja por una columna sin sólidos no necesita mirar la colisión en cada paso. Se calcula al empezar la sala y se ahorra la consulta.
- **`image_index > n` con una animación de n fotogramas nunca se cumple:** GM da la vuelta a `image_index` al llegar al final.
  - En SSB, el doble salto se da por acabado con `image_index > 5`. El de Mega Man tiene 5 fotogramas, así que su `djump` sigue a 1 hasta aterrizar.
  - Por eso su CPU nunca usa Rush para volver tras un doble salto. Es fiel al original y se conserva.
- **Variables globales que no se reponen al cambiar de sala:** `global.recoveryWidth` solo se cambia en algunas salas. En las demás se queda el valor de la última partida. El port usa el de `gameStart`, el de la primera partida.
- **Erratas que anulan una parte de la IA:** `ai_step` pone `alarm[5]` para darse la vuelta al chocar con una pared, pero ese código está en `alarm[7]`, así que nunca ocurre.
- **Bugs del original que anulan un efecto:** el soplido del árbol de Dreamland (`leaf_blow_box`) lee una variable que no tiene y se destruye en su primer paso. En la práctica el viento no existe. Antes de portar un efecto, comprobar que de verdad funciona en el original.
- **Nombres y símbolos de las herramientas de destino:** MD Engine corta los símbolos de sonido a 27 caracteres y solo compila lo que citan sus eventos. Ver [MD Engine — eventos](../../MegaDrive/herramientas/md-engine/eventos-y-scripts.md).

## Referencias cruzadas

- [Colisión con máscaras de píxel: trapecios y rampas](../../GLOSARIO/mask-collision-slopes.md)
- [Timers de juego y framerate](game-timers-framerate.md) · [Punto fijo](../../GLOSARIO/fixed-point.md)
- [MD Engine — eventos (juego en C dentro de MD Engine)](../../MegaDrive/herramientas/md-engine/eventos-y-scripts.md)
