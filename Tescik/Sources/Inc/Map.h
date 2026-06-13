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
	Point_t		jedynkaPixelPos;

	Sprite_t 	dwojkaSprite;
	Point_t		dwojkaPixelPos;

}Map_t;

int 	Map_Init(Map_t* p);
int 	Map_ReactToButtons(Map_t* p, uint32_t buttons_state);

int		Map_CameraBasedRender(Map_t* p);
int		Map_RenderScrollRect(Map_t* p, Rect_t newRect);

int		Map_RenderFloor(Map_t* p);
int		Map_RenderJedynka(Map_t* p);
int		Map_RenderJedynka2(Map_t* p);
int 	Map_RenderDwojka(Map_t* p);


#endif /* SOURCES_INC_MAP_H_ */
