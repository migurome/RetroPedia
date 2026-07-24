---
titulo: Timers de juego — problemas y soluciones (bucle principal y framerate)
plataforma: [General]
categoria: [arquitectura]
tags: [bucle-principal, timestep, framerate, sincronizacion, arquitectura-motor]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/timer_and_framerate/index.php]
---

# Timers de juego — problemas y soluciones (bucle principal y framerate)

## Resumen

Artículo breve pero fundamental de Fabien Sanglard (25 de diciembre de 2012) sobre el problema clásico del bucle principal de un motor de juego: un timestep variable (ligado al framerate real) hace que la simulación sea no determinista, rompe la detección de colisiones a bajo framerate, e impide grabar/reproducir sesiones o sincronizar simulaciones en red. Propone la solución estándar de timestep fijo con un bucle interno de simulación desacoplado del bucle de render — la base de cualquier bucle principal de motor, incluido motor-cenital.

## Detalle técnico

### El problema: framerate variable y timeslices variables

**Enfoque ingenuo original.** La arquitectura original del bucle de juego seguía esta secuencia:

1. Obtener la diferencia de tiempo (timeslice) desde la última actualización de pantalla
2. Actualizar el mundo según las entradas y el timeslice
3. Renderizar el estado del mundo en pantalla

**Implementación (pseudocódigo en C):**

```c
int lastTime = Timer_Gettime();

while (1){
  int currentTime = Timer_Gettime();
  int timeSlice = currentTime - lastTime ;
  
  UpdateWorld(timeSlice);
  RenderWorld();
  
  lastTime = currentTime;
}
```

**Características de rendimiento.** Con "un renderizador rápido, el mundo del juego se actualiza con diferencias de tiempo (timeslice) minúsculas de 16-17 milisegundos (60Hz) y todo va bien."

**Debilidad crítica:** "la debilidad del diseño es que simular el mundo con precisión está atado a tener timeslices pequeños (es decir, framerate alto)."

**Problema de detección de colisiones:** "actualizar a 30Hz en vez de 60Hz puede mover los objetos tanto que se pierdan colisiones (en un juego como [SHMUP] eso era un gran problema)."

**Dos problemas fundamentales identificados:**

**Problema 1 — Replay y depuración:** "grabar las entradas del usuario y reproducir una sesión de juego (para depuración o entretenimiento) se vuelve difícil ya que los timeslices son distintos. Esto es cierto incluso si la máquina de reproducción es la misma máquina que originalmente grabó la sesión, ya que las llamadas al sistema operativo nunca tendrán exactamente la misma duración." Aunque grabar el tiempo de simulación junto con las entradas podría resolverlo, "hace que el diseño del motor de juego [...] se vuelva desordenado."

**Problema 2 — Sincronización en red:** "si tu juego está basado en red y necesitas correr varias simulaciones simultáneamente en máquinas distintas con entradas idénticas: los timeslices no coincidirán y pequeñas diferencias aparecerán hasta que las simulaciones en cada máquina queden ¡desincronizadas!"

### La solución: timeslices fijos

**Arquitectura central.** La solución propuesta usa una estructura de bucle anidado que desacopla el renderizado de la simulación:

**Implementación (pseudocódigo en C):**

```c
int  gameOn = 1
int simulationTime = 0;

while (1){
  int realTime = Gettime();
  
  while (simulationTime < realTime){
    simulationTime += 16; //El timeslice es SIEMPRE 16ms.
    UpdateWorld(16);
  }
  
  RenderWorld();
}
```

**Características clave:**
- Valor de timeslice fijo: 16 milisegundos (constante)
- Estructura de bucle anidado: el bucle interno avanza la simulación, el bucle externo gestiona el renderizado
- Principio de desacoplamiento: "el motor ahora simula el mundo con timeslices CONSTANTES, sin importar la latencia de red ni el rendimiento del renderizador."

**Beneficios:**
1. "código simple y elegante para grabar/reproducir replays"
2. "sistema de detección de colisiones sólido"
3. Ventaja crítica: "permite que simulaciones corriendo en máquinas distintas se sincronicen, siempre que se les provean las mismas entradas/semillas aleatorias."

### Contexto de la industria

El autor señala que, tras investigar más, este enfoque ya estaba bien establecido: "la alegría de 'descubrir' este enfoque duró poco: este sistema es muy conocido en la industria del videojuego y es de hecho la piedra angular de juegos como Starcraft II o los motores de id Software."

### Referencias citadas

El artículo cita dos recursos adicionales para profundizar:

1. **"Timestepping"** de Peter Sundqvist (turborilla.com)
2. **"Fix your timestep"** de Glenn Fiedler (gafferongames.com) — descrito como que "lleva la idea más allá y también menciona las interpolaciones"

### Resumen técnico

**Problema del timestep variable:** los frames entregan valores de delta-time distintos (16-17ms a 60Hz, potencialmente mucho mayores si baja el framerate), causando:
- Física dependiente del framerate
- Colisiones perdidas a framerate bajo
- Desincronización de replays
- Divergencia de simulaciones en red

**Solución de timestep fijo:** mantener incrementos de tiempo de simulación constantes (16ms) dentro de un bucle interno, desacoplando el bucle de render, asegurando:
- Simulación determinista
- Consistencia de replays
- Sincronización en red
- Detección de colisiones robusta

## Notas / gotchas

- El bucle de timestep fijo desacopla explícitamente "cuántas veces se simula el mundo" de "cuántas veces se dibuja en pantalla": el bucle interno puede ejecutar UpdateWorld() cero, una o varias veces por iteración del bucle externo, según cuánto tiempo real haya transcurrido — esto es clave para no perder determinismo si el render se retrasa puntualmente (p. ej. un DMA largo o un frame de rasterizado pesado).
- El autor fija el timeslice en 16ms (asumiendo 60Hz) como constante — en un motor de Mega Drive con NTSC a 60Hz y PAL a 50Hz (ver los artículos de Genesis/SNES de esta misma serie, donde el VBLANK PAL es de 20ms frente a los 16ms de NTSC), esta constante tendría que parametrizarse por región en vez de asumir 16ms fijo universal, o el juego correría a una velocidad de simulación distinta en PAL.
- El determinismo del timestep fijo es lo que permite grabar solo las entradas (inputs) para reproducir una sesión exactamente, en vez de tener que grabar también los timeslices reales — simplifica mucho un sistema de replay o de sincronización en red basada en "lockstep" (mismas entradas + misma semilla aleatoria + mismo timestep = mismo resultado en todas las máquinas), pero exige que toda la lógica de simulación sea perfectamente determinista (sin depender de temporizadores reales, orden de iteración no determinista, etc.).
- Es un artículo corto comparado con la serie de "Another World", pero de aplicación transversal: cualquier motor con render y simulación en el mismo hilo (como es habitual en un motor de 8/16 bits de un solo procesador principal) se beneficia directamente de esta separación conceptual, incluso si en la práctica el "renderizado" en Mega Drive equivale a preparar los datos a copiar por DMA en el próximo VBLANK.

## Referencias cruzadas

- (pendiente de enlazar)
