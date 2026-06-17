/*
 * Mario.c
 *
 *  Created on: 4 cze 2026
 *      Author: tomasz
 */

#include "string.h"

#include "NES_Defs.h"
#include "NES_Types.h"
#include "NES_Functions.h"

#include "LCDControl.h"
#include "PADControl.h"
#include "RenderEngine.h"
#include "Mario.h"


// Kolory użyte w grafice (RGB565 MSB):
// 0x5DFF - Jasnoniebieski (Tło nieba)
// 0xD000 - Czerwony (Czapka i koszulka)
// 0x0210 - Niebieski (Ogrodniczki)
// 0xFCE0 - Beżowy / Skóra (Twarz i dłonie)
// 0x79E0 - Brązowy (Włosy i buty)

const uint16_t mario_16x16_dma[256] = {
		0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0xffff, 0xffff,
		0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0x00f8, 0xe0fd, 0x00f8, 0x00f8, 0xe0fd, 0x00f8, 0xe314, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0x00f8, 0xe314, 0xe314, 0x00f8, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0x00f8, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe0fd, 0xe314, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe0fd, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe0fd, 0xe0fd, 0xe314, 0xe0fd, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
	};


int Mario_Init(Mario_t* p)
{
	if (p == NULL)	{ return -1; }

	p->sprite.bitmap = mario_16x16_dma;
	p->sprite.size.x = 16;
	p->sprite.size.y = 16;

	memset(&p->currPixelPos, 0, sizeof(p->currPixelPos));
	memset(&p->prevPixelPos, 0, sizeof(p->prevPixelPos));

	p->currPixelPos.x = 0;
	p->currPixelPos.y = 32;

	p->prevPixelPos = p->currPixelPos;

	p->nextMove = MARIO_STANDSTILL;

	return 0;
}

Sprite_t* Mario_GetSprite(Mario_t* p)
{
	if (p == NULL)	{ return NULL; }
	return &p->sprite;
}

int Mario_LimitPixelPos(Mario_t* p, Point_t* newPos)
{
	if (p == NULL)	{ return -1; }
	if (newPos == NULL)	{ return -5; }

	if (newPos->x + p->sprite.size.x >= LCD_WIDTH)
	{
		newPos->x = LCD_WIDTH - 1 - p->sprite.size.x;
	}
	if (newPos->x < 0)
	{
		newPos->x = 0;
	}

	if (newPos->y + p->sprite.size.y >= LCD_HEIGHT)
	{
		newPos->y = LCD_HEIGHT - 1 - p->sprite.size.y;
	}
	if (newPos->y < 0)
	{
		newPos->y = 0;
	}

	return 0;
}

int Mario_SetPixelPos(Mario_t* p, Point_t pos)
{
	if (p == NULL)	{ return -1; }

	p->prevPixelPos = p->currPixelPos;

	p->currPixelPos = pos;
	Mario_LimitPixelPos(p, &p->currPixelPos);

	return 0;
}

int Mario_MovePixelPos(Mario_t* p, Point_t moveVector)
{
	if (p == NULL)	{ return -1; }

	p->prevPixelPos = p->currPixelPos;

	Point_Move(&p->currPixelPos, &moveVector);
	Mario_LimitPixelPos(p, &p->currPixelPos);

	return 0;
}

int Mario_ReactToButton(Mario_t* p, uint32_t buttons_state)
{
	if (p == NULL)	{ return -1; }

	Point_t moveVector = {0};
	Point_t setVector = {0};
	bool useMove = true;

	if (buttons_state & PAD_BUTTON_START)
	{
		setVector.x = 0;
		setVector.y = 0;
		useMove = false;
	}

	if (useMove)
	{
		if (buttons_state & PAD_BUTTON_RIGHT)
		{
			moveVector.x++;
		}
		if (buttons_state & PAD_BUTTON_LEFT)
		{
			moveVector.x--;
		}
		if (buttons_state & PAD_BUTTON_UP)
		{
			moveVector.y++;
		}
		if (buttons_state & PAD_BUTTON_DOWN)
		{
			moveVector.y--;
		}
		if (buttons_state & PAD_BUTTON_A)
		{
			moveVector.y += 5;
		}
		if (buttons_state & PAD_BUTTON_B)
		{
			moveVector.y -= 5;
		}

		Mario_MovePixelPos(p, moveVector);
	}
	else
	{
		Mario_SetPixelPos(p, setVector);
	}

	return 0;
}


int Mario_Render(Mario_t* p)
{
	if (p == NULL)	{ return -1; }

	Rect_t spriteRect;
	spriteRect.p1.x = 0;
	spriteRect.p1.y = 0;
	spriteRect.p2 = p->sprite.size;

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = p->prevPixelPos;
	prevDirtyRect.p2.x = p->prevPixelPos.x + spriteRect.p2.x;
	prevDirtyRect.p2.y = p->prevPixelPos.y + spriteRect.p2.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = p->currPixelPos;
	currDirtyRect.p2.x = p->currPixelPos.x + spriteRect.p2.x;
	currDirtyRect.p2.y = p->currPixelPos.y + spriteRect.p2.y;

	Rect_t commonDirtyRect;
	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	Point_t currPosToDirtyRectOffset;
	currPosToDirtyRectOffset.x = currDirtyRect.p1.x - commonDirtyRect.p1.x;
	currPosToDirtyRectOffset.y = currDirtyRect.p1.y - commonDirtyRect.p1.y;


	LCD_DrawRect(commonDirtyRect, 0);

	RE_RenderMario(p->sprite.bitmap, commonDirtyRect, spriteRect, currPosToDirtyRectOffset);

	return 0;
}

