/*
 * ADC.c
 *
 *  Created on: 27 paz 2026
 *      Author: tomasz
 */

#include "ADC.h"

#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#include <stm32f446xx.h>

#include "TimeoutTracker.h"
#include "BoardFunctions.h"
#include "BoardAssert.h"
#include "Common_Defs.h"

//----------------
// PRIVATE MACROS
//----------------
#define ADC_SAMPLE_BUFFER_SIZE      (32)
_Static_assert(ADC_SAMPLE_BUFFER_SIZE >= ADC_SEQUENCE_ORDER_SIZE, "ADC_SAMPLE_BUFFER_SIZE lower than ADC_SEQUENCE_ORDER_SIZE, can't be that.");

#define ADC_PROFILES_SIZE			(4)

#define ADC_SQR_SINGLE_CONVERSION_NUMOF_BITS    (5)
#define ADC_SQR_SINGLE_CONVERSION_MASK          (0x1F)
#define ADC_SQR_SINGLE_CONVERSION_MAX_VALUE     (19)

#define ADC_SQR3_NUMOF_CONVERSIONS      (6)
#define ADC_SQR2_NUMOF_CONVERSIONS      (6)
#define ADC_SQR1_NUMOF_CONVERSIONS      (4)

#define ADC_SMPR2_NUMOF_CHANNELS        (10)
#define ADC_SMPR1_NUMOF_CHANNELS        (9)

#define ADC_SMPR_SINGLE_CHANNEL_NUMOF_BITS    (3)
#define ADC_SMPR_SINGLE_CHANNEL_MASK          (0x07)
#define ADC_SMPR_SINGLE_CHANNEL_MAX_VALUE     (7)

#define ADC_END_OF_CONVERSION_TIMEOUT_US    (250)

//----------------
// PRIVATE TYPES
//----------------


//----------------
// PRIVATE VARIABLES
//----------------
static ADC_Profile	s_profiles[ADC_PROFILES_SIZE];
static bool         s_profileCreatedFlag[ADC_PROFILES_SIZE];
static unsigned     s_currentProfile; 
static uint16_t     s_sampleBuffer[ADC_SAMPLE_BUFFER_SIZE];

// Refer to RM
static const uint8_t s_sampleTimeLUT[ADC_SAMPLE_TIME_NUMOF] = {
    [ADC_SAMPLE_TIME_3_CYCLES]      = 0,
    [ADC_SAMPLE_TIME_15_CYCLES]     = 1,
    [ADC_SAMPLE_TIME_28_CYCLES]     = 2,
    [ADC_SAMPLE_TIME_56_CYCLES]     = 3,
    [ADC_SAMPLE_TIME_84_CYCLES]     = 4,
    [ADC_SAMPLE_TIME_112_CYCLES]    = 5,
    [ADC_SAMPLE_TIME_144_CYCLES]    = 6,
    [ADC_SAMPLE_TIME_480_CYCLES]    = 7,
};

// Refer to RM
static const uint8_t s_resolutionLUT[ADC_RESOLUTION_NUMOF] = {
    [ADC_RESOLUTION_12BIT] = 0,
    [ADC_RESOLUTION_10BIT] = 1,
    [ADC_RESOLUTION_8BIT]  = 2,
    [ADC_RESOLUTION_6BIT]  = 3,
};


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void ADC_ValidateResolution(const ADC_Profile* profile);
static void ADC_ConfigureResolution(const ADC_Profile* profile);

static void ADC_ValidateRegularSequence(const ADC_Profile* profile);
static void ADC_ConfigureRegularSequence(const ADC_Profile* profile);

static void ADC_ValidateSampleTime(const ADC_Profile* profile);
static void ADC_ConfigureSampleTime(const ADC_Profile* profile);


