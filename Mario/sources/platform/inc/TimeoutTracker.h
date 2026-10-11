/*
 * TimeoutTracker.h
 *
 *  Created on: 29 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_TIMEOUTTRACKER_H_
#define SOURCES_INC_TIMEOUTTRACKER_H_

#include <stdint.h>
#include <stdbool.h>

#include "NES_Functions.h"

// Struktura pilnująca czasu
typedef struct {
    uint32_t start_time;
    uint32_t timeout_ms;
} TimeoutTracker_t;

// Funkcja startująca odliczanie
static inline TimeoutTracker_t timeout_start_ms(uint32_t timeout_ms)
{
    TimeoutTracker_t t;
    t.start_time = GetTimestamp();
    t.timeout_ms = timeout_ms;
    return t;
}

// Funkcja sprawdzająca, czy czas minął
static inline bool timeout_has_expired(TimeoutTracker_t *t)
{
	if (t == NULL)	{ return true; }
	uint32_t time_spent = CalcTimeMS(t->start_time);
    return (time_spent > t->timeout_ms);
}

#endif /* SOURCES_INC_TIMEOUTTRACKER_H_ */
