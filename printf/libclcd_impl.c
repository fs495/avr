/*
 * LCD library implementation
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
#undef  CONFIG_LCD_SETCGGP
#undef  CONFIG_LCD_BARGRAPH

/*
 * timing method
 */
#define CONFIG_TIMER_DELAY	/* wait using fixed timer */

/*
 * LCD data bus
 */
#define LCD_DB_PORT PORTD
#define LCD_DB_PIN  PIND
#define LCD_DB_DIR  DDRD

#define LCD_DB_MASK  15
#define LCD_DB_SHIFT 0

/*
 * LCD control
 */
#define LCD_CTL_PORT PORTD
#define LCD_CTL_DIR  DDRD

#define LCD_RS 6
#define LCD_RW 5
#define LCD_E  4

/* includes template library */
#include "../libclcd/libclcd.c"
