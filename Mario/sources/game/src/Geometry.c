/*
 * Geometry.c
 *
 *  Created on: 11 paz 2026
 *      Author: tomasz
 */

#include "Geometry.h"

#include <stddef.h>
#include <stdlib.h>

#include "BoardAssert.h"
#include "Common_Math.h"


//----------------
// PUBLIC FUNCTIONS
//----------------
int RECT_CalcArea(const Rect_t* rect)
{
	BOARD_ASSERT(rect != NULL);

	int a = abs(rect->p1.x - rect->p2.x);
	int b = abs(rect->p1.y - rect->p2.y);
	int area = a*b;
	return area;
}

int RECT_CalcArea2(const Rect_t* rect)
{
	BOARD_ASSERT(rect != NULL);

	int a = abs(rect->p1.x - rect->p2.x) + 1;
	int b = abs(rect->p1.y - rect->p2.y) + 1;
	int area = a*b;
	return area;
}

int RECT_CalcXLen(const Rect_t* rect)
{
	BOARD_ASSERT(rect != NULL);

	int a = abs(rect->p1.x - rect->p2.x);
	return a;
}

int RECT_CalcYLen(const Rect_t* rect)
{
	BOARD_ASSERT(rect != NULL);

	int b = abs(rect->p1.y - rect->p2.y);
	return b;
}

void RECT_GetIntersection(const Rect_t* r1, const Rect_t* r2, Rect_t* result)
{
	BOARD_ASSERT(r1 != NULL && r2 != NULL && result != NULL);

	result->p1.x = MATH_Max(r1->p1.x, r2->p1.x);
	result->p1.y = MATH_Max(r1->p1.y, r2->p1.y);
	result->p2.x = MATH_Min(r1->p2.x, r2->p2.x);
	result->p2.y = MATH_Min(r1->p2.y, r2->p2.y);
}

bool RECT_IsIntersection(const Rect_t* commonRect)
{
	BOARD_ASSERT(commonRect != NULL);

	if (commonRect->p1.x <= commonRect->p2.x && commonRect->p1.y <= commonRect->p2.y) {
		return true;
	}
	return false;
}
