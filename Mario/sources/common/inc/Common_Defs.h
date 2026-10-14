/*
 * Common_Defs.h
 *
 *  Created on: 7 maj 2026
 *      Author: tomasz
 */

#ifndef SOURCES_INC_COMMON_DEFS_H_
#define SOURCES_INC_COMMON_DEFS_H_


#ifndef NDEBUG
    #define OPTIMIZE_FOR_DEBUG __attribute__((optimize("Og")))
#else
    #define OPTIMIZE_FOR_DEBUG // W Release makro jest puste i znika
#endif

#define MODIFY_BITS(var, mask, val) ((var) = ((var) & ~(mask)) | ((val) & (mask)))

#define SET_FIELD_32(var, mask, shift, val) do { \
    uint32_t temp = (var);                    \
    temp &= ~(mask);                          \
    temp |= (((val) << (shift)) & (mask));    \
    (var) = temp;                             \
} while(0)


#endif /* SOURCES_INC_COMMON_DEFS_H_ */
