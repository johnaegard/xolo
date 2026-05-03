#include <cx16.h>
#include "vera-util.h"
#include "tank.h"

// 8 rotations x 32 bytes each = 256 bytes, from 0x02800 to 0x028FF.
#define TANK_SPRITE_VRAM  0x02800UL
// Maze uses (VIEW_ROWS+1)*(VIEW_COLS+1) = 6*6 = 36 sprite slots (0-35).
#define TANK_SPRITE_INDEX 36
#define TANK_X            120
#define TANK_Y            120

static unsigned char tank_angle = 0;

// Update sprite attribute bytes 0-1 (VRAM address) for the current rotation.
// Bytes 2-7 (position, z, size) are set once in tank_init and don't change.
static void tank_update_addr(void) {
  unsigned long addr = TANK_SPRITE_VRAM + (unsigned long)(tank_angle >> 5) * 32UL;
  vera_set_addr(0x1FC00UL + (unsigned long)TANK_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(addr >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((addr >> 13) & 0x0F);
}

void tank_init(void) {
  char fname[] = "tank0.bin";
  unsigned char i;

  for (i = 0; i < 8; i++) {
    fname[4] = '0' + i;
    load_into_vera(fname, TANK_SPRITE_VRAM + (unsigned long)i * 32UL, NO_2_BYTE_HEADER);
  }

  tank_angle = 0;
  vera_set_addr(0x1FC00UL + (unsigned long)TANK_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(TANK_SPRITE_VRAM >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((TANK_SPRITE_VRAM >> 13) & 0x0F);
  VERA.data0 = (unsigned char)(TANK_X & 0xFF);
  VERA.data0 = (unsigned char)(TANK_X >> 8);
  VERA.data0 = (unsigned char)(TANK_Y & 0xFF);
  VERA.data0 = (unsigned char)(TANK_Y >> 8);
  VERA.data0 = SPRITE_BYTE6_Z_ABOVE_BACKGROUND;
  VERA.data0 = SPRITE_BYTE7_HEIGHT_8 | SPRITE_BYTE7_WIDTH_8;
}

void tank_rotate_cw(void) {
  tank_angle++;
  tank_update_addr();
}

void tank_rotate_ccw(void) {
  tank_angle--;
  tank_update_addr();
}
