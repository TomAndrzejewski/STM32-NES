/*
 * NES_Functions.h
 *
 *  Created on: 19 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NES_FUNCTIONS_H_
#define SOURCES_INC_NES_FUNCTIONS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void delay(uint32_t ms);
void delayUS(uint32_t us);

uint32_t GetTimestamp();
uint32_t CalcTimeUS(uint32_t startTime);
uint32_t CalcTimeMS(uint32_t startTime);
uint32_t CalcDiffTimeUS(uint32_t startTime, uint32_t finishTime);

void fast_memset(void *dest, uint32_t value, size_t bytes);


#endif /* SOURCES_INC_NES_FUNCTIONS_H_ */
