/*
 * ADC.h
 *
 *  Created on: 27 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_ADC_H_
#define SOURCES_INC_ADC_H_

#include <stdint.h>


#define ADC_SEQUENCE_ORDER_SIZE		(16)
#define ADC_NUMOF_CHANNELS          (19)


typedef enum {
    ADC_RESOLUTION_12BIT,
    ADC_RESOLUTION_10BIT,
    ADC_RESOLUTION_8BIT,
    ADC_RESOLUTION_6BIT,

    ADC_RESOLUTION_NUMOF
} ADC_ResolutionEnum;

typedef enum {
    ADC_SAMPLE_TIME_3_CYCLES,
    ADC_SAMPLE_TIME_15_CYCLES,
    ADC_SAMPLE_TIME_28_CYCLES,
    ADC_SAMPLE_TIME_56_CYCLES,
    ADC_SAMPLE_TIME_84_CYCLES,
    ADC_SAMPLE_TIME_112_CYCLES,
    ADC_SAMPLE_TIME_144_CYCLES,
    ADC_SAMPLE_TIME_480_CYCLES,

    ADC_SAMPLE_TIME_NUMOF,
} ADC_SampleTimeEnum;

typedef struct {
    ADC_ResolutionEnum resolution;

    ADC_SampleTimeEnum sampleTime[ADC_NUMOF_CHANNELS]; // Watchout for default 3 cycles sample time when set to zero!

    uint8_t sequenceLen;
    uint8_t sequenceOrder[ADC_SEQUENCE_ORDER_SIZE];
} ADC_Profile;


void ADC_Init(void);
int  ADC_CreateProfile(const ADC_Profile* profile, unsigned* handle);
// todotomka ADC_DeleteProfile()
int  ADC_ConfigureProfile(const unsigned handle);
int  ADC_Sample(uint16_t* samplesBuf, unsigned size);


#endif // SOURCES_INC_ADC_H_
