/*
 * Goomba.h
 *
 *  Created on: 28 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GOOMBA_H_
#define SOURCES_INC_GOOMBA_H_

#include "NES_Types.h"

typedef struct
{
	Sprite_t		sprite;

	Point_t			currMapPos;
	Point_t			prevMapPos;

}Goomba_t;


int 		Goomba_Init(Goomba_t* p, Point_t initMapPos);

Sprite_t* 	Goomba_GetSprite(Goomba_t* p);

int 		Goomba_GetDirtyRect(Goomba_t* p, Rect_t* dirtyRect);
int			Goomba_GetMapPos(Goomba_t* p, Point_t* mapPos);

#endif /* SOURCES_INC_GOOMBA_H_ */
