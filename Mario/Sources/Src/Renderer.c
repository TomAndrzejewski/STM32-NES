/*
 * Renderer.c
 *
 *  Created on: 3 paz 2026
 *      Author: tomasz
 */

#include "Renderer.h"

#include <string.h>

#include "Game_Types.h"
#include "NES_Functions.h"
#include "LCDControl.h"
#include "RenderEngine.h"

#include "Game.h"

#include "printf_logger.h"


int RENDERER_Update(GameContext_t* ctx)
{
	if (ctx == NULL) { return -1; }
	int ret = 0;

	ret = RENDERER_ScrollRender(&ctx->renderer, ctx);
	if (ret < 0) { return -1; }

	ret = RENDERER_DirtyRects_Calculate(&ctx->renderer, ctx);
	if (ret < 0) { return -1; }

	ret = RENDERER_DirtyRects_Render(&ctx->renderer, ctx);
	if (ret < 0) { return -1; }

	return 0;
}

int RENDERER_FirstRender(const GameContext_t* ctx)
{
	if (ctx == NULL)	{ return -1; }
	if (ctx->floorIndex < 0) { return -5; }

	LCD_WriteVertScrollStartAddr(ctx->renderer.LCDOffsetX);

	//sanity check
	if (ctx->map.floorYLevel < 0 || ctx->map.floorYLevel > 200) { return -10; }

	RENDERER_RenderFloor(&ctx->bgRepObjects[ctx->floorIndex]);

	for (int i = 0; i < LCD_WIDTH/16; i++)
	{
		Rect_t mapRect;
		mapRect.p1.x = i * 16;
		mapRect.p1.y = ctx->map.floorYLevel;
		mapRect.p2.x = i * 16 + 16;
		mapRect.p2.y = LCD_HEIGHT;

		Rect_t screenRect = mapRect;

		int baseRectArea = CalcRectArea(screenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			int indexLUT = ctx->bgObjectsLUT[j];
			if (!ctx->IsBGObjectActive[indexLUT]) {
				continue;
			}
			RENDERER_RenderBGObject(&ctx->bgObjects[indexLUT], &mapRect, &screenRect, ctx->renderer.LCDOffsetX);
		}

		RE_SendRect(screenRect, ctx->renderer.LCDOffsetX);
	}

	return 0;
}

int RENDERER_ScrollRender(RendererState_t* renderer, const GameContext_t* ctx)
{
	if (renderer == NULL || ctx == NULL) { return -1; }

	Point_t cameraDiff = {0};
	cameraDiff.x = ctx->camera.currPos.x - ctx->camera.prevPos.x;

	if (cameraDiff.x > 0)
	{
		int prevLCDOffsetX = renderer->LCDOffsetX;

		renderer->LCDOffsetX -= cameraDiff.x;
		// todotomka da sie ladniej, czytelniej robic sprawdzanie zakresow?
		if (renderer->LCDOffsetX > 319)
		{
			renderer->LCDOffsetX = renderer->LCDOffsetX - 320;
		}
		if (renderer->LCDOffsetX < 0)
		{
			renderer->LCDOffsetX = 320 + renderer->LCDOffsetX;
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
		LCD_WriteVertScrollStartAddr(renderer->LCDOffsetX);

		// RIGHT
		baseRectArea = CalcRectArea(rightScreenRect);
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		// uint32_t t1 = GetTimestamp();
		for (int i = 0; i < ctx->activebgObjects; i++)
		{
			int indexLUT = ctx->bgObjectsLUT[i];
			if (!ctx->IsBGObjectActive[indexLUT]) {
				continue;
			}
			RENDERER_RenderBGObject(&ctx->bgObjects[indexLUT], &rightMapRect, &rightScreenRect, renderer->LCDOffsetX);
		}
		// uint32_t tdiff = CalcTimeUS(t1);
		// printf_v("tdiff BG: %d\n", tdiff);

		// t1 = GetTimestamp();
		for (int i = 0; i < ctx->activefgObjects; i++)
		{
			int indexLUT = ctx->fgObjectsLUT[i];

			if (!ctx->IsFGObjectActive[indexLUT]) {
				continue;
			}

			const ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];
			if (!obj->currFlags.IsAlive) { continue; }
			if (!(obj->assetFlags & FG_SCROLL_RENDER)) { continue; }
			if (obj->assetFlags & HIDDEN) { continue; }

			RENDERER_RenderFGObject(obj, &rightMapRect, &rightScreenRect, renderer->LCDOffsetX);
		}
		// tdiff = CalcTimeUS(t1);
		// printf_v("tdiff FG: %d\n\n", tdiff);

		RE_SendRect(rightScreenRect, renderer->LCDOffsetX);
	}

	return 0;
}

