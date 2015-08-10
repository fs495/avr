-*- outline -*-

* 機能マクロ

機能を有効にするかしないかを決めるマクロを実装ファイル中で#defineか#undefする。

#undef CONFIG_LCD_CLEAR
#undef CONFIG_LCD_HOME
#undef CONFIG_LCD_LOCATE
#undef CONFIG_LCD_PUTC
#undef CONFIG_LCD_PRINT
#undef CONFIG_LCD_PRINTP
#undef CONFIG_LCD_SETCG
#undef CONFIG_LCD_SETCGGP
#undef CONFIG_LCD_BARGRAPH

* ウェイト方式

どちらかを選択して#defineする。

#undef CONFIG_TIMER_DELAY	/* wait using fixed timer */
#undef CONFIG_WAIT_BUSY_CLEAR	/* wait until busy flag is cleared */

* ハードウェア

ハードウェア定義を記述する

** データ信号
データ信号を割り当てたポート名とポート番号を定義する。
ポートは同じポートグループ内の連続する4信号。
以下はPORTC[3:0]を割り当てる例

#define LCD_DB_PORT PORTC
#define LCD_DB_PIN  PINC
#define LCD_DB_DIR  DDRC

#define LCD_DB_MASK  15
#define LCD_DB_SHIFT 0

** 制御信号
制御信号を割り当てたポート名とポート番号を定義。
ポートは同じポートグループ内の2~3信号。

LCD_RW は CONFIG_WAIT_BUSY_CLEAR を定義したときのみ定義が必要。

#define LCD_CTL_PORT PORTD
#define LCD_CTL_DIR  DDRD

#define LCD_RW 5
#define LCD_RS 6
#define LCD_E  7

* テンプレートライブラリのインクルード

#include "liblcd.c"
