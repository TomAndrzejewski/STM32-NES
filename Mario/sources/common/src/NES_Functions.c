/*
 * NES_Functions.c
 *
 *  Created on: 19 maj 2026
 *      Author: tomasz
 */

#include <stm32f446xx.h>
#include "core_cm4.h"

#include "NES_Defs.h"
#include "NES_Functions.h"
#include "NES_Assert.h"
#include "printf_logger.h"

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

uint32_t CalcDiffTimeUS(uint32_t startTime, uint32_t finishTime)
{
	uint32_t elapsed_cycles = finishTime - startTime;

	uint32_t cycles_per_us = (SYSCLOCK_MHZ / 1000000UL);

	return (elapsed_cycles / cycles_per_us);
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

void NES_AssertFailed(const char* func, int line)
{
	__disable_irq(); // stop DMA/sound, the game state can no longer be trusted

	printf_v("\n### ASSERT %s:%d ###\n", func, line);

	__BKPT(0);   // halt the debugger
	while (1);
}
