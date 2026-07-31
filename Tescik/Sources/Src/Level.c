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
	{ENEMY_GOOMBA_ID, 160, 32, 0 },
    {ENEMY_GOOMBA_ID, 240, 32, 0 },
	{FG_BLOCK_QMARK_OBJECT_ID, 272, 80, 0x0000000F},
	{FG_BRICKS_OBJECT_ID, 336, 80, 0x0000000F},
	{FG_BLOCK_QMARK_OBJECT_ID, 352, 80, 0x0000000F},
	{FG_BRICKS_OBJECT_ID, 368, 80, 0x0000000F},
	{FG_BLOCK_QMARK_OBJECT_ID, 368, 144, 0x0000000F},
	{FG_BLOCK_QMARK_OBJECT_ID, 384, 80, 0x0000000F},
	{FG_BRICKS_OBJECT_ID, 400, 80, 0x0000000F},
	{FG_RURA_DOL_OBJECT_ID, 464, 32, 0x0000001C},
	{OBJECT_NOT_USED, 464, 48, 0x0000001D},
	{FG_RURA_DOL_OBJECT_ID, 640, 32, 0x0000001C},
	{OBJECT_NOT_USED, 640, 64, 0x0000001D},
	{FG_RURA_DOL_OBJECT_ID, 640, 48, 0x0000001C},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 752, 32, 0x00000015},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 768, 32, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 768, 48, 0x00000015},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 784, 48, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 784, 32, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 784, 64, 0x00000015},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 800, 32, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 800, 48, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 800, 80, 0x00000015},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 800, 64, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 816, 96, 0x0000001D},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 816, 80, 0x00000018},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 816, 64, 0x00000018},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 816, 48, 0x00000000},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 816, 48, 0x00000018},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 816, 32, 0x00000018},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 864, 32, 0x00000014},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 864, 48, 0x00000014},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 864, 80, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 864, 96, 0x00000015},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 864, 80, 0x00000014},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 864, 64, 0x00000014},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 880, 96, 0x00000019},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 880, 80, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 880, 64, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 880, 48, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 880, 32, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 896, 48, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 896, 80, 0x00000019},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 896, 64, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 896, 32, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 912, 48, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 912, 64, 0x00000019},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 912, 32, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 928, 32, 0x00000010},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 928, 48, 0x00000019},
	{FG_PYRAMID_BLOCK_OBJECT_ID, 944, 32, 0x00000019},
};

const RepObjectLevelPos_t BGRepObjectsLevelPosition[] = {
	    { .id = BG_REP_FLOOR_ID,		.x = 0,	.y = 0,	.mulX = LCD_WIDTH/16,	.mulY = 2 },
};

// look at line numbers, lower line number -> higher priority
const GameObjectID ObjRenderPriorities[] = {
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

int LEVEL_GetObjRenderPriorities(const GameObjectID** prioTable, int* numOfObjects)
{
	if (prioTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(GameObjectID);
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
