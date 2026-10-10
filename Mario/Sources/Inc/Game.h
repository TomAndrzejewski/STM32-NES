/*
 * Game.h
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GAME_H_
#define SOURCES_INC_GAME_H_

#include "Game_Types.h"

int 	GAME_InitContext(GameContext_t* ctx);
int     GAME_Update(GameContext_t* ctx);
void    GAME_HandlePlayerLevelUp(GameContext_t* ctx);

bool 	MISC_IsThisPlayerID(const GameObjectID id);

int 	PLAYER_ClearFlags(PlayerState_t* player);
int 	PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect);
void    PLAYER_LevelUp(PlayerState_t* player);

int		ENEMIES_UpdateFlags(Enemies_t* enemies, const GameContext_t* ctx);
int		ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect);
bool	ENEMIES_CalcIsOnScreen(const EnemyState_t* enemy, const Rect_t* screenRect);

void    FGOBJECTS_ClearFlags(GameContext_t* ctx);
int     FGOBJECTS_GetDirtyRect(const ForegroundObject_t* obj, Rect_t* dirtyRect);

#endif /* SOURCES_INC_GAME_H_ */