//----------------
// PUBLIC FUNCTIONS
//----------------
void ADC_Init(void)
{
    for (int i = 0; i < (int)ARRAY_SIZE(s_profileCreatedFlag); i++)
    {
        s_profileCreatedFlag[i] = false;
    }
    fast_memset(s_profiles, 0, sizeof(s_profiles));
    s_currentProfile = UINT_MAX; // Absurdly high
    fast_memset(s_sampleBuffer, 0, sizeof(s_sampleBuffer));
    
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // ADC1 clock enable
    ADC1->CR2 |= ADC_CR2_ADON;  // A/D Converter ON
    delayUS(1);

    ADC1->CR2 |= ADC_CR2_EOCS; // EOC bit is set at the end of each regular conversion
}

// Return code: -1 - error, 0 - ok
int ADC_CreateProfile(const ADC_Profile* profile, unsigned* handle)
{
    BOARD_ASSERT(profile != NULL);
    BOARD_ASSERT(handle != NULL);

    // Find profile index for new profile
    int profileIndex = -1;
    for (int i = 0; i < (int)ARRAY_SIZE(s_profileCreatedFlag); i++)
    {
        if (!s_profileCreatedFlag[i]) {
            s_profileCreatedFlag[i] = true;
            profileIndex = i;
            break;
        }
    }
    if (profileIndex == -1) return -1;

    // Validate profile
    ADC_ValidateResolution(profile);
    ADC_ValidateRegularSequence(profile);
    ADC_ValidateSampleTime(profile);

    // Save as new profile
    s_profiles[profileIndex] = *profile;

    // Return handle
    *handle = profileIndex;

    return 0;
}

// Return code: -1 - error, 0 - ok
int ADC_ConfigureProfile(const unsigned handle)
{
    BOARD_ASSERT(handle < ADC_PROFILES_SIZE);

    // Already configured
    if (s_currentProfile == handle) return 0;

    // Profile not created
    if (!s_profileCreatedFlag[handle]) return -1;

    s_currentProfile = handle;
    ADC1->CR1 |= ADC_CR1_SCAN; // Scan mode
    ADC_ConfigureResolution(&s_profiles[s_currentProfile]);
    ADC_ConfigureRegularSequence(&s_profiles[s_currentProfile]);
    ADC_ConfigureSampleTime(&s_profiles[s_currentProfile]);

    return 0;
}

int ADC_Sample(uint16_t* samplesBuf, unsigned size)
{
    BOARD_ASSERT(samplesBuf != NULL && size > 0);
    BOARD_ASSERT(s_currentProfile < ADC_PROFILES_SIZE); // No profile was configured

    int retry = 3;
    while (retry > 0)
    {
        ADC1->SR = ~ADC_SR_OVR; // Clear overrun
        (void)ADC1->DR; // Clear old EOC

        ADC1->CR2 |= ADC_CR2_SWSTART; // Start conversion

        bool failed = false;
        for (int i = 0; i < s_profiles[s_currentProfile].sequenceLen; i++)
        {
            // Wait until conversion is finished
            TimeoutTracker_t timer = timeout_start_us(ADC_END_OF_CONVERSION_TIMEOUT_US);
            while (!(ADC1->SR & ADC_SR_EOC)) {
                if (timeout_has_expired(&timer)) {
                    failed = true;
                    break;
                }
            }
            if (failed) break;

            // Overrun bit set, conversion failed
            if (ADC1->SR & ADC_SR_OVR) {
                failed = true;
                break;
            } 

            // Read sample
            s_sampleBuffer[i] = ADC1->DR;
        }

        // Conversion succeed
        if (!failed) 
        {
            for (int i = 0; i < (int)size && i < s_profiles[s_currentProfile].sequenceLen; i++)
            {
                samplesBuf[i] = s_sampleBuffer[i];
            }
            break;
        }
        else {
            retry--;
        }
    }

    return (retry > 0) ? 0 : -1;
}



//----------------
// PRIVATE FUNCTIONS
//----------------
static void ADC_ValidateResolution(const ADC_Profile* profile)
{
    BOARD_ASSERT(profile->resolution < ADC_RESOLUTION_NUMOF);
}

static void ADC_ConfigureResolution(const ADC_Profile* profile)
{
    uint8_t resolution = s_resolutionLUT[profile->resolution];
	SET_FIELD_32(ADC1->CR1, ADC_CR1_RES_Msk , ADC_CR1_RES_Pos, resolution);
}

