---
titulo: Los polígonos de Another World — el port a Atari Jaguar
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, jaguar, blitter, gpu, polígonos-2d]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons_Jaguar/index.html]
---

# Los polígonos de Another World — el port a Atari Jaguar

## Resumen

Analiza el port homebrew de Sébastien Briais (proyecto de 2007 a 2013) del motor de Another World a Atari Jaguar, una consola con arquitectura de cinco procesadores (68000, TOM, JERRY, Object Processor, GPU+Blitter) mucho más compleja que Genesis, SNES o GBA. Relevante como caso extremo de "demasiado hardware especializado sin documentación fiable": el reparto de trabajo entre CPU, GPU y Blitter, y el uso del Object Processor para tratar el framebuffer completo como un único sprite, muestran tanto el potencial como el coste de programar múltiples procesadores paralelos poco documentados.

## Detalle técnico

### Arquitectura del Atari Jaguar

**Visión general del sistema:** la Jaguar (lanzada en noviembre de 1993) contaba con cinco procesadores:
- **Motorola 68000:** 13.295 MHz, 32 bits interno / bus de 16 bits
- **TOM (procesador gráfico):** 26.59 MHz, RISC de 32 bits con bus de 64 bits
- **JERRY (procesador de audio):** 26.59 MHz, RISC de 32 bits con 8KiB de SRAM
- **Object Processor:** motor de sprites, 64 bits interno/bus
- **GPU:** 32 bits con 2KiB de RAM dedicada, pilota el Blitter

**Arquitectura de memoria:**
- TOM: SRAM dedicada para operaciones gráficas
- JERRY: 8KiB de SRAM (más generosa que la de TOM)
- DRAM general: memoria de sistema compartida
- Capacidad de cartucho: espacio de direcciones de 4MiB (6MiB con banking de memoria)

Cada procesador RISC se alimenta desde su SRAM local para evitar contención en el bus de datos.

**Espacio de color: formato CRY.** La Jaguar usaba un sistema de color propio incompatible con monitores sRGB modernos:
- Primer byte: coordenadas X,Y en espacio 2D = 256 colores
- Segundo byte: control de oscuridad/brillo = 256 tonalidades
- Total: 256 colores × 256 tonalidades = 65.536 colores

### Sistema de salida de vídeo

**Object Processor (motor de sprites):**
- Genera la señal de vídeo hacia el televisor
- Lee comandos de 64 bits ("phrase")
- Cinco tipos de comando disponibles
- Capacidades: escalado de sprites (sin penalización de rendimiento), fuente CRY de 16 bits, color verdadero de 24 bits, paleta (CLUT) con índices de 1/2/4/8 bits
- Versátil: puede tratar el framebuffer entero como un único sprite gigantesco

**Unidad GPU + Blitter:**
- El GPU tiene instrucciones 3D especiales (manipulación de matrices)
- Pilota al Blitter para renderizar líneas de píxeles
- Soporta Gouraud shading y Z-testing
- Totalmente programable, sin pipeline fijo

### Retos técnicos y decisiones de diseño

**Citas de los desarrolladores sobre la dificultad del sistema:**

Sobre la complejidad del Blitter: "Programar el Blitter en Jaguar es una forma de arte donde hay que navegar comportamientos sutiles no documentados y bugs." — Sébastien Briais

Sobre el cuello de botella del 68000: "El 68k era lento. Este fue el problema principal del sistema." — John Carmack, señalando la ventaja de PlayStation con su procesador unificado.

**Comportamientos hardware no documentados encontrados:**
- El Object Processor modifica la lista de objetos durante las operaciones de lectura
- Bugs de "scoreboarding": los registros no pueden leerse inmediatamente después de escribirse (requiere relleno con NOP)
- La rotación del Blitter estaba pobremente documentada
- Existía un modo de detección de colisiones que nunca se logró aplicar por ingeniería inversa

### Implementación de Another World

**Cronología de desarrollo:** inicio en 2007 (a raíz de una discusión en una Convención Atari); finalización en 2013 (proyecto de siete años); lanzamiento limitado a unas ~200 copias por tirada.

