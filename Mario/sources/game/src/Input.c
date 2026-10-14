/*
 * Input.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Input.h"

#include <stddef.h>

#include "Game_Types.h"
#include "BoardAssert.h"
#include "PADControl.h"


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state);
static void INPUT_SetFrameTimeUS(InputState_t* input, uint32_t frameTimeUS);


//----------------
// PUBLIC FUNCTIONS
//----------------
void INPUT_Update(InputState_t* input, const GameContext_t* ctx, const uint32_t frameTimeUS)
{
	BOARD_ASSERT(input != NULL && ctx != NULL);

	uint32_t buttons_state = GetButtonsState();
	INPUT_SetButtonsState(input, buttons_state);

	INPUT_SetFrameTimeUS(input, frameTimeUS);
}


//----------------
// PRIVATE FUNCTIONS
//----------------
static void INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state)
{
	input->prev_buttons_state = input->buttons_state;
	input->buttons_state = buttons_state;
}

static void INPUT_SetFrameTimeUS(InputState_t* input, uint32_t frameTimeUS)
{
	input->frameData.frameTimeUS = frameTimeUS;
	input->frameData.frameTimeS = input->frameData.frameTimeUS/1000000.0f;
}
