/*
 * Game.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include "Game.h"

#include <string.h>

#include "Game_Defs.h"
#include "NES_Defs.h"
#include "NES_Functions.h"

#include "Game_Types.h"
#include "Level.h"
#include "GraphicsAssets.h"

#include "printf_logger.h"

#include "Physics.h"
#include "Input.h"
#include "ObjectsManager.h"
#include "Collision.h"
#include "Camera.h"
#include "Animator.h"
#include "Renderer.h"


int GAME_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }

	int timeStamps = 0;
	int ret = 0;
	uint32_t targetFrameTimeUS = 1000000/TARGET_FRAMERATE_HZ;

	GameStats_t* s = &ctx->stats;


	s->startTime[timeStamps] = GetTimestamp();
	ret = INPUT_Update(&ctx->input, ctx, targetFrameTimeUS);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -5; }
	timeStamps++;

	// startTime[timeStamps] = GetTimestamp();
	// FGOBJECTS_ClearFlags(pGameCtx);
	// finishTime[timeStamps] = GetTimestamp();
	// timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = PHYSICS_Player_Update(&ctx->player, ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -10; }
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = CAMERA_Update(&ctx->camera, ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -15; }
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = OBJECTS_MANAGER_Update(ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -20; }
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = ENEMIES_UpdateFlags(&ctx->enemies, ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -25; }
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = PHYSICS_Update(ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -30; }
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = PLAYER_ClearFlags(&ctx->player);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -35; }
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = COLLISION_Update(ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -40; }
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = ANIMATOR_Update(ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	if (ret < 0)	{ delay(1); return -45; }
	timeStamps++;

	if (ctx->firstGameLoop) {
		RENDERER_FirstRender(ctx);
	}

	// startTime[timeStaps] = GetTimestamp();
	// ret = RENDERER_Update(pGameCtx);
	// finishTime[timeStaps] = GetTimestamp();
	// if (ret < 0)	{ delay(1); continue; }
	// timeStaps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = RENDERER_ScrollRender(&ctx->renderer, ctx);
	if (ret < 0)	{ delay(1); return -50; }
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = RENDERER_DirtyRects_Calculate(&ctx->renderer, ctx);
	if (ret < 0)	{ delay(1); return -55; }
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ret = RENDERER_DirtyRects_Render(&ctx->renderer, ctx);
	if (ret < 0)	{ delay(1); return -60; }
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	// Statistics
	if (1 && !ctx->firstGameLoop)
	{
		s->maxTimeSum = 0;
		for (int i = 0; i < timeStamps; i++)
		{
			uint32_t tdiff = CalcDiffTimeUS(s->startTime[i], s->finishTime[i]);
			if (tdiff > s->maxTime[i]) {
				s->maxTime[i] = tdiff;
			}
			s->maxTimeSum += s->maxTime[i];
		}
		uint32_t tdiff = CalcDiffTimeUS(s->startTime[0], s->finishTime[timeStamps-1]);
		if (tdiff > s->maxFrameTime) {
			s->maxFrameTime = tdiff;
		}

		if (CalcTimeMS(s->printStatsTimer) > 1 * 1000) {
			s->printStatsTimer = GetTimestamp();
			printf_v("Frame %d maxFrameTime: %d us, maxTimeSum: %d us, max timestamps:\n", s->frameCounter, s->maxFrameTime, s->maxTimeSum);
			for (int i = 0; i < timeStamps; i++)
			{
				printf_uint(s->maxTime[i]);
				printf_c('\t');
			}
			printf_c('\n');
		}
	}

	if (ctx->firstGameLoop) {
		ctx->firstGameLoop = false;
	}

	s->frameCounter++;

	return 0;
}

int GAME_InitContext(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	memset(ctx, 0, sizeof(GameContext_t));

	ctx->firstGameLoop = true;

	///////////////////
	// STATISTICS
	///////////////////
	ctx->stats.printStatsTimer = GetTimestamp();
	ctx->stats.frameCounter = 0;
	memset(ctx->stats.startTime, 0, sizeof(ctx->stats.startTime));
	memset(ctx->stats.finishTime, 0, sizeof(ctx->stats.finishTime));
	memset(ctx->stats.maxTime, 0, sizeof(ctx->stats.maxTime));
	ctx->stats.maxFrameTime = 0;
	ctx->stats.maxTimeSum = 0;
	
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
	ctx->objectsManager.spawnBufferSize = 0;
	memset(&ctx->objectsManager.spawnBuffer, 0, sizeof(ctx->objectsManager.spawnBuffer));
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

bool MISC_IsThisPlayerID(const GameObjectID id)
{
	if (id >= PLAYER_ID_START && id <= PLAYER_ID_END) {
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



