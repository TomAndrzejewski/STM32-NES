/*
 * Input.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_INPUT_H_
#define SOURCES_INC_INPUT_H_

#include "Game_Types.h"

int 	INPUT_Update(InputState_t* input, const GameContext_t* ctx, const u32 frameTimeUS);
int 	INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state);
int 	INPUT_SetFrameTimeUS(InputState_t* frameData, u32 frameTimeUS);

#endif // SOURCES_INC_INPUT_H_