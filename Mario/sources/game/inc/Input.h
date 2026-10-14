/*
 * Input.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_INPUT_H_
#define SOURCES_INC_INPUT_H_

#include "Game_Types.h"

void    INPUT_Update(InputState_t* input, const GameContext_t* ctx, const uint32_t frameTimeUS);

#endif // SOURCES_INC_INPUT_H_