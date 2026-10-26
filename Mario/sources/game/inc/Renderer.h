/*
 * Renderer.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_RENDERER_H_
#define SOURCES_INC_RENDERER_H_

#include "Game_Types.h"

void    RENDERER_Update(GameContext_t* ctx);
void    RENDERER_FirstRender(const GameContext_t* ctx, bool renderOnlyBackground);
void    RENDERER_ScrollRender(RendererState_t* renderer, const GameContext_t* ctx);
void    RENDERER_DirtyRects_Calculate(RendererState_t* renderer, const GameContext_t* ctx);
void    RENDERER_DirtyRects_Render(RendererState_t* renderer, const GameContext_t* ctx);

#endif // SOURCES_INC_RENDERER_H_
