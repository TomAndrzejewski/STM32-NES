/*
 * NES_Functions.c
 *
 *  Created on: 19 maj 2026
 *      Author: tomasz
 */

#include <stm32f446xx.h>
#include "core_cm4.h"
#include "stdlib.h"

#include "NES_Defs.h"
#include "NES_Functions.h"

void delayUS(uint32_t us)
{
	uint32_t cycles_per_us = (SYSCLOCK_MHZ / 1000000UL);
	uint32_t total_cycles = us * cycles_per_us;
	uint32_t start_cycles = DWT->CYCCNT;

	while ((DWT->CYCCNT - start_cycles) < total_cycles);
}

void delay(uint32_t ms)
{
	delayUS(ms * 1000);
}

uint32_t GetTimestamp()
{
	uint32_t ts = DWT->CYCCNT;
	return ts;
}

uint32_t CalcTimeUS(uint32_t startTime)
{
	uint32_t current_cycles = DWT->CYCCNT;
	uint32_t elapsed_cycles = current_cycles - startTime;

	uint32_t cycles_per_us = (SYSCLOCK_MHZ / 1000000UL);

	return (elapsed_cycles / cycles_per_us);
}

int CalcRectArea(Rect_t rect)
{
	int a = abs(rect.p1.x - rect.p2.x);
	int b = abs(rect.p1.y - rect.p2.y);
	int area = a*b;
	return area;
}

int CalcRectXLen(Rect_t rect)
{
	int a = abs(rect.p1.x - rect.p2.x);
	return a;
}

int CalcRectYLen(Rect_t rect)
{
	int b = abs(rect.p1.y - rect.p2.y);
	return b;
}
