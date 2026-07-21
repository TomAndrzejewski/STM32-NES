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

int 	INPUT_Update(InputState_t* input, const GameContext_t* ctx, const u32 frameTimeUS);
int 	INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state);
int 	INPUT_SetFrameTimeUS(InputState_t* frameData, u32 frameTimeUS);

int		COLLISION_Update(GameContext_t* ctx);
int		COLLISION_Calculate(CollState_t* coll, const GameContext_t* ctx);
int 	COLLISION_Resolve(GameContext_t* ctx);
int 	COLLISION_Player_FGObject(PlayerState_t* player, ForegroundObject_t* obj, const Bump_t* bump, const GameContext_t* ctx);
int 	COLLISION_Player_Floor(PlayerState_t* player, const Bump_t* bump, const GameContext_t* ctx);
int 	COLLISION_Player_Enemy(PlayerState_t* player, EnemyState_t* enemy, const Bump_t* bump, const GameContext_t* ctx);

int		PHYSICS_Update(GameContext_t* ctx);
int		PHYSICS_Player_Update(PlayerState_t* player, const GameContext_t* ctx);
int 	PHYSICS_Player_Movement(PlayerState_t* player, const GameContext_t* ctx);
int 	PHYSICS_Player_CalcMapPos(PlayerState_t* player, const GameContext_t* ctx);

int 	CAMERA_Update(CameraState_t* camera, const GameContext_t* ctx);
int 	CAMERA_CalcPos(CameraState_t* camera, const PlayerState_t* player);
int 	CAMERA_CalcScreenRect(CameraState_t* camera);
int 	PLAYER_ClearFlags(PlayerState_t* player);

int 	PLAYER_TakeDamage(PlayerState_t* player, int damage);
int 	PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect);

int		ENEMIES_UpdateFlags(Enemies_t* enemies, const GameContext_t* ctx);
int		ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect);
bool	ENEMIES_CalcIsOnScreen(const EnemyState_t* enemy, const Rect_t* screenRect);

//int 	FGOBJECTS_GetDirtyRect(const ForegroundObject_t* obj, Rect_t* dirtyRect);

int 	RENDERER_Update(GameContext_t* ctx);
int 	RENDERER_FirstRender(const GameContext_t* ctx);
int 	REDNERER_ScrollRender(RendererState_t* renderer, const GameContext_t* ctx);
int 	RENDERER_RenderObjects(RendererState_t* renderer, const GameContext_t* ctx);
int 	RENDERER_RenderFloor(const BackgroundRepObject_t* floor);
int 	RENDERER_RenderBgdSprite(const Sprite_t* p, Point_t spritePos, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);

#endif /* SOURCES_INC_GAME_H_ */
