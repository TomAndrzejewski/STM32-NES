/*
 * PADControl.c
 *
 *  Created on: 31 maj 2026
 *      Author: tomasz
 */


#include <string.h>
#include <stdio.h>

#include "stm32f4xx_ll_gpio.h"

#include "NES_Functions.h"
#include "printf_logger.h"

#include "PADControl.h"


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
