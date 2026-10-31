---
titulo: Reglas comunes a todo proyecto — RetroPedia, Tools, ROADMAP, diario de builds, nombre de build, savestate como log y QR
plataforma: [General]
categoria: [herramientas]
tags: [proceso, herramientas-compartidas, build, version, roadmap, diario, depuracion, savestate, log, qr, hardware-real, checklist]
estado: revisado
fecha: 2026-10-31
fuentes: [proyecto SSB Port (SmashLandMD_B056, SSB_BUILD en core/gen_data.h, PLAN_PORT_MEGADRIVE.md), proyecto hyperdbz-dreamcast-port (dc/proto/src/replay.c, qr.c, tools/replay.py), decisión del usuario 2026-10-31]
---

# Reglas comunes a todo proyecto

## Resumen

Reglas que han salido de los desarrollos hechos hasta ahora (SSB Port, hyperdbz Dreamcast Port, demakes de
`Future`) y que **se aplican desde el primer día en cualquier desarrollo de juego nuevo**, sea de la plataforma
que sea. Existe la skill de usuario `nuevo-proyecto-retro` que las monta en un proyecto.

1. **RetroPedia es la referencia de consulta**, y los avances reales del proyecto vuelven a ella.
2. **Hay un `ROADMAP.md` con las fases** del desarrollo.
3. **Hay un `DIARIO.md` de builds**: qué entra en cada build, para poder consultarlo en el futuro.
4. **Toda build tiene nombre `Nombre.[Fase].[NúmeroBuild].extensión`** y ese identificador **se ve dentro del juego**.
5. **El savestate es el log.** Para saber qué ha pasado en la máquina, la demo o la compilación, el fichero que se
   mira es el savestate (o la partida guardada que haga de registro).
6. **En hardware real, el log sale en uno o varios códigos QR** en pantalla, que se fotografían con el móvil.
7. **Las herramientas son compartidas y viven en `C:\Users\migur\Desktop\Tools`**: emuladores, SDK, editores y
   trackers se usan desde ahí y no se copian a cada proyecto.

La idea común: cualquier informe (una captura, un savestate, una foto de un QR, una entrada del diario) dice **de
qué build viene** y contiene **información de primera mano** que se pasa al PC sin cables; y lo que se aprende no
se queda en el proyecto, sino que alimenta RetroPedia.

## Detalle técnico

### 1. RetroPedia como consulta y destino de los avances reales

RetroPedia está en `C:\Users\migur\Desktop\RetroPedia`.

- **Antes de implementar algo de plataforma** (gráficos, sonido, memoria, periféricos, herramientas) o de motor,
  se busca en RetroPedia: Grep sobre `INDEX.md` por secciones (`^## `, `^### `) y sobre el frontmatter
  (`plataforma:`, `tags:`). Lo documentado se usa en vez de investigarlo otra vez.
- El `CLAUDE.md` del proyecto dice qué carpetas de RetroPedia le tocan (p. ej. `Dreamcast/`, `MOTORES/mugen/`).
- **Los avances reales vuelven a RetroPedia**: lo que se comprueba en hardware o emulador durante el proyecto se
  vuelca como entrada nueva (desde `_plantilla.md`) o hace pasar una existente de `borrador` a `revisado`. Se cita
  el proyecto, los ficheros y la build en `fuentes:`, y se dice explícitamente dónde se comprobó.
- También se corrige lo que resulte falso: una entrada de RetroPedia que el proyecto desmiente se arregla, no se
  esquiva.
- Buen momento para volcar: al cerrar una fase del ROADMAP o al publicar una build con un hallazgo técnico.

### 2. ROADMAP con las fases

`ROADMAP.md` en la raíz del proyecto:

- Lista de fases numeradas, cada una con **objetivo**, **tareas** y **criterio de salida** (qué tiene que funcionar,
  y dónde: emulador o hardware real, para darla por cerrada).
- Marca la **fase actual** y, en las cerradas, **la build con la que se cerró**.
- Los nombres de fase son los que se usan en `[Fase]` del nombre de la build, así ROADMAP, builds y diario
  hablan el mismo idioma.
- Se actualiza cuando cambia el plan, no se reescribe la historia: una fase descartada se marca como tal.

Esquema típico (cada proyecto lo adapta; SSB Port usa 12 fases en `PLAN_PORT_MEGADRIVE.md`): preparación →
extracción de datos → presupuesto técnico → prototipo (vertical slice) → sistemas completos → contenido →
audio → IA → optimización y pruebas en hardware real → publicación.

### 3. Diario de builds

