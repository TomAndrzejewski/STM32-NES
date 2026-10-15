/*
 * Input.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Input.h"

#include <stddef.h>
#include <assert.h>

#include "Game_Types.h"
#include "BoardAssert.h"
#include "PADControl.h"


//----------------
// PRIVATE VARIABLES
//----------------
static uint32_t s_prevButtonsState = 0;
static uint32_t s_currButtonsState = 0;

#define ACTION_BUTTON_BINDINGS \
	X(INPUT_ACTION_JUMP, PAD_BUTTON_A) \
	X(INPUT_ACTION_CROUCH, PAD_BUTTON_DOWN) \
	X(INPUT_ACTION_RUN_LEFT, PAD_BUTTON_LEFT) \
	X(INPUT_ACTION_RUN_RIGHT, PAD_BUTTON_RIGHT)

// Look up table mapping action with button
static const uint32_t s_actionLUT[] = {
#define X(key, value) [(key)] = (value),
	ACTION_BUTTON_BINDINGS
#undef X
};

#define X(key, value) +1
static_assert((0 ACTION_BUTTON_BINDINGS) == INPUT_ACTIONS_SIZE, "Every action has to have a binding.");
#undef X

//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void INPUT_SetButtonsState(uint32_t buttons_state);
static void INPUT_SetFrameTimeUS(InputState_t* input, uint32_t frameTimeUS);


//----------------
// PUBLIC FUNCTIONS
//----------------
void INPUT_Update(InputState_t* input, const uint32_t frameTimeUS)
{
	BOARD_ASSERT(input != NULL);

	uint32_t buttons_state = GetButtonsState();
	INPUT_SetButtonsState(buttons_state);

	INPUT_SetFrameTimeUS(input, frameTimeUS);
}

bool INPUT_IsPressed(InputActions_t action)
{
	BOARD_ASSERT(action < INPUT_ACTIONS_SIZE);

	uint32_t actionBit = s_actionLUT[action];
	if (!(s_prevButtonsState & actionBit) && (s_currButtonsState & actionBit)) {
		return true;
	}
	return false;
}

bool INPUT_IsDown(InputActions_t action)
{
	BOARD_ASSERT(action < INPUT_ACTIONS_SIZE);

	uint32_t actionBit = s_actionLUT[action];
	if (s_currButtonsState & actionBit) {
		return true;
	}
	return false;
}

bool INPUT_IsReleased(InputActions_t action)
{
	BOARD_ASSERT(action < INPUT_ACTIONS_SIZE);

	uint32_t actionBit = s_actionLUT[action];
	if ((s_prevButtonsState & actionBit) && !(s_currButtonsState & actionBit)) {
		return true;
	}
	return false;
}

//----------------
// PRIVATE FUNCTIONS
//----------------
static void INPUT_SetButtonsState(uint32_t buttons_state)
{
	s_prevButtonsState = s_currButtonsState;
	s_currButtonsState = buttons_state;
}

static void INPUT_SetFrameTimeUS(InputState_t* input, uint32_t frameTimeUS)
{
	input->frameData.frameTimeUS = frameTimeUS;
	input->frameData.frameTimeS = input->frameData.frameTimeUS/1000000.0f;
}
