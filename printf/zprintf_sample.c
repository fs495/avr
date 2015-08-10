/*
 * Sample implementation of zprintf
 */
#include "zprintf.h"
#include "../libclcd/libclcd.h"

#define CONFIG_ZPUTC_REP
#define CONFIG_ZPUTS_REV
#define CONFIG_ZPUT_DEC
#define CONFIG_ZPUT_UDEC
#define CONFIG_ZPUT_HEX
#define CONFIG_ZPRINTF

#define zputc(c) lcd_putc(c)

#include "zprintf.c"
