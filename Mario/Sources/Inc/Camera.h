/*
 * Camera.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_CAMERA_H_
#define SOURCES_INC_CAMERA_H_

#include "Game_Types.h"

int 	CAMERA_Update(CameraState_t* camera, const GameContext_t* ctx);
int 	CAMERA_CalcPos(CameraState_t* camera, const PlayerState_t* player);
int 	CAMERA_CalcScreenRect(CameraState_t* camera);

#endif // SOURCES_INC_CAMERA_H_