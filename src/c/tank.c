#include <cx16.h>
#include "vera-util.h"
#include "tank.h"

// 8 rotations x 32 bytes each = 256 bytes, from 0x02800 to 0x028FF.
#define TANK_SPRITE_VRAM  0x02800UL
// Maze uses (VIEW_ROWS+1)*(VIEW_COLS+1) = 6*6 = 36 sprite slots (0-35).
#define TANK_SPRITE_INDEX 36
#define TANK_X            120
#define TANK_Y            120

// World position: viewport = (tank_world_x - 120, tank_world_y - 120).
// Initialised to maze centre so the viewport starts at the same position
// as the old MAZE_INIT_PX/PY (2280 = 2400 - 120).
unsigned int tank_world_x = 2400;
unsigned int tank_world_y = 2400;

static unsigned char tank_angle = 0;

// dx/dy per sprite frame (tank_angle >> 5), clockwise from up.
static const signed char move_dx[8] = { 0, 1, 1, 1, 0,-1,-1,-1};
static const signed char move_dy[8] = {-1,-1, 0, 1, 1, 1, 0,-1};

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
  VERA.data0 = SPRITE_BYTE6_Z_ABOVE_L2 | SPRITE_BYTE6_COLLMASK_1;
  VERA.data0 = SPRITE_BYTE7_HEIGHT_8 | SPRITE_BYTE7_WIDTH_8;
}

void tank_move_forward(void) {
  unsigned char frame = tank_angle >> 5;
  tank_world_x = (unsigned int)((int)tank_world_x + (int)move_dx[frame]);
  tank_world_y = (unsigned int)((int)tank_world_y + (int)move_dy[frame]);
}

void tank_rotate_cw(void) {
  tank_angle++;
  tank_update_addr();
}

void tank_rotate_ccw(void) {
  tank_angle--;
  tank_update_addr();
}

void tank_destroy(void) {
  vera_set_addr(0x1FC00UL + (unsigned long)TANK_SPRITE_INDEX * 8 + 6);
  VERA.data0 = SPRITE_BYTE6_Z_DISABLED;
}
