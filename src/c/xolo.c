#include <cx16.h>
#include <stdlib.h>
#include <joystick.h>
#include <6502.h>
#include <stdbool.h>
#include <stdio.h>
#include <conio.h>
#include "vera-util.h"
#include "maze.h"
#include "overlay.h"
#include "tank.h"
#include "enemy.h"
#include "explosion.h"
#include "wait.h"

#define MAX_PX ((MAZE_COLS - VIEW_COLS) * CELL_PX)
#define MAX_PY ((MAZE_ROWS - VIEW_ROWS) * CELL_PX)

// IRQ handler for sprite collision. Clears the SPRCOL bit (bit 2) in ISR so
// the interrupt stops firing; leaves the 4 collision-group bits (7:4) intact
// for the main loop to read. Returning IRQ_HANDLED suppresses further handling.
#define IRQ_STACK_SIZE 8
static unsigned char irqStack[IRQ_STACK_SIZE];
static bool collision_occurred = false;
unsigned char game_over = 0;
unsigned int px, py;
bool vsync=false;

static unsigned char irqHandler(void) {
  if (VERA.irq_flags & 0x01) {
    vsync = true;
    return IRQ_NOT_HANDLED;
  }
  else if (VERA.irq_flags & 0x04) {
    VERA.irq_flags = 0x04;  // clear SPRCOL
    collision_occurred = true;
    return IRQ_HANDLED;
  }
  return IRQ_NOT_HANDLED;
}

int main(void) {
  unsigned char joy;

  VERA.display.hscale = DC_HSCALE_320;
  VERA.display.vscale = DC_VSCALE_240;
  VERA.display.video  = SPRITES_ENABLED | LAYER0_ENABLED | LAYER1_DISABLED | VGA_ENABLED;
  VERA.display.border = 0xFF;  // cyan (default palette index 3)

  // Enable sprite collision interrupt and register the handler.
  VERA.irq_enable |= 0x04;
  set_irq(&irqHandler, irqStack, IRQ_STACK_SIZE);

  srand(42);
  maze_randomize();
  maze_init_sprites();
  overlay_init();
  tank_init();
  enemy_init();
  explosion_init();

  px = tank_world_x - 120;
  py = tank_world_y - 120;
  maze_draw_px(px, py);
  overlay_draw_coords(tank_world_x, tank_world_y);

  joy_install(cx16_std_joy);
  clrscr();

  while (1) {
    if (vsync) {

      joy = joy_read(0);

      {
        if (collision_occurred) {
          unsigned char coll = VERA.irq_flags & 0xF0;
          overlay_draw_collision(coll >> 4, COLOR_COLLISION_HIT);
          if ((coll & 0b10000000) || (coll & 0b01000000)) {
            unsigned char sub_x   = (unsigned char)((tank_world_x + 4) % CELL_PX);
            unsigned char sub_y   = (unsigned char)((tank_world_y + 4) % CELL_PX);
            unsigned char cell_col = (unsigned char)((tank_world_x +4) / CELL_PX);
            unsigned char cell_row = (unsigned char)((tank_world_y +4) / CELL_PX);
            unsigned char near_wall = 0;
            // left edge of current cell → check current cell's left wall
            if (sub_x <= 3 && (maze[cell_row][cell_col] & 2)) near_wall = 1;
            // right edge of current cell → check left wall of next column
            if (sub_x >= CELL_PX - 3 && cell_col + 1 < MAZE_COLS
                && (maze[cell_row][cell_col + 1] & 2)) near_wall = 1;
            // top edge of current cell → check current cell's top wall
            if (sub_y <= 4 && (maze[cell_row][cell_col] & 1)) near_wall = 1;
            // bottom edge of current cell → check top wall of next row
            if (sub_y >= CELL_PX - 3 && cell_row + 1 < MAZE_ROWS
                && (maze[cell_row + 1][cell_col] & 1)) near_wall = 1;
            if (near_wall) {
              game_over = 1;
              tank_destroy();
              explosion_trigger(116, 116);
            }
          }
          collision_occurred = false;
        } else {
          overlay_draw_collision(0, COLOR_COLLISION_IDLE);
        }
        VERA.irq_flags &= 0x0F;  // clear collision-group bits for next frame
      }

      enemy_update();
      explosion_update();
      if (!game_over) {
        if (joy & JOY_LEFT_MASK)  tank_rotate_ccw();
        if (joy & JOY_RIGHT_MASK) tank_rotate_cw();
        if (joy & JOY_UP_MASK) {
          tank_move_forward();
          px = tank_world_x - 120;
          py = tank_world_y - 120;
          if (px > MAX_PX) px = MAX_PX;
          if (py > MAX_PY) py = MAX_PY;
          maze_draw_px(px, py);
          overlay_draw_coords(tank_world_x, tank_world_y);
        }
      }
      vsync = false;
    }
  }
  return 0;
}
