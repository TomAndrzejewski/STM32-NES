/*
 * Game_Defs.h
 *
 *  Created on: 1 lip 2026
 *      Author: Tomek Andrzejewski
 */

#ifndef SOURCES_INC_GAME_DEFS_H_
#define SOURCES_INC_GAME_DEFS_H_

///////////////////////////////////////////////////////////////////
#define TARGET_FRAMERATE_HZ				(60)

#define SUBPIXEL_RESOLUTION				(16)

///////////////////////////////////////////////////////////////////
#define OBJECTS_MANAGER_LEFT_DESPAWN_OFFSET		(64)
#define OBJECTS_MANAGER_RIGHT_SPAWN_OFFSET		(64)

///////////////////////////////////////////////////////////////////
#define ENEMIES_MAX_SIZE				(64)

#define BACKGROUND_OBJECTS_MAX_SIZE		(256)

#define FOREGROUND_OBJECTS_MAX_SIZE		(256)

#define BACKGROUND_REP_OBJECTS_MAX_SIZE	(8)

///////////////////////////////////////////////////////////////////
#define DIRTY_RECTS_SIZE				(64)

#define COLLISIONS_SIZE					(64)
#define COLLISION_CONTEXTS_SIZE			(8)


///////////////////////////////////////////////////////////////////
#define COLL_TOP_ENABLED					(0x00000001)
#define COLL_DOWN_ENABLED					(0x00000002)
#define COLL_LEFT_ENABLED					(0x00000004)
#define COLL_RIGHT_ENABLED					(0x00000008)
#define COLL_ANY_ENABLED					( COLL_TOP_ENABLED \
											| COLL_DOWN_ENABLED \
											| COLL_LEFT_ENABLED \
											| COLL_RIGHT_ENABLED)
#define FG_SCROLL_RENDER					(0x00000010)
#define MIRROR_X							(0x00000020)

#endif /* SOURCES_INC_GAME_DEFS_H_ */
