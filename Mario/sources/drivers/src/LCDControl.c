/*
 * LCDControl.c
 *
 *  Created on: 19 maj 2026
 *      Author: tomasz
 */

#include "LCDControl.h"

#include "BoardAssert.h"
#include "RenderEngine.h"
#include "BoardFunctions.h"
#include "printf_logger.h"

#include "SPI.h"

#include <stm32f446xx.h>


//----------------
// PRIVATE MACROS
//----------------
#define DMA_SPI_TIMEOUT_MS	(10)

#define LCD_SLPOUT		(0x11)
#define LCD_COLMOD		(0x3A)
#define LCD_DISPON		(0x29)
#define LCD_CASET		(0x2A)
#define LCD_RASET		(0x2B)
#define LCD_RAMWR		(0x2C)
#define LCD_SWRESET		(0x01)
#define LCD_RDDMADCTL	(0x0B)
#define LCD_MADCTL		(0x36)
#define LCD_VSCRDEF		(0x33)
#define LCD_VSCSAD		(0x37)

#define LCD_RESET_LOW()		(GPIOA->BSRR = GPIO_BSRR_BR0)
#define LCD_RESET_HIGH()	(GPIOA->BSRR = GPIO_BSRR_BS0)
#define LCD_DC_CMD()		(GPIOA->BSRR = GPIO_BSRR_BR1)
#define LCD_DC_DATA()		(GPIOA->BSRR = GPIO_BSRR_BS1)

#define LCD_COLOR_BLUESKY	(0x7F3C)

typedef enum
{
	LCD_WHITE,
	LCD_RED,
	LCD_BLUE,
	LCD_GREEN,

	LCD_NUMOF_COLORS,
}LCD_ColorEnum;

static uint16_t const LCD_Colors[LCD_NUMOF_COLORS] = {
		0xFFFE,
		0x00F8,
		0xF800,
		0xE007
};


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void LCD_SPI_SendCmd_U8(uint8_t cmd);
static void LCD_SPI_SendData_U8(uint8_t data);
static void LCD_SPI_SendRead_U8(uint8_t data, uint8_t* output);

