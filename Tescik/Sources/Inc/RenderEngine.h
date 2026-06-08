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


#ifdef DEFINE_RENDER_ENGINE

#define GLOBAL_RENDER_ENGINE
#define INIT_RENDER_ENGINE(x) = x

#else

#define GLOBAL_RENDER_ENGINE extern
#define INIT_RENDER_ENGINE(x)

#endif


#define FRAMEBUFFER_SIZE			(40*240)
#define FRAMEBUFFER_NUMOF_PIXELS	(FRAMEBUFFER_SIZE/2)
#define BYTES_PER_PIXEL				(2)
#define FRAMEBUFFERS_PER_LCD		(LCD_SIZE/FRAMEBUFFER_NUMOF_PIXELS)


void 		RE_Init();
void 		RE_ResetFB();
uint8_t* 	RE_GetFB();
int 		RE_SendFB();

int 		RE_FillPixel(uint16_t pixel);
int 		RE_OmmitPixel();

int			RE_RenderFullBackgroud(uint16_t pixel);
int 		RE_FillBackgroud(uint16_t pixel, int numOfPixels);

int 		RE_FillSprite(const uint16_t* sprite, Rect_t baseRect, Rect_t spriteRect);
int 		RE_RenderMario(const uint16_t* sprite, Rect_t baseRect, Rect_t spriteRect, Point_t offset);
int 		RE_RenderSprite(const uint16_t* sprite, Rect_t baseRect, Rect_t spriteRect, Point_t offset, bool fillBG);

#endif /* SOURCES_INC_RENDERENGINE_H_ */
