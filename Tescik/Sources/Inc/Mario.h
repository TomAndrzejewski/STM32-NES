/*
 * Mario.h
 *
 *  Created on: 4 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_MARIO_H_
#define SOURCES_INC_MARIO_H_

#include "NES_Defs.h"
#include "NES_Types.h"



typedef enum
{
	MARIO_STANDSTILL,
	MARIO_UP,
	MARIO_DOWN,
	MARIO_LEFT,
	MARIO_RIGHT,
}MarioMoveEnum;

typedef struct
{
	const uint16_t* bitmap;
	Point_t			size;
}Sprite_t;

typedef struct
{
	Sprite_t		sprite;

	Point_t			currPixelPos;
	Point_t			prevPixelPos;

	MarioMoveEnum	nextMove;

}Mario_t;


int Mario_Init(Mario_t* p);

int Mario_Render(Mario_t* p);
int Mario_SetPixelPos(Mario_t* p, Point_t pos);



#endif /* SOURCES_INC_MARIO_H_ */
