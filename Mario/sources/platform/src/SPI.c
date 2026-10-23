/*
 * SPI.c
 *
 *  Created on: 22 paz 2026
 *      Author: tomasz
 */

#include "SPI.h"

#include "TimeoutTracker.h"
#include "Common_Defs.h"

#include "stm32f446xx.h"
#include "stm32f4xx_ll_gpio.h"


#define DMA_SPI_TIMEOUT_MS	(10)

#define LCD_CS_LOW()		(GPIOA->BSRR = GPIO_BSRR_BR4)
#define LCD_CS_HIGH()		(GPIOA->BSRR = GPIO_BSRR_BS4)


static volatile bool s_dmaDone = true;
static volatile uint32_t s_dmaStartTimestamp = 0;
static volatile uint32_t s_dmaTransferTime = 0;


static void IRQ_DMA2_SPI1_TX_Init(void);
static void DMA2_SPI1_TX_Init(void);
static uint8_t SPI_Write(uint8_t data);


void SPI_Init(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // GPIOA clock enable
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;  // SPI1 clock enable

	/////////////////////////////////
	// LCD_CS
	// PA4 pull-up output
	GPIOA->BSRR = GPIO_BSRR_BS4; // init as high
	SET_FIELD_32(GPIOA->PUPDR, GPIO_PUPDR_PUPD4_Msk, GPIO_PUPDR_PUPD4_Pos, LL_GPIO_PULL_UP); //pull-up
	SET_FIELD_32(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED4_Msk, GPIO_OSPEEDR_OSPEED4_Pos, LL_GPIO_SPEED_FREQ_VERY_HIGH); //fast speed
	SET_FIELD_32(GPIOA->OTYPER, GPIO_OTYPER_OT4_Msk, GPIO_OTYPER_OT4_Pos, LL_GPIO_OUTPUT_PUSHPULL); //push-pull
	SET_FIELD_32(GPIOA->MODER, GPIO_MODER_MODER4_Msk, GPIO_MODER_MODER4_Pos, LL_GPIO_MODE_OUTPUT); //output

	/////////////////////////////////
	// LCD_SCK
	// PA5 AF SPI1_SCK
	SET_FIELD_32(GPIOA->PUPDR, GPIO_PUPDR_PUPD5_Msk, GPIO_PUPDR_PUPD5_Pos, LL_GPIO_PULL_DOWN); //pull-dow
	SET_FIELD_32(GPIOA->OTYPER, GPIO_OTYPER_OT5_Msk, GPIO_OTYPER_OT5_Pos, LL_GPIO_OUTPUT_PUSHPULL); //push-pull
	SET_FIELD_32(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5_Msk, GPIO_OSPEEDR_OSPEED5_Pos, LL_GPIO_SPEED_FREQ_HIGH); //fast speed
	SET_FIELD_32(GPIOA->AFR[0], GPIO_AFRL_AFSEL5_Msk, GPIO_AFRL_AFSEL5_Pos, LL_GPIO_AF_5); //AF5 - SPI1_SCK
	SET_FIELD_32(GPIOA->MODER, GPIO_MODER_MODER5_Msk, GPIO_MODER_MODER5_Pos, LL_GPIO_MODE_ALTERNATE); //alternate function

	/////////////////////////////////
	// LCD_MISO
	// PA6 AF SPI1_MISO
	SET_FIELD_32(GPIOA->PUPDR, GPIO_PUPDR_PUPD6_Msk, GPIO_PUPDR_PUPD6_Pos, LL_GPIO_PULL_NO); //pull-down
	SET_FIELD_32(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED6_Msk, GPIO_OSPEEDR_OSPEED6_Pos, LL_GPIO_SPEED_FREQ_HIGH); //fast speed
	SET_FIELD_32(GPIOA->AFR[0], GPIO_AFRL_AFSEL6_Msk, GPIO_AFRL_AFSEL6_Pos, LL_GPIO_AF_5); //AF5 - SPI1_MISO
	SET_FIELD_32(GPIOA->MODER, GPIO_MODER_MODER6_Msk, GPIO_MODER_MODER6_Pos, LL_GPIO_MODE_ALTERNATE); //alternate function

	/////////////////////////////////
	// LCD_MOSI
	// PA7 AF SPI1_MOSI
	SET_FIELD_32(GPIOA->PUPDR, GPIO_PUPDR_PUPD7_Msk, GPIO_PUPDR_PUPD7_Pos, LL_GPIO_PULL_NO); //pull-down
	SET_FIELD_32(GPIOA->OTYPER, GPIO_OTYPER_OT7_Msk, GPIO_OTYPER_OT7_Pos, LL_GPIO_OUTPUT_PUSHPULL); //push-pull
	SET_FIELD_32(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED7_Msk, GPIO_OSPEEDR_OSPEED7_Pos, LL_GPIO_SPEED_FREQ_HIGH); //fast speed
	SET_FIELD_32(GPIOA->AFR[0], GPIO_AFRL_AFSEL7_Msk, GPIO_AFRL_AFSEL7_Pos, LL_GPIO_AF_5); //AF5 - SPI1_MOSI
	SET_FIELD_32(GPIOA->MODER, GPIO_MODER_MODER7_Msk, GPIO_MODER_MODER7_Pos, LL_GPIO_MODE_ALTERNATE); //alternate function

	/////////////////////////////////
	// LCD_SPI CONFIG
	SET_FIELD_32(SPI1->CR1, SPI_CR1_BIDIMODE_Msk, SPI_CR1_BIDIMODE_Pos, 0); // full duplex
	SET_FIELD_32(SPI1->CR1, SPI_CR1_DFF_Msk, SPI_CR1_DFF_Pos, 0); // 8-bit data transfer
	SET_FIELD_32(SPI1->CR1, SPI_CR1_SSM_Msk, SPI_CR1_SSM_Pos, 1); // manual CS
	SET_FIELD_32(SPI1->CR1, SPI_CR1_SSI_Msk, SPI_CR1_SSI_Pos, 1); // internal NSS pull to master
	SET_FIELD_32(SPI1->CR1, SPI_CR1_BR_Msk, SPI_CR1_BR_Pos, 0b000); // APB2 clock div 128
	SET_FIELD_32(SPI1->CR1, SPI_CR1_MSTR_Msk, SPI_CR1_MSTR_Pos, 1); // spi master mode
	SET_FIELD_32(SPI1->CR1, SPI_CR1_CPOL_Msk, SPI_CR1_CPOL_Pos, 0); // SCK line low in idle
	SET_FIELD_32(SPI1->CR1, SPI_CR1_CPHA_Msk, SPI_CR1_CPHA_Pos, 0); // SCK phase 1 edge
	SET_FIELD_32(SPI1->CR1, SPI_CR1_SPE_Msk, SPI_CR1_SPE_Pos, 1); // SPI1 enable

	IRQ_DMA2_SPI1_TX_Init();
	DMA2_SPI1_TX_Init();
}

