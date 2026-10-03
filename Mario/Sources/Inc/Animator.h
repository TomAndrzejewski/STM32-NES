/*
 * Animator.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_ANIMATOR_H_
#define SOURCES_INC_ANIMATOR_H_

#include "Game_Types.h"

int     ANIMATOR_Update(GameContext_t* ctx);
int     ANIMATOR_Player_Update(PlayerState_t* player, const GameContext_t* ctx);
int     ANIMATOR_Player_Decide(PlayerState_t* player, const GameContext_t* ctx);
int     ANIMATOR_Player_SetAsset(PlayerState_t* player);
int     ANIMATOR_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx);
int     ANIMATOR_FGObject_Decide(ForegroundObject_t* obj, const GameContext_t* ctx);
int     ANIMATOR_FGObject_SetAsset(ForegroundObject_t* obj);
int     ANIMATOR_Enemy_Update(EnemyState_t* enemy, const GameContext_t* ctx);
int     ANIMATOR_Enemy_Decide(EnemyState_t* enemy, const GameContext_t* ctx);
int     ANIMATOR_Enemy_SetAsset(EnemyState_t* enemy);

#endif // SOURCES_INC_ANIMATOR_H_