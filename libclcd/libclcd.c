/*
 * HD44780(Hitachi) / KS0066(Samsung)ベースのキャラクタ液晶モジュールの
 * テンプレートライブラリ
 * 使用時はこのCファイルをインクルードしてオブジェクトファイルを作成する
 */

/*
 * タイミング資料は以下を参照
 * ・ KS0066.pdf
 * ・ http://elm-chan.org/docs/lcd/hd44780_j.html 純正HD44780
 * 
 * 参考資料(SC1602BS-Bモジュール)
 * ・ http://www.oaks-ele.com/oaks_board/oaks16/data/document/oaks16-lcd/man_sc1602.pdf
 * ・ http://akizukidenshi.com/download/ds/sunlike/SC1602BS-B-XA-GB-K.pdf
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "libclcd.h"

/*-------------------------------------------------------------
 */
#ifndef F_CPU
# error "Symbol 'F_CPU' not defined"
#endif

#ifndef nop
# define nop() __asm__ __volatile__ ("nop")
#endif

#define NANO_SECONDS_PER_INSTRUCTION (1000UL * 1000 * 1000 / (F_CPU))

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(x, y) (((x) + (y) - 1) / (y))
#endif /* DIV_ROUND_UP */

#define NR_NOPS(ns) DIV_ROUND_UP((ns), NANO_SECONDS_PER_INSTRUCTION)

#ifndef _delay_ns
/*
 * nop命令実行による遅延。for文の展開はコンパイラ任せ。
 * 遅延の単位時間はクロックによって異なる。
 * 遅延時間は切り上げ。
 */
#define _delay_ns(ns)				\
    do {					\
	register unsigned char i;		\
	for(i = 0; i < NR_NOPS(ns); i++) {	\
	    nop();				\
	}					\
    } while(0)
#endif /* _delay_ns */

/*-------------------------------------------------------------
 * signal-level functions
 */
static inline void lcd_select_data(void)
{
    LCD_CTL_PORT |= _BV(LCD_RS);
}

static inline void lcd_select_cmd(void)
{
    LCD_CTL_PORT &= ~_BV(LCD_RS);
}

#ifdef CONFIG_WAIT_BUSY_CLEAR
static void lcd_set_readable(void)
{
    LCD_DB_PORT &= ~LCD_DB_MASK;
    LCD_DB_DIR &= ~LCD_DB_MASK;
    LCD_CTL_PORT |= _BV(LCD_RW);
}
#endif

static void lcd_set_writable(void)
{
    LCD_DB_DIR |= LCD_DB_MASK;
    LCD_CTL_PORT &= ~_BV(LCD_RW);
}

static void lcd_write_nibble(uint8_t data)
{
    LCD_DB_PORT = (LCD_DB_PORT & ~LCD_DB_MASK) | (data << LCD_DB_SHIFT);
    _delay_ns(40);

    LCD_CTL_PORT |= _BV(LCD_E);
    _delay_ns(230);

    LCD_CTL_PORT &= ~_BV(LCD_E);
    _delay_ns(270 - 40);
}

static void lcd_write_octet(uint8_t data)
{
    lcd_write_nibble((data >> 4) & 15);
    lcd_write_nibble(data & 15);
}

#ifdef CONFIG_WAIT_BUSY_CLEAR
static uint8_t lcd_read_nibble(void)
{
    uint8_t data;

    _delay_ns(40);
    LCD_CTL_PORT |= _BV(LCD_E);
    _delay_ns(230);
    data = (LCD_DB_PIN & LCD_DB_MASK) >> LCD_DB_SHIFT;
    LCD_CTL_PORT &= ~_BV(LCD_E);
    _delay_ns(270 - 40);

    return data;
}

static uint8_t lcd_read_octet(void)
{
    uint8_t data;

    data = lcd_read_nibble() << 4;
    data |= lcd_read_nibble();

    return data;
}
#endif /* CONFIG_WAIT_BUSY_CLEAR */


/*-------------------------------------------------------------
 * command level functions (without wait)
 */

#ifdef CONFIG_TIMER_DELAY

# define lcd_wait_for_ready()
# define lcd_wait_after_op(usec) _delay_us(usec)

#elif defined(CONFIG_WAIT_BUSY_CLEAR)

static void lcd_wait_for_ready(void)
{
    lcd_set_readable();
    lcd_select_cmd();
    while((lcd_read_octet() & 0x80) != 0)
	_delay_us(3);
}
# define lcd_wait_after_op(usec)

#else

#error "should define CONFIG_TIMER_DELAY or CONFIG_WAIT_BUSY_CLEAR!"

#endif

static void lcd_send_cmd_nowait(uint8_t cmd)
{
    lcd_set_writable();
    lcd_select_cmd();
    lcd_write_octet(cmd);
}

static void lcd_send_data_nowait(uint8_t data)
{
    lcd_set_writable();
    lcd_select_data();
    lcd_write_octet(data);
}

/*-------------------------------------------------------------
 * high-level functions (with wait)
 */
static void lcd_clear_display(void)
{
    lcd_wait_for_ready();
    lcd_send_cmd_nowait(0x01);
    lcd_wait_after_op(1530);
}

#ifdef CONFIG_LCD_HOME
static void lcd_return_home(void)
{
    lcd_wait_for_ready();
    lcd_send_cmd_nowait(0x02);
    lcd_wait_after_op(1530);
}
#endif

