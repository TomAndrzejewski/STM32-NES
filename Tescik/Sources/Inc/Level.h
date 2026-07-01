/*
 * Level.h
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_LEVEL_H_
#define SOURCES_INC_LEVEL_H_

#include "Game_Types.h"


typedef struct
{
	GameObjectID id;
	int		x;
	int		y;

}ObjectLevelPos_t;


int LEVEL_GetBGObjectsLocations(const ObjectLevelPos_t** posTable, int* numOfObjects);

int LEVEL_GetEnemiesLocations(const ObjectLevelPos_t** posTable, int* numOfObjects);


#endif /* SOURCES_INC_LEVEL_H_ */
