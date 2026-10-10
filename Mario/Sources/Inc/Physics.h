/*
 * Physics.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_PHYSICS_H_
#define SOURCES_INC_PHYSICS_H_

#include "Game_Types.h"

int		PHYSICS_Update(GameContext_t* ctx);
int		PHYSICS_Player_Update(PlayerState_t* player, const GameContext_t* ctx);
int     PHYSICS_Player_RestartFlags(PlayerState_t* player);
int 	PHYSICS_Player_Movement(PlayerState_t* player, const GameContext_t* ctx);
int 	PHYSICS_Player_CalcMapPos(PlayerState_t* player, const GameContext_t* ctx);
int     PHYSICS_Player_CalcMovementDirection(PlayerState_t* player);
int     PHYSICS_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx);
void    PHYSICS_FGObject_SaveFlags(ForegroundObject_t* obj);
void    PHYSICS_FGObject_Velocity_Movement(ForegroundObject_t* obj, const GameContext_t* ctx);
void    PHYSICS_FGObject_Velocity_CalcMapPos(ForegroundObject_t* obj);
void    PHYSICS_FGObject_Time_Movement(ForegroundObject_t* obj, const GameContext_t* ctx);
void    PHYSICS_FGObject_Time_Movement_Finish(ForegroundObject_t* obj);

#endif // SOURCES_INC_PHYSICS_H_