void SPI_StartTransaction(void)
{
    TimeoutTracker_t timer = timeout_start_ms(DMA_SPI_TIMEOUT_MS);
	while (!s_dmaDone) {
		if (timeout_has_expired(&timer)) {
			break;
		}
	}

	// Czyszczenie SPI na start
	volatile uint32_t dummy_read;
	dummy_read = SPI1->DR;
	dummy_read = SPI1->SR;
	(void)dummy_read;

	//todotomka decide what strategy I use here, bailing out or fighting?
	LCD_CS_LOW();
}

void SPI_EndTransaction(void)
{
	LCD_CS_HIGH();
}

uint8_t SPI_Send_U8(uint8_t data)
{
	return SPI_Write(data);
}

void DMA2_SPI1_PrepareSend_NoBlock(uint8_t* buffer, uint16_t length)
{
    TimeoutTracker_t timer = timeout_start_ms(DMA_SPI_TIMEOUT_MS);
	while (!s_dmaDone) {
		if (timeout_has_expired(&timer)) {
			break;
		}
	}
	if (timeout_has_expired(&timer)) { // Bail out!!!
		return;
	}
	
	s_dmaDone = false;
    // 1. Sprawdź, czy poprzedni transfer DMA się zakończył
    // Jeśli DMA jeszcze wysyła, musimy poczekać (lub zabezpieczyć to wyżej w kodzie)
    timer = timeout_start_ms(DMA_SPI_TIMEOUT_MS);
	while (DMA2_Stream3->CR & DMA_SxCR_EN) {
		if (timeout_has_expired(&timer)) {
			break;
		}
	}

    s_dmaStartTimestamp = GetTimestamp();

    // 2. Wyczyszczenie flag zakończenia poprzedniego transferu dla Strumienia 3 (rejestr LIFCR)
    DMA2->LIFCR = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CTEIF3;

	__DSB();
    // 3. Ustawienie adresu źródłowego w RAM i liczby bajtów
    DMA2_Stream3->M0AR = (uint32_t)buffer;
    DMA2_Stream3->NDTR = length;
}

