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


#define MAP_MAX_X					(1280)
#define MAP_MAX_Y					(240)

#define JEDYNKA_SIZE_X				(32)
#define JEDYNKA_SIZE_Y				(32)

#define DWOJKA_SIZE_X				(32)
#define DWOJKA_SIZE_Y				(32)

#define CHMURKA_SIZE_X				(32)
#define CHMURKA_SIZE_Y				(32)


#define MAP_TILE_SIZE_X		(16)
#define MAP_TILE_SIZE_Y		(16)

#define JEDYNKA_SPRITE_ID	(1)
#define DWOJKA_SPRITE_ID	(2)
#define CHMURKA_SPRITE_ID	(3)

typedef struct
{
	int		spriteID;
	int		x;
	int		y;
}SpritePos_t;

static const SpritePos_t MapSpriteLoc[] = {
	    { .spriteID = 2, .x = 96, .y = 208 },
	    { .spriteID = 3, .x = 192, .y = 208 },
	    { .spriteID = 1, .x = 336, .y = 208 },
	    { .spriteID = 2, .x = 304, .y = 144 },
	    { .spriteID = 3, .x = 96, .y = 128 },
	    { .spriteID = 3, .x = 176, .y = 96 },
	    { .spriteID = 1, .x = 272, .y = 80 },
	    { .spriteID = 1, .x = 64, .y = 32 }
};


typedef struct
{
	Sprite_t	sprite;
	Rect_t		posRect;

}ImmovableSprite_t;


#define DIRTY_RECTS_SIZE		(16)
#define DIRTY_RECTS_OBJ_ID_SIZE	(16)

#define MARIO_OBJ_ID			(1)
#define GOOMBA_OBJ_ID			(2)

typedef struct
{
	Rect_t 	dirtyRect;
	int		objIDIndex;
	int		objID[DIRTY_RECTS_OBJ_ID_SIZE];
	bool	used;

}DirtyRect_t;

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

	Sprite_t 	jedynkaSprite;
	Sprite_t 	dwojkaSprite;
	Sprite_t 	chmurkaSprite;

	DirtyRect_t	dirtyRects[DIRTY_RECTS_SIZE];

}Map_t;

int 	Map_Init(Map_t* p);
int 	Map_ReactToButtons(Map_t* p, uint32_t buttons_state);

int 	Map_FirstRender(Map_t* p);
int		Map_ScrollRender(Map_t* p);
int 	Map_RenderObjects(Map_t* p);

int		Map_RenderFloor(Map_t* p);

int 	Map_RenderBgdSprite(Sprite_t* p, SpritePos_t spritePos, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);

int		Map_RenderMario(Point_t cameraPos, Rect_t dirtyMapRect, Rect_t screenRect, int LCDOffsetX);
int 	Map_GetMarioDirtyRect(Rect_t cameraRect, Rect_t* dirtyMapRect);

int 	Map_RenderGoomba(Point_t cameraPos, Rect_t dirtyMapRect, Rect_t screenRect, int LCDOffsetX);
int 	Map_GetGoombaDirtyRect(Rect_t cameraRect, Rect_t* dirtyMapRect);

#endif /* SOURCES_INC_MAP_H_ */
