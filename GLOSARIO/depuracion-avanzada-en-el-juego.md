---
titulo: Depuración avanzada dentro del juego — flight recorder, invariantes, cápsulas de fallo, replay diferencial y otras técnicas
plataforma: [General]
categoria: [herramientas]
tags: [depuracion, flight-recorder, caja-negra, invariantes, assert, crash-capsule, replay, regresion, determinismo, watchpoints, perfilado, memoria, canarios, telemetria, hardware-real, qr]
estado: borrador
fecha: 2026-10-31
fuentes: [documento del usuario "Técnicas avanzadas de depuración para juegos retro" (2026-10-31), GLOSARIO/reglas-comunes-de-proyecto.md]
---

# Depuración avanzada dentro del juego

## Resumen

Técnicas para que **el propio juego genere las pruebas del fallo** en hardware real, sin depurador externo.
Amplían el savestate como log y el QR en hardware real ([reglas comunes](reglas-comunes-de-proyecto.md), reglas
5, 6 y 8–11). La meta es que el juego funcione como la **caja negra de un avión**: detecta que algo ha ido mal,
guarda lo que pasó justo antes y lo saca por QR para reproducirlo en el PC.

> **Ninguna de estas técnicas se ha implementado todavía en un proyecto real.** Son un plan de trabajo. Las notas
> de cada plataforma (registros, contadores, límites de RAM) son orientativas y hay que comprobarlas al aplicarlas.

## Detalle técnico

### Arquitectura común: un solo formato de eventos

Antes de empezar con ninguna técnica, conviene que **todas compartan un formato de evento y de exportación**.
Así una misma incidencia relaciona una corrupción de memoria con una entidad, una colisión y una divergencia de
estado.

```
INSTRUMENTACIÓN DEL JUEGO    eventos · estados · memoria · rendimiento · aserciones
        │
MÓDULOS DE CAPTURA           Flight Recorder (búfer circular)
                             State Monitor (snapshots y huellas por subsistema)
                             Memory Monitor (integridad y fugas)
                             Performance Monitor (tiempos y métricas)
        │
CAPA DE EXPORTACIÓN          savestate · QR · SRAM · VMU · fichero (dcload / emulador)
        │
ARNÉS DE PC                  replay · replay diferencial · comparación visual · informes
```

Un evento compacto para consolas con poca RAM, por ejemplo de 4 bytes:

```c
typedef struct {
    u16 tick;      /* tick de lógica (vuelve a 0; el registro guarda el tick completo en la cabecera) */
    u8  tipo;      /* EV_ENTRADA, EV_ESTADO, EV_COLISION, EV_ESCENA, EV_AUDIO, EV_ASSERT, EV_MARCA... */
    u8  dato;      /* id de entidad, estado nuevo, código de error... */
} Evento;
```

La exportación es siempre la misma que ya fija la regla 6: el primer QR lleva la **build** y `n/total`; el PC
procesa igual un registro del emulador que uno fotografiado.

### 1. Flight recorder (últimos N segundos)

Búfer circular en RAM con los **eventos** recientes: entradas, cambios de estado de entidades, colisiones,
cambios de escena, eventos de audio, asignaciones de memoria, errores y huellas parciales del estado. Cuando
salta un error se **congela** y se conserva.

- Diferencia con el savestate como log: no hace falta saber de antemano que va a fallar. Graba siempre.
- Diferencia con la caja negra de la regla 11: aquella solo guarda **entradas** (suficiente para reproducir si la
  lógica es determinista); el flight recorder guarda **qué pasó por dentro**, y sirve aunque la reproducción no
  salga idéntica.
- Tamaño orientativo: 1024 eventos × 4 bytes = 4 KB. En Mega Drive (64 KB de RAM de trabajo) o SNES (128 KB) es
  asumible; en Dreamcast sobra sitio.
- Escribir en el búfer tiene que ser barato: índice con máscara (`i = (i + 1) & (N - 1)`, N potencia de 2), sin
  llamadas ni comprobaciones.
