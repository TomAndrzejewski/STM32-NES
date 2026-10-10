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

#define OBJECTS_MANAGER_SPAWN_BUFFER_MAX_SIZE	(4)

///////////////////////////////////////////////////////////////////
#define ENEMIES_MAX_SIZE				(32)

#define BACKGROUND_OBJECTS_MAX_SIZE		(64)

#define FOREGROUND_OBJECTS_MAX_SIZE		(128)

#define BACKGROUND_REP_OBJECTS_MAX_SIZE	(8)

///////////////////////////////////////////////////////////////////
#define DIRTY_RECTS_SIZE				(64)

#define COLLISIONS_SIZE					(32)


///////////////////////////////////////////////////////////////////
#define COLL_TOP_ENABLED		(0x00000001)
#define COLL_DOWN_ENABLED		(0x00000002)
#define COLL_LEFT_ENABLED		(0x00000004)
#define COLL_RIGHT_ENABLED		(0x00000008)
#define COLL_ANY_ENABLED		( COLL_TOP_ENABLED \
								| COLL_DOWN_ENABLED \
								| COLL_LEFT_ENABLED \
								| COLL_RIGHT_ENABLED)
#define FG_SCROLL_RENDER		(0x00000010)
#define MIRROR_X				(0x00000020)
#define DESTROYABLE				(0x00000040)
#define HIDDEN					(0x00000080)
#define BUMPABLE				(0x00000100)
#define BUMPABLE_MULTIPLE_TIMES	(0x00000200)
#define REWARD_BIT_0			(0x00000400)
#define REWARD_BIT_1			(0x00000800)
#define REWARD_BIT_2			(0x00001000)
#define REWARD_BIT_MASK			(REWARD_BIT_0 | REWARD_BIT_1 | REWARD_BIT_2)
#define REWARD_PNACZE			(REWARD_BIT_0 | REWARD_BIT_1) // 3
#define REWARD_STARMAN			(REWARD_BIT_2) // 4
#define REWARD_EXTRA_LIFE		(REWARD_BIT_0 | REWARD_BIT_2) // 5
#define REWARD_LEVEL_UP			(REWARD_BIT_1 | REWARD_BIT_2) // 6
#define REWARD_SINGLE_COIN		(REWARD_BIT_0 | REWARD_BIT_1 | REWARD_BIT_2) // 7

#endif /* SOURCES_INC_GAME_DEFS_H_ */
