/*
 * Sound.c
 *
 *  Created on: 6 sie 2026
 *      Author: tomasz
 */

#include <string.h>

#include "NES_Defs.h"
#include "NES_Functions.h"
#include "printf_logger.h"

#include "Level.h"

#include "Sound.h"

#include <stm32f446xx.h>
#include "stm32f4xx_ll_gpio.h"



#include <stdint.h>

#define SINE_LUT_SIZE 100

// Tablica LUT przechowująca 1/4 okresu (od 0 do 90 stopni)
// Skalowanie: 0 do 32767 (max int16_t)
static const int16_t SINE_LUT[SINE_LUT_SIZE + 1] = {
        0,   514,  1028,  1542,  2056,  2569,  3082,  3593,  4103,  4611,
     5117,  5621,  6123,  6623,  7120,  7614,  8105,  8593,  9077,  9557,
    10033, 10505, 10972, 11435, 11893, 12346, 12794, 13236, 13673, 14103,
    14527, 14945, 15356, 15760, 16157, 16547, 16930, 17305, 17672, 18031,
    18381, 18723, 19056, 19380, 19695, 20000, 20296, 20583, 20859, 21126,
    21382, 21628, 21864, 22089, 22303, 22506, 22698, 22879, 23048, 23206,
    23352, 23486, 23608, 23719, 23817, 23903, 23977, 24039, 24088, 24125,
    24150, 24162, 24162, 24149, 24124, 24087, 24037, 23975, 23900, 23813,
    23714, 23603, 23480, 23345, 23198, 23039, 22869, 22687, 22493, 22288,
    22071, 21843, 21603, 21352, 21089, 20815, 20530, 20234, 19927, 19609,
    19280
};

// Pomocnicza funkcja Odczytu z symetrią (dla pełnych 360 st / idx od 0 do 400)
static inline int16_t get_sine_lut_sym(int32_t idx)
{
    // Cwiartka 1 (0-90 deg): [0..100] -> LUT[idx]
    if (idx <= 100) {
        return SINE_LUT[idx];
    }
    // Cwiartka 2 (90-180 deg): [101..200] -> LUT[200 - idx]
    if (idx <= 200) {
        return SINE_LUT[200 - idx];
    }
    // Cwiartka 3 (180-270 deg): [201..300] -> -LUT[idx - 200]
    if (idx <= 300) {
        return -SINE_LUT[idx - 200];
    }
    // Cwiartka 4 (270-360 deg): [301..399] -> -LUT[400 - idx]
    return -SINE_LUT[400 - idx];
}

int16_t get_sine_p360(float phase)
{
    // Zawijanie fazy do przedziału [0.0, 1.0)
    while (phase >= 1.0f) phase -= 1.0f;
    while (phase < 0.0f)  phase += 1.0f;

    // Skalowanie 0.0 - 1.0 na zakres 0.0 - 400.0 (4 ćwiartki po 100 próbek)
    float pos = phase * 400.0f;

    int32_t idx = (int32_t)pos;             // Indeks bazowy
    float frac = pos - (float)idx;          // Część ułamkowa do interpolacji

    int16_t y0 = get_sine_lut_sym(idx);
    int16_t y1 = get_sine_lut_sym(idx + 1);

    // Interpolacja liniowa: y0 + frac * (y1 - y0)
    return y0 + (int16_t)(frac * (float)(y1 - y0));
}



static int16_t _SoundBuffer[SOUND_BUFFER_SIZE];
static int _SoundBufferHalf = SOUND_BUFFER_FIRST_HALF ;

static SoundNote_t _CurrNote = {0};

static SquareWaveOscillator_t _SquareOsc = {0};


//------------------------------
// Note Synthesizer
//------------------------------
static volatile uint32_t dma_prevStartTimestamp = 0;

void DMA1_Stream4_IRQHandler(void)
{
	uint32_t ts = CalcTimeUS(dma_prevStartTimestamp);
	dma_prevStartTimestamp = GetTimestamp();

	if (DMA1->HISR & DMA_HISR_TCIF4) {
		DMA1->HIFCR = DMA_HIFCR_CTCIF4; // clear transfer complete flag

		if (_SoundBufferHalf == SOUND_BUFFER_FIRST_HALF) {
			_SoundBufferHalf = SOUND_BUFFER_SECOND_HALF;
//			printf_uint(ts);
//			printf_c('\t');
//			printf_uint(0);
//			printf_c('\n');
		}
	} else if (DMA1->HISR & DMA_HISR_HTIF4) {
		DMA1->HIFCR = DMA_HIFCR_CHTIF4; // clear half transfer complete flag

		if (_SoundBufferHalf == SOUND_BUFFER_SECOND_HALF) {
			_SoundBufferHalf = SOUND_BUFFER_FIRST_HALF;
//			printf_uint(ts);
//			printf_c('\t');
//			printf_uint(1);
//			printf_c('\n');
		}
	}

	SOUND_Update(_SoundBufferHalf);
}

