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

const ObjectLevelInstance_t Objects_level_1[] =
{
    {FG_BLOCK_QMARK_OBJECT_ID, 272, 80, 0x0000000F},
    {FG_BRICKS_OBJECT_ID, 336, 80, 0x0000000F},
    {FG_BRICKS_OBJECT_ID, 368, 80, 0x0000000F},
    {FG_BRICKS_OBJECT_ID, 400, 80, 0x0000000F},
    {FG_BLOCK_QMARK_OBJECT_ID, 352, 80, 0x0000000F},
    {FG_BLOCK_QMARK_OBJECT_ID, 384, 80, 0x0000000F},
    {FG_BLOCK_QMARK_OBJECT_ID, 368, 144, 0x0000000F},
    {FG_RURA_DOL_OBJECT_ID, 464, 32, 0x0000001C},
    {FG_RURA_DOL_OBJECT_ID, 640, 32, 0x0000001C},
    {FG_RURA_GORA_OBJECT_ID, 464, 48, 0x0000001D},
    {FG_RURA_GORA_OBJECT_ID, 640, 64, 0x0000001D},
    {FG_RURA_DOL_OBJECT_ID, 640, 48, 0x0000001C},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 752, 32, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 768, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 784, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 784, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 768, 48, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 784, 64, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 800, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 800, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 800, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 800, 80, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 816, 96, 0x0000001D},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 816, 80, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 816, 64, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 816, 48, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 816, 48, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 816, 32, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 864, 32, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 864, 48, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 864, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 880, 96, 0x00000019},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 864, 96, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 864, 80, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 864, 64, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 880, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 880, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 896, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 880, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 896, 80, 0x00000019},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 896, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 912, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 912, 64, 0x00000019},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 912, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 896, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 880, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 928, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 928, 48, 0x00000019},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 944, 32, 0x00000019},
};


const ObjectLevelInstance_t EnemiesLevelPosition[] = {
	    { .id = ENEMY_GOOMBA_ID,		.x = 0,		.y = 40,	.flags = 0 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 30,	.y = 40,	.flags = 0 },
		{ .id = ENEMY_GOOMBA_ID,		.x = 60,	.y = 40,	.flags = 0 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 90,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 120,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 150,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 180,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 210,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 240,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 270,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 300,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 330,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 360,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 390,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 420,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 450,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 480,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 510,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 540,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 570,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 600,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 630,	.y = 40 },
//		{ .id = ENEMY_GOOMBA_ID,		.x = 660,	.y = 40 },
};

const RepObjectLevelPos_t BGRepObjectsLevelPosition[] = {
	    { .id = BG_REP_FLOOR_ID,		.x = 0,	.y = 0,	.mulX = LCD_WIDTH/16,	.mulY = 2 },
};

// look at line numbers, lower line number -> higher priority
const int ObjRenderPriorities[] = {
		FG_PYRAMID_BLOCK_OBJECT_ID,
		FG_RURA_DOL_OBJECT_ID,
		FG_RURA_GORA_OBJECT_ID,
		FG_BLOCK_QMARK_OBJECT_ID,
		FG_BRICKS_OBJECT_ID,
		ENEMY_GOOMBA_ID,
		PLAYER_MARIO_ID,
};

int LEVEL_GetObjectsLocations(const ObjectLevelInstance_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(ObjectLevelInstance_t);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(Objects_level_1)/elementSize;
		*posTable = Objects_level_1;
		return 0;
	}
	else
	{
		return -5;
	}
}

//int LEVEL_GetBGObjectsLocations(const ObjectLevelInstance_t** posTable, int* numOfObjects)
//{
//	if (posTable == NULL || numOfObjects == NULL)
//	{
//		return -1;
//	}
//
//	int elementSize = sizeof(ObjectLevelInstance_t);
//	if (elementSize > 0)
//	{
//		*numOfObjects = sizeof(BGObjectsLevelPosition)/elementSize;
//		*posTable = BGObjectsLevelPosition;
//		return 0;
//	}
//	else
//	{
//		return -5;
//	}
//}

int LEVEL_GetEnemiesLocations(const ObjectLevelInstance_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(ObjectLevelInstance_t);
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
