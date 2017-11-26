// *************************************************************************************************
//
//      Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
//
//
//        Redistribution and use in source and binary forms, with or without
//        modification, are permitted provided that the following conditions
//        are met:
//
//          Redistributions of source code must retain the above copyright
//          notice, this list of conditions and the following disclaimer.
//
//          Redistributions in binary form must reproduce the above copyright
//          notice, this list of conditions and the following disclaimer in the
//          documentation and/or other materials provided with the
//          distribution.
//
//          Neither the name of Texas Instruments Incorporated nor the names of
//          its contributors may be used to endorse or promote products derived
//          from this software without specific prior written permission.
//
//        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Temperature measurement functions.
// *************************************************************************************************
#include <msp430.h>
#include "menu.h"

/* A macro that generates a bit mask according to a given bit number.
 * Example:
 * - BIT(0) expands to 1 (== 0x01)
 * - BIT(3) expands to 8 (== 0x08)
 */

#define BIT(x) (1uL << (x))

// LCD Segments
#define LCD_A    BIT4
#define LCD_B    BIT5
#define LCD_C    BIT6
#define LCD_D    BIT7
#define LCD_F    BIT0
#define LCD_G    BIT1
#define LCD_E    BIT2
#define LCD_H    BIT3

// LCD Segment Mapping
const unsigned char  LCD_Char_Map[] =
{
		LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F,        //INDEX 0 - '0' or 'O'
		LCD_B+LCD_C,                                //INDEX 1 - '1' or 'I'
		LCD_A+LCD_B+LCD_D+LCD_E+LCD_G,              //INDEX 2 - '2' or 'Z'
		LCD_A+LCD_B+LCD_C+LCD_D+LCD_G,              //INDEX 3 - '3'
		LCD_B+LCD_C+LCD_F+LCD_G,                    //INDEX 4 - '4' or 'y'
		LCD_A+LCD_C+LCD_D+LCD_F+LCD_G,              //INDEX 5 - '5' or 'S'
		LCD_A+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,        //INDEX 6 - '6' or 'b'
		LCD_A+LCD_B+LCD_C,                          //INDEX 7 - '7'
		LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,  //INDEX 8 - '8' or 'B'
		LCD_A+LCD_B+LCD_C+LCD_D+LCD_F+LCD_G,        //INDEX 9 - '9' or 'g'
		LCD_B+LCD_C+LCD_E+LCD_F+LCD_G,              //INDEX 10 - Displays "X"
		LCD_B+LCD_C+LCD_D+LCD_F+LCD_G,              //INDEX 11 - Displays "Y"
		LCD_D 										//INDEX 12 - '-'
};

const unsigned char  Move_Pattern[] ={

};
// *************************************************************************************************
// Include section

// system
//#include "project.h"

// driver
//#include "display.h"
#include "bmp_as.h"
//#include "cma_as.h"
#include "as.h"

// logic
#include "acceleration.h"
//#include "simpliciti.h"
//#include "user.h"

// *************************************************************************************************
// Global Variable section
struct accel sAccel;

// Conversion values from data to mgrav taken from BMA250 datasheet (rev 1.05, figure 4)
const unsigned short bmp_mgrav_per_bit[7] = { 16, 31, 63, 125, 250, 500, 1000 };
// Conversion values from data to mgrav taken from CMA3000-D0x datasheet (rev 0.4, table 4)
const unsigned short cma_mgrav_per_bit[7] = { 18, 36, 71, 143, 286, 571, 1142 };

// *************************************************************************************************
// Extern section


// *************************************************************************************************
// @fn          reset_acceleration
// @brief       Reset acceleration variables.
// @param       none
// @return      none
// *************************************************************************************************
void reset_acceleration(void)
{
	// Start with Y-axis display
	sAccel.view_style = DISPLAY_ACCEL_Y;

	// Clear timeout counter
	sAccel.timeout = 0;

	// Default mode is off
	sAccel.mode = ACCEL_MODE_OFF;
}

// *************************************************************************************************
// @fn          sx_acceleration
// @brief       Acceleration direct function. Button UP switches between X/Y/Z values.
// @param       unsigned char line         LINE2
// @return      none
// *************************************************************************************************
void sx_acceleration()
{
	if (++sAccel.view_style > 2)
		sAccel.view_style = 0;

	// Reset current acceleration value
	sAccel.data = 0;

	// Get data from sensor
	bmp_as_get_data(sAccel.xyz);
}

// *************************************************************************************************
// @fn          acceleration_value_is_positive
// @brief       Returns 1 if 2's complement number is positive
// @param       unsigned char value        2's complement number
// @return      unsigned char                      1 = number is positive, 0 = number is negavtive
// *************************************************************************************************
unsigned char acceleration_value_is_positive(unsigned char value)
{
	return ((value & BIT7) == 0);
}

// *************************************************************************************************
// @fn          convert_acceleration_value_to_mgrav
// @brief       Converts measured value to mgrav units
// @param       unsigned char value        g data from sensor
// @return      unsigned short                     Acceleration (mgrav)
// *************************************************************************************************
unsigned short convert_acceleration_value_to_mgrav(unsigned char value)
{
	unsigned short result;
	unsigned char i;

	if (!acceleration_value_is_positive(value))
	{
		// Convert 2's complement negative number to positive number
		value = ~value;
		value += 1;
	}

	result = 0;
	for (i = 0; i < 7; i++)
		result += ((value & (BIT(i))) >> i) * bmp_mgrav_per_bit[i];

	return (result);
}

