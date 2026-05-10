#!/usr/bin/env python3
"""Parse the VERA sprite attribute table from a VRAM dump and print a table.

Usage: python3 tools/dumpsprites.py <dump.bin>

The sprite attribute table lives at VRAM address 0x1FC00 (128 entries x 8 bytes).
Each 8-byte entry:
  Byte 0   : VRAM addr bits [12:5]  (addr >> 5, low 8 bits)
  Byte 1   : mode bit 7 + addr bits [15:13] in bits [3:0]
  Byte 2-3 : X position, 10-bit (byte3[1:0] << 8 | byte2); >= 512 means negative
  Byte 4-5 : Y position, same encoding
  Byte 6   : coll[7:4] | z[3:2] | vflip[1] | hflip[0]
  Byte 7   : height[7:6] | width[5:4] | palette_offset[3:0]
"""

import sys

SPRITE_TABLE_OFFSET = 0x1FC00
NUM_SPRITES = 128
SIZE_MAP = {0: 8, 1: 16, 2: 32, 3: 64}
Z_NAMES = {0: "off", 1: "bg", 2: ">L0", 3: ">L1"}


def parse_sprites(data):
    rows = []
    for i in range(NUM_SPRITES):
        off = SPRITE_TABLE_OFFSET + i * 8
        if off + 8 > len(data):
            break
        b = data[off:off + 8]
        if b == bytes(8):
            continue

        addr = ((b[1] & 0x0F) << 13) | (b[0] << 5)
        x = ((b[3] & 0x03) << 8) | b[2]
        y = ((b[5] & 0x03) << 8) | b[4]
        if x >= 512:
            x -= 1024
        if y >= 512:
            y -= 1024
        z = (b[6] >> 2) & 0x03
        coll = (b[6] >> 4) & 0x0F
        width = SIZE_MAP[(b[7] >> 4) & 0x03]
        height = SIZE_MAP[(b[7] >> 6) & 0x03]
        rows.append((i, addr, x, y, coll, height, width, z))
    return rows


def print_table(rows):
    if not rows:
        print("No non-zero sprite slots found.")
        return
    header = f"{'Idx':>4}  {'Addr':>8}  {'X':>5}  {'Y':>5}  {'Coll':>4}  {'H':>4}  {'W':>4}  {'Z':>4}"
    print(header)
    print("-" * len(header))
    for idx, addr, x, y, coll, h, w, z in rows:
        print(f"{idx:>4}  0x{addr:05X}  {x:>5}  {y:>5}  {coll:04b}  {h:>4}  {w:>4}  {Z_NAMES[z]:>4}")
    print(f"\n{len(rows)} non-zero sprite slot(s)")


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "distrib/dump.bin"
    with open(path, "rb") as f:
        data = f.read()
    if len(data) <= SPRITE_TABLE_OFFSET:
        print(f"File too small: {len(data)} bytes, need > 0x{SPRITE_TABLE_OFFSET:X}")
        sys.exit(1)
    rows = parse_sprites(data)
    print_table(rows)


if __name__ == "__main__":
    main()