`DIARIO.md` en la raíz del proyecto, **una entrada por build publicada**, la más reciente arriba:

```markdown
## SmashLandMD.Beta.B057 — 2026-10-31

**Fase:** Beta (ROADMAP fase 6)
**Cambios:** qué entra, con el porqué (peticiones del usuario marcadas como tales).
**Medidas:** rendimiento, tamaño de ROM, memoria libre... lo que se haya medido y dónde.
**Probado en:** emulador X / hardware real.
**Fallos conocidos:** lo que se sabe roto.
**Logs de referencia:** savestates o QR guardados de esta build (ruta).
**RetroPedia:** entradas creadas o revisadas a raíz de esta build.
```

- Se escribe **al publicar la build**, no de memoria semanas después.
- Es lo que se consulta para saber cuándo entró un cambio, qué build reproducir o qué contar en unas notas de
  versión; los comentarios del código citan la build (`B056: ...`) y el diario da el contexto.

### 4. Nombre de la build y referencia visible en el juego

Formato del fichero publicado:

```
Nombre.[Fase].[NúmeroBuild].extensión
```

| Parte | Qué es | Ejemplo |
|---|---|---|
| `Nombre` | Nombre corto del proyecto, sin espacios | `SmashLandMD` |
| `[Fase]` | Fase del ROADMAP en que está | `Beta` |
| `[NúmeroBuild]` | Número correlativo de build publicada; **nunca se reutiliza ni se reinicia al cambiar de fase** | `B057` |
| `extensión` | La que pida la plataforma o el documento | `.bin`, `.cdi`, `.neo`, `.sfc`, `.pdf` |

Ejemplos: `SmashLandMD.Beta.B057.bin`, `HyperDBZ.Proto.B012.cdi`, y los documentos que acompañan a la build
llevan el mismo identificador (`SmashLandMD_Manual.Beta.B057.pdf`).

Cómo se aplica:

- **Una sola fuente de verdad.** El identificador vive en una constante que genera o lee el script de build
  (p. ej. `BUILD` en `tools/build_proto.py` → `SSB_BUILD[]` en `gen_data.h`). El nombre del fichero de salida y
  el texto que se muestra en el juego salen de ahí; nunca se escriben a mano en dos sitios.
- **Se sube el número antes de publicar** una build nueva, no después, y se añade su entrada al diario.
- **Visible dentro del juego**: en una esquina de la pantalla de título, en el menú de opciones o de pausa, o en
  una pantalla de créditos/información. Tiene que poder leerse en una foto de la tele sin tener que adivinar.
- **Va también en la cabecera del savestate/registro y en el QR** (ver abajo), para que ningún log se pueda
  confundir de build.

### 5. El savestate como log

Lo que ha pasado en una partida se comprueba **con el savestate**, no con capturas ni descripciones:

- En **emulador**, el savestate (o la partida guardada en la VMU/SRAM, que es un fichero del PC) se copia tal
  cual al proyecto y se abre con las herramientas del PC.
- En la **demo** o en una **compilación nueva**, el savestate de referencia de la build anterior sirve para
  comprobar que la nueva se comporta igual.
- Para que valga como log, el juego debe ser **determinista** y el registro guardar, como mínimo: identificador
  de build, semilla, entradas del mando (solo los cambios), y cada cierto número de ticks una huella del estado.
  Con eso un arnés de PC reproduce la partida y dice en qué tramo se separa. El formato concreto, probado en
  Dreamcast, está en [Registro de partida](../Dreamcast/herramientas/registro-de-partida-y-qr.md).
- Si la cabecera del registro no coincide con la build o los datos que tiene el PC, la herramienta **avisa** en
  vez de reproducir algo que no corresponde.

### 6. QR en hardware real

En la consola real no hay savestates ni cable serie fiable, así que el mismo registro se saca **en pantalla como
uno o varios códigos QR** que se fotografían con el móvil:

- Primero va siempre un QR (o la cabecera del primero) con el **identificador de build** y el recuento de QR
  (`1/3`, `2/3`...), para poder pedir el que falte.
- Si el registro no cabe en uno, se trocea en varios y se pasan con un botón.
- Lo que llega al PC se decodifica y se trata igual que un savestate de emulador: mismo formato, mismas
  herramientas.
- La implementación (QR, base45, tamaños y lectura con zxing-cpp) está documentada en
  [Registro de partida](../Dreamcast/herramientas/registro-de-partida-y-qr.md); el generador es C puro y se
  porta a cualquier plataforma con pantalla suficiente.

### 7. Directorio `Tools` compartido

