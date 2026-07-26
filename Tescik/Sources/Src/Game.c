/*
 * Game.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include "string.h"

#include "NES_Functions.h"

#include "LCDControl.h"
#include "PADControl.h"
#include "RenderEngine.h"

#include "Game_Types.h"
#include "Assets.h"
#include "Level.h"

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
	// BACKGROUND
	///////////////////
	for (int i = 0; i < BACKGROUND_OBJECTS_MAX_SIZE; i++)
	{
		ctx->bgObjects[i].id = OBJECT_NOT_USED;
	}

	ctx->activebgObjects = 0;
	for (int i = 0; i < numOfObjects; i++)
	{
		if (ObjectsPos[i].id < BACKGROUND_OBJECT_ID_START || ObjectsPos[i].id > BACKGROUND_OBJECT_ID_END)
		{
			continue;
		}

		BackgroundObject_t* bgObject = &ctx->bgObjects[ctx->activebgObjects];

		bgObject->id = ObjectsPos[i].id;
		bgObject->mapPos.x = ObjectsPos[i].x;
		bgObject->mapPos.y = ObjectsPos[i].y;

		switch (ObjectsPos[i].id)
		{
		case BG_JEDYNKA_OBJECT_ID: {
			bgObject->asset = &JEDYNKA_ASSET;
			break;
		}
		case BG_DWOJKA_OBJECT_ID: {
			bgObject->asset = &DWOJKA_ASSET;
			break;
		}
		case BG_CHMURKA_OBJECT_ID: {
			bgObject->asset = &CHMURKA_ASSET;
			break;
		}
		default:
			break;
		}

		if (ctx->activebgObjects >= BACKGROUND_OBJECTS_MAX_SIZE - 1) {
			printf_s("\n### ERROR, max FGObjects reached ###\n");
			break;
		}

		ctx->activebgObjects++;
	}

	///////////////////
	// FOREGROUND
	///////////////////
	for (int i = 0; i < FOREGROUND_OBJECTS_MAX_SIZE; i++)
	{
		ctx->fgObjects[i].id = OBJECT_NOT_USED;
	}

	ctx->activefgObjects = 0;
	for (int i = 0; i < numOfObjects; i++)
	{
		if (ObjectsPos[i].id < FOREGROUND_OBJECT_ID_START || ObjectsPos[i].id > FOREGROUND_OBJECT_ID_END)
		{
			continue;
		}

		ForegroundObject_t* fgObject = &ctx->fgObjects[ctx->activefgObjects];

		switch (ObjectsPos[i].id)
		{
		case FG_BRICKS_OBJECT_ID: {
			fgObject->asset = &BRICKS_ASSET;
			break;
		}
		case FG_BLOCK_QMARK_OBJECT_ID: {
			fgObject->asset = &BLOCK_QMARK_ASSET;
			break;
		}
		case FG_RURA_DOL_OBJECT_ID: {
			fgObject->asset = &RURA_DOL_ASSET;
			break;
		}
		case FG_RURA_GORA_OBJECT_ID: {
			fgObject->asset = &RURA_GORA_ASSET;
			break;
		}
		case FG_PYRAMID_BLOCK_OBJECT_ID: {
			fgObject->asset = &PYRAMID_BLOCK_ASSET;
			break;
		}
		default:
			break;
		}

		fgObject->id = ObjectsPos[i].id;
		fgObject->flags = ObjectsPos[i].flags;
		fgObject->mapPos.x = ObjectsPos[i].x;
		fgObject->mapPos.y = ObjectsPos[i].y;
		fgObject->BBoxCenter.x = ObjectsPos[i].x + (fgObject->asset->BBox.p1.x + fgObject->asset->BBox.p2.x) / 2;
		fgObject->BBoxCenter.y = ObjectsPos[i].y + (fgObject->asset->BBox.p1.y + fgObject->asset->BBox.p2.y) / 2;
		fgObject->IsAlive = true;
		fgObject->IsOnScreen = true;

		if (ctx->activefgObjects >= FOREGROUND_OBJECTS_MAX_SIZE - 1) {
			printf_s("\n### ERROR, max FGObjects reached ###\n");
			break;
		}

		ctx->activefgObjects++;
	}


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
	ctx->player.asset = &MARIO_ASSET;
	ctx->player.id = ctx->player.asset->baseAsset.id;
	ctx->player.currMapPos.x = 80;
	ctx->player.currMapPos.y = ctx->map.floorYLevel;
	ctx->player.prevMapPos = ctx->player.currMapPos;
	ctx->player.body.vx = 0.0f;
	ctx->player.body.vy = 0.0f;
	ctx->player.body.subpixelX = 0.0f;
	ctx->player.body.subpixelY = 0.0f;
	ctx->player.collCtx.size = 0;
	fast_memset(&ctx->player.collCtx, 0, sizeof(ctx->player.collCtx));
	ctx->player.lifePoints = 1;
	ctx->player.IsImmune = false;
	ctx->player.damageTaken = false;
	ctx->player.IsGrounded = true;
	ctx->player.JustKilledFGObject = false;
	ctx->player.playerLevel = PLAYER_BIG;

	///////////////////
	// ENEMIES
	///////////////////
	const ObjectLevelInstance_t* EnemiesPos = NULL;
	int numOfEnemies = 0;
	ret = LEVEL_GetEnemiesLocations(&EnemiesPos, &numOfEnemies);
	if (ret < 0) { return -30; }
	if (EnemiesPos == NULL) { return -35; }

	if (numOfEnemies > ENEMIES_MAX_SIZE)
	{
		numOfEnemies = ENEMIES_MAX_SIZE;
	}

	ctx->enemies.activeEnemies = numOfEnemies;
	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		if (EnemiesPos[i].id < ENEMY_ID_START || EnemiesPos[i].id > ENEMY_ID_END)
		{
			continue;
		}

		ctx->enemies.pool[i].id = EnemiesPos[i].id;
		ctx->enemies.pool[i].IsAlive = true;
		ctx->enemies.pool[i].IsOnScreen = false;
		ctx->enemies.pool[i].currMapPos.x = EnemiesPos[i].x;
		ctx->enemies.pool[i].currMapPos.y = EnemiesPos[i].y;
		ctx->enemies.pool[i].prevMapPos = ctx->enemies.pool[i].currMapPos;

		switch (EnemiesPos[i].id)
		{
		case ENEMY_GOOMBA_ID: {
			ctx->enemies.pool[i].asset = &GOOMBA_ASSET;
			break;
		}
		}
	}

	///////////////////
	// RENDERER
	///////////////////
	ctx->renderer.LCDOffsetX = 0;
	fast_memset(ctx->renderer.dirtyRects, 0, sizeof(ctx->renderer.dirtyRects));

	///////////////////
	// COLLISION
	///////////////////
	ctx->coll.size = 0;
	fast_memset(ctx->coll.bumps, 0, sizeof(ctx->coll.bumps));

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

int	COLLISION_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	ret = COLLISION_Calculate(&ctx->coll, ctx);
	if (ret < 0) { return -5; }

	ret = COLLISION_Resolve(ctx);
	if (ret < 0) { return -10; }

	return 0;
}

int	COLLISION_Calculate(CollState_t* coll, const GameContext_t* ctx)
{
	if (coll == NULL || ctx == NULL) { return -1; }

	coll->size = 0;
	fast_memset(&coll->bumps, 0, sizeof(coll->bumps));

	Rect_t playerRect;
	playerRect.p1.x = ctx->player.currMapPos.x + ctx->player.asset->BBox.p1.x;
	playerRect.p1.y = ctx->player.currMapPos.y + ctx->player.asset->BBox.p1.y;
	playerRect.p2.x = ctx->player.currMapPos.x + ctx->player.asset->BBox.p2.x;
	playerRect.p2.y = ctx->player.currMapPos.y + ctx->player.asset->BBox.p2.y;

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
		const ForegroundObject_t* fgObject = &ctx->fgObjects[i];
		if (!fgObject->IsOnScreen) {
			continue;
		}
		if (!fgObject->IsAlive) { continue; }

		Rect_t objRect;
		objRect.p1.x = fgObject->mapPos.x + fgObject->asset->BBox.p1.x;
		objRect.p1.y = fgObject->mapPos.y + fgObject->asset->BBox.p1.y;
		objRect.p2.x = fgObject->mapPos.x + fgObject->asset->BBox.p2.x;
		objRect.p2.y = fgObject->mapPos.y + fgObject->asset->BBox.p2.y;

		Rect_t bumpRect = {0};
		Rect_GetIntersection(&playerRect, &objRect, &bumpRect);

		if (Rect_IsIntersection(&bumpRect)) {
			if (coll->size < COLLISIONS_SIZE) {
				coll->bumps[coll->size].bumpID = PLAYER_BUMP_FG_OBJECT;
				coll->bumps[coll->size].actor1 = (GameObjectRef_t){ .id = ctx->player.id, .index = 0 };
				coll->bumps[coll->size].actor2 = (GameObjectRef_t){ .id = fgObject->id, .index = i };
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
		const EnemyState_t* enemy = &ctx->enemies.pool[i];
		if (!enemy->IsOnScreen) {
			continue;
		}

		Rect_t enemyRect;
		enemyRect.p1.x = enemy->currMapPos.x + enemy->asset->BBox.p1.x;
		enemyRect.p1.y = enemy->currMapPos.y + enemy->asset->BBox.p1.y;
		enemyRect.p2.x = enemy->currMapPos.x + enemy->asset->BBox.p2.x;
		enemyRect.p2.y = enemy->currMapPos.y + enemy->asset->BBox.p2.y;

		Rect_t bumpRect = {0};
		Rect_GetIntersection(&playerRect, &enemyRect, &bumpRect);

		if (Rect_IsIntersection(&bumpRect)) {
			if (coll->size < COLLISIONS_SIZE) {
				coll->bumps[coll->size].bumpID = PLAYER_BUMP_ENEMY;
				coll->bumps[coll->size].actor1 = (GameObjectRef_t){ .id = ctx->player.id, .index = 0 };
				coll->bumps[coll->size].actor2 = (GameObjectRef_t){ .id = enemy->id, .index = i };
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

	for (int i = 0; i < ctx->coll.size; i++)
	{
		Bump_t* bump = &ctx->coll.bumps[i];
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

	return 0;
}

int COLLISION_Player_FGObject(PlayerState_t* player, ForegroundObject_t* obj, const Bump_t* bump, const GameContext_t* ctx)
{
	if (player == NULL || obj == NULL || bump == NULL || ctx == NULL) { return -1; }

	if (!(obj->flags & COLL_ANY_ENABLED)) { return 0; }

	const int bumpLenX = CalcRectXLen(bump->bumpRect);
	const int bumpLenY = CalcRectYLen(bump->bumpRect);

	const int COLLISION_THRESHOLD_VERTICAL = 3;
	const int COLLISION_THRESHOLD_HORIZONTAL = 1;

	// 1. VERTICAL COLLISION (UP/DOWN)
	if (bumpLenX >= bumpLenY) {
		if (!(obj->flags & COLL_TOP_ENABLED) && !(obj->flags & COLL_DOWN_ENABLED)) return 0;
		if (bumpLenX <= COLLISION_THRESHOLD_VERTICAL) return 0;

		int bumpCenterY = (bump->bumpRect.p1.y + bump->bumpRect.p2.y) / 2;

		// LANDING ON OBJECT (TOP OF THE OBJECT)
		if (bumpCenterY > obj->BBoxCenter.y) {
			if ((obj->flags & COLL_TOP_ENABLED) && !player->IsGrounded) {
				player->IsGrounded = true;
				player->currMapPos.y = obj->mapPos.y + obj->asset->BBox.p2.y;
				player->body.subpixelY = 0.0f;
				player->body.vy = 0.0f;
			}
		}
		// BUMPING FROM THE BOTTOM
		else {
			if ((obj->flags & COLL_DOWN_ENABLED)) {
				player->body.vy = -0.5f;
				player->currMapPos.y = obj->mapPos.y - player->asset->BBox.p2.y;

				bool isPlayerBig = (player->playerLevel == PLAYER_BIG || player->playerLevel == PLAYER_SHOOTING) ? true : false;
				if (isPlayerBig && !player->JustKilledFGObject) {
					player->JustKilledFGObject = true;
					obj->IsAlive = false;
				}
			}
		}
	}
	// 2. HORIZONTAL COLLISION (LEFT/RIGHT)
	else {
		if (!(obj->flags & COLL_LEFT_ENABLED) && !(obj->flags & COLL_RIGHT_ENABLED)) return 0;
		if (bumpLenY <= COLLISION_THRESHOLD_HORIZONTAL) return 0;

		int centerBumpX = (bump->bumpRect.p1.x + bump->bumpRect.p2.x) / 2;

		// BUMPING FROM RIGHT
		if (centerBumpX > obj->BBoxCenter.x) {
			if ((obj->flags & COLL_RIGHT_ENABLED)) {
				player->body.vx = 0.0f;
				player->body.subpixelX = 0.0f;
				player->currMapPos.x = obj->mapPos.x + obj->asset->BBox.p2.x + 1; // +1 in order to not "glue" to the object
			}
		}
		// BUMPING FROM LEFT
		else {
			if ((obj->flags & COLL_LEFT_ENABLED)) {
				player->body.vx = 0.0f;
				player->body.subpixelX = 0.0f;
				player->currMapPos.x = obj->mapPos.x - player->asset->BBox.p2.x - 1; // - 1 in order to not "glue" to the object
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

	PLAYER_TakeDamage(player, 1);

	return 0;
}

int COLLISION_Player_Resolve(PlayerState_t* player, const CollState_t* coll, const GameContext_t* ctx)
{
	if (player == NULL || coll == NULL || ctx == NULL) { return -1; }

	for (int i = 0; i < coll->size; i++)
	{
		const Bump_t* bump = &coll->bumps[i];
		if (bump->actor1.id != player->id || bump->actor2.id != player->id) {
			continue;
		}

		const GameObjectRef_t* actor = (bump->actor1.id == player->id) ? &bump->actor2 : &bump->actor1;

		switch (actor->id)
		{
		case ENEMY_GOOMBA_ID: {
			player->lifePoints--;
			if (player->lifePoints <= 0) {
				//???
			}
			break;
		}
		}
	}

	return 0;
}

int PHYSICS_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	ret = PHYSICS_Player_Update(&ctx->player, ctx);
	if (ret < 0) { return -5; }

	return 0;
}

int PHYSICS_Player_Update(PlayerState_t* player, const GameContext_t* ctx)
{
	if (player == NULL || ctx == NULL) { return -1; }
	int ret = 0;

	ret = PHYSICS_Player_Movement(player, ctx);
	if (ret < 0) { return -5; }

	ret = PHYSICS_Player_CalcMapPos(player, ctx);
	if (ret < 0) { return -10; }

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
			float dvx = ctx->input.frameData.frameTimeS * 3;
			if (player->body.vx + dvx < 1.0f) {
				player->body.vx += dvx;
			} else {
				player->body.vx = 1.0f;
			}
		}
	} else if (ctx->input.buttons_state & PAD_BUTTON_LEFT) {
		if (player->body.vx > -1.0f) {
			float dvx = ctx->input.frameData.frameTimeS * 3;
			if (player->body.vx - dvx > -1.0f) {
				player->body.vx -= dvx;
			} else {
				player->body.vx = -1.0f;
			}
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

	int diff = player->currMapPos.x - player->prevMapPos.x;
	if (diff >= 0 && camera->currPos.x + diff >= 0 && camera->currPos.x + diff <= levelBounds->p2.x) {
		if (player->currMapPos.x - camera->currPos.x >= 80) {
			camera->currPos.x += diff;
		}

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

int PLAYER_ClearFlags(PlayerState_t* player)
{
	if (player == NULL) { return -1; }

	player->IsGrounded = false;
	player->JustKilledFGObject = false;

	return 0;
}

int PLAYER_TakeDamage(PlayerState_t* player, int damage)
{
	if (player == NULL) { return -1; }

	if (player->IsImmune && player->lifePoints > 0) {
		player->lifePoints--;
		player->damageTaken = true;
	}

	return 0;
}



int PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect)
{
	if (player == NULL || dirtyRect == NULL) { return -1; }

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = player->prevMapPos;
	prevDirtyRect.p2.x = player->prevMapPos.x + player->asset->baseAsset.sprite.size.x;
	prevDirtyRect.p2.y = player->prevMapPos.y + player->asset->baseAsset.sprite.size.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = player->currMapPos;
	currDirtyRect.p2.x = player->currMapPos.x + player->asset->baseAsset.sprite.size.x;
	currDirtyRect.p2.y = player->currMapPos.y + player->asset->baseAsset.sprite.size.y;

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
	int ret = 0;

	for (int i = 0; i < enemies->activeEnemies; i++)
	{
		enemies->pool[i].IsOnScreen = ENEMIES_CalcIsOnScreen(&enemies->pool[i], &ctx->camera.screenRect);
	}

	return 0;
}

int ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect)
{
	if (enemy == NULL || dirtyRect == NULL) { return -1; }

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = enemy->prevMapPos;
	prevDirtyRect.p2.x = enemy->prevMapPos.x + enemy->asset->baseAsset.sprite.size.x;
	prevDirtyRect.p2.y = enemy->prevMapPos.y + enemy->asset->baseAsset.sprite.size.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = enemy->currMapPos;
	currDirtyRect.p2.x = enemy->currMapPos.x + enemy->asset->baseAsset.sprite.size.x;
	currDirtyRect.p2.y = enemy->currMapPos.y + enemy->asset->baseAsset.sprite.size.y;

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

bool ENEMIES_CalcIsOnScreen(const EnemyState_t* enemy, const Rect_t* screenRect)
{
	if (enemy == NULL || screenRect == NULL)	{ return false; }

	Rect_t enemyRect;
	enemyRect.p1 = enemy->currMapPos;
	enemyRect.p2.x = enemy->currMapPos.x + enemy->asset->baseAsset.sprite.size.x;
	enemyRect.p2.y = enemy->currMapPos.y + enemy->asset->baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(&enemyRect, screenRect, &commonRect);

	if (Rect_IsIntersection(&commonRect)) {
		return true;
	} else {
		return false;
	}
}

//int FGOBJECTS_GetDirtyRect(const ForegroundObject_t* obj, Rect_t* dirtyRect)
//{
//	if (obj == NULL || dirtyRect == NULL) { return -1; }
//
//	Rect_t prevDirtyRect;
//	prevDirtyRect.p1 = obj->prevMapPos;
//	prevDirtyRect.p2.x = obj->prevMapPos.x + obj->asset->baseAsset.sprite.size.x;
//	prevDirtyRect.p2.y = obj->prevMapPos.y + obj->asset->baseAsset.sprite.size.y;
//
//	Rect_t currDirtyRect;
//	currDirtyRect.p1 = obj->currMapPos;
//	currDirtyRect.p2.x = obj->currMapPos.x + obj->asset->baseAsset.sprite.size.x;
//	currDirtyRect.p2.y = obj->currMapPos.y + obj->asset->baseAsset.sprite.size.y;
//
//	Rect_t commonDirtyRect;
//	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
//	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
//	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
//	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);
//
//	if (commonDirtyRect.p1.x <= commonDirtyRect.p2.x && commonDirtyRect.p1.y <= commonDirtyRect.p2.y)
//	{
//		*dirtyRect = commonDirtyRect;
//	}
//	else
//	{
//		return -5;
//	}
//
//	return 0;
//}

int RENDERER_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	ret = RENDERER_ScrollRender(&ctx->renderer, ctx);
	if (ret < 0) { return -1; }

	ret = RENDERER_RenderObjects(&ctx->renderer, ctx);
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

	for (int i = 0; i < LCD_WIDTH/20; i++)
	{
		Rect_t mapRect;
		mapRect.p1.x = i * 20;
		mapRect.p1.y = ctx->map.floorYLevel;
		mapRect.p2.x = i * 20 + 20;
		mapRect.p2.y = LCD_HEIGHT;

		Rect_t screenRect = mapRect;

		int baseRectArea = CalcRectArea(screenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			Point_t spritePos = ctx->bgObjects[j].mapPos;
			RENDERER_RenderBgdSprite(&ctx->bgObjects[j].asset->baseAsset.sprite, spritePos, mapRect, screenRect, ctx->renderer.LCDOffsetX, false, false);
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


		for (int i = 0; i < ctx->activebgObjects; i++)
		{
			Point_t spritePos = ctx->bgObjects[i].mapPos;
			RENDERER_RenderBgdSprite(&ctx->bgObjects[i].asset->baseAsset.sprite, spritePos, rightMapRect, rightScreenRect, renderer->LCDOffsetX, false, false);
		}


		const int* prioArray = NULL;
		int numOfPriorities = 0;
		int ret = LEVEL_GetObjRenderPriorities(&prioArray, &numOfPriorities);
		if (ret < 0) { return -5; }

		for (int i = 0; i < numOfPriorities; i++)
		{
			int currentPrioObject = prioArray[i];

			for (int j = 0; j < ctx->activefgObjects; j++)
			{
				const ForegroundObject_t* obj = &ctx->fgObjects[j];

				if (obj->id != currentPrioObject) {
					continue;
				}

				switch (obj->id)
				{
				case FG_RURA_DOL_OBJECT_ID:
				case FG_RURA_GORA_OBJECT_ID:
				case FG_BLOCK_QMARK_OBJECT_ID:
				case FG_BRICKS_OBJECT_ID:
				case FG_PYRAMID_BLOCK_OBJECT_ID:
				{
					if (!obj->IsAlive) { break; }
					if (!(obj->flags & FG_SCROLL_RENDER)) { break; }

					Rect_t posRect;
					posRect.p1.x = obj->mapPos.x;
					posRect.p1.y = obj->mapPos.y;
					posRect.p2.x = obj->mapPos.x + obj->asset->baseAsset.sprite.size.x;
					posRect.p2.y = obj->mapPos.y + obj->asset->baseAsset.sprite.size.y;

					Rect_t commonRect;
					commonRect.p1.x = max(rightMapRect.p1.x, posRect.p1.x);
					commonRect.p1.y = max(rightMapRect.p1.y, posRect.p1.y);
					commonRect.p2.x = min(rightMapRect.p2.x, posRect.p2.x);
					commonRect.p2.y = min(rightMapRect.p2.y, posRect.p2.y);

					// czesc wspolna istnieje
					if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
					{
						SpriteRender_t renderContext;
						renderContext.commonRect = commonRect;
						renderContext.baseRect = rightScreenRect;
						renderContext.baseToSpriteOffset.x = obj->mapPos.x - rightMapRect.p1.x;
						renderContext.baseToSpriteOffset.y = obj->mapPos.y - rightMapRect.p1.y;
						renderContext.LCDOffsetX = renderer->LCDOffsetX;

						RE_FillSprite3(&obj->asset->baseAsset.sprite, renderContext);
					}
					break;
				}
				default:
					break;
				}
			}
		}

		RE_SendRect(rightScreenRect, renderer->LCDOffsetX);
	}

	return 0;
}

int RENDERER_RenderObjects(RendererState_t* renderer, const GameContext_t* ctx)
{
	if (renderer == NULL || ctx == NULL) { return -1; }

	int ret = 0;
	int dirtyRectIndex = 0;

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
		if ((ctx->fgObjects[i].flags & FG_SCROLL_RENDER)) { break; }

		Rect_t dirtyRect;
		dirtyRect.p1 = ctx->fgObjects[i].mapPos;
		dirtyRect.p2.x = ctx->fgObjects[i].mapPos.x + ctx->fgObjects[i].asset->baseAsset.sprite.size.x;
		dirtyRect.p2.y = ctx->fgObjects[i].mapPos.y + ctx->fgObjects[i].asset->baseAsset.sprite.size.y;

		Rect_t commonRect;
		commonRect.p1.x = max(cameraRect.p1.x, dirtyRect.p1.x);
		commonRect.p1.y = max(cameraRect.p1.y, dirtyRect.p1.y);
		commonRect.p2.x = min(cameraRect.p2.x, dirtyRect.p2.x);
		commonRect.p2.y = min(cameraRect.p2.y, dirtyRect.p2.y);

		// czesc wspolna istnieje
		if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
		{
			dirtyRects[dirtyRectIndex].rect = commonRect;
			dirtyRects[dirtyRectIndex].objects[dirtyRects[dirtyRectIndex].objectsSize++] = (ObjectRef_t){ .id = ctx->fgObjects[i].id, .index = i };
			dirtyRectIndex++;
		}
	}

	//////////////////////////
	// ENEMIES DIRTY RECTS
	//////////////////////////
	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		Rect_t dirtyRect;
		if (ENEMIES_GetDirtyRect(&ctx->enemies.pool[i], &dirtyRect) < 0) { continue; }

		Rect_t commonRect;
		commonRect.p1.x = max(cameraRect.p1.x, dirtyRect.p1.x);
		commonRect.p1.y = max(cameraRect.p1.y, dirtyRect.p1.y);
		commonRect.p2.x = min(cameraRect.p2.x, dirtyRect.p2.x);
		commonRect.p2.y = min(cameraRect.p2.y, dirtyRect.p2.y);

		// czesc wspolna istnieje
		if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
		{
			dirtyRects[dirtyRectIndex].rect = commonRect;
			dirtyRects[dirtyRectIndex].objects[dirtyRects[dirtyRectIndex].objectsSize++] = (ObjectRef_t){ .id = ctx->enemies.pool[i].id, .index = i };
			dirtyRectIndex++;
		}
	}

	//////////////////////////
	// PLAYER DIRTY RECT
	//////////////////////////
	Rect_t dirtyRect;
	if (!PLAYER_GetDirtyRect(&ctx->player, &dirtyRect))
	{
		Rect_t commonRect;
		commonRect.p1.x = max(cameraRect.p1.x, dirtyRect.p1.x);
		commonRect.p1.y = max(cameraRect.p1.y, dirtyRect.p1.y);
		commonRect.p2.x = min(cameraRect.p2.x, dirtyRect.p2.x);
		commonRect.p2.y = min(cameraRect.p2.y, dirtyRect.p2.y);

		// czesc wspolna istnieje
		if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
		{
			dirtyRects[dirtyRectIndex].rect = commonRect;
			dirtyRects[dirtyRectIndex].objects[dirtyRects[dirtyRectIndex].objectsSize++] = (ObjectRef_t){ .id = ctx->player.id, .index = 0 };
			dirtyRectIndex++;
		}
	}


	//////////////////////////
	// COMPOUNDING OVERLAPPING DIRTY RECTS
	//////////////////////////
	for (int i = 0; i < dirtyRectIndex; i++)
	{
		if (dirtyRects[i].used)	{ continue; }

		bool commonRectFound = false;
		for (int j = 0; j < dirtyRectIndex; j++)
		{
			if (i == j)	{ continue; }
			if (dirtyRects[j].used)	{ continue; }

			Rect_t commonRect;
			commonRect.p1.x = max(dirtyRects[i].rect.p1.x, dirtyRects[j].rect.p1.x);
			commonRect.p1.y = max(dirtyRects[i].rect.p1.y, dirtyRects[j].rect.p1.y);
			commonRect.p2.x = min(dirtyRects[i].rect.p2.x, dirtyRects[j].rect.p2.x);
			commonRect.p2.y = min(dirtyRects[i].rect.p2.y, dirtyRects[j].rect.p2.y);

			// czesc wspolna istnieje
			if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
			{
				Rect_t commonORRect;
				commonORRect.p1.x = min(dirtyRects[i].rect.p1.x, dirtyRects[j].rect.p1.x);
				commonORRect.p1.y = min(dirtyRects[i].rect.p1.y, dirtyRects[j].rect.p1.y);
				commonORRect.p2.x = max(dirtyRects[i].rect.p2.x, dirtyRects[j].rect.p2.x);
				commonORRect.p2.y = max(dirtyRects[i].rect.p2.y, dirtyRects[j].rect.p2.y);

				commonRectFound = true;
				dirtyRects[j].used = true;
				dirtyRects[i].rect = commonORRect;
				for (int k = 0; k < dirtyRects[j].objectsSize; k++)
				{
					dirtyRects[i].objects[dirtyRects[i].objectsSize++] = dirtyRects[j].objects[k];
				}
				break;
			}
		}

		if (commonRectFound)
		{
			i--;
		}
	}

	for (int i = 0; i < dirtyRectIndex; i++)
	{
		if (dirtyRects[i].used)	{ continue; }

		DirtyRect_t* dirtyRect = &dirtyRects[i];

		Rect_t screenRect;
		screenRect.p1.x = dirtyRect->rect.p1.x - ctx->camera.currPos.x;
		screenRect.p1.y = dirtyRect->rect.p1.y - ctx->camera.currPos.y;
		screenRect.p2.x = dirtyRect->rect.p2.x - ctx->camera.currPos.x;
		screenRect.p2.y = dirtyRect->rect.p2.y - ctx->camera.currPos.y;

		// BACKGROUND COLOR
		int baseRectArea = CalcRectArea(dirtyRect->rect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		// BACKGROUND SPRITES
		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			Point_t spritePos = ctx->bgObjects[j].mapPos;
			RENDERER_RenderBgdSprite(&ctx->bgObjects[j].asset->baseAsset.sprite, spritePos, dirtyRect->rect, screenRect, renderer->LCDOffsetX, false, false);
		}


		const int* prioArray = NULL;
		int numOfPriorities = 0;
		ret = LEVEL_GetObjRenderPriorities(&prioArray, &numOfPriorities);
		if (ret < 0) { return -5; }

		for (int j = 0; j < numOfPriorities; j++)
		{
			int currentPrioObject = prioArray[j];

			for (int k = 0; k < ctx->activefgObjects; k++)
			{
				const ForegroundObject_t* obj = &ctx->fgObjects[k];

				if (obj->id != currentPrioObject) { continue; }
				if (!obj->IsAlive) { continue; }

				switch (obj->id)
				{
				case FG_RURA_DOL_OBJECT_ID:
				case FG_RURA_GORA_OBJECT_ID:
				case FG_BLOCK_QMARK_OBJECT_ID:
				case FG_BRICKS_OBJECT_ID:
				case FG_PYRAMID_BLOCK_OBJECT_ID:
				{
					Rect_t posRect;
					posRect.p1.x = obj->mapPos.x;
					posRect.p1.y = obj->mapPos.y;
					posRect.p2.x = obj->mapPos.x + obj->asset->baseAsset.sprite.size.x;
					posRect.p2.y = obj->mapPos.y + obj->asset->baseAsset.sprite.size.y;

					Rect_t commonRect;
					commonRect.p1.x = max(dirtyRect->rect.p1.x, posRect.p1.x);
					commonRect.p1.y = max(dirtyRect->rect.p1.y, posRect.p1.y);
					commonRect.p2.x = min(dirtyRect->rect.p2.x, posRect.p2.x);
					commonRect.p2.y = min(dirtyRect->rect.p2.y, posRect.p2.y);

					// czesc wspolna istnieje
					if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
					{
						SpriteRender_t renderContext;
						renderContext.commonRect = commonRect;
						renderContext.baseRect = screenRect;
						renderContext.baseToSpriteOffset.x = obj->mapPos.x - dirtyRect->rect.p1.x;
						renderContext.baseToSpriteOffset.y = obj->mapPos.y - dirtyRect->rect.p1.y;
						renderContext.LCDOffsetX = renderer->LCDOffsetX;

						RE_FillSprite3(&obj->asset->baseAsset.sprite, renderContext);
					}
					break;
				}
				default:
					break;
				}
			}

		}

		for (int j = 0; j < numOfPriorities; j++)
		{
			int currentPrioObject = prioArray[j];

			for (int k = 0; k < dirtyRect->objectsSize; k++)
			{
				GameObjectID id = dirtyRect->objects[k].id;
				if (id != currentPrioObject)
				{
					continue;
				}

				switch (id)
				{
				case PLAYER_MARIO_ID:
				{
					Rect_t posRect;
					posRect.p1.x = ctx->player.currMapPos.x;
					posRect.p1.y = ctx->player.currMapPos.y;
					posRect.p2.x = ctx->player.currMapPos.x + ctx->player.asset->baseAsset.sprite.size.x;
					posRect.p2.y = ctx->player.currMapPos.y + ctx->player.asset->baseAsset.sprite.size.y;

					Rect_t commonRect;
					commonRect.p1.x = max(dirtyRect->rect.p1.x, posRect.p1.x);
					commonRect.p1.y = max(dirtyRect->rect.p1.y, posRect.p1.y);
					commonRect.p2.x = min(dirtyRect->rect.p2.x, posRect.p2.x);
					commonRect.p2.y = min(dirtyRect->rect.p2.y, posRect.p2.y);

					// czesc wspolna istnieje
					if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
					{
						SpriteRender_t renderContext;
						renderContext.commonRect = commonRect;
						renderContext.baseRect = screenRect;
						renderContext.baseToSpriteOffset.x = ctx->player.currMapPos.x - dirtyRect->rect.p1.x;
						renderContext.baseToSpriteOffset.y = ctx->player.currMapPos.y - dirtyRect->rect.p1.y;
						renderContext.LCDOffsetX = renderer->LCDOffsetX;

						RE_FillSprite3(&ctx->player.asset->baseAsset.sprite, renderContext);
					}
					break;
				}
				case ENEMY_GOOMBA_ID:
				{
					if (dirtyRect->objects[k].index < 0 || dirtyRect->objects[k].index >= ENEMIES_MAX_SIZE)
					{
						break;
					}

					const EnemyState_t* enemy = &ctx->enemies.pool[dirtyRect->objects[k].index];

					Rect_t posRect;
					posRect.p1.x = enemy->currMapPos.x;
					posRect.p1.y = enemy->currMapPos.y;
					posRect.p2.x = enemy->currMapPos.x + enemy->asset->baseAsset.sprite.size.x;
					posRect.p2.y = enemy->currMapPos.y + enemy->asset->baseAsset.sprite.size.y;

					Rect_t commonRect;
					commonRect.p1.x = max(dirtyRect->rect.p1.x, posRect.p1.x);
					commonRect.p1.y = max(dirtyRect->rect.p1.y, posRect.p1.y);
					commonRect.p2.x = min(dirtyRect->rect.p2.x, posRect.p2.x);
					commonRect.p2.y = min(dirtyRect->rect.p2.y, posRect.p2.y);

					// czesc wspolna istnieje
					if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
					{
						SpriteRender_t renderContext;
						renderContext.commonRect = commonRect;
						renderContext.baseRect = screenRect;
						renderContext.baseToSpriteOffset.x = enemy->currMapPos.x - dirtyRect->rect.p1.x;
						renderContext.baseToSpriteOffset.y = enemy->currMapPos.y - dirtyRect->rect.p1.y;
						renderContext.LCDOffsetX = renderer->LCDOffsetX;

						RE_FillSprite3(&enemy->asset->baseAsset.sprite, renderContext);
					}
					break;
				}
//				case FG_RURA_DOL_OBJECT_ID:
//				case FG_RURA_GORA_OBJECT_ID:
//				case FG_BLOCK_QMARK_OBJECT_ID:
//				case FG_BRICKS_OBJECT_ID:
//				case FG_PYRAMID_BLOCK_OBJECT_ID:
//				{
//					if (dirtyRect->objects[k].index < 0 || dirtyRect->objects[k].index >= FOREGROUND_OBJECTS_MAX_SIZE)
//					{
//						break;
//					}
//
//					const ForegroundObject_t* obj = &ctx->fgObjects[dirtyRect->objects[k].index];
//					if (!obj->IsAlive) { break; }
//
//					Rect_t posRect;
//					posRect.p1.x = obj->mapPos.x;
//					posRect.p1.y = obj->mapPos.y;
//					posRect.p2.x = obj->mapPos.x + obj->asset->baseAsset.sprite.size.x;
//					posRect.p2.y = obj->mapPos.y + obj->asset->baseAsset.sprite.size.y;
//
//					Rect_t commonRect;
//					commonRect.p1.x = max(dirtyRect->rect.p1.x, posRect.p1.x);
//					commonRect.p1.y = max(dirtyRect->rect.p1.y, posRect.p1.y);
//					commonRect.p2.x = min(dirtyRect->rect.p2.x, posRect.p2.x);
//					commonRect.p2.y = min(dirtyRect->rect.p2.y, posRect.p2.y);
//
//					// czesc wspolna istnieje
//					if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
//					{
//						SpriteRender_t renderContext;
//						renderContext.commonRect = commonRect;
//						renderContext.baseRect = screenRect;
//						renderContext.baseToSpriteOffset.x = obj->mapPos.x - dirtyRect->rect.p1.x;
//						renderContext.baseToSpriteOffset.y = obj->mapPos.y - dirtyRect->rect.p1.y;
//						renderContext.LCDOffsetX = renderer->LCDOffsetX;
//
//						RE_FillSprite3(&obj->asset->baseAsset.sprite, renderContext);
//					}
//					break;
//				}
				default:
					break;
				}
			}
		}

		RE_SendRect(screenRect, renderer->LCDOffsetX);
	}

	return 0;
}

int RENDERER_RenderFloor(const BackgroundRepObject_t* floor)
{
	if (floor == NULL)	{ return -1; }

	const GameAsset_t* baseAsset = &floor->asset->baseAsset;
	SpriteRender_t renderContext;

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

			RE_RenderSprite(&baseAsset->sprite, renderContext, false);
		}
	}

	return 0;
}

int RENDERER_RenderBgdSprite(const Sprite_t* p, Point_t spritePos, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG)
{
	if (p == NULL)	{ return -1; }

	Rect_t posRect;
	posRect.p1.x = spritePos.x;
	posRect.p1.y = spritePos.y;
	posRect.p2.x = spritePos.x + p->size.x;
	posRect.p2.y = spritePos.y + p->size.y;

	Rect_t commonRect;
	commonRect.p1.x = max(mapRectToDraw.p1.x, posRect.p1.x);
	commonRect.p1.y = max(mapRectToDraw.p1.y, posRect.p1.y);
	commonRect.p2.x = min(mapRectToDraw.p2.x, posRect.p2.x);
	commonRect.p2.y = min(mapRectToDraw.p2.y, posRect.p2.y);

	// czesc wspolna istnieje
	if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
	{
		SpriteRender_t renderContext;
		renderContext.commonRect = commonRect;
		renderContext.baseRect = screenRect;
		renderContext.baseToSpriteOffset.x = posRect.p1.x - mapRectToDraw.p1.x;
		renderContext.baseToSpriteOffset.y = posRect.p1.y - mapRectToDraw.p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;

		if (render)
		{
			RE_RenderSprite(p, renderContext, fillBG);
		}
		else
		{
			RE_FillSprite3(p, renderContext);
		}
	}

	return 0;
}

