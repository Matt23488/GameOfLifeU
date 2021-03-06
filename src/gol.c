#include <stdio.h>
#include <stdlib.h>
#include "dynamic_libs/os_functions.h"
#include "gol.h"
// #include <math.h>

void init (struct golGlobals *gameState)
{
	gameState->state = STATE_INITIAL;
	gameState->rows = 56;//28;
	gameState->cols = 100;//50;
	gameState->cells = (struct golCell**)calloc(gameState->rows, sizeof(struct golCell*));
	gameState->prevCells = (struct golCell**)calloc(gameState->rows, sizeof(struct golCell*));
	for (int i = 0; i < gameState->rows; i++)
	{
		gameState->cells[i] = (struct golCell*)calloc(gameState->cols, sizeof(struct golCell));
		gameState->prevCells[i] = (struct golCell*)calloc(gameState->cols, sizeof(struct golCell));
	}
	gameState->cellSize = 12;
    for (int i = 0; i < 7; i++)
    {
        gameState->colors[i] = convertColor(i);
    }
    gameState->aliveColor = 5;
	gameState->gridPos.x = 0;//2;
	gameState->gridPos.y = 0;//1;
	gameState->lineSize = 5;

	int neighborIndex;
	// Calculate neighbors
	for (int y = 0; y < gameState->rows; y++)
	{
		for (int x = 0; x < gameState->cols; x++)
		{
			neighborIndex = 0;
			// up/left
			if (x > 0 && y > 0)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y-1][x-1];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y-1][x-1];
			}
			// up
			if (y > 0)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y-1][x];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y-1][x];
			}
			// up/right
			if (x < gameState->cols - 1 && y > 0)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y-1][x+1];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y-1][x+1];
			}
			// left
			if (x > 0)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y][x-1];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y][x-1];
			}
			// right
			if (x < gameState->cols - 1)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y][x+1];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y][x+1];
			}
			// down/left
			if (x > 0 && y < gameState->rows - 1)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y+1][x-1];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y+1][x-1];
			}
			// down
			if (y < gameState->rows - 1)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y+1][x];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y+1][x];
			}
			// down/right
			if (x < gameState->cols - 1 && y < gameState->rows - 1)
			{
				gameState->cells[y][x].neighbors[neighborIndex] = &gameState->cells[y+1][x+1];
				gameState->prevCells[y][x].neighbors[neighborIndex++] = &gameState->prevCells[y+1][x+1];
			}

			gameState->cells[y][x].neighborCount = neighborIndex;
			gameState->prevCells[y][x].neighborCount = neighborIndex;
		}
	}
}

// TODO: Thought of an optimization. Instead of looping through the entire grid,
//       If I kept another array of golCell* that ONLY contains alive cells, I could
//       loop through that instead. Well, not an array, but a linked list would be better,
//       since I would be removing items from the middle. Maybe a doubley-linked list, but
//       I think I would just keep track of the previous node within the loop. That would save
//       on memory.
void renderGrid(struct golGlobals *gameState, bool advanceGeneration)
{
    if (advanceGeneration)
    {
        // swap cell buffers
        struct golCell **temp = gameState->cells;
        gameState->cells = gameState->prevCells;
        gameState->prevCells = temp;
    }

    int offsetX = gameState->gridPos.x;
    int offsetY = gameState->gridPos.y;
    int r, g, b, aliveNeighborCount;
    struct golCell *cell;
    for (int y = 0; y < gameState->rows; y++)
    {
        for (int x = 0; x < gameState->cols; x++)
        {
            if (advanceGeneration)
            {
                aliveNeighborCount = 0;
                for (int i = 0; i < gameState->prevCells[y][x].neighborCount; i++)
                {
                    if (gameState->prevCells[y][x].neighbors[i]->isAlive)
                        aliveNeighborCount++;
                }

                if (gameState->prevCells[y][x].isAlive)
                {
                    gameState->cells[y][x].isAlive = aliveNeighborCount == 2 || aliveNeighborCount == 3;
                }
                else
                {
                    gameState->cells[y][x].isAlive = aliveNeighborCount == 3;
                }
            }

            // TODO: fix this. This optimization did NOT make the game too fast. It was a problem with the two buffers.
            // I'll need to keep a third grid to keep track of the state 2 generations ago and compare that against the
            // current state, rather than the previous. That should do the trick.
            if (true || !advanceGeneration || gameState->cells[y][x].isAlive != gameState->prevCells[y][x].isAlive)
            {
                cell = &gameState->cells[y][x];
                if (cell->isAlive)
                {
                    r = gameState->colors[gameState->aliveColor].r;
                    g = gameState->colors[gameState->aliveColor].g;
                    b = gameState->colors[gameState->aliveColor].b;
                }
                else
                {
                    r = g = b = 48;
                }
                drawFillRect(
                    x * gameState->cellSize + offsetX,
                    y * gameState->cellSize + offsetY,
                    x * gameState->cellSize + offsetX + gameState->cellSize,
                    y * gameState->cellSize + offsetY + gameState->cellSize,
                    r, g, b, 0);
            }
            offsetX += gameState->lineSize;
        }
        offsetX = gameState->gridPos.x;
        offsetY += gameState->lineSize;
    }
}

