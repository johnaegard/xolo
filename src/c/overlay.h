#ifndef OVERLAY_H
#define OVERLAY_H

void overlay_init(void);
void overlay_draw_coords(unsigned int x, unsigned int y);
// Display the 4 VERA sprite collision bits (irq_flags bits 7:4, passed as bits 3:0).
void overlay_draw_collision(unsigned char coll_nibble);

#endif  // OVERLAY_H
