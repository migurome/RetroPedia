---
titulo: Neo Geo — síntomas y causas
plataforma: [NeoGeo]
categoria: [referencia]
tags: [depuracion, ngdevkit, paletas]
estado: borrador
comprobado: emulador
comprobado_en: "GnGeo y ngdevkit 0.5, RickAndMorty/neogeo"
fecha: 2026-11-06
fuentes: [NeoGeo/graficos/rom-c-s-y-paletas.md, NeoGeo/graficos/sprites-scb.md, NeoGeo/herramientas/caso-practico-simulockdown.md, NeoGeo/herramientas/ngdevkit-windows.md]
---

# Neo Geo — síntomas y causas

Todo pasó en `RickAndMorty/neogeo`, con GnGeo o al montar el entorno. Nada está comprobado en la consola.

| Síntoma | Causa probable | Cómo confirmarlo | Entrada |
|---|---|---|---|
| Un personaje "de 12 colores" no cabe en una paleta | Tonos sueltos de 1–5 píxeles por ruido de edición | Contar colores; quedarse con los de ≥ 0,1 % de los píxeles y llevar el resto al más cercano | [ROM C y paletas](graficos/rom-c-s-y-paletas.md) |
| Las cuentas de tamaño de un personaje no cuadran | La hoja está exportada a escala no entera (×3,5, no ×1) | Medir la escala antes de decidir | [caso práctico](herramientas/caso-practico-simulockdown.md) |
| Los gráficos salen mal y no se sabe si es la ROM C | Conversión o motor | Decodificar los tiles ya codificados (`preview_<pj>.png`): si la vista previa sale bien, la ROM C está bien | [ROM C y paletas](graficos/rom-c-s-y-paletas.md) |
| El personaje se mueve al revés en vertical | SCB3 guarda `496 - y`: subir es sumar | Revisar la conversión | [sprites](graficos/sprites-scb.md) |
| El personaje se detiene lejos del borde | Los límites se aplican a la caja de tiles, más ancha que el dibujo | Aplicarlos a los pies (el pivote) | [sprites](graficos/sprites-scb.md) |
| Falta texto en los bordes de la capa fix | GnGeo, como muchos televisores, recorta 8 px por lado | Usar las columnas 1–38 | [sprites](graficos/sprites-scb.md) |
| El texto pisa los pies de los personajes | La capa fix va siempre encima | Reservar filas de HUD y limitar la `y` | [caso práctico](herramientas/caso-practico-simulockdown.md) |
| `pacman -S` falla al descargar | Fallo de un mirror | Usar `pacman -U` sobre los paquetes locales; comprobar con `pacman -Q` y `pacman -Dk` | [ngdevkit](herramientas/ngdevkit-windows.md) |
| La captura F2 de GnGeo no funciona | Está en código desactivado de esa versión | No usarla en pruebas | [ngdevkit](herramientas/ngdevkit-windows.md) |
| MAME no arranca la ROM con nullbios | MAME exige la BIOS original | Usar GnGeo | [ngdevkit](herramientas/ngdevkit-windows.md) |

Sin ver todavía: lo que pase al superar los sprites por línea ([límites](LIMITES.md)).
