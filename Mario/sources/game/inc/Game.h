/*
 * Game.h
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GAME_H_
#define SOURCES_INC_GAME_H_

#include "Game_Types.h"

void    GAME_Init(void);
void    GAME_Update(void);

bool    MISC_IsThisPlayerID(const GameObjectID id);

void    PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect);
void    PLAYER_LevelUp(PlayerState_t* player);

void    ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect);

void    FGOBJECTS_ClearFlags(GameContext_t* ctx);
void    FGOBJECTS_GetDirtyRect(const ForegroundObject_t* obj, Rect_t* dirtyRect);

#endif // SOURCES_INC_GAME_H_
