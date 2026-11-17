"""Genera IP.BIN a partir de la plantilla de KOS (sustituye a makeip.exe, que no arranca
en DreamSDK R4). Mismos campos y CRC que utils/makeip/src/field.c y crc.c.

Uso:
    py -I make_ip.py <ip.tmpl> <IP.BIN> [título] [versión, p. ej. V0.001]
"""
import datetime
import sys

FIELDS = {  # nombre: (offset, longitud)
    'device': (0x20, 0x10), 'area': (0x30, 0x8), 'periph': (0x38, 0x8), 'product': (0x40, 0xA),
    'version': (0x4A, 0x6), 'date': (0x50, 0x10), 'boot': (0x60, 0x10), 'maker': (0x70, 0x10),
    'title': (0x80, 0x80),
}


def crc(buf):
    n = 0xFFFF
    for b in buf:
        n ^= b << 8
        for _ in range(8):
            n = ((n << 1) ^ 4129) if n & 0x8000 else (n << 1)
    return n & 0xFFFF


def main():
    tmpl, out = sys.argv[1], sys.argv[2]
    title = sys.argv[3] if len(sys.argv) > 3 else 'SNIPPET'
    version = sys.argv[4] if len(sys.argv) > 4 else 'V1.000'
    ip = bytearray(open(tmpl, 'rb').read())
    values = {
        'device': '0000 CD-ROM1/1', 'area': 'JUE', 'periph': 'E000F10', 'product': 'T-99901',
        'version': version, 'date': datetime.date.today().strftime('%Y%m%d'), 'boot': '1ST_READ.BIN',
        'maker': 'RETROPEDIA', 'title': title.upper(),
    }
    for k, (off, ln) in FIELDS.items():
        ip[off:off + ln] = values[k].encode('ascii')[:ln].ljust(ln, b' ')
    ip[0x20:0x24] = f'{crc(ip[0x40:0x50]):04X}'.encode()
    open(out, 'wb').write(ip)


if __name__ == '__main__':
    main()
