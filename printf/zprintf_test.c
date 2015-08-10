#include "zprintf.h"
#include "../libclcd/libclcd.h"
#include <util/delay.h>

void sysinit(void)
{
    /* enable pull-up */
    MCUCR &= ~_BV(PUD);

    /* port A[7-0] are pulled-up input */
    PORTA = 255;
    DDRA = 0;

    /* port B[7-1] are pulled-up input,  B[0] is output */
    PORTB = 255;
    DDRB = 1;
}

#define zputc(c) lcd_putc(c)

void wait(void)
{
    _delay_ms(1000);
    lcd_clear();
}

int main(void)
{
    sysinit();
    lcd_init();

    for(;;) {
	zputs(PSTR("Hello, world!"));
	wait();
	zputc_rep(10, 'x');
	wait();
	zputs_rev("9876543210", 10);
	wait();
	zput_dec(1); zput_dec(200); zput_dec(30000); zput_dec(-30000);
	wait();
	zput_udec(1); zput_udec(200); zput_udec(30000); zput_udec(65535);
	wait();
	zput_hex(1); zput_hex(0xeeee);
	wait();
    }
    return 0;
}
