/*
 * ObjectsManager.h
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_OBJECTSMANAGER_H_
#define SOURCES_INC_OBJECTSMANAGER_H_

#include "Game_Types.h"

int     OBJECTS_MANAGER_Update(GameContext_t* ctx);
void    OBJECTS_MANAGER_CalcActiveRegion(ObjectsManager_t* mgr, const GameContext_t* ctx);
int     OBJECTS_MANAGER_LoadObjects(GameContext_t* ctx);
int     OBJECTS_MANAGER_SpawnObject(GameContext_t* ctx, const ObjectLevelInstance_t* objectDef);
int     OBJECTS_MANAGER_DeleteObjects(GameContext_t* ctx);
int     OBJECTS_MANAGER_Enemy_Load(EnemyState_t* enemy, const ObjectLevelInstance_t* objectDef);
int     OBJECTS_MANAGER_FGObject_Load(ForegroundObject_t* obj, const ObjectLevelInstance_t* objectDef);
int     OBJECTS_MANAGER_BGObject_Load(BackgroundObject_t* obj, const ObjectLevelInstance_t* objectDef);
int     OBJECTS_MANAGER_OrderSpawn(ObjectsManager_t* mgr, const ObjectLevelInstance_t* objectToSpawn);
bool    OBJECTS_MANAGER_IsThisEnemyID(const GameObjectID id);
bool    OBJECTS_MANAGER_IsThisFGID(const GameObjectID id);
bool    OBJECTS_MANAGER_IsThisBGID(const GameObjectID id);

#endif // SOURCES_INC_OBJECTSMANAGER_H_