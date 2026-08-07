/*
 * Sound.c
 *
 *  Created on: 6 sie 2026
 *      Author: tomasz
 */

#include <string.h>

#include "Level.h"

#include "Sound.h"


static int16_t _SoundBuffer[SOUND_BUFFER_SIZE] = {0};
static int	_SoundBufferHalf = SOUND_BUFFER_FIRST_HALF;

static SoundNote_t _CurrNote = {0};

static SquareWaveOscillator_t _SquareOsc = {0};


//------------------------------
// Note Synthesizer
//------------------------------
int SOUND_Init()
{
	int ret = SOUND_SetFirstSoundNote();
	return ret;
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
		SOUND_SynthSamples(_SoundBuffer + HALF_SOUND_BUFFER_SIZE, HALF_SOUND_BUFFER_SIZE);
	}
	else if (bufferHalf == SOUND_BUFFER_SECOND_HALF)
	{
		SOUND_SynthSamples(_SoundBuffer, HALF_SOUND_BUFFER_SIZE);
	}

	return 0;
}

int SOUND_SetFirstSoundNote()
{
	int ret = SOUND_SetNextSoundNote(&_CurrNote);
	return ret;
}

int SOUND_SetNextSoundNote(SoundNote_t* note)
{
	if (note == NULL) return -1;

	// Configure next sound note
	LEVEL_GetNextSoundNote(&note->asset);

	note->sampleNumber = 0;
	note->samplesToPlay = SOUND_SAMPLE_RATE_KHZ * note->asset.time_ms;

	// Configure oscillator
	SOUND_SetAmplitude(&_SquareOsc, SOUND_SQUARE_WAVE_AMPLITUDE);
	SOUND_SetPhaseStep(&_SquareOsc, note->asset.freq, SOUND_SAMPLE_RATE_HZ);

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
		samplesBuf[i] = sample;

		// Increase sample and check if note has finished
		_CurrNote.sampleNumber++;
		if (_CurrNote.sampleNumber > _CurrNote.samplesToPlay)
		{
			SOUND_SetNextSoundNote(&_CurrNote);
		}
	}

	return 0;
}

//------------------------------
// Square Wave Oscillator
//------------------------------
int SOUND_SetAmplitude(SquareWaveOscillator_t* osc, int16_t amplitude)
{
	if (osc == NULL) return -1;
	osc->amplitude = amplitude;
	return 0;
}

int SOUND_SetPhaseStep(SquareWaveOscillator_t* osc, uint16_t freq, uint16_t sampleRate)
{
	if (osc == NULL || sampleRate == 0)	return -1;
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
