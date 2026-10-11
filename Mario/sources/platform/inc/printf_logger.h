/*
 * printf_logger.h
 *
 *  Created on: 23 kwi 2026
 *      Author: tomas
 */

#ifndef PRINTF_LOGGER_H_
#define PRINTF_LOGGER_H_

#include <stdint.h>
#include <stdarg.h>

void printf_init(void);

void printf_v(const char *fmt, ...);

void printf_c(char c);
void printf_str(const char *s);
void printf_int(int value);
void printf_uint(uint32_t value);

#define LOG(msg) do { printf_str(msg); printf_str("\n"); } while(0)

#endif /* PRINTF_LOGGER_H_ */
