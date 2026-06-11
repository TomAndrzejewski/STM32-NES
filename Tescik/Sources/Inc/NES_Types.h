/*
 * NES_Types.h
 *
 *  Created on: 4 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NES_TYPES_H_
#define SOURCES_INC_NES_TYPES_H_


typedef struct{
	int x;
	int y;
}Point_t;

typedef struct{
	Point_t p1; //LEFT-DOWN
	Point_t p2; //RIGHT-UP
}Rect_t;

typedef struct
{
	Rect_t	visiblePartRect;

	Rect_t	baseRect;
	Point_t	baseToSpriteOffset;
	Rect_t	movedSpriteRect;

}SpriteRender_t;

typedef struct
{
	const uint16_t* bitmap;
	Point_t			size;

	SpriteRender_t	render;

}Sprite_t;

typedef struct
{
	Sprite_t	sprite;
	Point_t		mulVector;
}RepSprite_t;


#endif /* SOURCES_INC_NES_TYPES_H_ */
