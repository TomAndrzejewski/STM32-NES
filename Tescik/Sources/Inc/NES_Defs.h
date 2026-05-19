/*
 * Defines.h
 *
 *  Created on: 7 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_NES_DEFS_H_
#define SOURCES_INC_NES_DEFS_H_

#include "stdint.h"

#define SYSCLOCK_16MHZ		(16000000UL)
#define SYSCLOCK_144MHZ		(144000000UL)

#define SYSCLOCK_MHZ		SYSCLOCK_144MHZ



#define MODIFY_BITS(var, mask, val) ((var) = ((var) & ~(mask)) | ((val) & (mask)))

#define SET_FIELD_32(var, mask, shift, val) do { \
    uint32_t temp = (var);                    \
    temp &= ~(mask);                          \
    temp |= (((val) << (shift)) & (mask));    \
    (var) = temp;                             \
} while(0)


#endif /* SOURCES_INC_NES_DEFS_H_ */
