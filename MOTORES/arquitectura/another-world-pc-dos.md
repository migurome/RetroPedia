---
titulo: Los polígonos de Another World — el port a IBM PC/DOS
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, pc-dos, vga, ega, polígonos-2d, rendimiento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons_PC_DOS/index.html]
---

# Los polígonos de Another World — el port a IBM PC/DOS

## Resumen

Analiza cómo Daniel Morais portó el motor de polígonos 2D vectoriales de Another World (Éric Chahi, 1991) al PC/DOS de la época, un ecosistema "horizontal" (CPU de Intel, sistema de Microsoft, tarjetas de vídeo de fabricantes distintos) con una fragmentación de hardware de vídeo brutal frente a las plataformas "verticales" (Amiga, Atari ST) para las que se diseñó originalmente el motor. Relevante porque muestra cómo un mismo formato de datos de polígonos se adapta a un framebuffer mapeado en memoria sin blitter dedicado, mediante trucos de bajo nivel sobre los registros de EGA/VGA.

## Detalle técnico

### Contexto histórico y arquitectura

El port de PC exigía una capa de abstracción sofisticada por culpa de la variación masiva de dispositivos de vídeo. Perfil de CPU/memoria "típico" de 1991 usado como referencia:

- Intel 386 a 16 MHz
- 2 MiB de RAM
- Sin blitter hardware (a diferencia del Amiga)
- Controlador DMA dedicado a disquetera/HDD (no reutilizable para gráficos)

### Evolución del sistema de vídeo (1981-1991)

| Estándar | Año  | Resolución        | Colores | VRAM  |
|----------|------|-------------------|---------|-------|
| MDA/CGA  | 1981 | 360×400 a 720×400 | 2-16    | 16KiB |
| TGA      | 1984 | 320×200           | 16      | 64KiB |
| EGA      | 1985 | 640×350           | 16      | 128KiB|
| VGA      | 1987 | 640×480           | 16+     | 256KiB|

Problema clave: las tarjetas TGA/EGA antiguas usaban color de 2 bits por canal (6 bits totales por píxel), mientras que la VGA soportaba 6 bits por canal (18 bits por píxel).

### El problema crítico de rendimiento

El diseño de vídeo mapeado en memoria era catastróficamente lento para animación. Un borrado de pantalla ingenuo lo demuestra:

```c
char far * VGA = ( byte far *) 0xA0000000L ;
#define SC_MAPMASK  0x02
#define SC_INDEX    0x03c4
#define SC_DATA     0x03c5

void selectBank(uint_8 bank) {
  outp(SC_INDEX , SC_MAPMASK );
  outp(SC_DATA , 1 << bank );
}  

void ClearScreen() {
  for(int y = 0 ; y < 200 ; y ++) {
    for(int x = 0; x < 160 ; x ++) {
      selectBank(x % 4);
      VGA[x + y * 200] = 0;
    }
  }
}
```

Resultado: "5fps" con el borrado ingenuo de izquierda a derecha. El borrado vertical optimizado alcanzaba solo "10fps" incluso en el mejor hardware de 1991. El artículo señala que "Wolfenstein 3D ... dibujaba todo ... verticalmente ... e incluso así solo se refrescaba el 72% de la pantalla cada frame para llegar a 11fps en un 386SX a 16Mhz."

### Selección de modo gráfico

**Camino TGA/EGA:**
- Modo 9h (320×200, 16 colores)
- TGA: requería una extensión especial de VRAM de 128KiB con "memoria entrelazada de cuatro vías" (four-way interleaved memory)
- Los framebuffers se alojaban en RAM (no en VRAM), los píxeles "distribuidos linealmente como nibbles (medio byte) codificando índices de color de 4 bits"
- Se requería conversión manual de paleta (4 bits a 2 bits), causando "aliasing de color"

**Camino EGA/VGA:**
- Ambos usaban el modo Dh (320×200, 16 colores)
- La VRAM se accedía mediante una "ventana de RAM mapeada en memoria de 64 KiB"
- El framebuffer "está organizado ... salvo que cada bitplane está en un banco de memoria dedicado. Hay cuatro bancos"
- Tarjetas VGA: 256KiB de VRAM (4×64KiB); tarjetas EGA: 128KiB (4×32KiB)

Descubrimiento crítico citado en el artículo: "la tarjeta VGA no se comportaba como la tarjeta EGA en el modo Dh. La carga de paleta no esperaba color de 6 bits sino color de 18 bits, como en el modo 13h."

### Soluciones para el renderizado de polígonos

#### Dibujado de contornos (opcode DRAWN)