int	RENDERER_DirtyRects_Calculate(RendererState_t* renderer, const GameContext_t* ctx)
{
	if (renderer == NULL || ctx == NULL) { return -1; }

	renderer->activeDirtyRects = 0;
	DirtyRect_t* dirtyRects = renderer->dirtyRects;

	fast_memset(dirtyRects, 0, sizeof(renderer->dirtyRects));

	Rect_t cameraRect;
	cameraRect.p1.x = ctx->camera.currPos.x;
	cameraRect.p1.y = ctx->camera.currPos.y;
	cameraRect.p2.x = cameraRect.p1.x + LCD_WIDTH;
	cameraRect.p2.y = cameraRect.p1.y + LCD_HEIGHT;

	//////////////////////////
	// FOREGROUND OBJECTS DIRTY RECTS
	//////////////////////////
	for (int i = 0; i < ctx->activefgObjects; i++)
	{
		int indexLUT = ctx->fgObjectsLUT[i];
		if (!ctx->IsFGObjectActive[indexLUT]) {
			continue;
		}
		const ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];

		// Code below is very bad, fix pls :(
		// if ((obj->assetFlags & DESTROYABLE) || (obj->assetFlags & HIDDEN) | (obj->assetFlags & BUMPABLE))
		// {
		// 	*((uint32_t*)&obj->assetFlags) &= ~FG_SCROLL_RENDER;
		// }

		if (obj->assetFlags & HIDDEN) {
			continue;
		}

		if (obj->assetFlags & FG_SCROLL_RENDER) {
			continue;
		}

		// Code below is very bad, fix pls :(
		// if (obj->assetFlags & FG_SCROLL_RENDER) { // OBJECT WILL BE RENDERED IN SCROLL RENDER
		// 	if (!obj->currFlags.clearRenderedSprite) { // FCK IT, I WANT IT IN DIRTY RECTS ANYWAY!
		// 		*((uint32_t*)&obj->assetFlags) &= ~FG_SCROLL_RENDER; // BULLSHIT! DO IT OTHER WAY!
		// 		continue;
		// 	}
		// }

		Rect_t dirtyRect;
		if (FGOBJECTS_GetDirtyRect(obj, &dirtyRect) < 0) { continue; }

		Rect_t commonRect = {0};
		Rect_GetIntersection(&cameraRect, &dirtyRect, &commonRect);

		if (Rect_IsIntersection(&commonRect))
		{
			if (renderer->activeDirtyRects >= DIRTY_RECTS_SIZE) {
				printf_str("\n### activeDirtyRects exeeds DIRTY_RECTS_SIZE ###\n");
			} else {
				dirtyRects[renderer->activeDirtyRects].rect = commonRect;
				renderer->activeDirtyRects++;
			}
		}
	}

	//////////////////////////
	// ENEMIES DIRTY RECTS
	//////////////////////////
	for (int i = 0; i < ctx->enemies.activeEnemies; i++)
	{
		int indexLUT = ctx->enemies.enemiesLUT[i];

		if (!ctx->enemies.IsEnemyActive[indexLUT]) {
			continue;
		}

		Rect_t dirtyRect;
		if (ENEMIES_GetDirtyRect(&ctx->enemies.pool[indexLUT], &dirtyRect) < 0) { continue; }

		Rect_t commonRect = {0};
		Rect_GetIntersection(&cameraRect, &dirtyRect, &commonRect);

		if (Rect_IsIntersection(&commonRect))
		{
			if (renderer->activeDirtyRects >= DIRTY_RECTS_SIZE) {
				printf_str("\n### activeDirtyRects exeeds DIRTY_RECTS_SIZE ###\n");
			} else {
				dirtyRects[renderer->activeDirtyRects].rect = commonRect;
				renderer->activeDirtyRects++;
			}
		}
	}

	//////////////////////////
	// PLAYER DIRTY RECT
	//////////////////////////
	Rect_t dirtyRect;
	if (!PLAYER_GetDirtyRect(&ctx->player, &dirtyRect))
	{
		Rect_t commonRect = {0};
		Rect_GetIntersection(&cameraRect, &dirtyRect, &commonRect);

		if (Rect_IsIntersection(&commonRect))
		{
			if (renderer->activeDirtyRects >= DIRTY_RECTS_SIZE) {
				printf_str("\n### activeDirtyRects exeeds DIRTY_RECTS_SIZE ###\n");
			} else {
				dirtyRects[renderer->activeDirtyRects].rect = commonRect;
				renderer->activeDirtyRects++;
			}
		}
	}


	//////////////////////////
	// COMPOUNDING OVERLAPPING DIRTY RECTS
	//////////////////////////
	// printf_str("\nPrzed:\n");
	// for (int i = 0; i < renderer->activeDirtyRects; i++)
	// {
	// 	printf_v("x1: %d, y1: %d, x2: %d, y2: %d\n", dirtyRects[i].rect.p1.x, dirtyRects[i].rect.p1.y, dirtyRects[i].rect.p2.x, dirtyRects[i].rect.p2.y);
	// }

	for (int i = 0; i < renderer->activeDirtyRects; i++)
	{
		if (dirtyRects[i].used)	{ continue; }

		bool commonRectFound = false;
		for (int j = 0; j < renderer->activeDirtyRects; j++)
		{
			if (i == j)	{ continue; }
			if (dirtyRects[j].used)	{ continue; }

			Rect_t commonRect = {0};
			Rect_GetIntersection(&dirtyRects[i].rect, &dirtyRects[j].rect, &commonRect);

			if (Rect_IsIntersection(&commonRect))
			{
				Rect_t commonORRect;
				commonORRect.p1.x = min(dirtyRects[i].rect.p1.x, dirtyRects[j].rect.p1.x);
				commonORRect.p1.y = min(dirtyRects[i].rect.p1.y, dirtyRects[j].rect.p1.y);
				commonORRect.p2.x = max(dirtyRects[i].rect.p2.x, dirtyRects[j].rect.p2.x);
				commonORRect.p2.y = max(dirtyRects[i].rect.p2.y, dirtyRects[j].rect.p2.y);

				commonRectFound = true;
				dirtyRects[j].used = true;
				dirtyRects[i].rect = commonORRect;
				break;
			}
		}

		if (commonRectFound)
		{
			i--;
		}
	}

	// printf_str("Po:\n");
	// for (int i = 0; i < renderer->activeDirtyRects; i++)
	// {
	// 	if (dirtyRects[i].used)	{ continue; }
	// 	printf_v("x1: %d, y1: %d, x2: %d, y2: %d\n", dirtyRects[i].rect.p1.x, dirtyRects[i].rect.p1.y, dirtyRects[i].rect.p2.x, dirtyRects[i].rect.p2.y);
	// }

	return 0;
}

