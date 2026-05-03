#include <cx16.h>
#include "vera-util.h"
#include "tank.h"

// Tank pixel data lives right after the overlay map (overlay: 0x01800–0x027FF).
#define TANK_SPRITE_VRAM  0x02800UL
// Maze uses (VIEW_ROWS+1)*(VIEW_COLS+1) = 6*6 = 36 sprite slots (0-35).
#define TANK_SPRITE_INDEX 36
#define TANK_X            120
#define TANK_Y            120

void tank_init(void) {
  load_into_vera("tank0.bin", TANK_SPRITE_VRAM, NO_2_BYTE_HEADER);

  vera_set_addr(0x1FC00UL + (unsigned long)TANK_SPRITE_INDEX * 8);
  VERA.data0 = (unsigned char)(TANK_SPRITE_VRAM >> 5);                          // ADDR[12:5]
  VERA.data0 = SPRITE_BYTE1_4BPP | (unsigned char)((TANK_SPRITE_VRAM >> 13) & 0x0F);  // 4bpp + ADDR[16:13]
  VERA.data0 = (unsigned char)(TANK_X & 0xFF);                // X low
  VERA.data0 = (unsigned char)(TANK_X >> 8);                  // X high
  VERA.data0 = (unsigned char)(TANK_Y & 0xFF);                // Y low
  VERA.data0 = (unsigned char)(TANK_Y >> 8);                  // Y high
  VERA.data0 = SPRITE_BYTE6_Z_ABOVE_BACKGROUND;
  VERA.data0 = SPRITE_BYTE7_HEIGHT_8 | SPRITE_BYTE7_WIDTH_8;
}
