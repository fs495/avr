/*
 * LCD display test
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "libclcd.h"

/*-------------------------------------------------------------*/
static void sys_init(void)
{
}

/*-------------------------------------------------------------*/

const uint8_t cg_pattern[2][8] PROGMEM = {
    {0x1f, 0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f},
};

void main(void)
{
    char i, j;

    sys_init();
    lcd_init();

    for(;;) {
	/* 文字列表示 */
	lcd_clear();
	lcd_printp(PSTR("Print String"));
	lcd_locate(1, 0);
	lcd_printp(PSTR("Hello cruel world!"));
	_delay_ms(1000);

	/* 文字全種類表示 */
	lcd_clear();
	lcd_printp(PSTR("All Character"));
	for(i = 0; i < 16; i++) {
	    lcd_locate(0, 14);
	    lcd_putc('0' + i / 10);
	    lcd_putc('0' + i % 10);

	    lcd_locate(1, 0);
	    for(j = 0; j < 16; j++)
		lcd_putc(i * 16 + j);
	    _delay_ms(1000);
	}

	/* CGRAMテスト */
	lcd_clear();
	lcd_printp(PSTR("CGRAM test"));
	lcd_setcgp(1, 2, cg_pattern);
	lcd_locate(1, 0);
	for(i = 0; i < 8; i++) {
	    lcd_putc(1);
	    lcd_putc(2);
	}
	_delay_ms(1000);

	/* バーグラフテスト */
	lcd_init_bargraph();
	lcd_clear();
	lcd_printp(PSTR("Bar Graph"));

	for(i = 0; i <= 80; i++) {
	    lcd_locate(0, 14);
	    lcd_putc('0' + i / 10);
	    lcd_putc('0' + i % 10);

	    lcd_locate(1, 0);
	    lcd_printbargraph(i);
	    _delay_ms(50);
	}
    }
}
