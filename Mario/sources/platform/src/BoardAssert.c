/*
 * BoardAssert.c
 *
 *  Created on: 14 paz 2026
 *      Author: tomasz
 */

#include <stm32f446xx.h>
#include "core_cm4.h"

#include "BoardAssert.h"
#include "printf_logger.h"

void BOARD_AssertFailed(const char* func, int line)
{
	__disable_irq(); // stop DMA/sound, the game state can no longer be trusted

	printf_v("\n### ASSERT %s:%d ###\n", func, line);

	__BKPT(0);   // halt the debugger
	while (1);
}