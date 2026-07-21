/*
 * Level.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include "string.h"

#include "Game_Types.h"
#include "NES_Defs.h"

#include "Level.h"


const Rect_t LevelBoundaries = {
		.p1.x = 0,
		.p1.y = 0,
		.p2.x = 6400,
		.p2.y = 240
};

const ObjectLevelPos_t FGObjectsLevelPosition[] = {
		{ .id = FG_BRICKS_OBJECT_ID,		.x = 240,	.y = 80 },
};

const ObjectLevelPos_t BGObjectsLevelPosition[] = {
	    { .id = BG_DWOJKA_OBJECT_ID,	.x = 96,	.y = 208 },
	    { .id = BG_CHMURKA_OBJECT_ID,	.x = 192,	.y = 208 },
	    { .id = BG_JEDYNKA_OBJECT_ID,	.x = 336,	.y = 208 },
	    { .id = BG_DWOJKA_OBJECT_ID, 	.x = 304,	.y = 144 },
	    { .id = BG_CHMURKA_OBJECT_ID, 	.x = 96,	.y = 128 },
	    { .id = BG_CHMURKA_OBJECT_ID, 	.x = 176,	.y = 96 },
	    { .id = BG_JEDYNKA_OBJECT_ID, 	.x = 272,	.y = 80 },
	    { .id = BG_JEDYNKA_OBJECT_ID, 	.x = 64,	.y = 32 },
	    { .id = BG_CHMURKA_OBJECT_ID,	.x = 800,	.y = 208 },
};


const ObjectLevelPos_t EnemiesLevelPosition[] = {
	    { .id = ENEMY_GOOMBA_ID,		.x = 0,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 30,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 60,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 90,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 120,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 150,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 180,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 210,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 240,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 270,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 300,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 330,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 360,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 390,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 420,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 450,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 480,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 510,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 540,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 570,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 600,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 630,	.y = 40 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 660,	.y = 40 },
};

const RepObjectLevelPos_t BGRepObjectsLevelPosition[] = {
	    { .id = BG_REP_FLOOR_ID,		.x = 0,	.y = 0,	.mulX = LCD_WIDTH/16,	.mulY = 2 },
};

// look at line numbers, lower line number -> higher priority
const int ObjRenderPriorities[] = {
		FG_BRICKS_OBJECT_ID,
		ENEMY_GOOMBA_ID,
		PLAYER_MARIO_ID,
};

int LEVEL_GetFGObjectsLocations(const ObjectLevelPos_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(ObjectLevelPos_t);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(FGObjectsLevelPosition)/elementSize;
		*posTable = FGObjectsLevelPosition;
		return 0;
	}
	else
	{
		return -5;
	}
}

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

int LEVEL_GetBGRepObjectsLocations(const RepObjectLevelPos_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(RepObjectLevelPos_t);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(BGRepObjectsLevelPosition)/elementSize;
		*posTable = BGRepObjectsLevelPosition;
		return 0;
	}
	else
	{
		return -5;
	}
}

int LEVEL_GetLevelBoundaries(const Rect_t** levelBoundaries)
{
	if (levelBoundaries == NULL) { return -1; }
	*levelBoundaries = &LevelBoundaries;
	return 0;
}

int LEVEL_GetObjRenderPriorities(const int** prioTable, int* numOfObjects)
{
	if (prioTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(int);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(ObjRenderPriorities)/elementSize;
		*prioTable = ObjRenderPriorities;
		return 0;
	}
	else
	{
		return -5;
	}
}
