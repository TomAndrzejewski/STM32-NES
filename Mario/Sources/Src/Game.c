/*
 * Game.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include <string.h>
#include <math.h>

#include "Game_Defs.h"
#include "NES_Defs.h"
#include "NES_Functions.h"

#include "LCDControl.h"
#include "PADControl.h"
#include "RenderEngine.h"

#include "Game_Types.h"
#include "Level.h"
#include "GraphicsAssets.h"

#include "printf_logger.h"

#include "Game.h"





int GAME_InitContext(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	memset(ctx, 0, sizeof(GameContext_t));

	///////////////////
	// LEVEL DEFINITION
	///////////////////
	const ObjectLevelInstance_t* ObjectsPos = NULL;
	int numOfObjects = 0;
	ret = LEVEL_GetObjectsLocations(&ObjectsPos, &numOfObjects);
	if (ret < 0) { return -5; }
	if (ObjectsPos == NULL) { return -10; }

	///////////////////
	// OBJECTS MANAGER
	///////////////////
	ctx->objectsManager.objectPool = ObjectsPos;
	ctx->objectsManager.objectPoolSize = numOfObjects;
	ctx->objectsManager.objectPoolIndex = 0;
	ctx->objectsManager.activeWorldRect.p1 = (Point_t){0,0};
	ctx->objectsManager.activeWorldRect.p2 = (Point_t){LCD_WIDTH,LCD_HEIGHT};

	///////////////////
	// BACKGROUND
	///////////////////
	for (int i = 0; i < BACKGROUND_OBJECTS_MAX_SIZE; i++)
	{
		ctx->bgObjects[i].id = OBJECT_NOT_USED;
		ctx->IsBGObjectActive[i] = false;
		ctx->bgObjectsLUT[i] = 0;
	}
	ctx->activebgObjects = 0;

	///////////////////
	// FOREGROUND
	///////////////////
	for (int i = 0; i < FOREGROUND_OBJECTS_MAX_SIZE; i++)
	{
		ctx->IsFGObjectActive[i] = false;
		ctx->fgObjects[i].id = OBJECT_NOT_USED;
		ctx->fgObjectsLUT[i] = 0;
	}
	ctx->activefgObjects = 0;

	///////////////////
	// ENEMIES
	///////////////////
	for (int i = 0; i < ENEMIES_MAX_SIZE; i++)
	{
		ctx->enemies.pool[i].id = OBJECT_NOT_USED;
		ctx->enemies.IsEnemyActive[i] = false;
		ctx->enemies.enemiesLUT[i] = 0;
	}
	ctx->enemies.activeEnemies = 0;

	// ctx->enemies.activeEnemies = 0;
	// for (int i = 0; i < numOfObjects; i++)
	// {
	// 	if (!MISC_IsThisEnemyID(ObjectsPos[i].id))
	// 	{
	// 		continue;
	// 	}

	// 	EnemyState_t* enemy = &ctx->enemies.pool[ctx->enemies.activeEnemies];

	// 	switch (ObjectsPos[i].id)
	// 	{
	// 	case ENEMY_GOOMBA_ID: {
	// 		enemy->asset = &GOOMBA_ASSET;
	// 		break;
	// 	}
	// 	default:
	// 		break;
	// 	}

	// 	enemy->id = ObjectsPos[i].id;
	// 	enemy->IsAlive = true;
	// 	enemy->IsOnScreen = false;
	// 	enemy->currMapPos.x = ObjectsPos[i].x;
	// 	enemy->currMapPos.y = ObjectsPos[i].y;
	// 	enemy->prevMapPos = enemy->currMapPos;
	// 	enemy->prevSpriteSize = enemy->asset->baseAsset.sprite.size;

	// 	if (ctx->enemies.activeEnemies >= ENEMIES_MAX_SIZE - 1) {
	// 		printf_str("\n### ERROR, max Enemies reached ###\n");
	// 		break;
	// 	}

	// 	ctx->enemies.activeEnemies++;
	// }

	///////////////////
	// BACKGROUND REPETITION OBJECTS
	///////////////////
	for (int i = 0; i < BACKGROUND_REP_OBJECTS_MAX_SIZE; i++)
	{
		ctx->bgRepObjects[i].id = OBJECT_NOT_USED;
	}

	const RepObjectLevelPos_t* BGRepObjectsPos = NULL;
	int numOfBGRepObjects = 0;
	ret = LEVEL_GetBGRepObjectsLocations(&BGRepObjectsPos, &numOfBGRepObjects);
	if (ret < 0) { return -15; }
	if (BGRepObjectsPos == NULL) { return -20; }

	if (numOfBGRepObjects > BACKGROUND_REP_OBJECTS_MAX_SIZE)
	{
		numOfBGRepObjects = BACKGROUND_REP_OBJECTS_MAX_SIZE;
	}

	ctx->floorIndex = -1;
	ctx->activebgRepObjects = 0;
	for (int i = 0; i < numOfBGRepObjects; i++)
	{
		if (BGRepObjectsPos[i].id < BACKGROUND_REP_OBJECT_ID_START || BGRepObjectsPos[i].id > BACKGROUND_REP_OBJECT_ID_END)
		{
			continue;
		}

		ctx->bgRepObjects[i].id = BGRepObjectsPos[i].id;
		ctx->bgRepObjects[i].mapPos.x = BGRepObjectsPos[i].x;
		ctx->bgRepObjects[i].mapPos.y = BGRepObjectsPos[i].y;
		ctx->bgRepObjects[i].mulVector.x = BGRepObjectsPos[i].mulX;
		ctx->bgRepObjects[i].mulVector.y = BGRepObjectsPos[i].mulY;

		switch (BGRepObjectsPos[i].id)
		{
		case BG_REP_FLOOR_ID: {
			ctx->bgRepObjects[i].asset = &FLOOR_ASSET;
			ctx->floorIndex = i;
			break;
		}
		default:
			break;
		}

		ctx->activebgRepObjects++;
	}
	// floor is very important for next components
	if (ctx->floorIndex == -1)
	{
		return -25;
	}

	///////////////////
	// MAP
	///////////////////
	ctx->map.floorYLevel = ctx->bgRepObjects[ctx->floorIndex].asset->baseAsset.sprite.size.y * ctx->bgRepObjects[ctx->floorIndex].mulVector.y;

	///////////////////
	// CAMERA
	///////////////////
	ctx->camera.currPos = (Point_t){.x = 0, .y = 0};

	///////////////////
	// INPUT
	///////////////////
	ctx->input.buttons_state = 0;
	ctx->input.frameData.frameTimeUS = 0;
	ctx->input.frameData.frameTimeS = 0;

	///////////////////
	// PLAYER
	///////////////////
	ctx->player.animableAsset = &MARIO_ANIMABLE_ASSET;
	if (ctx->player.animableAsset->baseAssetsCount <= 0 || ctx->player.animableAsset->baseAssets[0].baseAsset == NULL) {
		return -30; //sanity check
	}
	ctx->player.asset.baseAsset = *ctx->player.animableAsset->baseAssets[0].baseAsset;
	ctx->player.asset.id = ctx->player.animableAsset->id;
	ctx->player.asset.BBox = ctx->player.animableAsset->BBox;
	ctx->player.id = ctx->player.asset.id;
	ctx->player.currMapPos.x = 80;
	ctx->player.currMapPos.y = ctx->map.floorYLevel;
	ctx->player.prevMapPos = ctx->player.currMapPos;
	ctx->player.prevSpriteSize = ctx->player.asset.baseAsset.sprite.size;
	ctx->player.currPhysicsFlags.lastMovementDirectionRight = true;
	ctx->player.currPhysicsFlags.IsDecelerating = false;
	ctx->player.prevPhysicsFlags = ctx->player.currPhysicsFlags;
	ctx->player.body.vx = 0.0f;
	ctx->player.body.vy = 0.0f;
	ctx->player.body.subpixelX = 0.0f;
	ctx->player.body.subpixelY = 0.0f;
	ctx->player.animator.currAnimation = MARIO_STANDSTILL_ANIMATION_ID;
	ctx->player.animator.state = PLAYER_ANIMATOR_STANDSTILL;
	ctx->player.lifePoints = 1;
	ctx->player.IsImmune = false;
	ctx->player.damageTaken = false;
	ctx->player.IsGrounded = true;
	ctx->player.JustHitFGObjectFromBottom = false;
	ctx->player.playerLevel = PLAYER_LITTLE;

	///////////////////
	// RENDERER
	///////////////////
	ctx->renderer.LCDOffsetX = 0;
	fast_memset(ctx->renderer.dirtyRects, 0, sizeof(ctx->renderer.dirtyRects));

	///////////////////
	// COLLISION
	///////////////////
	ctx->collision.size = 0;
	fast_memset(ctx->collision.bumps, 0, sizeof(ctx->collision.bumps));

	return 0;
}

int INPUT_Update(InputState_t* input, const GameContext_t* ctx, const u32 frameTimeUS)
{
	if (input == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	uint32_t buttons_state = GetButtonsState();
	ret = INPUT_SetButtonsState(input, buttons_state);
	if (ret < 0) { return -1; }

	ret = INPUT_SetFrameTimeUS(input, frameTimeUS);
	if (ret < 0) { return -1; }

	return 0;
}

int INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state)
{
	if (input == NULL) { return -1; }
	input->prev_buttons_state = input->buttons_state;
	input->buttons_state = buttons_state;
	return 0;
}

int INPUT_SetFrameTimeUS(InputState_t* input, u32 frameTimeUS)
{
	if (input == NULL) { return -1; }
	input->frameData.frameTimeUS = frameTimeUS;
	input->frameData.frameTimeS = input->frameData.frameTimeUS/1000000.0;
	return 0;
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

	if (mgr->objectPool == NULL) { return -1; }

	int loadedObjects = 0;
	while (loadedObjects < 100) // could be while(1) but safety first
	{
		if (mgr->objectPoolIndex >= mgr->objectPoolSize - 1) { // no more objects available
			break;
		}

		const ObjectLevelInstance_t* objectDef = &mgr->objectPool[mgr->objectPoolIndex];
		if (objectDef->x > mgr->activeWorldRect.p2.x) { // object is outside of active region
			break;
		}

		mgr->objectPoolIndex++; // assume load went succesfully to not block next objects

		// load object
		if (MISC_IsThisFGID(objectDef->id)) {
			if (ctx->activefgObjects >= FOREGROUND_OBJECTS_MAX_SIZE - 1) {
				printf_str("\n### ERROR, max FGObjects reached ###\n");
				continue;
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
				continue;
			}

			// fill free slot with new object
			ctx->IsFGObjectActive[index] = true;
			ForegroundObject_t* fgObject = &ctx->fgObjects[index];
			OBJECTS_MANAGER_FGObject_Load(fgObject, objectDef);

			// update LUT
			ctx->fgObjectsLUT[ctx->activefgObjects] = index;
			ctx->activefgObjects++;
		}
		else if (MISC_IsThisEnemyID(objectDef->id)) {
			if (ctx->enemies.activeEnemies >= ENEMIES_MAX_SIZE - 1) {
				printf_str("\n### ERROR, max enemies reached ###\n");
				continue;
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
				continue;
			}

			// fill free slot with new object
			ctx->enemies.IsEnemyActive[index] = true;
			EnemyState_t* enemy = &ctx->enemies.pool[index];
			OBJECTS_MANAGER_Enemy_Load(enemy, objectDef);

			ctx->enemies.enemiesLUT[ctx->enemies.activeEnemies] = index;
			ctx->enemies.activeEnemies++;
		}
		else if (MISC_IsThisBGID(objectDef->id)) {
			if (ctx->activebgObjects >= BACKGROUND_OBJECTS_MAX_SIZE - 1) {
				printf_str("\n### ERROR, max BGObjects reached ###\n");
				continue;
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
				continue;
			}

			ctx->IsBGObjectActive[index] = true;
			BackgroundObject_t* bgObject = &ctx->bgObjects[index];
			OBJECTS_MANAGER_BGObject_Load(bgObject, objectDef);

			// update LUT
			ctx->bgObjectsLUT[ctx->activebgObjects] = index;
			ctx->activebgObjects++;
		}

		loadedObjects++;
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

	obj->animableAsset = NULL;

	switch (objectDef->id)
	{
	case FG_BRICKS_OBJECT_ID: {
		obj->asset = BRICKS_ASSET;
		break;
	}
	case FG_BLOCK_QMARK_OBJECT_ID: {
		obj->animableAsset = &BLOCK_QMARK_ANIMABLE_ASSET;
		obj->asset.id = obj->animableAsset->id;
		obj->asset.BBox = obj->animableAsset->BBox;
		obj->currAnimation = FG_BLOCK_QMARK_1_ANIMATION_ID;
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
	obj->currFlags.IsOnScreen = true;
	obj->currFlags.playerBumpedFromBelow = false;
	obj->currFlags.clearRenderedSprite = false;
	obj->prevFlags = obj->currFlags;
	
	return 0;
}

int OBJECTS_MANAGER_BGObject_Load(BackgroundObject_t* obj, const ObjectLevelInstance_t* objectDef)
{
	if (obj == NULL || objectDef == NULL) { return -1;}

	obj->id = objectDef->id;
	obj->mapPos.x = objectDef->x;
	obj->mapPos.y = objectDef->y;
	obj->flags = objectDef->flags;

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

int	COLLISION_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	ret = COLLISION_Calculate(&ctx->collision, ctx);
	if (ret < 0) { return -5; }

	ret = COLLISION_Resolve(ctx);
	if (ret < 0) { return -10; }

	return 0;
}

int	COLLISION_Calculate(CollisionState_t* coll, const GameContext_t* ctx)
{
	if (coll == NULL || ctx == NULL) { return -1; }

	coll->size = 0;
	fast_memset(&coll->bumps, 0, sizeof(coll->bumps));

	Rect_t playerRect;
	playerRect.p1.x = ctx->player.currMapPos.x + ctx->player.asset.BBox.p1.x;
	playerRect.p1.y = ctx->player.currMapPos.y + ctx->player.asset.BBox.p1.y;
	playerRect.p2.x = ctx->player.currMapPos.x + ctx->player.asset.BBox.p2.x;
	playerRect.p2.y = ctx->player.currMapPos.y + ctx->player.asset.BBox.p2.y;

	//-------------------------
	// PLAYER BUMPS FLOOR
	//-------------------------
	const Rect_t* levelBounds = NULL;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	if (ret < 0 || levelBounds == NULL) { return -5; }

	Rect_t floorRect;
	floorRect.p1.x = 0;
	floorRect.p1.y = 0;
	floorRect.p2.x = levelBounds->p2.x;
	floorRect.p2.y = ctx->map.floorYLevel;

	Rect_t bumpRect = {0};
	Rect_GetIntersection(&playerRect, &floorRect, &bumpRect);

	if (Rect_IsIntersection(&bumpRect)) {
		if (coll->size < COLLISIONS_SIZE) {
			coll->bumps[coll->size].bumpID = PLAYER_BUMP_FLOOR;
			coll->bumps[coll->size].actor1 = (GameObjectRef_t){ .id = ctx->player.id, .index = 0 };
			coll->bumps[coll->size].actor2 = (GameObjectRef_t){ .id = 0, .index = 0 };
			coll->bumps[coll->size].bumpRect = bumpRect;
			coll->size++;
		}
	}

	//-------------------------
	// PLAYER BUMPS FG OBJECTS
	//-------------------------
	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];

		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}

		const ForegroundObject_t* fgObject = &ctx->fgObjects[indexLUT];
		if (!fgObject->currFlags.IsOnScreen) {
			continue;
		}
		if (!fgObject->currFlags.IsAlive) { continue; }

		Rect_t objRect;
		objRect.p1.x = fgObject->currMapPos.x + fgObject->asset.BBox.p1.x;
		objRect.p1.y = fgObject->currMapPos.y + fgObject->asset.BBox.p1.y;
		objRect.p2.x = fgObject->currMapPos.x + fgObject->asset.BBox.p2.x;
		objRect.p2.y = fgObject->currMapPos.y + fgObject->asset.BBox.p2.y;

		Rect_t bumpRect = {0};
		Rect_GetIntersection(&playerRect, &objRect, &bumpRect);

		if (Rect_IsIntersection(&bumpRect)) {
			if (coll->size < COLLISIONS_SIZE) {
				coll->bumps[coll->size].bumpID = PLAYER_BUMP_FG_OBJECT;
				coll->bumps[coll->size].actor1 = (GameObjectRef_t){ .id = ctx->player.id, .index = 0 };
				coll->bumps[coll->size].actor2 = (GameObjectRef_t){ .id = fgObject->id, .index = indexLUT };
				coll->bumps[coll->size].bumpRect = bumpRect;
				coll->size++;
			}
		}
	}

	//-------------------------
	// PLAYER BUMPS ENEMIES
	//-------------------------
	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		int indexLUT = ctx->enemies.enemiesLUT[i];

		if (!ctx->enemies.IsEnemyActive[indexLUT]) {
			continue;
		}
		
		const EnemyState_t* enemy = &ctx->enemies.pool[indexLUT];
		if (!enemy->IsOnScreen) {
			continue;
		}

		Rect_t enemyRect;
		enemyRect.p1.x = enemy->currMapPos.x + enemy->asset.BBox.p1.x;
		enemyRect.p1.y = enemy->currMapPos.y + enemy->asset.BBox.p1.y;
		enemyRect.p2.x = enemy->currMapPos.x + enemy->asset.BBox.p2.x;
		enemyRect.p2.y = enemy->currMapPos.y + enemy->asset.BBox.p2.y;

		Rect_t bumpRect = {0};
		Rect_GetIntersection(&playerRect, &enemyRect, &bumpRect);

		if (Rect_IsIntersection(&bumpRect)) {
			if (coll->size < COLLISIONS_SIZE) {
				coll->bumps[coll->size].bumpID = PLAYER_BUMP_ENEMY;
				coll->bumps[coll->size].actor1 = (GameObjectRef_t){ .id = ctx->player.id, .index = 0 };
				coll->bumps[coll->size].actor2 = (GameObjectRef_t){ .id = enemy->id, .index = indexLUT };
				coll->bumps[coll->size].bumpRect = bumpRect;
				coll->size++;
			}
		}
	}

	return 0;
}

int COLLISION_Resolve(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	for (int i = 0; i < ctx->collision.size; i++)
	{
		Bump_t* bump = &ctx->collision.bumps[i];
		switch (bump->bumpID)
		{
		case PLAYER_BUMP_FLOOR:
		{
			COLLISION_Player_Floor(&ctx->player, bump, ctx);
			break;
		}
		case PLAYER_BUMP_ENEMY:
		{
			const GameObjectRef_t* actor = (bump->actor1.id == ctx->player.id) ? &bump->actor2 : &bump->actor1;
			EnemyState_t* enemy = &ctx->enemies.pool[actor->index];

			ret = COLLISION_Player_Enemy(&ctx->player, enemy, bump, ctx);
			break;
		}
		case PLAYER_BUMP_FG_OBJECT:
		{
			const GameObjectRef_t* actor = (bump->actor1.id == ctx->player.id) ? &bump->actor2 : &bump->actor1;
			ForegroundObject_t* obj = &ctx->fgObjects[actor->index];

			ret = COLLISION_Player_FGObject(&ctx->player, obj, bump, ctx);
			break;
		}
		}
	}

	// return 0;
	return ret; // todo nie mam jeszcze pomyslu jak obsluzyc ret < 0
}

int COLLISION_Player_FGObject(PlayerState_t* player, ForegroundObject_t* obj, const Bump_t* bump, const GameContext_t* ctx)
{
	if (player == NULL || obj == NULL || bump == NULL || ctx == NULL) { return -1; }

	if (!(obj->assetFlags & COLL_ANY_ENABLED)) { return 0; }

	const int bumpLenX = CalcRectXLen(&bump->bumpRect);
	const int bumpLenY = CalcRectYLen(&bump->bumpRect);

	const int COLLISION_THRESHOLD_VERTICAL = 3;
	const int COLLISION_THRESHOLD_HORIZONTAL = 1;

	// 1. VERTICAL COLLISION (UP/DOWN)
	if (bumpLenX >= bumpLenY) {
		if (!(obj->assetFlags & COLL_TOP_ENABLED) && !(obj->assetFlags & COLL_DOWN_ENABLED)) return 0;
		if (bumpLenX <= COLLISION_THRESHOLD_VERTICAL) return 0;

		int bumpCenterY = (bump->bumpRect.p1.y + bump->bumpRect.p2.y) / 2;

		// LANDING ON OBJECT (TOP OF THE OBJECT)
		if (bumpCenterY > obj->BBoxCenter.y) {
			if ((obj->assetFlags & COLL_TOP_ENABLED) && !player->IsGrounded) {
				player->IsGrounded = true;
				player->currMapPos.y = obj->currMapPos.y + obj->asset.BBox.p2.y;
				player->body.subpixelY = 0.0f;
				player->body.vy = 0.0f;
			}
		}
		// BUMPING FROM THE BOTTOM
		else {
			if ((obj->assetFlags & COLL_DOWN_ENABLED)) {
				player->body.vy = -0.5f;
				player->currMapPos.y = obj->currMapPos.y - player->asset.BBox.p2.y;

				COLLISION_FGObject_Player_Action(obj, player, BUMP_SIDE_BOTTOM, ctx);
			}
		}
	}
	// 2. HORIZONTAL COLLISION (LEFT/RIGHT)
	else {
		if (!(obj->assetFlags & COLL_LEFT_ENABLED) && !(obj->assetFlags & COLL_RIGHT_ENABLED)) return 0;
		if (bumpLenY <= COLLISION_THRESHOLD_HORIZONTAL) return 0;

		int centerBumpX = (bump->bumpRect.p1.x + bump->bumpRect.p2.x) / 2;

		// BUMPING FROM RIGHT
		if (centerBumpX > obj->BBoxCenter.x) {
			if ((obj->assetFlags & COLL_RIGHT_ENABLED)) {
				player->body.vx = 0.0f;
				player->body.subpixelX = 0.0f;
				player->currMapPos.x = obj->currMapPos.x + obj->asset.BBox.p2.x + 1; // +1 in order to not "glue" to the object
			}
		}
		// BUMPING FROM LEFT
		else {
			if ((obj->assetFlags & COLL_LEFT_ENABLED)) {
				player->body.vx = 0.0f;
				player->body.subpixelX = 0.0f;
				player->currMapPos.x = obj->currMapPos.x - player->asset.BBox.p2.x - 1; // - 1 in order to not "glue" to the object
			}
		}
	}

	return 0;
}

int COLLISION_Player_Floor(PlayerState_t* player, const Bump_t* bump, const GameContext_t* ctx)
{
	if (player == NULL || bump == NULL || ctx == NULL) { return -1; }

	if (!player->IsGrounded) {
		player->IsGrounded = true;
		player->currMapPos.y = ctx->map.floorYLevel;
		player->body.subpixelY = 0.0f;
		player->body.vy = 0.0f;
	}

	return 0;
}

int COLLISION_Player_Enemy(PlayerState_t* player, EnemyState_t* enemy, const Bump_t* bump, const GameContext_t* ctx)
{
	if (player == NULL || enemy == NULL || bump == NULL || ctx == NULL) { return -1; }

	return 0;
}

int COLLISION_FGObject_Player_Action(ForegroundObject_t* obj, PlayerState_t* player, BumpSideEnum bumpSide, const GameContext_t* ctx)
{
	if (obj == NULL || player == NULL || ctx == NULL) { return -1; }

	switch (obj->id)
	{
	case FG_BRICKS_OBJECT_ID:
	case FG_BLOCK_QMARK_OBJECT_ID:
	{
		if (bumpSide == BUMP_SIDE_BOTTOM && !player->JustHitFGObjectFromBottom) {
			obj->currFlags.playerBumpedFromBelow = true;
			player->JustHitFGObjectFromBottom = true;
		}

		if (obj->id == FG_BRICKS_OBJECT_ID) {
			bool isPlayerBig = (player->playerLevel == PLAYER_BIG || player->playerLevel == PLAYER_SHOOTING) ? true : false;
			if (isPlayerBig) {
				obj->currFlags.IsAlive = false;
				obj->currFlags.clearRenderedSprite = true;
			} else {
				obj->assetFlags &= ~FG_SCROLL_RENDER; //todotomka bardzo zle ze zmieniam flags, ale nie mam pomyslu jak zrobic to inaczej
			}
		}

		break;
	}
	default:
		break;
	}

	return 0;
}

int PHYSICS_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];
		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}

		ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];
		if (!obj->currFlags.IsAlive) {
			continue;
		}

		ret = PHYSICS_FGObject_Update(obj, ctx);
		if (ret < 0) { return -5; }		
	}

	return 0;
}

int PHYSICS_Player_Update(PlayerState_t* player, const GameContext_t* ctx)
{
	if (player == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	ret = PHYSICS_Player_RestartFlags(player);
	if (ret < 0) { return -4; }

	ret = PHYSICS_Player_Movement(player, ctx);
	if (ret < 0) { return -5; }

	ret = PHYSICS_Player_CalcMapPos(player, ctx);
	if (ret < 0) { return -10; }

	ret = PHYSICS_Player_CalcMovementDirection(player);
	if (ret < 0) { return -15; }

	return 0;
}

int PHYSICS_Player_RestartFlags(PlayerState_t* player)
{
	if (player == NULL) { return -1; }

	player->prevPhysicsFlags = player->currPhysicsFlags;

	player->currPhysicsFlags.IsDecelerating = false;

	return 0;
}

int PHYSICS_Player_Movement(PlayerState_t* player, const GameContext_t* ctx)
{
	if (player == NULL || ctx == NULL) { return -1; }

	///////////////////
	// Y AXIS
	///////////////////
	if (player->IsGrounded) {
		if ((ctx->input.prev_buttons_state & PAD_BUTTON_A) == 0 && (ctx->input.buttons_state & PAD_BUTTON_A)) {
			player->IsGrounded = false;
			player->body.vy = 1.0f;
		}
	} else {
		if (player->body.vy > -1.0f) {
			float multiplier = 6.0f;
			if (player->body.vy > 0.0f && (ctx->input.prev_buttons_state & PAD_BUTTON_A) && (ctx->input.buttons_state & PAD_BUTTON_A)) {
				multiplier = 2.0f;
			}
			float dvy = ctx->input.frameData.frameTimeS * multiplier;
			player->body.vy -= dvy;
		}
	}

	///////////////////
	// X AXIS
	///////////////////
	if (ctx->input.buttons_state & PAD_BUTTON_RIGHT) {
		if (player->body.vx < 1.0f) {
			// Calculate new velocity
			float dvx = ctx->input.frameData.frameTimeS * 3;
			if (player->body.vx + dvx < 1.0f) {
				player->body.vx += dvx;
			} else {
				player->body.vx = 1.0f;
			}
			// Update deceleration flag
			player->currPhysicsFlags.IsDecelerating = (player->body.vx < 0.0f)? true : false;
		}
	} else if (ctx->input.buttons_state & PAD_BUTTON_LEFT) {
		if (player->body.vx > -1.0f) {
			// Calculate new velocity
			float dvx = ctx->input.frameData.frameTimeS * 3;
			if (player->body.vx - dvx > -1.0f) {
				player->body.vx -= dvx;
			} else {
				player->body.vx = -1.0f;
			}
			// Update deceleration flag
			player->currPhysicsFlags.IsDecelerating = (player->body.vx > 0.0f)? true : false;
		}
	} else {
		if (player->body.vx > 0.0f) {
			float dvx = ctx->input.frameData.frameTimeS * 2;
			if (player->body.vx - dvx > 0.0f) {
				player->body.vx -= dvx;
			} else {
				player->body.vx = 0.0f;
			}
		} else {
			float dvx = ctx->input.frameData.frameTimeS * 2;
			if (player->body.vx + dvx < 0.0f) {
				player->body.vx += dvx;
			} else {
				player->body.vx = 0.0f;
			}
		}
	}

	//todotomka 128 jako ustawienie (settings)
	player->body.subpixelX += (player->body.vx * SUBPIXEL_RESOLUTION * 128) / TARGET_FRAMERATE_HZ;
	player->body.subpixelY += (player->body.vy * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;

	return 0;
}

int PHYSICS_Player_CalcMapPos(PlayerState_t* player, const GameContext_t* ctx)
{
	if (player == NULL || ctx == NULL) { return -1; }
	int pixelsToMove = 0;

	const Rect_t* levelBounds = NULL;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	if (ret < 0 || levelBounds == NULL) { return -5; }

	player->prevMapPos = player->currMapPos;

	// New map position

	///////////////////
	// Y AXIS
	///////////////////
	pixelsToMove = (int)player->body.subpixelY / SUBPIXEL_RESOLUTION;
	if (pixelsToMove != 0) {
		player->body.subpixelY -= pixelsToMove * SUBPIXEL_RESOLUTION;

		int movedPosY = player->currMapPos.y + pixelsToMove;
		if (movedPosY >= levelBounds->p1.y && movedPosY < levelBounds->p2.y) {
			player->currMapPos.y += pixelsToMove;
		}
	}

	///////////////////
	// X AXIS
	///////////////////
	pixelsToMove = (int)player->body.subpixelX / SUBPIXEL_RESOLUTION;
	if (pixelsToMove != 0) {
		player->body.subpixelX -= pixelsToMove * SUBPIXEL_RESOLUTION;

		int movedPosX = player->currMapPos.x + pixelsToMove;
		if (	movedPosX >= levelBounds->p1.x &&
				movedPosX >= ctx->camera.screenRect.p1.x &&
				movedPosX < levelBounds->p2.x &&
				movedPosX < ctx->camera.screenRect.p2.x)
		{
			player->currMapPos.x += pixelsToMove;
		}
	}

	return 0;
}

int PHYSICS_Player_CalcMovementDirection(PlayerState_t* player)
{
	if (player == NULL) { return -1; }

	if (player->currMapPos.x > player->prevMapPos.x) {
		player->currPhysicsFlags.lastMovementDirectionRight = true;
	} else if (player->currMapPos.x < player->prevMapPos.x) {
		player->currPhysicsFlags.lastMovementDirectionRight = false;
	}

	return 0;
}

int PHYSICS_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	if (obj == NULL || ctx == NULL) { return -1; }

	PHYSICS_FGObject_Movement(obj, ctx);

	PHYSICS_FGObject_CalcMapPos(obj);

	PHYSICS_FGObject_SaveFlags(obj);

	// ret = PHYSICS_FGObject_CalcMovementDirection(obj);
	// if (ret < 0) { return -15; }

	return 0;
}

void PHYSICS_FGObject_SaveFlags(ForegroundObject_t* obj)
{
	obj->prevFlags = obj->currFlags;
}

void PHYSICS_FGObject_Movement(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	switch(obj->id)
	{
	case FG_BRICKS_OBJECT_ID:
	case FG_BLOCK_QMARK_OBJECT_ID:
	{
		Body_t* body = &obj->body;

		if (!obj->prevFlags.playerBumpedFromBelow 
			&& obj->currFlags.playerBumpedFromBelow // this just happened
			&& !obj->currFlags.bumpedAnimationOngoing) 
		{ 
			if (obj->id == FG_BRICKS_OBJECT_ID) {
				obj->currFlags.playerBumpedFromBelow = false; // allow multiple bumps
			}
			obj->currFlags.bumpedAnimationOngoing = true;
			printf_str("\nPodbitka\n");
			body->vy = 0.3f;
		} else if (body->vy > -1.0f) {
			body->vy -= ctx->input.frameData.frameTimeS * 4;
		}

		if (obj->currFlags.bumpedAnimationOngoing) {
			body->subpixelY += (body->vy * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;
		}

		break;
	}
	default:
		break;
	}
}

void PHYSICS_FGObject_CalcMapPos(ForegroundObject_t* obj)
{
	int pixelsToMove = 0;

	obj->prevMapPos = obj->currMapPos;

	// New map position
	switch (obj->id)
	{
	case FG_BRICKS_OBJECT_ID:
	case FG_BLOCK_QMARK_OBJECT_ID:
	{
		///////////////////
		// Y AXIS
		///////////////////
		if (obj->currFlags.bumpedAnimationOngoing) {
			pixelsToMove = (int)obj->body.subpixelY / SUBPIXEL_RESOLUTION;
			if (pixelsToMove != 0) {
				obj->body.subpixelY -= pixelsToMove * SUBPIXEL_RESOLUTION;

				int movedPosY = obj->currMapPos.y + pixelsToMove;
				if (movedPosY >= obj->origMapPos.y) {
					obj->currMapPos.y += pixelsToMove;
					printf_str("Ruch\n");
				} else {
					obj->currFlags.bumpedAnimationOngoing = false; // finish animation
					obj->currMapPos.y = obj->origMapPos.y; // make sure object is back in original position 
					obj->body.subpixelY = 0.0f;
					obj->body.vy = 0.0f;
					printf_str("Zero\n");
				}
			}
		}

		break;
	}
	default:
		break;
	}
}

int CAMERA_Update(CameraState_t* camera, const GameContext_t* ctx)
{
	if (camera == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	ret = CAMERA_CalcPos(camera, &ctx->player);
	if (ret < 0) { return -5; }

	ret = CAMERA_CalcScreenRect(camera);
	if (ret < 0) { return -10; }

	return 0;
}

int CAMERA_CalcPos(CameraState_t* camera, const PlayerState_t* player)
{
	if (camera == NULL || player == NULL) { return -1; }

	const Rect_t* levelBounds = NULL;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	if (ret < 0 || levelBounds == NULL) { return -5; }

	camera->prevPos = camera->currPos;

	if (player->currMapPos.x - camera->currPos.x > 80)
	{
		camera->currPos.x += player->currMapPos.x - camera->currPos.x - 80;
	}

	return 0;
}

int CAMERA_CalcScreenRect(CameraState_t* camera)
{
	if (camera == NULL) { return -1; }

	camera->screenRect.p1 = camera->currPos;
	camera->screenRect.p2.x = camera->currPos.x + LCD_WIDTH;
	camera->screenRect.p2.y = camera->currPos.y + LCD_HEIGHT;

	return 0;
}

bool MISC_IsThisPlayerID(const GameObjectID id)
{
	if (id >= PLAYER_ID_START && id <= PLAYER_ID_END) {
		return true;
	}
	return false;
}

bool MISC_IsThisEnemyID(const GameObjectID id)
{
	if (id >= ENEMY_ID_START && id <= ENEMY_ID_END) {
		return true;
	}
	return false;
}

bool MISC_IsThisFGID(const GameObjectID id)
{
	if (id >= FOREGROUND_OBJECT_ID_START && id <= FOREGROUND_OBJECT_ID_END) {
		return true;
	}
	return false;
}

bool MISC_IsThisBGID(const GameObjectID id)
{
	if (id >= BACKGROUND_OBJECT_ID_START && id <= BACKGROUND_OBJECT_ID_END) {
		return true;
	}
	return false;
}

int PLAYER_ClearFlags(PlayerState_t* player)
{
	if (player == NULL) { return -1; }

	player->IsGrounded = false;
	player->JustHitFGObjectFromBottom = false;

	return 0;
}


int PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect)
{
	if (player == NULL || dirtyRect == NULL) { return -1; }

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = player->prevMapPos;
	prevDirtyRect.p2.x = player->prevMapPos.x + player->prevSpriteSize.x;
	prevDirtyRect.p2.y = player->prevMapPos.y + player->prevSpriteSize.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = player->currMapPos;
	currDirtyRect.p2.x = player->currMapPos.x + player->asset.baseAsset.sprite.size.x;
	currDirtyRect.p2.y = player->currMapPos.y + player->asset.baseAsset.sprite.size.y;

	Rect_t commonDirtyRect;
	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	if (commonDirtyRect.p1.x <= commonDirtyRect.p2.x && commonDirtyRect.p1.y <= commonDirtyRect.p2.y)
	{
		*dirtyRect = commonDirtyRect;
	}
	else
	{
		return -5;
	}

	return 0;
}

int	ENEMIES_UpdateFlags(Enemies_t* enemies, const GameContext_t* ctx)
{
	if (enemies == NULL || ctx == NULL) { return -1; }

	for (int i = 0; i < enemies->activeEnemies; i++)
	{
		int indexLUT = enemies->enemiesLUT[i];

		if (!enemies->IsEnemyActive[indexLUT]) {
			continue;
		}
		enemies->pool[indexLUT].IsOnScreen = ENEMIES_CalcIsOnScreen(&enemies->pool[indexLUT], &ctx->camera.screenRect);
	}

	return 0;
}

int ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect)
{
	if (enemy == NULL || dirtyRect == NULL) { return -1; }

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = enemy->prevMapPos;
	prevDirtyRect.p2.x = enemy->prevMapPos.x + enemy->asset.baseAsset.sprite.size.x;
	prevDirtyRect.p2.y = enemy->prevMapPos.y + enemy->asset.baseAsset.sprite.size.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = enemy->currMapPos;
	currDirtyRect.p2.x = enemy->currMapPos.x + enemy->asset.baseAsset.sprite.size.x;
	currDirtyRect.p2.y = enemy->currMapPos.y + enemy->asset.baseAsset.sprite.size.y;

	Rect_t commonDirtyRect;
	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	if (commonDirtyRect.p1.x <= commonDirtyRect.p2.x && commonDirtyRect.p1.y <= commonDirtyRect.p2.y)
	{
		*dirtyRect = commonDirtyRect;
	}
	else
	{
		*dirtyRect = (Rect_t){0};
		return -5;
	}

	return 0;
}

bool ENEMIES_CalcIsOnScreen(const EnemyState_t* enemy, const Rect_t* screenRect)
{
	if (enemy == NULL || screenRect == NULL)	{ return false; }

	Rect_t enemyRect;
	enemyRect.p1 = enemy->currMapPos;
	enemyRect.p2.x = enemy->currMapPos.x + enemy->asset.baseAsset.sprite.size.x;
	enemyRect.p2.y = enemy->currMapPos.y + enemy->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(&enemyRect, screenRect, &commonRect);

	if (Rect_IsIntersection(&commonRect)) {
		return true;
	} else {
		return false;
	}
}

void FGOBJECTS_ClearFlags(GameContext_t* ctx)
{
	if (ctx == NULL) { return; }

	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];
		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}

		// ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];
	}
}

int FGOBJECTS_GetDirtyRect(const ForegroundObject_t* obj, Rect_t* dirtyRect)
{
	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = obj->prevMapPos;
	prevDirtyRect.p2.x = obj->prevMapPos.x + obj->asset.baseAsset.sprite.size.x;
	prevDirtyRect.p2.y = obj->prevMapPos.y + obj->asset.baseAsset.sprite.size.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = obj->currMapPos;
	currDirtyRect.p2.x = obj->currMapPos.x + obj->asset.baseAsset.sprite.size.x;
	currDirtyRect.p2.y = obj->currMapPos.y + obj->asset.baseAsset.sprite.size.y;

	Rect_t commonDirtyRect;
	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	if (commonDirtyRect.p1.x <= commonDirtyRect.p2.x && commonDirtyRect.p1.y <= commonDirtyRect.p2.y)
	{
		*dirtyRect = commonDirtyRect;
		return 0;
	}
	else
	{
		*dirtyRect = (Rect_t){0};
		return -5;
	}
}

int ANIMATOR_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	ret = ANIMATOR_Player_Update(&ctx->player, ctx);
	if (ret < 0) { return -5; }

	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];
		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}

		if (ctx->fgObjects[indexLUT].animableAsset == NULL) {
			continue;
		}

		ret = ANIMATOR_FGObject_Update(&ctx->fgObjects[indexLUT], ctx);
		if (ret < 0) { return -10; }
	}

	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		int indexLUT = ctx->enemies.enemiesLUT[i];
		if (!ctx->enemies.IsEnemyActive[indexLUT]) {
			continue;
		}

		EnemyState_t* enemy = &ctx->enemies.pool[indexLUT]; 
		if (enemy->animableAsset == NULL) {
			continue;
		}

		ret = ANIMATOR_Enemy_Update(enemy, ctx);
		if (ret < 0) { return -15; }
	}

	return 0;
}

int ANIMATOR_Player_Update(PlayerState_t* player, const GameContext_t* ctx)
{
	if (player == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	ret = ANIMATOR_Player_Decide(player, ctx);
	if (ret < 0) { return -5; }

	ret = ANIMATOR_Player_SetAsset(player);
	if (ret < 0) { return -5; }

	return 0;
}

int ANIMATOR_Player_Decide(PlayerState_t* player, const GameContext_t* ctx)
{
	if (player == NULL || ctx == NULL) { return -1; }

	const float vxThreshold = 0.0f;
	const float vyThreshold = 0.0f;

	bool isPlayerStandstill = false;
	bool isPlayerRunning = false;
	bool isPlayerJumping = false;
	bool isPlayerDecelerating = false;
	// bool isPlayerDead = false;

	PlayerAnimatorStateEnum prevState = player->animator.state;

 	if (fabsf(player->body.vy) > vyThreshold && (player->prevMapPos.y != player->currMapPos.y || prevState == PLAYER_ANIMATOR_JUMPING)) {
		isPlayerJumping = true;
	} else if (fabsf(player->body.vx) > vxThreshold && (player->prevMapPos.y != player->currMapPos.y || player->currPhysicsFlags.IsDecelerating)) {
		isPlayerDecelerating = true;
	} else if (fabsf(player->body.vx) > vxThreshold && (player->prevMapPos.x != player->currMapPos.x || prevState == PLAYER_ANIMATOR_RUNNING)) {
		isPlayerRunning = true;
	} else {
		isPlayerStandstill = true;
	}

	if (isPlayerStandstill) {
		player->animator.state = PLAYER_ANIMATOR_STANDSTILL;
		player->animator.currAnimation = MARIO_STANDSTILL_ANIMATION_ID;
	} else if (isPlayerJumping) {
		player->animator.state = PLAYER_ANIMATOR_JUMPING;
		player->animator.currAnimation = MARIO_JUMP_ANIMATION_ID;
	} else if (isPlayerDecelerating) {
		player->animator.state = PLAYER_ANIMATOR_DECELERATING;
		player->animator.currAnimation = MARIO_DECELERATE_ANIMATION_ID;
	} else if (isPlayerRunning) {
		player->animator.state = PLAYER_ANIMATOR_RUNNING;

		if (prevState != PLAYER_ANIMATOR_RUNNING) {
			player->animator.runAnimationFrameTimeUS = 0;
			player->animator.currAnimation = MARIO_RUN_1_ANIMATION_ID;
		} else {
			player->animator.runAnimationFrameTimeUS += ctx->input.frameData.frameTimeUS;

			uint32_t runAnimationVelTimeMultiplier = player->animator.runAnimationFrameTimeUS * fabsf(player->body.vx);
			
			if (runAnimationVelTimeMultiplier >= 0 && player->animator.runAnimationFrameTimeUS < 75000) {
				player->animator.currAnimation = MARIO_RUN_1_ANIMATION_ID;
			} else if (runAnimationVelTimeMultiplier > 75000 && runAnimationVelTimeMultiplier < 150000) {
				player->animator.currAnimation = MARIO_RUN_2_ANIMATION_ID;
			} else if (runAnimationVelTimeMultiplier > 150000 && runAnimationVelTimeMultiplier < 225000) {
				player->animator.currAnimation = MARIO_RUN_3_ANIMATION_ID;
			} else if (runAnimationVelTimeMultiplier > 225000) {
				player->animator.runAnimationFrameTimeUS = 0;
			}
		}
	}

		
// 	} else if (isPlayerDead) {
// //		player->animator.currAnimation = MARIO_DEAD_ANIMATION_ID;
// 	}

	return 0;
}

int ANIMATOR_Player_SetAsset(PlayerState_t* player)
{
	if (player == NULL) { return -1; }
	if (player->animableAsset == NULL) { return -5; }
	if (player->animableAsset->baseAssetsCount <= 0) { return -10; }

	// save sprite size for dirty rects
	player->prevSpriteSize = player->asset.baseAsset.sprite.size;

	int assetIndex = 0;
	for (int i = 0; i < player->animableAsset->baseAssetsCount; i++)
	{
		if (player->animableAsset->baseAssets[i].animationID == player->animator.currAnimation) {
			assetIndex = i;
			break;
		}
	}

	if (player->animableAsset->baseAssets[assetIndex].baseAsset != NULL) {
		player->asset.baseAsset = *player->animableAsset->baseAssets[assetIndex].baseAsset;
	}

	return 0;
}

int ANIMATOR_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	if (obj == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	ret = ANIMATOR_FGObject_Decide(obj, ctx);
	if (ret < 0) { return -5; }

	ret = ANIMATOR_FGObject_SetAsset(obj);
	if (ret < 0) { return -10; }

	// ret = ANIMATOR_FGObject_Movement(obj);
	// if (ret < 0) { return -15; }

	return 0;
}

int ANIMATOR_FGObject_Decide(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	if (obj == NULL || ctx == NULL) { return -1; }

	switch (obj->id)
	{
	case FG_BLOCK_QMARK_OBJECT_ID:
	{
		if (obj->currFlags.playerBumpedFromBelow) {
			obj->currAnimation = FG_BLOCK_QMARK_2_ANIMATION_ID;
		} else {
			obj->currAnimation = FG_BLOCK_QMARK_1_ANIMATION_ID;
		}
		break;
	}
	default:
		break;
	}

	return 0;
}

int ANIMATOR_FGObject_SetAsset(ForegroundObject_t* obj)
{
	if (obj == NULL) { return -1; }
	if (obj->animableAsset == NULL) { return -5; }
	if (obj->animableAsset->baseAssetsCount <= 0) { return -10; }

	int assetIndex = 0;
	for (int i = 0; i < obj->animableAsset->baseAssetsCount; i++)
	{
		if (obj->animableAsset->baseAssets[i].animationID == obj->currAnimation) {
			assetIndex = i;
			break;
		}
	}

	if (obj->animableAsset->baseAssets[assetIndex].baseAsset != NULL) {
		obj->asset.baseAsset = *obj->animableAsset->baseAssets[assetIndex].baseAsset;
	}

	return 0;
}

// int ANIMATOR_FGObject_Movement(ForegroundObject_t* obj)
// {
// 	if (obj == NULL) { return -1; }

// 	switch (obj->id)
// 	{
// 	case FG_BLOCK_QMARK_OBJECT_ID:
// 	{
// 		SimpleBlockAnimator_t* anim = &obj->animator.simpleBlockAnim;

// 		if (obj->playerJustBumpedFromBelow && anim->timeUS == 0) { // start animation
// 			anim->timeUS = GetTimestamp();
// 			anim->numOfMoves = 0;
// 		} 

// 		 // animation started, react
// 		if (anim->timeUS > 0)
// 		{
// 			obj->prevMapPos = obj->currMapPos; // backup for dirty rects

// 			// proper animation below
// 			uint32_t tdiff = CalcTimeMS(anim->timeUS);
// 			if (tdiff <= 30 && anim->numOfMoves == 0) {
// 				anim->numOfMoves = 1;
// 				obj->currMapPos.y++;
// 			} else if (tdiff > 30 && tdiff <= 75 && anim->numOfMoves == 1) {
// 				anim->numOfMoves = 2;
// 				obj->currMapPos.y++;
// 			} else if (tdiff > 75 && tdiff <= 150 && anim->numOfMoves == 2) {
// 				anim->numOfMoves = 3;
// 				obj->currMapPos.y++;
// 			} else if (tdiff > 150 && tdiff <= 225 && anim->numOfMoves == 3) {
// 				anim->numOfMoves = 4;
// 				obj->currMapPos.y--;
// 			} else if (tdiff > 225 && tdiff <= 270 && anim->numOfMoves == 4) {
// 				anim->numOfMoves = 5;
// 				obj->currMapPos.y--;
// 			} else if (tdiff > 270 && tdiff <= 300 && anim->numOfMoves == 5) {
// 				anim->numOfMoves = 6;
// 				obj->currMapPos.y--;
// 			} else if (anim->numOfMoves == 6) { // finish, clear animation
// 				anim->timeUS = 0;
// 				anim->numOfMoves = 0;
// 			}
// 		}
		
// 		break;
// 	}
// 	default:
// 		break;
// 	}

// 	return 0;
// }

int ANIMATOR_Enemy_Update(EnemyState_t* enemy, const GameContext_t* ctx)
{
	if (enemy == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	ret = ANIMATOR_Enemy_Decide(enemy, ctx);
	if (ret < 0) { return -5; }

	ret = ANIMATOR_Enemy_SetAsset(enemy);
	if (ret < 0) { return -10; }

	return 0;
}

int ANIMATOR_Enemy_Decide(EnemyState_t* enemy, const GameContext_t* ctx)
{
	if (enemy == NULL || ctx == NULL) { return -1; }

	switch (enemy->id)
	{
	case ENEMY_KOOPA_ID:
	{
		enemy->currAnimation = KOOPA_WALK_1_ANIMATION_ID;
		// if (obj->playerBumpedFromBelow) {
		// 	obj->currAnimation = FG_BLOCK_QMARK_2_ANIMATION_ID;
		// } else {
		// 	obj->currAnimation = FG_BLOCK_QMARK_1_ANIMATION_ID;
		// }
		break;
	}
	default:
		break;
	}

	return 0;
}

int ANIMATOR_Enemy_SetAsset(EnemyState_t* enemy)
{
	if (enemy) { return -1; }
	if (enemy->animableAsset == NULL) { return -5; }
	if (enemy->animableAsset->baseAssetsCount <= 0) { return -10; }

	int assetIndex = 0;
	for (int i = 0; i < enemy->animableAsset->baseAssetsCount; i++)
	{
		if (enemy->animableAsset->baseAssets[i].animationID == enemy->currAnimation) {
			assetIndex = i;
			break;
		}
	}

	if (enemy->animableAsset->baseAssets[assetIndex].baseAsset != NULL) {
		enemy->asset.baseAsset = *enemy->animableAsset->baseAssets[assetIndex].baseAsset;
	}

	return 0;
}

int RENDERER_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	ret = RENDERER_ScrollRender(&ctx->renderer, ctx);
	if (ret < 0) { return -1; }

	ret = RENDERER_DirtyRects_Calculate(&ctx->renderer, ctx);
	if (ret < 0) { return -1; }

	ret = RENDERER_DirtyRects_Render(&ctx->renderer, ctx);
	if (ret < 0) { return -1; }

	return 0;
}

int RENDERER_FirstRender(const GameContext_t* ctx)
{
	if (ctx == NULL)	{ return -1; }
	if (ctx->floorIndex < 0) { return -5; }

	LCD_WriteVertScrollStartAddr(ctx->renderer.LCDOffsetX);

	//sanity check
	if (ctx->map.floorYLevel < 0 || ctx->map.floorYLevel > 200) { return -10; }

	RENDERER_RenderFloor(&ctx->bgRepObjects[ctx->floorIndex]);

	for (int i = 0; i < LCD_WIDTH/16; i++)
	{
		Rect_t mapRect;
		mapRect.p1.x = i * 16;
		mapRect.p1.y = ctx->map.floorYLevel;
		mapRect.p2.x = i * 16 + 16;
		mapRect.p2.y = LCD_HEIGHT;

		Rect_t screenRect = mapRect;

		int baseRectArea = CalcRectArea(screenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			int indexLUT = ctx->bgObjectsLUT[j];
			if (!ctx->IsBGObjectActive[indexLUT]) {
				continue;
			}
			RENDERER_RenderBGObject(&ctx->bgObjects[indexLUT], &mapRect, &screenRect, ctx->renderer.LCDOffsetX);
		}

		RE_SendRect(screenRect, ctx->renderer.LCDOffsetX);
	}

	return 0;
}

int RENDERER_ScrollRender(RendererState_t* renderer, const GameContext_t* ctx)
{
	if (renderer == NULL || ctx == NULL) { return -1; }

	Point_t cameraDiff = {0};
	cameraDiff.x = ctx->camera.currPos.x - ctx->camera.prevPos.x;

	if (cameraDiff.x > 0)
	{
		int prevLCDOffsetX = renderer->LCDOffsetX;

		renderer->LCDOffsetX -= cameraDiff.x;
		// todotomka da sie ladniej, czytelniej robic sprawdzanie zakresow?
		if (renderer->LCDOffsetX > 319)
		{
			renderer->LCDOffsetX = renderer->LCDOffsetX - 320;
		}
		if (renderer->LCDOffsetX < 0)
		{
			renderer->LCDOffsetX = 320 + renderer->LCDOffsetX;
		}


		Rect_t rightMapRect;
		Rect_t leftScreenRect;
		Rect_t rightScreenRect;

		rightMapRect.p1.x = ctx->camera.prevPos.x + LCD_WIDTH - 1;
		rightMapRect.p1.y = ctx->map.floorYLevel;
		rightMapRect.p2.x = ctx->camera.currPos.x + LCD_WIDTH - 1;
		rightMapRect.p2.y = LCD_HEIGHT;

		leftScreenRect.p1.x = 0;
		leftScreenRect.p1.y = ctx->map.floorYLevel;
		leftScreenRect.p2.x = cameraDiff.x;
		leftScreenRect.p2.y = LCD_HEIGHT;

		rightScreenRect.p1.x = LCD_WIDTH - cameraDiff.x - 1;
		rightScreenRect.p1.y = ctx->map.floorYLevel;
		rightScreenRect.p2.x = LCD_WIDTH - 1;
		rightScreenRect.p2.y = LCD_HEIGHT;

		int baseRectArea;

		// LEFT
		baseRectArea = CalcRectArea(leftScreenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);
		RE_SendRect(leftScreenRect, prevLCDOffsetX);

		// SHIFT SCROLL
		LCD_WriteVertScrollStartAddr(renderer->LCDOffsetX);

		// RIGHT
		baseRectArea = CalcRectArea(rightScreenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		// uint32_t t1 = GetTimestamp();
		for (int i = 0; i < ctx->activebgObjects; i++)
		{
			int indexLUT = ctx->bgObjectsLUT[i];
			if (!ctx->IsBGObjectActive[indexLUT]) {
				continue;
			}
			RENDERER_RenderBGObject(&ctx->bgObjects[indexLUT], &rightMapRect, &rightScreenRect, renderer->LCDOffsetX);
		}
		// uint32_t tdiff = CalcTimeUS(t1);
		// printf_v("tdiff BG: %d\n", tdiff);

		// t1 = GetTimestamp();
		for (int i = 0; i < ctx->activefgObjects; i++)
		{
			int indexLUT = ctx->fgObjectsLUT[i];

			if (!ctx->IsFGObjectActive[indexLUT]) {
				continue;
			}

			const ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];
			if (!obj->currFlags.IsAlive) { continue; }
			if (!(obj->assetFlags & FG_SCROLL_RENDER)) { continue; }

			RENDERER_RenderFGObject(obj, &rightMapRect, &rightScreenRect, renderer->LCDOffsetX);
		}
		// tdiff = CalcTimeUS(t1);
		// printf_v("tdiff FG: %d\n\n", tdiff);

		RE_SendRect(rightScreenRect, renderer->LCDOffsetX);
	}

	return 0;
}

int	RENDERER_DirtyRects_Calculate(RendererState_t* renderer, const GameContext_t* ctx)
{
	if (renderer == NULL || ctx == NULL) { return -1; }

	renderer->activeDirtyRects = 0;
	DirtyRect_t* dirtyRects = renderer->dirtyRects;

	fast_memset(dirtyRects, 0, sizeof(renderer->dirtyRects));

	Rect_t cameraRect;
	cameraRect.p1.x = ctx->camera.currPos.x;
	cameraRect.p1.y = ctx->camera.currPos.y;
	cameraRect.p2.x = cameraRect.p1.x + LCD_WIDTH;
	cameraRect.p2.y = cameraRect.p1.y + LCD_HEIGHT;

	//////////////////////////
	// FOREGROUND OBJECTS DIRTY RECTS
	//////////////////////////
	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];
		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}
		const ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];

		if ((obj->assetFlags & FG_SCROLL_RENDER) && !obj->currFlags.clearRenderedSprite) {
			continue;
		}

		Rect_t dirtyRect;
		if (FGOBJECTS_GetDirtyRect(obj, &dirtyRect) < 0) { continue; }

		Rect_t commonRect = {0};
		Rect_GetIntersection(&cameraRect, &dirtyRect, &commonRect);

		if (Rect_IsIntersection(&commonRect))
		{
			if (renderer->activeDirtyRects >= DIRTY_RECTS_SIZE) {
				printf_str("\n### activeDirtyRects exeeds DIRTY_RECTS_SIZE ###\n");
			} else {
				dirtyRects[renderer->activeDirtyRects].rect = commonRect;
				renderer->activeDirtyRects++;
			}
		}
	}

	//////////////////////////
	// ENEMIES DIRTY RECTS
	//////////////////////////
	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		int indexLUT = ctx->enemies.enemiesLUT[i];

		if (!ctx->enemies.IsEnemyActive[indexLUT]) {
			continue;
		}

		Rect_t dirtyRect;
		if (ENEMIES_GetDirtyRect(&ctx->enemies.pool[indexLUT], &dirtyRect) < 0) { continue; }

		Rect_t commonRect = {0};
		Rect_GetIntersection(&cameraRect, &dirtyRect, &commonRect);

		if (Rect_IsIntersection(&commonRect))
		{
			if (renderer->activeDirtyRects >= DIRTY_RECTS_SIZE) {
				printf_str("\n### activeDirtyRects exeeds DIRTY_RECTS_SIZE ###\n");
			} else {
				dirtyRects[renderer->activeDirtyRects].rect = commonRect;
				renderer->activeDirtyRects++;
			}
		}
	}

	//////////////////////////
	// PLAYER DIRTY RECT
	//////////////////////////
	Rect_t dirtyRect;
	if (!PLAYER_GetDirtyRect(&ctx->player, &dirtyRect))
	{
		Rect_t commonRect = {0};
		Rect_GetIntersection(&cameraRect, &dirtyRect, &commonRect);

		if (Rect_IsIntersection(&commonRect))
		{
			if (renderer->activeDirtyRects >= DIRTY_RECTS_SIZE) {
				printf_str("\n### activeDirtyRects exeeds DIRTY_RECTS_SIZE ###\n");
			} else {
				dirtyRects[renderer->activeDirtyRects].rect = commonRect;
				renderer->activeDirtyRects++;
			}
		}
	}


	//////////////////////////
	// COMPOUNDING OVERLAPPING DIRTY RECTS
	//////////////////////////
	// printf_str("\nPrzed:\n");
	// for (int i = 0; i < renderer->activeDirtyRects; i++)
	// {
	// 	printf_v("x1: %d, y1: %d, x2: %d, y2: %d\n", dirtyRects[i].rect.p1.x, dirtyRects[i].rect.p1.y, dirtyRects[i].rect.p2.x, dirtyRects[i].rect.p2.y);
	// }

	for (int i = 0; i < renderer->activeDirtyRects; i++)
	{
		if (dirtyRects[i].used)	{ continue; }

		bool commonRectFound = false;
		for (int j = 0; j < renderer->activeDirtyRects; j++)
		{
			if (i == j)	{ continue; }
			if (dirtyRects[j].used)	{ continue; }

			Rect_t commonRect = {0};
			Rect_GetIntersection(&dirtyRects[i].rect, &dirtyRects[j].rect, &commonRect);

			if (Rect_IsIntersection(&commonRect))
			{
				Rect_t commonORRect;
				commonORRect.p1.x = min(dirtyRects[i].rect.p1.x, dirtyRects[j].rect.p1.x);
				commonORRect.p1.y = min(dirtyRects[i].rect.p1.y, dirtyRects[j].rect.p1.y);
				commonORRect.p2.x = max(dirtyRects[i].rect.p2.x, dirtyRects[j].rect.p2.x);
				commonORRect.p2.y = max(dirtyRects[i].rect.p2.y, dirtyRects[j].rect.p2.y);

				commonRectFound = true;
				dirtyRects[j].used = true;
				dirtyRects[i].rect = commonORRect;
				break;
			}
		}

		if (commonRectFound)
		{
			i--;
		}
	}

	// printf_str("Po:\n");
	// for (int i = 0; i < renderer->activeDirtyRects; i++)
	// {
	// 	if (dirtyRects[i].used)	{ continue; }
	// 	printf_v("x1: %d, y1: %d, x2: %d, y2: %d\n", dirtyRects[i].rect.p1.x, dirtyRects[i].rect.p1.y, dirtyRects[i].rect.p2.x, dirtyRects[i].rect.p2.y);
	// }

	return 0;
}

int	RENDERER_DirtyRects_Render(RendererState_t* renderer, const GameContext_t* ctx)
{
	if (renderer == NULL || ctx == NULL) { return -1; }

	DirtyRect_t* dirtyRects = renderer->dirtyRects;

	for (int i = 0; i < renderer->activeDirtyRects; i++)
	{
		if (dirtyRects[i].used)	{ continue; }

		DirtyRect_t* dirtyRect = &dirtyRects[i];

		Rect_t screenRect;
		screenRect.p1.x = dirtyRect->rect.p1.x - ctx->camera.currPos.x;
		screenRect.p1.y = dirtyRect->rect.p1.y - ctx->camera.currPos.y;
		screenRect.p2.x = dirtyRect->rect.p2.x - ctx->camera.currPos.x;
		screenRect.p2.y = dirtyRect->rect.p2.y - ctx->camera.currPos.y;

		//-----------------------
		// BACKGROUND COLOR
		//-----------------------
		int baseRectArea = CalcRectArea(dirtyRect->rect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		//-----------------------
		// BACKGROUND OBJECTS
		//-----------------------
		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			int indexLUT = ctx->bgObjectsLUT[j];
			if (!ctx->IsBGObjectActive[indexLUT]) {
				continue;
			}
			RENDERER_RenderBGObject(&ctx->bgObjects[indexLUT], &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
		}

		//-----------------------
		// FOREGROUND OBJECTS
		//-----------------------
		for (int j = 0; j < ctx->activefgObjects; j++)
		{
			int indexLUT = ctx->fgObjectsLUT[j];
			if (!ctx->IsFGObjectActive[indexLUT]) {
				continue;
			}
			const ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];
			if (!obj->currFlags.IsAlive) { continue; }

			RENDERER_RenderFGObject(obj, &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
		}

		//-----------------------
		// ENEMIES
		//-----------------------
		for (int j = 0; j < ctx->enemies.activeEnemies; j++)
		{
			int indexLUT = ctx->enemies.enemiesLUT[j];	
			if (!ctx->enemies.IsEnemyActive[indexLUT]) {
				continue;
			}
			const EnemyState_t* enemy = &ctx->enemies.pool[indexLUT];

			RENDERER_RenderEnemy(enemy, &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
		}

		//
		//-----------------------
		// PLAYER
		//-----------------------
		RENDERER_RenderPlayer(&ctx->player, &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
			
		
		RE_SendRect(screenRect, renderer->LCDOffsetX);
	}

	return 0;
}

int RENDERER_RenderFloor(const BackgroundRepObject_t* floor)
{
	if (floor == NULL)	{ return -1; }

	const BaseAsset_t* baseAsset = &floor->asset->baseAsset;
	SpriteRender_t renderContext = {0};

	for (int i = 0; i < floor->mulVector.y; i++)
	{
		for (int j = 0; j < floor->mulVector.x; j++)
		{
			renderContext.baseRect.p1.x = j * baseAsset->sprite.size.x;
			renderContext.baseRect.p1.y = i * baseAsset->sprite.size.y;
			renderContext.baseRect.p2.x = renderContext.baseRect.p1.x + baseAsset->sprite.size.x;
			renderContext.baseRect.p2.y = renderContext.baseRect.p1.y + baseAsset->sprite.size.y;

			renderContext.commonRect = renderContext.baseRect;

			renderContext.baseToSpriteOffset.x = 0;
			renderContext.baseToSpriteOffset.y = 0;
			renderContext.LCDOffsetX = 0;
			renderContext.mirrorX = false;
			renderContext.activeColorSwap = 0;

			RE_RenderSprite(&baseAsset->sprite, renderContext, false);
		}
	}

	return 0;
}

int RENDERER_RenderBGObject(const BackgroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (obj == NULL || mapRectToDraw == NULL || screenRect == NULL)	{ return -1; }

	Rect_t posRect;
	posRect.p1.x = obj->mapPos.x;
	posRect.p1.y = obj->mapPos.y;
	posRect.p2.x = obj->mapPos.x + obj->asset->baseAsset.sprite.size.x;
	posRect.p2.y = obj->mapPos.y + obj->asset->baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = posRect.p1.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = posRect.p1.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = (obj->flags & MIRROR_X) ? true : false;
		renderContext.activeColorSwap = 0;

		RE_FillSprite(&obj->asset->baseAsset.sprite, &renderContext);
	}

	return 0;
}

int	RENDERER_RenderFGObject(const ForegroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (obj == NULL || mapRectToDraw == NULL || screenRect == NULL) { return -1; }

	Rect_t posRect;
	posRect.p1.x = obj->currMapPos.x;
	posRect.p1.y = obj->currMapPos.y;
	posRect.p2.x = obj->currMapPos.x + obj->asset.baseAsset.sprite.size.x;
	posRect.p2.y = obj->currMapPos.y + obj->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = obj->currMapPos.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = obj->currMapPos.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = (obj->assetFlags & MIRROR_X) ? true : false;;
		renderContext.activeColorSwap = 0;

		RE_FillSprite(&obj->asset.baseAsset.sprite, &renderContext);
	}

	return 0;
}

int	RENDERER_RenderEnemy(const EnemyState_t* enemy, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (enemy == NULL || mapRectToDraw == NULL || screenRect == NULL) { return -1; }

	Rect_t posRect;
	posRect.p1.x = enemy->currMapPos.x;
	posRect.p1.y = enemy->currMapPos.y;
	posRect.p2.x = enemy->currMapPos.x + enemy->asset.baseAsset.sprite.size.x;
	posRect.p2.y = enemy->currMapPos.y + enemy->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = enemy->currMapPos.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = enemy->currMapPos.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = false;
		renderContext.activeColorSwap = 0;

		RE_FillSprite(&enemy->asset.baseAsset.sprite, &renderContext);
	}

	return 0;
}

int	RENDERER_RenderPlayer(const PlayerState_t* player, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (player == NULL || mapRectToDraw == NULL || screenRect == NULL) { return -1; }

	Rect_t posRect;
	posRect.p1.x = player->currMapPos.x;
	posRect.p1.y = player->currMapPos.y;
	posRect.p2.x = player->currMapPos.x + player->asset.baseAsset.sprite.size.x;
	posRect.p2.y = player->currMapPos.y + player->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = player->currMapPos.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = player->currMapPos.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = player->currPhysicsFlags.lastMovementDirectionRight ? false : true;
		renderContext.activeColorSwap = 0;
		// renderContext.activeColorSwap = 4;
		// renderContext.colorSwap[0][0] = 0x00f8;
		// renderContext.colorSwap[0][1] = 0xe0fd;
		// renderContext.colorSwap[1][0] = 0x00f9;
		// renderContext.colorSwap[1][1] = 0xe0fe;
		// renderContext.colorSwap[2][0] = 0x00fa;
		// renderContext.colorSwap[2][1] = 0xe0fe;
		// renderContext.colorSwap[3][0] = 0x00fb;
		// renderContext.colorSwap[3][1] = 0xe0ff;

		// uint32_t t1 = GetTimestamp();
		RE_FillSprite(&player->asset.baseAsset.sprite, &renderContext);
		// uint32_t tdiff = CalcTimeUS(t1);
		// printf_int(tdiff); printf_c('\n');
	}

	return 0;
}


