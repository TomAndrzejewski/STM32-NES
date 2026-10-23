/*
 * GameState.c
 *
 *  Created on: 23 paz 2026
 *      Author: tomasz
 */

#include "GameState.h"

#include <stddef.h>

#include "Game.h"
#include "Game_Defs.h"
#include "Game_Types.h"
#include "BoardAssert.h"
#include "Physics.h"
#include "printf_logger.h"


//----------------
// PRIVATE MACROS
//----------------
// Time the world stays frozen while the player grows
#define GAME_STATE_LEVEL_UP_DURATION_US		(1000000)
// Time of the death jump before the game is over
#define GAME_STATE_DYING_DURATION_US		(3000000)


//----------------
// PRIVATE VARIABLES
//----------------
// Game systems running in each game state
static const uint32_t s_stateSystems[] = {
	[GAME_STATE_PLAYING]			= GAME_SYSTEM_SIMULATION_TIME
									| GAME_SYSTEM_PLAYER_PHYSICS
									| GAME_SYSTEM_WORLD_PHYSICS
									| GAME_SYSTEM_COLLISION
									| GAME_SYSTEM_ANIMATOR,
	[GAME_STATE_PLAYER_LEVEL_UP]	= 0,
	[GAME_STATE_PLAYER_DYING]		= GAME_SYSTEM_ANIMATOR,
	[GAME_STATE_GAME_OVER]			= 0,
};

// Systems table is shorter than the states list: add an entry for the new state
_Static_assert(sizeof(s_stateSystems) / sizeof(s_stateSystems[0]) == GAME_STATE_COUNT, "s_stateSystems has no entry for every game state");


//----------------
// PRIVATE FUNCTION PROTOTYPES
//----------------
static void GAME_STATE_HandleEvent(GameStateEnum state, GameEventEnum event, GameStateEnum* nextState);
static void GAME_STATE_ChangeState(GameStateMachine_t* sm, GameStateEnum nextState, GameContext_t* ctx);
static void GAME_STATE_Exit(GameStateEnum state, GameContext_t* ctx);
static void GAME_STATE_Enter(GameStateEnum state, GameContext_t* ctx);
static void GAME_STATE_Run(const GameStateMachine_t* sm, GameContext_t* ctx, GameStateEnum* nextState);


//----------------
// PUBLIC FUNCTIONS
//----------------
void GAME_STATE_Init(GameStateMachine_t* sm)
{
	BOARD_ASSERT(sm != NULL);

	sm->state = GAME_STATE_PLAYING;
	sm->timeInStateUS = 0;
	sm->eventsSize = 0;
	sm->droppedEvents = 0;
}

void GAME_STATE_Update(GameStateMachine_t* sm, GameContext_t* ctx)
{
	BOARD_ASSERT(sm != NULL && ctx != NULL);

	sm->timeInStateUS += ctx->input.frameData.frameTimeUS;

	// Events raised since the last update, in the order they came, may switch the state
	for (int i = 0; i < sm->eventsSize; i++)
	{
		GameStateEnum nextState = sm->state;
		GAME_STATE_HandleEvent(sm->state, sm->events[i], &nextState);
		if (nextState != sm->state) {
			GAME_STATE_ChangeState(sm, nextState, ctx);
		}
	}
	sm->eventsSize = 0;

	// Per frame actions of the current state, which may also finish the state
	GameStateEnum nextState = sm->state;
	GAME_STATE_Run(sm, ctx, &nextState);
	if (nextState != sm->state) {
		GAME_STATE_ChangeState(sm, nextState, ctx);
	}
}

void GAME_STATE_RaiseEvent(GameStateMachine_t* sm, GameEventEnum event)
{
	BOARD_ASSERT(sm != NULL);

	// Event buffer is full: skip the event and count it
	if (sm->eventsSize >= GAME_STATE_EVENTS_MAX_SIZE) {
		sm->droppedEvents++;
		return;
	}

	sm->events[sm->eventsSize] = event;
	sm->eventsSize++;
}

bool GAME_STATE_IsSystemEnabled(const GameStateMachine_t* sm, uint32_t system)
{
	BOARD_ASSERT(sm != NULL);
	BOARD_ASSERT(sm->state < GAME_STATE_COUNT);

	return (s_stateSystems[sm->state] & system) != 0;
}


//----------------
// PRIVATE FUNCTIONS
//----------------
static void GAME_STATE_HandleEvent(GameStateEnum state, GameEventEnum event, GameStateEnum* nextState)
{
	switch (state)
	{
	case GAME_STATE_PLAYING:
	{
		if (event == GAME_EVENT_PLAYER_LEVEL_UP) {
			*nextState = GAME_STATE_PLAYER_LEVEL_UP;
		} else if (event == GAME_EVENT_PLAYER_DIED) {
			*nextState = GAME_STATE_PLAYER_DYING;
		}
		break;
	}
	case GAME_STATE_PLAYER_LEVEL_UP:
	case GAME_STATE_PLAYER_DYING:
	case GAME_STATE_GAME_OVER:
		break;
	default:
		// Unknown game state: add a missing case for it
		BOARD_ASSERT(false);
		break;
	}
}

static void GAME_STATE_ChangeState(GameStateMachine_t* sm, GameStateEnum nextState, GameContext_t* ctx)
{
	printf_v("Game state: %d -> %d\n", (int)sm->state, (int)nextState);

	GAME_STATE_Exit(sm->state, ctx);

	sm->state = nextState;
	sm->timeInStateUS = 0;

	GAME_STATE_Enter(sm->state, ctx);
}

static void GAME_STATE_Exit(GameStateEnum state, GameContext_t* ctx)
{
	(void)ctx;

	switch (state)
	{
	case GAME_STATE_PLAYING:
	case GAME_STATE_PLAYER_LEVEL_UP:
	case GAME_STATE_PLAYER_DYING:
	case GAME_STATE_GAME_OVER:
		break;
	default:
		// Unknown game state: add a missing case for it
		BOARD_ASSERT(false);
		break;
	}
}

static void GAME_STATE_Enter(GameStateEnum state, GameContext_t* ctx)
{
	switch (state)
	{
	case GAME_STATE_PLAYER_LEVEL_UP:
	{
		PLAYER_LevelUp(&ctx->player);
		break;
	}
	case GAME_STATE_PLAYER_DYING:
	{
		PHYSICS_Player_DeathStart(&ctx->player);
		break;
	}
	case GAME_STATE_PLAYING:
	case GAME_STATE_GAME_OVER:
		break;
	default:
		// Unknown game state: add a missing case for it
		BOARD_ASSERT(false);
		break;
	}
}

static void GAME_STATE_Run(const GameStateMachine_t* sm, GameContext_t* ctx, GameStateEnum* nextState)
{
	switch (sm->state)
	{
	case GAME_STATE_PLAYER_LEVEL_UP:
	{
		// Resume the game once the growing time has passed
		if (sm->timeInStateUS >= GAME_STATE_LEVEL_UP_DURATION_US) {
			*nextState = GAME_STATE_PLAYING;
		}
		break;
	}
	case GAME_STATE_PLAYER_DYING:
	{
		// Move the player along the death jump until its time has passed
		PHYSICS_Player_DeathUpdate(&ctx->player, ctx);
		if (sm->timeInStateUS >= GAME_STATE_DYING_DURATION_US) {
			*nextState = GAME_STATE_GAME_OVER;
		}
		break;
	}
	case GAME_STATE_PLAYING:
	case GAME_STATE_GAME_OVER:
		break;
	default:
		// Unknown game state: add a missing case for it
		BOARD_ASSERT(false);
		break;
	}
}
