#include <cx16.h>
#include "vera-util.h"

// Palette indices used in this demo.
#define COLOR_BORDER     3   // cyan
#define COLOR_BACKGROUND 12  // medium gray (used as layer background when layers are enabled)

int main(void) {
  // Disable sprites and both layers; enable VGA output.
  VERA.display.hscale = DC_HSCALE_320;
  VERA.display.vscale = DC_VSCALE_240;
  VERA.display.video  = SPRITES_DISABLED | LAYER0_DISABLED | LAYER1_DISABLED | VGA_ENABLED;

  // DC_BORDER is the palette index shown in the border area (NTSC) and as the
  // background whenever no sprite or layer paints a pixel.
  VERA.display.border = 0x48;

  // COLOR_BACKGROUND (12, medium gray) will be used as the layer background
  // color when a layer is configured.

  while (1) {}

  return 0;
}
