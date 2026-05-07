#include <cx16.h>
#include <stdio.h>
#include "vera-util.h"
#include "explosion.h"

// 16 frames x 128 bytes each, after enemy sprites (0x02900 + 16*128 = 0x03100).
#define EXPLOSION_SPRITE_VRAM  0x03100UL
// Maze 0-35, tank 36, enemy 37.
#define EXPLOSION_SPRITE_INDEX 38

static unsigned char expl_frame = 0;
static unsigned char frame_count = 0;
static unsigned char expl_done = 1;  // inactive until triggered

static void write_sprite_addr(void) {
  unsigned long addr = EXPLOSION_SPRITE_VRAM + (unsigned long)expl_frame * 128UL;
  vera_set_addr(0x1FC00UL + (unsigned long)EXPLOSION_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(addr >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((addr >> 13) & 0x0F);
}

void explosion_init(void) {
  char fname[16];
  unsigned char i;

  for (i = 0; i < 16; i++) {
    sprintf(fname, "explosion%d.bin", i);
    load_into_vera(fname, EXPLOSION_SPRITE_VRAM + (unsigned long)i * 128UL, NO_2_BYTE_HEADER);
  }

  // Initialise sprite as hidden (Z disabled).
  vera_set_addr(0x1FC00UL + (unsigned long)EXPLOSION_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(EXPLOSION_SPRITE_VRAM >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((EXPLOSION_SPRITE_VRAM >> 13) & 0x0F);
  VERA.data0 = 0; VERA.data0 = 0;  // x
  VERA.data0 = 0; VERA.data0 = 0;  // y
  VERA.data0 = SPRITE_BYTE6_Z_DISABLED;
  VERA.data0 = SPRITE_BYTE7_HEIGHT_16 | SPRITE_BYTE7_WIDTH_16;
}

void explosion_trigger(unsigned int x, unsigned int y) {
  expl_frame  = 0;
  frame_count = 0;
  expl_done   = 0;

  vera_set_addr(0x1FC00UL + (unsigned long)EXPLOSION_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(EXPLOSION_SPRITE_VRAM >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((EXPLOSION_SPRITE_VRAM >> 13) & 0x0F);
  VERA.data0 = (unsigned char)(x & 0xFF);
  VERA.data0 = (unsigned char)(x >> 8);
  VERA.data0 = (unsigned char)(y & 0xFF);
  VERA.data0 = (unsigned char)(y >> 8);
  VERA.data0 = SPRITE_BYTE6_Z_ABOVE_BACKGROUND;
  VERA.data0 = SPRITE_BYTE7_HEIGHT_16 | SPRITE_BYTE7_WIDTH_16;
}

unsigned char explosion_active(void) {
  return !expl_done;
}

void explosion_update(void) {
  if (expl_done) return;

  if (++frame_count >= 2) {
    frame_count = 0;
    expl_frame++;
    if (expl_frame >= 16) {
      expl_done = 1;
      // Hide sprite.
      vera_set_addr(0x1FC00UL + (unsigned long)EXPLOSION_SPRITE_INDEX * 8 + 6);
      VERA.data0 = SPRITE_BYTE6_Z_DISABLED;
      return;
    }
    write_sprite_addr();
  }
}
