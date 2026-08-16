/*
 * Sound.c
 *
 *  Created on: 6 sie 2026
 *      Author: tomasz
 */

#include <string.h>

#include "NES_Functions.h"
#include "printf_logger.h"

#include "Level.h"

#include "Sound.h"

#include <stm32f446xx.h>
#include "stm32f4xx_ll_gpio.h"



static int16_t _SoundBuffer[SOUND_BUFFER_SIZE] = {0};
static int _SoundBufferHalf = SOUND_BUFFER_FIRST_HALF;

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
//		DMA1_Stream4->CR |= DMA_SxCR_EN; // start new transfer

		if (_SoundBufferHalf == SOUND_BUFFER_FIRST_HALF) {
			_SoundBufferHalf = SOUND_BUFFER_SECOND_HALF;
			printf_uint(ts);
			printf_c('\t');
			printf_uint(0);
			printf_c('\n');
		}
	} else if (DMA1->HISR & DMA_HISR_HTIF4) {
		DMA1->HIFCR = DMA_HIFCR_CHTIF4; // clear half transfer complete flag

		if (_SoundBufferHalf == SOUND_BUFFER_SECOND_HALF) {
			_SoundBufferHalf = SOUND_BUFFER_FIRST_HALF;
			printf_uint(ts);
			printf_c('\t');
			printf_uint(1);
			printf_c('\n');
		}
	}

	SOUND_Update(_SoundBufferHalf);
}

int SOUND_Init()
{
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

	osc->phase = 0.0f;
	osc->phaseStep = (float)freq/(float)sampleRate;

	return 0;
}

OPTIMIZE_FOR_DEBUG
int16_t SOUND_GetSample(SquareWaveOscillator_t* osc)
{
	if (osc == NULL) return 0;
	int16_t sample = 0;

	// Get sample
	if (osc->phase < 0.5f) {
		sample = osc->amplitude;
	} else {
		sample = -osc->amplitude;
	}

	// Move phase
	osc->phase += osc->phaseStep;

	// Rotate phase
	if (osc->phase >= 1.0f) {
		osc->phase -= 1.0f;
	}

	return sample;
}
