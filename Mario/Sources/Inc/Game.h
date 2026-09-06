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
int		COLLISION_Calculate(CollisionState_t* coll, const GameContext_t* ctx);
int 	COLLISION_Resolve(GameContext_t* ctx);
int 	COLLISION_Player_FGObject(PlayerState_t* player, ForegroundObject_t* obj, const Bump_t* bump, const GameContext_t* ctx);
int 	COLLISION_Player_Floor(PlayerState_t* player, const Bump_t* bump, const GameContext_t* ctx);
int 	COLLISION_Player_Enemy(PlayerState_t* player, EnemyState_t* enemy, const Bump_t* bump, const GameContext_t* ctx);
int     COLLISION_FGObject_Player_Action(ForegroundObject_t* obj, PlayerState_t* player, BumpSideEnum bumpSide, const GameContext_t* ctx);

int		PHYSICS_Update(GameContext_t* ctx);
int		PHYSICS_Player_Update(PlayerState_t* player, const GameContext_t* ctx);
int     PHYSICS_Player_RestartFlags(PlayerState_t* player);
int 	PHYSICS_Player_Movement(PlayerState_t* player, const GameContext_t* ctx);
int 	PHYSICS_Player_CalcMapPos(PlayerState_t* player, const GameContext_t* ctx);
int     PHYSICS_Player_CalcMovementDirection(PlayerState_t* player);

int 	CAMERA_Update(CameraState_t* camera, const GameContext_t* ctx);
int 	CAMERA_CalcPos(CameraState_t* camera, const PlayerState_t* player);
int 	CAMERA_CalcScreenRect(CameraState_t* camera);

bool 	MISC_IsThisPlayerID(const GameObjectID id);
bool 	MISC_IsThisEnemyID(const GameObjectID id);
bool 	MISC_IsThisFGID(const GameObjectID id);
bool 	MISC_IsThisBGID(const GameObjectID id);

int 	PLAYER_ClearFlags(PlayerState_t* player);
int 	PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect);

int		ENEMIES_UpdateFlags(Enemies_t* enemies, const GameContext_t* ctx);
int		ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect);
bool	ENEMIES_CalcIsOnScreen(const EnemyState_t* enemy, const Rect_t* screenRect);


int     ANIMATOR_Update(GameContext_t* ctx);
int     ANIMATOR_Player_Update(PlayerState_t* player, const GameContext_t* ctx);
int     ANIMATOR_Player_Decide(PlayerState_t* player, const GameContext_t* ctx);
int     ANIMATOR_Player_SetAsset(PlayerState_t* player);
int     ANIMATOR_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx);
int     ANIMATOR_FGObject_Decide(ForegroundObject_t* obj, const GameContext_t* ctx);
int     ANIMATOR_FGObject_SetAsset(ForegroundObject_t* obj);






int 	RENDERER_Update(GameContext_t* ctx);
int 	RENDERER_Update1(GameContext_t* ctx);
int 	RENDERER_Update2(GameContext_t* ctx);
int 	RENDERER_FirstRender(const GameContext_t* ctx);
int 	RENDERER_ScrollRender(RendererState_t* renderer, const GameContext_t* ctx);
int		RENDERER_DirtyRects_Calculate(RendererState_t* renderer, const GameContext_t* ctx);
int		RENDERER_DirtyRects_Render(RendererState_t* renderer, const GameContext_t* ctx);
int 	RENDERER_RenderFloor(const BackgroundRepObject_t* floor);
int 	RENDERER_RenderBGObject(const BackgroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);
int		RENDERER_RenderFGObject(const ForegroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);
int		RENDERER_RenderEnemy(const EnemyState_t* enemy, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);
int		RENDERER_RenderPlayer(const PlayerState_t* player, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);

#endif /* SOURCES_INC_GAME_H_ */
