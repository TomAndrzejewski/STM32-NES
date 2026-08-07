/*
 * Defines.h
 *
 *  Created on: 7 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NES_DEFS_H_
#define SOURCES_INC_NES_DEFS_H_

#include "stdint.h"

#define TESTY_MUZYKI

#ifndef NDEBUG
    #define OPTIMIZE_FOR_DEBUG __attribute__((optimize("Og")))
#else
    #define OPTIMIZE_FOR_DEBUG // W Release makro jest puste i znika
#endif

#define SYSCLOCK_16MHZ		(16000000UL)
#define SYSCLOCK_144MHZ		(144000000UL)

#define SYSCLOCK_MHZ		SYSCLOCK_144MHZ


#define LCD_HEIGHT		(240)
#define LCD_WIDTH		(320)
#define LCD_SIZE		(LCD_HEIGHT * LCD_WIDTH)


#define LCD_TRANSPARENT_COLOR		(0xFFFF)


#define MODIFY_BITS(var, mask, val) ((var) = ((var) & ~(mask)) | ((val) & (mask)))

#define SET_FIELD_32(var, mask, shift, val) do { \
    uint32_t temp = (var);                    \
    temp &= ~(mask);                          \
    temp |= (((val) << (shift)) & (mask));    \
    (var) = temp;                             \
} while(0)


#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))


#endif /* SOURCES_INC_NES_DEFS_H_ */