static void ADC_ValidateRegularSequence(const ADC_Profile* profile)
{
    BOARD_ASSERT(profile->sequenceLen > 0 && profile->sequenceLen <= ADC_SEQUENCE_ORDER_SIZE);

    for (int i = 0; i < profile->sequenceLen; i++)
    {
        BOARD_ASSERT(profile->sequenceOrder[i] < ADC_SQR_SINGLE_CONVERSION_MAX_VALUE);
    }
}    

static void ADC_ConfigureRegularSequence(const ADC_Profile* profile)
{
    // Configure regular sequence of conversions
	SET_FIELD_32(ADC1->SQR1, ADC_SQR1_L_Msk , ADC_SQR1_L_Pos, profile->sequenceLen - 1); // Sequence length

    int conversionIndex = 0;

    for (int i = 0; i < ADC_SQR3_NUMOF_CONVERSIONS; i++)
    {
        if (conversionIndex >= profile->sequenceLen) break;

        const uint8_t conversionOrder = profile->sequenceOrder[conversionIndex];

        const uint32_t pos = ADC_SQR_SINGLE_CONVERSION_NUMOF_BITS * i;
        const uint32_t mask = (ADC_SQR_SINGLE_CONVERSION_MASK << pos);
        SET_FIELD_32(ADC1->SQR3, mask, pos, conversionOrder); // Sequence order
        conversionIndex++;
    }

    for (int i = 0; i < ADC_SQR2_NUMOF_CONVERSIONS; i++)
    {
        if (conversionIndex >= profile->sequenceLen) break;

        const uint8_t conversionOrder = profile->sequenceOrder[conversionIndex];

        const uint32_t pos = ADC_SQR_SINGLE_CONVERSION_NUMOF_BITS * i;
        const uint32_t mask = (ADC_SQR_SINGLE_CONVERSION_MASK << pos);
        SET_FIELD_32(ADC1->SQR2, mask, pos, conversionOrder); // Sequence order
        conversionIndex++;
    }

    for (int i = 0; i < ADC_SQR1_NUMOF_CONVERSIONS; i++)
    {
        if (conversionIndex >= profile->sequenceLen) break;

        const uint8_t conversionOrder = profile->sequenceOrder[conversionIndex];

        const uint32_t pos = ADC_SQR_SINGLE_CONVERSION_NUMOF_BITS * i;
        const uint32_t mask = (ADC_SQR_SINGLE_CONVERSION_MASK << pos);
        SET_FIELD_32(ADC1->SQR1, mask, pos, conversionOrder); // Sequence order
        conversionIndex++;
    }
}

static void ADC_ValidateSampleTime(const ADC_Profile* profile)
{
    for (int i = 0; i < ADC_NUMOF_CHANNELS; i++)
    {
        BOARD_ASSERT(profile->sampleTime[i] < ADC_SAMPLE_TIME_NUMOF);
    }
}

static void ADC_ConfigureSampleTime(const ADC_Profile* profile)
{
    for (int i = 0; i < ADC_NUMOF_CHANNELS; i++)
    {
        const uint8_t sampleTime = s_sampleTimeLUT[profile->sampleTime[i]];

        if (i < ADC_SMPR2_NUMOF_CHANNELS)
        {
            const uint32_t pos = ADC_SMPR_SINGLE_CHANNEL_NUMOF_BITS * i;
            const uint32_t mask = (ADC_SMPR_SINGLE_CHANNEL_MASK << pos);
            SET_FIELD_32(ADC1->SMPR2, mask, pos, sampleTime);
        }
        else
        {
            const uint32_t pos = ADC_SMPR_SINGLE_CHANNEL_NUMOF_BITS * (i - ADC_SMPR2_NUMOF_CHANNELS);
            const uint32_t mask = (ADC_SMPR_SINGLE_CHANNEL_MASK << pos);
            SET_FIELD_32(ADC1->SMPR1, mask, pos, sampleTime);
        }
    }
}