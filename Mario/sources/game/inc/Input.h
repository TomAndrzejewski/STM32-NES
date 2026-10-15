/*
 * Input.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_INPUT_H_
#define SOURCES_INC_INPUT_H_

#include "Game_Types.h"

typedef enum {
    INPUT_ACTION_JUMP,
    INPUT_ACTION_CROUCH,
    INPUT_ACTION_RUN_LEFT,
    INPUT_ACTION_RUN_RIGHT,

    INPUT_ACTIONS_SIZE
} InputActions_t;

void    INPUT_Update(InputState_t* input, uint32_t frameTimeUS);

bool    INPUT_IsPressed(InputActions_t action); // Rising edge
bool    INPUT_IsDown(InputActions_t action); // Pressed now
bool    INPUT_IsReleased(InputActions_t action); // Falling edge

#endif // SOURCES_INC_INPUT_H_
