#ifndef __zprint_h__
#define __zprint_h__

#include <stdarg.h>
#ifdef __AVR__
# include <avr/pgmspace.h>
#else
# define prog_char char
# define pgm_read_byte(p) (*(p))
#endif

/*
 Note: #define or #undef following configuration macros in implementation file

#undef CONFIG_ZPUTC_REP
#undef CONFIG_ZPUTS_REV
#undef CONFIG_ZPUT_DEC
#undef CONFIG_ZPUT_UDEC
#undef CONFIG_ZPUT_HEX
#undef CONFIG_ZPRINTF
*/

/* character and string */
/* void zputc(char c); */
/* void zputs(const char *s); */
void zputc_rep(char ch, char n);
void zputs_rev(const char *str, char n);

/* integer */
void zput_dec(int x);
void zput_udec(unsigned x);
void zput_hex(unsigned x);

/* formatting */
void zvprintf(const char *fmt, va_list ap);
void zprintf(const char* fmt, ...);

#endif /* __zprint_h__ */
