#include <cx16.h>
#include <stdlib.h>
#include "vera-util.h"
#include "maze.h"

unsigned char maze[MAZE_ROWS][MAZE_COLS];

// Sprite pixel data in VRAM (32x32, 4bpp = 512 bytes each):
//   Shape 1 (top wall)  : 0x00000  -> addr byte0 = (0x00000 >> 5) = 0x00
//   Shape 2 (left wall) : 0x00200  -> addr byte0 = (0x00200 >> 5) = 0x10
//   Shape 3 (top+left)  : 0x00400  -> addr byte0 = (0x00400 >> 5) = 0x20
static const unsigned char shape_addr_byte0[4] = {0x00, 0x00, 0x10, 0x20};

// Assign maze cells randomly: 10% each for values 1, 2, 3; 70% value 0.
void maze_randomize(void) {
  unsigned char r, c, rv;
  for (r = 0; r < MAZE_ROWS; r++) {
    for (c = 0; c < MAZE_COLS; c++) {
      rv = (unsigned char)(rand() % 10);
      maze[r][c] = (rv < 3) ? rv + 1 : 0;
    }
  }
}

// Write 32x32 4bpp pixel data for all three wall shapes.
// Color 6 (0x6) = blue; color 0 = transparent.
// Clears all 128 sprite attribute entries.
void maze_init_sprites(void) {
  unsigned char row, col;
  unsigned int i;

  // Shape 1: top row blue, remaining rows transparent (32 rows x 16 bytes)
  vera_set_addr(0x00000UL);
  for (col = 0; col < 16; col++) VERA.data0 = 0x66;
  for (row = 1; row < 32; row++)
    for (col = 0; col < 16; col++) VERA.data0 = 0x00;

  // Shape 2: leftmost pixel blue per row (high nibble=6, low=0), rest transparent
  vera_set_addr(0x00200UL);
  for (row = 0; row < 32; row++) {
    VERA.data0 = 0x60;
    for (col = 1; col < 16; col++) VERA.data0 = 0x00;
  }

  // Shape 3: top row blue, left pixel blue for remaining rows
  vera_set_addr(0x00400UL);
  for (col = 0; col < 16; col++) VERA.data0 = 0x66;
  for (row = 1; row < 32; row++) {
    VERA.data0 = 0x60;
    for (col = 1; col < 16; col++) VERA.data0 = 0x00;
  }

  // Clear all 128 sprite attribute entries
  vera_set_addr(0x1FC00UL);
  for (i = 0; i < (unsigned int)128 * 8; i++) VERA.data0 = 0x00;
}

// Encode a signed screen coordinate as a 10-bit VERA position.
// Negative positions encode as (1024 + p) so VERA clips them at the edge.
static unsigned int encode_pos(int p) {
  return (p < 0) ? (unsigned int)(1024 + p) : (unsigned int)p;
}

// Draw the viewport anchored at pixel offset (px, py) within the map.
// Renders (VIEW_COLS+1) x (VIEW_ROWS+1) sprites in a single sequential VERA write.
void maze_draw_px(unsigned int px, unsigned int py) {
  unsigned char cell_col = (unsigned char)(px / CELL_PX);
  unsigned char cell_row = (unsigned char)(py / CELL_PX);
  unsigned char sub_x    = (unsigned char)(px % CELL_PX);
  unsigned char sub_y    = (unsigned char)(py % CELL_PX);
  unsigned char sc, sr, mr, mc, val;
  unsigned int enc_sx, enc_sy;

  vera_set_addr(0x1FC00UL);

  for (sr = 0; sr <= VIEW_ROWS; sr++) {
    for (sc = 0; sc <= VIEW_COLS; sc++) {
      mr  = cell_row + sr;
      mc  = cell_col + sc;
      val = (mr < MAZE_ROWS && mc < MAZE_COLS) ? maze[mr][mc] : 0;

      enc_sx = encode_pos((int)sc * CELL_PX - (int)sub_x);
      enc_sy = encode_pos((int)sr * CELL_PX - (int)sub_y);

      if (val == 0) {
        VERA.data0 = 0x00;
        VERA.data0 = 0x00;
        VERA.data0 = (unsigned char)(enc_sx & 0xFF);
        VERA.data0 = (unsigned char)(enc_sx >> 8);
        VERA.data0 = (unsigned char)(enc_sy & 0xFF);
        VERA.data0 = (unsigned char)(enc_sy >> 8);
        VERA.data0 = SPRITE_BYTE6_Z_DISABLED;
        VERA.data0 = 0x00;
      } else {
        VERA.data0 = shape_addr_byte0[val];
        VERA.data0 = SPRITE_BYTE1_4BPP;
        VERA.data0 = (unsigned char)(enc_sx & 0xFF);
        VERA.data0 = (unsigned char)(enc_sx >> 8);
        VERA.data0 = (unsigned char)(enc_sy & 0xFF);
        VERA.data0 = (unsigned char)(enc_sy >> 8);
        VERA.data0 = SPRITE_BYTE6_Z_ABOVE_BACKGROUND;
        VERA.data0 = SPRITE_BYTE7_HEIGHT_32 | SPRITE_BYTE7_WIDTH_32;
      }
    }
  }
}
