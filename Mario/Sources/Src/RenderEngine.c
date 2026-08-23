/*
 * RenderEngine.c
 *
 *  Created on: 31 maj 2026
 *      Author: tomasz
 */

#include <string.h>
#include <stdbool.h>

#include "NES_Functions.h"

#include "LCDControl.h"

#include "printf_logger.h"

#define DEFINE_RENDER_ENGINE
#include "RenderEngine.h"


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

	DMA2_SPI1_Send_NoBlock(_FB, pixelsToSend*BYTES_PER_PIXEL);

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

int RE_SendFB_withOffset(int pixelsToSend, int pixelsOffset, bool exchangeFramebuffers)
{
	if (pixelsToSend <= 0 || pixelsOffset > FRAMEBUFFER_NUMOF_PIXELS)
	{
		return -1;
	}

	DMA2_SPI1_Send_NoBlock(_FB + pixelsOffset*BYTES_PER_PIXEL, pixelsToSend*BYTES_PER_PIXEL);

	if (exchangeFramebuffers)
	{
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
	}


	return 0;
}

uint8_t* RE_GetFB()
{
	return _FB;
}

void RE_ResetFB()
{
	_WritePos = 0;
}

OPTIMIZE_FOR_DEBUG
int RE_FillPixel(uint16_t pixel)
{
	if (_WritePos + BYTES_PER_PIXEL > FRAMEBUFFER_SIZE)
	{
		return -1;
	}

	*(uint16_t*)(_FB + _WritePos) = pixel;
	_WritePos += BYTES_PER_PIXEL;

	return 0;
}

int RE_OmmitPixel()
{
	if (_WritePos + BYTES_PER_PIXEL > FRAMEBUFFER_SIZE)
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

OPTIMIZE_FOR_DEBUG
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

int RE_SendRect(Rect_t inputRect, int LCDOffsetX)
{
	Rect_t baseRect = inputRect;
	// Translate to LCD coordinates
	if (baseRect.p1.x == LCD_WIDTH)
	{
		baseRect.p1.x = LCD_WIDTH - 1;
	}
	if (baseRect.p1.y == LCD_HEIGHT)
	{
		baseRect.p1.y = LCD_HEIGHT - 1;
	}
	if (baseRect.p2.x > 0)
	{
		baseRect.p2.x--;
	}
	if (baseRect.p2.y > 0)
	{
		baseRect.p2.y--;
	}

	if (baseRect.p1.x < LCDOffsetX && baseRect.p2.x >= LCDOffsetX)
	{
		// Draw two rectangles divided by vertical scroll

		// left rectangle
		Rect_t leftRect = baseRect;
		leftRect.p2.x = LCDOffsetX - 1;

		int leftRectArea = CalcRectArea2(&leftRect);
		LCD_DrawRect(leftRect, LCDOffsetX);

		int ret = RE_SendFB_withOffset(leftRectArea, 0, false);
		if (ret < 0)
		{
			printf_v("\nRE_SendRect leftRect error: %d", ret);
		}

		// right rectangle
		Rect_t rightRect = baseRect;
		rightRect.p1.x = LCDOffsetX;

		int rightRectArea = CalcRectArea2(&rightRect);
		LCD_DrawRect(rightRect, LCDOffsetX);

		ret = RE_SendFB_withOffset(rightRectArea, leftRectArea, true);
		RE_ResetFB();
		if (ret < 0)
		{
			printf_v("\nRE_SendRect rightRect error: %d", ret);
		}
	}
	else
	{
		// Draw one rectangle
		int baseRectArea = CalcRectArea2(&baseRect);
		LCD_DrawRect(baseRect, LCDOffsetX);

		int ret = RE_SendFB(baseRectArea);
		RE_ResetFB();
		if (ret < 0)
		{
			return -15;
		}
	}

	return 0;
}

int RE_RenderSprite(const Sprite_t* sprite, SpriteRender_t renderContext, bool fillBG)
{
	if (sprite == NULL)
	{
		return -1;
	}

	RE_ResetFB();

	int ret = 0;
	int baseRectArea = CalcRectArea(renderContext.baseRect);

	if (fillBG)
	{
		int ret = RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);
		if (ret < 0)
		{
			return -10;
		}
	}

	RE_FillSprite(sprite, &renderContext);

	// Translate to LCD coordinates
	if (renderContext.baseRect.p1.x == LCD_WIDTH)
	{
		renderContext.baseRect.p1.x = LCD_WIDTH - 1;
	}
	if (renderContext.baseRect.p1.y == LCD_HEIGHT)
	{
		renderContext.baseRect.p1.y = LCD_HEIGHT - 1;
	}
	if (renderContext.baseRect.p2.x > 0)
	{
		renderContext.baseRect.p2.x--;
	}
	if (renderContext.baseRect.p2.y > 0)
	{
		renderContext.baseRect.p2.y--;
	}
	LCD_DrawRect(renderContext.baseRect, renderContext.LCDOffsetX);

	ret = RE_SendFB(baseRectArea);
	RE_ResetFB();
	if (ret < 0)
	{
		return -15;
	}

	return 0;
}

