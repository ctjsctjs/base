/*
 * menu.c
 *
 *  Created on: 22 Nov 2017
 *      Author: Clement
 */

#include "menu.h"
#include <msp430.h>

void init_buttons(void)
{
	// Set button ports to input
	BUTTONS_DIR &= ~ALL_BUTTONS;

	// Enable internal pull-downs
	BUTTONS_OUT &= ~ALL_BUTTONS;
	BUTTONS_REN |= ALL_BUTTONS;

	// IRQ triggers on rising edge
	BUTTONS_IES &= ~ALL_BUTTONS;

	// Reset IRQ flags
	BUTTONS_IFG &= ~ALL_BUTTONS;

	// Enable button interrupts
	BUTTONS_IE |= ALL_BUTTONS;
}

