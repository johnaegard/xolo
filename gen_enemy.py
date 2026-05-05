#!/usr/bin/env python3
"""Generate 16 rotations of the 16x16 4bpp enemy sprite."""
import math, os

# 4bpp palette indices (X16 default palette)
T = 0  # transparent
O = 8  # orange
Y = 7  # yellow

# Base sprite: 16x16 grid, pattern centered from prompt-009.
# 9-row body (tip at top), centered: rows 3-11, cols 5-9.
BASE = [[T] * 16 for _ in range(16)]

def set_px(row, col, color):
    BASE[row][col] = color

# Row 3: tip
set_px(3, 7, O)
# Row 4: OOO
for c in (6, 7, 8):
    set_px(4, c, O)
# Row 5: OOYOO
for c, col in zip("OOYOO", range(5, 10)):
    set_px(5, col, O if c == "O" else Y)
# Row 6: OOYOO
for c, col in zip("OOYOO", range(5, 10)):
    set_px(6, col, O if c == "O" else Y)
# Row 7: OYYYO
for c, col in zip("OYYYO", range(5, 10)):
    set_px(7, col, O if c == "O" else Y)
# Row 8: OOYOO
for c, col in zip("OOYOO", range(5, 10)):
    set_px(8, col, O if c == "O" else Y)
# Row 9: OOOOO
for col in range(5, 10):
    set_px(9, col, O)
# Row 10: O O O
for col in (5, 7, 9):
    set_px(10, col, O)
# Row 11: O   O
for col in (5, 9):
    set_px(11, col, O)


def rotate_frame(angle_deg):
    """Return a new 16x16 grid rotated clockwise by angle_deg around center."""
    cx, cy = 7.5, 7.5
    rad = math.radians(-angle_deg)  # negative = clockwise
    cos_a, sin_a = math.cos(rad), math.sin(rad)
    out = [[T] * 16 for _ in range(16)]
    for row in range(16):
        for col in range(16):
            # Map output pixel back to source
            dx, dy = col - cx, row - cy
            sx = cos_a * dx - sin_a * dy + cx
            sy = sin_a * dx + cos_a * dy + cy
            sc = int(round(sx))
            sr = int(round(sy))
            if 0 <= sr < 16 and 0 <= sc < 16:
                out[row][col] = BASE[sr][sc]
    return out


def make_bin(grid):
    """Convert 16x16 grid to 128-byte 4bpp binary (2 pixels per byte, hi=left)."""
    data = []
    for row in grid:
        for i in range(0, 16, 2):
            data.append((row[i] << 4) | row[i + 1])
    return bytes(data)


os.makedirs("assets", exist_ok=True)
for i in range(16):
    grid = rotate_frame(i * 22.5)
    path = f"assets/enemya{i}.bin"
    with open(path, "wb") as f:
        f.write(make_bin(grid))
    print(f"wrote {path}")
