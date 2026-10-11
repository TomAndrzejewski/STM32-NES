/*
 * Physics.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_PHYSICS_H_
#define SOURCES_INC_PHYSICS_H_

#include "Game_Types.h"

void    PHYSICS_Update(GameContext_t* ctx);
void    PHYSICS_Player_Update(PlayerState_t* player, const GameContext_t* ctx);

#endif // SOURCES_INC_PHYSICS_H_