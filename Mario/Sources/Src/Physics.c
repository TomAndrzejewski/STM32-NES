/*
 * Physics.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

 #include "Physics.h"

#include <string.h>

#include "Game_Defs.h"
#include "NES_Functions.h"

#include "Game_Types.h"
#include "Level.h"

#include "NES_Defs.h"


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

	obj->prevMapPos = obj->currMapPos;

	//todotomka slabo ze to sie tutaj robi, trzeba jakos usystematyzowac flagi
	obj->currFlags.IsGrounded = false;

	PHYSICS_FGObject_Time_Movement(obj, ctx);

	PHYSICS_FGObject_Velocity_Movement(obj, ctx);

	PHYSICS_FGObject_Velocity_CalcMapPos(obj);

	PHYSICS_FGObject_SaveFlags(obj);

	return 0;
}

void PHYSICS_FGObject_SaveFlags(ForegroundObject_t* obj)
{
	obj->prevFlags = obj->currFlags;
}

void PHYSICS_FGObject_Time_Movement(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	if (obj->physics.type != PHYSICS_TIME) { // Time based physics switched off
		return;
	}

	TimeBasedMovement_t* mv = &obj->physics.engine.timeBased;
	if (mv->asset == NULL) {
		return;
	}

    // trigger physics if bumped
    if (!obj->prevFlags.playerBumpedFromBelow && obj->currFlags.playerBumpedFromBelow) {
        obj->currFlags.startPhysics = true;
    }

    if (obj->currFlags.startPhysics) { // start 
		obj->currFlags.startPhysics = false;
        obj->currFlags.physicsOngoing = true;
		mv->elapsedTimeUS = ctx->input.frameData.frameTimeUS;
	}

	if (obj->currFlags.physicsOngoing) 
	{ 
		if (mv->currentIndex >= mv->asset->framesCount) { // finish 
			obj->currFlags.physicsOngoing = false;
            mv->currentIndex = 0; // restart movement frames
			// Important call below!
			PHYSICS_FGObject_Time_Movement_Finish(obj); // custom behaviour on movement frames finish	
			return;
		}

		if (mv->asset->movementFramesY != NULL && obj->currFlags.physicsOngoing) {
			const MovementFrameY_t* mvFrame = &mv->asset->movementFramesY[mv->currentIndex];

			uint32_t tdiffUS = CalcTimeUS(mv->elapsedTimeUS);
			if (tdiffUS > mvFrame->durationUS) { // proceed with movement frame
				obj->currMapPos.y += mvFrame->dy; // movement
				mv->elapsedTimeUS = ctx->input.frameData.frameTimeUS; // get ready for next tdiff
				mv->repeatedCounter++;
				if (mv->repeatedCounter >= mvFrame->repeatCount) { 
					mv->currentIndex++; // next movement frame
					mv->repeatedCounter = 0;
				}
			} 
		}
	}
}

void PHYSICS_FGObject_Time_Movement_Finish(ForegroundObject_t* obj)
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
		// Switch to vecolity based physics
		memset(&obj->physics.engine, 0, sizeof(obj->physics.engine));
		obj->physics.type = PHYSICS_VELOCITY; 
		obj->physics.engine.body.vx = 0.3f; // Init vx 
		obj->currFlags.physicsOngoing = true; // Don't turn off the engine!
		break;
	}
    case FG_BLOCK_QMARK_OBJECT_ID:
    case FG_BRICKS_OBJECT_ID:
    {
        if (obj->assetFlags & BUMPABLE_MULTIPLE_TIMES) {
            obj->currFlags.playerBumpedFromBelow = false; // allow multiple bumps
        }
        break;
    }
	default:
		break;
	}
}

void PHYSICS_FGObject_Velocity_Movement(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	switch(obj->id)
	{
	case FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID:
	{
		if (obj->physics.type != PHYSICS_VELOCITY) {
			break;
		}

		Body_t* body = &obj->physics.engine.body;

		///////////////////
		// Y AXIS
		///////////////////
		if (obj->currFlags.IsGrounded) {
			body->vy = 0.0f;
		}
		else if (body->vy > -0.7f) {
			float multiplier = 1.6f;
			float dvy = ctx->input.frameData.frameTimeS * multiplier;
			body->vy -= dvy;
		}

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

void PHYSICS_FGObject_Velocity_CalcMapPos(ForegroundObject_t* obj)
{
	int pixelsToMove = 0;

	const Rect_t* levelBounds = NULL;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	if (ret < 0 || levelBounds == NULL) { return; }

	// New map position
	switch (obj->id)
	{
	case FG_REWARD_LEVEL_UP_MUSHROOM_OBJECT_ID:
	{
		if (obj->physics.type != PHYSICS_VELOCITY) {
			break;
		}

		Body_t* body = &obj->physics.engine.body;

		///////////////////
		// Y AXIS
		///////////////////
		pixelsToMove = (int)body->subpixelY / SUBPIXEL_RESOLUTION;
		if (pixelsToMove != 0) {
			body->subpixelY -= pixelsToMove * SUBPIXEL_RESOLUTION;

			int movedPosY = obj->currMapPos.y + pixelsToMove;
			if (movedPosY >= levelBounds->p1.y && movedPosY < levelBounds->p2.y) {
				obj->currMapPos.y += pixelsToMove;
			}
		}

		///////////////////
		// X AXIS
		///////////////////
		pixelsToMove = (int)body->subpixelX / SUBPIXEL_RESOLUTION;
		if (pixelsToMove != 0) {
			body->subpixelX -= pixelsToMove * SUBPIXEL_RESOLUTION;

			int movedPosX = obj->currMapPos.x + pixelsToMove;
			if (movedPosX >= levelBounds->p1.x && movedPosX < levelBounds->p2.x)
			{
				obj->currMapPos.x += pixelsToMove;
			}
		}

		break;
	}
	default:
		break;
	}
}