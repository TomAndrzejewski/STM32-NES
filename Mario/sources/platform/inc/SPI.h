/*
 * SPI.h
 *
 *  Created on: 22 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_SPI_H_
#define SOURCES_INC_SPI_H_

#include <stdint.h>

void SPI_Init(void);

void SPI_StartTransaction(void);
void SPI_EndTransaction(void);

uint8_t SPI_Send_U8(uint8_t data);

void DMA2_SPI1_PrepareSend_NoBlock(uint8_t* buffer, uint16_t length);
void DMA2_SPI1_Send_NoBlock();


#endif /* SOURCES_INC_SPI_H_ */