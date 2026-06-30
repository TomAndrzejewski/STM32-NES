/*
 * Game_Types.h
 *
 *  Created on: 30 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GAME_TYPES_H_
#define SOURCES_INC_GAME_TYPES_H_

#include "NES_Defs.h"
#include "NES_Types.h"


// Systems:
// - Physics.c
// - Input.c
// - Renderer.c
// - Collision.c
// - Level.c
// - Assets.c
// - Settings.c


typedef struct
{
	// Mario position
	// Mario physics

	// Enemies position
	// Enemies physics

	// Current visible world
	// Visible background objects
	// Visible enemies
	// Visible collision objects

	// Game machine state enum

	// Renderer data


}GameContext_t;

typedef struct
{
	// Physics settings
	// Controller settings


}GameSettings_t;

typedef struct
{
	// Sprite
	// Collision BBox
	// ID

}GameAsset_t;



#endif /* SOURCES_INC_GAME_TYPES_H_ */
