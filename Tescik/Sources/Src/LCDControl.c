/*
 * LCDControl.c
 *
 *  Created on: 19 maj 2026
 *      Author: tomasz
 */


#include "LCDControl.h"

#include "NES_Functions.h"

#define FRAMEBUFFER_SIZE	(40*240)
uint16_t framebuffer[FRAMEBUFFER_SIZE];

// Kolory użyte w grafice (RGB565 MSB):
// 0x5DFF - Jasnoniebieski (Tło nieba)
// 0xD000 - Czerwony (Czapka i koszulka)
// 0x0210 - Niebieski (Ogrodniczki)
// 0xFCE0 - Beżowy / Skóra (Twarz i dłonie)
// 0x79E0 - Brązowy (Włosy i buty)

const uint16_t mario_16x16_dma[256] = {
		0xffff, 0xffff, 0xffff, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe0fd, 0xe0fd, 0xe314, 0xe0fd, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe0fd, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe0fd, 0xe314, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0x00f8, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0x00f8, 0xe314, 0xe314, 0x00f8, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff,
		0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff,
		0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0xe314, 0x00f8, 0xe0fd, 0x00f8, 0x00f8, 0xe0fd, 0x00f8, 0xe314, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0xe0fd, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xe0fd, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xe0fd, 0xe0fd, 0xe0fd, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xe0fd, 0xe0fd, 0xffff, 0xffff,
		0xffff, 0xffff, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0x00f8, 0x00f8, 0x00f8, 0x00f8, 0xffff, 0xffff, 0xffff,
		0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff,
		0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff, 0xffff, 0xffff, 0xe314, 0xe314, 0xe314, 0xe314, 0xe314, 0xffff
	};

void DMA2_SPI1_Send_NoBlock(uint8_t* buffer, uint16_t length)
{
    // 1. Sprawdź, czy poprzedni transfer DMA się zakończył
    // Jeśli DMA jeszcze wysyła, musimy poczekać (lub zabezpieczyć to wyżej w kodzie)
    while (DMA2_Stream3->CR & DMA_SxCR_EN);

    // 2. Wyczyszczenie flag zakończenia poprzedniego transferu dla Strumienia 3 (rejestr LIFCR)
    DMA2->LIFCR = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CTEIF3;

	__DSB();
    // 3. Ustawienie adresu źródłowego w RAM i liczby bajtów
    DMA2_Stream3->M0AR = (uint32_t)buffer;
    DMA2_Stream3->NDTR = length;

	LCD_DC_DATA();
	LCD_CS_LOW();


    // 4. Włączenie strumienia DMA - w tym momencie rusza ciągły zegar SPI na 36 MHz!
    DMA2_Stream3->CR |= DMA_SxCR_EN;

	while (!(DMA2->LISR & DMA_LISR_TCIF3));
	while (SPI1->SR & SPI_SR_BSY);

	// --- DODAJ TE LINIE TUTAJ, ABY UBIĆ SZUM ---
	volatile uint8_t dummy_dr = *(volatile uint8_t *)&SPI1->DR;
	volatile uint32_t dummy_sr = SPI1->SR;
	(void)dummy_dr;
	(void)dummy_sr; // Te 4 linijki czyszczą flagę RXNE oraz kasują błąd Overrun (OVR)
	// -------------------------------------------

	LCD_CS_HIGH();
}

uint8_t SPI1_Write(uint8_t data)
{
	while (!(SPI1->SR & SPI_SR_TXE)); // Czekaj na wolny bufor
	*(volatile uint8_t *)&SPI1->DR = data;

	while (!(SPI1->SR & SPI_SR_RXNE));    // Czekaj na odebranie ostatniego bitu
//	while ((SPI1->SR & SPI_SR_BSY));    // flaga busy
	return *((volatile uint8_t*)&SPI1->DR);
}

void SPI1_SendCmd_U8(uint8_t data)
{
	// Czyszczenie SPI na start
//	volatile uint32_t dummy_read;
//	dummy_read = SPI1->DR;
//	dummy_read = SPI1->SR;
//	(void)dummy_read;

	LCD_DC_CMD();
	LCD_CS_LOW();

	SPI1_Write(data);

	LCD_CS_HIGH();
	LCD_DC_DATA();
}

void SPI1_SendData_U8(uint8_t data)
{
	// Czyszczenie SPI na start
//	volatile uint32_t dummy_read;
//	dummy_read = SPI1->DR;
//	dummy_read = SPI1->SR;
//	(void)dummy_read;

	LCD_DC_DATA();
	LCD_CS_LOW();

	SPI1_Write(data);

	LCD_CS_HIGH();
//	LCD_DC_DATA();
}

