/*
 * Background.h
 *
 *  Created on: 5 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_BACKGROUND_H_
#define SOURCES_INC_BACKGROUND_H_

#include "NES_Defs.h"
#include "NES_Types.h"


typedef struct
{
	RepSprite_t floorSprite;
	Point_t		floorPixelPos;

	Sprite_t 	jedynkaSprite;
	Point_t		jedynkaPixelPos;

	Sprite_t 	dwojkaSprite;
	Point_t		dwojkaPixelPos;

}Background_t;


int 	BG_Init(Background_t* p);

int		BG_RenderFloor(Background_t* p);
int		BG_RenderJedynka(Background_t* p);
int 	BG_RenderDwojka(Background_t* p);


#endif /* SOURCES_INC_BACKGROUND_H_ */
