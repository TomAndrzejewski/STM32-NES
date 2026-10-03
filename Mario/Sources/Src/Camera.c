/*
 * Camera.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Camera.h"

#include <string.h>

#include "Game_Types.h"
#include "NES_Defs.h"
#include "Level.h"


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