/*
 * Level.h
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_LEVEL_H_
#define SOURCES_INC_LEVEL_H_

#include "Game_Types.h"

//todotomka flagi gdzies indziej trzeba dac
#define COLL_TOP_ENABLED					(0x00000001)
#define COLL_DOWN_ENABLED					(0x00000002)
#define COLL_LEFT_ENABLED					(0x00000004)
#define COLL_RIGHT_ENABLED					(0x00000008)
#define COLL_ANY_ENABLED					( COLL_TOP_ENABLED \
											| COLL_DOWN_ENABLED \
											| COLL_LEFT_ENABLED \
											| COLL_RIGHT_ENABLED)
#define FG_SCROLL_RENDER					(0x00000010)



typedef struct
{
	GameObjectID id;
	int		x;
	int		y;
	uint32_t flags;

}ObjectLevelInstance_t;

typedef struct
{
	GameObjectID id;
	int		x;
	int		y;
	int		mulX;
	int		mulY;

}RepObjectLevelPos_t;


int LEVEL_GetObjectsLocations(const ObjectLevelInstance_t** posTable, int* numOfObjects);

//int LEVEL_GetBGObjectsLocations(const ObjectLevelInstance_t** posTable, int* numOfObjects);

int LEVEL_GetEnemiesLocations(const ObjectLevelInstance_t** posTable, int* numOfObjects);

int LEVEL_GetBGRepObjectsLocations(const RepObjectLevelPos_t** posTable, int* numOfObjects);

int LEVEL_GetLevelBoundaries(const Rect_t** levelBoundaries);

int LEVEL_GetObjRenderPriorities(const int** prioTable, int* numOfObjects);


#endif /* SOURCES_INC_LEVEL_H_ */
