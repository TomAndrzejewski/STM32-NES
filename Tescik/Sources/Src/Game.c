/*
 * Game.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include "string.h"

#include "Game_Types.h"
#include "Assets.h"
#include "Level.h"


int GAME_InitContext(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	memset(ctx, 0, sizeof(GameContext_t));

	///////////////////
	// BACKGROUND
	///////////////////
	for (int i = 0; i < BACKGROUND_OBJECTS_MAX_SIZE; i++)
	{
		ctx->bgObjects[i].id = OBJECT_NOT_USED;
	}

	const ObjectLevelPos_t* BGObjectsPos = NULL;
	int numOfBGObjects = 0;
	ret = LEVEL_GetBGObjectsLocations(&BGObjectsPos, &numOfBGObjects);
	if (ret < 0) { return -1; }
	if (BGObjectsPos == NULL) { return -5; }

	if (numOfBGObjects > BACKGROUND_OBJECTS_MAX_SIZE)
	{
		numOfBGObjects = BACKGROUND_OBJECTS_MAX_SIZE;
	}

	for (int i = 0; i < numOfBGObjects; i++)
	{
		if (BGObjectsPos[i].id < BACKGROUND_OBJECT_ID_START || BGObjectsPos[i].id > BACKGROUND_OBJECT_ID_END)
		{
			continue;
		}

		ctx->bgObjects[i].id = BGObjectsPos[i].id;
		ctx->bgObjects[i].mapPos.x = BGObjectsPos[i].x;
		ctx->bgObjects[i].mapPos.y = BGObjectsPos[i].y;

		switch (BGObjectsPos[i].id)
		{
		case BG_JEDYNKA_OBJECT_ID: {
			ctx->bgObjects[i].asset = &JEDYNKA_ASSET;
			break;
		}
		case BG_DWOJKA_OBJECT_ID: {
			ctx->bgObjects[i].asset = &DWOJKA_ASSET;
			break;
		}
		case BG_CHMURKA_OBJECT_ID: {
			ctx->bgObjects[i].asset = &CHMURKA_ASSET;
			break;
		}
		default:
			break;
		}
	}

	///////////////////
	// ENEMIES
	///////////////////
	const ObjectLevelPos_t* EnemiesPos = NULL;
	int numOfEnemies = 0;
	ret = LEVEL_GetEnemiesLocations(&EnemiesPos, &numOfEnemies);
	if (ret < 0) { return -10; }
	if (EnemiesPos == NULL) { return -15; }

	if (numOfEnemies > ENEMIES_MAX_SIZE)
	{
		numOfEnemies = ENEMIES_MAX_SIZE;
	}

	for (int i = 0; i < numOfEnemies; i++)
	{
		if (EnemiesPos[i].id < ENEMY_ID_START || EnemiesPos[i].id > ENEMY_ID_END)
		{
			continue;
		}

		ctx->enemies[i].id = EnemiesPos[i].id;
		ctx->enemies[i].currMapPos.x = EnemiesPos[i].x;
		ctx->enemies[i].currMapPos.y = EnemiesPos[i].y;
		ctx->enemies[i].prevMapPos = ctx->enemies[i].currMapPos;

		switch (EnemiesPos[i].id)
		{
		case ENEMY_GOOMBA_ID: {
			ctx->enemies[i].asset = &GOOMBA_ASSET;
			break;
		}
		}
	}


	return 0;
}
