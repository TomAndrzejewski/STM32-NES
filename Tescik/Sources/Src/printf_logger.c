/*
 * printf_logger.c
 *
 *  Created on: 23 kwi 2026
 *      Author: tomas
 */

#include "stdint.h"

#include "stm32f446xx.h"

#include "printf_logger.h"

#include "NES_Defs.h"


void printf_init(void)
{
	DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN;

	RCC->APB2ENR |= (1 << 0);

	ITM->TCR = 0;

    ITM->LAR = 0xC5ACCE55;

    TPIU->SPPR = 2; // NRZ

#if SYSCLOCK_MHZ == SYSCLOCK_144MHZ
    TPIU->ACPR = (144000000 / 2000000) - 1; // 2 MHz
#else
    TPIU->ACPR = (16000000 / 1000000) - 1; // 2 MHz
#endif

    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

	ITM->TCR = 0;

    ITM->TCR |= ITM_TCR_ITMENA_Msk;

    ITM->TER = 1;
}

void printf_c(char c)
{
#ifdef DEBUG
    if (!(ITM->TCR & ITM_TCR_ITMENA_Msk)) return;
    if (!(ITM->TER & 1)) return;

    while (!(ITM->PORT[0].u32 & 1));
    ITM->PORT[0].u8 = c;
#endif
}

static void printf_hex(uint32_t val)
{
#ifdef DEBUG
    char hex[] = "0123456789ABCDEF";
    for (int i = 7; i >= 0; i--)
    {
    	printf_c(hex[(val >> (i * 4)) & 0xF]);
    }
#endif
}

void printf_v(const char *fmt, ...)
{
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);

    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;

            switch (*fmt)
            {
                case 'd':
                {
                    int v = va_arg(args, int);
                    printf_int(v);
                    break;
                }

                case 'x':
                {
                    uint32_t v = va_arg(args, uint32_t);
                    printf_hex(v);
                    break;
                }

                case 's':
                {
                    char *s = va_arg(args, char*);
                    printf_s(s ? s : "(null)");
                    break;
                }

                case '%':
                {
                	printf_c('%');
                    break;
                }

                default:
                    // nieznany format → wypisz literalnie
                	printf_c('%');
                	printf_c(*fmt);
                    break;
            }
        }
        else
        {
        	printf_c(*fmt);
        }

        fmt++;
    }

    va_end(args);
#endif
}

void printf_s(const char *s)
{
#ifdef DEBUG
    while (*s)
    {
    	printf_c(*s++);
    }
#endif
}

void printf_uint(uint32_t value)
{
#ifdef DEBUG
    char buf[10];
    int i = 0;

    if (value == 0)
    {
    	printf_c('0');
        return;
    }

    while (value > 0)
    {
        buf[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i--)
    {
    	printf_c(buf[i]);
    }
#endif
}

void printf_int(int value)
{
#ifdef DEBUG
    if (value < 0)
    {
    	printf_c('-');
        value = -value;
    }
    printf_uint((uint32_t)value);
#endif
}
