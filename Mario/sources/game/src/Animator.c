/*
 * Animator.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Animator.h"

#include <math.h>

#include "Game_Types.h"
#include "NES_Assert.h"


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void ANIMATOR_Player_Update(PlayerState_t* player, const GameContext_t* ctx);
static void ANIMATOR_Player_Decide(PlayerState_t* player, const GameContext_t* ctx);
// static void ANIMATOR_Player_Decide_LevelUp(PlayerState_t* player, const GameContext_t* ctx);
static void ANIMATOR_Player_Decide_Normal(PlayerState_t* player, const GameContext_t* ctx);
static void ANIMATOR_Player_SetAsset(PlayerState_t* player);
static void ANIMATOR_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx);
static void ANIMATOR_FGObject_Decide(ForegroundObject_t* obj, const GameContext_t* ctx);
static void ANIMATOR_FGObject_SetAsset(ForegroundObject_t* obj);
static void ANIMATOR_Enemy_Update(EnemyState_t* enemy, const GameContext_t* ctx);
static void ANIMATOR_Enemy_Decide(EnemyState_t* enemy, const GameContext_t* ctx);
static void ANIMATOR_Enemy_SetAsset(EnemyState_t* enemy);


//----------------
// PUBLIC FUNCTIONS
//----------------
void ANIMATOR_Update(GameContext_t* ctx)
{
	NES_ASSERT(ctx != NULL);

	ANIMATOR_Player_Update(&ctx->player, ctx);

	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];
		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}

		if (ctx->fgObjects[indexLUT].animableAsset == NULL) {
			continue;
		}

		ANIMATOR_FGObject_Update(&ctx->fgObjects[indexLUT], ctx);
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

		ANIMATOR_Enemy_Update(enemy, ctx);
	}
}


//----------------
// PRIVATE FUNCTIONS
//----------------
static void ANIMATOR_Player_Update(PlayerState_t* player, const GameContext_t* ctx)
{
	ANIMATOR_Player_Decide(player, ctx);

	ANIMATOR_Player_SetAsset(player);
}

static void ANIMATOR_Player_Decide(PlayerState_t* player, const GameContext_t* ctx)
{
	// if (player->triggerLevelUp && !player->levelUpOngoing) {
	// 	player->triggerLevelUp = false;
	// 	player->levelUpOngoing = true;
	// }

	// if (player->levelUpOngoing) {
	// 	ANIMATOR_Player_Decide_LevelUp(player, ctx);
	// }
	// else {
		ANIMATOR_Player_Decide_Normal(player, ctx);	
	// }
}

// static void ANIMATOR_Player_Decide_LevelUp(PlayerState_t* player, const GameContext_t* ctx)
// {

// }

static void ANIMATOR_Player_Decide_Normal(PlayerState_t* player, const GameContext_t* ctx)
{
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

			uint32_t runAnimationVelTimeMultiplier = player->animator.runAnimationFrameTimeUS * (uint32_t)fabsf(player->body.vx);
			
			if (player->animator.runAnimationFrameTimeUS < 75000) {
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
}

static void ANIMATOR_Player_SetAsset(PlayerState_t* player)
{
	NES_ASSERT(player->animableAsset != NULL);

	// save sprite size for dirty rects
	player->prevSpriteSize = player->asset.baseAsset.sprite.size;

	int assetIndex = -1;
	for (int i = 0; i < player->animableAsset->baseAssetsCount; i++)
	{
		if (player->animableAsset->baseAssets[i].animationID == player->animator.currAnimation) {
			assetIndex = i;
			break;
		}
	}

	// The animable asset has no usable entry for the current animation: check its base assets table.
	NES_ASSERT(assetIndex >= 0);
	NES_ASSERT(player->animableAsset->baseAssets[assetIndex].baseAsset != NULL);

	player->asset.baseAsset = *player->animableAsset->baseAssets[assetIndex].baseAsset;
}

static void ANIMATOR_FGObject_Update(ForegroundObject_t* obj, const GameContext_t* ctx)
{
	ANIMATOR_FGObject_Decide(obj, ctx);

	ANIMATOR_FGObject_SetAsset(obj);

	// ANIMATOR_FGObject_Movement(obj);
}

static void ANIMATOR_FGObject_Decide(ForegroundObject_t* obj, const GameContext_t* ctx)
{
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
	case FG_REWARD_COIN_OBJECT_ID:
	{
		if (obj->animationFrameTimeUS == 0) {
			obj->animationFrameTimeUS = ctx->input.frameData.simulationTimeUS;
		}

		uint32_t tdiff = ctx->input.frameData.simulationTimeUS - obj->animationFrameTimeUS;
		if (tdiff > 60000) {
			obj->animationFrameTimeUS = ctx->input.frameData.simulationTimeUS;
			switch (obj->currAnimation)
			{
			case FG_REWARD_COIN_1_ANIMATION_ID: 
				obj->currAnimation = FG_REWARD_COIN_2_ANIMATION_ID;
				break;
			case FG_REWARD_COIN_2_ANIMATION_ID: 
				obj->currAnimation = FG_REWARD_COIN_3_ANIMATION_ID;
				break;
			case FG_REWARD_COIN_3_ANIMATION_ID: 
				obj->currAnimation = FG_REWARD_COIN_4_ANIMATION_ID;
				break;
			case FG_REWARD_COIN_4_ANIMATION_ID: 
				obj->currAnimation = FG_REWARD_COIN_1_ANIMATION_ID;
				break;
			default:
				break;
			}
		}

		break;
	}
	default:
		break;
	}
}

static void ANIMATOR_FGObject_SetAsset(ForegroundObject_t* obj)
{
	int assetIndex = -1;
	for (int i = 0; i < obj->animableAsset->baseAssetsCount; i++)
	{
		if (obj->animableAsset->baseAssets[i].animationID == obj->currAnimation) {
			assetIndex = i;
			break;
		}
	}

	// The animable asset has no usable entry for the current animation: check its base assets table.
	NES_ASSERT(assetIndex >= 0);
	NES_ASSERT(obj->animableAsset->baseAssets[assetIndex].baseAsset != NULL);

	obj->asset.baseAsset = *obj->animableAsset->baseAssets[assetIndex].baseAsset;
}

static void ANIMATOR_Enemy_Update(EnemyState_t* enemy, const GameContext_t* ctx)
{
	ANIMATOR_Enemy_Decide(enemy, ctx);

	ANIMATOR_Enemy_SetAsset(enemy);
}

static void ANIMATOR_Enemy_Decide(EnemyState_t* enemy, const GameContext_t* ctx)
{
	(void)ctx;

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
}

static void ANIMATOR_Enemy_SetAsset(EnemyState_t* enemy)
{
	int assetIndex = -1;
	for (int i = 0; i < enemy->animableAsset->baseAssetsCount; i++)
	{
		if (enemy->animableAsset->baseAssets[i].animationID == enemy->currAnimation) {
			assetIndex = i;
			break;
		}
	}

	// The animable asset has no usable entry for the current animation: check its base assets table.
	NES_ASSERT(assetIndex >= 0);
	NES_ASSERT(enemy->animableAsset->baseAssets[assetIndex].baseAsset != NULL);

	enemy->asset.baseAsset = *enemy->animableAsset->baseAssets[assetIndex].baseAsset;
}