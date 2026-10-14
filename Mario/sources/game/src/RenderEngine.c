/*
 * RenderEngine.c
 *
 *  Created on: 31 maj 2026
 *      Author: tomasz
 */

#include "RenderEngine.h"

#include <stdbool.h>
#include <string.h>

#include "Geometry.h"
#include "LCDControl.h"
#include "BoardAssert.h"
#include "Common_Defs.h"


//----------------
// PRIVATE MACROS
//----------------
#define FB_0	(0)
#define FB_1	(1)


//----------------
// PRIVATE VARIABLES
//----------------
static uint8_t _Framebuffer0[FRAMEBUFFER_SIZE];
static uint8_t _Framebuffer1[FRAMEBUFFER_SIZE];

static int _WritePos = 0;

static uint8_t* _FB = NULL;
static uint8_t	_CurrFB = FB_0;


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void RE_SendFB_withOffset(int pixelsToSend, int pixelsOffset, bool exchangeFramebuffers);
static void RE_SendFB(int pixelsToSend);
static void RE_FillPixel(uint16_t pixel);
static void RE_ResetFB();
static uint8_t* RE_GetFB();


//----------------
// PUBLIC FUNCTIONS
//----------------
void RE_Init()
{
	memset(_Framebuffer0, 0, sizeof(_Framebuffer0));
	memset(_Framebuffer1, 0, sizeof(_Framebuffer1));
	_WritePos = 0;

	_CurrFB = FB_0;
	_FB = _Framebuffer0;
}

void RE_SendRect(Rect_t inputRect, int LCDOffsetX)
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

		int leftRectArea = RECT_CalcArea2(&leftRect);
		LCD_DrawRect(leftRect, LCDOffsetX);

		RE_SendFB_withOffset(leftRectArea, 0, false);

		// right rectangle
		Rect_t rightRect = baseRect;
		rightRect.p1.x = LCDOffsetX;

		int rightRectArea = RECT_CalcArea2(&rightRect);
		LCD_DrawRect(rightRect, LCDOffsetX);

		RE_SendFB_withOffset(rightRectArea, leftRectArea, true);
		RE_ResetFB();
	}
	else
	{
		// Draw one rectangle
		int baseRectArea = RECT_CalcArea2(&baseRect);
		LCD_DrawRect(baseRect, LCDOffsetX);

		RE_SendFB(baseRectArea);
		RE_ResetFB();
	}
}

void RE_OmmitPixel()
{
	if (_WritePos + BYTES_PER_PIXEL > FRAMEBUFFER_SIZE)
	{
		return;
	}

	_WritePos += BYTES_PER_PIXEL;
}

void RE_RenderFullBackgroud(uint16_t pixel)
{
	for (int i = 0; i < FRAMEBUFFERS_PER_LCD; i++)
	{
		RE_FillBackgroud(pixel, FRAMEBUFFER_NUMOF_PIXELS);
		RE_SendFB(FRAMEBUFFER_NUMOF_PIXELS);
		RE_ResetFB();
	}
}

OPTIMIZE_FOR_DEBUG
void RE_FillBackgroud(uint16_t pixel, int numOfPixels)
{
	if (numOfPixels <= 0 || numOfPixels > FRAMEBUFFER_NUMOF_PIXELS)
	{
		return;
	}

	for (int i = 0; i < numOfPixels; i++)
	{
		RE_FillPixel(pixel);
	}
}

OPTIMIZE_FOR_DEBUG
void RE_FillSprite(const Sprite_t* sprite, const SpriteRender_t* renderContext)
{
	BOARD_ASSERT(sprite != NULL && renderContext != NULL);

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
	RECT_GetIntersection(&spriteFBRect, &fbRect, &commonFBRect);

	if (RECT_IsIntersection(&commonFBRect))
	{
		int spriteStartOffsetX = (renderContext->baseToSpriteOffset.x < 0) ? -renderContext->baseToSpriteOffset.x : 0;
		int spriteStartOffsetY = (renderContext->baseToSpriteOffset.y < 0) ? -renderContext->baseToSpriteOffset.y : 0;
		int fbStartOffsetX = (renderContext->baseToSpriteOffset.x > 0) ? renderContext->baseToSpriteOffset.x : 0;
		int fbStartOffsetY = (renderContext->baseToSpriteOffset.y > 0) ? renderContext->baseToSpriteOffset.y : 0;

		int endY = RECT_CalcYLen(&renderContext->commonRect);
		int endX = RECT_CalcXLen(&renderContext->commonRect);

		for (int i = 0; i < endX; i++)
		{
			int spriteIndX = (renderContext->mirrorX) ? (sprite->size.x - spriteStartOffsetX - 1 - i) : i + spriteStartOffsetX;
			int fbIndX = i + fbStartOffsetX;

			const uint16_t* srcColumn = s2d[spriteIndX];
			uint16_t* destColumn = fb[fbIndX];

			if (renderContext->activeColorSwap <= 0)
			{
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
			else
			{
				for (int j = 0; j < endY; j++)
				{
					int spriteIndY = j + spriteStartOffsetY;
					int fbIndY = j + fbStartOffsetY;

					if (srcColumn[spriteIndY] != LCD_TRANSPARENT_COLOR)
					{
						uint16_t color = srcColumn[spriteIndY];
						for (int k = 0; k < renderContext->activeColorSwap; k++)
						{
							if (srcColumn[spriteIndY] == renderContext->colorSwap[k][0])
							{
								color = renderContext->colorSwap[k][1];
								break;
							}
						}
						destColumn[fbIndY] = color;
					}
				}
			}
			
		}
	}
}

void RE_RenderSprite(const Sprite_t* sprite, SpriteRender_t renderContext, bool fillBG)
{
	BOARD_ASSERT(sprite != NULL);

	RE_ResetFB();

	int baseRectArea = RECT_CalcArea(&renderContext.baseRect);

	if (fillBG)
	{
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);
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

	RE_SendFB(baseRectArea);
	RE_ResetFB();
}


//----------------
// PRIVATE FUNCTIONS
//----------------
static void RE_SendFB_withOffset(int pixelsToSend, int pixelsOffset, bool exchangeFramebuffers)
{
	if (pixelsToSend <= 0 || pixelsOffset > FRAMEBUFFER_NUMOF_PIXELS)
	{
		return;
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
}

static void RE_SendFB(int pixelsToSend)
{
	if (pixelsToSend <= 0)
	{
		return;
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
}

OPTIMIZE_FOR_DEBUG
static void RE_FillPixel(uint16_t pixel)
{
	if (_WritePos + BYTES_PER_PIXEL > FRAMEBUFFER_SIZE)
	{
		return;
	}

	*(uint16_t*)(_FB + _WritePos) = pixel;
	_WritePos += BYTES_PER_PIXEL;
}

static void RE_ResetFB()
{
	_WritePos = 0;
}

static uint8_t* RE_GetFB()
{
	return _FB;
}
