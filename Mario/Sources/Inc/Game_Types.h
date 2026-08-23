/*
 * Game_Types.h
 *
 *  Created on: 30 cze 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_GAME_TYPES_H_
#define SOURCES_INC_GAME_TYPES_H_

#include <stdbool.h>

// #include "NES_Defs.h"
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
#define BACKGROUND_REP_OBJECT_ID_END	(0x00003FFF)

#define PLAYER_ID_START					(0x00004000)
#define PLAYER_ID_END					(0x00004FFF)


//-----------------------------------------------------------
// REMEMBER! ADD NEW ID TO Level.c ObjRenderPriorities TAB
//-----------------------------------------------------------
typedef enum
{
	OBJECT_NOT_USED = -1,

	ENEMY_GOOMBA_ID = ENEMY_ID_START,

	BG_JEDYNKA_OBJECT_ID = BACKGROUND_OBJECT_ID_START,
	BG_DWOJKA_OBJECT_ID,
	BG_CHMURKA_OBJECT_ID,

	FG_RURA_OBJECT_ID = FOREGROUND_OBJECT_ID_START,
	FG_BRICKS_OBJECT_ID,
	FG_BLOCK_QMARK_OBJECT_ID,
	FG_RURA_DOL_OBJECT_ID,
	FG_RURA_GORA_OBJECT_ID,
	FG_PYRAMID_BLOCK_OBJECT_ID,

	BG_REP_FLOOR_ID = BACKGROUND_REP_OBJECT_ID_START,

	PLAYER_MARIO_ID = PLAYER_ID_START,

}GameObjectID;


typedef struct
{
	Sprite_t sprite;

}BaseAsset_t;

typedef struct
{
	GameObjectID id;
	BaseAsset_t baseAsset;

}BackgroundAsset_t;

typedef struct
{
	GameObjectID id;
	BaseAsset_t baseAsset;

	Rect_t BBox; // p1 - offset from (0,0) in sprite, p2 - length

}Asset_t;

//////////////
// ANIMACJE
//
// ANIMABLEASSET_T TRZYMA WSKAZNIK NA TABLICE WSKAZNIKOW DO BASEASSETOW WE FLASHU I NA ANIMATION_ID
// W FOR SZUKAMY ODPOWIEDNIEGO DO USTAWIENIA ANIMATION_ID
// ANIMATOR STERUJE WYBOREM ANIMATION_ID I PRZY TYM ASSETEM
//

typedef enum
{
	DUPA,
}AnimationID_t;

typedef struct
{
	GameObjectID id;

	int baseAssetsCount;
	BaseAsset_t** baseAssets;
	AnimationID_t currAnimation;

	Rect_t BBox; // p1 - offset from (0,0) in sprite, p2 - length

}AnimableAsset_t;

//typedef struct
//{
//
//}Animator_t;

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

}CollisionState_t;

typedef enum
{
	PLAYER_LITTLE,
	PLAYER_BIG,
	PLAYER_SHOOTING,
}PlayerLevel;

typedef struct
{
	GameObjectID id;

	Point_t		currMapPos;
	Point_t		prevMapPos;

	Body_t body;

	const Asset_t* asset;

	PlayerLevel	playerLevel;
	int lifePoints;

	bool IsImmune;
	bool damageTaken;
	bool IsGrounded;
	bool JustKilledFGObject;

}PlayerState_t;

typedef struct
{
	GameObjectID id;

	Point_t		currMapPos;
	Point_t		prevMapPos;

	const Asset_t* asset;

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
	Point_t BBoxCenter;

	const Asset_t* asset;

	uint32_t flags;

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
	Rect_t 	rect;
	int		used;

}DirtyRect_t;

typedef struct
{
	int activeEnemies;
	EnemyState_t pool[ENEMIES_MAX_SIZE];

}Enemies_t;

typedef struct
{
	int LCDOffsetX;

	int activeDirtyRects;
	DirtyRect_t	dirtyRects[DIRTY_RECTS_SIZE];

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

	CollisionState_t collision;

}GameContext_t;

typedef struct
{
	// Physics settings
	// Controller settings
	// Player BBox settings

}GameSettings_t;



#endif /* SOURCES_INC_GAME_TYPES_H_ */
