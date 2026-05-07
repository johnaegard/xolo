#ifndef EXPLOSION_H
#define EXPLOSION_H

void explosion_init(void);
void explosion_trigger(unsigned int x, unsigned int y);
unsigned char explosion_active(void);
void explosion_update(void);

#endif  // EXPLOSION_H