// *************************************************************************************************
// @fn          is_acceleration_measurement
// @brief       Returns 1 if acceleration is currently measured.
// @param       none
// @return      unsigned char              1 = acceleration measurement ongoing
// *************************************************************************************************
unsigned char is_acceleration_measurement(void)
{
	return ((sAccel.mode == ACCEL_MODE_ON) && (sAccel.timeout > 0));
}

// *************************************************************************************************
// @fn          do_acceleration_measurement
// @brief       Get sensor data and store in sAccel struct
// @param       none
// @return      none
// *************************************************************************************************
void do_acceleration_measurement(void)
{
	// Get data from sensor
	bmp_as_get_data(sAccel.xyz);

	// Set display update flag
	//    display.flag.update_acceleration = 1;
}

// *************************************************************************************************
// @fn          display_acceleration
// @brief       Display routine.
// @param       unsigned char line                 LINE1
//                              unsigned char update               DISPLAY_LINE_UPDATE_FULL, DISPLAY_LINE_CLEAR
// @return      none
// *************************************************************************************************
//void display_acceleration(unsigned char line, unsigned char update)
static char MoveX[10] = {0000000000};
static unsigned int xCounter = 0;

void pushMoveX(unsigned int val){

	unsigned int i;
	for (i = 0; i < xCounter; i++)
	    {
	        MoveX[i] = MoveX[i + 1];
	    };

	if (xCounter < 11){
		MoveX[xCounter] = val;
	} else {
		xCounter
	}
}

void display_acceleration()
{
	//    unsigned char *str;
	unsigned char raw_data;
	//    unsigned char percent[3];
	unsigned short accel_data;

	// Show warning if acceleration sensor was not initialised properly
	if (!as_ok)
	{
		//        display_chars(LCD_SEG_L1_2_0, (unsigned char *) "ERR", SEG_ON);

		__no_operation();
	}
	else
	{
		// Redraw whole screen
		//        if (update == DISPLAY_LINE_UPDATE_FULL)
		{
			{
				// Start acceleration sensor
				if (!is_acceleration_measurement())
				{
					// Clear previous acceleration value
					sAccel.data = 0;

					// Start sensor
					bmp_as_start();

					// Set timeout counter
					sAccel.timeout = ACCEL_MEASUREMENT_TIMEOUT;

					// Set mode
					sAccel.mode = ACCEL_MODE_ON;

					// Start with X-axis values
					//sAccel.view_style = DISPLAY_ACCEL_X;
				}

				// Display decimal point
				//                display_symbol(LCD_SEG_L1_DP1, SEG_ON);
				__no_operation();
			}
		}


		unsigned int valX, valY, valZ;
		static unsigned int oldX, oldY, oldZ;
		unsigned int dirX=0, dirY=0, dirZ=0;

		//GET CURRENT ACCEL DATA
		valX = convert_acceleration_value_to_mgrav(sAccel.xyz[0]) / 10;
		valY = convert_acceleration_value_to_mgrav(sAccel.xyz[1]) / 10;
		valZ = convert_acceleration_value_to_mgrav(sAccel.xyz[2]) / 10;

		//CURRENT ACCEL DATA - PREVIOUS ACCEL DATA TO GET DIRECTION
		if (valX >= oldX){
			dirX = 1;
			pushMoveX(1);
		} else {
			pushMoveX(0);
		}
		if (valY >= oldY){
			dirY = 1;
		}
		if (valZ >= oldZ){
			dirZ = 1;
		}

		oldX = valX;
		oldY = valY;
		oldZ = valZ;

		LCDM2 = LCD_Char_Map[12];	  // Display Z Character
		//LCDM2 = LCD_Char_Map[2];	  // Display Z Character
		//LCDM2 = LCD_Char_Map[11];     // Display Y Character
		//LCDM2 = LCD_Char_Map[10];     // Display X Character


		// Filter acceleration
		accel_data = valZ;
		accel_data = (unsigned short) ((accel_data * 0.2) + (sAccel.data * 0.8));

		// Store average acceleration
		sAccel.data = accel_data;

		// Display acceleration in xxx format
		unsigned char hundreds, tens, ones;
		unsigned char *lcdmem;

		// Display sign
		//if (acceleration_value_is_positive(raw_data))
		if (MoveX == "1111111111")
		{
			hundreds = valX % 1000 / 100;
			tens = valX % 100 / 10;
			ones = valX % 10;

			LCDM3 = LCD_Char_Map[hundreds];      // Display Character
			LCDM4 = LCD_Char_Map[tens];          // Display Character
			LCDM6 = LCD_Char_Map[ones];          // Display Character

			lcdmem  = (unsigned char *)0x0A20;
			*lcdmem = (unsigned char)(*lcdmem | (BIT2));	// arrow display

			__delay_cycles(100000);           // Delay ~0.1sec
			*lcdmem = (unsigned char)(*lcdmem & (BIT2));
			__no_operation();
		}
	}
}

