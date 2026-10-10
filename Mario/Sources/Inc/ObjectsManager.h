/*
 * ObjectsManager.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_OBJECTSMANAGER_H_
#define SOURCES_INC_OBJECTSMANAGER_H_

#include "Game_Types.h"

void    OBJECTS_MANAGER_Update(GameContext_t* ctx);
int     OBJECTS_MANAGER_OrderSpawn(ObjectsManager_t* mgr, const ObjectLevelInstance_t* objectToSpawn);

#endif // SOURCES_INC_OBJECTSMANAGER_H_