void clearGrid(struct golGlobals *gameState)
{
    for (int y = 0; y < gameState->rows; y++)
    {
        for (int x = 0; x < gameState->cols; x++)
        {
            gameState->cells[y][x].isAlive = false;
        }
    }
}

struct vec2 pointToGrid(struct golGlobals *gameState, VPADData *vpad)
{
    double tx1 = 102;
    double ty1 = 189;

    double tx2 = 3952;
    double ty2 = 3882;

    double sw = 854;
    double sh = 480;

    double scaleX = (tx2 - tx1) / sw;
    double scaleY = (ty2 - ty1) / sh;

    struct vec2 cellPos;
    cellPos.x = ((vpad->tpdata.x - tx1) / scaleX - (double)gameState->gridPos.x) / (double)(gameState->cellSize + gameState->lineSize);
    cellPos.y = (sh - ((vpad->tpdata.y - ty1) / scaleY) - 1 - (double)gameState->gridPos.y) / (double)(gameState->cellSize + gameState->lineSize);

    if (cellPos.x < 0) cellPos.x = 0;
    else if (cellPos.x >= gameState->cols) cellPos.x = gameState->cols - 1;
    if (cellPos.y < 0) cellPos.y = 0;
    else if (cellPos.y >= gameState->rows) cellPos.y = gameState->rows - 1;

    return cellPos;
}

void enableCell(struct golGlobals *gameState, int x, int y, bool alive)
{
    gameState->cells[y][x].isAlive = alive;
}

struct rgb convertColor(int colorCode)
{
    struct rgb color;
    color.r = 0;
    color.g = 0;
    color.b = 0;

    switch (colorCode)
    {
        case COLOR_RED:
            color.r = 255;
            break;
        case COLOR_ORANGE:
            color.r = 255;
            color.g = 127;
            break;
        case COLOR_YELLOW:
            color.r = 255;
            color.g = 255;
            break;
        case COLOR_GREEN:
            color.g = 255;
            break;
        case COLOR_CYAN:
            color.g = 255;
            color.b = 255;
            break;
        case COLOR_BLUE:
            color.b = 255;
            break;
        case COLOR_MAGENTA:
            color.r = 255;
            color.b = 255;
            break;
    }

    return color;
}

void cycleColor(struct golGlobals *gameState, int direction)
{
    if (direction == CYCLE_FORWARD)
    {
        gameState->aliveColor++;
        if (gameState->aliveColor == 7) gameState->aliveColor = 0;
    }
    else
    {
        gameState->aliveColor--;
        if (gameState->aliveColor == -1) gameState->aliveColor = 6;
    }
}

// TODO: Thinking of moving all input to its own "class"
// This will be refactored and made better by using the actual
// analog data, rather than just the direction.
void moveGrid(struct golGlobals *gameState, direction_t direction)
{
    int deltaX = 0;
    int deltaY = 0;

    if (direction & DIRECTION_UP) deltaY = 20;
    else if (direction & DIRECTION_DOWN) deltaY = -20;
    if (direction & DIRECTION_LEFT) deltaX = 20;
    else if (direction & DIRECTION_RIGHT) deltaX = -20;

    gameState->gridPos.x += deltaX;
    gameState->gridPos.y += deltaY;

    if (gameState->gridPos.x > 0) gameState->gridPos.x = 0;
    if (gameState->gridPos.y > 0) gameState->gridPos.y = 0;
    fillScreen(0,0,0,0);
}