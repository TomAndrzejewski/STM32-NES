/*
 * NES_Functions.h
 *
 *  Created on: 19 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NES_FUNCTIONS_H_
#define SOURCES_INC_NES_FUNCTIONS_H_

#include "NES_Defs.h"
#include "NES_Types.h"

void delay(uint32_t ms);
void delayUS(uint32_t us);

uint32_t GetTimestamp();
uint32_t CalcTimeUS(uint32_t startTime);
uint32_t CalcTimeMS(uint32_t startTime);

void fast_memset(void *dest, uint32_t value, size_t bytes);

int CalcRectArea(Rect_t rect);
int CalcRectXLen(Rect_t rect);
int CalcRectYLen(Rect_t rect);

int Point_Move(Point_t* src, Point_t* moveVector);



#endif /* SOURCES_INC_NES_FUNCTIONS_H_ */