int SPI1_SendRead_U8(uint8_t data, uint8_t* output)
{
	if (output == NULL)	{ return -1; }

	// Czyszczenie SPI na start
	volatile uint32_t dummy_read;
	dummy_read = SPI1->DR;
	dummy_read = SPI1->SR;
	(void)dummy_read;

	LCD_DC_CMD();
	LCD_CS_LOW();
	SPI1_Write(data);

	LCD_DC_DATA();

	uint8_t retByte = SPI1_Write(0x00);

	LCD_CS_HIGH();
	LCD_DC_DATA();

	*output = retByte;

	return 0;
}

int SPI1_SendRead_U24(uint8_t data, uint8_t* output2, uint8_t* output1, uint8_t* output0)
{
	if (output2 == NULL || output1 == NULL || output0 == NULL)	{ return -1; }

	// Czyszczenie SPI na start
	volatile uint32_t dummy_read;
	dummy_read = SPI1->DR;
	dummy_read = SPI1->SR;
	(void)dummy_read;

	LCD_DC_CMD();
	LCD_CS_LOW();
	SPI1_Write(data);

	LCD_DC_DATA();

//	SPI1_Write(0x00); //dummy cycle
	uint8_t retByte2 = SPI1_Write(0x00);
	uint8_t retByte1 = SPI1_Write(0x00);
	uint8_t retByte0 = SPI1_Write(0x00);

	LCD_CS_HIGH();
	LCD_DC_DATA();

	*output2 = retByte2;
	*output1 = retByte1;
	*output0 = retByte0;

	return 0;
}

void LCD_init()
{
	printf_v("LCD Reset Start\n");
	LCD_RESET_LOW();
	delay(10);
	LCD_RESET_HIGH();
	delay(50);
	printf_v("LCD Reset End\n");

	printf_v("LCD Exit Sleep Start\n");
	SPI1_SendCmd_U8(LCD_SLPOUT); // Exit Sleep Mode
	delay(250);
	printf_v("LCD Exit Sleep End\n");


	LCD_WritePixelFormat();
	delay(1);
	LCD_WriteDisplayON();
	delay(200);

	LCD_ReadDisplayMADCTL();
	delay(1);
	LCD_WriteDisplayMADCTL();
	delay(2000);
	LCD_ReadDisplayMADCTL();


//	LCD_SetBackground(LCD_WHITE);
//	delay(1000);
//	LCD_SetBackground(LCD_GREEN);
}

void LCD_ReadDisplayMADCTL()
{
	printf_v("LCD LCD_ReadDisplayMADCTL Start\n");
	uint8_t madctl = 0;
	SPI1_SendRead_U8(LCD_RDDMADCTL, &madctl);
	printf_v("LCD LCD_ReadDisplayMADCTL End, bytes: 0x%x\n", madctl);
}

void LCD_WriteDisplayMADCTL()
{
	uint8_t madctl = 0b10100000;
	printf_v("LCD LCD_WriteDisplayMADCTL Start\n");
	SPI1_SendCmd_U8(LCD_MADCTL);
	SPI1_SendData_U8(madctl);
	printf_v("LCD LCD_WriteDisplayMADCTL End, bytes: 0x%x\n", madctl);
}

void LCD_ReadPixelFormat()
{
	printf_v("LCD Read PixelFormat Start\n");
	uint8_t pixelFormat = 0;
	SPI1_SendRead_U8(0x0C, &pixelFormat);
	printf_v("LCD Read PixelFormat End, bytes: 0x%x\n", pixelFormat);
}

void LCD_WritePixelFormat()
{
	uint8_t pixelFormat = 0x55;
	printf_v("LCD Write PixelFormat: 0x%x Start\n", pixelFormat);
	SPI1_SendCmd_U8(LCD_COLMOD);
	SPI1_SendData_U8(pixelFormat);
	printf_v("LCD Write PixelFormat End\n");
}

void LCD_WriteDisplayON()
{
	printf_v("LCD LCD_WriteDisplayON Start\n");
	SPI1_SendCmd_U8(LCD_DISPON);
	printf_v("LCD Write PixelFormat End\n");
}

void LCD_SoftwareReset()
{
	printf_v("LCD LCD_SoftwareReset Start\n");
	SPI1_SendCmd_U8(LCD_SWRESET);
	printf_v("LCD LCD_SoftwareReset End\n");
}

