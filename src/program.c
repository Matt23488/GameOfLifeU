#include "program.h"
#include "dynamic_libs/os_functions.h"
#include <string.h>
#include <stdlib.h>

void printButtons(VPADData*);

int _entryPoint()
{
	srand(time(0));

    InitOSFunctionPointers();
    InitVPadFunctionPointers();

	//Call the Screen initilzation function.
	OSScreenInit();
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	//Set the buffer area.
	OSScreenSetBufferEx(0, (void *)0xF4000000);
	OSScreenSetBufferEx(1, (void *)0xF4000000 + buf0_size);

	int ii;
	for (ii = 0; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);

	struct golGlobals gameState;
	init(&gameState);

	//randomizeColors(&gameState);

	/****************************>            VPAD Loop            <****************************/
	int error;
	VPADData vpad_data;
	// char buffer[255];
	// __os_snprintf(buffer, 255, "%d, %d <- Touch Coords\n%d, %d <- Grid Coords", /*-1, -1,*/ -1, -1, -1, -1);
	while (1)
	{
		VPADRead(0, &vpad_data, 1, &error);
		
		// fillScreen(0,0,0,0);
		// renderGrid(&gameState);
		// flipBuffers();

		if (vpad_data.tpdata.touched)
		{
			struct vec2 cellPt = pointToGrid(&gameState, &vpad_data);
			enableCell(&gameState, cellPt.x, cellPt.y, !(vpad_data.btns_h & BUTTON_ZL));
		}

		if (vpad_data.btns_d & BUTTON_X)
		{
			for (int y = 0; y < gameState.rows; y++)
			{
				for (int x = 0; x < gameState.cols; x++)
				{
					gameState.cells[y][x].isAlive = false;
				}
			}
		}
		if (vpad_data.btns_d & BUTTON_L || (vpad_data.btns_h & BUTTON_L && vpad_data.btns_h & BUTTON_R)) cycleColor(&gameState, CYCLE_BACKWARD);
		if (vpad_data.btns_d & BUTTON_R) cycleColor(&gameState, CYCLE_FORWARD);

		if (vpad_data.btns_h & BUTTON_HOME) break;

		renderGrid(&gameState, vpad_data.btns_d & BUTTON_A || vpad_data.btns_h & BUTTON_B);
		flipBuffers();
	}
	for (int i = 0; i < gameState.rows; i++)
	{
		free(gameState.cells[i]);
		free(gameState.prevCells[i]);
	}
	free(gameState.cells);
	free(gameState.prevCells);

	//WARNING: DO NOT CHANGE THIS. YOU MUST CLEAR THE FRAMEBUFFERS AND IMMEDIATELY CALL EXIT FROM THIS FUNCTION. RETURNING TO LOADER CAUSES FREEZE.
	for(ii = 0;ii<2;ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
	return 0;
}