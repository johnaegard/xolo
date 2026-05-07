#ifndef TANK_H
#define TANK_H

extern unsigned int tank_world_x;
extern unsigned int tank_world_y;

void tank_init(void);
void tank_rotate_cw(void);
void tank_rotate_ccw(void);
void tank_move_forward(void);
void tank_destroy(void);

#endif  // TANK_H
