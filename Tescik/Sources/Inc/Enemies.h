/*
 * Enemies.h
 *
 *  Created on: 28 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_ENEMIES_H_
#define SOURCES_INC_ENEMIES_H_

#include "Goomba.h"


#define NUM_OF_GOOMBAS		(2)

static const Point_t Goombas_InitMapPos[NUM_OF_GOOMBAS] = {
		{ .x = 100, .y = 32 },
		{ .x = 104, .y = 50 },
};

typedef struct
{
	Goomba_t	goombas[NUM_OF_GOOMBAS];

}Enemies_t;


int 		Enemies_Init(Enemies_t* p);

int 		Enemies_GetGoomba(Enemies_t* p, int idx, Goomba_t** goomba);

#endif /* SOURCES_INC_ENEMIES_H_ */
