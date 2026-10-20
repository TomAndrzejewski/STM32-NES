/*
 * Level.c
 *
 *  Created on: 1 lip 2026
 *      Author: tomasz
 */

#include <string.h>

#include "Game_Types.h"
#include "LCDControl.h"
#include "Sound.h"

#include "Level.h"


const Rect_t LevelBoundaries = {
		.p1.x = 0,
		.p1.y = 0,
		.p2.x = 6400,
		.p2.y = 240
};

const ObjectLevelInstance_t Objects_level_1[] =
{
    {BG_HILL_1_OBJECT_ID, 0, 32, 0x00000020},
    {BG_HILL_1_OBJECT_ID, 16, 48, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 16, 32, 0x00000000},
    {BG_HILL_0_OBJECT_ID, 32, 32, 0x00000000},
    {BG_HILL_2_OBJECT_ID, 32, 48, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 32, 64, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 48, 48, 0x00000000},
    {BG_HILL_3_OBJECT_ID, 48, 32, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 64, 32, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 128, 160, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 200, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 216, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 232, 32, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 272, 80, 0x0000050F},
    {BG_HILL_1_OBJECT_ID, 272, 32, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 288, 32, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 288, 48, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 304, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 336, 80, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 336, 176, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 352, 80, 0x0000190F},
    {FG_BRICKS_OBJECT_ID, 368, 80, 0x0000031F},
    {FG_BLOCK_QMARK_OBJECT_ID, 368, 144, 0x00001D0F},
    {ENEMY_GOOMBA_ID, 368, 32, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 384, 80, 0x00001D0F},
    {BG_KRZAK_OBJECT_ID, 392, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 400, 80, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 448, 160, 0x00000000},
    {FG_RURA_DOL_OBJECT_ID, 464, 32, 0x0000001C},
    {FG_RURA_GORA_OBJECT_ID, 464, 48, 0x0000001D},
    {BG_CHMURKA_OBJECT_ID, 464, 160, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 480, 160, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 608, 176, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 624, 176, 0x00000000},
    {FG_RURA_DOL_OBJECT_ID, 640, 32, 0x0000001C},
    {FG_RURA_DOL_OBJECT_ID, 640, 48, 0x0000001C},
    {FG_RURA_GORA_OBJECT_ID, 640, 64, 0x0000001D},
    {ENEMY_GOOMBA_ID, 672, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 696, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 712, 32, 0x00000000},
    {FG_RURA_DOL_OBJECT_ID, 768, 32, 0x0000001C},
    {FG_RURA_DOL_OBJECT_ID, 768, 48, 0x0000001C},
    {FG_RURA_DOL_OBJECT_ID, 768, 64, 0x0000001C},
    {FG_RURA_GORA_OBJECT_ID, 768, 80, 0x0000001D},
    {BG_HILL_1_OBJECT_ID, 800, 32, 0x00000020},
    {BG_HILL_1_OBJECT_ID, 816, 48, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 816, 32, 0x00000000},
    {BG_HILL_0_OBJECT_ID, 832, 32, 0x00000000},
    {BG_HILL_2_OBJECT_ID, 832, 48, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 832, 64, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 848, 48, 0x00000000},
    {BG_HILL_3_OBJECT_ID, 848, 32, 0x00000000},
    {ENEMY_GOOMBA_ID, 848, 32, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 864, 32, 0x00000000},
    {ENEMY_GOOMBA_ID, 864, 32, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 928, 160, 0x00000000},
    {FG_RURA_DOL_OBJECT_ID, 944, 32, 0x0000001C},
    {FG_RURA_DOL_OBJECT_ID, 944, 48, 0x0000001C},
    {FG_RURA_DOL_OBJECT_ID, 944, 64, 0x0000001C},
    {FG_RURA_GORA_OBJECT_ID, 944, 80, 0x0000001D},
    {BG_KRZAK_OBJECT_ID, 984, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 1000, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 1016, 32, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 1056, 32, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 1072, 32, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 1072, 48, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 1088, 32, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 1104, 176, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 1176, 32, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 1232, 160, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 1248, 160, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1264, 80, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 1264, 160, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 1280, 80, 0x0000190F},
    {FG_BRICKS_OBJECT_ID, 1296, 80, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 1312, 144, 0x0000031F},
    {ENEMY_GOOMBA_ID, 1312, 160, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1328, 144, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 1344, 144, 0x0000031F},
    {ENEMY_GOOMBA_ID, 1344, 160, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1360, 144, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 1376, 144, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 1376, 176, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1392, 144, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 1392, 176, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1408, 144, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 1424, 144, 0x0000031F},
    {BG_KRZAK_OBJECT_ID, 1464, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 1480, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1488, 144, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 1504, 144, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 1520, 144, 0x0000031F},
    {FG_BLOCK_QMARK_OBJECT_ID, 1536, 144, 0x00001D0F},
    {FG_BRICKS_OBJECT_ID, 1536, 80, 0x00001F1F},
    {BG_HILL_1_OBJECT_ID, 1568, 32, 0x00000020},
    {BG_HILL_1_OBJECT_ID, 1584, 48, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 1584, 32, 0x00000000},
    {ENEMY_GOOMBA_ID, 1584, 32, 0x00000000},
    {BG_HILL_0_OBJECT_ID, 1600, 32, 0x00000000},
    {BG_HILL_2_OBJECT_ID, 1600, 48, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 1600, 64, 0x00000000},
    {ENEMY_GOOMBA_ID, 1608, 32, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 1616, 48, 0x00000000},
    {BG_HILL_3_OBJECT_ID, 1616, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1632, 80, 0x0000031F},
    {BG_HILL_1_OBJECT_ID, 1632, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1648, 80, 0x0000131F},
    {BG_CHMURKA_OBJECT_ID, 1712, 160, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 1728, 80, 0x00001D0F},
    {BG_KRZAK_OBJECT_ID, 1752, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 1768, 32, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 1776, 80, 0x00001D0F},
    {FG_BLOCK_QMARK_OBJECT_ID, 1776, 144, 0x0000190F},
    {BG_KRZAK_OBJECT_ID, 1784, 32, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 1824, 80, 0x00001D0F},
    {BG_HILL_1_OBJECT_ID, 1824, 32, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 1840, 32, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 1840, 48, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 1856, 32, 0x00000000},
    {ENEMY_GOOMBA_ID, 1856, 32, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 1872, 176, 0x00000000},
    {ENEMY_GOOMBA_ID, 1880, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1920, 80, 0x0000031F},
    {BG_KRZAK_OBJECT_ID, 1944, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 1968, 144, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 1984, 144, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 2000, 144, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 2000, 160, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 2016, 160, 0x00000000},
    {ENEMY_GOOMBA_ID, 2016, 32, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 2032, 160, 0x00000000},
    {ENEMY_GOOMBA_ID, 2040, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 2080, 144, 0x0000031F},
    {ENEMY_GOOMBA_ID, 2080, 32, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 2096, 144, 0x00001D0F},
    {FG_BRICKS_OBJECT_ID, 2096, 80, 0x0000031F},
    {ENEMY_GOOMBA_ID, 2104, 32, 0x00000000},
    {FG_BLOCK_QMARK_OBJECT_ID, 2112, 144, 0x00001D0F},
    {FG_BRICKS_OBJECT_ID, 2112, 80, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 2128, 144, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 2144, 176, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 2160, 176, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2176, 32, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2192, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2192, 48, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2208, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2208, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2208, 64, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2224, 32, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2224, 48, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2224, 80, 0x0000001D},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2224, 64, 0x00000018},
    {BG_KRZAK_OBJECT_ID, 2232, 32, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 2248, 32, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2272, 48, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2272, 80, 0x0000001D},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2272, 64, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2272, 32, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2288, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2288, 64, 0x00000019},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2288, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2304, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2304, 48, 0x00000019},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2320, 32, 0x00000019},
    {BG_HILL_1_OBJECT_ID, 2336, 32, 0x00000020},
    {BG_HILL_1_OBJECT_ID, 2352, 48, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 2352, 32, 0x00000000},
    {BG_HILL_0_OBJECT_ID, 2368, 32, 0x00000000},
    {BG_HILL_2_OBJECT_ID, 2368, 48, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 2368, 64, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 2384, 48, 0x00000000},
    {BG_HILL_3_OBJECT_ID, 2384, 32, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2400, 32, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2416, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2416, 48, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2432, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2432, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2432, 64, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2448, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2448, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2448, 80, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2448, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2464, 32, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2464, 48, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2464, 80, 0x0000001D},
    {BG_CHMURKA_OBJECT_ID, 2464, 160, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2464, 64, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2512, 48, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2512, 80, 0x0000001D},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2512, 64, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2512, 32, 0x00000014},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2528, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2528, 64, 0x00000019},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2528, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2544, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2544, 48, 0x00000019},
    {BG_KRZAK_OBJECT_ID, 2552, 32, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2560, 32, 0x00000019},
    {BG_HILL_1_OBJECT_ID, 2592, 32, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 2608, 32, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 2608, 48, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 2624, 32, 0x00000000},
    {FG_RURA_DOL_OBJECT_ID, 2640, 32, 0x0000001C},
    {FG_RURA_GORA_OBJECT_ID, 2640, 48, 0x0000001D},
    {BG_CHMURKA_OBJECT_ID, 2640, 176, 0x00000000},
    {BG_KRZAK_OBJECT_ID, 2712, 32, 0x00000000},
    {FG_BRICKS_OBJECT_ID, 2720, 80, 0x0000031F},
    {FG_BRICKS_OBJECT_ID, 2736, 80, 0x0000031F},
    {FG_BLOCK_QMARK_OBJECT_ID, 2752, 80, 0x00001D0F},
    {FG_BRICKS_OBJECT_ID, 2768, 80, 0x0000031F},
    {BG_CHMURKA_OBJECT_ID, 2768, 160, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 2784, 160, 0x00000000},
    {BG_CHMURKA_OBJECT_ID, 2800, 160, 0x00000000},
    {ENEMY_GOOMBA_ID, 2816, 32, 0x00000000},
    {ENEMY_GOOMBA_ID, 2840, 32, 0x00000000},
    {FG_RURA_DOL_OBJECT_ID, 2896, 32, 0x0000001C},
    {FG_RURA_GORA_OBJECT_ID, 2896, 48, 0x0000001D},
    {BG_CHMURKA_OBJECT_ID, 2912, 176, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2928, 32, 0x00000015},
    {BG_CHMURKA_OBJECT_ID, 2928, 176, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2944, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2944, 48, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2960, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2960, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2960, 64, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2976, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2976, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2976, 80, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2976, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2992, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2992, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2992, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2992, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 2992, 96, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3008, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3008, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3008, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3008, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3008, 112, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3008, 96, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3024, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3024, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3024, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3024, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3024, 128, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3024, 96, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3024, 112, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 144, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 112, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 96, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3040, 128, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 160, 0x00000015},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 96, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 112, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 128, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3056, 144, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 32, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 48, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 64, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 80, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 176, 0x0000001D},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 96, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 112, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 128, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 144, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3072, 160, 0x00000010},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 80, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 64, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 48, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 48, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 32, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 176, 0x0000001D},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 96, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 112, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 128, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 144, 0x00000018},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3088, 160, 0x00000018},
    {BG_HILL_1_OBJECT_ID, 3136, 32, 0x00000020},
    {BG_HILL_1_OBJECT_ID, 3152, 48, 0x00000020},
    {BG_HILL_2_OBJECT_ID, 3152, 32, 0x00000000},
    {BG_HILL_0_OBJECT_ID, 3168, 32, 0x00000000},
    {BG_HILL_2_OBJECT_ID, 3168, 48, 0x00000000},
    {BG_HILL_4_OBJECT_ID, 3168, 64, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 3184, 48, 0x00000000},
    {BG_HILL_3_OBJECT_ID, 3184, 32, 0x00000000},
    {BG_HILL_1_OBJECT_ID, 3200, 32, 0x00000000},
    {FG_PYRAMID_BLOCK_OBJECT_ID, 3232, 32, 0x0000001D},
    {BG_CHMURKA_OBJECT_ID, 3280, 160, 0x00000000},
};

