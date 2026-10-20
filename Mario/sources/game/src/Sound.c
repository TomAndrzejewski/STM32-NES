/*
 * Sound.c
 *
 *  Created on: 6 sie 2026
 *      Author: tomasz
 */

#include "Sound.h"

#include <string.h>
#include <stdint.h>

#include <stm32f446xx.h>

#include "Common_Defs.h"
#include "Level.h"


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



static int16_t s_soundBuffer[SOUND_BUFFER_SIZE];
static int s_soundBufferHalf = SOUND_BUFFER_FIRST_HALF;

static SoundNote_t s_currNote = {0};

static SquareWaveOscillator_t s_squareOsc = {0};

// Volume buttons are wired to GPIOB inputs and read as low when pressed
#define SOUND_MUTE_BUTTON_PIN			(GPIO_IDR_ID2)
#define SOUND_UNMUTE_BUTTON_PIN			(GPIO_IDR_ID1)

static int16_t s_volumeGain = SOUND_VOLUME_MIN;
static uint32_t s_prevVolumeButtons = 0;


static void SOUND_Irq(void);
static void SOUND_HandleVolumeButtons(void);

static int SOUND_Update(int bufferHalf);
static int SOUND_SynthSamples(int16_t samplesBuf[], int samplesToSynth);

static int SOUND_SetFirstSoundNote(void);
static int SOUND_SetNextSoundNote(SoundNote_t* noteToSet);

static int SOUND_ConfigureOscillator(SquareWaveOscillator_t* osc, int16_t amplitude, uint16_t freq, uint16_t sampleRate);
static int16_t SOUND_GetSample(SquareWaveOscillator_t* osc);


void DMA1_Stream7_IRQHandler(void)
{
	SOUND_Irq();
}

int SOUND_Init(void)
{
	memset(&s_soundBuffer, 0, sizeof(s_soundBuffer));
	int ret = SOUND_SetFirstSoundNote();
	return ret;
}

int16_t* SOUND_GetBufferPtr(void)
{
	return s_soundBuffer;
}

// static volatile uint32_t dma_prevStartTimestamp = 0;

static void SOUND_Irq(void)
{
	// uint32_t ts = CalcTimeUS(dma_prevStartTimestamp);
	// dma_prevStartTimestamp = GetTimestamp();

	if (DMA1->HISR & DMA_HISR_TCIF7) {
		DMA1->HIFCR = DMA_HIFCR_CTCIF7; // clear transfer complete flag

		if (s_soundBufferHalf == SOUND_BUFFER_FIRST_HALF) {
			s_soundBufferHalf = SOUND_BUFFER_SECOND_HALF;
//			printf_uint(ts);
//			printf_c('\t');
//			printf_uint(0);
//			printf_c('\n');
		}
	} else if (DMA1->HISR & DMA_HISR_HTIF7) {
		DMA1->HIFCR = DMA_HIFCR_CHTIF7; // clear half transfer complete flag

		if (s_soundBufferHalf == SOUND_BUFFER_SECOND_HALF) {
			s_soundBufferHalf = SOUND_BUFFER_FIRST_HALF;
//			printf_uint(ts);
//			printf_c('\t');
//			printf_uint(1);
//			printf_c('\n');
		}
	}

	SOUND_HandleVolumeButtons();
	SOUND_Update(s_soundBufferHalf);	
}

static void SOUND_HandleVolumeButtons(void)
{
	// Invert the active low inputs and keep only the buttons pressed since the previous check
	uint32_t buttons = ~GPIOB->IDR & (SOUND_MUTE_BUTTON_PIN | SOUND_UNMUTE_BUTTON_PIN);
	uint32_t pressed = buttons & ~s_prevVolumeButtons;
	s_prevVolumeButtons = buttons;

	if (pressed & SOUND_MUTE_BUTTON_PIN)
	{
		s_volumeGain = SOUND_VOLUME_MIN;
	}
	if (pressed & SOUND_UNMUTE_BUTTON_PIN)
	{
		s_volumeGain = SOUND_VOLUME_MAX;
	}
}

static int SOUND_Update(int bufferHalf)
{
	if (bufferHalf == SOUND_BUFFER_FIRST_HALF)
	{
		SOUND_SynthSamples(s_soundBuffer, HALF_SOUND_BUFFER_SAMPLES);
	}
	else if (bufferHalf == SOUND_BUFFER_SECOND_HALF)
	{
		SOUND_SynthSamples(s_soundBuffer + HALF_SOUND_BUFFER_SIZE, HALF_SOUND_BUFFER_SAMPLES);
	}

	return 0;
}

//------------------------------
// Note Synthesizer
//------------------------------

static int SOUND_SetFirstSoundNote(void)
{
	int ret = SOUND_SetNextSoundNote(&s_currNote);
	return ret;
}

static int SOUND_SetNextSoundNote(SoundNote_t* noteToSet)
{
	if (noteToSet == NULL) return -1;

	// Configure next sound note
	const NoteAsset_t* asset = NULL;
	LEVEL_GetNextSoundNote(&asset);
	if (asset == NULL) return -2;

	noteToSet->sampleNumber = 0;
	noteToSet->samplesToPlay = SOUND_SAMPLE_RATE_KHZ * asset->time_ms;

	// Configure oscillator
	SOUND_ConfigureOscillator(&s_squareOsc, SOUND_SQUARE_WAVE_AMPLITUDE, asset->freq, SOUND_SAMPLE_RATE_HZ);

	return 0;
}

OPTIMIZE_FOR_DEBUG
static int SOUND_SynthSamples(int16_t samplesBuf[], int samplesToSynth)
{
	if (samplesBuf == NULL) return -1;

	for (int i = 0; i < samplesToSynth; i++)
	{
		// Trigger oscillator
		int16_t sample = SOUND_GetSample(&s_squareOsc);
		sample = (int16_t)(sample * s_volumeGain);
		samplesBuf[2 * i] = sample;
		samplesBuf[2 * i + 1] = sample;

		// Increase sample and check if note has finished
		s_currNote.sampleNumber++;
		if (s_currNote.sampleNumber >= s_currNote.samplesToPlay)
		{
			SOUND_SetNextSoundNote(&s_currNote);
		}
	}

	return 0;
}

//------------------------------
// Square Wave Oscillator
//------------------------------
OPTIMIZE_FOR_DEBUG
static int SOUND_ConfigureOscillator(SquareWaveOscillator_t* osc, int16_t amplitude, uint16_t freq, uint16_t sampleRate)
{
	if (osc == NULL || sampleRate == 0)	return -1;

	osc->amplitude = amplitude;
	osc->phaseStep = (float)freq/(float)sampleRate;
	return 0;
}

OPTIMIZE_FOR_DEBUG
static int16_t SOUND_GetSample(SquareWaveOscillator_t* osc)
{
	if (osc == NULL) return 0;
	int16_t sample = 0;

	// Get sample
	sample = get_sine_p360(osc->phase);
	sample = sample/32;
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
