/*
 * GameState.h
 *
 *  Created on: 23 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GAMESTATE_H_
#define SOURCES_INC_GAMESTATE_H_

#include <stdbool.h>
#include <stdint.h>

#include "Game_Types.h"

// Game systems that a game state can switch on or off
#define GAME_SYSTEM_SIMULATION_TIME		(0x00000001)
#define GAME_SYSTEM_PLAYER_PHYSICS		(0x00000002)
#define GAME_SYSTEM_WORLD_PHYSICS		(0x00000004)
#define GAME_SYSTEM_COLLISION			(0x00000008)
#define GAME_SYSTEM_ANIMATOR			(0x00000010)

void    GAME_STATE_Init(GameStateMachine_t* sm);
void    GAME_STATE_Update(GameStateMachine_t* sm, GameContext_t* ctx);
void    GAME_STATE_RaiseEvent(GameStateMachine_t* sm, GameEventEnum event);
bool    GAME_STATE_IsSystemEnabled(const GameStateMachine_t* sm, uint32_t system);

#endif // SOURCES_INC_GAMESTATE_H_
