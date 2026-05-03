#include <cx16.h>
#include <stdlib.h>
#include <joystick.h>
#include "vera-util.h"
#include "maze.h"
#include "overlay.h"
#include "wait.h"

#define MAX_PX ((MAZE_COLS - VIEW_COLS) * CELL_PX)
#define MAX_PY ((MAZE_ROWS - VIEW_ROWS) * CELL_PX)

int main(void) {
  unsigned int px = MAZE_INIT_PX;
  unsigned int py = MAZE_INIT_PY;
  unsigned char joy, redraw;

  VERA.display.hscale = DC_HSCALE_320;
  VERA.display.vscale = DC_VSCALE_240;
  VERA.display.video = SPRITES_ENABLED | LAYER0_ENABLED | LAYER1_DISABLED | VGA_ENABLED;

  srand(42);
  maze_randomize();
  maze_init_sprites();
  overlay_init();
  maze_draw_px(px, py);

  joy_install(cx16_std_joy);

  while (1) {
    wait();
    joy = joy_read(0);
    redraw = 0;

    if ((joy & JOY_UP_MASK)    && py > 0)      { py--; redraw = 1; }
    if ((joy & JOY_DOWN_MASK)  && py < MAX_PY)  { py++; redraw = 1; }
    if ((joy & JOY_LEFT_MASK)  && px > 0)       { px--; redraw = 1; }
    if ((joy & JOY_RIGHT_MASK) && px < MAX_PX)  { px++; redraw = 1; }

    if (redraw) maze_draw_px(px, py);
  }

  return 0;
}
