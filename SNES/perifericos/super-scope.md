---
titulo: Super Scope (pistola de luz de SNES)
plataforma: [SNES]
categoria: [perifericos]
tags: [super-scope, pistola-de-luz, io, protocolo, ppu]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Super_Scope]
---

# Super Scope (pistola de luz de SNES)

## Resumen

El Super Scope es la pistola de luz oficial de SNES; se conecta al puerto del segundo mando y detecta hacia dónde apunta el jugador mediante un sensor óptico que capta el barrido de luz de un televisor CRT. Es inalámbrico entre el arma y un receptor conectado a la consola, comunicándose por infrarrojos. Esta entrada documenta el formato de datos que expone al sistema y el mecanismo óptico/temporal en el que se apoya.

## Detalle técnico

### Principio óptico de detección

El arma incorpora una óptica que limita la visión a una pequeña sección de la pantalla en cada instante, y filtra la luz roja (que persiste más tiempo en la fósforo de un CRT) para no interferir en la detección. La luz filtrada llega a un detector que procesa tanto pulsaciones de botones como la posición de apuntado. Para que una imagen resulte detectable por el sensor, debe contener al menos un 40% de componente verde y/o al menos un 60% de componente azul.

### Formato de datos (lectura automática, 16 bits)

```
JOY2H ($421B) | JOY2L ($421A)
FCTP 00nN     | 1111 1111
```

- **F**: botón de disparo (Fire)
- **C**: botón de Cursor
- **T**: interruptor de Turbo
- **P**: botón de Pause
- **n**: Null Flag — receptor activo pero sin apuntador válido detectado
- **N**: Noise Flag — los datos recibidos están corruptos/son ruido
- El resto de bits permanecen fijos según la especificación (los bits bajos del byte de arriba son cero, los del byte de abajo son todo unos)

En lectura manual mediante JOYSER1, el botón Fire se transmite antes que el de Cursor.

### Códigos de comando por infrarrojos

El arma transmite seis códigos distintos de 5 bits desde el emisor al receptor:

| Comando | Código binario (5 bits) |
|---------|--------------------------|
| Pause | `10001` |
| Single Fire | `11001` |
| Turbo Fire | `11101` |
| Cursor | `10011` |
| Turbo + Cursor | `11111` |
| Single + Turbo | `11011` |

Cada bit se transmite de forma redundante: para que un bit se interprete como alto, al menos 5 de cada 8 pulsos que lo componen deben estar en alto.

### Temporización

- Cada comando completo dura aproximadamente 5.5 ms (cerca de 1/3 de un fotograma de vídeo).
- Existe una ventana de detección de aproximadamente 85 ms (unos 5 fotogramas) para que el receptor capte el comando.
- Tras cada ciclo hay un período de reposo de 10 ms antes de que pueda comenzar el siguiente.
- Cuando el LED emisor se ha activado 6 veces dentro de un ciclo, el receptor genera un pulso alto y luego bajo que sirve para hacer "latch" de las coordenadas de pantalla en ese instante.

### Integración con la PPU

Cuando la línea I/O del puerto del segundo mando se asierta (evento generado por el propio Super Scope al detectar posición), la PPU actualiza automáticamente los registros OPHCT y OPVCT con la coordenada de barrido que se estaba renderizando en ese momento. Estos registros son los que el software debe leer para obtener la posición de apuntado en pantalla.

## Notas / gotchas

- Existe un retraso inherente entre el disparo físico y el "latch" de posición, por lo que los juegos comerciales solían incluir una pantalla de calibración para que el jugador ajustara el punto de mira percibido.
- Si se pulsan Fire o Cursor mientras el Turbo está activo, el ciclo de transmisión se reinicia inmediatamente (no espera a completar el ciclo en curso).
- Cuando se reciben varios comandos simultáneos, sus valores se combinan mediante un OR a nivel de bits antes de interpretarse.
- El Null Flag y el Noise Flag son imprescindibles para descartar lecturas de posición inválidas: sin apuntador detectado (n) o con datos corruptos (N), la posición reportada por OPHCT/OPVCT no debe usarse.

## Referencias cruzadas

- (pendiente de enlazar)
