/*
 * Enemies.c
 *
 *  Created on: 28 cze 2026
 *      Author: tomasz
 */

#include "string.h"

#include "Enemies.h"


int Enemies_Init(Enemies_t* p)
{
	if (p == NULL)	{ return -1; }

	for (int i = 0; i < NUM_OF_GOOMBAS; i++)
	{
		int ret = Goomba_Init(&p->goombas[i], Goombas_InitMapPos[i]);
		if (ret < 0)	{ return -5; }
	}

	return 0;
}

int Enemies_GetGoomba(Enemies_t* p, int idx, Goomba_t** goomba)
{
	if (p == NULL || goomba == NULL)	{ return -1; }
	if (idx < 0 || idx >= NUM_OF_GOOMBAS)	{ return -5; }

	*goomba = &p->goombas[idx];
	return 0;
}
