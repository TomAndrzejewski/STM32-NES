/*
 * BatteryControl.c
 *
 *  Created on: 27 paz 2026
 *      Author: tomasz
 */

#include "BatteryControl.h"

#include <stddef.h>
#include <stdbool.h>

#include <stm32f446xx.h>
#include <stm32f4xx_ll_gpio.h>

#include "TimeoutTracker.h"
#include "BoardFunctions.h"
#include "BoardAssert.h"
#include "Common_Defs.h"


//----------------
// PRIVATE MACROS
//----------------
#define BATTERY_ADC_CHANNEL                 (8)    // PB0 = ADC1_IN8 (Nucleo A3)

#define BATTERY_ADC_PRESCALER_DIV4          (1)    // ADCCLK = PCLK2 / 4 = 18 MHz
#define BATTERY_ADC_SAMPLE_TIME_480_CYCLES  (7)
#define BATTERY_ADC_RESOLUTION_12BIT        (0)

#define BATTERY_ADC_STAB_DELAY_US           (3)
#define BATTERY_ADC_CONVERSION_TIMEOUT_US   (100)


//----------------
// PRIVATE TYPES
//----------------



//----------------
// PRIVATE VARIABLES
//----------------


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void BATTERY_ADCInit(void);


//----------------
// PUBLIC FUNCTIONS
//----------------
void BATTERY_Init(void)
{
    BATTERY_ADCInit();
}

// Return code: -1 - error, 0 - ok
int BATTERY_GetVoltage(uint16_t* voltage)
{
    BOARD_ASSERT(voltage != NULL);

    (void)ADC1->DR; // Clear EOC left by an earlier timed-out conversion

    ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion

    // Wait until conversion is finished
    TimeoutTracker_t timer = timeout_start_us(BATTERY_ADC_CONVERSION_TIMEOUT_US);
    while (!(ADC1->SR & ADC_SR_EOC)) {
        if (timeout_has_expired(&timer)) return -1;
    }

    uint16_t analog = (uint16_t)ADC1->DR;
    uint32_t tmp = analog * 3300;
    *voltage = tmp / 4096; 

    return 0;
}


//----------------
// PRIVATE FUNCTIONS
//----------------
static void BATTERY_ADCInit(void)
{
    // PB0 analog input, no pull
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // GPIOB clock enable
    SET_FIELD_32(GPIOB->PUPDR, GPIO_PUPDR_PUPD0_Msk, GPIO_PUPDR_PUPD0_Pos, LL_GPIO_PULL_NO); //no pull
    SET_FIELD_32(GPIOB->MODER, GPIO_MODER_MODER0_Msk, GPIO_MODER_MODER0_Pos, LL_GPIO_MODE_ANALOG); //analog

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // ADC1 clock enable
    SET_FIELD_32(ADC123_COMMON->CCR, ADC_CCR_ADCPRE_Msk, ADC_CCR_ADCPRE_Pos, BATTERY_ADC_PRESCALER_DIV4);

    // Single conversion of one regular channel, right-aligned 12-bit result
    SET_FIELD_32(ADC1->CR1, ADC_CR1_RES_Msk, ADC_CR1_RES_Pos, BATTERY_ADC_RESOLUTION_12BIT);
    ADC1->CR1 &= ~ADC_CR1_SCAN;
    ADC1->CR2 &= ~(ADC_CR2_CONT | ADC_CR2_ALIGN);

    // Regular sequence: one conversion of the battery channel
    SET_FIELD_32(ADC1->SQR1, ADC_SQR1_L_Msk, ADC_SQR1_L_Pos, 0);
    SET_FIELD_32(ADC1->SQR3, ADC_SQR3_SQ1_Msk, ADC_SQR3_SQ1_Pos, BATTERY_ADC_CHANNEL);
    SET_FIELD_32(ADC1->SMPR2, ADC_SMPR2_SMP8_Msk, ADC_SMPR2_SMP8_Pos, BATTERY_ADC_SAMPLE_TIME_480_CYCLES);

    ADC1->CR2 |= ADC_CR2_ADON; // A/D Converter ON
    delayUS(BATTERY_ADC_STAB_DELAY_US);
}
