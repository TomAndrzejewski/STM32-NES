/*
 * NESEngine.h
 *
 *  Created on: 4 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NESENGINE_H_
#define SOURCES_INC_NESENGINE_H_

#include "Mario.h"
#include "Background.h"


#ifdef DEFINE_NES_ENGINE

#define GLOBAL_NES_ENGINE
#define INIT_NES_ENGINE(x) = x

#else

#define GLOBAL_NES_ENGINE extern
#define INIT_NES_ENGINE(x)

#endif


GLOBAL_NES_ENGINE Mario_t 	gMario;
GLOBAL_NES_ENGINE Mario_t*	pMario INIT_NES_ENGINE(&gMario);

GLOBAL_NES_ENGINE Background_t 	gBG;
GLOBAL_NES_ENGINE Background_t*	pBG INIT_NES_ENGINE(&gBG);


#endif /* SOURCES_INC_NESENGINE_H_ */
