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
#include "Map.h"
#include "Mario.h"
#include "NESEngine.h"


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

//	p->currPixelPos.x = 0;
//	p->currPixelPos.y = 32;
//	p->prevPixelPos = p->currPixelPos;

	p->currMapPos.x = 0;
	p->currMapPos.y = 40;
	p->prevMapPos = p->currMapPos;

	p->nextMove = MARIO_STANDSTILL;

	return 0;
}

Sprite_t* Mario_GetSprite(Mario_t* p)
{
	if (p == NULL)	{ return NULL; }
	return &p->sprite;
}

int Mario_ReactToButton(Mario_t* p, uint32_t buttons_state)
{
	if (p == NULL)	{ return -1; }

	Point_t moveVector = {0};
	Point_t setVector = {0};
	bool useMove = true;

	Point_t cameraPos = Map_GetCameraPos(pMap);


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

		Mario_MoveMapPos(p, moveVector, cameraPos);
	}
	else
	{
		Mario_SetMapPos(p, setVector);
	}

	return 0;
}

//int Mario_LimitPixelPos(Mario_t* p, Point_t* newPos)
//{
//	if (p == NULL)	{ return -1; }
//	if (newPos == NULL)	{ return -5; }
//
//	if (newPos->x + p->sprite.size.x >= LCD_WIDTH)
//	{
//		newPos->x = LCD_WIDTH - 1 - p->sprite.size.x;
//	}
//	if (newPos->x < 0)
//	{
//		newPos->x = 0;
//	}
//
//	if (newPos->y + p->sprite.size.y >= LCD_HEIGHT)
//	{
//		newPos->y = LCD_HEIGHT - 1 - p->sprite.size.y;
//	}
//	if (newPos->y < 0)
//	{
//		newPos->y = 0;
//	}
//
//	return 0;
//}
//
//int Mario_SetPixelPos(Mario_t* p, Point_t pos)
//{
//	if (p == NULL)	{ return -1; }
//
//	p->prevPixelPos = p->currPixelPos;
//
//	p->currPixelPos = pos;
//	Mario_LimitPixelPos(p, &p->currPixelPos);
//
//	return 0;
//}
//
//int Mario_MovePixelPos(Mario_t* p, Point_t moveVector)
//{
//	if (p == NULL)	{ return -1; }
//
//	p->prevPixelPos = p->currPixelPos;
//
//	Point_Move(&p->currPixelPos, &moveVector);
//	Mario_LimitPixelPos(p, &p->currPixelPos);
//
//	return 0;
//}

int Mario_LimitMapPos(Mario_t* p, Point_t* newPos)
{
	if (p == NULL)	{ return -1; }
	if (newPos == NULL)	{ return -5; }

	if (newPos->x + p->sprite.size.x >= LCD_WIDTH)
	{
		newPos->x = MAP_MAX_X - 1 - p->sprite.size.x;
	}
	if (newPos->x < 0)
	{
		newPos->x = 0;
	}

	if (newPos->y + p->sprite.size.y >= LCD_HEIGHT)
	{
		newPos->y = MAP_MAX_Y - 1 - p->sprite.size.y;
	}
	if (newPos->y < 0)
	{
		newPos->y = 0;
	}

	return 0;
}

int Mario_SetMapPos(Mario_t* p, Point_t pos)
{
	if (p == NULL)	{ return -1; }

	p->prevMapPos = p->currMapPos;

	p->currMapPos = pos;
	Mario_LimitMapPos(p, &p->currMapPos);

	return 0;
}

int Mario_MoveMapPos(Mario_t* p, Point_t moveVector, Point_t cameraPos)
{
	if (p == NULL)	{ return -1; }

	p->prevMapPos = p->currMapPos;

	Point_t newMapPos = p->currMapPos;

	Point_Move(&newMapPos, &moveVector);
	Mario_LimitMapPos(p, &newMapPos);

	Rect_t cameraRect;
	cameraRect.p1.x = cameraPos.x;
	cameraRect.p1.y = cameraPos.y;
	cameraRect.p2.x = cameraRect.p1.x + LCD_WIDTH;
	cameraRect.p2.y = cameraRect.p1.y + LCD_HEIGHT;

	if (newMapPos.x < cameraRect.p1.x)
	{
		newMapPos.x = cameraRect.p1.x;
	}
	if (newMapPos.y < cameraRect.p1.y)
	{
		newMapPos.y = cameraRect.p1.y;
	}

	p->currMapPos = newMapPos;

	return 0;
}

int Mario_GetDirtyRect(Mario_t* p, Rect_t* dirtyRect)
{
	if (p == NULL || dirtyRect == NULL)	{ return -1; }

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = p->prevMapPos;
	prevDirtyRect.p2.x = p->prevMapPos.x + p->sprite.size.x;
	prevDirtyRect.p2.y = p->prevMapPos.y + p->sprite.size.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = p->currMapPos;
	currDirtyRect.p2.x = p->currMapPos.x + p->sprite.size.x;
	currDirtyRect.p2.y = p->currMapPos.y + p->sprite.size.y;

	Rect_t commonDirtyRect;
	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	if (commonDirtyRect.p1.x <= commonDirtyRect.p2.x && commonDirtyRect.p1.y <= commonDirtyRect.p2.y)
	{
		*dirtyRect = commonDirtyRect;
	}
	else
	{
		return -5;
	}

	return 0;
}

int Mario_GetMapPos(Mario_t* p, Point_t* mapPos)
{
	if (p == NULL || mapPos == NULL)	{ return -1; }
	*mapPos = p->currMapPos;
	return 0;
}

int Mario_Render(Mario_t* p)
{
//	if (p == NULL)	{ return -1; }
//
//	Rect_t spriteRect;
//	spriteRect.p1.x = 0;
//	spriteRect.p1.y = 0;
//	spriteRect.p2 = p->sprite.size;
//
//	Rect_t prevDirtyRect;
//	prevDirtyRect.p1 = p->prevPixelPos;
//	prevDirtyRect.p2.x = p->prevPixelPos.x + spriteRect.p2.x;
//	prevDirtyRect.p2.y = p->prevPixelPos.y + spriteRect.p2.y;
//
//	Rect_t currDirtyRect;
//	currDirtyRect.p1 = p->currPixelPos;
//	currDirtyRect.p2.x = p->currPixelPos.x + spriteRect.p2.x;
//	currDirtyRect.p2.y = p->currPixelPos.y + spriteRect.p2.y;
//
//	Rect_t commonDirtyRect;
//	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
//	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
//	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
//	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);
//
//	Point_t currPosToDirtyRectOffset;
//	currPosToDirtyRectOffset.x = currDirtyRect.p1.x - commonDirtyRect.p1.x;
//	currPosToDirtyRectOffset.y = currDirtyRect.p1.y - commonDirtyRect.p1.y;
//
//
//	LCD_DrawRect(commonDirtyRect, 0);
//
//	RE_RenderMario(p->sprite.bitmap, commonDirtyRect, spriteRect, currPosToDirtyRectOffset);

	return 0;
}

