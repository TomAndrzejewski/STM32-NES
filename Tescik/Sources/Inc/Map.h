/*
 * Map.h
 *
 *  Created on: 10 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_MAP_H_
#define SOURCES_INC_MAP_H_

#include "NES_Defs.h"
#include "NES_Types.h"


#define JEDYNKA_NUM_OF_INSTANCES	(2)

#define JEDYNKA_SIZE_X				(32)
#define JEDYNKA_SIZE_Y				(32)

static const Point_t JEDYNKA_POS[JEDYNKA_NUM_OF_INSTANCES] = {
		{ .x = 160, .y = 120 },
		{ .x = 440, .y = 120 },
};

#define CHMURKA_NUM_OF_INSTANCES	(2)

#define CHMURKA_SIZE_X				(32)
#define CHMURKA_SIZE_Y				(32)

static const Point_t CHMURKA_POS[CHMURKA_NUM_OF_INSTANCES] = {
		{ .x = 100, .y = 180 },
		{ .x = 490, .y = 80 },
};


typedef struct
{
	Sprite_t	sprite;
	Rect_t		posRect;

}ImmovableSprite_t;

typedef struct
{
	Rect_t 	mapSize;
	Point_t	currCameraPos;
	Point_t	prevCameraPos;
	Point_t cameraDiff;
	int		LCDOffsetX;


	RepSprite_t floorSprite;
	Point_t		floorPixelPos;
	int			floorYLevel;

	Sprite_t 	jedynkaSprite[JEDYNKA_NUM_OF_INSTANCES];
	Rect_t		jedynkaPosRect[JEDYNKA_NUM_OF_INSTANCES];

//	MapImmovableSprite_t	jedynkaSprite2[JEDYNKA_NUM_OF_INSTANCES];

	Sprite_t 	dwojkaSprite;
	Rect_t		dwojkaPosRect;

	Sprite_t 	chmurkaSprite2[CHMURKA_NUM_OF_INSTANCES];
	Rect_t		chmurkaPosRect[CHMURKA_NUM_OF_INSTANCES];

	ImmovableSprite_t	chmurka[CHMURKA_NUM_OF_INSTANCES];

}Map_t;

int 	Map_Init(Map_t* p);
int 	Map_ReactToButtons(Map_t* p, uint32_t buttons_state);

int 	Map_FirstRender(Map_t* p);
int		Map_ScrollRender(Map_t* p);

int		Map_RenderFloor(Map_t* p);
int 	Map_RenderJedynka(Map_t* p, int spriteInstance, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);
int 	Map_RenderDwojka(Map_t* p, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);
int 	Map_RenderChmurka(ImmovableSprite_t* p, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);


#endif /* SOURCES_INC_MAP_H_ */