const RepObjectLevelPos_t BGRepObjectsLevelPosition[] = {
	    { .id = BG_REP_FLOOR_ID,		.x = 0,	.y = 0,	.mulX = LCD_WIDTH/16,	.mulY = 2 },
};

// Length of one melody slot (eighth note) and of the silence that follows every tone
#define MUSIC_SLOT_MS		(150)
#define MUSIC_GAP_MS		(25)

// Note lengths in milliseconds: a tone plus its gap fills whole slots, three triplet tones
// fill 4 slots, a rest absorbs the gap of the preceding tone
#define NOTE_MS(slots)		((slots) * MUSIC_SLOT_MS - MUSIC_GAP_MS)
#define TRIPLET_MS			((4 * MUSIC_SLOT_MS) / 3 - MUSIC_GAP_MS)
#define REST_MS(slots)		((slots) * MUSIC_SLOT_MS + MUSIC_GAP_MS)
#define NOTE_GAP_MS			(MUSIC_GAP_MS)

const NoteAsset_t SoundNotes[] = {

		// Intro
		{  659, NOTE_MS(1) },        // E5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  784, NOTE_MS(4) },        // G5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(4) },        // G4
		{    0, NOTE_GAP_MS },

		// Theme A, first time
		{  523, NOTE_MS(3) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(3) },        // G4
		{    0, NOTE_GAP_MS },
		{  330, NOTE_MS(3) },        // E4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(2) },        // B4
		{    0, NOTE_GAP_MS },
		{  466, NOTE_MS(1) },        // Bb4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  392, TRIPLET_MS },        // G4
		{    0, NOTE_GAP_MS },
		{  659, TRIPLET_MS },        // E5
		{    0, NOTE_GAP_MS },
		{  784, TRIPLET_MS },        // G5
		{    0, NOTE_GAP_MS },
		{  880, NOTE_MS(2) },        // A5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  784, NOTE_MS(2) },        // G5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(1) },        // D5
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(3) },        // B4
		{    0, NOTE_GAP_MS },

		// Theme A, second time
		{  523, NOTE_MS(3) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(3) },        // G4
		{    0, NOTE_GAP_MS },
		{  330, NOTE_MS(3) },        // E4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(2) },        // B4
		{    0, NOTE_GAP_MS },
		{  466, NOTE_MS(1) },        // Bb4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  392, TRIPLET_MS },        // G4
		{    0, NOTE_GAP_MS },
		{  659, TRIPLET_MS },        // E5
		{    0, NOTE_GAP_MS },
		{  784, TRIPLET_MS },        // G5
		{    0, NOTE_GAP_MS },
		{  880, NOTE_MS(2) },        // A5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  784, NOTE_MS(2) },        // G5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(1) },        // D5
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(3) },        // B4
		{    0, REST_MS(2) },        //      [rest]

		// Theme B, line 1
		{  784, NOTE_MS(1) },        // G5
		{    0, NOTE_GAP_MS },
		{  740, NOTE_MS(1) },        // F#5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(2) },        // D5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(1) },        // G4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(1) },        // D5
		{    0, REST_MS(2) },        //      [rest]

		// Theme B, line 2
		{  784, NOTE_MS(1) },        // G5
		{    0, NOTE_GAP_MS },
		{  740, NOTE_MS(1) },        // F#5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(2) },        // D5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{ 1047, NOTE_MS(2) },        // C6
		{    0, NOTE_GAP_MS },
		{ 1047, NOTE_MS(1) },        // C6
		{    0, NOTE_GAP_MS },
		{ 1047, NOTE_MS(4) },        // C6
		{    0, REST_MS(2) },        //      [rest]

		// Theme B, line 3
		{  784, NOTE_MS(1) },        // G5
		{    0, NOTE_GAP_MS },
		{  740, NOTE_MS(1) },        // F#5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(2) },        // D5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(1) },        // G4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(1) },        // D5
		{    0, REST_MS(2) },        //      [rest]

		// Theme B, line 4
		{  622, NOTE_MS(3) },        // D#5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(3) },        // D5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(8) },        // C5
		{    0, NOTE_GAP_MS },

		// Theme C, line 1
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(2) },        // D5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(1) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(4) },        // G4
		{    0, NOTE_GAP_MS },

		// Theme C, line 2
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(1) },        // D5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(9) },        // E5
		{    0, NOTE_GAP_MS },

		// Theme C, line 3
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(2) },        // D5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(1) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(4) },        // G4
		{    0, NOTE_GAP_MS },

		// Theme C, line 4
		{  659, NOTE_MS(1) },        // E5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  784, NOTE_MS(4) },        // G5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(4) },        // G4
		{    0, NOTE_GAP_MS },

		// Theme A, first time
		{  523, NOTE_MS(3) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(3) },        // G4
		{    0, NOTE_GAP_MS },
		{  330, NOTE_MS(3) },        // E4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(2) },        // B4
		{    0, NOTE_GAP_MS },
		{  466, NOTE_MS(1) },        // Bb4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  392, TRIPLET_MS },        // G4
		{    0, NOTE_GAP_MS },
		{  659, TRIPLET_MS },        // E5
		{    0, NOTE_GAP_MS },
		{  784, TRIPLET_MS },        // G5
		{    0, NOTE_GAP_MS },
		{  880, NOTE_MS(2) },        // A5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  784, NOTE_MS(2) },        // G5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(1) },        // D5
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(3) },        // B4
		{    0, NOTE_GAP_MS },

		// Theme A, second time
		{  523, NOTE_MS(3) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(3) },        // G4
		{    0, NOTE_GAP_MS },
		{  330, NOTE_MS(3) },        // E4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(2) },        // B4
		{    0, NOTE_GAP_MS },
		{  466, NOTE_MS(1) },        // Bb4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(2) },        // A4
		{    0, NOTE_GAP_MS },
		{  392, TRIPLET_MS },        // G4
		{    0, NOTE_GAP_MS },
		{  659, TRIPLET_MS },        // E5
		{    0, NOTE_GAP_MS },
		{  784, TRIPLET_MS },        // G5
		{    0, NOTE_GAP_MS },
		{  880, NOTE_MS(2) },        // A5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  784, NOTE_MS(2) },        // G5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(2) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  587, NOTE_MS(1) },        // D5
		{    0, NOTE_GAP_MS },
		{  494, NOTE_MS(3) },        // B4
		{    0, NOTE_GAP_MS },

		// Theme D, line 1
		{  659, NOTE_MS(1) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(3) },        // G4
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(2) },        // G4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(2) },        // F5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(4) },        // A4
		{    0, NOTE_GAP_MS },

		// Theme D, line 2
		{  494, TRIPLET_MS },        // B4
		{    0, NOTE_GAP_MS },
		{  880, TRIPLET_MS },        // A5
		{    0, NOTE_GAP_MS },
		{  880, TRIPLET_MS },        // A5
		{    0, NOTE_GAP_MS },
		{  880, TRIPLET_MS },        // A5
		{    0, NOTE_GAP_MS },
		{  784, TRIPLET_MS },        // G5
		{    0, NOTE_GAP_MS },
		{  698, TRIPLET_MS },        // F5
		{    0, NOTE_GAP_MS },
		{  659, NOTE_MS(1) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(4) },        // G4
		{    0, NOTE_GAP_MS },

		// Theme D, line 3
		{  659, NOTE_MS(1) },        // E5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(2) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(3) },        // G4
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(2) },        // G4
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(1) },        // A4
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(2) },        // F5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  440, NOTE_MS(4) },        // A4
		{    0, NOTE_GAP_MS },

		// Theme D, line 4
		{  494, NOTE_MS(1) },        // B4
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(2) },        // F5
		{    0, NOTE_GAP_MS },
		{  698, NOTE_MS(1) },        // F5
		{    0, NOTE_GAP_MS },
		{  698, TRIPLET_MS },        // F5
		{    0, NOTE_GAP_MS },
		{  659, TRIPLET_MS },        // E5
		{    0, NOTE_GAP_MS },
		{  587, TRIPLET_MS },        // D5
		{    0, NOTE_GAP_MS },
		{  523, NOTE_MS(1) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(2) },        // G4
		{    0, NOTE_GAP_MS },
		{  330, NOTE_MS(1) },        // E4
		{    0, NOTE_GAP_MS },
		{  262, NOTE_MS(4) },        // C4
		{    0, NOTE_GAP_MS },

		// Ending, line 1
		{  523, NOTE_MS(3) },        // C5
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(3) },        // G4
		{    0, NOTE_GAP_MS },
		{  330, NOTE_MS(2) },        // E4
		{    0, NOTE_GAP_MS },
		{  440, TRIPLET_MS },        // A4
		{    0, NOTE_GAP_MS },
		{  494, TRIPLET_MS },        // B4
		{    0, NOTE_GAP_MS },
		{  440, TRIPLET_MS },        // A4
		{    0, NOTE_GAP_MS },
		{  415, TRIPLET_MS },        // G#4
		{    0, NOTE_GAP_MS },
		{  466, TRIPLET_MS },        // Bb4
		{    0, NOTE_GAP_MS },
		{  415, TRIPLET_MS },        // G#4
		{    0, NOTE_GAP_MS },

		// Ending, line 2
		{  392, NOTE_MS(1) },        // G4
		{    0, NOTE_GAP_MS },
		{  370, NOTE_MS(1) },        // F#4
		{    0, NOTE_GAP_MS },
		{  392, NOTE_MS(6) },        // G4
		{    0, REST_MS(8) }         //      [rest]
};

