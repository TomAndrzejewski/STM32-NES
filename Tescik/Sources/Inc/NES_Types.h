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


#endif /* SOURCES_INC_NES_TYPES_H_ */
