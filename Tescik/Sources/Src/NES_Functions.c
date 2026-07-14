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

uint32_t CalcTimeMS(uint32_t startTime)
{
	uint32_t current_cycles = DWT->CYCCNT;
	uint32_t elapsed_cycles = current_cycles - startTime;

	uint32_t cycles_per_us = (SYSCLOCK_MHZ / 1000UL);

	return (elapsed_cycles / cycles_per_us);
}

__attribute__((optimize("-O3"), noinline))
void fast_memset(void *dest, uint32_t value, size_t bytes)
{
    uint8_t *p_byte = (uint8_t *)dest;

    // 1. Zapisuj bajt po bajcie, dopóki adres nie stanie się podzielny przez 4
    //    oraz dopóki mamy jeszcze bajty do zapisania.
    while (((uintptr_t)p_byte & 3) && bytes > 0) {
        *p_byte++ = (uint8_t)value; // Można też zrekonstruować bajt z odpowiedniej pozycji 'value'
        bytes--;
    }

    // 2. Teraz mamy pewność, że p_byte jest wyrównany do 4 bajtów!
    //    Możemy bezpiecznie przejść na operacje 32-bitowe.
    uint32_t *p_word = (uint32_t *)p_byte;
    size_t words = bytes >> 2;

    while (words--) {
        *p_word++ = value;
    }

    // 3. Obsługa końcówki (ogona) – to co zostało (0 do 3 bajtów)
    size_t tail = bytes & 3;
    if (tail) {
        p_byte = (uint8_t *)p_word;
        while (tail--) {
            *p_byte++ = (uint8_t)value;
        }
    }
}

int CalcRectArea(Rect_t rect)
{
	int a = abs(rect.p1.x - rect.p2.x);
	int b = abs(rect.p1.y - rect.p2.y);
	int area = a*b;
	return area;
}

int CalcRectArea2(const Rect_t* rect)
{
	if (rect == NULL) { return 0; }

	int a = abs(rect->p1.x - rect->p2.x) + 1;
	int b = abs(rect->p1.y - rect->p2.y) + 1;
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

int Point_Move(Point_t* src, Point_t* moveVector)
{
	if (src == NULL)		{ return -1; }
	if (moveVector == NULL)	{ return -5; }

	src->x += moveVector->x;
	src->y += moveVector->y;

	return 0;
}