**Construcción del toolchain:**
- **Ensamblador:** MadMac (herramienta original de alto rendimiento de Atari) — soporta instrucciones 68K y RISC de TOM/JERRY; closed source, formato a.out, 32 bits; requirió un módulo de kernel especial para ejecutar binarios de 27 años en Linux moderno
- **Linker:** inicialmente ALN (Atari Linker), reemplazado por un jlinker propio
- **Compilador C:** paquetes GCC/binutils mantenidos por Vincent Rivière para ArchLinux, apuntando a 68K
- **Herramienta de imágenes:** jconverter propio para generación de imágenes CLUT
- **Plataforma de desarrollo:** prototipado en C sobre ArchLinux sin audio, luego conversión a ASM
- **Despliegue en hardware:** cartucho Skunkboard con almacenamiento flash, conexión USB al PC

**Estrategia de implementación de la máquina virtual.** En lugar de traducir directamente la versión 100% ASM 68000 de Atari ST, Briais implementó:

1. **Ejecución de la VM:** la mayor parte del bytecode corre en el 68000
2. **Compilación JIT:** las funciones de bytecode se convierten a funciones 68K sobre la marcha
3. **Renderizado de polígonos:** rutinas de GPU optimizadas
4. **Reparto de trabajo:**
   - La VM extrae los parámetros de dibujo del bytecode: baseSprite, index, x, y, scale
   - Los parámetros se escriben en la SRAM de TOM
   - Una rutina de GPU gestiona el renderizado de polígonos vía Blitter como líneas de píxeles
   - Las llamadas al Blitter retornan antes de completarse; el 68000 retoma la interpretación de bytecode mientras terminan las operaciones de dibujo restantes
   - Object Processor: papel mínimo, renderiza el framebuffer como un único sprite

**Decisión de formato de píxel.** Briais eligió **CLUT de 8 bits por píxel** pese a que el original solo usa 16 colores:
- Razón: permitir los fondos remasterizados de 256 colores
- Alternativa (4 bits): "complicaría las cosas para poca ganancia"
- Motivación personal: "traumatizado por el Atari ST, que usaba 4 bits y era un suplicio de gestionar"

**Pipeline de assets de fondo:**

Método original: polígonos codificados en el bytecode.

Innovación de la edición del 20º aniversario: se introdujo el opcode #25 (0x19 LOADRES) para cargar un bitmap directamente en el framebuffer #0. Seguía renderizando los fondos con polígonos y después los sobrescribía con el bitmap remasterizado, permitiendo alternar sin fisuras entre modo "clásico" y "remasterizado".

**Implementación en Jaguar:**
- Se usaron los fondos de 1152x720 a 256 colores de la Edición Aniversario
- La capacidad del cartucho forzó reducir la resolución a 320x200
- **Método de compresión:** códec LZ77 con rutina de descompresión propia en GPU
- Resultado: los fondos se redujeron de 6MiB a 4MiB (código incluido)
- Excepción: la escena del ascensor en la prisión requirió almacenamiento sin comprimir (por el rendimiento del scroll)

Cita sobre el rendimiento de la compresión: "funcionó bien salvo en la escena del ascensor en la prisión, donde interviene un scroll."

**Implementación de audio.** Enfoque: un emulador de Paula corriendo en JERRY, adaptando las rutinas de audio de alto nivel en 68000 del Amiga. El emulador de Paula basado en JERRY habla con el DSP. Sin documentación nativa; se hizo ingeniería inversa a partir del código fuente de Amiga.

**Rendimiento de framerate:**
- Objetivo: 25Hz (estándar PAL)
- Real: excedió el framerate máximo
- Problema: el registro de la VM que controla la duración de visualización del frame no estaba poblado para escenas intensivas en gráficos (la arena de tanques)
- Solución: inyección manual de delay en las secciones críticas de rendimiento
- Optimización no aprovechada: un análisis posterior al lanzamiento reveló capacidad sobrante suficiente para implementar una versión entrelazada a 640x480

### Bug notable encontrado

