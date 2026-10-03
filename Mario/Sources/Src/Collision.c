/*
 * Collision.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Collision.h"

#include <string.h>

#include "Game_Types.h"
#include "NES_Functions.h"
#include "Level.h"
#include "ObjectsManager.h"


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
			coll->bumps[coll->size].actor1 = (GameObjectRef_t){ ctx->player.id, 0 };
			coll->bumps[coll->size].actor2 = (GameObjectRef_t){ 0, 0 };
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

		if (!fgObject->currFlags.IsAlive) { continue; }

		if (!(fgObject->assetFlags & COLL_ANY_ENABLED)) { continue; }

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
				coll->bumps[coll->size].actor1 = (GameObjectRef_t){ ctx->player.id, 0 };
				coll->bumps[coll->size].actor2 = (GameObjectRef_t){ fgObject->id, indexLUT };
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
				coll->bumps[coll->size].actor1 = (GameObjectRef_t){ ctx->player.id, 0 };
				coll->bumps[coll->size].actor2 = (GameObjectRef_t){ enemy->id, indexLUT };
				coll->bumps[coll->size].bumpRect = bumpRect;
				coll->size++;
			}
		}
	}

	//-------------------------
	// FG OBJECTS BUMPS FG OBJECTS OR FLOOR
	//-------------------------
	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];

		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}

		const ForegroundObject_t* fgObject = &ctx->fgObjects[indexLUT];

		// Only objects below are allowed to initialize collision
		if (fgObject->id != FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID) {
			continue;
		}

		if (!fgObject->currFlags.IsAlive) { 
			continue; 
		}

		if (fgObject->physics.type != PHYSICS_VELOCITY) {
			continue; 
		}

		Rect_t objRect;
		objRect.p1.x = fgObject->currMapPos.x + fgObject->asset.BBox.p1.x;
		objRect.p1.y = fgObject->currMapPos.y + fgObject->asset.BBox.p1.y;
		objRect.p2.x = fgObject->currMapPos.x + fgObject->asset.BBox.p2.x;
		objRect.p2.y = fgObject->currMapPos.y + fgObject->asset.BBox.p2.y;


		//-------------------------
		// FG OBJECTS BUMPS FLOOR
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
		Rect_GetIntersection(&objRect, &floorRect, &bumpRect);

		if (Rect_IsIntersection(&bumpRect)) {
			if (coll->size < COLLISIONS_SIZE) {
				coll->bumps[coll->size].bumpID = FG_OBJECT_BUMP_FLOOR;
				coll->bumps[coll->size].actor1 = (GameObjectRef_t){ fgObject->id, indexLUT};
				coll->bumps[coll->size].actor2 = (GameObjectRef_t){ 0, 0 };
				coll->bumps[coll->size].bumpRect = bumpRect;
				coll->size++;
			}
		}


		//-------------------------
		// FG OBJECTS BUMPS FG OBJECTS
		//-------------------------
		for (int j = 0; j < ctx->activefgObjects; j++)
		{
			if (i == j) {
				continue;
			}

			int indexLUT2 = ctx->fgObjectsLUT[j];

			if (!ctx->IsFGObjectActive[indexLUT2]) {
				continue;
			}

			const ForegroundObject_t* fgObject2 = &ctx->fgObjects[indexLUT2];

			if (!fgObject2->currFlags.IsAlive) { 
				continue; 
			}	

			if ((fgObject2->assetFlags & COLL_ANY_ENABLED) == 0) {
				continue;
			}

			Rect_t objRect2;
			objRect2.p1.x = fgObject2->currMapPos.x + fgObject2->asset.BBox.p1.x;
			objRect2.p1.y = fgObject2->currMapPos.y + fgObject2->asset.BBox.p1.y;
			objRect2.p2.x = fgObject2->currMapPos.x + fgObject2->asset.BBox.p2.x;
			objRect2.p2.y = fgObject2->currMapPos.y + fgObject2->asset.BBox.p2.y;

			Rect_t bumpRect = {0};
			Rect_GetIntersection(&objRect, &objRect2, &bumpRect);

			if (Rect_IsIntersection(&bumpRect)) {
				if (coll->size < COLLISIONS_SIZE) {
					coll->bumps[coll->size].bumpID = FG_OBJECT_BUMP_FG_OBJECT;
					coll->bumps[coll->size].actor1 = (GameObjectRef_t){ fgObject->id, indexLUT };
					coll->bumps[coll->size].actor2 = (GameObjectRef_t){ fgObject2->id, indexLUT2 };
					coll->bumps[coll->size].bumpRect = bumpRect;
					coll->size++;
				}
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

			ret = COLLISION_Player_FGObject(&ctx->player, obj, bump);
			if (ret > 0) { // call action after collision, collision side set as return value
				BumpSideEnum bumpSide = ret;
				COLLISION_FGObject_Player_Action(obj, &ctx->player, bumpSide, &ctx->objectsManager, ctx);
			}
			break;
		}
		case FG_OBJECT_BUMP_FG_OBJECT:
		{
			ForegroundObject_t* obj1 = &ctx->fgObjects[bump->actor1.index];
			ForegroundObject_t* obj2 = &ctx->fgObjects[bump->actor2.index];

			ret = COLLISION_FGObject_FGObject(obj1, obj2, bump);
			break;
		}
		case FG_OBJECT_BUMP_FLOOR:
		{
			ForegroundObject_t* obj = &ctx->fgObjects[bump->actor1.index];
			COLLISION_FGObject_Floor(obj, ctx);
			break;
		}
		default:
			break;
		}
	}

	return 0;
}

// return code: < 0 error, 0 ok, > 0 BumpSideEnum returned
int COLLISION_Player_FGObject(PlayerState_t* player, ForegroundObject_t* obj, const Bump_t* bump)
{
	if (player == NULL || obj == NULL || bump == NULL) { return -1; }

	const int bumpLenX = CalcRectXLen(&bump->bumpRect);
	const int bumpLenY = CalcRectYLen(&bump->bumpRect);

	const int COLLISION_THRESHOLD_VERTICAL = 3;
	const int COLLISION_THRESHOLD_HORIZONTAL = 1;

	int retCode = 0;

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
				retCode = BUMP_SIDE_BOTTOM;
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

	return retCode;
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

int COLLISION_FGObject_Player_Action(
	ForegroundObject_t* obj, 
	PlayerState_t* player, 
	BumpSideEnum bumpSide, 
	ObjectsManager_t* mgr, 
	const GameContext_t* ctx)
{
	if (obj == NULL || player == NULL || ctx == NULL) { return -1; }

	bool prevPlayerHitFGObjectFromBottom = player->JustHitFGObjectFromBottom;

	if (bumpSide == BUMP_SIDE_BOTTOM && !player->JustHitFGObjectFromBottom) {
		// only one bump per frame!
		player->JustHitFGObjectFromBottom = true;
		// Code below is very bad, fix pls :(
		obj->assetFlags &= ~FG_SCROLL_RENDER;
	}

	//--------------
	if (obj->assetFlags & BUMPABLE) 
	{
		if (bumpSide == BUMP_SIDE_BOTTOM && !prevPlayerHitFGObjectFromBottom) 
		{
			// General info to other systems: I've beed bumped!
			obj->currFlags.playerBumpedFromBelow = true;

			if (!(obj->assetFlags & BUMPABLE_MULTIPLE_TIMES)) {
				//todotomka very bad, assetFlags should be const!
				obj->assetFlags &= ~BUMPABLE; // NO MORE BUMPING FOR YOU!
			}
			obj->bumpCounter++;
		}
	}

	//--------------
	if (obj->assetFlags & DESTROYABLE) 
	{
		if (bumpSide == BUMP_SIDE_BOTTOM && !prevPlayerHitFGObjectFromBottom) {
			bool isPlayerBig = (player->playerLevel == PLAYER_BIG || player->playerLevel == PLAYER_SHOOTING) ? true : false;
			if (isPlayerBig) {
				// General Info to every system: I'm dead!
				obj->currFlags.IsAlive = false;
				// Specific info for renderer to clear previous dirty rect 
				obj->currFlags.clearRenderedSprite = true;
			}
		}
	}

	//--------------
	if (obj->assetFlags & HIDDEN)
	{
		if (bumpSide == BUMP_SIDE_BOTTOM && !prevPlayerHitFGObjectFromBottom) 
		{
			obj->assetFlags &= ~HIDDEN; // IM NOT HIDDEN ANYMORE!
		}
	}

	//--------------
	if (obj->assetFlags & REWARD_BIT_MASK)
	{
		if (bumpSide == BUMP_SIDE_BOTTOM && !prevPlayerHitFGObjectFromBottom) 
		{
			obj->rewardCounter++;
			if (obj->rewardCounter == 1 ||
				((obj->assetFlags & BUMPABLE_MULTIPLE_TIMES) && obj->rewardCounter <= 10))
			{
				ObjectLevelInstance_t rewardToSpawn = {0};

				// Spawn reward
				uint32_t rewardType = obj->assetFlags & REWARD_BIT_MASK;
				switch (rewardType)
				{
				case REWARD_SINGLE_COIN: {
					rewardToSpawn.id = FG_REWARD_COIN_OBJECT_ID;
					rewardToSpawn.x = obj->origMapPos.x + (obj->asset.baseAsset.sprite.size.x / 4);
					rewardToSpawn.y = obj->origMapPos.y + obj->asset.baseAsset.sprite.size.y;
					rewardToSpawn.flags = 0;
					break;
				}
				case REWARD_LEVEL_UP: {
					rewardToSpawn.id = FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID;
					rewardToSpawn.x = obj->origMapPos.x;
					rewardToSpawn.y = obj->origMapPos.y;
					rewardToSpawn.flags = 0;
					break;
				}
				default: {
					rewardToSpawn.id = FG_REWARD_COIN_OBJECT_ID;
					rewardToSpawn.x = obj->origMapPos.x + (obj->asset.baseAsset.sprite.size.x / 4);
					rewardToSpawn.y = obj->origMapPos.y + obj->asset.baseAsset.sprite.size.y;
					rewardToSpawn.flags = 0;
					break;
				}
				}

				OBJECTS_MANAGER_OrderSpawn(mgr, &rewardToSpawn);
			}
		}
	}

	return 0;
}

int COLLISION_FGObject_FGObject(ForegroundObject_t* actor, ForegroundObject_t* obj, const Bump_t* bump)
{	
	int retCode = 0;
	
	const int bumpLenX = CalcRectXLen(&bump->bumpRect);
	const int bumpLenY = CalcRectYLen(&bump->bumpRect);

	const int COLLISION_THRESHOLD_VERTICAL = 3;
	const int COLLISION_THRESHOLD_HORIZONTAL = 1;

	Body_t* actorBody = &actor->physics.engine.body;
	
	// 1. VERTICAL COLLISION (UP/DOWN)
	if (bumpLenX >= bumpLenY) {
		if (!(obj->assetFlags & COLL_TOP_ENABLED) && !(obj->assetFlags & COLL_DOWN_ENABLED)) return 0;
		if (bumpLenX <= COLLISION_THRESHOLD_VERTICAL) return 0;

		int bumpCenterY = (bump->bumpRect.p1.y + bump->bumpRect.p2.y) / 2;

		// LANDING ON OBJECT (TOP OF THE OBJECT)
		if (bumpCenterY > obj->BBoxCenter.y) {
			if ((obj->assetFlags & COLL_TOP_ENABLED) && !actor->currFlags.IsGrounded) {
				actor->currFlags.IsGrounded = true;
				actor->currMapPos.y = obj->currMapPos.y + obj->asset.BBox.p2.y;
				actorBody->subpixelY = 0.0f;
				actorBody->vy = 0.0f;
			}
		}
		// BUMPING FROM THE BOTTOM
		else {
			if ((obj->assetFlags & COLL_DOWN_ENABLED)) {
				actorBody->vy = -0.7f;
				actor->currMapPos.y = obj->currMapPos.y - actor->asset.BBox.p2.y;
				retCode = BUMP_SIDE_BOTTOM;
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
				actorBody->vx = 0.3f;
				actorBody->subpixelX = 0.0f;
				actor->currMapPos.x = obj->currMapPos.x + obj->asset.BBox.p2.x + 1; // +1 in order to not "glue" to the object
			}
		}
		// BUMPING FROM LEFT
		else {
			if ((obj->assetFlags & COLL_LEFT_ENABLED)) {
				actorBody->vx = -0.3f;
				actorBody->subpixelX = 0.0f;
				actor->currMapPos.x = obj->currMapPos.x - actor->asset.BBox.p2.x - 1; // - 1 in order to not "glue" to the object
			}
		}
	}

	return retCode;
}

void COLLISION_FGObject_Floor(ForegroundObject_t* actor, const GameContext_t* ctx)
{
	if (!actor->currFlags.IsGrounded) {
		actor->currFlags.IsGrounded = true;
		actor->currMapPos.y = ctx->map.floorYLevel;
		actor->physics.engine.body.subpixelY = 0.0f;
		actor->physics.engine.body.vy = 0.0f;
	}
}
