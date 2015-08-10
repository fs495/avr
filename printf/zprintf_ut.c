/*
 * unit test for printf-like functions
 */
#include <stdio.h>

#define CONFIG_ZPUTC_REP
#define CONFIG_ZPUTS_REV
#define CONFIG_ZPUT_DEC
#define CONFIG_ZPUT_UDEC
#define CONFIG_ZPUT_HEX
#define CONFIG_ZPRINTF
#define zputc(c)	putchar(c)

#include "zprintf.h"
#include "zprintf.c"

void test_single_functions()
{
    zputs("Hello, world!\n");
    zputc_rep(10, 'x');
    zputc('\n');
    zputs_rev("9876543210", 10);
    zputc('\n');

    zput_dec(1); zput_dec(200); zput_dec(30000); zput_dec(-30000);
    zputc('\n');
    zput_udec(1); zput_udec(200); zput_udec(30000); zput_udec(65535);
    zputc('\n');
    zput_hex(1); zput_hex(0xeeee);
    zputc('\n');
}

#define TEST_DEC(fmt, n) zprintf("%" fmt " " fmt "\n", (n))

void test_decimal(int n)
{
    /* [flags: -, +, space, 0] [minwidth] type*/
    TEST_DEC("%d<   ", n);
    TEST_DEC("%0d<  ", n);
    TEST_DEC("% d<  ", n);
    TEST_DEC("% 0d< ", n);
    TEST_DEC("%+d<  ", n);
    TEST_DEC("%+0d< ", n);
    TEST_DEC("% +d< ", n);
    TEST_DEC("% +0d<", n);
    TEST_DEC("%-d<  ", n);
    TEST_DEC("%-0d< ", n);
    TEST_DEC("% -d< ", n);
    TEST_DEC("% -0d<", n);

    TEST_DEC("%5d<   ", n);
    TEST_DEC("%05d<  ", n);
    TEST_DEC("% 5d<  ", n);
    TEST_DEC("% 05d< ", n);
    TEST_DEC("%+5d<  ", n);
    TEST_DEC("%+05d< ", n);
    TEST_DEC("% +5d< ", n);
    TEST_DEC("% +05d<", n);
    TEST_DEC("%-5d<  ", n);
    TEST_DEC("%-05d< ", n);
    TEST_DEC("% -5d< ", n);
    TEST_DEC("% -05d<", n);
}

void test_unsigned(void)
{
    zprintf("%u\n", 123);
    zprintf("%u\n", -123);
    zprintf("%15u\n", 123);
    zprintf("%15u\n", -123);
    zprintf("%015u\n", 123);
    zprintf("%015u\n", -123);
}

void test_hex(void)
{
    zprintf("%%8x  %8x<\n", 0xabc);
    zprintf("%%08x %08x<\n", 0xabc);
    zprintf("%%-8x %-8x<\n", 0xabc);
    zprintf("%%8X  %8X<\n", 0xabc);
    zprintf("%%8p  %8p<\n", 0xabc);
}

void test_narrow(void)
{
    zprintf("%%3d  %3d<\n", 12345);
    zprintf("%%03d %03d<\n", 12345);
    zprintf("%%-3d %-3d<\n", 12345);
    zprintf("%%3u  %3u<\n", 12345);
    zprintf("%%3u  %3u<\n", -1);
    zprintf("%%3x  %3x<\n", 0x1234);
}

void test_string(void)
{
    zprintf("<%c>\n", 'a');
    zprintf("<%s>\n", "abcde");
    zprintf("<%2s>\n", "abcde");
    zprintf("<%10s>\n", "abcde");
    zprintf("<%-10s>\n", "abcde");
}

int main()
{
    test_single_functions();
    test_decimal(123);
    test_decimal(-123);
    test_unsigned();
    test_hex();
    test_narrow();
    test_string();

    return 0;
}
