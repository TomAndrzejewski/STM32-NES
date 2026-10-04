/*
 * Collision.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_COLLISION_H_
#define SOURCES_INC_COLLISION_H_

#include "Game_Types.h"

int		COLLISION_Update(GameContext_t* ctx);
void	COLLISION_Calculate(CollisionState_t* coll, const GameContext_t* ctx);
void 	COLLISION_Resolve(GameContext_t* ctx);
BumpSideEnum COLLISION_Player_FGObject(PlayerState_t* player, ForegroundObject_t* obj, const Bump_t* bump);
void 	COLLISION_Player_Floor(PlayerState_t* player, const GameContext_t* ctx);
void 	COLLISION_Player_Enemy(PlayerState_t* player, EnemyState_t* enemy, const Bump_t* bump, const GameContext_t* ctx);
void    COLLISION_FGObject_Player_Action(ForegroundObject_t* obj, PlayerState_t* player, BumpSideEnum bumpSide, ObjectsManager_t* mgr);
BumpSideEnum COLLISION_FGObject_FGObject(ForegroundObject_t* actor, ForegroundObject_t* obj, const Bump_t* bump);
void    COLLISION_FGObject_Floor(ForegroundObject_t* actor, const GameContext_t* ctx);

#endif // SOURCES_INC_COLLISION_H_