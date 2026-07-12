/*
 * Game.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include "string.h"

#include "NES_Functions.h"

#include "LCDControl.h"
#include "PADControl.h"
#include "RenderEngine.h"

#include "Game_Types.h"
#include "Assets.h"
#include "Level.h"

#include "Game.h"


int GAME_InitContext(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	memset(ctx, 0, sizeof(GameContext_t));

	///////////////////
	// BACKGROUND
	///////////////////
	for (int i = 0; i < BACKGROUND_OBJECTS_MAX_SIZE; i++)
	{
		ctx->bgObjects[i].id = OBJECT_NOT_USED;
	}

	const ObjectLevelPos_t* BGObjectsPos = NULL;
	int numOfBGObjects = 0;
	ret = LEVEL_GetBGObjectsLocations(&BGObjectsPos, &numOfBGObjects);
	if (ret < 0) { return -5; }
	if (BGObjectsPos == NULL) { return -10; }

	if (numOfBGObjects > BACKGROUND_OBJECTS_MAX_SIZE)
	{
		numOfBGObjects = BACKGROUND_OBJECTS_MAX_SIZE;
	}

	ctx->activebgObjects = 0;
	for (int i = 0; i < numOfBGObjects; i++)
	{
		if (BGObjectsPos[i].id < BACKGROUND_OBJECT_ID_START || BGObjectsPos[i].id > BACKGROUND_OBJECT_ID_END)
		{
			continue;
		}

		ctx->bgObjects[i].id = BGObjectsPos[i].id;
		ctx->bgObjects[i].mapPos.x = BGObjectsPos[i].x;
		ctx->bgObjects[i].mapPos.y = BGObjectsPos[i].y;

		switch (BGObjectsPos[i].id)
		{
		case BG_JEDYNKA_OBJECT_ID: {
			ctx->bgObjects[i].asset = &JEDYNKA_ASSET;
			break;
		}
		case BG_DWOJKA_OBJECT_ID: {
			ctx->bgObjects[i].asset = &DWOJKA_ASSET;
			break;
		}
		case BG_CHMURKA_OBJECT_ID: {
			ctx->bgObjects[i].asset = &CHMURKA_ASSET;
			break;
		}
		default:
			break;
		}

		ctx->activebgObjects++;
	}

	///////////////////
	// BACKGROUND REPETITION OBJECTS
	///////////////////
	for (int i = 0; i < BACKGROUND_REP_OBJECTS_MAX_SIZE; i++)
	{
		ctx->bgRepObjects[i].id = OBJECT_NOT_USED;
	}

	const RepObjectLevelPos_t* BGRepObjectsPos = NULL;
	int numOfBGRepObjects = 0;
	ret = LEVEL_GetBGRepObjectsLocations(&BGRepObjectsPos, &numOfBGRepObjects);
	if (ret < 0) { return -15; }
	if (BGRepObjectsPos == NULL) { return -20; }

	if (numOfBGRepObjects > BACKGROUND_REP_OBJECTS_MAX_SIZE)
	{
		numOfBGRepObjects = BACKGROUND_REP_OBJECTS_MAX_SIZE;
	}

	ctx->floorIndex = -1;
	ctx->activebgRepObjects = 0;
	for (int i = 0; i < numOfBGRepObjects; i++)
	{
		if (BGRepObjectsPos[i].id < BACKGROUND_REP_OBJECT_ID_START || BGRepObjectsPos[i].id > FOREGROUND_REP_OBJECT_ID_END)
		{
			continue;
		}

		ctx->bgRepObjects[i].id = BGRepObjectsPos[i].id;
		ctx->bgRepObjects[i].mapPos.x = BGRepObjectsPos[i].x;
		ctx->bgRepObjects[i].mapPos.y = BGRepObjectsPos[i].y;
		ctx->bgRepObjects[i].mulVector.x = BGRepObjectsPos[i].mulX;
		ctx->bgRepObjects[i].mulVector.y = BGRepObjectsPos[i].mulY;

		switch (BGRepObjectsPos[i].id)
		{
		case BG_REP_FLOOR_ID: {
			ctx->bgRepObjects[i].asset = &FLOOR_ASSET;
			ctx->floorIndex = i;
			break;
		}
		default:
			break;
		}

		ctx->activebgRepObjects++;
	}
	// floor is very important for next components
	if (ctx->floorIndex == -1)
	{
		return -25;
	}

	///////////////////
	// MAP
	///////////////////
	ctx->map.LCDOffsetX = 0;
	ctx->map.floorYLevel = ctx->bgRepObjects[ctx->floorIndex].asset->baseAsset.sprite.size.y * ctx->bgRepObjects[ctx->floorIndex].mulVector.y;

	///////////////////
	// CAMERA
	///////////////////
	ctx->camera.currPos = (Point_t){.x = 0, .y = 0};

	///////////////////
	// INPUT
	///////////////////
	ctx->input.buttons_state = 0;

	///////////////////
	// FRAME DATA
	///////////////////
	ctx->frameData.frameTimeUS = 0;

	///////////////////
	// PLAYER
	///////////////////
	ctx->player.asset = &MARIO_ASSET;
	ctx->player.id = ctx->player.asset->baseAsset.id;
	ctx->player.currMapPos.x = 32;
	ctx->player.currMapPos.y = ctx->map.floorYLevel;
	ctx->player.prevMapPos = ctx->player.currMapPos;
	ctx->player.vx = 0.0f;
	ctx->player.vy = 0.0f;

	///////////////////
	// ENEMIES
	///////////////////
	const ObjectLevelPos_t* EnemiesPos = NULL;
	int numOfEnemies = 0;
	ret = LEVEL_GetEnemiesLocations(&EnemiesPos, &numOfEnemies);
	if (ret < 0) { return -30; }
	if (EnemiesPos == NULL) { return -35; }

	if (numOfEnemies > ENEMIES_MAX_SIZE)
	{
		numOfEnemies = ENEMIES_MAX_SIZE;
	}

	ctx->enemies.activeEnemies = numOfEnemies;
	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		if (EnemiesPos[i].id < ENEMY_ID_START || EnemiesPos[i].id > ENEMY_ID_END)
		{
			continue;
		}

		ctx->enemies.enemiesArr[i].id = EnemiesPos[i].id;
		ctx->enemies.enemiesArr[i].currMapPos.x = EnemiesPos[i].x;
		ctx->enemies.enemiesArr[i].currMapPos.y = EnemiesPos[i].y;
		ctx->enemies.enemiesArr[i].prevMapPos = ctx->enemies.enemiesArr[i].currMapPos;

		switch (EnemiesPos[i].id)
		{
		case ENEMY_GOOMBA_ID: {
			ctx->enemies.enemiesArr[i].asset = &GOOMBA_ASSET;
			break;
		}
		}
	}


	return 0;
}

int INPUT_SetButtonsState(InputState_t* input, uint32_t buttons_state)
{
	if (input == NULL) { return -1; }
	input->buttons_state = buttons_state;
	return 0;
}

int INPUT_SetFrameTimeUS(FrameData_t* frameData, u32 frameTimeUS)
{
	if (frameData == NULL) { return -1; }
	frameData->frameTimeUS = frameTimeUS;
	frameData->frameTimeS = frameData->frameTimeUS/1000000.0;
	return 0;
}

int PHYSICS_Player_Movement(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }

	if (ctx->input.buttons_state & PAD_BUTTON_RIGHT)
	{
		if (ctx->player.vx < 1)
		{
			float dvx = ctx->frameData.frameTimeS;
			if (ctx->player.vx + dvx < 1)
			{
				ctx->player.vx += dvx;
			}
			else
			{
				ctx->player.vx = 1;
			}
		}
	}
	else
	{
		if (ctx->player.vx > 0)
		{
			float dvx = ctx->frameData.frameTimeS;
			if (ctx->player.vx - dvx > 0)
			{
				ctx->player.vx -= dvx;
			}
			else
			{
				ctx->player.vx = 0;
			}
		}
	}

	//todotomka 128 jako ustawienie (settings)
	ctx->player.subpixelX += (ctx->player.vx * SUBPIXEL_RESOLUTION * 128) / TARGET_FRAMERATE_HZ;

	return 0;
}

int PHYSICS_Player_CalcMapPos(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }

	const Rect_t* levelBounds;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	if (ret < 0) { return -5; }

	ctx->player.prevMapPos = ctx->player.currMapPos;

	// New map position

	int pixelsToMove = (int)ctx->player.subpixelX / SUBPIXEL_RESOLUTION;
	if (pixelsToMove > 0)
	{
		ctx->player.subpixelX -= pixelsToMove * SUBPIXEL_RESOLUTION;
		if (ctx->player.currMapPos.x < levelBounds->p2.x)
		{
			ctx->player.currMapPos.x += pixelsToMove;
		}
	}

	return 0;
}

int CAMERA_CalcPos(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }

	const Rect_t* levelBounds;
	int ret = LEVEL_GetLevelBoundaries(&levelBounds);
	if (ret < 0) { return -5; }

	ctx->camera.prevPos = ctx->camera.currPos;

	int diff = ctx->player.currMapPos.x - ctx->player.prevMapPos.x;
	if (diff != 0)
	{
		if (ctx->camera.currPos.x + diff >= 0 && ctx->camera.currPos.x + diff <= levelBounds->p2.x)
		{
			ctx->camera.currPos.x += diff;
		}
	}

	return 0;
}

int PLAYER_GetDirtyRect(const PlayerState_t* player, Rect_t* dirtyRect)
{
	if (player == NULL || dirtyRect == NULL) { return -1; }

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = player->prevMapPos;
	prevDirtyRect.p2.x = player->prevMapPos.x + player->asset->baseAsset.sprite.size.x;
	prevDirtyRect.p2.y = player->prevMapPos.y + player->asset->baseAsset.sprite.size.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = player->currMapPos;
	currDirtyRect.p2.x = player->currMapPos.x + player->asset->baseAsset.sprite.size.x;
	currDirtyRect.p2.y = player->currMapPos.y + player->asset->baseAsset.sprite.size.y;

	Rect_t commonDirtyRect;
	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	if (commonDirtyRect.p1.x <= commonDirtyRect.p2.x && commonDirtyRect.p1.y <= commonDirtyRect.p2.y)
	{
		*dirtyRect = commonDirtyRect;
	}
	else
	{
		return -5;
	}

	return 0;
}

int ENEMIES_GetDirtyRect(const EnemyState_t* enemy, Rect_t* dirtyRect)
{
	if (enemy == NULL || dirtyRect == NULL) { return -1; }

	Rect_t prevDirtyRect;
	prevDirtyRect.p1 = enemy->prevMapPos;
	prevDirtyRect.p2.x = enemy->prevMapPos.x + enemy->asset->baseAsset.sprite.size.x;
	prevDirtyRect.p2.y = enemy->prevMapPos.y + enemy->asset->baseAsset.sprite.size.y;

	Rect_t currDirtyRect;
	currDirtyRect.p1 = enemy->currMapPos;
	currDirtyRect.p2.x = enemy->currMapPos.x + enemy->asset->baseAsset.sprite.size.x;
	currDirtyRect.p2.y = enemy->currMapPos.y + enemy->asset->baseAsset.sprite.size.y;

	Rect_t commonDirtyRect;
	commonDirtyRect.p1.x = min(prevDirtyRect.p1.x, currDirtyRect.p1.x);
	commonDirtyRect.p1.y = min(prevDirtyRect.p1.y, currDirtyRect.p1.y);
	commonDirtyRect.p2.x = max(prevDirtyRect.p2.x, currDirtyRect.p2.x);
	commonDirtyRect.p2.y = max(prevDirtyRect.p2.y, currDirtyRect.p2.y);

	if (commonDirtyRect.p1.x <= commonDirtyRect.p2.x && commonDirtyRect.p1.y <= commonDirtyRect.p2.y)
	{
		*dirtyRect = commonDirtyRect;
	}
	else
	{
		return -5;
	}

	return 0;
}

int RENDER_FirstRender(const GameContext_t* ctx)
{
	if (ctx == NULL)	{ return -1; }
	if (ctx->floorIndex < 0) { return -5; }

	LCD_WriteVertScrollStartAddr(ctx->map.LCDOffsetX);

	RENDER_RenderFloor(&ctx->bgRepObjects[ctx->floorIndex]);

	for (int i = 0; i < LCD_WIDTH/20; i++)
	{
		//sanity check
		if (ctx->map.floorYLevel < 0 || ctx->map.floorYLevel > 200) { return -10; }

		Rect_t mapRect;
		mapRect.p1.x = i * 20;
		mapRect.p1.y = ctx->map.floorYLevel;
		mapRect.p2.x = i * 20 + 20;
		mapRect.p2.y = LCD_HEIGHT;

		Rect_t screenRect = mapRect;

		int baseRectArea = CalcRectArea(screenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			Point_t spritePos = ctx->bgObjects[j].mapPos;
			RENDER_RenderBgdSprite(&ctx->bgObjects[j].asset->baseAsset.sprite, spritePos, mapRect, screenRect, ctx->map.LCDOffsetX, false, false);
		}

		RE_SendRect(screenRect, ctx->map.LCDOffsetX);
	}

	return 0;
}

int REDNER_ScrollRender(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }

	Point_t cameraDiff = {0};
	cameraDiff.x = ctx->camera.currPos.x - ctx->camera.prevPos.x;

	if (cameraDiff.x > 0)
	{
		int prevLCDOffsetX = ctx->map.LCDOffsetX;

		ctx->map.LCDOffsetX -= cameraDiff.x;
		// todotomka da sie ladniej, czytelniej robic sprawdzanie zakresow?
		if (ctx->map.LCDOffsetX > 319)
		{
			ctx->map.LCDOffsetX = ctx->map.LCDOffsetX - 320;
		}
		if (ctx->map.LCDOffsetX < 0)
		{
			ctx->map.LCDOffsetX = 320 + ctx->map.LCDOffsetX;
		}


		Rect_t rightMapRect;
		Rect_t leftScreenRect;
		Rect_t rightScreenRect;

		rightMapRect.p1.x = ctx->camera.prevPos.x + LCD_WIDTH - 1;
		rightMapRect.p1.y = ctx->map.floorYLevel;
		rightMapRect.p2.x = ctx->camera.currPos.x + LCD_WIDTH - 1;
		rightMapRect.p2.y = LCD_HEIGHT;

		leftScreenRect.p1.x = 0;
		leftScreenRect.p1.y = ctx->map.floorYLevel;
		leftScreenRect.p2.x = cameraDiff.x;
		leftScreenRect.p2.y = LCD_HEIGHT;

		rightScreenRect.p1.x = LCD_WIDTH - cameraDiff.x - 1;
		rightScreenRect.p1.y = ctx->map.floorYLevel;
		rightScreenRect.p2.x = LCD_WIDTH - 1;
		rightScreenRect.p2.y = LCD_HEIGHT;

		int baseRectArea;

		// LEFT
		baseRectArea = CalcRectArea(leftScreenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);
		RE_SendRect(leftScreenRect, prevLCDOffsetX);

		// SHIFT SCROLL
		LCD_WriteVertScrollStartAddr(ctx->map.LCDOffsetX);

		// RIGHT
		baseRectArea = CalcRectArea(rightScreenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);


		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			Point_t spritePos = ctx->bgObjects[j].mapPos;
			RENDER_RenderBgdSprite(&ctx->bgObjects[j].asset->baseAsset.sprite, spritePos, rightMapRect, rightScreenRect, ctx->map.LCDOffsetX, false, false);
		}

		RE_SendRect(rightScreenRect, ctx->map.LCDOffsetX);
	}

	return 0;
}

int RENDER_RenderObjects(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }

	int ret = 0;
	int dirtyRectIndex = 0;

	fast_memset(&ctx->dirtyRects, 0, sizeof(ctx->dirtyRects));

	Rect_t cameraRect;
	cameraRect.p1.x = ctx->camera.currPos.x;
	cameraRect.p1.y = ctx->camera.currPos.y;
	cameraRect.p2.x = cameraRect.p1.x + LCD_WIDTH;
	cameraRect.p2.y = cameraRect.p1.y + LCD_HEIGHT;

	//////////////////////////
	// ENEMIES DIRTY RECTS
	//////////////////////////
	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		Rect_t dirtyRect;
		if (ENEMIES_GetDirtyRect(&ctx->enemies.enemiesArr[i], &dirtyRect) < 0) { continue; }

		Rect_t commonRect;
		commonRect.p1.x = max(cameraRect.p1.x, dirtyRect.p1.x);
		commonRect.p1.y = max(cameraRect.p1.y, dirtyRect.p1.y);
		commonRect.p2.x = min(cameraRect.p2.x, dirtyRect.p2.x);
		commonRect.p2.y = min(cameraRect.p2.y, dirtyRect.p2.y);

		// czesc wspolna istnieje
		if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
		{
			ctx->dirtyRects[dirtyRectIndex].rect = commonRect;
			ctx->dirtyRects[dirtyRectIndex].objects[ctx->dirtyRects[dirtyRectIndex].objectsSize++] = ctx->enemies.enemiesArr[i].id;
			dirtyRectIndex++;
		}
	}

	//////////////////////////
	// PLAYER DIRTY RECT
	//////////////////////////
	Rect_t dirtyRect;
	if (!PLAYER_GetDirtyRect(&ctx->player, &dirtyRect))
	{
		Rect_t commonRect;
		commonRect.p1.x = max(cameraRect.p1.x, dirtyRect.p1.x);
		commonRect.p1.y = max(cameraRect.p1.y, dirtyRect.p1.y);
		commonRect.p2.x = min(cameraRect.p2.x, dirtyRect.p2.x);
		commonRect.p2.y = min(cameraRect.p2.y, dirtyRect.p2.y);

		// czesc wspolna istnieje
		if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
		{
			ctx->dirtyRects[dirtyRectIndex].rect = commonRect;
			ctx->dirtyRects[dirtyRectIndex].objects[ctx->dirtyRects[dirtyRectIndex].objectsSize++] = ctx->player.id;
			dirtyRectIndex++;
		}
	}

	for (int i = 0; i < dirtyRectIndex; i++)
	{
		if (ctx->dirtyRects[i].used)	{ continue; }

		bool commonRectFound = false;
		for (int j = 0; j < dirtyRectIndex; j++)
		{
			if (i == j)	{ continue; }
			if (ctx->dirtyRects[j].used)	{ continue; }

			Rect_t commonRect;
			commonRect.p1.x = max(ctx->dirtyRects[i].rect.p1.x, ctx->dirtyRects[j].rect.p1.x);
			commonRect.p1.y = max(ctx->dirtyRects[i].rect.p1.y, ctx->dirtyRects[j].rect.p1.y);
			commonRect.p2.x = min(ctx->dirtyRects[i].rect.p2.x, ctx->dirtyRects[j].rect.p2.x);
			commonRect.p2.y = min(ctx->dirtyRects[i].rect.p2.y, ctx->dirtyRects[j].rect.p2.y);

			// czesc wspolna istnieje
			if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
			{
				Rect_t commonORRect;
				commonORRect.p1.x = min(ctx->dirtyRects[i].rect.p1.x, ctx->dirtyRects[j].rect.p1.x);
				commonORRect.p1.y = min(ctx->dirtyRects[i].rect.p1.y, ctx->dirtyRects[j].rect.p1.y);
				commonORRect.p2.x = max(ctx->dirtyRects[i].rect.p2.x, ctx->dirtyRects[j].rect.p2.x);
				commonORRect.p2.y = max(ctx->dirtyRects[i].rect.p2.y, ctx->dirtyRects[j].rect.p2.y);

				commonRectFound = true;
				ctx->dirtyRects[j].used = true;
				ctx->dirtyRects[i].rect = commonORRect;
				for (int k = 0; k < ctx->dirtyRects[j].objectsSize; k++)
				{
					ctx->dirtyRects[i].objects[ctx->dirtyRects[i].objectsSize++] = ctx->dirtyRects[j].objects[k];
				}
				break;
			}
		}

		if (commonRectFound)
		{
			i--;
		}
	}

	for (int i = 0; i < dirtyRectIndex; i++)
	{
		if (ctx->dirtyRects[i].used)	{ continue; }

		DirtyRect_t* dirtyRect = &ctx->dirtyRects[i];

		Rect_t screenRect;
		screenRect.p1.x = dirtyRect->rect.p1.x - ctx->camera.currPos.x;
		screenRect.p1.y = dirtyRect->rect.p1.y - ctx->camera.currPos.y;
		screenRect.p2.x = dirtyRect->rect.p2.x - ctx->camera.currPos.x;
		screenRect.p2.y = dirtyRect->rect.p2.y - ctx->camera.currPos.y;

		// BACKGROUND COLOR
		int baseRectArea = CalcRectArea(dirtyRect->rect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		// BACKGROUND SPRITES
		int elementSize = sizeof(SpritePos_t);
		if (elementSize > 0)
		{
			int numOfSprites = sizeof(MapSpriteLoc)/elementSize;
			for (int i = 0; i < numOfSprites; i++)
			{
				SpritePos_t spritePos = MapSpriteLoc[i];
				switch (spritePos.spriteID)
				{
				case JEDYNKA_SPRITE_ID:
				{
					Map_RenderBgdSprite(&p->jedynkaSprite, spritePos, dirtyRect->rect, screenRect, p->LCDOffsetX, false, false);
					break;
				}
				case DWOJKA_SPRITE_ID:
				{
					Map_RenderBgdSprite(&p->dwojkaSprite, spritePos, dirtyRect->rect, screenRect, p->LCDOffsetX, false, false);
					break;
				}
				case CHMURKA_SPRITE_ID:
				{
					Map_RenderBgdSprite(&p->chmurkaSprite, spritePos, dirtyRect->rect, screenRect, p->LCDOffsetX, false, false);
					break;
				}
				}
			}
		}

		for (int j = 0; j < NUM_OF_OBJ_IDS; j++)
		{
			int currentPrioObject = ObjPriorities[j];

			for (int k = 0; k < dirtyRect->objIDIndex; k++)
			{
				int objID = dirtyRect->objID[k].objID;
				if (objID != currentPrioObject)
				{
					continue;
				}

				switch (objID)
				{
				case MARIO_OBJ_ID:
				{
					Map_RenderMario(p->currCameraPos, dirtyRect->rect, screenRect, p->LCDOffsetX);
					break;
				}
				case GOOMBA_OBJ_ID:
				{
					Goomba_t* goomba = NULL;
					int ret = Enemies_GetGoomba(pEnemies, dirtyRect->objID[k].objIndex, &goomba);
					if (ret < 0)	{ break; }

					Map_RenderGoomba(goomba, p->currCameraPos, dirtyRect->rect, screenRect, p->LCDOffsetX);
					break;
				}
				default:
					break;
				}
			}
		}

		RE_SendRect(screenRect, p->LCDOffsetX);
	}







//	if (p == NULL)	{ return -1; }
//	int ret = 0;
//	int dirtyRectIndex = 0;
//
//	uint32_t startTime = 0, elapsedUS = 0;
//	startTime = GetTimestamp();
////	memset(&p->dirtyRects, 0, sizeof(p->dirtyRects));
//	fast_memset(&p->dirtyRects, 0, sizeof(p->dirtyRects));
//	elapsedUS = CalcTimeUS(startTime);
////	if (elapsedUS > 0)
//	{
//		printf_uint(elapsedUS); printf_c('\t');
//	}
//
//
//	Rect_t cameraRect;
//	cameraRect.p1.x = p->currCameraPos.x;
//	cameraRect.p1.y = p->currCameraPos.y;
//	cameraRect.p2.x = cameraRect.p1.x + LCD_WIDTH;
//	cameraRect.p2.y = cameraRect.p1.y + LCD_HEIGHT;
//
//	for (int i = 0; i < NUM_OF_GOOMBAS; i++)
//	{
//		Goomba_t* goomba = NULL;
//		int ret = Enemies_GetGoomba(pEnemies, i, &goomba);
//		if (ret < 0)	{ continue; }
//
//		Rect_t dirtyRect;
//		if (Goomba_GetDirtyRect(goomba, &dirtyRect) < 0)	{ continue; }
//
//		Rect_t commonRect;
//		commonRect.p1.x = max(cameraRect.p1.x, dirtyRect.p1.x);
//		commonRect.p1.y = max(cameraRect.p1.y, dirtyRect.p1.y);
//		commonRect.p2.x = min(cameraRect.p2.x, dirtyRect.p2.x);
//		commonRect.p2.y = min(cameraRect.p2.y, dirtyRect.p2.y);
//
//		// czesc wspolna istnieje
//		if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
//		{
//			p->dirtyRects[dirtyRectIndex].rect = commonRect;
//			p->dirtyRects[dirtyRectIndex].objID[p->dirtyRects[dirtyRectIndex].objIDIndex++] = (ObjectID_t){ .objID = GOOMBA_OBJ_ID, .objIndex = i};
//			dirtyRectIndex++;
//		}
//	}
//
//	Rect_t marioDirtyMapRect;
//	ret = Map_GetMarioDirtyRect(cameraRect, &marioDirtyMapRect);
//	if (!ret)
//	{
//		p->dirtyRects[dirtyRectIndex].rect = marioDirtyMapRect;
//		p->dirtyRects[dirtyRectIndex].objID[p->dirtyRects[dirtyRectIndex].objIDIndex++] = (ObjectID_t){ .objID = MARIO_OBJ_ID, .objIndex = 0};
//		dirtyRectIndex++;
//	}
//
//	for (int i = 0; i < dirtyRectIndex; i++)
//	{
//		if (p->dirtyRects[i].used)	{ continue; }
//
//		bool commonRectFound = false;
//		for (int j = 0; j < dirtyRectIndex; j++)
//		{
//			if (i == j)	{ continue; }
//			if (p->dirtyRects[j].used)	{ continue; }
//
//			Rect_t commonRect;
//			commonRect.p1.x = max(p->dirtyRects[i].rect.p1.x, p->dirtyRects[j].rect.p1.x);
//			commonRect.p1.y = max(p->dirtyRects[i].rect.p1.y, p->dirtyRects[j].rect.p1.y);
//			commonRect.p2.x = min(p->dirtyRects[i].rect.p2.x, p->dirtyRects[j].rect.p2.x);
//			commonRect.p2.y = min(p->dirtyRects[i].rect.p2.y, p->dirtyRects[j].rect.p2.y);
//
//			// czesc wspolna istnieje
//			if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
//			{
//				Rect_t commonORRect;
//				commonORRect.p1.x = min(p->dirtyRects[i].rect.p1.x, p->dirtyRects[j].rect.p1.x);
//				commonORRect.p1.y = min(p->dirtyRects[i].rect.p1.y, p->dirtyRects[j].rect.p1.y);
//				commonORRect.p2.x = max(p->dirtyRects[i].rect.p2.x, p->dirtyRects[j].rect.p2.x);
//				commonORRect.p2.y = max(p->dirtyRects[i].rect.p2.y, p->dirtyRects[j].rect.p2.y);
//
//				commonRectFound = true;
//				p->dirtyRects[j].used = true;
//				p->dirtyRects[i].rect = commonORRect;
//				for (int k = 0; k < p->dirtyRects[j].objIDIndex; k++)
//				{
//					p->dirtyRects[i].objID[p->dirtyRects[i].objIDIndex++] = p->dirtyRects[j].objID[k];
//				}
//				break;
//			}
//		}
//
//		if (commonRectFound)
//		{
//			i--;
//		}
//	}
//
//	for (int i = 0; i < dirtyRectIndex; i++)
//	{
//		if (p->dirtyRects[i].used)	{ continue; }
//
//		DirtyRect_t* dirtyRect = &p->dirtyRects[i];
//
//		Rect_t screenRect;
//		screenRect.p1.x = dirtyRect->rect.p1.x - p->currCameraPos.x;
//		screenRect.p1.y = dirtyRect->rect.p1.y - p->currCameraPos.y;
//		screenRect.p2.x = dirtyRect->rect.p2.x - p->currCameraPos.x;
//		screenRect.p2.y = dirtyRect->rect.p2.y - p->currCameraPos.y;
//
//		// BACKGROUND COLOR
//		int baseRectArea = CalcRectArea(dirtyRect->rect);
//		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);
//
//		// BACKGROUND SPRITES
//		int elementSize = sizeof(SpritePos_t);
//		if (elementSize > 0)
//		{
//			int numOfSprites = sizeof(MapSpriteLoc)/elementSize;
//			for (int i = 0; i < numOfSprites; i++)
//			{
//				SpritePos_t spritePos = MapSpriteLoc[i];
//				switch (spritePos.spriteID)
//				{
//				case JEDYNKA_SPRITE_ID:
//				{
//					Map_RenderBgdSprite(&p->jedynkaSprite, spritePos, dirtyRect->rect, screenRect, p->LCDOffsetX, false, false);
//					break;
//				}
//				case DWOJKA_SPRITE_ID:
//				{
//					Map_RenderBgdSprite(&p->dwojkaSprite, spritePos, dirtyRect->rect, screenRect, p->LCDOffsetX, false, false);
//					break;
//				}
//				case CHMURKA_SPRITE_ID:
//				{
//					Map_RenderBgdSprite(&p->chmurkaSprite, spritePos, dirtyRect->rect, screenRect, p->LCDOffsetX, false, false);
//					break;
//				}
//				}
//			}
//		}
//
//		for (int j = 0; j < NUM_OF_OBJ_IDS; j++)
//		{
//			int currentPrioObject = ObjPriorities[j];
//
//			for (int k = 0; k < dirtyRect->objIDIndex; k++)
//			{
//				int objID = dirtyRect->objID[k].objID;
//				if (objID != currentPrioObject)
//				{
//					continue;
//				}
//
//				switch (objID)
//				{
//				case MARIO_OBJ_ID:
//				{
//					Map_RenderMario(p->currCameraPos, dirtyRect->rect, screenRect, p->LCDOffsetX);
//					break;
//				}
//				case GOOMBA_OBJ_ID:
//				{
//					Goomba_t* goomba = NULL;
//					int ret = Enemies_GetGoomba(pEnemies, dirtyRect->objID[k].objIndex, &goomba);
//					if (ret < 0)	{ break; }
//
//					Map_RenderGoomba(goomba, p->currCameraPos, dirtyRect->rect, screenRect, p->LCDOffsetX);
//					break;
//				}
//				default:
//					break;
//				}
//			}
//		}
//
//		RE_SendRect(screenRect, p->LCDOffsetX);
//	}
}

int RENDER_RenderFloor(const BackgroundRepObject_t* floor)
{
	if (floor == NULL)	{ return -1; }

	const GameAsset_t* baseAsset = &floor->asset->baseAsset;
	SpriteRender_t renderContext;

	for (int i = 0; i < floor->mulVector.y; i++)
	{
		for (int j = 0; j < floor->mulVector.x; j++)
		{
			renderContext.baseRect.p1.x = j * baseAsset->sprite.size.x;
			renderContext.baseRect.p1.y = i * baseAsset->sprite.size.y;
			renderContext.baseRect.p2.x = renderContext.baseRect.p1.x + baseAsset->sprite.size.x;
			renderContext.baseRect.p2.y = renderContext.baseRect.p1.y + baseAsset->sprite.size.y;

			renderContext.commonRect = renderContext.baseRect;

			renderContext.baseToSpriteOffset.x = 0;
			renderContext.baseToSpriteOffset.y = 0;
			renderContext.LCDOffsetX = 0;

			RE_RenderSprite(&baseAsset->sprite, renderContext, false);
		}
	}

	return 0;
}

int RENDER_RenderBgdSprite(const Sprite_t* p, Point_t spritePos, Rect_t mapRectToDraw, Rect_t screenRect, int LCDOffsetX, bool render, bool fillBG)
{
	if (p == NULL)	{ return -1; }

	Rect_t posRect;
	posRect.p1.x = spritePos.x;
	posRect.p1.y = spritePos.y;
	posRect.p2.x = spritePos.x + p->size.x;
	posRect.p2.y = spritePos.y + p->size.y;

	Rect_t commonRect;
	commonRect.p1.x = max(mapRectToDraw.p1.x, posRect.p1.x);
	commonRect.p1.y = max(mapRectToDraw.p1.y, posRect.p1.y);
	commonRect.p2.x = min(mapRectToDraw.p2.x, posRect.p2.x);
	commonRect.p2.y = min(mapRectToDraw.p2.y, posRect.p2.y);

	// czesc wspolna istnieje
	if (commonRect.p1.x <= commonRect.p2.x && commonRect.p1.y <= commonRect.p2.y)
	{
		SpriteRender_t renderContext;
		renderContext.commonRect = commonRect;
		renderContext.baseRect = screenRect;
		renderContext.baseToSpriteOffset.x = posRect.p1.x - mapRectToDraw.p1.x;
		renderContext.baseToSpriteOffset.y = posRect.p1.y - mapRectToDraw.p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;

		if (render)
		{
			RE_RenderSprite(p, renderContext, fillBG);
		}
		else
		{
			RE_FillSprite3(p, renderContext);
		}
	}

	return 0;
}