**Bug de desplazamiento con signo:**
- Síntoma: el personaje Lester se quedaba atascado frente a puertas automáticas
- Causa raíz: el desplazamiento a la derecha (right shift) se implementó con una variable con signo, corrompiendo el patrón de bits con unos no deseados por la izquierda
- Solución: cambiar a entero sin signo
- Dificultad de depuración: no existía debugger para Jaguar; el prototipo en Linux fue crítico para el diagnóstico

### Producción del cartucho ("Le Tour de France")

**Fabricación de PCB:** socio inicial en Alemania (calidad insatisfactoria); socio final Tom-IC (Suiza); diseño con componentes flash modernos en vez de EEPROM (PCB Jagtopus); soporte de electrónica de SCPCD y Zerosquare (diseño de PCB).

**Carcasa física:** stock sobrante comprado a Best Electronics y B&C; hojas 2D precortadas dobladas en cajas 3D, ensambladas a mano una por una.

**Impresión de la caja:** herramienta de diseño Inkscape; enfoque imitando la estética original de los juegos de Jaguar (capturas de pantalla con esquinas redondeadas); búsqueda de imprenta en París mayormente rechazada al principio; solución final: una empresa de Toulouse dispuesta a imprimir y cortar, con un mínimo de 1000 unidades por tirada.

**Documentación del manual:** herramienta LaTeX; impresión en una imprenta de Orleans.

**Programación de cartuchos ROM:** proceso tedioso, uno por uno; hardware necesario: una Jaguar especial con la BIOS de desarrollador BJL (Behind Jaggy Lines); conexión: PC a la consola vía puerto paralelo conectado al puerto del joypad; procedimiento: insertar cartucho en blanco, subir la ROM desde el PC.

### Protección anticopia y autenticación RSA

**Sistema de seguridad asimétrica de Atari:** los cartuchos de Jaguar (también 7800 y Lynx) empleaban cifrado RSA de 520 bits.

**Proceso de arranque:**
1. Una cabecera cifrada arranca el cartucho, dividida en bloques de 65 bytes
2. Cada bloque se cifra con la clave completa de 520 bits
3. El código se descifra en el GPU y luego se ejecuta
4. El código en ejecución calcula un hash MD5 del cartucho
5. Compara el hash calculado con el hash embebido
6. Si es válido, escribe el valor mágico 0x03D0DEAD en una dirección de RAM del GPU

**Proceso de firmado:** el estudio envía el registro maestro a Atari; Atari usa la clave privada para generar el programa RISC firmado + MD5; el programa/hash firmado se graba en el cartucho; la Jaguar contiene la clave pública para la validación.

**Problema de pérdida de la clave privada:**
- Crisis: bancarrota de Atari; la clave privada supuestamente se perdió
- Workaround (Battlesphere 2000): los desarrolladores encontraron un arranque RISC firmado ya existente y modificaron la carga útil del juego hasta lograr una colisión MD5
- Resolución (12 de noviembre de 2003): la clave privada "se encontró" y fue publicada por atarimuseum.com
- Resultado: la comunidad homebrew generó un programa firmado que escribe 0x03D0DEAD independientemente del hash de la carga útil, habilitando el Skunkboard

### Resumen de dificultades de desarrollo

Los aspectos más difíciles según Briais:

1. **Documentación:** "los manuales de Atari pueden ser bastante enigmáticos a veces, y algunos tutoriales online no son fiables"
2. **Rotación del Blitter:** proceso de implementación no documentado
3. **Detección de colisiones:** modo avanzado del Blitter nunca aplicado por ingeniería inversa
4. **Peculiaridades de hardware:** comportamientos sutiles que requerían experimentación
5. **Depuración:** cero disponibilidad de debugger; el prototipado en Linux fue esencial

**Contribución de conocimiento:** para ayudar a futuros desarrolladores, Briais publicó rmvlib (repositorio en GitHub), conteniendo: código del emulador RISC de Paula, implementación del renderizador de polígonos, implementaciones del gestor de sprites, y conocimiento documentado del hardware de Jaguar.

### Análisis comparativo con otras plataformas

