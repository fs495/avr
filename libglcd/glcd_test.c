#include <avr/pgmspace.h>
#include <util/delay.h>

#include "libglcd.h"

#include "toho-komakyo.c"
#include "../font/font8x16.c"

void hw_init(void);

void main(void)
{
    hw_init();
    DDRD = _BV(7);

    glcd_connect_spi();
    glcd_init();
    glcd_disconnect_spi();

    for(;;) {
	uint8_t i;
	//PORTD ^= _BV(7);

#if 0
	glcd_connect_spi();
	glcd_write_blockp(0, 0, IMG_TOHO_KOMAKYO_WIDTH,
			  IMG_TOHO_KOMAKYO_HEIGHT / 8,
			  img_toho_komakyo);
	glcd_disconnect_spi();
	_delay_ms(1000);
#endif

	glcd_connect_spi();
	PORTD |= _BV(7);
#if 0
	glcd_write_blockp(64, 0, 8, 6, font8x16 + 16);
#endif
#if 1
	for(i = 0; i < 16 * 3; i++) {
	    glcd_write_blockp(8 * (i % 16), 2 * (i / 16), 8, 2,
			      font8x16 + i * 16);
	}
#endif
	PORTD &= ~_BV(7);
	glcd_disconnect_spi();
	_delay_ms(1000);
    }
}
