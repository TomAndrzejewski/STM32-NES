/*
 * NES_Types.h
 *
 *  Created on: 4 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NES_TYPES_H_
#define SOURCES_INC_NES_TYPES_H_

#include <stdint.h>
#include <stdbool.h>
// #include "NES_Defs.h"

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
	Rect_t	commonRect;
	Rect_t	baseRect;
	Point_t	baseToSpriteOffset;
	int		LCDOffsetX;
	bool 	mirrorX;

}SpriteRender_t;

typedef struct
{
	const uint16_t* bitmap;
	Point_t			size;

}Sprite_t;

typedef struct
{
	Sprite_t	sprite;
	Point_t		mulVector;
}RepSprite_t;


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;


#endif /* SOURCES_INC_NES_TYPES_H_ */
