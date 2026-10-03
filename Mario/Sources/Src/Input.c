/*
 * Input.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Input.h"

#include <string.h>

#include "Game_Types.h"
#include "PADControl.h"


int INPUT_Update(InputState_t* input, const GameContext_t* ctx, const u32 frameTimeUS)
{
	if (input == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	uint32_t buttons_state = GetButtonsState();
	ret = INPUT_SetButtonsState(input, buttons_state);
	if (ret < 0) { return -1; }

	ret = INPUT_SetFrameTimeUS(input, frameTimeUS);
	if (ret < 0) { return -1; }

	return 0;
}

int INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state)
{
	if (input == NULL) { return -1; }
	input->prev_buttons_state = input->buttons_state;
	input->buttons_state = buttons_state;
	return 0;
}

int INPUT_SetFrameTimeUS(InputState_t* input, u32 frameTimeUS)
{
	if (input == NULL) { return -1; }
	input->frameData.frameTimeUS = frameTimeUS;
	input->frameData.frameTimeS = input->frameData.frameTimeUS/1000000.0;
	return 0;
}