- Al congelar, la pantalla puede ofrecer `[EXPORTAR QR]`; el tick del fallo y los últimos eventos van en texto
  por si el QR no se lee.

### 2. Invariantes de estado (reglas que siempre se cumplen)

No buscan un error concreto, sino **estados imposibles**. Son lo que más rinde por lo poco que cuestan.

```c
ASSERT(player->health <= player->max_health);
ASSERT(enemy_count <= MAX_ENEMIES);
ASSERT(camera.x >= 0);
ASSERT(active_entities <= MAX_ENTITIES);
ASSERT(player->state != STATE_DEAD || player->velocity_y == 0);
```

Agrupadas por subsistema:

- **Colisiones**: ninguna entidad colisiona consigo misma; ninguna colisión con una entidad destruida; la
  geometría de la caja es válida (ancho y alto > 0).
- **Entidades**: ninguna entidad activa sin id; no hay dos con el mismo id; los contadores coinciden con las
  entidades reales.
- **Escenas**: ninguna referencia a una entidad de una escena destruida; no quedan recursos sin liberar al salir.

Cuando una falla se registra el tick, la entidad y el contexto (evento `EV_ASSERT` en el flight recorder) y se
pasa a la pantalla de cuelgue de la regla 11.

Para no gastar CPU: las comprobaciones caras (recorrer todas las entidades) se reparten **un subsistema por
tick** en rotación, y todo el bloque se compila solo en builds de prueba (`#ifdef DEBUG_INVARIANTES`).

### 3. Cápsula de fallo (paquete de diagnóstico)

Lo que se exporta cuando algo grave falla. Es la pantalla de cuelgue de la regla 11 ampliada:

| Campo | Contenido |
|---|---|
| Build | Identificador `Nombre.[Fase].[Build]` |
| Hardware | Modelo/región/revisión (ver huella del hardware) |
| Tick | Tick del fallo |
| RNG | Estado del generador aleatorio |
| Estado | Snapshot o huellas por subsistema, si cabe |
| Flight recorder | Últimos eventos |
| Memoria | Estadísticas de pools y canarios |
| Aserciones | Últimos fallos |
| Pila | Trozo de pila y registros de la CPU, si la plataforma lo permite |

Salida por QR, SRAM o VMU. El PC genera el informe y asigna un **identificador de fallo** (por ejemplo, una
huella de build + PC del fallo + tipo de assert) para agrupar varias apariciones del mismo problema.

### 4. Replay diferencial entre builds

La misma secuencia de entradas en la build estable (A) y en la experimental (B), comparando el estado
periódicamente. Es la regla 9 (partidas de referencia) llevada más lejos: en vez de una huella global, **una
huella por subsistema**, para saber qué se separó primero.

```
COMPARACIÓN DE ESTADO        tick 18450
PLAYER       IGUAL
ENEMIES      IGUAL
COLLISION    IGUAL
CAMERA       DISTINTO   ← primera divergencia
AUDIO        IGUAL
```

- Requiere lógica determinista; en Dreamcast, cuidado con la coma flotante del SH-4 frente al PC (ver
  [Coma flotante determinista](../Dreamcast/cpu/coma-flotante-determinista.md)).
- Cuando una huella falla, el arnés vuelca las variables de ese subsistema en las dos builds, en tabla.
- Si el cambio es intencionado (nuevo balance), se regeneran las referencias y se apunta en el diario.

### 5. Telemetría visual en pantalla

Modo de depuración activado por una combinación de botones que pinta encima del juego: tick, posición y
velocidad de los actores, estado de su FSM, colisiones por frame, entidades activas, memoria libre y pico,
llamadas por subsistema, colas de render y últimos eventos. Con selección de una entidad para ver sus datos.

- Mega Drive: plano WINDOW o un plano libre con la fuente del juego; cuidado con el ancho de banda de VRAM, pintar
  solo lo que cambia.
- La telemetría **no puede alterar la simulación**: activarla no cambia las huellas del registro.

