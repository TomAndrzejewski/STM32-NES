/*
 * PADControl.c
 *
 *  Created on: 31 maj 2026
 *      Author: tomasz
 */

#include "PADControl.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "stm32f4xx_ll_gpio.h"

#include "BoardAssert.h"
#include "BoardDefs.h"
#include "BoardFunctions.h"
#include "printf_logger.h"


//----------------
// PRIVATE MACROS
//----------------
#define PAD_SAMPLE_RATE_HZ		(500UL)

// APB2 timers run at twice the APB2 clock, which equals SYSCLK with the APB2 prescaler at 2
#define PAD_TIMER_CLOCK_HZ		(SYSCLOCK_MHZ)
#define PAD_TIMER_TICK_HZ		(1000000UL)
#define PAD_TIMER_PSC			((PAD_TIMER_CLOCK_HZ / PAD_TIMER_TICK_HZ) - 1)
#define PAD_TIMER_ARR			((PAD_TIMER_TICK_HZ / PAD_SAMPLE_RATE_HZ) - 1)

#define PAD_TIMER_IRQ_PRIORITY	(12)


//----------------
// PRIVATE VARIABLES
//----------------
static volatile uint32_t s_currentButtons = 0;
static volatile uint32_t s_pressedButtons = 0;
static volatile uint32_t s_releasedButtons = 0;


//----------------
// PUBLIC FUNCTIONS
//----------------
uint32_t GetButtonsState()
{
	uint32_t buttons_state = 0;

	PAD_LATCH_HIGH();
	delayUS(2);
	PAD_LATCH_LOW();

	for (int i = 0; i < 8; i++)
	{
		if ((GPIOB->IDR & LL_GPIO_PIN_10) == 0)
		{
			buttons_state |= (1 << i);
		}

		PAD_CLK_HIGH();
		delayUS(2);
		PAD_CLK_LOW();
		delayUS(2);
	}

	return buttons_state;
}

void PrintButtons(uint32_t buttons_state)
{
	char txt[200];
	sprintf(txt, "Buttons pressed: ");
	if (buttons_state & PAD_BUTTON_A)
	{
		strcat(txt, "A, ");
	}
	if (buttons_state & PAD_BUTTON_B)
	{
		strcat(txt, "B, ");
	}
	if (buttons_state & PAD_BUTTON_SELECT)
	{
		strcat(txt, "Select, ");
	}
	if (buttons_state & PAD_BUTTON_START)
	{
		strcat(txt, "Start, ");
	}
	if (buttons_state & PAD_BUTTON_UP)
	{
		strcat(txt, "UP, ");
	}
	if (buttons_state & PAD_BUTTON_DOWN)
	{
		strcat(txt, "DOWN, ");
	}
	if (buttons_state & PAD_BUTTON_LEFT)
	{
		strcat(txt, "LEFT, ");
	}
	if (buttons_state & PAD_BUTTON_RIGHT)
	{
		strcat(txt, "RIGHT, ");
	}
	strcat(txt, "\n");
	if (buttons_state != 0)
	{
		printf_v("%s", txt);
	}
}

void PAD_Init(void)
{
	// Start from the real pad state so buttons held at boot do not produce a press edge
	s_currentButtons = GetButtonsState();
	s_pressedButtons = 0;
	s_releasedButtons = 0;

	RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
	(void)RCC->APB2ENR;

	// Periodic update event at the pad sampling rate
	TIM11->PSC = PAD_TIMER_PSC;
	TIM11->ARR = PAD_TIMER_ARR;

	// Load the shadow registers and drop the update flag raised by that load
	TIM11->EGR = TIM_EGR_UG;
	TIM11->SR = ~TIM_SR_UIF;

	NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, PAD_TIMER_IRQ_PRIORITY);
	NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

	TIM11->DIER = TIM_DIER_UIE;
	TIM11->CR1 |= TIM_CR1_CEN;
}

void PAD_TakeSnapshot(PadSnapshot_t* snapshot)
{
	BOARD_ASSERT(snapshot != NULL);

	// Copy and clear the collected edges without the sampling interrupt slipping in between
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	snapshot->current = s_currentButtons;
	snapshot->pressed = s_pressedButtons;
	snapshot->released = s_releasedButtons;

	s_pressedButtons = 0;
	s_releasedButtons = 0;

	__set_PRIMASK(primask);
}


//----------------
// INTERRUPT HANDLERS
//----------------
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
	if (TIM11->SR & TIM_SR_UIF)
	{
		TIM11->SR = ~TIM_SR_UIF;

		// Accumulate edges between snapshots so short taps are not lost
		uint32_t buttons = GetButtonsState();
		uint32_t changed = buttons ^ s_currentButtons;

		s_pressedButtons |= changed & buttons;
		s_releasedButtons |= changed & ~buttons;
		s_currentButtons = buttons;
	}
}
