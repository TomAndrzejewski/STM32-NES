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

#define ENEMY_ID_START				(0x00000001)
#define ENEMY_ID_END				(0x00000FFF)
#define BACKGROUND_OBJECT_ID_START	(0x00001000)
#define BACKGROUND_OBJECT_ID_END	(0x00001FFF)
#define FOREGROUND_OBJECT_ID_START	(0x00002000)
#define FOREGROUND_OBJECT_ID_END	(0x00002FFF)

typedef enum
{
	OBJECT_NOT_USED = -1,

	ENEMY_GOOMBA_ID = ENEMY_ID_START,

	BG_JEDYNKA_OBJECT_ID = BACKGROUND_OBJECT_ID_START,
	BG_DWOJKA_OBJECT_ID,
	BG_CHMURKA_OBJECT_ID,

	FG_RURA_OBJECT_ID = FOREGROUND_OBJECT_ID_START,
	FG_CEGLY_OBJECT_ID,

}GameObjectID;


typedef struct
{
	Sprite_t sprite;
	GameObjectID id;

}GameAsset_t;

typedef struct
{
	const GameAsset_t* baseAsset;

}BackgroundAsset_t;

typedef struct
{
	const GameAsset_t* baseAsset;

	Rect_t BBox; // p1 - offset from (0,0) in sprite, p2 - length

}ForegroundAsset_t;

typedef struct
{
	const GameAsset_t* baseAsset;

	Rect_t BBox; // p1 - offset from (0,0) in sprite, p2 - length

}EnemyAsset_t;

typedef struct
{
	Point_t		currMapPos;
	Point_t		prevMapPos;

	float		vx;
	float		vy;

}PlayerState_t;

typedef struct
{
	GameObjectID id;

	Point_t		currMapPos;
	Point_t		prevMapPos;

	const EnemyAsset_t* asset;

}EnemyState_t;

typedef struct
{
	GameObjectID id;

	Point_t mapPos;

	const BackgroundAsset_t* asset;

}BackgroundObject_t;

typedef struct
{
	GameObjectID id;

	Point_t mapPos;

	const ForegroundAsset_t* asset;

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
