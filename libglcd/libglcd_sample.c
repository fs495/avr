/*
 * libglcdのサンプル実装ファイル
 *
 * PB7/6 - XTAL1/2
 * PB5 - SCK
 * PB4 - MISO
 * PB3 - MOSI
 * PB2 - デフォルトSS(使用しない)
 * PB1 - RS
 * PB0 - ユーザーSS(使用する)
 *
 * SCLK ___||||||||_____||||||||___
 * MOSI ___XXXXXXXX_____XXXXXXXX___
 * SPIF __________|----|_______|---
 * pend ___|-----------------------
 */
#include <avr/io.h>
#include "libglcd.h"

#define GLCD_RS 1 /* PB1 */
#define GLCD_SS 0 /* PB0 */

#define SPCR_CLK 0 /* fosc / 2 */

static uint8_t hw_pending;

void hw_init(void)
{
    /* MOSI, SCK, デフォルトSSは出力設定 */
    DDRB |= _BV(5) | _BV(3) | _BV(2);

    /* RS, ユーザSSも出力設定 */
    DDRB |= _BV(GLCD_SS) | _BV(GLCD_RS);

    /* SSをネゲート */
    PORTB |= _BV(GLCD_SS);

    hw_pending = 0;
}

static void hw_flush(void)
{
    /* 未転送状態なら何もしない */
    if(!hw_pending)
	return;

    /* 転送途中の場合は、転送完了まで待つ */
    while((SPSR & _BV(SPIF)) == 0)
	;
    hw_pending = 0;
}

void glcd_connect_spi(void)
{
    /* SPIはマスター側、モード3(SCKはアイドル時H, 立ち上がり時にラッチ) */
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | SPCR_CLK;

    /* クロック2倍 */
    SPSR = 1;

    /* SSをアサート */
    PORTB &= ~_BV(GLCD_SS);

    hw_pending = 0;
}

void glcd_disconnect_spi(void)
{
    hw_flush();

    /* SSをネゲート */
    PORTB |= _BV(GLCD_SS);
}

void glcd_select_cmd(void)
{
    hw_flush();
    PORTB &= ~_BV(GLCD_RS);
}

void glcd_select_data(void)
{
    hw_flush();
    PORTB |= _BV(GLCD_RS);
}

void glcd_send_byte(uint8_t byte)
{
    /* 前回送信されたデータが転送完了するまで待つ。 */
    if(hw_pending)
	while((SPSR & _BV(SPIF)) == 0)
	    ;

    /* 転送開始する。
     * SPIFフラグはここでいったんクリアされ、転送完了後に再セットされる */
    SPDR = byte;
    hw_pending = 1;
}

#include "libglcd.c"
