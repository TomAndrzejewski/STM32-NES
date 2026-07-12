/*
 * NESEngine.h
 *
 *  Created on: 4 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NESENGINE_H_
#define SOURCES_INC_NESENGINE_H_

#include "Mario.h"
#include "Map.h"
#include "Enemies.h"
#include "Game_Types.h"


#ifdef DEFINE_NES_ENGINE

#define GLOBAL_NES_ENGINE
#define INIT_NES_ENGINE(x) = x

#else

#define GLOBAL_NES_ENGINE extern
#define INIT_NES_ENGINE(x)

#endif

GLOBAL_NES_ENGINE GameContext_t 	gGameCtx INIT_NES_ENGINE({0});
GLOBAL_NES_ENGINE GameContext_t*	pGameCtx INIT_NES_ENGINE(&gGameCtx);

GLOBAL_NES_ENGINE Mario_t 	gMario INIT_NES_ENGINE({0});
GLOBAL_NES_ENGINE Mario_t*	pMario INIT_NES_ENGINE(&gMario);

GLOBAL_NES_ENGINE Enemies2_t 	gEnemies INIT_NES_ENGINE({0});
GLOBAL_NES_ENGINE Enemies2_t*	pEnemies INIT_NES_ENGINE(&gEnemies);

GLOBAL_NES_ENGINE Map_t 	gMap INIT_NES_ENGINE({0});
GLOBAL_NES_ENGINE Map_t*	pMap INIT_NES_ENGINE(&gMap);


#endif /* SOURCES_INC_NESENGINE_H_ */