int	RENDERER_DirtyRects_Render(RendererState_t* renderer, const GameContext_t* ctx)
{
	if (renderer == NULL || ctx == NULL) { return -1; }

	DirtyRect_t* dirtyRects = renderer->dirtyRects;

	for (int i = 0; i < renderer->activeDirtyRects; i++)
	{
		if (dirtyRects[i].used)	{ continue; }

		DirtyRect_t* dirtyRect = &dirtyRects[i];

		Rect_t screenRect;
		screenRect.p1.x = dirtyRect->rect.p1.x - ctx->camera.currPos.x;
		screenRect.p1.y = dirtyRect->rect.p1.y - ctx->camera.currPos.y;
		screenRect.p2.x = dirtyRect->rect.p2.x - ctx->camera.currPos.x;
		screenRect.p2.y = dirtyRect->rect.p2.y - ctx->camera.currPos.y;

		//todotomka dodac dzielenie na kilka gdy za duży na jeden framebuffer
		//todotomka scroll_render nie powiniene dzialac przy pierwszym renderze


		int baseRectArea = CalcRectArea(dirtyRect->rect);
		if (baseRectArea > FRAMEBUFFER_NUMOF_PIXELS) { // validate if framebuffer is too large
			continue;
		}

		//-----------------------
		// BACKGROUND COLOR
		//-----------------------
		RE_FillBackgroud(LCD_COLOR_BLUESKY, baseRectArea);

		//-----------------------
		// BACKGROUND OBJECTS
		//-----------------------
		for (int j = 0; j < ctx->activebgObjects; j++)
		{
			int indexLUT = ctx->bgObjectsLUT[j];
			if (!ctx->IsBGObjectActive[indexLUT]) {
				continue;
			}
			RENDERER_RenderBGObject(&ctx->bgObjects[indexLUT], &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
		}

		//-----------------------
		// FOREGROUND OBJECTS
		//-----------------------
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < ctx->activefgObjects; k++)
			{
				int indexLUT = ctx->fgObjectsLUT[k];
				if (!ctx->IsFGObjectActive[indexLUT]) {
					continue;
				}
				const ForegroundObject_t* obj = &ctx->fgObjects[indexLUT];
				if (!obj->currFlags.IsAlive) { continue; }
				if (obj->assetFlags & HIDDEN) { continue; }

				// 2 priority levels in rendering
				// Objects renderded in second iteration are visible as "behind" these rendered in first iteration
				if (j == 0 && !obj->currFlags.renderInBackground) {
					continue;
				}
				if (j == 1 && obj->currFlags.renderInBackground) {
					continue;
				}

				RENDERER_RenderFGObject(obj, &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
			}
		}
		

		//-----------------------
		// ENEMIES
		//-----------------------
		for (int j = 0; j < ctx->enemies.activeEnemies; j++)
		{
			int indexLUT = ctx->enemies.enemiesLUT[j];	
			if (!ctx->enemies.IsEnemyActive[indexLUT]) {
				continue;
			}
			const EnemyState_t* enemy = &ctx->enemies.pool[indexLUT];

			RENDERER_RenderEnemy(enemy, &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
		}

		//-----------------------
		// PLAYER
		//-----------------------
		RENDERER_RenderPlayer(&ctx->player, &dirtyRect->rect, &screenRect, renderer->LCDOffsetX);
			
		
		RE_SendRect(screenRect, renderer->LCDOffsetX);
	}

	return 0;
}

int RENDERER_RenderFloor(const BackgroundRepObject_t* floor)
{
	if (floor == NULL)	{ return -1; }

	const BaseAsset_t* baseAsset = &floor->asset->baseAsset;
	SpriteRender_t renderContext = {0};

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
			renderContext.mirrorX = false;
			renderContext.activeColorSwap = 0;

			RE_RenderSprite(&baseAsset->sprite, renderContext, false);
		}
	}

	return 0;
}

