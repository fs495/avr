/*
 * Template library of printf-like functions
 */

#include <stdint.h>
#include "zprintf.h"

#if defined(CONFIG_ZPRINTF)
#define CONFIG_ZPUTC_REP
#endif

#if defined(CONFIG_ZPUT_DEC) || defined(CONFIG_ZPUT_UDEC) || defined(CONFIG_ZPUT_HEX)
#define CONFIG_ZPUTS_REV
#endif

#if defined(CONFIG_ZPUT_DEC)
#define CONFIG_ZPUT_UDEC
#endif

#define DECIMAL_DIGITS(bits) (((bits) * 10 + 32) / 33)

/*----------------------------------------------------------------------*/

#ifndef zputc
# error "macro 'zputc' not defined"
#endif /* zputc */

#ifndef zputs
/* define function if macro version of zputs is not provided */
void zputs(const char* str)
{
    char c;
    while((c = pgm_read_byte(str++)) != 0)
	zputc(c);
}
#endif /* zputs */

#ifdef CONFIG_ZPUTC_REP
/* note: allows n <= 0 */
void zputc_rep(char n, char ch)
{
    if(n <= 0)
	return;
    do {
	zputc(ch);
	--n;
    } while(n > 0);
}
#endif

#ifdef CONFIG_ZPUTS_REV
/* note: n should be > 0 */
void zputs_rev(const char *str, char n)
{
    const char *p = str + n;
    do {
	--p;
	--n;
	zputc(*p);
    } while(n > 0);
}
#endif

/*----------------------------------------------------------------------*/
#ifdef CONFIG_ZPUT_DEC
void zput_dec(int x)
{
    if(x < 0) {
	zputc('-');
	zput_udec(-x);
    } else {
	zput_udec(x);
    }
}
#endif

#ifdef CONFIG_ZPUT_UDEC
void zput_udec(unsigned x)
{
    char s[DECIMAL_DIGITS(sizeof(unsigned) * 8)];
    uint8_t width = 0;
    do {
	s[width++] = x % 10 + '0';
	x = x / 10;
    } while(x > 0);
    zputs_rev(s, width);
}
#endif

#ifdef CONFIG_ZPUT_HEX
void zput_hex(unsigned x)
{
    char s[2 * sizeof(unsigned)];
    uint8_t width = 0;
    do {
	char t = x % 16;
	s[width++] = (t >= 10) ? t + 'a' - 10 : t + '0';
	x = x / 16;
    } while(x > 0);
    zputs_rev(s, width);
}
#endif

/*----------------------------------------------------------------------*/

#ifdef CONFIG_ZPRINTF
void zvprintf(const char *fmt, va_list ap)
{
    char c, signc, flags;
    uint8_t minwidth, width, padwidth;
    unsigned long v;
    char s[10];

    enum {
	FLAG_LEFT_ADJ = 1,
	FLAG_ALWAYS_SIGN = 2,
	FLAG_INCLUDE_SIGN = 4,
	FLAG_ZERO_PADDING = 8,
	FLAG_NEGATIVE = 16,
	FLAG_SHORT_TYPE = 32,
	FLAG_LONG_TYPE = 64
    };

    for(;;) {
	c = pgm_read_byte(fmt++);
	if(c == 0) {
	    break;
	} else if(c != '%') {
	    zputc(c);
	    continue;
	}

	c = pgm_read_byte(fmt++);
	if(c == '%') {
	    zputc(c);
	    continue;
	}

	/* flags */
	flags = 0;
	for(;;) {
	    if(c == '-')
		flags |= FLAG_LEFT_ADJ;
	    else if(c == '+')
		flags |= FLAG_ALWAYS_SIGN;
	    else if(c == ' ')
		flags |= FLAG_INCLUDE_SIGN;
	    else if(c == '0')
		flags |= FLAG_ZERO_PADDING;
	    else
		break;
	    c = pgm_read_byte(fmt++);
	}
	if(flags & FLAG_LEFT_ADJ)
	    flags &= ~FLAG_ZERO_PADDING;

	/* minimum width */
	minwidth = 0;
	while(c >= '0' && c <= '9') {
	    minwidth = minwidth * 10 + c - '0';
	    c = pgm_read_byte(fmt++);
	}

	/* size qualifier */
	if(c == 'h') {
	    flags |= FLAG_SHORT_TYPE;
	    flags &= ~FLAG_LONG_TYPE;
	    c = pgm_read_byte(fmt++);
	} else if(c == 'l') {
	    flags |= FLAG_LONG_TYPE;
	    flags &= ~FLAG_SHORT_TYPE;
	    c = pgm_read_byte(fmt++);
	}

	/* c */
	if(c == 'c') {
	    char t = va_arg(ap, int);
	    zputc(t);
	    continue;
	}

	/* s */
	if(c == 's') {
	    char *p = va_arg(ap, char *);
	    width = 0;
	    while(p[width] != 0)
		++width;
	    padwidth = minwidth - width;
	    if(!(flags & FLAG_LEFT_ADJ))
		zputc_rep(' ', padwidth);
	    zputs(p);
	    if(flags & FLAG_LEFT_ADJ)
		zputc_rep(' ', padwidth);
	    continue;
	}

	/* d, i, u, p, x, X */
	signc = 0;
	if(c == 'd' || c == 'i') {
	    signed long sl;
	    if(flags & FLAG_SHORT_TYPE) {
		sl = (short)va_arg(ap, int);
	    } else if(flags & FLAG_LONG_TYPE) {
		sl = va_arg(ap, long);
	    } else {
		sl = va_arg(ap, int);
	    }
	    if(sl < 0) {
		signc = '-';
		v = -sl;
	    } else {
		v = sl;
	    }
	} else if(c == 'u' || c == 'x' || c == 'X') {
	    if(flags & FLAG_SHORT_TYPE) {
		v = (unsigned short)va_arg(ap, unsigned int);
	    } else if(flags & FLAG_SHORT_TYPE) {
		v = va_arg(ap, unsigned long);
	    } else {
		v = va_arg(ap, unsigned int);
	    }
	} else if(c == 'p') {
	    v = (unsigned long)va_arg(ap, void *);
	} else {
	    /* bad type specifier */
	    continue;
	}

	width = 0;
	if(c == 'd' || c == 'i' || c == 'u') {
	    do {
		s[width++] = v % 10 + '0';
		v = v / 10;
	    } while(v != 0);
	    if(signc != '-') {
		if(flags & FLAG_ALWAYS_SIGN)
		    signc = '+';
		else if(flags & FLAG_INCLUDE_SIGN)
		    signc = ' ';
	    }
	} else {
	    do {
		char t = v % 16;
		if(t >= 10)
		    if(c == 'x')
			t += 'a' - 10;
		    else
			t += 'A' - 10;
		else
		    t += '0';
		s[width++] = t;
		v = v / 16;
	    } while(v != 0);
	}

	padwidth = minwidth - width - (signc != 0 ? 1 : 0);
	if(!(flags & FLAG_LEFT_ADJ) && !(flags & FLAG_ZERO_PADDING))
	    zputc_rep(' ', padwidth);
	if(signc != 0)
	    zputc(signc);
	if((flags & FLAG_ZERO_PADDING))
	    zputc_rep('0', padwidth);
	while(width > 0)
	    zputc(s[--width]);
	if(flags & FLAG_LEFT_ADJ)
	    zputc_rep(' ', padwidth);
    }
}

/* Put a formatted string to the default device */
void zprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    va_end(ap);
}

#endif /* CONFIG_ZPRINTF */
