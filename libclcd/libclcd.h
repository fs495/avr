#ifndef __liblcd_h__
#define __liblcd_h__

#include <stdint.h>
#include <avr/pgmspace.h>

#define LCD_H_DOTS 5
#define LCD_V_DOTS 8

/*
 * Note: after calling lcd_setcg(), lcd_setcgp() or lcd_init_bargraph(),
 * lcd_locate() must be used to display characters.
 */

void lcd_init(void);
void lcd_clear(void);
void lcd_locate(uint8_t row, uint8_t col);
void lcd_putc(uint8_t ch);
void lcd_print(const char *s);
void lcd_printp(const char *s);
void lcd_setcg(uint8_t ch, uint8_t n, const void *ptn);
void lcd_setcgp(uint8_t ch, uint8_t n, const void *ptn);
void lcd_init_bargraph(void);
void lcd_printbargraph(uint8_t value);

#endif /* __liblcd_h__ */
