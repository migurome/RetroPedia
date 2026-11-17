---
titulo: Sprites obligatorios y grupos recomendados de un personaje de MUGEN
plataforma: [General]
categoria: [motores]
tags: [mugen, sprites, formatos, animacion, referencia]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [https://network.mugenguild.com/justnopoint/1.0docs/spr.html]
---

# Sprites obligatorios y grupos recomendados

## Resumen

Numeración `grupo,índice` que todo personaje debe respetar. Los obligatorios tienen que estar en el SFF aunque sus
animaciones no los usen, porque se necesitan para que se vea bien al ser lanzado. Un conversor puede avisar si faltan.

## Detalle técnico

### Obligatorios

| Grupo | Índices | Situación | Eje |
|---|---|---|---|
| 5000 | 0, 10, 20 | Golpe alto de pie, con la cabeza cada vez más atrás | Pies |
| 5001 / 5002 | 0, 10, 20 | Los mismos frames | Torso / cabeza |
| 5010 | 0, 10, 20 | Golpe bajo de pie, inclinándose cada vez más | Pies |
| 5011 / 5012 | 0, 10, 20 | Los mismos frames | Torso / cabeza |
| 5020 | 0, 10, 20 | Golpe agachado, con la cabeza cada vez más atrás | Pies |
| 5030 | 0 a 50, de 10 en 10 | Golpe por detrás, en el estómago y caída | Donde estarían los pies |
| 5031 / 5032 | 0 a 50, de 10 en 10 | Los mismos frames | Torso / cabeza |
| 5040 | 0, 10, 20 | Caída al suelo, tendido e impacto | Nivel del suelo |
| 5041 / 5042 | 0, 10, 20 | Los mismos frames | Centro del cuerpo / cabeza |
| 5060 | 0, 10 | Golpe hacia arriba, con la cabeza arriba o abajo | Donde estarían los pies |
| 5061 / 5062 | 0, 10 | Los mismos frames | Torso / cabeza |
| 5070 | 0, 10, 20 | Lanzado, cada vez más horizontal | Donde estarían los pies |
| 5071 / 5072 | 0, 10, 20 | Los mismos frames | Torso / cabeza |

Los índices intermedios (de 5000,1 a 5000,9) son opcionales.

### Recomendados

| Grupo | Significado |
|---|---|
| 0 | De pie |
| 5 | Girando |
| 10 / 11 / 12 | De pie a agachado / agachado / de pie a agachado |
| 20 / 21 | Caminar adelante / atrás |
| 40 | Inicio de salto, en el suelo |
| 41 / 42 / 43 | Subiendo: neutro / adelante / atrás |
| 44 / 45 / 46 | Bajando: neutro / adelante / atrás |
| 47 | Aterrizaje |
| 100 / 105 | Correr adelante / salto corto atrás |
| 120 / 121 / 122 | Inicio de guardia: de pie / agachado / aire |
| 130 / 131 / 132 | Guardia: de pie / agachado / aire |
| 140 / 141 / 142 | Fin de guardia: de pie / agachado / aire |
| 150 / 151 / 152 | Golpe en guardia: de pie / agachado / aire |

## Notas / gotchas

- **Eje horizontal:** alrededor del centro del personaje, salvo que se indique otra cosa.
- **Grupos 10 y 12:** la documentación describe los dos como «de pie a agachado».
- **Para un port:** estos sprites cuentan como memoria aunque el personaje no los use en sus animaciones; no se pueden
  quitar sin comprobar los estados de recibir golpes.
- La página no habla de tamaños de sprite.

## Referencias cruzadas

- [Formatos SFF, AIR y SND](formatos-sff-air-snd.md)
- [Física y golpes del motor](fisica-y-golpes-del-motor.md)
