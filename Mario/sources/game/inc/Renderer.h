/*
 * Renderer.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_RENDERER_H_
#define SOURCES_INC_RENDERER_H_

#include "Game_Types.h"

int 	RENDERER_Update(GameContext_t* ctx);
int 	RENDERER_FirstRender(const GameContext_t* ctx);
int 	RENDERER_ScrollRender(RendererState_t* renderer, const GameContext_t* ctx);
int		RENDERER_DirtyRects_Calculate(RendererState_t* renderer, const GameContext_t* ctx);
int		RENDERER_DirtyRects_Render(RendererState_t* renderer, const GameContext_t* ctx);
int 	RENDERER_RenderFloor(const BackgroundRepObject_t* floor);
int 	RENDERER_RenderBGObject(const BackgroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);
int		RENDERER_RenderFGObject(const ForegroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);
int		RENDERER_RenderEnemy(const EnemyState_t* enemy, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);
int		RENDERER_RenderPlayer(const PlayerState_t* player, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX);

#endif // SOURCES_INC_RENDERER_H_