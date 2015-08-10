#ifndef __flan_led_h__
#define __flan_led_h__

/*----------------------------------------------------------------------
 * - Pos: 1,2,3,4,5 (根本側が1、先端側が5)
 *	指定位置を点灯させるときH、消灯時Hi-Z。
 *	逆電圧保護のため、Lレベルに駆動しない。
 *	任意のパターンで設定可能。
 * - Color: R1,G1,B1, R2,G2,B2
 *	指定色を点灯させるときH、消灯時L。
 *	同時に複数のColor出力をHレベルにしない(Pos側のファンアウト制限のため)
 */

enum pos_consts {
    /* 実装されたLEDの位置数 */
    NR_POS = 5,
    /* 高速化したい場合は2のべき乗、メモリ使用量減らすならNR_POSにする */
    NR_RESV_POS = 8
};

enum color_consts {
    COL_R1, COL_G1, COL_B1, /* 右 */
    COL_R2, COL_G2, COL_B2, /* 左 */
    NR_COLOR
};

enum duty_consts {
    AMPL_BASE = 16
};

enum hue_consts {
    HUE_BASE = (AMPL_BASE - 1) * 3
};

/*----------------------------------------------------------------------
 * グローバル変数
 */
/* 各行各列の光量値。AMPL_BASE分率 */
extern unsigned char ampl[NR_COLOR][NR_RESV_POS];

/* 色位相: ドライブする色の位相。0〜NR_COLOR-1。 */
extern unsigned char color_phase;

/* 点灯位相: デューティー決定用の位相。0〜duty_period-1。
 * color_phaseが一巡して+1される */
extern unsigned char duty_phase;

/* 点灯周期: デューティー決定用の周期 */
extern unsigned char duty_period;

/*----------------------------------------------------------------------
 * ハードウェア
 */
void set_pos_pattern(char ptn);
void select_color(char col);

#endif /* __flan_led_h__ */
