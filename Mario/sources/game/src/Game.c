/*
 * Game.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include "Game.h"

#include <string.h>

#include "Animator.h"
#include "Camera.h"
#include "Collision.h"
#include "Game_Defs.h"
#include "Game_Types.h"
#include "Geometry.h"
#include "GraphicsAssets.h"
#include "Input.h"
#include "LCDControl.h"
#include "Level.h"
#include "BoardAssert.h"
#include "BoardFunctions.h"
#include "Common_Math.h"
#include "ObjectsManager.h"
#include "Physics.h"
#include "printf_logger.h"
#include "Renderer.h"


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void GAME_HandlePlayerLevelUp(GameContext_t* ctx);
static void ENEMIES_UpdateFlags(Enemies_t* enemies, const GameContext_t* ctx);
static void PLAYER_ClearFlags(PlayerState_t* player);
static void GAME_CalcDirtyRect(const Point_t* prevPos, const Point_t* prevSize, const Point_t* currPos, const Point_t* currSize, Rect_t* dirtyRect);
static bool ENEMIES_CalcIsOnScreen(const EnemyState_t* enemy, const Rect_t* screenRect);


static GameContext_t s_gameCtx = {0};


//----------------
// PUBLIC FUNCTIONS
//----------------
void GAME_Init(void)
{
	GameContext_t* ctx = &s_gameCtx;
	int ret = 0;

	memset(ctx, 0, sizeof(GameContext_t));

	ctx->firstGameLoop = true;
	ctx->simulationPause = false;

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
	BOARD_ASSERT(ret >= 0 && ObjectsPos != NULL);

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
	BOARD_ASSERT(ret >= 0 && BGRepObjectsPos != NULL);

	// Level defines more repetition objects than the context can hold: enlarge the buffer or trim the level data
	BOARD_ASSERT(numOfBGRepObjects <= BACKGROUND_REP_OBJECTS_MAX_SIZE);

	ctx->floorIndex = -1;
	ctx->activebgRepObjects = 0;
	for (int i = 0; i < numOfBGRepObjects; i++)
	{
		// Level entry has an ID outside of the repetition objects range: check the level data
		BOARD_ASSERT(BGRepObjectsPos[i].id >= BACKGROUND_REP_OBJECT_ID_START && BGRepObjectsPos[i].id <= BACKGROUND_REP_OBJECT_ID_END);

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
	BOARD_ASSERT(ctx->floorIndex != -1);

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
	ctx->input.frameData.simulationTimeUS = 0;
	ctx->input.frameData.frameTimeUS = 0;
	ctx->input.frameData.frameTimeS = 0;

	///////////////////
	// PLAYER
	///////////////////
	ctx->player.animableAsset = &MARIO_SMALL_ANIMABLE_ASSET;
	BOARD_ASSERT(ctx->player.animableAsset->baseAssetsCount > 0 && ctx->player.animableAsset->baseAssets[0].baseAsset != NULL);
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
	ctx->player.triggerLevelUp = false;
	ctx->player.levelUpOngoing = false;
	ctx->player.playerLevel = PLAYER_SMALL_MARIO;

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
}

void GAME_Update(void)
{
	GameContext_t* ctx = &s_gameCtx;

	int timeStamps = 0;
	uint32_t targetFrameTimeUS = 1000000/TARGET_FRAMERATE_HZ;

	GameStats_t* s = &ctx->stats;

	// Big event, later use state machine
	GAME_HandlePlayerLevelUp(ctx);

	if (!ctx->simulationPause) {
		ctx->input.frameData.simulationTimeUS += targetFrameTimeUS;
	}

	s->startTime[timeStamps] = GetTimestamp();
	INPUT_Update(&ctx->input, targetFrameTimeUS);
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	// startTime[timeStamps] = GetTimestamp();
	// FGOBJECTS_ClearFlags(pGameCtx);
	// finishTime[timeStamps] = GetTimestamp();
	// timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	if (!ctx->simulationPause) {
		PHYSICS_Player_Update(&ctx->player, ctx);
	}
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	CAMERA_Update(&ctx->camera, ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	OBJECTS_MANAGER_Update(ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ENEMIES_UpdateFlags(&ctx->enemies, ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	if (!ctx->simulationPause) {
		PHYSICS_Update(ctx);
	}
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	PLAYER_ClearFlags(&ctx->player);
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	if (!ctx->simulationPause) {
		COLLISION_Update(ctx);
	}
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	ANIMATOR_Update(ctx);
	s->finishTime[timeStamps] = GetTimestamp();
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
	RENDERER_ScrollRender(&ctx->renderer, ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	RENDERER_DirtyRects_Calculate(&ctx->renderer, ctx);
	s->finishTime[timeStamps] = GetTimestamp();
	timeStamps++;

	s->startTime[timeStamps] = GetTimestamp();
	RENDERER_DirtyRects_Render(&ctx->renderer, ctx);
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

			const ObjectsManager_t* mgr = &ctx->objectsManager;
			printf_v("Dropped FG: %d, enemies: %d, BG: %d, spawn orders: %d\n",
				(int)mgr->droppedFGObjects, (int)mgr->droppedEnemies, (int)mgr->droppedBGObjects, (int)mgr->droppedSpawnOrders);
		}
	}

	if (ctx->firstGameLoop) {
		ctx->firstGameLoop = false;
	}

	s->frameCounter++;
}

bool MISC_IsThisPlayerID(const GameObjectID id)
{
	if (id >= PLAYER_ID_START && id <= PLAYER_ID_END) {
		return true;
	}
	return false;
}

void PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect)
{
	BOARD_ASSERT(player != NULL && dirtyRect != NULL);

	GAME_CalcDirtyRect(&player->prevMapPos, &player->prevSpriteSize, &player->currMapPos, &player->asset.baseAsset.sprite.size, dirtyRect);
}

void PLAYER_LevelUp(PlayerState_t* player)
{
	BOARD_ASSERT(player != NULL);

	switch (player->playerLevel)
	{
	case PLAYER_SMALL_MARIO:
	{
		player->playerLevel = PLAYER_SUPER_MARIO;
		break;
	}
	case PLAYER_SUPER_MARIO:
	{
		player->playerLevel = PLAYER_FIRE_MARIO;
		break;
	}
	case PLAYER_FIRE_MARIO:
	{
		// Add points here and with every levelup
		break;
	}
	default:
		// Unknown player level: add a missing case for it
		BOARD_ASSERT(false);
		break;
	}
}

void ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect)
{
	BOARD_ASSERT(enemy != NULL && dirtyRect != NULL);

	GAME_CalcDirtyRect(&enemy->prevMapPos, &enemy->asset.baseAsset.sprite.size, &enemy->currMapPos, &enemy->asset.baseAsset.sprite.size, dirtyRect);
}

void FGOBJECTS_ClearFlags(GameContext_t* ctx)
{
	BOARD_ASSERT(ctx != NULL);
}

void FGOBJECTS_GetDirtyRect(const ForegroundObject_t* obj, Rect_t* dirtyRect)
{
	BOARD_ASSERT(obj != NULL && dirtyRect != NULL);

	GAME_CalcDirtyRect(&obj->prevMapPos, &obj->asset.baseAsset.sprite.size, &obj->currMapPos, &obj->asset.baseAsset.sprite.size, dirtyRect);
}


//----------------
// PRIVATE FUNCTIONS
//----------------
static void GAME_HandlePlayerLevelUp(GameContext_t* ctx)
{
	if (ctx->player.triggerLevelUp && ctx->player.levelUpOngoing) {
		ctx->simulationPause = true;
	}
	else {
		ctx->simulationPause = false;
	}
}

static void ENEMIES_UpdateFlags(Enemies_t* enemies, const GameContext_t* ctx)
{
	for (int i = 0; i < enemies->activeEnemies; i++)
	{
		int indexLUT = enemies->enemiesLUT[i];

		if (!enemies->IsEnemyActive[indexLUT]) {
			continue;
		}
		enemies->pool[indexLUT].IsOnScreen = ENEMIES_CalcIsOnScreen(&enemies->pool[indexLUT], &ctx->camera.screenRect);
	}
}

static void PLAYER_ClearFlags(PlayerState_t* player)
{
	player->IsGrounded = false;
	player->JustHitFGObjectFromBottom = false;
}

static void GAME_CalcDirtyRect(const Point_t* prevPos, const Point_t* prevSize, const Point_t* currPos, const Point_t* currSize, Rect_t* dirtyRect)
{
	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = *prevPos;
	prevDirtyRect.p2.x = prevPos->x + prevSize->x;
	prevDirtyRect.p2.y = prevPos->y + prevSize->y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = *currPos;
	currDirtyRect.p2.x = currPos->x + currSize->x;
	currDirtyRect.p2.y = currPos->y + currSize->y;

	// Smallest rect covering both the previous and the current sprite area
	dirtyRect->p1.x = MATH_Min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	dirtyRect->p1.y = MATH_Min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	dirtyRect->p2.x = MATH_Max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	dirtyRect->p2.y = MATH_Max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	// Inverted rect means a negative sprite size: check the asset definition
	BOARD_ASSERT(dirtyRect->p1.x <= dirtyRect->p2.x && dirtyRect->p1.y <= dirtyRect->p2.y);
}

static bool ENEMIES_CalcIsOnScreen(const EnemyState_t* enemy, const Rect_t* screenRect)
{
	Rect_t enemyRect;
	enemyRect.p1 = enemy->currMapPos;
	enemyRect.p2.x = enemy->currMapPos.x + enemy->asset.baseAsset.sprite.size.x;
	enemyRect.p2.y = enemy->currMapPos.y + enemy->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	RECT_GetIntersection(&enemyRect, screenRect, &commonRect);

	if (RECT_IsIntersection(&commonRect)) {
		return true;
	} else {
		return false;
	}
}
