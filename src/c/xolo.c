#include <cx16.h>
#include <stdlib.h>
#include <joystick.h>
#include <6502.h>
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

static unsigned char irqHandler(void) {
  if (VERA.irq_flags & 0x04) {
    VERA.irq_flags = 0x04;  // clear SPRCOL
    return IRQ_HANDLED;
  }
  return IRQ_NOT_HANDLED;
}

int main(void) {
  unsigned int px, py;
  unsigned char joy;
  unsigned char game_over = 0;

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

  while (1) {
    wait();
    joy = joy_read(0);

    // ISR bits 7:4 are set by VERA when sprites sharing a COLLMASK bit overlap.
    // Tank and walls both carry COLLMASK_0, so ISR bit 4 = tank hit a wall.
    // Enemy has no mask → never fires this bit.
    {
      unsigned char coll = VERA.irq_flags & 0xF0;
      if (!game_over && (coll & 0x10)) {
        overlay_draw_collision(coll >> 4);
        game_over = 1;
        tank_destroy();
        explosion_trigger(116, 116);
      }
      VERA.irq_flags = 0xF0;  // clear collision-group bits for next frame
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
  }

  return 0;
}
