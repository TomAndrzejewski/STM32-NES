/*
 * Camera.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Camera.h"

#include <stddef.h>

#include "Game_Types.h"
#include "LCDControl.h"
#include "BoardAssert.h"


static void CAMERA_CalcPos(CameraState_t* camera, const PlayerState_t* player)
{
	camera->prevPos = camera->currPos;

	if (player->currMapPos.x - camera->currPos.x > 80)
	{
		camera->currPos.x += player->currMapPos.x - camera->currPos.x - 80;
	}
}

static void CAMERA_CalcScreenRect(CameraState_t* camera)
{
	camera->screenRect.p1 = camera->currPos;
	camera->screenRect.p2.x = camera->currPos.x + LCD_WIDTH;
	camera->screenRect.p2.y = camera->currPos.y + LCD_HEIGHT;
}

void CAMERA_Update(CameraState_t* camera, const GameContext_t* ctx)
{
	BOARD_ASSERT(camera != NULL);
	BOARD_ASSERT(ctx != NULL);

	CAMERA_CalcPos(camera, &ctx->player);
	CAMERA_CalcScreenRect(camera);
}
