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

#include "Game_Defs.h"


// Systems:
// - Physics.c
// - Input.c
// - Renderer.c
// - Collision.c
// - Level.c
// - Assets.c
// - Settings.c


typedef enum
{
	ENEMY_GOOMBA_ID,

}EnemyID;

typedef enum
{
	BG_JEDYNKA_OBJECT_ID,
	BG_DWOJKA_OBJECT_ID,

}BackgroundObjectID;

typedef enum
{
	FG_RURA_OBJECT_ID,
	FG_CEGLY_OBJECT_ID,

}ForegroundObjectID;


typedef struct
{
	// Sprite
	// Collision BBox
	// ID
	Sprite_t	sprite;
	Rect_t		BBox; // p1 - offset from (0,0) in sprite, p2 - length



}GameAsset_t;

typedef struct
{
	Point_t		currMapPos;
	Point_t		prevMapPos;

	Point_t		vel;

}PlayerState_t;

typedef struct
{
	EnemyID		id;

	Point_t		currMapPos;
	Point_t		prevMapPos;

	Point_t		vel;

}EnemyState_t;



typedef struct
{
	BackgroundObjectID id;

	Point_t		mapPos;

}BackgroundObject_t;



typedef struct
{
	ForegroundObjectID id;

	Point_t		mapPos;

}ForegroundObject_t;

typedef struct
{
	Point_t cameraPos;

}MapState_t;

typedef struct
{
	// Player position
	// Player physics
	PlayerState_t player;

	// Enemies position
	// Enemies physics
	EnemyState_t enemies[ENEMIES_MAX_SIZE];

	// Current visible world
	// Visible background objects
	// Visible enemies
	// Visible collision objects
	BackgroundObject_t bgObjects[BACKGROUND_OBJECTS_MAX_SIZE];

	ForegroundObject_t fgObjects[FOREGROUND_OBJECTS_MAX_SIZE];

	MapState_t map;


	// Game machine state enum

	// Renderer data


}GameContext_t;

typedef struct
{
	// Physics settings
	// Controller settings
	// Player BBox settings

}GameSettings_t;



#endif /* SOURCES_INC_GAME_TYPES_H_ */