**Frente a SNES/Genesis:** las plataformas objetivo originales dominaban con una arquitectura de procesador único; el diseño de cinco procesadores de la Jaguar representaba una complejidad sin precedentes para los desarrolladores.

**Frente a PlayStation.** Según Carmack: la PlayStation "estaba programada como un único procesador serie con un único acelerador rápido", frente a la Jaguar, que requería "trozos de ensamblador paralelos superpuestos" (overlayed parallel asm chunks).

**Frente al port de 3DO:** el port de 3DO fue pionero en el enfoque de fondos remasterizados; los artistas de 3DO tomaron una postura agresiva de temperatura de color/forma; la Edición Aniversario intentó preservar el tono de color original.

### Lanzamiento y desempeño de mercado

- Tirada de producción 1 (2013): ~200 copias
- Tirada de producción 2: segundo lote completado, agotado posteriormente
- Lista de espera mantenida en la web de The Removers
- Valoración de calidad del juego: "entre los mejores disponibles en la plataforma"

### Retrospectiva de plataforma

Conclusión del autor: "si la consola hubiera contado con más juegos de calidad como Another World y DOOM, estoy convencido de que los jugadores podrían haber superado el precio, el mando, e incluso la infame afirmación de '64-bit'." Causa raíz del fracaso de mercado, según el autor: "la dificultad superior a la media involucrada en producir buenos juegos. Los estudios no tenían tiempo para elaborar instrucciones RISC y navegar los bugs."

## Notas / gotchas

- La Jaguar es el caso extremo de esta serie: cinco procesadores (68000, TOM, JERRY, Object Processor, GPU+Blitter) frente al procesador único de Genesis/SNES/GBA/PC. El propio John Carmack señala que el 68000 "lento" era el problema principal, y que programar la Jaguar exigía "trozos de asm paralelos superpuestos" en vez del modelo serie con un acelerador rápido de PlayStation — un contraste directo sobre los costes de la complejidad multiprocesador sin herramientas maduras.
- El Blitter no tiene documentación fiable: su rotación es "poco documentada" y su modo de detección de colisiones nunca se descifró por ingeniería inversa pese al esfuerzo dedicado. El desarrollador lo resume como "una forma de arte" que exige navegar bugs y comportamientos no documentados — el riesgo genérico de depender de hardware potente pero mal documentado, sin debugger disponible.
- El reparto de trabajo (VM/bytecode en 68000, JIT de funciones de bytecode a 68K, renderizado real vía rutina de GPU que pilota el Blitter, llamadas no bloqueantes al Blitter para solapar dibujo con interpretación de bytecode) es el diseño más sofisticado de toda la serie de ports — aprovecha el paralelismo real del hardware en vez de recaer en un framebuffer software puro como Genesis/SNES/GBA.
- El bug del "right shift con signo" (una variable con signo usada para un desplazamiento a la derecha, introduciendo unos no deseados) es un recordatorio genérico válido para cualquier motor en C/ASM: los desplazamientos aritméticos vs lógicos deben elegirse explícitamente según el tipo de dato, y sin debugger este tipo de bug puede tardar mucho en diagnosticarse (aquí se resolvió gracias al prototipo Linux).
- Decisión de formato de píxel (8bpp CLUT en vez de 4bpp) motivada en parte por trauma de un port anterior (Atari ST con 4 bits) y en parte por necesidad de soportar los fondos remasterizados de 256 colores del 20 aniversario — ilustra cómo una decisión de "qué versión del juego soportar" (clásica vs remasterizada) puede determinar el formato de píxel de todo el motor.
- La protección anticopia RSA de 520 bits con clave privada "perdida" por la bancarrota de Atari, y luego recuperada, es una curiosidad de la industria más que una lección de arquitectura de motor, pero explica por qué el homebrew en Jaguar solo fue posible gracias al hallazgo tardío de la clave (o el bypass previo por colisión MD5 con Battlesphere 2000).

## Referencias cruzadas

- MOTORES/arquitectura/another-world-pc-dos.md
- MOTORES/arquitectura/another-world-genesis.md
- MOTORES/arquitectura/another-world-snes.md
- MOTORES/arquitectura/another-world-gba.md
