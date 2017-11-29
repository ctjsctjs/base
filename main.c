#include <msp430.h>
#include <stdio.h>
#include "as.h"
#include "acceleration.h"
#include "menu.h"

// *************************************************************************************************
// @fn          lcd_init
// @brief       Erase LCD memory. Init LCD peripheral.
// @param       none
// @return      none
// *************************************************************************************************
void lcd_init(void)
{
	// Clear entire display memory
	LCDBMEMCTL |= LCDCLRBM + LCDCLRM;

	// LCD_FREQ = ACLK/16/8 = 256Hz
	// Frame frequency = 256Hz/4 = 64Hz, LCD mux 4, LCD on
	LCDBCTL0 = (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3) | (LCDPRE0 + LCDPRE1) | LCD4MUX | LCDON;

	// LCB_BLK_FREQ = ACLK/8/4096 = 1Hz
	LCDBBLKCTL = LCDBLKPRE0 | LCDBLKPRE1 | LCDBLKDIV0 | LCDBLKDIV1 | LCDBLKDIV2 | LCDBLKMOD0;

	// I/O to COM outputs
	P5SEL |= (BIT5 | BIT6 | BIT7);
	P5DIR |= (BIT5 | BIT6 | BIT7);

	// Select LCD Segments
	LCDBPCTL0 = 0x0C00 + 0x00C0 + 0x0030 + 0x000C + 0x0003;
	LCDBPCTL1 = 0x0000;
}

/*
 * main.c
 */


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	lcd_init();
	//Init buttons
	as_init();

	// Reset acceleration measurement
	reset_acceleration();

	// *************************************************************************************************
	//MY CODE
	init_buttons();
	// Init acceleration sensor


	// *************************************************************************************************
	setPatt();

	while(1) {
		do_acceleration_measurement();
		display_acceleration();

		if (BUTTON_STAR_IS_PRESSED){
			setPatt();
		}
	}
	//return 0;
}
