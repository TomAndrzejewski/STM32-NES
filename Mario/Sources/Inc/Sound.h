/*
 * Sound.h
 *
 *  Created on: 6 sie 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_SOUND_H_
#define SOURCES_INC_SOUND_H_

// #include "Game_Types.h"
#include <stdint.h>

#define SOUND_BUFFER_SAMPLES			(512)
#define HALF_SOUND_BUFFER_SAMPLES		(SOUND_BUFFER_SAMPLES / 2)

#define SOUND_BUFFER_SIZE				(SOUND_BUFFER_SAMPLES * 2)
#define HALF_SOUND_BUFFER_SIZE			(SOUND_BUFFER_SIZE / 2)

#define SOUND_BUFFER_FIRST_HALF			(0)
#define SOUND_BUFFER_SECOND_HALF		(1)


#define SOUND_SAMPLE_RATE_HZ			(16000)
#define SOUND_SAMPLE_RATE_KHZ			(16)

#define SOUND_SQUARE_WAVE_AMPLITUDE		(8000)


typedef struct
{
	float		phase;		// current phase, [0.0...1.0]
	float		phaseStep;
	int16_t		amplitude;	// aplitude of square wave

} SquareWaveOscillator_t;

typedef struct
{
	uint16_t	freq;		// Hz
	uint16_t	time_ms;

}NoteAsset_t;

typedef struct
{
	int samplesToPlay;
	int sampleNumber;

} SoundNote_t;


void	SOUND_Irq();
int16_t* SOUND_GetBufferPtr();

int 	SOUND_Init();
int 	SOUND_Update(int bufferHalf);
int 	SOUND_SetFirstSoundNote();

int 	SOUND_SynthSamples(int16_t samplesBuf[], int samplesToSynth);

int		SOUND_SetNote(const NoteAsset_t* note);
int 	SOUND_SetNextSoundNote(SoundNote_t* note);

int 	SOUND_ConfigureOscillator(SquareWaveOscillator_t* osc, int16_t amplitude, uint16_t freq, uint16_t sampleRate);

int16_t SOUND_GetSample(SquareWaveOscillator_t* osc);




#endif /* SOURCES_INC_SOUND_H_ */
