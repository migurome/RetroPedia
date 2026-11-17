"""Convierte un PNG en una cabecera C con píxeles ARGB4444 para el PVR.
Uso: py -I png2argb.py entrada.png salida.h nombre
"""
import sys

from PIL import Image


def main():
    entrada, salida, nombre = sys.argv[1:4]
    im = Image.open(entrada).convert("RGBA")
    w, h = im.size
    # El PVR solo admite texturas con lados potencia de 2, entre 8 y 1024.
    for lado in (w, h):
        if lado < 8 or lado > 1024 or lado & (lado - 1):
            sys.exit(f"{entrada}: {w}x{h} no vale para el PVR: cada lado debe ser potencia de 2 entre 8 y 1024")
    pixeles = [((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4) for r, g, b, a in zip(*[iter(im.tobytes())] * 4)]
    with open(salida, "w", encoding="ascii") as f:
        f.write(f"#define {nombre.upper()}_W {w}\n#define {nombre.upper()}_H {h}\n")
        f.write(f"static const uint16_t {nombre}[] __attribute__((aligned(32))) = {{\n")
        for i in range(0, len(pixeles), w):
            f.write(",".join(f"0x{p:04X}" for p in pixeles[i:i + w]) + ",\n")
        f.write("};\n")


if __name__ == "__main__":
    main()