void DMA1_Stream7_IRQHandler(void)
{
	uint32_t ts = CalcTimeUS(dma_prevStartTimestamp);
	dma_prevStartTimestamp = GetTimestamp();

	if (DMA1->HISR & DMA_HISR_TCIF7) {
		DMA1->HIFCR = DMA_HIFCR_CTCIF7; // clear transfer complete flag

		if (_SoundBufferHalf == SOUND_BUFFER_FIRST_HALF) {
			_SoundBufferHalf = SOUND_BUFFER_SECOND_HALF;
//			printf_uint(ts);
//			printf_c('\t');
//			printf_uint(0);
//			printf_c('\n');
		}
	} else if (DMA1->HISR & DMA_HISR_HTIF7) {
		DMA1->HIFCR = DMA_HIFCR_CHTIF7; // clear half transfer complete flag

		if (_SoundBufferHalf == SOUND_BUFFER_SECOND_HALF) {
			_SoundBufferHalf = SOUND_BUFFER_FIRST_HALF;
//			printf_uint(ts);
//			printf_c('\t');
//			printf_uint(1);
//			printf_c('\n');
		}
	}

	SOUND_Update(_SoundBufferHalf);
}

int SOUND_Init()
{
	memset(&_SoundBuffer, 0, sizeof(_SoundBuffer));
	int ret = SOUND_SetFirstSoundNote();
	return ret;
}

int16_t* SOUND_GetBufferPtr()
{
	return _SoundBuffer;
}

void SOUND_Irq()
{
	static int irq = 1;

	if (irq == 0 && _SoundBufferHalf == SOUND_BUFFER_SECOND_HALF)
	{
		irq = 1;
		_SoundBufferHalf = SOUND_BUFFER_FIRST_HALF;
	}
	else if (irq == 1 && _SoundBufferHalf == SOUND_BUFFER_FIRST_HALF)
	{
		irq = 0;
		_SoundBufferHalf = SOUND_BUFFER_SECOND_HALF;
	}

	SOUND_Update(_SoundBufferHalf);
}

int SOUND_Update(int bufferHalf)
{
	if (bufferHalf == SOUND_BUFFER_FIRST_HALF)
	{
		SOUND_SynthSamples(_SoundBuffer, HALF_SOUND_BUFFER_SAMPLES);
	}
	else if (bufferHalf == SOUND_BUFFER_SECOND_HALF)
	{
		SOUND_SynthSamples(_SoundBuffer + HALF_SOUND_BUFFER_SIZE, HALF_SOUND_BUFFER_SAMPLES);
	}

	return 0;
}

int SOUND_SetFirstSoundNote()
{
	int ret = SOUND_SetNextSoundNote(&_CurrNote);
	return ret;
}

int SOUND_SetNextSoundNote(SoundNote_t* noteToSet)
{
	if (noteToSet == NULL) return -1;

	// Configure next sound note
	const NoteAsset_t* asset = NULL;
	LEVEL_GetNextSoundNote(&asset);
	if (asset == NULL) return -2;

	noteToSet->sampleNumber = 0;
	noteToSet->samplesToPlay = SOUND_SAMPLE_RATE_KHZ * asset->time_ms;

	// Configure oscillator
	SOUND_ConfigureOscillator(&_SquareOsc, SOUND_SQUARE_WAVE_AMPLITUDE, asset->freq, SOUND_SAMPLE_RATE_HZ);

	return 0;
}

OPTIMIZE_FOR_DEBUG
int SOUND_SynthSamples(int16_t samplesBuf[], int samplesToSynth)
{
	if (samplesBuf == NULL) return -1;

	for (int i = 0; i < samplesToSynth; i++)
	{
		// Trigger oscillator
		int16_t sample = SOUND_GetSample(&_SquareOsc);
		samplesBuf[2 * i] = sample;
		samplesBuf[2 * i + 1] = sample;

		// Increase sample and check if note has finished
		_CurrNote.sampleNumber++;
		if (_CurrNote.sampleNumber >= _CurrNote.samplesToPlay)
		{
			SOUND_SetNextSoundNote(&_CurrNote);
		}
	}

	return 0;
}

//------------------------------
// Square Wave Oscillator
//------------------------------
OPTIMIZE_FOR_DEBUG
int SOUND_ConfigureOscillator(SquareWaveOscillator_t* osc, int16_t amplitude, uint16_t freq, uint16_t sampleRate)
{
	if (osc == NULL || sampleRate == 0)	return -1;

	osc->amplitude = amplitude;
	osc->phaseStep = (float)freq/(float)sampleRate;
	return 0;
}

OPTIMIZE_FOR_DEBUG
int16_t SOUND_GetSample(SquareWaveOscillator_t* osc)
{
	if (osc == NULL) return 0;
	int16_t sample = 0;

	// Get sample
	sample = get_sine_p360(osc->phase);
	sample = sample/2;
//	if (osc->phase < 0.5f) {
//		sample = osc->amplitude;
//	} else {
//		sample = -osc->amplitude;
//	}

	// Move phase
	osc->phase += osc->phaseStep;

	// Rotate phase
	if (osc->phase >= 1.0f) {
		osc->phase -= 1.0f;
	}

	return sample;
}