### 6. Watchpoints y breakpoints virtuales

Condiciones de vigilancia dentro del propio juego, para el hardware real sin depurador:

```c
WATCH_EQ(player->health, 0);        /* avisa si vale 0 */
WATCH_CHANGE(player->state);        /* avisa si cambia fuera de la FSM */
BREAK_IF(player->x < 0);
BREAK_IF(collision_count > 50);
BREAK_IF(memory_free < 1024);
```

Al dispararse: congelar, guardar snapshot, anotar en el flight recorder, mostrar QR y, si se quiere, seguir.

- En **emulador** ya hay watchpoints de verdad (BlastEm y Exodus en Mega Drive, Mesen en SNES, el depurador de
  Flycast/GDB en Dreamcast): úsalos allí y deja los virtuales para el hardware.
- Conviene nombres explícitos (`WATCH_EQ`, `WATCH_GT`, `WATCH_CHANGE`) en vez de un `WATCH(x, v)` ambiguo.
- Coste: cada macro se evalúa cada tick; se compilan fuera en las builds publicadas.

### 7. Memory poisoning y canarios

Para errores de memoria que aparecen tras mucho rato de juego:

- Rellenar la memoria liberada con un patrón conocido (`0xDEAD`, `0xA5A5`) para que un uso tras liberar se vea.
- **Canarios** antes y después de cada bloque o cada ranura de pool; comprobarlos al liberar y en rotación.
- Registrar quién asignó y liberó cada bloque (id de subsistema en la cabecera del bloque).
- Contadores de asignaciones por subsistema para ver fugas.
- **Canario en el fondo de la pila**: en Mega Drive y SNES, un desbordamiento de pila es de los fallos más
  típicos y más silenciosos; comprobarlo una vez por frame.

En Mega Drive y SNES casi todo son **pools estáticos**: mejor un verificador por pool (ranuras libres
coherentes, canarios, ids únicos) que imitar herramientas de sistemas modernos. En Dreamcast con KOS hay `malloc`,
y ahí los canarios alrededor de cada bloque tienen más sentido.

### 8. Perfilado: mapas de calor y distribución de tiempos

Medir dónde se va el tiempo, no solo los FPS: render, física, colisiones, IA, animación, audio, entidades y
actualización del mapa.

- Métricas: media, máximo, percentiles 95 y 99, y **frames que superan el presupuesto**. En consolas retro una
  media buena oculta parones sueltos, por eso importa la distribución.
- Para los percentiles no hace falta guardar cada frame: basta un **histograma** de tiempos (p. ej. en líneas de
  barrido) de unas decenas de casillas.
- Cómo medir (por comprobar en cada caso): en Mega Drive, el contador V del VDP (puerto HV) o barras de color
  cambiando el color de fondo; en SNES, el contador latcheado de la PPU; en Dreamcast, los temporizadores del SH-4
  vía KOS.
- Mapa de calor sobre el juego: cada entidad tintada (o con un marcador) según su coste.

### 9. Comparación visual de frames

Complementa el replay determinista: capturas en ticks concretos y diferencia píxel a píxel con una referencia
(por regiones: HUD, jugador, fondo, enemigos; diferencias de paleta; sprites desaparecidos o movidos). Detecta
regresiones gráficas que no cambian la lógica.

- Hacerlo en **emulador**: en hardware real las fotos cambian con la geometría, el escalado y la señal.
- Alternativa barata y exacta: en vez de píxeles, **huellas de VRAM, CRAM/paletas y tabla de sprites** en ese
  tick. Si coinciden, la imagen coincide; si no, se sabe qué tabla cambió.

### 10. Huella del hardware

Datos del hardware y el entorno en la cabecera del registro, para separar fallos de una revisión de consola o de
un emulador: modelo y región, NTSC/PAL, temporización de vídeo, periféricos conectados, diferencias de
arranque.

