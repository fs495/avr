/*
 * Sample implementation of LCD library
 *
 * Assuming following connections:
 *  - port D[0-3] <-> LCD D[4-7]
 *  - port D[4] -> LCD E
 *  - port D[5] -> LCD RW
 *  - port D[6] -> LCD RS
 */

#define CONFIG_LCD_CLEAR
#undef  CONFIG_LCD_HOME
#define CONFIG_LCD_LOCATE
#define CONFIG_LCD_PUTC
#define CONFIG_LCD_PRINT
#define CONFIG_LCD_PRINTP
#undef  CONFIG_LCD_SETCG
#define CONFIG_LCD_SETCGGP
#define CONFIG_LCD_BARGRAPH

#define CONFIG_TIMER_DELAY
#undef CONFIG_WAIT_BUSY_CLEAR

/*
 * LCD data bus
 */
#define LCD_DB_PORT PORTC
#define LCD_DB_PIN  PINC
#define LCD_DB_DIR  DDRC

#define LCD_DB_MASK  15
#define LCD_DB_SHIFT 0

/*
 * LCD control
 */
#define LCD_CTL_PORT PORTD
#define LCD_CTL_DIR  DDRD

#define LCD_RW 5
#define LCD_RS 6
#define LCD_E  7

/* includes template library */
#include "libclcd.c"
