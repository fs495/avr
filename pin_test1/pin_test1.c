#include <avr/io.h>
#include <util/delay.h>

static void sys_init(void)
{
    /* enable pull-up */
    MCUCR &= ~_BV(PUD);

    /* port B[7-1] are pulled-up input,  B[0] is output */
    PORTB = 255;
    DDRB = 1;
}

void main(void)
{
    char tmp = 0;

    sys_init();

    for(;;) {
	PORTB ^= 1;

	++tmp;
	_delay_ms(50);
    }
}
