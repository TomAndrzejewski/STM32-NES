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


#define PAD_BUTTON_A		(0x01)
#define PAD_BUTTON_B		(0x02)
#define PAD_BUTTON_SELECT	(0x04)
#define PAD_BUTTON_START	(0x08)
#define PAD_BUTTON_UP		(0x10)
#define PAD_BUTTON_DOWN		(0x20)
#define PAD_BUTTON_LEFT		(0x40)
#define PAD_BUTTON_RIGHT	(0x80)

#define PAD_CLK_LOW()		(GPIOA->BSRR = GPIO_BSRR_BR10)
#define PAD_CLK_HIGH()		(GPIOA->BSRR = GPIO_BSRR_BS10)
#define PAD_LATCH_LOW()		(GPIOB->BSRR = GPIO_BSRR_BR5)
#define PAD_LATCH_HIGH()	(GPIOB->BSRR = GPIO_BSRR_BS5)


// Pad state collected by the sampling timer since the previous snapshot
typedef struct
{
	uint32_t current;	// Buttons held at the last sample
	uint32_t pressed;	// Buttons that went down at least once
	uint32_t released;	// Buttons that went up at least once
} PadSnapshot_t;


uint32_t GetButtonsState();

void PrintButtons(uint32_t buttons_state);

void PAD_Init(void);
void PAD_TakeSnapshot(PadSnapshot_t* snapshot);


#endif /* SOURCES_SRC_PADCONTROL_H_ */