void LCD_SetRectToDraw(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	if (x > 319 || y > 239)
	{
		return;
	}

	if (x + w > 319 || y + h > 239)
	{
		return;
	}

	uint16_t x2 = x + w;
	uint16_t y2 = y + h;

	uint8_t msb = 0;
	uint8_t lsb = 0;


	SPI1_SendCmd_U8(LCD_CASET);

	msb = (uint8_t)(x >> 8);
	lsb = (uint8_t)(x & 0xFF);
	SPI1_SendData_U8(msb);
	SPI1_SendData_U8(lsb);

	msb = (uint8_t)(x2 >> 8);
	lsb = (uint8_t)(x2 & 0xFF);
	SPI1_SendData_U8(msb);
	SPI1_SendData_U8(lsb);


	SPI1_SendCmd_U8(LCD_RASET);

	msb = (uint8_t)(y >> 8);
	lsb = (uint8_t)(y & 0xFF);
	SPI1_SendData_U8(msb);
	SPI1_SendData_U8(lsb);

	msb = (uint8_t)(y2 >> 8);
	lsb = (uint8_t)(y2 & 0xFF);
	SPI1_SendData_U8(msb);
	SPI1_SendData_U8(lsb);
}

void LCD_SetBackground(LCD_Color color)
{
	printf_v("LCD LCD_SetGreenBackground Start\n");

	LCD_SetRectToDraw(0, 0, 319, 239);

	SPI1_SendCmd_U8(LCD_RAMWR);

	uint8_t msByte = 0, lsByte = 0;
	uint16_t twoBytes = 0;
	switch (color)
	{
	case LCD_GREEN:
	{
		twoBytes = 0xE007;
		break;
	}
	case LCD_WHITE:
	{
		twoBytes = 0xFFFF;
		break;
	}
	case LCD_RED:
	{
		twoBytes = 0xF800;
		break;
	}
	case LCD_BLUE:
	{
		twoBytes = 0x00F8;
		break;
	}
	}

	msByte = (uint8_t)(twoBytes >> 8);
	lsByte = (uint8_t)(twoBytes & 0xFF);


	if (false)
	{
		int maxLoop = 320*240;
		for (int i = 0; i < maxLoop; i++)
		{
			SPI1_SendData_U8(msByte);
			SPI1_SendData_U8(lsByte);
		}
	}
	else
	{
		for (int i = 0; i < FRAMEBUFFER_SIZE; i++)
		{
			framebuffer[i] = twoBytes;
		}

		for (int i = 0; i < 8; i++)
		{
			DMA2_SPI1_Send_NoBlock((uint8_t*)framebuffer, FRAMEBUFFER_SIZE*2);
//			DMA2_SPI1_Send_NoBlock((uint8_t*)framebuffer, 0x10*0x30*2);
//			DMA2_SPI1_Send_NoBlock((uint8_t*)mario_16x16_dma, 256*2);

		}
	}


	printf_v("LCD LCD_SetGreenBackground End\n");
}

void LCD_DrawMario()
{
	printf_v("LCD LCD_DrawMario Start\n");

	LCD_SetRectToDraw(0, 0, 15, 15);

	SPI1_SendCmd_U8(LCD_RAMWR);

	DMA2_SPI1_Send_NoBlock((uint8_t*)mario_16x16_dma, 256*2);

	printf_v("LCD LCD_DrawMario End\n");
}



void LCD_ReadID3()
{
	printf_v("LCD Read ID3 Start\n");
	uint8_t ID3 = 0;
	SPI1_SendRead_U8(0xDC, &ID3);
	printf_v("LCD Read ID3 End, ID3: 0x%x\n", ID3);
}

void LCD_ReadID2()
{
	printf_v("LCD Read ID2 Start\n");
	uint8_t ID2 = 0;
	SPI1_SendRead_U8(0xDB, &ID2);
	printf_v("LCD Read ID2 End, ID2: 0x%x\n", ID2);
}

void LCD_ReadID1()
{
	printf_v("LCD Read ID1 Start\n");
	uint8_t ID1 = 0;
	SPI1_SendRead_U8(0xDA, &ID1);
	printf_v("LCD Read ID1 End, ID1: 0x%x\n", ID1);
}

void LCD_ReadCTRLDisplay()
{
	printf_v("LCD_ReadCTRLDisplay Start\n");
	uint8_t ctrl = 0;
	SPI1_SendRead_U8(0x54, &ctrl);
	printf_v("LCD_ReadCTRLDisplay End, bytes: 0x%x\n", ctrl);
}

void LCD_WriteCTRLDisplay()
{
	uint8_t ctrl = 0b00101100;
	printf_v("LCD_WriteCTRLDisplay: 0x%x Start\n", ctrl);
	SPI1_SendCmd_U8(0x53);
	SPI1_SendData_U8(ctrl);
	printf_v("LCD_WriteCTRLDisplay End\n");
}
