---
titulo: Dolmexica Infinite — intérprete de MUGEN para Dreamcast y qué recorta
plataforma: [General, Dreamcast]
categoria: [motores]
tags: [mugen, arquitectura, arranque, efectos, paletas, memoria]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [https://captaindreamcast.github.io/DolmexicaInfinite/dolmexica.html, https://captaindreamcast.github.io/DolmexicaInfinite/differences.html, https://captaindreamcast.github.io/DolmexicaInfinite/makeyourowngamedreamcast.html, https://captaindreamcast.github.io/DolmexicaInfinite/sctrls.html, https://captaindreamcast.github.io/DolmexicaInfinite/trigger.html, https://github.com/CaptainDreamcast/DolmexicaInfinite]
---

# Dolmexica Infinite

## Resumen

Intérprete de MUGEN de código abierto (MIT, CaptainDreamcast) para Dreamcast, Web, Windows y Vita. Es la ruta contraria a
la de `hyperdbz-dreamcast-port`: en vez de [compilar el CNS](cns-compilacion.md) y escribir un motor a medida, se meten
los ficheros de MUGEN en una carpeta y se genera un CDI. Todo lo de esta entrada sale de su documentación: **no se ha
probado ni en emulador ni en consola**.

## Detalle técnico

### Compatibilidad

Incluye funciones desde WinMugen hasta MUGEN 1.1b. Si dos versiones chocan sin solución, gana la más antigua, porque en
Dreamcast la memoria favorece a los personajes antiguos. Esa preferencia es una decisión de diseño: un juego pensado para
1.0 o 1.1 puede comportarse distinto aunque cargue.

### Lo que falta en Dreamcast

| Función | Motivo que da la documentación |
|---|---|
| After images | Rendimiento |
| Blend de sustracción | Limitación técnica |
| Sombras de color | Limitación técnica |
| PalFX de inversión de color y de fuerza de color | Limitación técnica |

### Lo que falta o cambia en todas las plataformas

- **Equipos:** eliminados, con sus triggers y los elementos relacionados de los controladores.
- **Bancos de fuente:** eliminados, porque Dreamcast no tiene bancos de paleta y la versión de Windows respeta el mismo
  límite. Se sustituyen por colores fijos: 0 blanco, 1 rojo, 2 verde, 3 azul, 4 cian, 5 amarillo, 6 magenta, 7 negro,
  8 gris claro.
- **RemapPal:** solo con `source = 1,1`, y el parámetro está desactivado en los controladores de helpers y proyectiles.
- **Barras de vida:** no admiten todas las animaciones y textos (el DKO siempre es texto). La documentación lo da por
  un descuido que se corregirá.
- **CNS:** se elimina `data.ko.echo`.
- **`mugen.cfg`:** desaparecen, entre otros, `AfterImageMax`, `LayeredSpriteMax`, `ExplodMax`, `SysExplodMax`,
  `HelperMax` y `PlayerProjectileMax`.

### Opciones que ayudan en Dreamcast

| Opción | Dónde | Qué hace |
|---|---|---|
| `portraits` | `[Select Info]` del motif | No carga los retratos pequeños ni grandes |
| `skipcharscriptloading` | `[Select Info]` del motif | No carga los scripts de los personajes en la selección, para forzar menos el lector. No funciona en Credits ni si `portraits` no es 0 |
| `staticassignments` | `[Config]` de `mugen.cfg` o `dolmexica.cfg` | Acelera las máquinas de estados a cambio de más memoria. Por defecto, 0 |
| carpeta `data/480p` | `data/` | Barras de vida de 480p o MUGEN 1.0 |

El Continue forma parte de la pantalla de combate, no es una pantalla aparte, para evitar tiempos de carga. Se configura
en `[Continue]` de `fight.def`.

### Crear el CDI

El kit viene en la descarga del motor (la versión actual está en itch.io) y tiene dos caminos: el programa
`Dolmexica Infinite Make-your-own-game kit for Dreamcast.exe` o el script de la carpeta `dreamcast`:

```bat
make_cdi.bat CD_NAME CdiName
make_cdi.bat DOLMEXICA_INFINITE_DEMO_100 DolmexicaInfiniteDemo100
```

- Los recursos van en `assets`, con la estructura de MUGEN (`chars/`, `data/`, `stages/`, `sound/`, `font/`).
- `data/mugen.cfg` puede llamarse `data/dolmexica.cfg`.
- Datos del IP.BIN: `dreamcast/ip/ip.txt`.
- Logo de arranque: `dreamcast/ip/INSERT.png`, con paleta, 320x90 y 128 colores como máximo.
- El CDI se graba en CD-R con Padus DiscJuggler.
- Usa cdi4dc, logotools y makeip.

### Añadidos al lenguaje

- Controladores `GlobalVarSet` y `GlobalVarAdd`, con variables que se conservan entre combates y storyboards.
- Triggers `GlobalVar`, `GlobalFVar`, `GlobalSVar`, `StrLen` y `Platform` (devuelve `"dreamcast"` en la consola). Con
  `Platform` un mismo CNS puede tener una rama para cada plataforma.
- En `select.def`, `dependency=` (p. ej. `dependency=globalvar(100)`) para personajes desbloqueables y `displayname=`.

## Notas / gotchas

- **Los nombres de fichero no pueden llevar guiones (`-`).** Los personajes de MUGEN suelen tenerlos: conviene revisarlos
  antes de generar el CDI.
- **Un `CD_NAME` largo rompe la creación de la imagen.** La documentación no da el límite: usa un nombre corto.
- **La documentación no da cifras de memoria** ni límites de sprites, ni dice cómo convertir assets aparte del logo. No
  aclara si un personaje SFF v2 cabe en la consola: hay que medirlo con el juego real.
- **Comparación con Hyper DBZ (deducido de las entradas de este repositorio, no probado):** el port usa `AfterImage`,
  `invertall` y `RemapPal` en los helpers de Saibamen y Dr. Briefs ([efectos en el PVR](efectos-visuales-en-dreamcast.md)).
  Según la documentación, Dolmexica no ofrece ninguno de los tres en Dreamcast, así que esos efectos se perderían o se
  verían distintos.

- **La resta sí se puede aproximar en el PVR** ([efectos](efectos-visuales-en-dreamcast.md)), aunque Dolmexica no la ofrezca.

### Cuándo conviene cada ruta

- **Dolmexica:** probar rápido si un personaje o juego arranca en la consola, sobre todo si es de la época de WinMugen y
  no depende de los efectos de la tabla.
- **Motor compilado a medida:** cuando el juego usa esos efectos, es un juego de 1.0 o 1.1, o hay que controlar la
  memoria y el coste por tick. Es mucho más trabajo.

## Referencias cruzadas

- [Compilar la lógica CNS de MUGEN](cns-compilacion.md)
- [Efectos visuales de MUGEN en el PVR de Dreamcast](efectos-visuales-en-dreamcast.md)
- [Formatos de MUGEN — SFF, AIR y SND](formatos-sff-air-snd.md)
- [Barra de vida, fuentes .fnt y asaltos](barra-de-vida-y-asaltos.md)
- [Bibliotecas, motores y recursos de Dreamcast](../../Dreamcast/herramientas/bibliotecas-motores-y-recursos.md)
