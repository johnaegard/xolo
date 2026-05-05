#include <cx16.h>
#include <stdio.h>
#include "vera-util.h"
#include "enemy.h"

// 16 rotations x 128 bytes each, starting after tank sprites (0x02800 + 0x100).
#define ENEMY_SPRITE_VRAM  0x02900UL
// Maze uses slots 0-35, tank uses 36.
#define ENEMY_SPRITE_INDEX 37
#define ENEMY_X            150
#define ENEMY_Y            50

static unsigned char enemy_rot   = 0;
static unsigned char frame_count = 0;

static void enemy_update_addr(void) {
  unsigned long addr = ENEMY_SPRITE_VRAM + (unsigned long)enemy_rot * 128UL;
  vera_set_addr(0x1FC00UL + (unsigned long)ENEMY_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(addr >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((addr >> 13) & 0x0F);
}

void enemy_init(void) {
  char fname[13];
  unsigned char i;

  for (i = 0; i < 16; i++) {
    sprintf(fname, "enemya%d.bin", i);
    load_into_vera(fname, ENEMY_SPRITE_VRAM + (unsigned long)i * 128UL, NO_2_BYTE_HEADER);
  }

  enemy_rot   = 0;
  frame_count = 0;

  vera_set_addr(0x1FC00UL + (unsigned long)ENEMY_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(ENEMY_SPRITE_VRAM >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((ENEMY_SPRITE_VRAM >> 13) & 0x0F);
  VERA.data0 = (unsigned char)(ENEMY_X & 0xFF);
  VERA.data0 = (unsigned char)(ENEMY_X >> 8);
  VERA.data0 = (unsigned char)(ENEMY_Y & 0xFF);
  VERA.data0 = (unsigned char)(ENEMY_Y >> 8);
  VERA.data0 = SPRITE_BYTE6_Z_ABOVE_BACKGROUND;
  VERA.data0 = SPRITE_BYTE7_HEIGHT_16 | SPRITE_BYTE7_WIDTH_16;
}

void enemy_update(void) {
  if (++frame_count >= 5) {
    frame_count = 0;
    enemy_rot = (enemy_rot + 1) & 15;
    enemy_update_addr();
  }
}