#define SOUND_NOTES_SIZE 	(sizeof(SoundNotes) / sizeof(SoundNotes[0]))

static int s_currNoteAsset = {0};




int LEVEL_GetObjectsLocations(const ObjectLevelInstance_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(ObjectLevelInstance_t);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(Objects_level_1)/elementSize;
		*posTable = Objects_level_1;
		return 0;
	}
	else
	{
		return -5;
	}
}

int LEVEL_GetBGRepObjectsLocations(const RepObjectLevelPos_t** posTable, int* numOfObjects)
{
	if (posTable == NULL || numOfObjects == NULL)
	{
		return -1;
	}

	int elementSize = sizeof(RepObjectLevelPos_t);
	if (elementSize > 0)
	{
		*numOfObjects = sizeof(BGRepObjectsLevelPosition)/elementSize;
		*posTable = BGRepObjectsLevelPosition;
		return 0;
	}
	else
	{
		return -5;
	}
}

int LEVEL_GetLevelBoundaries(const Rect_t** levelBoundaries)
{
	if (levelBoundaries == NULL) { return -1; }
	*levelBoundaries = &LevelBoundaries;
	return 0;
}

int LEVEL_GetNextSoundNote(const NoteAsset_t** note)
{
	if (note == NULL) return -1;

	if (s_currNoteAsset >= 0 && s_currNoteAsset < (int)SOUND_NOTES_SIZE)
	{
		*note = &SoundNotes[s_currNoteAsset];
	}

	s_currNoteAsset++;
	if (s_currNoteAsset == SOUND_NOTES_SIZE) {
		s_currNoteAsset = 0;
	}

	return 0;
}




