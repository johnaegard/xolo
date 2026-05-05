#!/usr/bin/env python3
"""Generate 16 frames of a 16x16 4bpp explosion sprite."""
import math, os

# 4bpp palette indices (X16 default palette)
T = 0  # transparent
W = 1  # white
Y = 7  # yellow
O = 8  # orange
R = 2  # red

CX, CY = 7.5, 7.5


def dist(x, y):
    return math.sqrt((x - CX) ** 2 + (y - CY) ** 2)


# Outer radius and inner dead-zone radius per frame.
# Frames 0-7: fireball grows. Frames 8-15: inner zone expands, ring fades.
MAX_R  = [1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5,
          5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 7.5, 7.5]
INNER_R = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
           1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 6.5, 7.0]


def pixel(frame, x, y):
    d = dist(x, y)
    r_max = MAX_R[frame]
    r_in  = INNER_R[frame]

    if d > r_max or d < r_in:
        return T

    span = r_max - r_in
    if span < 0.01:
        return T
    ratio = (d - r_in) / span  # 0 = inner edge, 1 = outer edge

    if frame <= 1:
        # Pure white spark
        return W
    elif frame <= 3:
        # White core -> yellow
        return W if ratio < 0.4 else Y
    elif frame <= 5:
        # Yellow core -> orange
        return Y if ratio < 0.4 else O
    elif frame <= 7:
        # Orange core -> red rim
        if ratio < 0.3:
            return Y
        return O if ratio < 0.7 else R
    elif frame <= 10:
        # Ring: orange inside, red outside
        return O if ratio < 0.5 else R
    elif frame <= 12:
        # Mostly red ring
        return R
    elif frame == 13:
        # Thin red ring, slight checkerboard thinning
        return T if (int(x) + int(y)) % 3 == 0 else R
    elif frame == 14:
        # Sparser embers
        return T if (int(x) + int(y)) % 2 == 0 else R
    else:
        # frame 15: nearly gone
        return T if (int(x) * int(y)) % 3 != 0 else R


def make_frame(frame):
    data = []
    for row in range(16):
        for col in range(0, 16, 2):
            hi = pixel(frame, col, row)
            lo = pixel(frame, col + 1, row)
            data.append((hi << 4) | lo)
    return bytes(data)


os.makedirs("assets", exist_ok=True)
for i in range(16):
    path = f"assets/explosion{i}.bin"
    with open(path, "wb") as f:
        f.write(make_frame(i))
    print(f"wrote {path}")