static void lcd_entry_mode_set(uint8_t flags)
{
    lcd_wait_for_ready();
    lcd_send_cmd_nowait(0x06 | flags);
    lcd_wait_after_op(39);
}

static void lcd_display_control(uint8_t flags)
{
    lcd_wait_for_ready();
    lcd_send_cmd_nowait(0x0c | flags);
    lcd_wait_after_op(39);
}

static void lcd_set_cgram_addr(uint8_t addr)
{
    lcd_wait_for_ready();
    lcd_send_cmd_nowait(0x40 | addr);
    lcd_wait_after_op(39);
}

static void lcd_set_ddram_addr(uint8_t addr)
{
    lcd_wait_for_ready();
    lcd_send_cmd_nowait(0x80 | addr);
    lcd_wait_after_op(39);
}

static void lcd_send_data(uint8_t data)
{
    lcd_wait_for_ready();
    lcd_send_data_nowait(data);
    lcd_wait_after_op(43);
}

/*-------------------------------------------------------------
 * API-level functions
 */

void lcd_init(void)
{
    /* RS=L(command), E=L(not enabled), RW=H(read) */
    LCD_CTL_PORT &= ~_BV(LCD_RS) | ~_BV(LCD_E);
    LCD_CTL_PORT |= _BV(LCD_RW);
    /* port D[RS:RW:E] are output, port D[D3-D1] are Hi-Z input */
    LCD_CTL_DIR |= _BV(LCD_RS) | _BV(LCD_RW) | _BV(LCD_E);

    lcd_set_writable();
    lcd_select_cmd();
    _delay_ms(30);

    /*
     *                KS008    HD44780
     * power on       30ms     15ms
     * function set   39us     37us (4.1ms/0.1ms)
     * display clear  1.53ms
     */

    /* function set (become 8-bit mode from any state) */
    lcd_write_nibble(0x3);
    _delay_ms(2);
    lcd_write_nibble(0x3);
    _delay_ms(2);
    lcd_write_nibble(0x3);
    _delay_ms(2);

    /* function set (become intermediate 4-bit mode from 8-bit mode) */
    lcd_write_nibble(0x2);
    _delay_ms(2);

    /* function set (become final 4-bit mode from 4-bit mode) */
    lcd_write_nibble(0x2);
    lcd_write_nibble(0x8);
    _delay_us(39);

    /* display on */
    lcd_display_control(0);

    /* clear display */
    lcd_clear_display();

    /* entry mode set */
    lcd_entry_mode_set(0);
}

#ifdef CONFIG_LCD_CLEAR
void lcd_clear(void)
{
    lcd_clear_display();
    lcd_set_ddram_addr(0);
}
#endif /* CONFIG_LCD_CLEAR */

#ifdef CONFIG_LCD_HOME
void lcd_home(void)
{
    lcd_return_home();
    lcd_set_ddram_addr(0);
}
#endif /* CONFIG_LCD_HOME */

#ifdef CONFIG_LCD_LOCATE
void lcd_locate(uint8_t row, uint8_t col)
{
    uint8_t tmp = col & 0x3f;
    if(row > 0)
	tmp += 0x40;
    lcd_set_ddram_addr(tmp);
}
#endif /* CONFIG_LCD_LOCATE */

#ifdef CONFIG_LCD_PUTC
void lcd_putc(uint8_t ch)
{
    lcd_send_data(ch);
}
#endif /* CONFIG_LCD_PUTC */

#ifdef CONFIG_LCD_PRINT
void lcd_print(const char *s)
{
    while(*s)
	lcd_send_data(*s++);
}
#endif /* CONFIG_LCD_PRINT */

#ifdef CONFIG_LCD_PRINTP
void lcd_printp(const char *s)
{
    uint8_t c;
    while((c = pgm_read_byte(s++)) != 0)
	lcd_send_data(c);
}
#endif /* CONFIG_LCD_PRINTP */

#ifdef CONFIG_LCD_SETCG
void lcd_setcg(uint8_t ch, uint8_t n, const void *ptn)
{
    uint8_t i;
    lcd_set_cgram_addr(ch * LCD_V_DOTS); /* set CGRAM address */
    for(i = 0; i < n * LCD_V_DOTS; i++)
	lcd_send_data(*ptn++);
}
#endif /* CONFIG_LCD_SETCG */

#ifdef CONFIG_LCD_SETCGGP
void lcd_setcgp(uint8_t ch, uint8_t n, const void *ptn)
{
    uint8_t i;
    lcd_set_cgram_addr(ch * LCD_V_DOTS);

    for(i = 0; i < n * LCD_V_DOTS; i++)
	lcd_send_data(pgm_read_byte(ptn++));
}
#endif /* CONFIG_LCD_SETCGGP */

#ifdef CONFIG_LCD_BARGRAPH
void lcd_init_bargraph(void)
{
    uint8_t i, j, ptn;
    lcd_set_cgram_addr(0);

    for(i = 0; i <= LCD_H_DOTS; i++) {
	ptn = ((1 << LCD_H_DOTS) - 1) << (LCD_H_DOTS - i);
	for(j = 0; j < LCD_V_DOTS; j++) {
	    lcd_send_data(ptn);
	}
    }
}

void lcd_printbargraph(uint8_t value)
{
    while(value >= LCD_H_DOTS) {
	lcd_send_data(LCD_H_DOTS);
	value -= LCD_H_DOTS;
    }
    lcd_send_data(value);
}
#endif /* CONFIG_LCD_BARGRAPH */
