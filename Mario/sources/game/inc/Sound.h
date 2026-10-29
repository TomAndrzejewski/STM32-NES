/*
 * Sound.h
 *
 *  Created on: 6 sie 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_SOUND_H_
#define SOURCES_INC_SOUND_H_

#include <stdint.h>

	
#define SOUND_BUFFER_SAMPLES			(512)
#define HALF_SOUND_BUFFER_SAMPLES		(SOUND_BUFFER_SAMPLES / 2)

#define SOUND_BUFFER_SIZE				(SOUND_BUFFER_SAMPLES * 2)
#define HALF_SOUND_BUFFER_SIZE			(SOUND_BUFFER_SIZE / 2)

#define SOUND_BUFFER_FIRST_HALF			(0)
#define SOUND_BUFFER_SECOND_HALF		(1)


#define SOUND_SAMPLE_RATE_HZ			(16000)
#define SOUND_SAMPLE_RATE_KHZ			(16)

#define SOUND_SQUARE_WAVE_AMPLITUDE		(4000)

#define SOUND_VOLUME_MIN				(0)
#define SOUND_VOLUME_MAX				(4)
#define SOUND_VOLUME_INC				(1)
#define SOUND_VOLUME_STEPS				((SOUND_VOLUME_MAX - SOUND_VOLUME_MIN) / SOUND_VOLUME_INC)


typedef struct {
	float		phase;		// current phase, [0.0...1.0]
	float		phaseStep;
	int16_t		amplitude;	// aplitude of square wave

} SquareWaveOscillator_t;

typedef struct {
	uint16_t	freq;		// Hz
	uint16_t	time_ms;

} NoteAsset_t;

typedef struct {
	int samplesToPlay;
	int sampleNumber;

} SoundNote_t;


int SOUND_Init(void);
int16_t* SOUND_GetBufferPtr(void);

void I2S_Start(void);
void I2S_Stop(void);


#endif /* SOURCES_INC_SOUND_H_ */
