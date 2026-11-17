---
titulo: Receta — del PNG al sprite en pantalla en Dreamcast
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [sprites, powervr, kos, toolchain, emuladores]
estado: revisado
comprobado: emulador
comprobado_en: "parcial: Flycast 2.7 con snippets/dreamcast-kos/sprite-mando; dibujo y transparencia sí, cruceta no"
fecha: 2026-11-06
fuentes: [snippets/dreamcast-kos/sprite-mando, KOS de DreamSDK R4, proyecto hyperdbz-dreamcast-port (tools/make_disc.sh, tools/make_ip.py)]
---

# Del PNG al sprite en pantalla

## Resumen

Encadena las piezas que están en entradas separadas: convertir la imagen, subirla a la VRAM, dibujarla, montar el disco y
arrancarlo. El código completo está en [`snippets/dreamcast-kos/sprite-mando`](../../snippets/README.md).

## Detalle técnico

### Atajo

```sh
sh snippets/dreamcast-kos/sprite-mando/construir.sh      # genera out/sprite.cdi
C:\Users\migur\Desktop\Tools\flycast-win64-2.7\flycast.exe C:\ruta\absoluta\out\sprite.cdi
```

### Los pasos

| Paso | Qué se hace | Dónde | Más detalle |
|---|---|---|---|
| 1. Imagen | PNG con lados potencia de 2, de 8 a 1024; aquí, 32×32 | `robot.png` | [límites](../LIMITES.md) |
| 2. Convertir | De RGBA a ARGB4444, en una cabecera C | `png2argb.py` | [texturas](../graficos/texturas-paletas-y-sprites.md) |
| 3. Subir | `pvr_mem_malloc` y `pvr_txr_load` | `main.c` | [texturas](../graficos/texturas-paletas-y-sprites.md) |
| 4. Describir | `pvr_poly_cxt_txr` en la lista translúcida, sin filtro; `pvr_poly_compile` | `main.c` | [PowerVR](../graficos/powervr-pipeline.md) |
| 5. Dibujar | Cabecera y cuatro vértices en tira, el último con `PVR_CMD_VERTEX_EOL` | `main.c` | [PowerVR](../graficos/powervr-pipeline.md) |
| 6. Compilar | `make` con el entorno de KOS | `construir.sh` | [DreamSDK](dreamsdk-windows.md) |
| 7. Disco | `objcopy`, `scramble`, `IP.BIN`, `mkisofs` y `cdi4dc` | `construir.sh` | [imagen CDI](imagen-cdi.md) |
| 8. Probar | Flycast, con ruta absoluta | — | [Flycast automatizado](flycast-automatizado.md) |

### Para un juego de verdad

Este ejemplo prioriza que se lea, no que rinda:

- **Formato:** ARGB4444 da 16 niveles de alfa, pero pierde color. Para sprites con paleta, 4 u 8 bpp ahorran mucha VRAM.
- **Muchos sprites:** una textura por imagen no escala. Ver la [caché de sprites](../graficos/cache-sprites-vram.md).
- **Datos:** la imagen va compilada dentro del ejecutable. Un juego la lee de `/cd`: ver
  [lectura del CD](../avanzado/lectura-del-cd-kos.md).

## Notas / gotchas

- **El fondo sale verde en Flycast:** con `pvr_set_bg_color(0.1, 0.1, 0.3)` se vio (25, 127, 0). Ya estaba visto en
  Hyper DBZ; falta confirmarlo en la consola.
- **Ruta sin espacios:** los makefiles de KOS no los admiten.
- **`make_ip.py`** existe porque `makeip.exe` no arranca en DreamSDK R4.

## Referencias cruzadas

- [Snippets](../../snippets/README.md)
- [Texturas, paletas y sprites](../graficos/texturas-paletas-y-sprites.md)
- [Imagen de disco CDI](imagen-cdi.md)
- [Síntomas de Dreamcast](../SINTOMAS.md)
