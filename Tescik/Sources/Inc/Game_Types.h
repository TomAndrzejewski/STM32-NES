/*
 * Game_Types.h
 *
 *  Created on: 30 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GAME_TYPES_H_
#define SOURCES_INC_GAME_TYPES_H_

#include <stdbool.h>

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

// Systems V2:
//├── INPUT
//├── PLAYER
//├── ENEMIES
//├── PHYSICS
//├── COLLISION
//├── WORLD
//├── CAMERA
//└── RENDER

#define ENEMY_ID_START					(0x00000001)
#define ENEMY_ID_END					(0x00000FFF)

#define BACKGROUND_OBJECT_ID_START		(0x00001000)
#define BACKGROUND_OBJECT_ID_END		(0x00001FFF)

#define FOREGROUND_OBJECT_ID_START		(0x00002000)
#define FOREGROUND_OBJECT_ID_END		(0x00002FFF)

#define BACKGROUND_REP_OBJECT_ID_START	(0x00003000)
#define FOREGROUND_REP_OBJECT_ID_END	(0x00003FFF)

#define PLAYER_ID_START					(0x00004000)
#define PLAYER_ID_END					(0x00004FFF)

typedef enum
{
	OBJECT_NOT_USED = -1,

	ENEMY_GOOMBA_ID = ENEMY_ID_START,

	BG_JEDYNKA_OBJECT_ID = BACKGROUND_OBJECT_ID_START,
	BG_DWOJKA_OBJECT_ID,
	BG_CHMURKA_OBJECT_ID,

	FG_RURA_OBJECT_ID = FOREGROUND_OBJECT_ID_START,
	FG_BRICKS_OBJECT_ID,

	BG_REP_FLOOR_ID = BACKGROUND_REP_OBJECT_ID_START,

	PLAYER_MARIO_ID = PLAYER_ID_START,

}GameObjectID;


typedef struct
{
	Sprite_t sprite;
	GameObjectID id;

}GameAsset_t;

typedef struct
{
	GameAsset_t baseAsset;

}BackgroundAsset_t;

typedef struct
{
	GameAsset_t baseAsset;

	Rect_t BBox; // p1 - offset from (0,0) in sprite, p2 - length

}ForegroundAsset_t;

typedef struct
{
	GameAsset_t baseAsset;

	Rect_t BBox; // p1 - offset from (0,0) in sprite, p2 - length

}EnemyAsset_t;

typedef struct
{
	GameAsset_t baseAsset;

	Rect_t BBox; // p1 - offset from (0,0) in sprite, p2 - length

}PlayerAsset_t;

typedef struct
{
	float		subpixelX;
	float		subpixelY;

	float		vx;	// 0:1, 1 = 16 pixels in a second
	float		vy;

}Body_t;

typedef struct
{
	GameObjectID id;
	int index;

}GameObjectRef_t;

typedef enum
{
	PLAYER_BUMP_ENEMY,
	PLAYER_BUMP_FG_OBJECT,
	PLAYER_BUMP_FLOOR,

}BumpID_t;

typedef struct
{
	BumpID_t bumpID;
	GameObjectRef_t actor1;
	GameObjectRef_t actor2;
	Rect_t bumpRect;

}Bump_t;

typedef struct
{
	int size;
	Bump_t bumps[COLLISIONS_SIZE] __attribute__((aligned(4)));

}CollState_t;

typedef struct
{
	int size;
	int bumps[COLLISION_CONTEXTS_SIZE];

}CollContext_t;

typedef struct
{
	GameObjectID id;

	Point_t		currMapPos;
	Point_t		prevMapPos;

	Body_t body;

	CollContext_t collCtx;

	const PlayerAsset_t* asset;

	int lifePoints;

	bool IsImmune;
	bool damageTaken;
	bool IsGrounded;

}PlayerState_t;

typedef struct
{
	GameObjectID id;

	Point_t		currMapPos;
	Point_t		prevMapPos;

	const EnemyAsset_t* asset;

	bool IsAlive;
	bool IsOnScreen;

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

	bool IsAlive;
	bool IsOnScreen;

}ForegroundObject_t;

typedef struct
{
	GameObjectID id;

	Point_t mapPos;
	Point_t	mulVector;

	const BackgroundAsset_t* asset;

}BackgroundRepObject_t;

typedef struct
{
	int floorYLevel;

}MapState_t;

typedef struct
{
	Point_t prevPos;
	Point_t currPos;

	Rect_t screenRect;

}CameraState_t;

typedef struct
{
	int frameTimeUS;
	float frameTimeS; // same as frameTimeUS only in float and in S

}FrameData_t;

typedef struct
{
	uint32_t buttons_state;
	uint32_t prev_buttons_state;

	FrameData_t frameData;

}InputState_t;

typedef struct
{
	GameObjectID id;
	int index;

}ObjectRef_t;

typedef struct
{
	Rect_t 			rect;
	int				objectsSize;
	ObjectRef_t		objects[DIRTY_RECTS_OBJ_ID_SIZE];
	int				used;

}DirtyRect_t;

typedef struct
{
	int activeEnemies;
	EnemyState_t pool[ENEMIES_MAX_SIZE];

}Enemies_t;

typedef struct
{
	int LCDOffsetX;

	DirtyRect_t	dirtyRects[DIRTY_RECTS_SIZE] __attribute__((aligned(4)));

}RendererState_t;


typedef struct
{
	// Player position
	// Player physics
	PlayerState_t player;

	// Enemies position
	// Enemies physics
	Enemies_t enemies;

	// Current visible world
	// Visible background objects
	// Visible enemies
	// Visible collision objects
	int activebgObjects;
	BackgroundObject_t bgObjects[BACKGROUND_OBJECTS_MAX_SIZE];

	int activefgObjects;
	ForegroundObject_t fgObjects[FOREGROUND_OBJECTS_MAX_SIZE];

	int activebgRepObjects;
	BackgroundRepObject_t bgRepObjects[BACKGROUND_REP_OBJECTS_MAX_SIZE];
	int floorIndex;

	MapState_t map;
	CameraState_t camera;

	InputState_t input;

	// Game machine state enum

	// Renderer data
	RendererState_t	renderer;

	CollState_t coll;

}GameContext_t;

typedef struct
{
	// Physics settings
	// Controller settings
	// Player BBox settings

}GameSettings_t;



#endif /* SOURCES_INC_GAME_TYPES_H_ */
