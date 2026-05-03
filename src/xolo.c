#include <cx16.h>
#include <stdlib.h>
#include <joystick.h>
#include "vera-util.h"
#include "maze.h"
#include "overlay.h"
#include "tank.h"
#include "wait.h"

#define MAX_PX ((MAZE_COLS - VIEW_COLS) * CELL_PX)
#define MAX_PY ((MAZE_ROWS - VIEW_ROWS) * CELL_PX)

int main(void) {
  unsigned int px, py;
  unsigned char joy;

  VERA.display.hscale = DC_HSCALE_320;
  VERA.display.vscale = DC_VSCALE_240;
  VERA.display.video = SPRITES_ENABLED | LAYER0_ENABLED | LAYER1_DISABLED | VGA_ENABLED;

  srand(42);
  maze_randomize();
  maze_init_sprites();
  overlay_init();
  tank_init();

  px = tank_world_x - 120;
  py = tank_world_y - 120;
  maze_draw_px(px, py);
  overlay_draw_coords(tank_world_x, tank_world_y);

  joy_install(cx16_std_joy);

  while (1) {
    wait();
    joy = joy_read(0);

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

  return 0;
}
