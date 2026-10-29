/*
 * Input.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Input.h"

#include <stddef.h>
#include <assert.h>

#include "BoardAssert.h"
#include "PADControl.h"


//----------------
// PRIVATE MACROS
//----------------
#define US_PER_SECOND   (1000000.0f)

#define ACTION_BUTTON_BINDINGS \
	X(INPUT_ACTION_JUMP, PAD_BUTTON_A) \
	X(INPUT_ACTION_CROUCH, PAD_BUTTON_DOWN) \
	X(INPUT_ACTION_RUN_LEFT, PAD_BUTTON_LEFT) \
	X(INPUT_ACTION_RUN_RIGHT, PAD_BUTTON_RIGHT)


//----------------
// PRIVATE VARIABLES
//----------------
static uint32_t s_currButtonsState = 0;
static uint32_t s_pressedButtonsState = 0;
static uint32_t s_releasedButtonsState = 0;

// Maps each action to its pad button mask
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
static void INPUT_SetButtonsState(const PadSnapshot_t* snapshot);
static void INPUT_SetFrameTimeUS(InputState_t* input, uint32_t frameTimeUS);


//----------------
// PUBLIC FUNCTIONS
//----------------
void INPUT_Update(InputState_t* input, uint32_t frameTimeUS)
{
	BOARD_ASSERT(input != NULL);

	PadSnapshot_t snapshot;
	PAD_TakeSnapshot(&snapshot);
	INPUT_SetButtonsState(&snapshot);

	INPUT_SetFrameTimeUS(input, frameTimeUS);
}

bool INPUT_IsPressed(InputActionEnum action)
{
	BOARD_ASSERT(action < INPUT_ACTIONS_SIZE);

	uint32_t actionBit = s_actionLUT[action];
	return (s_pressedButtonsState & actionBit) != 0;
}

bool INPUT_IsDown(InputActionEnum action)
{
	BOARD_ASSERT(action < INPUT_ACTIONS_SIZE);

	uint32_t actionBit = s_actionLUT[action];
	return (s_currButtonsState & actionBit) != 0;
}

bool INPUT_IsReleased(InputActionEnum action)
{
	BOARD_ASSERT(action < INPUT_ACTIONS_SIZE);

	uint32_t actionBit = s_actionLUT[action];
	return (s_releasedButtonsState & actionBit) != 0;
}

//----------------
// PRIVATE FUNCTIONS
//----------------
static void INPUT_SetButtonsState(const PadSnapshot_t* snapshot)
{
	s_currButtonsState = snapshot->current;
	s_pressedButtonsState = snapshot->pressed;
	s_releasedButtonsState = snapshot->released;
}

static void INPUT_SetFrameTimeUS(InputState_t* input, uint32_t frameTimeUS)
{
	input->frameData.frameTimeUS = frameTimeUS;
	input->frameData.frameTimeS = input->frameData.frameTimeUS / US_PER_SECOND;
}
