#include <cx16.h>
#include <stdlib.h>
#include <stdio.h>
#include "vera-util.h"
#include "maze.h"
#include "enemy.h"

// 16 rotations x 128 bytes each, starting after tank sprites.
#define ENEMY_SPRITE_VRAM  0x02900UL
#define ENEMY_SPRITE_INDEX 37

// Start 2 cells right of the tank's initial world position.
#define ENEMY_START_X  (2400 + 2 * CELL_PX)
#define ENEMY_START_Y  2400

unsigned int enemy_world_x;
unsigned int enemy_world_y;
static unsigned char enemy_rot;
static unsigned char enemy_dir;  // 0=up 1=right 2=down 3=left
static unsigned char move_tick = 0;

static const signed char dir_dx[4] = { 0,  1, 0, -1};
static const signed char dir_dy[4] = {-1,  0, 1,  0};

// Returns 1 if the enemy can step from (row,col) in direction d without crossing a wall.
static unsigned char can_move_dir(unsigned char row, unsigned char col, unsigned char d) {
  switch (d) {
    case 0: return (row > 0)             && !(maze[row][col] & 1);
    case 1: return (col + 1 < MAZE_COLS) && !(maze[row][col + 1] & 2);
    case 2: return (row + 1 < MAZE_ROWS) && !(maze[row + 1][col] & 1);
    case 3: return (col > 0)             && !(maze[row][col] & 2);
    default: return 0;
  }
}

static void update_sprite_addr(void) {
  unsigned long addr = ENEMY_SPRITE_VRAM + (unsigned long)enemy_rot * 128UL;
  vera_set_addr(0x1FC00UL + (unsigned long)ENEMY_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(addr >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((addr >> 13) & 0x0F);
}

// At a cell boundary, choose a new direction at random from those without walls,
// excluding a 180-degree reversal unless it is the only option (dead end).
static void pick_direction(void) {
  unsigned char row, col, open[4], n, d, opposite;

  row      = (unsigned char)(enemy_world_y / CELL_PX);
  col      = (unsigned char)(enemy_world_x / CELL_PX);
  opposite = (enemy_dir + 2) & 3;
  n        = 0;

  for (d = 0; d < 4; d++) {
    if (d != opposite && can_move_dir(row, col, d)) open[n++] = d;
  }
  if (n == 0) {
    if (can_move_dir(row, col, opposite)) {
      enemy_dir = opposite;
      enemy_rot = enemy_dir * 4;
      update_sprite_addr();
    }
    return;
  }
  enemy_dir = open[rand() % n];
  enemy_rot = enemy_dir * 4;
  update_sprite_addr();
}

// The enemy is always the viewport centre, so its screen position is fixed.
static void update_sprite_pos(void) {
  vera_set_addr(0x1FC00UL + (unsigned long)ENEMY_SPRITE_INDEX * 8 + 2);
  VERA.data0 = 120; VERA.data0 = 0;
  VERA.data0 = 120; VERA.data0 = 0;
}

void enemy_init(void) {
  char fname[13];
  unsigned char i;

  for (i = 0; i < 16; i++) {
    sprintf(fname, "enemya%d.bin", i);
    load_into_vera(fname, ENEMY_SPRITE_VRAM + (unsigned long)i * 128UL, NO_2_BYTE_HEADER);
  }

  enemy_world_x = ENEMY_START_X;
  enemy_world_y = ENEMY_START_Y;
  enemy_dir     = 2;  // pick_direction() in first enemy_update() will override this
  enemy_rot     = enemy_dir * 4;

  vera_set_addr(0x1FC00UL + (unsigned long)ENEMY_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(ENEMY_SPRITE_VRAM >> 5);
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((ENEMY_SPRITE_VRAM >> 13) & 0x0F);
  VERA.data0 = 0; VERA.data0 = 0;  // x (filled by update_sprite_pos)
  VERA.data0 = 0; VERA.data0 = 0;  // y
  VERA.data0 = SPRITE_BYTE6_Z_ABOVE_BACKGROUND | SPRITE_BYTE6_COLLMASK_3;
  VERA.data0 = SPRITE_BYTE7_HEIGHT_16 | SPRITE_BYTE7_WIDTH_16;

  update_sprite_pos();
}

void enemy_update(void) {
  move_tick ^= 1;
  if (move_tick) {
    if (enemy_world_x % CELL_PX == 0 && enemy_world_y % CELL_PX == 0) {
      pick_direction();
    }
    enemy_world_x = (unsigned int)((int)enemy_world_x + (int)dir_dx[enemy_dir]);
    enemy_world_y = (unsigned int)((int)enemy_world_y + (int)dir_dy[enemy_dir]);
  }
  update_sprite_pos();
}
