/*
 * NESEngine.h
 *
 *  Created on: 4 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NESENGINE_H_
#define SOURCES_INC_NESENGINE_H_

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


#endif /* SOURCES_INC_NESENGINE_H_ */
