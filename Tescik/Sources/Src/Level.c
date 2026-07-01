/*
 * Level.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include "string.h"

#include "Game_Types.h"

#include "Level.h"


const ObjectLevelPos_t BGObjectsLevelPosition[] = {
	    { .id = BG_DWOJKA_OBJECT_ID,	.x = 96,	.y = 208 },
	    { .id = BG_CHMURKA_OBJECT_ID,	.x = 192,	.y = 208 },
	    { .id = BG_JEDYNKA_OBJECT_ID,	.x = 336,	.y = 208 },
	    { .id = BG_DWOJKA_OBJECT_ID, 	.x = 304,	.y = 144 },
	    { .id = BG_CHMURKA_OBJECT_ID, 	.x = 96,	.y = 128 },
	    { .id = BG_CHMURKA_OBJECT_ID, 	.x = 176,	.y = 96 },
	    { .id = BG_JEDYNKA_OBJECT_ID, 	.x = 272,	.y = 80 },
	    { .id = BG_JEDYNKA_OBJECT_ID, 	.x = 64,	.y = 32 }
};


const ObjectLevelPos_t EnemiesLevelPosition[] = {
	    { .id = ENEMY_GOOMBA_ID,		.x = 40,	.y = 40 },
};


int LEVEL_GetBGObjectsLocations(const ObjectLevelPos_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(ObjectLevelPos_t);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(BGObjectsLevelPosition)/elementSize;
		*posTable = BGObjectsLevelPosition;
		return 0;
	}
	else
	{
		return -5;
	}
}

int LEVEL_GetEnemiesLocations(const ObjectLevelPos_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(ObjectLevelPos_t);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(EnemiesLevelPosition)/elementSize;
		*posTable = EnemiesLevelPosition;
		return 0;
	}
	else
	{
		return -5;
	}
}
