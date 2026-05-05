# VERA Memory Map

## VRAM Layout

| Range               | Size    | Contents                                                        |
|---------------------|---------|-----------------------------------------------------------------|
| 0x00000 – 0x007FF   | 2048 B  | Maze wall sprite: shape 1 (top wall), 64×64 4bpp               |
| 0x00800 – 0x00FFF   | 2048 B  | Maze wall sprite: shape 2 (left wall), 64×64 4bpp              |
| 0x01000 – 0x017FF   | 2048 B  | Maze wall sprite: shape 3 (top+left wall), 64×64 4bpp          |
| 0x01800 – 0x027FF   | 4096 B  | Overlay tilemap: 64×32 tiles × 2 bytes (layer 0 map)           |
| 0x02800 – 0x028FF   | 256 B   | Tank sprites: 8 rotations × 32 bytes (8×8 4bpp)               |
| 0x02900 – 0x030FF   | 2048 B  | Enemy sprites: 16 rotations × 128 bytes (16×16 4bpp)          |
| 0x03100 – 0x038FF   | 2048 B  | Explosion sprites: 16 frames × 128 bytes (16×16 4bpp)         |
| 0x1F000 – 0x1F7FF   | 2048 B  | PETSCII charset (loaded by KERNAL at boot, used by layer 0)    |
| 0x1FC00 – 0x1FFFF   | 1024 B  | Sprite attribute table: 128 entries × 8 bytes                  |

**Total used:** 17,664 B (17 KB) — **Remaining:** 113,408 B (110 KB) of 128 KB

## Sprite Attribute Table (0x1FC00)

Each sprite occupies 8 bytes at `0x1FC00 + index * 8`.

| Index | Sprite      |
|-------|-------------|
| 0–35  | Maze tiles (6×6 viewport grid) |
| 36    | Tank        |
| 37    | Enemy       |
| 38    | Explosion   |

## Layer 0

Tilemap at 0x01800, charset at 0x1F000. Used for the overlay sidebar (10-column panel at columns 30–39).
