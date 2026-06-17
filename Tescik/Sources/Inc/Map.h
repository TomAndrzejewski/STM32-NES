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
	Rect_t		jedynkaPosRect;

	Sprite_t 	dwojkaSprite;
	Rect_t		dwojkaPosRect;

}Map_t;

int 	Map_Init(Map_t* p);
int 	Map_ReactToButtons(Map_t* p, uint32_t buttons_state);

int 	Map_FirstRender(Map_t* p);
int		Map_ScrollRender(Map_t* p);

int		Map_RenderFloor(Map_t* p);
int 	Map_RenderJedynka(Map_t* p, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);
int 	Map_RenderDwojka(Map_t* p, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG);


#endif /* SOURCES_INC_MAP_H_ */
