#ifndef OVERLAY_H
#define OVERLAY_H

#define COLOR_COLLISION_HIT  0x21  // bg=red(2), fg=white(1)
#define COLOR_COLLISION_IDLE 0x51  // bg=green(5), fg=white(1)

void overlay_init(void);
void overlay_draw_coords(unsigned int x, unsigned int y);
// Display the 4 VERA sprite collision bits (irq_flags bits 7:4, passed as bits 3:0).
// color: COLOR_COLLISION_HIT (red bg) or COLOR_COLLISION_IDLE (green bg).
void overlay_draw_collision(unsigned char coll_nibble, unsigned char color);

#endif  // OVERLAY_H
