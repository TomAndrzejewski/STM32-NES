/*
 * Geometry.h
 *
 *  Created on: 11 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GEOMETRY_H_
#define SOURCES_INC_GEOMETRY_H_

#include <stdbool.h>

typedef struct {
	int x;
	int y;
} Point_t;

typedef struct {
	Point_t p1; //LEFT-DOWN
	Point_t p2; //RIGHT-UP
} Rect_t;

int RECT_CalcArea(const Rect_t* rect);
int RECT_CalcArea2(const Rect_t* rect);
int RECT_CalcXLen(const Rect_t* rect);
int RECT_CalcYLen(const Rect_t* rect);

void RECT_GetIntersection(const Rect_t* r1, const Rect_t* r2, Rect_t* result);
bool RECT_IsIntersection(const Rect_t* commonRect);

#endif /* SOURCES_INC_GEOMETRY_H_ */
