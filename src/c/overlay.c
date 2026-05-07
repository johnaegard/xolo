#include <cx16.h>
#include "vera-util.h"
#include "overlay.h"

// Layer 0 map: 64 tiles wide x 32 tall (minimum to cover 40x30 screen at 8px).
// Map lives at 0x01800, immediately after the three 2048-byte 64x64 sprite blobs.
// Charset (standard PETSCII, loaded by KERNAL at boot) lives at 0x1F000.
#define MAP_VRAM    0x01800UL
#define TILE_VRAM   0x1F000UL
#define MAP_STRIDE  64
#define SCREEN_ROWS 30
#define PLAY_COLS   30
#define TOTAL_COLS  40

// Screen code 0x18 = uppercase 'X' in PETSCII.
// Color byte: bits 7:4 = bg palette index, bits 3:0 = fg palette index.
#define CHAR_SPACE   0x20
#define CHAR_X       0x18
#define COLOR_TRANS  0x00  // palette 0 = transparent for both fg and bg
#define COLOR_PANEL  0x61  // bg=6 (blue), fg=1 (white)
#define COLOR_DEBUG  0x51  // bg=5 (green), fg=1 (white)

// Write a label char, ':', and 4 decimal digits into the sidebar at (row, col 30).
static void write_coord(unsigned char row, unsigned char label, unsigned int val) {
  vera_set_addr(MAP_VRAM + (unsigned long)(row * MAP_STRIDE + PLAY_COLS) * 2);
  VERA.data0 = label; VERA.data0 = COLOR_DEBUG;
  VERA.data0 = 0x3A;  VERA.data0 = COLOR_DEBUG;  // ':'
  VERA.data0 = 0x30 + (unsigned char)((val / 1000) % 10); VERA.data0 = COLOR_DEBUG;
  VERA.data0 = 0x30 + (unsigned char)((val / 100)  % 10); VERA.data0 = COLOR_DEBUG;
  VERA.data0 = 0x30 + (unsigned char)((val / 10)   % 10); VERA.data0 = COLOR_DEBUG;
  VERA.data0 = 0x30 + (unsigned char)(val           % 10); VERA.data0 = COLOR_DEBUG;
}

void overlay_draw_coords(unsigned int x, unsigned int y) {
  write_coord(1, 0x18, x);  // 'X'
  write_coord(2, 0x19, y);  // 'Y'
}

void overlay_draw_collision(unsigned char coll_nibble) {
  vera_set_addr(MAP_VRAM + (unsigned long)(3 * MAP_STRIDE + PLAY_COLS) * 2);
  VERA.data0 = 0x03; VERA.data0 = COLOR_DEBUG;  // 'C'
  VERA.data0 = 0x3A; VERA.data0 = COLOR_DEBUG;  // ':'
  VERA.data0 = 0x30 + ((coll_nibble >> 3) & 1); VERA.data0 = COLOR_DEBUG;  // bit 3
  VERA.data0 = 0x30 + ((coll_nibble >> 2) & 1); VERA.data0 = COLOR_DEBUG;  // bit 2
  VERA.data0 = 0x30 + ((coll_nibble >> 1) & 1); VERA.data0 = COLOR_DEBUG;  // bit 1
  VERA.data0 = 0x30 + ((coll_nibble     ) & 1); VERA.data0 = COLOR_DEBUG;  // bit 0
}

void overlay_init(void) {
  unsigned char row, col;

  VERA.layer0.config   = LAYER_MAP_WIDTH_64 | LAYER_MAP_HEIGHT_32 |
                         LAYER_T256C_OFF | LAYER_BITMAP_OFF | LAYER_BPP_1;
  VERA.layer0.mapbase  = (unsigned char)(MAP_VRAM >> 9);
  VERA.layer0.tilebase = (unsigned char)(((TILE_VRAM >> 11) << 2) |
                         TILE_HEIGHT_8PX | TILE_WIDTH_8PX);

  vera_set_addr(MAP_VRAM);
  for (row = 0; row < SCREEN_ROWS; row++) {
    for (col = 0; col < MAP_STRIDE; col++) {
      if (col >= PLAY_COLS && col < TOTAL_COLS) {
        VERA.data0 = CHAR_SPACE;
        VERA.data0 = COLOR_PANEL;
      } else {
        VERA.data0 = CHAR_SPACE;
        VERA.data0 = COLOR_TRANS;
      }
    }
  }
}