Las herramientas de terceros están en `C:\Users\migur\Desktop\Tools`, una sola vez para todos los proyectos:

| Herramienta | Carpeta | Para qué |
|---|---|---|
| Aseprite 1.3.17.2 | `Aseprite-v1.3.17.2-x64` | Sprites, paletas y animaciones |
| Furnace 0.6.8.3 | `furnace-0.6.8.3-win32` | Música y efectos (YM2612/PSG, YM2610, SPC700...) |
| BlastEm 1.0.0 | `blastem-win64-1.0.0` | Emulador de Mega Drive preciso, con depurador |
| Gens 2.14 | `Gens_v2.14` | Emulador de Mega Drive antiguo (contraste de comportamientos) |
| Flycast 2.7 | `flycast-win64-2.7` | Emulador de Dreamcast; `flycast.lua` para automatizar (ver [Flycast automatizado](../Dreamcast/herramientas/flycast-automatizado.md)) |
| Flycast (fuentes) | `flycast-master` | Código del emulador, para consultar cómo emula algo |
| DreamSDK R4 | `DreamSDK-R4-Setup` | Instalador del SDK de Dreamcast (KallistiOS); ver [DreamSDK en Windows](../Dreamcast/herramientas/dreamsdk-windows.md) |
| ngdevkit | `ngdevkit-master` | SDK de Neo Geo (fuentes) |
| Recursos | `Resources.txt` | Webs de assets de referencia (The Spriters Resource, VGMRips) |

Cómo se aplica:

- **Los proyectos referencian las herramientas por ruta**, en su `CLAUDE.md` y en sus scripts (una variable o
  constante con la ruta de `Tools`), en vez de llevar una copia dentro. SSB Port lleva su propio BlastEm: es lo
  que se evita en adelante.
- **Una herramienta nueva se instala en `Tools`** y se añade a esta tabla; si tiene trucos de uso, se documenta
  en la carpeta `herramientas/` de la plataforma en RetroPedia.
- Si un proyecto necesita **una versión concreta** distinta, se instala también en `Tools` con la versión en el
  nombre de la carpeta, y el proyecto apunta a esa.
- Las herramientas **propias** de cada proyecto (conversores `tools/*.py`) siguen dentro del proyecto; si una
  sirve para varios, se saca a un repositorio propio en el escritorio, como `PXDraw` o `PXExtractor`.

## Checklist para arrancar un proyecto nuevo

- [ ] Consultado RetroPedia para la plataforma y el tipo de juego; el `CLAUDE.md` del proyecto dice qué carpetas usar.
- [ ] Emuladores, SDK y editores tomados de `Desktop\Tools` y referenciados por ruta en el `CLAUDE.md`; lo que falte se instala allí.
- [ ] `ROADMAP.md` con fases, criterios de salida y fase actual.
- [ ] `DIARIO.md` creado, con la entrada de la primera build.
- [ ] Elegido el `Nombre` corto; las fases del nombre de build son las del ROADMAP.
- [ ] Constante de build única en el script de build; el fichero de salida se llama `Nombre.[Fase].[Build].ext`.
- [ ] El identificador se ve en el título, opciones, pausa o créditos.
- [ ] Lógica determinista (semilla controlada, sin dependencias de tiempos de carga ni de coma flotante distinta).
- [ ] Registro de partida: cabecera con build + semilla, entradas, huellas periódicas.
- [ ] Herramienta de PC que lee el savestate/registro y reproduce la partida.
- [ ] Salida del registro por QR en pantalla para el hardware real, con la build en el primero.
- [ ] Al cerrar cada fase: avances reales volcados a RetroPedia.

## Notas / gotchas

- Si la build visible en el juego y el nombre del fichero no coinciden, el número se escribió a mano en dos
  sitios: corregir el script, no el texto.
- Un savestate de emulador solo sirve en ese emulador y esa versión; el **registro de partida** (entradas +
  huellas) es portable entre emulador, hardware y arnés de PC. Siempre que se pueda, el log es el registro.
- En plataformas con poca resolución o poca RAM (Mega Drive, SNES) el QR debe ser pequeño: menos datos por
  código y más códigos.
- Un diario que se rellena "luego" acaba vacío: la entrada se escribe en el mismo paso que se sube el número.

## Referencias cruzadas

- [Registro de partida — VMU y códigos QR](../Dreamcast/herramientas/registro-de-partida-y-qr.md)
- [Coma flotante determinista](../Dreamcast/cpu/coma-flotante-determinista.md)
- [Flycast automatizado](../Dreamcast/herramientas/flycast-automatizado.md)