Enfoque: "un simple algoritmo de doble Bresenham genera las coordenadas de inicio y fin de las líneas horizontales en espacio de pantalla."

**El truco de la máscara de banco (bank mask):** en lugar de escribir secuencialmente en cada banco, "la máscara de banco puede configurarse para escribir en todos los bancos a la vez."

```
Escribir 1 byte en RAM escribe 4 bytes en VRAM
```

Resultado: "ocho píxeles horizontales consecutivos fijados con un único byte escrito," produciendo una "aceleración de 8x." Morais declaró: "recuerdo que dibujar polígonos era ENDIABLADAMENTE RÁPIDO ... el framerate acabó bastante por encima del objetivo de 24 fps."

#### Relleno (opcode FILL)

Aprovechó que los polígonos son de un solo color y los latches de la VGA — "cada banco tiene un latch de un byte. Al 'recordar' el último byte leído, el trazado de bits se facilita considerablemente."

Reutilizaron los latches para "leer y escribir cuatro bytes a la vez," logrando una "aceleración de 4x" en esta operación.

#### Copia de framebuffers (opcode COPY FB)

El reto crítico: almacenar el fondo requería copiar entre VRAM y RAM. La solución invocó la técnica "Mode X Marks the Latch" — almacenando BKGD en VRAM junto con los doble-buffers, logrando una "aceleración de 4x".

### Arquitectura de software

Morais implementó "una capa de abstracción basada en punteros a función" enteramente en ensamblador x86: "la VM estaba escrita al 100% en asm x86."

Los usuarios ejecutaban CONFIG.EXE (herramienta en modo texto) para especificar los periféricos; los parámetros se guardaban en CONFIG.DAT. Al arrancar, ANOTHER.EXE leía el fichero y "asignaba los punteros a función según las tarjetas de vídeo y sonido a usar."

### Implementación de audio

El subsistema más difícil. El PC carecía de las capacidades PCM de Sound Blaster/Gravis UltraSound. Solución: modificar el timer de IRQ desde "80 o 100 Hz hasta 8000Hz," interrumpiendo el programa principal "8.000 veces por segundo" para "hacer vibrar el altavoz interno del PC o el AdLib a la frecuencia deseada, simulando una muestra PCM."

Morais describió el resultado: "lejos del original, pero si arrugas las orejas casi te lo puedes creer" (far from the original but if you frown your ears you could kinda believe it).

### Especificaciones finales

- Resolución: 320×200 (igualando Atari ST/Amiga)
- Colores: 16 por paleta
- Configuración mínima: CPU 286 a 10MHz + 640KiB RAM + cualquier tarjeta TGA/EGA/VGA
- Objetivo de framerate (alcanzado): 24 fps, por mandato de Éric Chahi
- Contenido añadido: 15 niveles (aumentado desde 12) tras la queja de Interplay de que el juego se completaba en 20 minutos

### Referencias técnicas citadas por el autor del port

Morais acreditó el artículo "Mode X Marks the Latch" de Michael Abrash y un libro técnico sobre latches de EGA/VGA comprado en una librería de París ("Le monde en tique") como recursos clave que hicieron viable el port.

## Notas / gotchas

- Sin blitter ni DMA de gráficos disponibles: todo el rendimiento depende de trucos sobre los registros de bank/latch de EGA/VGA, no de aceleración hardware.
- El acceso a VRAM mapeada en memoria es intrínsecamente lento si se hace ingenuamente (5-10 fps solo para borrar pantalla); hay que explotar la máscara de banco y los latches para escribir 4 bytes (o más) por operación.
- Diferencias sutiles entre tarjetas del "mismo" modo (EGA vs VGA en modo Dh) rompían la carga de paleta (6 bits vs 18 bits) — la fragmentación de hardware obligaba a detectar la tarjeta concreta, no solo el modo de vídeo.
- El audio fue el subsistema más comprometido: sin PCM nativo, se simuló mediante interrupciones de timer a 8kHz forzando el altavoz interno/AdLib — una solución claramente inferior a la de otras plataformas.
- La abstracción de periféricos se resolvió con una capa de punteros a función seleccionada en tiempo de arranque (CONFIG.EXE/CONFIG.DAT), un patrón directamente aplicable a separar motor de plataforma en cualquier engine multiplataforma.

## Referencias cruzadas

- MOTORES/arquitectura/another-world-genesis.md
- MOTORES/arquitectura/another-world-snes.md
- MOTORES/arquitectura/another-world-gba.md
- MOTORES/arquitectura/another-world-jaguar.md
