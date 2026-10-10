/*
 * ObjectsManager.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "ObjectsManager.h"

#include <string.h>

#include "Game_Types.h"
#include "GraphicsAssets.h"
#include "NES_Functions.h"
#include "printf_logger.h"


bool OBJECTS_MANAGER_IsThisEnemyID(const GameObjectID id)
{
	if (id >= ENEMY_ID_START && id <= ENEMY_ID_END) {
		return true;
	}
	return false;
}

bool OBJECTS_MANAGER_IsThisFGID(const GameObjectID id)
{
	if (id >= FOREGROUND_OBJECT_ID_START && id <= FOREGROUND_OBJECT_ID_END) {
		return true;
	}
	return false;
}

bool OBJECTS_MANAGER_IsThisBGID(const GameObjectID id)
{
	if (id >= BACKGROUND_OBJECT_ID_START && id <= BACKGROUND_OBJECT_ID_END) {
		return true;
	}
	return false;
}

int OBJECTS_MANAGER_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	OBJECTS_MANAGER_CalcActiveRegion(&ctx->objectsManager, ctx);

	ret = OBJECTS_MANAGER_DeleteObjects(ctx);
	if (ret < 0) { return -5; }

	ret = OBJECTS_MANAGER_LoadObjects(ctx);
	if (ret < 0) { return -10; }

	return 0;
}

void OBJECTS_MANAGER_CalcActiveRegion(ObjectsManager_t* mgr, const GameContext_t* ctx)
{
	mgr->activeWorldRect.p1.x = ctx->camera.screenRect.p1.x - OBJECTS_MANAGER_LEFT_DESPAWN_OFFSET;
	if (mgr->activeWorldRect.p1.x < 0) {
		mgr->activeWorldRect.p1.x = 0;
	}
	mgr->activeWorldRect.p1.y = ctx->camera.screenRect.p1.y; 
	mgr->activeWorldRect.p2.x = ctx->camera.screenRect.p2.x + OBJECTS_MANAGER_RIGHT_SPAWN_OFFSET; 
	mgr->activeWorldRect.p2.y = ctx->camera.screenRect.p2.y;
}

int OBJECTS_MANAGER_LoadObjects(GameContext_t* ctx)
{
	ObjectsManager_t* mgr = &ctx->objectsManager;

	//----------------
	// OBJECTS FROM LEVEL POOL
	//----------------
	if (mgr->objectPool == NULL) { return -1; }

	int loadedObjects = 0;
	while (loadedObjects < 100) // could be while(1) but safety first
	{
		if (mgr->objectPoolIndex >= mgr->objectPoolSize) { // no more objects available
			break;
		}

		const ObjectLevelInstance_t* objectDef = &mgr->objectPool[mgr->objectPoolIndex];
		if (objectDef->x > mgr->activeWorldRect.p2.x) { // object is outside of active region
			break;
		}

		mgr->objectPoolIndex++; // assume load went succesfully to not block next objects

		OBJECTS_MANAGER_SpawnObject(ctx, objectDef);

		loadedObjects++;
	}

	//----------------
	// OBJECTS FROM SPAWN QUEUE
	//----------------
	for (int i = 0; i < mgr->spawnBufferSize; i++)
	{
		const ObjectLevelInstance_t* objectDef = &mgr->spawnBuffer[i];

		if (objectDef->x > mgr->activeWorldRect.p2.x) { // object is outside of active region
			continue;
		}

		OBJECTS_MANAGER_SpawnObject(ctx, objectDef);
	}
	mgr->spawnBufferSize = 0; // assume every object has been spawned

	return 0;
}

int OBJECTS_MANAGER_SpawnObject(GameContext_t* ctx, const ObjectLevelInstance_t* objectDef)
{
	// load object
	if (OBJECTS_MANAGER_IsThisFGID(objectDef->id)) 
	{
		if (ctx->activefgObjects >= FOREGROUND_OBJECTS_MAX_SIZE) {
			printf_str("\n### ERROR, max FGObjects reached ###\n");
			return -5;
		}

		// find free slot
		int index = -1;
		for (int i = 0; i < FOREGROUND_OBJECTS_MAX_SIZE; i++)
		{
			if (!ctx->IsFGObjectActive[i]) {
				index = i;
				break;
			}
		}

		if (index < 0) {
			printf_str("\n### ERROR, no free FGObject found ###\n");
			return -10;
		}

		// fill free slot with new object
		ctx->IsFGObjectActive[index] = true;
		ForegroundObject_t* fgObject = &ctx->fgObjects[index];
		OBJECTS_MANAGER_FGObject_Load(fgObject, objectDef);

		// update LUT
		ctx->fgObjectsLUT[ctx->activefgObjects] = index;
		ctx->activefgObjects++;
	}
	else if (OBJECTS_MANAGER_IsThisEnemyID(objectDef->id)) 
	{
		if (ctx->enemies.activeEnemies >= ENEMIES_MAX_SIZE) {
			printf_str("\n### ERROR, max enemies reached ###\n");
			return -15;
		}

		// find free slot
		int index = -1;
		for (int i = 0; i < ENEMIES_MAX_SIZE; i++)
		{
			if (!ctx->enemies.IsEnemyActive[i]) {
				index = i;
				break;
			}
		}

		if (index < 0) {
			printf_str("\n### ERROR, no free enemies found ###\n");
			return -20;
		}

		// fill free slot with new object
		ctx->enemies.IsEnemyActive[index] = true;
		EnemyState_t* enemy = &ctx->enemies.pool[index];
		OBJECTS_MANAGER_Enemy_Load(enemy, objectDef);

		ctx->enemies.enemiesLUT[ctx->enemies.activeEnemies] = index;
		ctx->enemies.activeEnemies++;
	}
	else if (OBJECTS_MANAGER_IsThisBGID(objectDef->id)) 
	{
		if (ctx->activebgObjects >= BACKGROUND_OBJECTS_MAX_SIZE) {
			printf_str("\n### ERROR, max BGObjects reached ###\n");
			return -25;
		}

		// find free slot
		int index = -1;
		for (int i = 0; i < BACKGROUND_OBJECTS_MAX_SIZE; i++)
		{
			if (!ctx->IsBGObjectActive[i]) {
				index = i;
				break;
			}
		}

		if (index < 0) {
			printf_str("\n### ERROR, no free BGObject found ###\n");
			return -30;
		}

		ctx->IsBGObjectActive[index] = true;
		BackgroundObject_t* bgObject = &ctx->bgObjects[index];
		OBJECTS_MANAGER_BGObject_Load(bgObject, objectDef);

		// update LUT
		ctx->bgObjectsLUT[ctx->activebgObjects] = index;
		ctx->activebgObjects++;
	}

	return 0;
}

int OBJECTS_MANAGER_DeleteObjects(GameContext_t* ctx)
{
	ObjectsManager_t* mgr = &ctx->objectsManager;

	for (int i = 0; i < FOREGROUND_OBJECTS_MAX_SIZE; i++)
	{
		if (!ctx->IsFGObjectActive[i]) {
			continue;
		}

		ForegroundObject_t* fgObject = &ctx->fgObjects[i];

		if (fgObject->currMapPos.x < mgr->activeWorldRect.p1.x) { // object is out of active region
			ctx->IsFGObjectActive[i] = false;

			// delete object from LUT and make LUT sorted again
			for (int j = 0; j < ctx->activefgObjects; j++)
			{
				if (ctx->fgObjectsLUT[j] == i) { // found object to delete
					for (int k = j; k < ctx->activefgObjects - 1; k++) 
					{ 
						ctx->fgObjectsLUT[k] = ctx->fgObjectsLUT[k + 1]; // move by one position
					}
					ctx->activefgObjects--;
					break;
				}
			}
		}
	}

	for (int i = 0; i < BACKGROUND_OBJECTS_MAX_SIZE; i++)
	{
		if (!ctx->IsBGObjectActive[i]) {
			continue;
		}

		BackgroundObject_t* bgObject = &ctx->bgObjects[i];

		if (bgObject->mapPos.x < mgr->activeWorldRect.p1.x) { // object is out of active region
			ctx->IsBGObjectActive[i] = false;

			// delete object from LUT and make LUT sorted again
			for (int j = 0; j < ctx->activebgObjects; j++)
			{
				if (ctx->bgObjectsLUT[j] == i) { // found object to delete
					for (int k = j; k < ctx->activebgObjects - 1; k++) 
					{ 
						ctx->bgObjectsLUT[k] = ctx->bgObjectsLUT[k + 1]; // move by one position
					}
					ctx->activebgObjects--;
					break;
				}
			}
		}
	}

	for (int i = 0; i < ENEMIES_MAX_SIZE; i++)
	{
		if (!ctx->enemies.IsEnemyActive[i]) {
			continue;
		}

		EnemyState_t* enemy = &ctx->enemies.pool[i];

		if (enemy->currMapPos.x < mgr->activeWorldRect.p1.x) { // object is out of active region
			ctx->enemies.IsEnemyActive[i] = false;

			// delete object from LUT and make LUT sorted again
			for (int j = 0; j < ctx->enemies.activeEnemies; j++)
			{
				if (ctx->enemies.enemiesLUT[j] == i) { // found object to delete
					for (int k = j; k < ctx->enemies.activeEnemies - 1; k++) 
					{ 
						ctx->enemies.enemiesLUT[k] = ctx->enemies.enemiesLUT[k + 1]; // move by one position
					}
					ctx->enemies.activeEnemies--;
					break;
				}
			}
		}
	}

	return 0;
}

int OBJECTS_MANAGER_Enemy_Load(EnemyState_t* enemy, const ObjectLevelInstance_t* objectDef)
{
	if (enemy == NULL || objectDef == NULL) { return -1; }

	fast_memset(enemy, 0, sizeof(EnemyState_t));

	enemy->id = objectDef->id;
	enemy->animableAsset = NULL;

	switch (enemy->id)
	{
	case ENEMY_GOOMBA_ID: {
		enemy->asset = GOOMBA_ASSET;
		break;
	}
	case ENEMY_KOOPA_ID: {
		enemy->animableAsset = &KOOPA_ANIMABLE_ASSET;
		enemy->asset.id = enemy->animableAsset->id;
		enemy->asset.BBox = enemy->animableAsset->BBox;
		enemy->currAnimation = KOOPA_WALK_1_ANIMATION_ID;
		break;
	}
	default:
		break;
	}

	enemy->IsAlive = true;
	enemy->IsOnScreen = false;
	enemy->currMapPos.x = objectDef->x;
	enemy->currMapPos.y = objectDef->y;
	enemy->prevMapPos = enemy->currMapPos;
	enemy->prevSpriteSize = enemy->asset.baseAsset.sprite.size;

	return 0;
}

int OBJECTS_MANAGER_FGObject_Load(ForegroundObject_t* obj, const ObjectLevelInstance_t* objectDef)
{
	if (obj == NULL || objectDef == NULL) { return -1;}

	fast_memset(obj, 0, sizeof(ForegroundObject_t));
	
	obj->animableAsset = NULL;
    obj->currFlags.startPhysics = false;
	obj->currFlags.renderInBackground = false;

	switch (objectDef->id)
	{
	case FG_BRICKS_OBJECT_ID: {
		obj->asset = BRICKS_ASSET;

		// Configure time based movement
		obj->physics.type = PHYSICS_TIME;
        TimeBasedMovement_t* tbased = &obj->physics.engine.timeBased;
		tbased->asset = &BLOCK_QMARK_MOVEMENT_ASSET;
		tbased->elapsedTimeUS = 0;
		tbased->currentIndex = 0;
		tbased->repeatedCounter = 0;

		break;
	}
	case FG_BLOCK_QMARK_OBJECT_ID: {
		obj->animableAsset = &BLOCK_QMARK_ANIMABLE_ASSET;
		obj->asset.id = obj->animableAsset->id;
		obj->asset.BBox = obj->animableAsset->BBox;
		obj->currAnimation = FG_BLOCK_QMARK_1_ANIMATION_ID;
		obj->animationFrameTimeUS = 0;
		
		// Configure time based movement
		obj->physics.type = PHYSICS_TIME;
        TimeBasedMovement_t* tbased = &obj->physics.engine.timeBased;
		tbased->asset = &BLOCK_QMARK_MOVEMENT_ASSET;
		tbased->elapsedTimeUS = 0;
		tbased->currentIndex = 0;
		tbased->repeatedCounter = 0;

        break;
	}
	case FG_RURA_DOL_OBJECT_ID: {
		obj->asset = RURA_DOL_ASSET;
		break;
	}
	case FG_RURA_GORA_OBJECT_ID: {
		obj->asset = RURA_GORA_ASSET;
		break;
	}
	case FG_PYRAMID_BLOCK_OBJECT_ID: {
		obj->asset = PYRAMID_BLOCK_ASSET;
		break;
	}
	case FG_REWARD_COIN_OBJECT_ID: {
		// animations
		obj->animableAsset = &REWARD_COIN_ANIMABLE_ASSET;
		obj->asset.id = obj->animableAsset->id;
		obj->asset.BBox = obj->animableAsset->BBox;
		obj->currAnimation = FG_REWARD_COIN_1_ANIMATION_ID;
		obj->animationFrameTimeUS = 0;

		// Configure time based movement
		obj->physics.type = PHYSICS_TIME;
        TimeBasedMovement_t* tbased = &obj->physics.engine.timeBased;
		tbased->asset = &REWARD_COIN_MOVEMENT_ASSET;
		tbased->elapsedTimeUS = 0;
		tbased->currentIndex = 0;
		tbased->repeatedCounter = 0;

        obj->currFlags.startPhysics = true; // trigger physics instantly

		break;
	}
	case FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID: {
		obj->asset = REWARD_LEVEL_UP_MUSHROOM_ASSET;

		// Configure time based movement
		obj->physics.type = PHYSICS_TIME;
        TimeBasedMovement_t* tbased = &obj->physics.engine.timeBased;
		tbased->asset = &REWARD_LEVEL_UP_MUSHROOM_MOVEMENT_ASSET;
		tbased->elapsedTimeUS = 0;
		tbased->currentIndex = 0;
		tbased->repeatedCounter = 0;

        obj->currFlags.startPhysics = true; // trigger physics instantly

		obj->currFlags.renderInBackground = true;

		break;
	}
	default:
		break;
	}

	obj->id = objectDef->id;
	obj->assetFlags = objectDef->flags;
	obj->origMapPos.x = objectDef->x;
	obj->origMapPos.y = objectDef->y;
	obj->currMapPos = obj->origMapPos;
	obj->prevMapPos = obj->origMapPos;
	obj->BBoxCenter.x = objectDef->x + (obj->asset.BBox.p1.x + obj->asset.BBox.p2.x) / 2;
	obj->BBoxCenter.y = objectDef->y + (obj->asset.BBox.p1.y + obj->asset.BBox.p2.y) / 2;
	obj->currFlags.IsAlive = true;
	obj->currFlags.playerBumpedFromBelow = false;
	obj->currFlags.clearRenderedSprite = false;
	obj->currFlags.IsGrounded = false;
    obj->currFlags.physicsOngoing = false;
	obj->prevFlags = obj->currFlags;
	obj->bumpCounter = 0;
	obj->rewardCounter = 0;
	
	return 0;
}

int OBJECTS_MANAGER_BGObject_Load(BackgroundObject_t* obj, const ObjectLevelInstance_t* objectDef)
{
	if (obj == NULL || objectDef == NULL) { return -1;}

	fast_memset(obj, 0, sizeof(BackgroundObject_t));

	obj->id = objectDef->id;
	obj->mapPos.x = objectDef->x;
	obj->mapPos.y = objectDef->y;
	obj->assetFlags = objectDef->flags;

	switch (objectDef->id)
	{
	case BG_JEDYNKA_OBJECT_ID: {
		obj->asset = &JEDYNKA_ASSET;
		break;
	}
	case BG_DWOJKA_OBJECT_ID: {
		obj->asset = &DWOJKA_ASSET;
		break;
	}
	case BG_CHMURKA_OBJECT_ID: {
		obj->asset = &CHMURKA_ASSET;
		break;
	}
	case BG_KRZAK_OBJECT_ID: {
		obj->asset = &KRZAK_ASSET;
		break;
	}
	case BG_HILL_0_OBJECT_ID: {
		obj->asset = &HILL_0_ASSET;
		break;
	}
	case BG_HILL_1_OBJECT_ID: {
		obj->asset = &HILL_1_ASSET;
		break;
	}
	case BG_HILL_2_OBJECT_ID: {
		obj->asset = &HILL_2_ASSET;
		break;
	}
	case BG_HILL_3_OBJECT_ID: {
		obj->asset = &HILL_3_ASSET;
		break;
	}
	case BG_HILL_4_OBJECT_ID: {
		obj->asset = &HILL_4_ASSET;
		break;
	}
	default:
		break;
	}
	
	return 0;
}

int	OBJECTS_MANAGER_OrderSpawn(ObjectsManager_t* mgr, const ObjectLevelInstance_t* objectToSpawn)
{
	if (mgr == NULL || objectToSpawn == NULL) { return -1;}

	if (mgr->spawnBufferSize > OBJECTS_MANAGER_SPAWN_BUFFER_MAX_SIZE - 1) {
		return -5;
	}

	mgr->spawnBuffer[mgr->spawnBufferSize] = *objectToSpawn;
	mgr->spawnBufferSize++;

	return 0;
}