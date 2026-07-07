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
	ctx->map.currCameraPos.x = 0;
	ctx->map.currCameraPos.y = 0;
	ctx->map.LCDOffsetX = 0;
	ctx->map.floorYLevel = ctx->bgRepObjects[ctx->floorIndex].asset->baseAsset.sprite.size.y * ctx->bgRepObjects[ctx->floorIndex].mulVector.y;

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
	ctx->player.currMapPos.x = 0;
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

	for (int i = 0; i < numOfEnemies; i++)
	{
		if (EnemiesPos[i].id < ENEMY_ID_START || EnemiesPos[i].id > ENEMY_ID_END)
		{
			continue;
		}

		ctx->enemies[i].id = EnemiesPos[i].id;
		ctx->enemies[i].currMapPos.x = EnemiesPos[i].x;
		ctx->enemies[i].currMapPos.y = EnemiesPos[i].y;
		ctx->enemies[i].prevMapPos = ctx->enemies[i].currMapPos;

		switch (EnemiesPos[i].id)
		{
		case ENEMY_GOOMBA_ID: {
			ctx->enemies[i].asset = &GOOMBA_ASSET;
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

		ctx->player.subpixelX += (ctx->player.vx * 16) / 60; // 16 pixels per second
	}

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
	if (ctx->player.currMapPos.x < levelBounds->p2.x)
	{
		if (ctx->player.subpixelX >= 16)
		{
			ctx->player.subpixelX -= 16;
			ctx->player.currMapPos.x++;
		}
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
		int floorYLevel = ctx->bgRepObjects[ctx->floorIndex].asset->baseAsset.sprite.size.y * ctx->bgRepObjects[ctx->floorIndex].mulVector.y;
		//sanity check
		if (floorYLevel < 0 || floorYLevel > 200) { return -10; }

		Rect_t mapRect;
		mapRect.p1.x = i * 20;
		mapRect.p1.y = floorYLevel;
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
	}

	return 0;

//	Map_RenderFloor(p);
//
//	for (int i = 0; i < LCD_WIDTH/20; i++)
//	{
//		Rect_t mapRect;
//		mapRect.p1.x = i * 20;
//		mapRect.p1.y = p->floorYLevel;
//		mapRect.p2.x = i * 20 + 20;
//		mapRect.p2.y = LCD_HEIGHT;
//
//		Rect_t screenRect = mapRect;
//
//		int baseRectArea = CalcRectArea(screenRect);
//		int ret = RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);
//
//		int elementSize = sizeof(SpritePos_t);
//		if (elementSize > 0)
//		{
//			int numOfSprites = sizeof(MapSpriteLoc)/elementSize;
//			for (int j = 0; j < numOfSprites; j++)
//			{
//				SpritePos_t spritePos = MapSpriteLoc[j];
//				switch (spritePos.spriteID)
//				{
//				case JEDYNKA_SPRITE_ID:
//				{
//					Map_RenderBgdSprite(&p->jedynkaSprite, spritePos, mapRect, screenRect, p->LCDOffsetX, false, false);
//					break;
//				}
//				case DWOJKA_SPRITE_ID:
//				{
//					Map_RenderBgdSprite(&p->dwojkaSprite, spritePos, mapRect, screenRect, p->LCDOffsetX, false, false);
//					break;
//				}
//				case CHMURKA_SPRITE_ID:
//				{
//					Map_RenderBgdSprite(&p->chmurkaSprite, spritePos, mapRect, screenRect, p->LCDOffsetX, false, false);
//					break;
//				}
//				}
//			}
//
//		}
//
//		RE_SendRect(screenRect, p->LCDOffsetX);
//	}
//
//	return 0;
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

int REDNER_ScrollRender(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }

	Point_t cameraDiff = {0};
	cameraDiff.x = ctx->map.currCameraPos.x - p->prevCameraPos.x;

	if (cameraDiff.x > 0)
	{
		int prevLCDOffsetX = p->LCDOffsetX;

	p->LCDOffsetX -= cameraDiff.x;
		if (p->LCDOffsetX > 319)
		{
			p->LCDOffsetX = p->LCDOffsetX - 320;
		}
		if (p->LCDOffsetX < 0)
		{
			p->LCDOffsetX = 320 + p->LCDOffsetX;
		}

//		Rect_t leftMapRect;
		Rect_t rightMapRect;
		Rect_t leftScreenRect;
		Rect_t rightScreenRect;

//		leftMapRect.p1.x = p->prevCameraPos.x;
//		leftMapRect.p1.y = p->floorYLevel;
//		leftMapRect.p2.x = p->currCameraPos.x;
//		leftMapRect.p2.y = LCD_HEIGHT;

		rightMapRect.p1.x = p->prevCameraPos.x + LCD_WIDTH - 1;
		rightMapRect.p1.y = p->floorYLevel;
		rightMapRect.p2.x = p->currCameraPos.x + LCD_WIDTH - 1;
		rightMapRect.p2.y = LCD_HEIGHT;

		leftScreenRect.p1.x = 0;
		leftScreenRect.p1.y = p->floorYLevel;
		leftScreenRect.p2.x = p->cameraDiff.x;
		leftScreenRect.p2.y = LCD_HEIGHT;

		rightScreenRect.p1.x = LCD_WIDTH - p->cameraDiff.x - 1;
		rightScreenRect.p1.y = p->floorYLevel;
		rightScreenRect.p2.x = LCD_WIDTH - 1;
		rightScreenRect.p2.y = LCD_HEIGHT;

		int baseRectArea, ret;

		// LEFT
		baseRectArea = CalcRectArea(leftScreenRect);
		ret = RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);
		RE_SendRect(leftScreenRect, prevLCDOffsetX);

		// SHIFT SCROLL
		LCD_WriteVertScrollStartAddr(p->LCDOffsetX);

		// RIGHT
		baseRectArea = CalcRectArea(rightScreenRect);
		ret = RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

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
					Map_RenderBgdSprite(&p->jedynkaSprite, spritePos, rightMapRect, rightScreenRect, p->LCDOffsetX, false, false);
					break;
				}
				case DWOJKA_SPRITE_ID:
				{
					Map_RenderBgdSprite(&p->dwojkaSprite, spritePos, rightMapRect, rightScreenRect, p->LCDOffsetX, false, false);
					break;
				}
				case CHMURKA_SPRITE_ID:
				{
					Map_RenderBgdSprite(&p->chmurkaSprite, spritePos, rightMapRect, rightScreenRect, p->LCDOffsetX, false, false);
					break;
				}
				}
			}
		}

		RE_SendRect(rightScreenRect, p->LCDOffsetX);
	}

	return 0;
}
