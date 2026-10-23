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

#include "Geometry.h"


#define LCD_HEIGHT		(240)
#define LCD_WIDTH		(320)
#define LCD_SIZE		(LCD_HEIGHT * LCD_WIDTH)


void LCD_init(void);
void LCD_ReadAvailableRegisters(void);

void LCD_WriteVertScrollDef(void);
void LCD_WriteVertScrollStartAddr(uint16_t startAddr);
void LCD_DrawRect(Rect_t baseRect, int LCDOffsetX);

void LCD_SendRect_DMA(uint8_t* buffer, uint16_t length);


#endif /* SOURCES_INC_LCDCONTROL_H_ */
