/**
 * グラフィック液晶モジュールAQM1248のテンプレートライブラリ
 *
 * 使用側では以下の4つの関数を実装する。
 * (1) glcd_connect_spi()	AQM1248に対してSPI接続する。SS信号をアサート。
 * (2) glcd_disconnect_spi()	AQM1248に対してSPI接続を解除する
 * (3) glcd_select_cmd()	コマンド送信状態(RS=L)にする
 * (4) glcd_select_data()	データ送信状態(RS=H)にする
 * (5) glcd_send_byte()		データ/コマンドを出力する
 *
 * すべてのAPI関数はglcd_connect_spi()とglcd_disconnect_spi()の
 * 呼び出しの間に呼びださなければならない。
 * 
 * ユーザ側はデータ送信状態にして、glcd_send_byte()でデータやコマンドを送信しても
 * 構わないが、API関数呼び出し時はコマンド送信状態でなければならない。
 */

#include <avr/pgmspace.h>
#include <util/delay.h>
#include "libglcd.h"

/*======================================================================
 * 初期化
 */

void glcd_init(void)
{
    glcd_select_cmd();

    glcd_send_byte(0xae); /* display = off */
    glcd_send_byte(0xa0); /* ADC(address counter?) = normal */
    glcd_send_byte(0xc8); /* common output = reverse*/
    glcd_send_byte(0xa3); /* LCD bias = 1/7 */

    glcd_send_byte(0x2c); /* power control 1 */
    _delay_ms(2);
    glcd_send_byte(0x2e); /* power control 2 */
    _delay_ms(2);
    glcd_send_byte(0x2f); /* power control 3 */

    glcd_send_byte(0x23);
    glcd_send_byte(0x81);
    glcd_send_byte(0x19);

    glcd_send_byte(0xa4); /* display all point = normal*/
    glcd_send_byte(0x40); /* display start line = 0 */
    glcd_send_byte(0xa6); /* common output = normal */
    glcd_send_byte(0xaf); /* display = on */

    glcd_clear_vram();
}

/*======================================================================
 * 低レベルコマンドの発行
 * 呼び出し前にコマンド送信可能(SPI有効、SSアサート済、RS=コマンド)であること
 */

void glcd_display_on(void)
{
    glcd_send_byte(0xae);
}

void glcd_display_off(void)
{
    glcd_send_byte(0xaf);
}

void glcd_set_display_page(uint8_t page)
{
    glcd_send_byte(0x40 | page);
}

void glcd_set_addr_page(uint8_t page)
{
    glcd_send_byte(0xb0 | page);
}

void glcd_set_addr_col(uint8_t col)
{
    glcd_send_byte(0x10 | (col >> 4));
    glcd_send_byte(0x00 | (col & 0xf));
}

void glcd_set_resistor_ratio(uint8_t val)
{
    glcd_send_byte(0x20 | val);
}

void glcd_set_contrast(uint8_t val)
{
    glcd_send_byte(0x81);
    glcd_send_byte(val);
}

void glcd_set_sleep_mode(void)
{
    glcd_send_byte(0xac);
    glcd_send_byte(0x00);
}

void glcd_leave_sleep_mode(void)
{
    glcd_send_byte(0xad);
    glcd_send_byte(0x00);
}

/*======================================================================
 * ブロック書き込み・ブロックフィル
 */

/*
 * ブロックデータを(sx,sy)の位置から、横wドット、縦hページ分書き込む
 */
void glcd_write_block(uint8_t sx, uint8_t sy, uint8_t w, uint8_t h,
		      const uint8_t *p)
{
    uint8_t x, y;
    for(y = 0; y < h; y++) {
	glcd_select_cmd();
	glcd_set_addr_page(sy + y);

	glcd_set_addr_col(sx);
	glcd_select_data();
	for(x = 0; x < w; x++)
	    glcd_send_byte(*p++);
    }
    glcd_select_cmd();
}

/*
 * ブロックデータを(sx,sy)の位置から、横wドット、縦hページ分書き込む
 */
void glcd_write_blockp(uint8_t sx, uint8_t sy, uint8_t w, uint8_t h,
		       const uint8_t *p)
{
    uint8_t x, y;
    for(y = 0; y < h; y++) {
	glcd_select_cmd();
	glcd_set_addr_page(sy + y);

	glcd_set_addr_col(sx);
	glcd_select_data();
	for(x = 0; x < w; x++)
	    glcd_send_byte(pgm_read_byte(p++));
    }
    glcd_select_cmd();
}

/*
 * 表示メモリをクリアする
 */
void glcd_clear_vram()
{
    uint8_t x, y;
    for(y = 0; y < GLCD_PAGES; y++) {
	glcd_select_cmd();
	glcd_set_addr_page(y);

	glcd_set_addr_col(0);
	glcd_select_data();
	for(x = 0; x < GLCD_WIDTH; x++)
	    glcd_send_byte(0);
    }
    glcd_select_cmd();
}
