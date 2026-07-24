---
titulo: Bumbershoot Software — scroll de pantalla completa dirigido por interrupción VBLANK
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bumbershoot, scroll, vblank, hblank, interrupciones, vdp, dma]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/04/11/genesis-scrolling-and-interrupts/]
---

# Bumbershoot Software — scroll de pantalla completa dirigido por interrupción VBLANK

## Resumen

Quinta entrada de la serie Bumbershoot Software (2018). Es un artículo de diseño más que de código — no incluye listados de ensamblador — pero documenta una arquitectura concreta para separar la simulación de juego (un autómata celular, aún sin implementar en el momento de escribir el artículo) de su renderizado, usando la interrupción VBLANK como "hilo de renderizado" que sincroniza el scroll de pantalla completa de los dos planos del VDP.

## Detalle técnico

### Arquitectura: bucle principal + VBLANK como "hilo de render"

El autor divide el trabajo en dos partes: el bucle principal calcula el siguiente estado del autómata celular (lógica de juego), mientras que la rutina de interrupción VBLANK asume el papel de un "hilo de renderizado" que se dispara una vez por frame para actualizar lo que se ve en pantalla, en vez de que el bucle principal escriba directamente al VDP en mitad de su cálculo.

### Configuración de interrupciones

El autor habilita interrupciones con:

```
MOVE.W #$2500, sr
```

El VDP dispara VBLANK como interrupción de **nivel 6** del 68000 y HBLANK como interrupción de **nivel 4** (niveles ya documentados de forma más sistemática en `segaretro-interrupts.md`). Además, hay que activar explícitamente el bit correspondiente en el **registro 1 del VDP** para que la interrupción VBLANK llegue a dispararse.

### Escritura del valor de scroll horizontal

El autor mantiene ambos planos de scroll (A y B) sincronizados, moviéndolos siempre juntos, lo que simplifica la actualización: la tabla de scroll horizontal vive en la dirección VRAM `$AC00`, y advierte que "no es un simple registro sino un conjunto de tablas" que permiten granularidad de hasta celda de carácter o incluso línea de scan individual (aunque el ejemplo de este artículo usa un único valor de pantalla completa para sincronizar ambos planos). El único cuidado necesario es que cada valor debe escribirse **dos veces** en VRAM: una para el plano A y otra para el plano B.

### Presupuesto de tiempo de la rutina VBLANK

El autor cifra la rutina completa de VBLANK (incluyendo sus funciones de soporte) en **214 bytes**, pero señala que en la práctica esos 214 bytes "tardan la mayor parte de un frame en ejecutarse" en los peores casos, porque a veces hace falta copiar 8 KB de datos "por un canal de datos increíblemente saturado" (referencia implícita al ancho de banda compartido de VRAM/bus durante el blanking).

### Trabajo futuro reconocido por el propio autor

- Implementar la rutina `CCAStep` del autómata celular (todavía no escrita en el momento del artículo).
- Ampliar la lectura de mando.
- Si el tearing visual resulta perceptible, reimplementar la técnica de doble buffer que el autor ya había usado en una versión anterior del mismo proyecto para Game Boy.
- Añadir banda sonora usando el chip YM2612, como última pieza obligatoria del proyecto.

## Notas / gotchas

- **Posible inconsistencia a verificar contra el código fuente real:** la máscara de interrupción del registro de estado del 68000 solo acepta interrupciones de nivel *estrictamente superior* al valor de sus bits 8-10 (salvo el nivel 7, no enmascarable). `$2500` fija esos bits a `101` (5), lo que en teoría solo dejaría pasar interrupciones de nivel 6 y 7 — bloqueando el nivel 4 (HBLANK) que el propio artículo dice también necesitar. Puede ser una simplificación del autor en el texto, un valor efectivamente usado solo para VBLANK (con HBLANK gestionado de otra forma en el código real), o una imprecisión del artículo; no se ha podido contrastar contra el listado de código fuente completo del proyecto.
- Este artículo es una nota de diseño *previa* a tener el autómata celular implementado (el propio autor dice "todavía no he implementado el autómata"); tratar sus cifras (214 bytes, presupuesto de frame) como orientativas de un prototipo, no como benchmark final.
- La estrategia de sincronizar ambos planos de scroll con el mismo valor es una simplificación deliberada del autor para este proyecto concreto; para scroll con paralaje (planos a velocidades distintas) haría falta escribir valores diferentes por plano en vez de duplicar el mismo valor.

## Referencias cruzadas

- [Interrupciones del VDP y externas (VInt, HInt, external)](segaretro-interrupts.md) — detalla de forma sistemática los niveles de interrupción del 68000 (VInt=6, HInt=4) que este artículo usa de forma práctica.
- [Mapas y planos de scroll — enfoque Big Evil Corporation](bec-maps-scrolling.md) — implementación real con código de scroll horizontal de pantalla completa y su bucle de actualización dentro de la ventana de vblank, ausente en este artículo.
- [Referencia de registros del VDP](vdp-registers.md) — registro 1 (habilitación de interrupciones) y registros de configuración de scroll mencionados aquí sin detalle bit a bit.