- Mega Drive: registro de versión del puerto de E/S (`0xA10001`: región, PAL/NTSC, revisión de hardware).
- SNES: registros de versión de las PPU (`$213E`/`$213F`, este último también indica PAL/NTSC).
- Dreamcast: región y modelo desde la flashrom vía KOS (por comprobar).
- La huella **solo se anota**: el juego no debe cambiar de comportamiento según ella, o el replay deja de ser
  fiable.

### 11. Monitor de anomalías (experimental)

Estadísticas del comportamiento normal (enemigos por zona, tiempo en cada estado, colisiones por segundo,
eventos por segundo, llamadas a funciones) y aviso cuando una se sale del rango.

```
ANOMALÍA   ENEMY AI   tick 18450
Cambios de estado esperados: 2-5   observados: 47   → posible oscilación de la IA
```

No sustituye a las aserciones, pero encuentra problemas que nadie había previsto. Los rangos se pueden sacar en el
PC de las partidas de `regresion/` en vez de calcularlos en la consola.

## Priorización

| Técnica | Valor | Coste |
|---|---|---|
| Flight recorder | Muy alto | Medio |
| Invariantes de estado | Muy alto | Bajo |
| Cápsula de fallo | Muy alto | Medio |
| Replay diferencial | Muy alto | Alto |
| Memory poisoning / canarios | Alto | Medio |
| Comparación visual | Alto | Medio |
| Telemetría visual | Alto | Bajo |
| Watchpoints virtuales | Alto | Medio |
| Huella del hardware | Alto | Medio |
| Perfilado / mapas de calor | Medio-alto | Medio |
| Monitor de anomalías | Experimental | Alto |

Orden sugerido: **1. flight recorder, 2. invariantes, 3. cápsula de fallo**, después replay diferencial y
canarios. Las tres primeras son independientes entre sí y encajan directamente con el savestate como log y el QR.

### Flujo ideal: depurar sin intervención

1. Falla algo a los 40 minutos de partida en una consola.
2. El juego detecta una invariante incumplida.
3. Congela el flight recorder.
4. Monta la cápsula de fallo.
5. Muestra uno o varios QR.
6. Se fotografían y se importan en el PC.
7. El arnés identifica build y hardware.
8. Reproduce la partida en el emulador.
9. Localiza el tick exacto y la primera divergencia.
10. Genera el informe con el contexto.

## Notas / gotchas

- **Nada de esto puede alterar la simulación**: activar telemetría, watchpoints o invariantes no debe cambiar las
  huellas del registro. Si las cambia, la build de prueba y la publicada no son la misma partida.
- Todo lo caro va tras `#ifdef` y se compila fuera en la build publicada; pero el flight recorder, la cápsula y la
  pantalla de cuelgue conviene **dejarlos en la publicada**, que es donde aparecen los fallos de hardware real.
- El manejador que exporta la cápsula trabaja con el estado roto: sin heap, sin interrupciones, con pila propia
  (regla 11).
- En Mega Drive y SNES el QR admite pocos datos por código: la cápsula se reparte en varios y lo esencial (build,
  tick, tipo de fallo, PC) va en el primero y también en texto.
- Al implementar una técnica en un proyecto, documentar el resultado real en la plataforma (`herramientas/`) y
  pasar esta entrada a `revisado` en lo que se haya comprobado.

## Referencias cruzadas

- [Reglas comunes a todo proyecto](reglas-comunes-de-proyecto.md) — reglas 5–6 (savestate como log y QR) y 8–11
  (marcador, regresión, `publicar.py`, pantalla de cuelgue)
- [Registro de partida — VMU y códigos QR](../Dreamcast/herramientas/registro-de-partida-y-qr.md)
- [Coma flotante determinista](../Dreamcast/cpu/coma-flotante-determinista.md)
- [Flycast automatizado](../Dreamcast/herramientas/flycast-automatizado.md)
- [Puerto de depuración del VDP](../MegaDrive/graficos/vdp-debug.md)
- [Build de depuración en ASM puro](../MegaDrive/herramientas/build-debug-asm-puro.md)
