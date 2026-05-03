#ifndef MAZE_H_
#define MAZE_H_

#define MAZE_ROWS  100
#define MAZE_COLS  100
#define CELL_PX    32
#define VIEW_COLS  8
#define VIEW_ROWS  8

// Pixel offset that centers the viewport on the map.
#define MAZE_INIT_PX  (((MAZE_COLS - VIEW_COLS) * CELL_PX) / 2)   /* 1472 */
#define MAZE_INIT_PY  (((MAZE_ROWS - VIEW_ROWS) * CELL_PX) / 2)   /* 1488 */

// Cell values: 0=no walls, 1=top wall, 2=left wall, 3=top+left walls
extern unsigned char maze[MAZE_ROWS][MAZE_COLS];

void maze_randomize(void);
void maze_init_sprites(void);
// Draw the viewport anchored at pixel offset (px, py) within the map.
// Uses (VIEW_COLS+1) x (VIEW_ROWS+1) sprites to handle sub-cell scrolling.
void maze_draw_px(unsigned int px, unsigned int py);

#endif // MAZE_H_
