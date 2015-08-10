#include <avr/interrupt.h>
#include "flan_led.h"

/*----------------------------------------------------------------------
 * グローバル変数の定義
 */
unsigned char ampl[NR_COLOR][NR_RESV_POS];
unsigned char color_phase;
unsigned char duty_phase;
unsigned char duty_period = AMPL_BASE;

/*----------------------------------------------------------------------
 * 割り込みルーチン
 */
ISR(TIMER0_COMPA_vect)
{
    unsigned char i, ptn;

    /* 出力設定 */
    select_color(color_phase);
    ptn = 0;
    for(i = 0; i < NR_POS; i++) {
	if(ampl[color_phase][i] > duty_phase)
	    ptn |= 1 << NR_POS;
	ptn = ptn >> 1;
    }
    set_pos_pattern(ptn);

    /* 位相カウンタをインクリメント */
    if(++color_phase >= NR_COLOR) {
	color_phase = 0;
	if(++duty_phase >= duty_period)
	    duty_phase = 0;
    }
}
