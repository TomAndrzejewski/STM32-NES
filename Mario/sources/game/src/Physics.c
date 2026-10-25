/*
 * Physics.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Physics.h"

#include <string.h>

#include "Game_Defs.h"
#include "Game_Types.h"
#include "Input.h"
#include "Level.h"
#include "BoardAssert.h"


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void PHYSICS_Enemy_Update(EnemyState_t* enemy, const GameContext_t* ctx);
static void PHYSICS_Enemy_Time_Movement(EnemyState_t* enemy, const GameContext_t* ctx);
static void PHYSICS_Enemy_Time_Movement_Finish(EnemyState_t* enemy);
static void PHYSICS_Enemy_Velocity_Movement(EnemyState_t* enemy, const GameContext_t* ctx);
static void PHYSICS_Enemy_Velocity_CalcMapPos(EnemyState_t* enemy);
static void PHYSICS_Enemy_SaveFlags(EnemyState_t* enemy);

static void PHYSICS_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx);
static void PHYSICS_FGObject_Time_Movement(ForegroundObject_t* obj, const GameContext_t* ctx);
static void PHYSICS_FGObject_Time_Movement_Finish(ForegroundObject_t* obj);
static void PHYSICS_FGObject_Velocity_Movement(ForegroundObject_t* obj, const GameContext_t* ctx);
static void PHYSICS_FGObject_Velocity_CalcMapPos(ForegroundObject_t* obj);
static void PHYSICS_FGObject_SaveFlags(ForegroundObject_t* obj);

static void PHYSICS_Player_RestartFlags(PlayerState_t* player);
static void PHYSICS_Player_Movement(PlayerState_t* player, const GameContext_t* ctx);
static void PHYSICS_Player_CalcMapPos(PlayerState_t* player, const GameContext_t* ctx);
static void PHYSICS_Player_CalcMovementDirection(PlayerState_t* player);


//----------------
// PUBLIC FUNCTIONS
//----------------
void PHYSICS_Update(GameContext_t* ctx)
{
	BOARD_ASSERT(ctx != NULL);

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

		PHYSICS_FGObject_Update(obj, ctx);
	}

	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		int indexLUT = ctx->enemies.enemiesLUT[i];
		if (!ctx->enemies.IsEnemyActive[indexLUT]) continue;

		EnemyState_t* enemy = &ctx->enemies.pool[indexLUT];
		if (!enemy->currFlags.IsAlive) continue;

		PHYSICS_Enemy_Update(enemy, ctx);
	}
}

void PHYSICS_Player_Update(PlayerState_t* player, const GameContext_t* ctx)
{
	BOARD_ASSERT(player != NULL);
	BOARD_ASSERT(ctx != NULL);

	PHYSICS_Player_RestartFlags(player);

	PHYSICS_Player_Movement(player, ctx);

	PHYSICS_Player_CalcMapPos(player, ctx);

	PHYSICS_Player_CalcMovementDirection(player);
}

void PHYSICS_Player_DeathStart(PlayerState_t* player)
{
	BOARD_ASSERT(player != NULL);

	// Stop the horizontal movement and throw the player up
	player->IsGrounded = false;
	player->body.vx = 0.0f;
	player->body.vy = 1.0f;
	player->body.subpixelX = 0.0f;
	player->body.subpixelY = 0.0f;
}

void PHYSICS_Player_DeathUpdate(PlayerState_t* player, const GameContext_t* ctx)
{
	BOARD_ASSERT(player != NULL);
	BOARD_ASSERT(ctx != NULL);

	// Y axis: fall with gravity up to the terminal velocity, ignoring the input and the ground
	if (player->body.vy > -1.0f) {
		float multiplier = 3.0f;
		float dvy = ctx->input.frameData.frameTimeS * multiplier;
		player->body.vy -= dvy;
	}

	// Accumulate subpixel movement from the vertical velocity
	player->body.subpixelY += (player->body.vy * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;

	PHYSICS_Player_CalcMapPos(player, ctx);
}


//----------------
// PRIVATE FUNCTIONS
//----------------
static void PHYSICS_Enemy_Update(EnemyState_t* enemy, const GameContext_t* ctx)
{
	enemy->prevMapPos = enemy->currMapPos;

	//todotomka slabo ze to sie tutaj robi, trzeba jakos usystematyzowac flagi
	enemy->currFlags.IsGrounded = false;

	PHYSICS_Enemy_Time_Movement(enemy, ctx);

	PHYSICS_Enemy_Velocity_Movement(enemy, ctx);

	PHYSICS_Enemy_Velocity_CalcMapPos(enemy);

	PHYSICS_Enemy_SaveFlags(enemy);
}

static void PHYSICS_Enemy_Time_Movement(EnemyState_t* enemy, const GameContext_t* ctx)
{
	// Time based physics switched off
	if (enemy->physics.type != PHYSICS_TIME) {
		return;
	}

	TimeBasedMovement_t* mv = &enemy->physics.engine.timeBased;
	if (mv->asset == NULL) {
		return;
	}

	// Trigger physics if bumped
	if (!enemy->prevFlags.playerBumpedFromBelow && enemy->currFlags.playerBumpedFromBelow) {
		enemy->currFlags.startTimeBasedPhysics = true;
	}

	// Start the movement
	if (enemy->currFlags.startTimeBasedPhysics) {
		enemy->currFlags.startTimeBasedPhysics = false;
		enemy->currFlags.physicsOngoing = true;
		mv->elapsedTimeUS = ctx->input.frameData.simulationTimeUS;
	}

	if (enemy->currFlags.physicsOngoing)
	{
		// Finish the movement and rewind its frames
		if (mv->currentIndex >= mv->asset->framesCount) {
			enemy->currFlags.physicsOngoing = false;
			mv->currentIndex = 0;
			// Object-specific follow-up: decides what the object becomes once its movement has ended
			PHYSICS_Enemy_Time_Movement_Finish(enemy);
			return;
		}

		if (mv->asset->movementFramesY != NULL && enemy->currFlags.physicsOngoing) {
			const MovementFrameY_t* mvFrame = &mv->asset->movementFramesY[mv->currentIndex];

			// Move by one step once the frame duration has passed, then advance to the next frame after all its repeats
			uint32_t tdiffUS = ctx->input.frameData.simulationTimeUS - mv->elapsedTimeUS;
			if (tdiffUS > mvFrame->durationUS) {
				enemy->currMapPos.y += mvFrame->dy;
				mv->elapsedTimeUS = ctx->input.frameData.simulationTimeUS;
				mv->repeatedCounter++;
				if (mv->repeatedCounter >= mvFrame->repeatCount) {
					mv->currentIndex++;
					mv->repeatedCounter = 0;
				}
			}
		}
	}
}

static void PHYSICS_Enemy_Time_Movement_Finish(EnemyState_t* enemy)
{
	switch(enemy->id)
	{
	default:
		break;
	}
}

static void PHYSICS_Enemy_Velocity_Movement(EnemyState_t* enemy, const GameContext_t* ctx)
{
	switch(enemy->id)
	{
	case ENEMY_GOOMBA_ID:
	{
		if (enemy->physics.type != PHYSICS_VELOCITY) {
			break;
		}

		Body_t* body = &enemy->physics.engine.body;

		// Y axis: stand on the ground or fall with gravity up to the terminal velocity
		if (enemy->currFlags.IsGrounded) {
			body->vy = 0.0f;
		}
		else if (body->vy > -0.7f) {
			float multiplier = 1.6f;
			float dvy = ctx->input.frameData.frameTimeS * multiplier;
			body->vy -= dvy;
		}

		// Accumulate subpixel movement from the velocities
		if (enemy->currFlags.physicsOngoing) {
			body->subpixelX += (body->vx * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;
			body->subpixelY += (body->vy * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;
		}

		break;
	}
	default:
		break;
	}
}

static void PHYSICS_Enemy_Velocity_CalcMapPos(EnemyState_t* enemy)
{
	int pixelsToMove = 0;

	const Rect_t* levelBounds = NULL;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	BOARD_ASSERT(ret == 0 && levelBounds != NULL);

	switch (enemy->id)
	{
	case ENEMY_GOOMBA_ID:
	{
		if (enemy->physics.type != PHYSICS_VELOCITY) break;

		Body_t* body = &enemy->physics.engine.body;

		// Y axis: move by the whole pixels collected in subpixels, only within the level
		pixelsToMove = (int)body->subpixelY / SUBPIXEL_RESOLUTION;
		if (pixelsToMove != 0) {
			body->subpixelY -= pixelsToMove * SUBPIXEL_RESOLUTION;

			int movedPosY = enemy->currMapPos.y + pixelsToMove;
			if (movedPosY >= levelBounds->p1.y && movedPosY < levelBounds->p2.y) {
				enemy->currMapPos.y += pixelsToMove;
			}
		}

		// X axis: move by the whole pixels collected in subpixels, only within the level
		pixelsToMove = (int)body->subpixelX / SUBPIXEL_RESOLUTION;
		if (pixelsToMove != 0) {
			body->subpixelX -= pixelsToMove * SUBPIXEL_RESOLUTION;

			int movedPosX = enemy->currMapPos.x + pixelsToMove;
			if (movedPosX >= levelBounds->p1.x && movedPosX < levelBounds->p2.x) {
				enemy->currMapPos.x += pixelsToMove;
			}
		}

		break;
	}
	default:
		break;
	}
}

static void PHYSICS_Enemy_SaveFlags(EnemyState_t* enemy)
{
	enemy->prevFlags = enemy->currFlags;
}



static void PHYSICS_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	obj->prevMapPos = obj->currMapPos;

	//todotomka slabo ze to sie tutaj robi, trzeba jakos usystematyzowac flagi
	obj->currFlags.IsGrounded = false;

	PHYSICS_FGObject_Time_Movement(obj, ctx);

	PHYSICS_FGObject_Velocity_Movement(obj, ctx);

	PHYSICS_FGObject_Velocity_CalcMapPos(obj);

	PHYSICS_FGObject_SaveFlags(obj);
}

static void PHYSICS_FGObject_Time_Movement(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	// Time based physics switched off
	if (obj->physics.type != PHYSICS_TIME) {
		return;
	}

	TimeBasedMovement_t* mv = &obj->physics.engine.timeBased;
	if (mv->asset == NULL) {
		return;
	}

	// Trigger physics if bumped
	if (!obj->prevFlags.playerBumpedFromBelow && obj->currFlags.playerBumpedFromBelow) {
		obj->currFlags.startTimeBasedPhysics = true;
	}

	// Start the movement
	if (obj->currFlags.startTimeBasedPhysics) {
		obj->currFlags.startTimeBasedPhysics = false;
		obj->currFlags.physicsOngoing = true;
		mv->elapsedTimeUS = ctx->input.frameData.simulationTimeUS;
	}

	if (obj->currFlags.physicsOngoing)
	{
		// Finish the movement and rewind its frames
		if (mv->currentIndex >= mv->asset->framesCount) {
			obj->currFlags.physicsOngoing = false;
			mv->currentIndex = 0;
			// Object-specific follow-up: decides what the object becomes once its movement has ended
			PHYSICS_FGObject_Time_Movement_Finish(obj);
			return;
		}

		if (mv->asset->movementFramesY != NULL && obj->currFlags.physicsOngoing) {
			const MovementFrameY_t* mvFrame = &mv->asset->movementFramesY[mv->currentIndex];

			// Move by one step once the frame duration has passed, then advance to the next frame after all its repeats
			uint32_t tdiffUS = ctx->input.frameData.simulationTimeUS - mv->elapsedTimeUS;
			if (tdiffUS > mvFrame->durationUS) {
				obj->currMapPos.y += mvFrame->dy;
				mv->elapsedTimeUS = ctx->input.frameData.simulationTimeUS;
				mv->repeatedCounter++;
				if (mv->repeatedCounter >= mvFrame->repeatCount) {
					mv->currentIndex++;
					mv->repeatedCounter = 0;
				}
			}
		}
	}
}

static void PHYSICS_FGObject_Time_Movement_Finish(ForegroundObject_t* obj)
{
	switch(obj->id)
	{
	case FG_REWARD_COIN_OBJECT_ID:
	{
		obj->currFlags.IsAlive = false;
		break;
	}
	case FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID:
	{
		// Switch to velocity based physics with an initial horizontal speed and keep the physics running
		memset(&obj->physics.engine, 0, sizeof(obj->physics.engine));
		obj->physics.type = PHYSICS_VELOCITY;
		obj->physics.engine.body.vx = 0.3f;
		obj->currFlags.physicsOngoing = true;
		break;
	}
	case FG_BLOCK_QMARK_OBJECT_ID:
	case FG_BRICKS_OBJECT_ID:
	{
		// Allow multiple bumps
		if (obj->assetFlags & BUMPABLE_MULTIPLE_TIMES) {
			obj->currFlags.playerBumpedFromBelow = false;
		}
		break;
	}
	default:
		break;
	}
}

static void PHYSICS_FGObject_Velocity_Movement(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	switch(obj->id)
	{
	case FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID:
	{
		if (obj->physics.type != PHYSICS_VELOCITY) {
			break;
		}

		Body_t* body = &obj->physics.engine.body;

		// Y axis: stand on the ground or fall with gravity up to the terminal velocity
		if (obj->currFlags.IsGrounded) {
			body->vy = 0.0f;
		}
		else if (body->vy > -0.7f) {
			float multiplier = 1.6f;
			float dvy = ctx->input.frameData.frameTimeS * multiplier;
			body->vy -= dvy;
		}

		// Accumulate subpixel movement from the velocities
		if (obj->currFlags.physicsOngoing) {
			body->subpixelX += (body->vx * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;
			body->subpixelY += (body->vy * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;
		}

		break;
	}
	default:
		break;
	}
}

static void PHYSICS_FGObject_Velocity_CalcMapPos(ForegroundObject_t* obj)
{
	int pixelsToMove = 0;

	const Rect_t* levelBounds = NULL;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	BOARD_ASSERT(ret == 0 && levelBounds != NULL);

	switch (obj->id)
	{
	case FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID:
	{
		if (obj->physics.type != PHYSICS_VELOCITY) {
			break;
		}

		Body_t* body = &obj->physics.engine.body;

		// Y axis: move by the whole pixels collected in subpixels, only within the level
		pixelsToMove = (int)body->subpixelY / SUBPIXEL_RESOLUTION;
		if (pixelsToMove != 0) {
			body->subpixelY -= pixelsToMove * SUBPIXEL_RESOLUTION;

			int movedPosY = obj->currMapPos.y + pixelsToMove;
			if (movedPosY >= levelBounds->p1.y && movedPosY < levelBounds->p2.y) {
				obj->currMapPos.y += pixelsToMove;
			}
		}

		// X axis: move by the whole pixels collected in subpixels, only within the level
		pixelsToMove = (int)body->subpixelX / SUBPIXEL_RESOLUTION;
		if (pixelsToMove != 0) {
			body->subpixelX -= pixelsToMove * SUBPIXEL_RESOLUTION;

			int movedPosX = obj->currMapPos.x + pixelsToMove;
			if (movedPosX >= levelBounds->p1.x && movedPosX < levelBounds->p2.x) {
				obj->currMapPos.x += pixelsToMove;
			}
		}

		break;
	}
	default:
		break;
	}
}

static void PHYSICS_FGObject_SaveFlags(ForegroundObject_t* obj)
{
	obj->prevFlags = obj->currFlags;
}

static void PHYSICS_Player_RestartFlags(PlayerState_t* player)
{
	player->prevPhysicsFlags = player->currPhysicsFlags;

	player->currPhysicsFlags.IsDecelerating = false;
}

static void PHYSICS_Player_Movement(PlayerState_t* player, const GameContext_t* ctx)
{
	// Y axis: start a jump on a fresh button press, otherwise fall with gravity (weaker while the button is held on the way up)
	if (player->IsGrounded) {
		if (INPUT_IsPressed(INPUT_ACTION_JUMP)) {
			player->IsGrounded = false;
			player->body.vy = 1.0f;
		}
	} else {
		if (player->body.vy > -1.0f) {
			float multiplier = 6.0f;
			if (player->body.vy > 0.0f && INPUT_IsDown(INPUT_ACTION_JUMP)) {
				multiplier = 2.0f;
			}
			float dvy = ctx->input.frameData.frameTimeS * multiplier;
			player->body.vy -= dvy;
		}
	}

	// X axis: accelerate towards the pressed direction up to the maximum speed, or slow down to a stop when no direction is pressed
	if (INPUT_IsDown(INPUT_ACTION_RUN_RIGHT)) {
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
	} else if (INPUT_IsDown(INPUT_ACTION_RUN_LEFT)) {
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
			// Calculate new velocity
			float dvx = ctx->input.frameData.frameTimeS * 2;
			if (player->body.vx - dvx > 0.0f) {
				player->body.vx -= dvx;
			} else {
				player->body.vx = 0.0f;
			}
		} else {
			// Calculate new velocity
			float dvx = ctx->input.frameData.frameTimeS * 2;
			if (player->body.vx + dvx < 0.0f) {
				player->body.vx += dvx;
			} else {
				player->body.vx = 0.0f;
			}
		}
	}

	// Accumulate subpixel movement from the velocities
	//todotomka 128 jako ustawienie (settings)
	player->body.subpixelX += (player->body.vx * SUBPIXEL_RESOLUTION * 128) / TARGET_FRAMERATE_HZ;
	player->body.subpixelY += (player->body.vy * SUBPIXEL_RESOLUTION * 256) / TARGET_FRAMERATE_HZ;
}

static void PHYSICS_Player_CalcMapPos(PlayerState_t* player, const GameContext_t* ctx)
{
	int pixelsToMove = 0;

	const Rect_t* levelBounds = NULL;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	BOARD_ASSERT(ret == 0 && levelBounds != NULL);

	player->prevMapPos = player->currMapPos;

	// Y axis: move by the whole pixels collected in subpixels, only within the level
	pixelsToMove = (int)player->body.subpixelY / SUBPIXEL_RESOLUTION;
	if (pixelsToMove != 0) {
		player->body.subpixelY -= pixelsToMove * SUBPIXEL_RESOLUTION;

		int movedPosY = player->currMapPos.y + pixelsToMove;
		if (movedPosY >= levelBounds->p1.y && movedPosY < levelBounds->p2.y) {
			player->currMapPos.y += pixelsToMove;
		}
	}

	// X axis: move by the whole pixels collected in subpixels, only within the level and the screen
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
}

static void PHYSICS_Player_CalcMovementDirection(PlayerState_t* player)
{
	if (player->currMapPos.x > player->prevMapPos.x) {
		player->currPhysicsFlags.lastMovementDirectionRight = true;
	} else if (player->currMapPos.x < player->prevMapPos.x) {
		player->currPhysicsFlags.lastMovementDirectionRight = false;
	}
}
