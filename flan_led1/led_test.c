#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "flan_led.h"

void init_port(void)
{
    /*
     * PORTD[7:6] = NC, output(L)
     * PORTD[5:0] = color[5:0], output(L)
     */
    PORTD = 0b00000000;
    DDRD  = 0b11111111;

    /*
     * PORTB[7:5] = ISP, input(pull-up)
     * PORTB[4] = NC, output(L)
     * PORTB[3] = SPKOUT, output(L)
     * PORTB[2:0] = pos[3:1], input(Hi-Z)
     */
    PORTB = 0b11100000;
    DDRB  = 0b00011000;

    /*
     * PORTA[2] = ISP, input(pull-up)
     * PORTA[1:0] = pos[5:4], input(Hi-Z)
     */
    PORTA = 0b100;
    DDRA  = 0b000;
}

void set_pos_pattern(char ptn)
{
    /*
     * DDRxクリア -> PORTxクリア -> PORTxセット -> DDRxセット
     * としてセット・クリアともにL駆動状態を避ける
     */
    unsigned char t = ptn & 7;
    DDRB &= ~0b111;
    PORTB &= ~0b111;
    PORTB |= t;
    DDRB |= t;

    t = (ptn >> 3) & 3;
    DDRA &= ~0b11;
    PORTA &= ~0b11;
    PORTA |= t;
    DDRA |= t;
}

void select_color(char col)
{
    PORTD = 1 << col;
}

/*
 * Timer0: 割り込みインターバルの生成
 *	8MHz -> プリスケーラで1/8 -> CTCで1/200分周 -> トリガ1/2 -> 2.5kHz
 *	これを6色16分解能で割った、約26Hzが点灯周期
 */
void init_timer(void)
{
    /*         ++-------- OC0A切断(ピン出力せず)
     *         ||    ++-- CTC動作 */
    TCCR0A = 0b00000010;
    /*             +----- CTC動作
     *             |+++-- prescale(1/8)  */
    TCCR0B = 0b00000010;
    OCR0A = 200;
    TIMSK = 1 << OCIE0A;
    sei();
}

void set_all_duty(char duty)
{
    unsigned char color, pos;
    for(color = 0; color < NR_COLOR; color++)
	for(pos = 0; pos < NR_POS; pos++)
	    ampl[color][pos] = duty;
}

void change_amplitude_demo(void)
{
    unsigned char duty;
    for(;;) {
	for(duty = 0; duty < AMPL_BASE; duty++) {
	    set_all_duty(duty);
	    _delay_ms(10);
	}
	for(duty = 0; duty < AMPL_BASE; duty++) {
	    set_all_duty(AMPL_BASE - 1 - duty);
	    _delay_ms(10);
	}
    }
}

void get_color(unsigned char colors[3], unsigned char hue)
{
    while(hue >= HUE_BASE)
	hue -= HUE_BASE;
    if(hue < HUE_BASE / 3) {
	colors[0] = AMPL_BASE - hue;
	colors[1] = hue;
	colors[2] = 0;
    } else if(hue < HUE_BASE * 2 / 3) {
	hue -= HUE_BASE / 3;
	colors[0] = 0;
	colors[1] = AMPL_BASE - hue;
	colors[2] = hue;
    } else {
	hue -= HUE_BASE * 2 / 3;
	colors[0] = hue;
	colors[1] = 0;
	colors[2] = AMPL_BASE - hue;
    }
}

void change_hue_demo(void)
{
    unsigned char hue_offset, pos, colors[3];

    hue_offset = 0;
    for(;;) {
	for(pos = 0; pos < NR_POS; pos++) {
	    get_color(colors, hue_offset + pos * HUE_BASE / NR_POS);
	    ampl[0][pos] = ampl[3][pos] = colors[0] / 2;
	    ampl[1][pos] = ampl[4][pos] = colors[1] / 2;
	    ampl[2][pos] = ampl[5][pos] = colors[2] / 2;
	}
	_delay_ms(50);

	if(++hue_offset >= HUE_BASE)
	    hue_offset = 0;
    }
}

void main(void)
{
    init_port();
    init_timer();
    /* change_amplitude_demo(); */
    change_hue_demo();
}