int RENDERER_RenderBGObject(const BackgroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (obj == NULL || mapRectToDraw == NULL || screenRect == NULL)	{ return -1; }

	Rect_t posRect;
	posRect.p1.x = obj->mapPos.x;
	posRect.p1.y = obj->mapPos.y;
	posRect.p2.x = obj->mapPos.x + obj->asset->baseAsset.sprite.size.x;
	posRect.p2.y = obj->mapPos.y + obj->asset->baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = posRect.p1.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = posRect.p1.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = (obj->assetFlags & MIRROR_X) ? true : false;
		renderContext.activeColorSwap = 0;

		RE_FillSprite(&obj->asset->baseAsset.sprite, &renderContext);
	}

	return 0;
}

int	RENDERER_RenderFGObject(const ForegroundObject_t* obj, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (obj == NULL || mapRectToDraw == NULL || screenRect == NULL) { return -1; }

	Rect_t posRect;
	posRect.p1.x = obj->currMapPos.x;
	posRect.p1.y = obj->currMapPos.y;
	posRect.p2.x = obj->currMapPos.x + obj->asset.baseAsset.sprite.size.x;
	posRect.p2.y = obj->currMapPos.y + obj->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = obj->currMapPos.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = obj->currMapPos.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = (obj->assetFlags & MIRROR_X) ? true : false;;
		renderContext.activeColorSwap = 0;

		RE_FillSprite(&obj->asset.baseAsset.sprite, &renderContext);
	}

	return 0;
}

