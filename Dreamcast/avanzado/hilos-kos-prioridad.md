---
titulo: Hilos de KOS — prioridad, envejecimiento y trabajo de fondo
plataforma: [Dreamcast]
categoria: [avanzado]
tags: [kos, optimizacion, audio, sh4]
estado: borrador
comprobado: emulador
comprobado_en: "Flycast 2.7, hyperdbz-dreamcast-port, paso 4.5 (13-11-2026)"
fecha: 2026-11-13
fuentes: [KOS kernel/thread/thread.c (thd_calc_prio, thd_schedule), KOS include/kos/thread.h, proyecto hyperdbz-dreamcast-port]
---

# Hilos de KOS: un hilo de fondo que no moleste al juego

## Resumen

En KOS, un número de prioridad más alto significa menos prioridad (`PRIO_DEFAULT` = 10). **Pero la prioridad envejece:** un hilo que espera listo para ejecutarse va ganando prioridad con el tiempo. Un hilo de fondo con `PRIO_DEFAULT + 1` acaba adelantando al hilo principal **en mitad de su frame**. Para que solo corra cuando el juego espera (al vblank, por ejemplo), hay que crearlo con `PRIO_MAX`: KOS no envejece esa prioridad.

## Detalle técnico

`thd_calc_prio` (kernel/thread/thread.c):

```c
prio_t prio = thd->prio;
if(prio < PRIO_MAX)
    prio >>= (now - thd->cpu_time.scheduled) >> thd_ageing_ms_log2;
```

- `thd_ageing_ms_log2 = log2_rup(thd_sched_ms * 8)`: con el planificador a 100 Hz (10 ms), cada 128 ms sin ejecutarse la prioridad se divide por 2. A los 128 ms, 11 pasa a 5 y adelanta al hilo principal (10).
- `PRIO_MAX` (4096) queda fuera del envejecimiento. El hilo ocioso de KOS usa `PRIO_MAX + 1` («disable ageing»).
- El planificador escoge el hilo listo con la prioridad efectiva más baja. A igual prioridad, por turnos.

```c
kthread_attr_t attr = { 0 };
attr.create_detached = true;
attr.stack_size = 16 * 1024;
attr.prio = PRIO_MAX;          /* solo cuando el juego espera; nunca en mitad del frame */
attr.label = "sonido";
thd_create_ex(&attr, hilo, NULL);
```

## Notas / gotchas

- **Medido en un caso real:** un hilo que alimenta streams de audio desde el CD (`snd_stream_start` y `snd_stream_poll`, hasta 17 ms cada uno en Flycast) a `PRIO_DEFAULT + 1` causaba picos de 18 ms en la lógica del juego, siempre en el mismo tick. Con `PRIO_MAX`: 0 vblanks perdidos (antes, 2), el peor frame bajó de 26,9 a 20,0 ms y la latencia del audio no cambió.
- **Síntoma:** un pico determinista que en el PC no aparece (el mismo tick hace el mismo trabajo que sus vecinos) y que, al medirlo por fases, cae en una función trivial. Algo de fuera para al hilo; no es el código.
- Aun con `PRIO_MAX`, las secciones de KOS que cortan las interrupciones (`g2_lock()` al escribir en la RAM de audio, los accesos al GD-ROM) pueden retrasar al juego si coinciden con el vblank. Esas secciones no se pueden adelantar.

## Referencias cruzadas

- [Lectura del CD con KOS](lectura-del-cd-kos.md)
- [ADPCM del AICA](../sonido/aica-adpcm.md)
