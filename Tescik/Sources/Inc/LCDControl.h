/*
 * LCDControl.h
 *
 *  Created on: 19 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_LCDCONTROL_H_
#define SOURCES_INC_LCDCONTROL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <stm32f446xx.h>
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_spi.h"

#include "NES_Defs.h"

#include "printf_logger.h"


#define LCD_SLPOUT		(0x11)
#define LCD_COLMOD		(0x3A)
#define LCD_DISPON		(0x29)
#define LCD_CASET		(0x2A)
#define LCD_RASET		(0x2B)
#define LCD_RAMWR		(0x2C)
#define LCD_SWRESET		(0x01)
#define LCD_RDDMADCTL	(0x0B)
#define LCD_MADCTL		(0x36)


#define LCD_RESET_LOW()		(GPIOA->BSRR = GPIO_BSRR_BR0)
#define LCD_RESET_HIGH()	(GPIOA->BSRR = GPIO_BSRR_BS0)
#define LCD_DC_CMD()		(GPIOA->BSRR = GPIO_BSRR_BR1)
#define LCD_DC_DATA()		(GPIOA->BSRR = GPIO_BSRR_BS1)
#define LCD_CS_LOW()		(GPIOA->BSRR = GPIO_BSRR_BR4)
#define LCD_CS_HIGH()		(GPIOA->BSRR = GPIO_BSRR_BS4)


typedef enum
{
	LCD_WHITE,
	LCD_BLUE,
	LCD_RED,
	LCD_GREEN,
}LCD_Color;

void LCD_init();

void LCD_ReadID3();
void LCD_ReadID2();
void LCD_ReadID1();
void LCD_ReadPixelFormat();
void LCD_WritePixelFormat();
void LCD_ReadCTRLDisplay();
void LCD_WriteCTRLDisplay();
void LCD_WriteDisplayON();
void LCD_SetBackground(LCD_Color color);
void LCD_SoftwareReset();
void LCD_ReadDisplayMADCTL();
void LCD_WriteDisplayMADCTL();
void LCD_DrawMario(uint16_t x, uint16_t y);

#endif /* SOURCES_INC_LCDCONTROL_H_ */
