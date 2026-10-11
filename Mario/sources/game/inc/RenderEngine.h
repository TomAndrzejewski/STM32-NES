/*
 * RenderEngine.h
 *
 *  Created on: 31 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_RENDERENGINE_H_
#define SOURCES_INC_RENDERENGINE_H_

#include <stdint.h>
#include <stdbool.h>

#include "NES_Defs.h"
#include "NES_Types.h"


#define BYTES_PER_PIXEL				(2)
#define FRAMEBUFFER_SIZE			(64*240)
#define FRAMEBUFFER_NUMOF_PIXELS	(FRAMEBUFFER_SIZE/BYTES_PER_PIXEL)
#define FRAMEBUFFERS_PER_LCD		(LCD_SIZE/FRAMEBUFFER_NUMOF_PIXELS)


void    RE_Init();
void    RE_SendRect(Rect_t inputRect, int LCDOffsetX);

void    RE_OmmitPixel();

void    RE_RenderFullBackgroud(uint16_t pixel);
void    RE_FillBackgroud(uint16_t pixel, int numOfPixels);

void    RE_FillSprite(const Sprite_t* sprite, const SpriteRender_t* renderContext);
void    RE_RenderSprite(const Sprite_t* sprite, SpriteRender_t renderContext, bool fillBG);

#endif // SOURCES_INC_RENDERENGINE_H_
