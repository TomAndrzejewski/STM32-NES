/*
 * PADControl.h
 *
 *  Created on: 31 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_SRC_PADCONTROL_H_
#define SOURCES_SRC_PADCONTROL_H_

#include <stdint.h>
#include <stdbool.h>

#include <stm32f446xx.h>


// #include "NES_Defs.h"

// #include "printf_logger.h"


#define PAD_CLK_LOW()		(GPIOA->BSRR = GPIO_BSRR_BR10)
#define PAD_CLK_HIGH()		(GPIOA->BSRR = GPIO_BSRR_BS10)
#define PAD_LATCH_LOW()		(GPIOB->BSRR = GPIO_BSRR_BR5)
#define PAD_LATCH_HIGH()	(GPIOB->BSRR = GPIO_BSRR_BS5)

#define PAD_BUTTON_A		(0x01)
#define PAD_BUTTON_B		(0x02)
#define PAD_BUTTON_SELECT	(0x04)
#define PAD_BUTTON_START	(0x08)
#define PAD_BUTTON_UP		(0x10)
#define PAD_BUTTON_DOWN		(0x20)
#define PAD_BUTTON_LEFT		(0x40)
#define PAD_BUTTON_RIGHT	(0x80)


uint32_t GetButtonsState();

void PrintButtons(uint32_t buttons_state);


#endif /* SOURCES_SRC_PADCONTROL_H_ */