int	RENDERER_RenderEnemy(const EnemyState_t* enemy, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (enemy == NULL || mapRectToDraw == NULL || screenRect == NULL) { return -1; }

	Rect_t posRect;
	posRect.p1.x = enemy->currMapPos.x;
	posRect.p1.y = enemy->currMapPos.y;
	posRect.p2.x = enemy->currMapPos.x + enemy->asset.baseAsset.sprite.size.x;
	posRect.p2.y = enemy->currMapPos.y + enemy->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = enemy->currMapPos.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = enemy->currMapPos.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = false;
		renderContext.activeColorSwap = 0;

		RE_FillSprite(&enemy->asset.baseAsset.sprite, &renderContext);
	}

	return 0;
}

int	RENDERER_RenderPlayer(const PlayerState_t* player, const Rect_t* mapRectToDraw, const Rect_t* screenRect, const int LCDOffsetX)
{
	if (player == NULL || mapRectToDraw == NULL || screenRect == NULL) { return -1; }

	Rect_t posRect;
	posRect.p1.x = player->currMapPos.x;
	posRect.p1.y = player->currMapPos.y;
	posRect.p2.x = player->currMapPos.x + player->asset.baseAsset.sprite.size.x;
	posRect.p2.y = player->currMapPos.y + player->asset.baseAsset.sprite.size.y;

	Rect_t commonRect = {0};
	Rect_GetIntersection(mapRectToDraw, &posRect, &commonRect);

	if (Rect_IsIntersection(&commonRect))
	{
		SpriteRender_t renderContext = {0};
		renderContext.commonRect = commonRect;
		renderContext.baseRect = *screenRect;
		renderContext.baseToSpriteOffset.x = player->currMapPos.x - mapRectToDraw->p1.x;
		renderContext.baseToSpriteOffset.y = player->currMapPos.y - mapRectToDraw->p1.y;
		renderContext.LCDOffsetX = LCDOffsetX;
		renderContext.mirrorX = player->currPhysicsFlags.lastMovementDirectionRight ? false : true;
		renderContext.activeColorSwap = 0;
		// renderContext.activeColorSwap = 4;
		// renderContext.colorSwap[0][0] = 0x00f8;
		// renderContext.colorSwap[0][1] = 0xe0fd;
		// renderContext.colorSwap[1][0] = 0x00f9;
		// renderContext.colorSwap[1][1] = 0xe0fe;
		// renderContext.colorSwap[2][0] = 0x00fa;
		// renderContext.colorSwap[2][1] = 0xe0fe;
		// renderContext.colorSwap[3][0] = 0x00fb;
		// renderContext.colorSwap[3][1] = 0xe0ff;

		// uint32_t t1 = GetTimestamp();
		RE_FillSprite(&player->asset.baseAsset.sprite, &renderContext);
		// uint32_t tdiff = CalcTimeUS(t1);
		// printf_int(tdiff); printf_c('\n');
	}

	return 0;
}