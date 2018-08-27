#ifndef GOL_H
#define GOL_H

#include "dynamic_libs/vpad_functions.h"
//Using modified version of draw to render at twice the scale to improve framerate
#include "draw.h"

typedef int button_t;
#define BUTTON_A        0x8000
#define BUTTON_B        0x4000
#define BUTTON_X        0x2000
#define BUTTON_Y        0x1000
#define BUTTON_LEFT     0x0800
#define BUTTON_RIGHT    0x0400
#define BUTTON_UP       0x0200
#define BUTTON_DOWN     0x0100
#define BUTTON_ZL       0x0080
#define BUTTON_ZR       0x0040
#define BUTTON_L        0x0020
#define BUTTON_R        0x0010
#define BUTTON_PLUS     0x0008
#define BUTTON_MINUS    0x0004
#define BUTTON_HOME     0x0002
#define BUTTON_SYNC     0x0001
#define BUTTON_STICK_R  0x00020000
#define BUTTON_STICK_L  0x00040000
#define BUTTON_TV       0x00010000

typedef int state_t;
#define STATE_INITIAL 0x0
#define STATE_PAUSED  0x1
#define STATE_PLAYING 0x2

struct rgb
{
	int r;
	int g;
	int b;
};

struct golCell
{
	int x;
	int y;
	bool isAlive;
	struct golCell *neighbors[8];
	int neighborCount;
	int wrapNeighborCount;
};

struct vec2
{
	int x;
	int y;
};

struct golGlobals
{
	state_t state;
	int rows;
	int cols;
	struct golCell **cells;
	struct golCell **prevCells;
	int cellSize;
	struct rgb colors[7];
	int aliveColor;
	struct vec2 gridPos;
	int lineSize;
};

void init (struct golGlobals *gameState);
void renderGrid(struct golGlobals *gameState, bool advanceGeneration);
void clearGrid(struct golGlobals *gameState);
struct vec2 pointToGrid(struct golGlobals *gameState, VPADData *vpad);
void enableCell(struct golGlobals *gameState, int x, int y, bool alive);


// Color Stuff
typedef int color_t;
#define COLOR_RED 0
#define COLOR_ORANGE 1
#define COLOR_YELLOW 2
#define COLOR_GREEN 3
#define COLOR_CYAN 4
#define COLOR_BLUE 5
#define COLOR_MAGENTA 6

typedef int cycleDirection_t;
#define CYCLE_FORWARD 0
#define CYCLE_BACKWARD 1

struct rgb convertColor(color_t colorCode);
void cycleColor(struct golGlobals *gameState, cycleDirection_t direction);

#endif /* GOL_H */
