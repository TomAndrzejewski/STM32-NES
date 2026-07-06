/*
 * Game.h
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GAME_H_
#define SOURCES_INC_GAME_H_

#include "Game_Types.h"

int GAME_InitContext(GameContext_t* ctx);

int INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state);

int PHYSICS_Player_Movement(GameContext_t* ctx);
int PHYSICS_Player_CalcMapPos(GameContext_t* ctx);

int RENDER_FirstRender(const GameContext_t* ctx);
int RENDER_RenderFloor(const BackgroundRepObject_t* floor);
int RENDER_RenderBgdSprite(const Sprite_t* p, Point_t spritePos, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);

#endif /* SOURCES_INC_GAME_H_ */