OPTIMIZE_FOR_DEBUG
int RE_FillSprite(const Sprite_t* sprite, const SpriteRender_t* renderContext)
{
	if (sprite == NULL || renderContext == NULL)
	{
		return -1;
	}

	Rect_t fbRect;
	fbRect.p1.x = 0;
	fbRect.p1.y = 0;
	fbRect.p2.x = renderContext->baseRect.p2.x - renderContext->baseRect.p1.x;
	fbRect.p2.y = renderContext->baseRect.p2.y - renderContext->baseRect.p1.y;

	RE_ResetFB();
	const uint16_t (*s2d)[sprite->size.y] = (const uint16_t (*)[sprite->size.y])sprite->bitmap;
	uint16_t (*fb)[fbRect.p2.y] = (uint16_t (*)[fbRect.p2.y])RE_GetFB();

	Rect_t spriteFBRect;
	spriteFBRect.p1.x = 0;
	spriteFBRect.p1.y = 0;
	spriteFBRect.p2.x = sprite->size.x;
	spriteFBRect.p2.y = sprite->size.y;

	Rect_t commonFBRect = {0};
	Rect_GetIntersection(&spriteFBRect, &fbRect, &commonFBRect);

	if (Rect_IsIntersection(&commonFBRect))
	{
		int spriteStartOffsetX = (renderContext->baseToSpriteOffset.x < 0) ? -renderContext->baseToSpriteOffset.x : 0;
		int spriteStartOffsetY = (renderContext->baseToSpriteOffset.y < 0) ? -renderContext->baseToSpriteOffset.y : 0;
		int fbStartOffsetX = (renderContext->baseToSpriteOffset.x > 0) ? renderContext->baseToSpriteOffset.x : 0;
		int fbStartOffsetY = (renderContext->baseToSpriteOffset.y > 0) ? renderContext->baseToSpriteOffset.y : 0;

		int endY = CalcRectYLen(&renderContext->commonRect);
		int endX = CalcRectXLen(&renderContext->commonRect);
		for (int i = 0; i < endX; i++)
		{
			int spriteIndX = i + spriteStartOffsetX;
			int fbIndX = i + fbStartOffsetX;

			const uint16_t* srcColumn = s2d[spriteIndX];
			uint16_t* destColumn = fb[fbIndX];

			for (int j = 0; j < endY; j++)
			{
				int spriteIndY = j + spriteStartOffsetY;
				int fbIndY = j + fbStartOffsetY;

				if (srcColumn[spriteIndY] != LCD_TRANSPARENT_COLOR)
				{
					destColumn[fbIndY] = srcColumn[spriteIndY];
				}
			}
		}
	}

	return 0;
}


