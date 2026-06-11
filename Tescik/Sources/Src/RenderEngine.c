/*
 * RenderEngine.c
 *
 *  Created on: 31 maj 2026
 *      Author: tomasz
 */

#include <string.h>
#include <stdbool.h>

#include "NES_Functions.h"

#define DEFINE_RENDER_ENGINE
#include "RenderEngine.h"

#include "LCDControl.h"


#define FB_0	(0)
#define FB_1	(1)

static uint8_t _Framebuffer0[FRAMEBUFFER_SIZE];
static uint8_t _Framebuffer1[FRAMEBUFFER_SIZE];

static int _WritePos = 0;

static uint8_t* _FB = NULL;
static uint8_t	_CurrFB = FB_0;


void RE_Init()
{
	memset(_Framebuffer0, 0, sizeof(_Framebuffer0));
	memset(_Framebuffer1, 0, sizeof(_Framebuffer1));
	_WritePos = 0;

	_CurrFB = FB_0;
	_FB = _Framebuffer0;
}

int RE_SendFB(int pixelsToSend)
{
	if (pixelsToSend <= 0)
	{
		return -1;
	}

	DMA2_SPI1_Send_NoBlock(_FB, pixelsToSend*2);

	if (_CurrFB == FB_0)
	{
		_FB = _Framebuffer1;
		_CurrFB = FB_1;
	}
	else
	{
		_FB = _Framebuffer0;
		_CurrFB = FB_0;
	}

	return 0;
}

void RE_ResetFB()
{
	_WritePos = 0;
}

int RE_FillPixel(uint16_t pixel)
{
	if (_WritePos + BYTES_PER_PIXEL >= FRAMEBUFFER_SIZE)
	{
		return -1;
	}

	*(uint16_t*)(_FB + _WritePos) = pixel;
	_WritePos += BYTES_PER_PIXEL;

	return 0;
}

int RE_OmmitPixel()
{
	if (_WritePos + BYTES_PER_PIXEL >= FRAMEBUFFER_SIZE)
	{
		return -1;
	}

	_WritePos += BYTES_PER_PIXEL;

	return 0;
}

int RE_RenderFullBackgroud(uint16_t pixel)
{
	for (int i = 0; i < FRAMEBUFFERS_PER_LCD; i++)
	{
		int ret = RE_FillBackgroud(pixel, FRAMEBUFFER_NUMOF_PIXELS);
		if (ret < 0)
		{
			return -1;
		}
		RE_SendFB(FRAMEBUFFER_NUMOF_PIXELS);
		RE_ResetFB();
	}

	return 0;
}

int RE_FillBackgroud(uint16_t pixel, int numOfPixels)
{
	if (numOfPixels <= 0)
	{
		return -5;
	}
	if (numOfPixels > FRAMEBUFFER_NUMOF_PIXELS)
	{
		return -10;
	}

	for (int i = 0; i < numOfPixels; i++)
	{
		RE_FillPixel(pixel);
	}

	return 0;
}

int RE_RenderMario(const uint16_t* sprite, Rect_t baseRect, Rect_t spriteRect, Point_t offset)
{
	if (sprite == NULL)
	{
		return -1;
	}

	RE_ResetFB();

	int baseRectArea = CalcRectArea(baseRect);
	int ret = RE_FillBackgroud(LCD_Colors[LCD_GREEN], baseRectArea);
	if (ret < 0)
	{
		return -10;
	}

	Rect_t movedSpriteRect = spriteRect;
	movedSpriteRect.p1.x += baseRect.p1.x;
	movedSpriteRect.p2.x += baseRect.p1.x;
	movedSpriteRect.p1.y += baseRect.p1.y;
	movedSpriteRect.p2.y += baseRect.p1.y;

	movedSpriteRect.p1.x += offset.x;
	movedSpriteRect.p2.x += offset.x;
	movedSpriteRect.p1.y += offset.y;
	movedSpriteRect.p2.y += offset.y;

	Rect_t visibleRect;
	visibleRect.p1.x = 0;
	visibleRect.p1.y = 0;
	visibleRect.p2.x = 16;
	visibleRect.p2.y = 16;

	RE_FillSprite(sprite, baseRect, movedSpriteRect, visibleRect);

	ret = RE_SendFB(baseRectArea);
	RE_ResetFB();
	if (ret < 0)
	{
		return -15;
	}

	return 0;
}

int RE_RenderSprite(Sprite_t* sprite, bool fillBG)
{
	if (sprite == NULL)
	{
		return -1;
	}

	Rect_t movedSpriteRect;
	movedSpriteRect.p1.x = 0;
	movedSpriteRect.p1.y = 0;
	movedSpriteRect.p2.x = sprite->size.x;
	movedSpriteRect.p2.y = sprite->size.y;

	movedSpriteRect.p1.x += sprite->render.baseRect.p1.x;
	movedSpriteRect.p2.x += sprite->render.baseRect.p1.x;
	movedSpriteRect.p1.y += sprite->render.baseRect.p1.y;
	movedSpriteRect.p2.y += sprite->render.baseRect.p1.y;

	movedSpriteRect.p1.x += sprite->render.baseToSpriteOffset.x;
	movedSpriteRect.p2.x += sprite->render.baseToSpriteOffset.x;
	movedSpriteRect.p1.y += sprite->render.baseToSpriteOffset.y;
	movedSpriteRect.p2.y += sprite->render.baseToSpriteOffset.y;


	RE_ResetFB();

	int ret = 0;
	int baseRectArea = CalcRectArea(sprite->render.baseRect);

	if (fillBG)
	{
		int ret = RE_FillBackgroud(LCD_Colors[LCD_GREEN], baseRectArea);
		if (ret < 0)
		{
			return -10;
		}
	}

	RE_FillSprite(sprite->bitmap, sprite->render.baseRect, movedSpriteRect, sprite->render.visiblePartRect);

	LCD_DrawRect(sprite->render.baseRect);

	ret = RE_SendFB(baseRectArea);
	RE_ResetFB();
	if (ret < 0)
	{
		return -15;
	}

	return 0;
}

int RE_FillSprite(const uint16_t* sprite, Rect_t baseRect, Rect_t spriteRect, Rect_t visibleRect)
{
	if (sprite == NULL)
	{
		return -1;
	}

	RE_ResetFB();

	int baseRectXLen = CalcRectXLen(baseRect);
	int spriteRectXLen = CalcRectXLen(spriteRect);
	int spriteRectYLen = CalcRectYLen(spriteRect);

	int spriteToVisibleOffsetX = spriteRectXLen + visibleRect.p1.x - visibleRect.p2.x;

	int offsetX = spriteRect.p1.x - baseRect.p1.x;
	int offsetY = spriteRect.p1.y - baseRect.p1.y;
	int startOffset = offsetY * baseRectXLen + offsetX + visibleRect.p1.x;
	int jump = baseRectXLen + spriteToVisibleOffsetX - spriteRectXLen;


	_WritePos += startOffset * BYTES_PER_PIXEL;
	for (int i = 0; i < spriteRectYLen; i++)
	{
		for (int j = visibleRect.p1.x; j < visibleRect.p2.x; j++)
		{
			if (sprite[i * spriteRectXLen + j] == LCD_TRANSPARENT_COLOR)
			{
				RE_OmmitPixel();
			}
			else
			{
				RE_FillPixel(sprite[i * spriteRectXLen + j]);
			}
		}

		_WritePos += jump * BYTES_PER_PIXEL;
	}

	return 0;
}