static int LCD_SetRectToDraw(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
static void LCD_ReadDisplayMADCTL(void);
static void LCD_WriteDisplayMADCTL(void);
static void LCD_ReadPixelFormat(void);
static void LCD_WritePixelFormat(void);
static void LCD_WriteDisplayON(void);
static void LCD_ReadID3(void);
static void LCD_ReadID2(void);
static void LCD_ReadID1(void);



//----------------
// PUBLIC FUNCTIONS
//----------------
void LCD_init(void)
{
	SPI_Init();
	
	printf_v("LCD Reset Start\n");
	LCD_RESET_LOW();
	delay(10);
	LCD_RESET_HIGH();
	delay(50);
	printf_v("LCD Reset End\n");

	printf_v("LCD Exit Sleep Start\n");
	LCD_SPI_SendCmd_U8(LCD_SLPOUT); // Exit Sleep Mode
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
	
	LCD_WriteVertScrollDef();
	LCD_WriteVertScrollStartAddr(0);

	// not essential start
	delay(200);

	Rect_t screenRect;
	screenRect.p1.x = 0;
	screenRect.p1.y = 0;
	screenRect.p2.x = 319;
	screenRect.p2.y = 239;

	LCD_DrawRect(screenRect, 0);
	RE_RenderFullBackgroud(LCD_Colors[LCD_WHITE]);

	delay(1000);

	LCD_DrawRect(screenRect, 0);
	RE_RenderFullBackgroud(LCD_COLOR_BLUESKY);

	delay(1000);
	// not essential end
}

void LCD_SendRect_DMA(uint8_t* buffer, uint16_t length)
{
	DMA2_SPI1_PrepareSend_NoBlock(buffer, length);

	LCD_DC_DATA();

	DMA2_SPI1_Send_NoBlock();
}

void LCD_DrawRect(Rect_t baseRect, int LCDOffsetX)
{
	Rect_t lcdRect = baseRect;

	lcdRect.p1.x = (baseRect.p1.x + (LCD_WIDTH - LCDOffsetX)) % LCD_WIDTH;
	lcdRect.p2.x = (baseRect.p2.x + (LCD_WIDTH - LCDOffsetX)) % LCD_WIDTH;
	if (lcdRect.p1.x < 0 ||  lcdRect.p2.x < 0)
	{
		delay(1);
	}

	uint16_t x = lcdRect.p1.x;
	uint16_t y = lcdRect.p1.y;
	uint16_t w = lcdRect.p2.x-lcdRect.p1.x;
	uint16_t h = lcdRect.p2.y-lcdRect.p1.y;
	int ret = LCD_SetRectToDraw(x, y, w, h);
	if (ret < 0)
	{
		printf_v("SetRectToDraw err: %d %d %d %d (%d)\n", x, y, w, h, ret);
	}

	LCD_SPI_SendCmd_U8(LCD_RAMWR);
}

void LCD_WriteVertScrollDef(void)
{
	uint16_t tfa = 0;
	uint16_t vsa = 320;
	uint16_t bfa = 0;

	printf_v("LCD LCD_WriteVertScrollDef Start\n");

	LCD_SPI_SendCmd_U8(LCD_VSCRDEF);

	uint8_t msb = 0;
	uint8_t lsb = 0;

	msb = (uint8_t)(tfa >> 8);
	lsb = (uint8_t)(tfa & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);

	msb = (uint8_t)(vsa >> 8);
	lsb = (uint8_t)(vsa & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);

	msb = (uint8_t)(bfa >> 8);
	lsb = (uint8_t)(bfa & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);
}

void LCD_WriteVertScrollStartAddr(uint16_t startAddr)
{
	LCD_SPI_SendCmd_U8(LCD_VSCSAD);

	uint8_t msb = 0;
	uint8_t lsb = 0;

	msb = (uint8_t)(startAddr >> 8);
	lsb = (uint8_t)(startAddr & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);
}

void LCD_ReadAvailableRegisters(void)
{
	LCD_ReadDisplayMADCTL();
	LCD_ReadPixelFormat();
	LCD_ReadID1();
	LCD_ReadID2();
	LCD_ReadID3();
}

static void LCD_SPI_SendCmd_U8(uint8_t cmd)
{
	SPI_StartTransaction();

	LCD_DC_CMD();

	SPI_Send_U8(cmd);

	LCD_DC_DATA();

	SPI_EndTransaction();
}

static void LCD_SPI_SendData_U8(uint8_t data)
{
	SPI_StartTransaction();

	LCD_DC_DATA();

	SPI_Send_U8(data);

	SPI_EndTransaction();
}

static void LCD_SPI_SendRead_U8(uint8_t data, uint8_t* output)
{
	BOARD_ASSERT(output != NULL);

	SPI_StartTransaction();

	LCD_DC_CMD();

	SPI_Send_U8(data);

	LCD_DC_DATA();

	uint8_t retByte = SPI_Send_U8(0x00);

	LCD_DC_DATA();

	SPI_EndTransaction();

	*output = retByte;
}

static int LCD_SetRectToDraw(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	if (x > 319 || y > 239)
	{
		return -1;
	}

	if (x + w > 319 || y + h > 239)
	{
		return -5;
	}

	uint16_t x2 = x + w;
	uint16_t y2 = y + h;

	uint8_t msb = 0;
	uint8_t lsb = 0;


	LCD_SPI_SendCmd_U8(LCD_RASET);

	msb = (uint8_t)(x >> 8);
	lsb = (uint8_t)(x & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);

	msb = (uint8_t)(x2 >> 8);
	lsb = (uint8_t)(x2 & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);


	LCD_SPI_SendCmd_U8(LCD_CASET);

	msb = (uint8_t)(y >> 8);
	lsb = (uint8_t)(y & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);

	msb = (uint8_t)(y2 >> 8);
	lsb = (uint8_t)(y2 & 0xFF);
	LCD_SPI_SendData_U8(msb);
	LCD_SPI_SendData_U8(lsb);

	return 0;
}

static void LCD_ReadDisplayMADCTL(void)
{
	printf_v("LCD LCD_ReadDisplayMADCTL Start\n");
	uint8_t madctl = 0;
	LCD_SPI_SendRead_U8(LCD_RDDMADCTL, &madctl);
	printf_v("LCD LCD_ReadDisplayMADCTL End, bytes: 0x%x\n", madctl);
}

static void LCD_WriteDisplayMADCTL(void)
{
	uint8_t madctl = (uint8_t)0b11000000;
	printf_v("LCD LCD_WriteDisplayMADCTL Start\n");
	LCD_SPI_SendCmd_U8(LCD_MADCTL);
	LCD_SPI_SendData_U8(madctl);
	printf_v("LCD LCD_WriteDisplayMADCTL End, bytes: 0x%x\n", madctl);
}

static void LCD_ReadPixelFormat(void)
{
	printf_v("LCD Read PixelFormat Start\n");
	uint8_t pixelFormat = 0;
	LCD_SPI_SendRead_U8(0x0C, &pixelFormat);
	printf_v("LCD Read PixelFormat End, bytes: 0x%x\n", pixelFormat);
}

static void LCD_WritePixelFormat(void)
{
	uint8_t pixelFormat = 0x55;
	printf_v("LCD Write PixelFormat: 0x%x Start\n", pixelFormat);
	LCD_SPI_SendCmd_U8(LCD_COLMOD);
	LCD_SPI_SendData_U8(pixelFormat);
	printf_v("LCD Write PixelFormat End\n");
}

static void LCD_WriteDisplayON(void)
{
	printf_v("LCD LCD_WriteDisplayON Start\n");
	LCD_SPI_SendCmd_U8(LCD_DISPON);
	printf_v("LCD Write PixelFormat End\n");
}

static void LCD_ReadID3(void)
{
	printf_v("LCD Read ID3 Start\n");
	uint8_t ID3 = 0;
	LCD_SPI_SendRead_U8(0xDC, &ID3);
	printf_v("LCD Read ID3 End, ID3: 0x%x\n", ID3);
}

static void LCD_ReadID2(void)
{
	printf_v("LCD Read ID2 Start\n");
	uint8_t ID2 = 0;
	LCD_SPI_SendRead_U8(0xDB, &ID2);
	printf_v("LCD Read ID2 End, ID2: 0x%x\n", ID2);
}

static void LCD_ReadID1(void)
{
	printf_v("LCD Read ID1 Start\n");
	uint8_t ID1 = 0;
	LCD_SPI_SendRead_U8(0xDA, &ID1);
	printf_v("LCD Read ID1 End, ID1: 0x%x\n", ID1);
}