void DMA2_SPI1_Send_NoBlock()
{
	LCD_CS_LOW();

    // 4. Włączenie strumienia DMA - w tym momencie rusza ciągły zegar SPI na 36 MHz!
    DMA2_Stream3->CR |= DMA_SxCR_EN;
}

void DMA2_Stream3_IRQHandler(void)
{
    if (DMA2->LISR & DMA_LISR_TCIF3)
    {
        // wyczyść flagę Transfer Complete
        DMA2->LIFCR = DMA_LIFCR_CTCIF3;

        TimeoutTracker_t timer = timeout_start_ms(DMA_SPI_TIMEOUT_MS);
    	while (SPI1->SR & SPI_SR_BSY) {
    		if (timeout_has_expired(&timer)) {
    			break;
    		}
    	}

    	// --- DODAJ TE LINIE TUTAJ, ABY UBIĆ SZUM ---
    	volatile uint8_t dummy_dr = *(volatile uint8_t *)&SPI1->DR;
    	volatile uint32_t dummy_sr = SPI1->SR;
    	(void)dummy_dr;
    	(void)dummy_sr; // Te 4 linijki czyszczą flagę RXNE oraz kasują błąd Overrun (OVR)
    	// -------------------------------------------

    	LCD_CS_HIGH();

        s_dmaDone = true;

        s_dmaTransferTime = CalcTimeUS(s_dmaStartTimestamp);
//        printf_uint(s_dmaTransferTime); printf_c('\t');
    }
}

static void IRQ_DMA2_SPI1_TX_Init(void)
{
	NVIC_SetPriority(DMA2_Stream3_IRQn, 10); // not too high not too low :)
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

static void DMA2_SPI1_TX_Init(void)
{
    // 1. Włączenie zegara dla DMA2
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // Upewniamy się, że strumień jest wyłączony przed konfiguracją
    DMA2_Stream3->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream3->CR & DMA_SxCR_EN); // Czekaj aż faktycznie się wyłączy

    // 2. Czyszczenie rejestru konfiguracyjnego (CR) i reset flag błędów/transferu
    DMA2_Stream3->CR = 0;
    DMA2->LIFCR = DMA_LIFCR_CTCIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CFEIF3;

    // 3. Konfiguracja parametrów strumienia w rejestrze CR:
    // - Wybór Kanału 3 (CHSEL = 011b -> shift o 25)
    DMA2_Stream3->CR |= (3U << DMA_SxCR_CHSEL_Pos);

    // - Priorytet wysoki (PL = 10b -> shift o 16)
    DMA2_Stream3->CR |= (2U << DMA_SxCR_PL_Pos);

    // - Rozmiar danych pamięci (MSIZE) = 8-bit (00b)
    // - Rozmiar danych peryferium (PSIZE) = 8-bit (00b)
    DMA2_Stream3->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);

    // - Inkrementacja pamięci włączona (MINC = 1)
    DMA2_Stream3->CR |= DMA_SxCR_MINC;

    // - Inkrementacja peryferium wyłączona (PINC = 0) - domyślnie czysta

    // - Kierunek: Pamięć do Peryferium (DIR = 01b -> shift o 6)
    DMA2_Stream3->CR |= (1U << DMA_SxCR_DIR_Pos);

    // 4. Konfiguracja adresów stałych
    // Adres docelowy to rejestr danych SPI1
    DMA2_Stream3->PAR = (uint32_t)&(SPI1->DR);

    // 5. Włączenie żądania DMA ze strony SPI1 (SPI TX DMA Enable)
    SPI1->CR2 |= SPI_CR2_TXDMAEN;

    // 6. Włącz przerwanie od zakończenia transferu
    DMA2_Stream3->CR |= DMA_SxCR_TCIE;
}

static uint8_t SPI_Write(uint8_t data)
{
    TimeoutTracker_t timer = timeout_start_ms(DMA_SPI_TIMEOUT_MS);
	while (!s_dmaDone) {
		if (timeout_has_expired(&timer)) {
			break;
		}
	}

	timer = timeout_start_ms(DMA_SPI_TIMEOUT_MS);
	while (!(SPI1->SR & SPI_SR_TXE)) {	// Zakonczono wysylanie
		if (timeout_has_expired(&timer)) {
			break;
		}
	}

	*(volatile uint8_t *)&SPI1->DR = data;

	timer = timeout_start_ms(DMA_SPI_TIMEOUT_MS);
	while (!(SPI1->SR & SPI_SR_RXNE)) { // Czekaj na odebranie ostatniego bitu
		if (timeout_has_expired(&timer)) {
			break;
		}
	}

	return *((volatile uint8_t*)&SPI1->DR);
}
