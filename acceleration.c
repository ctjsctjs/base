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
#define patLength 30
#define patSeq 15

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
unsigned int MoveX[patLength];
unsigned int MoveY[patLength];
unsigned int MoveZ[patLength];

int timesX=0;
int timesY=0;
int timesZ=0;

void reset(char letter){
	int i;
	if (letter == 'x'){
		for (i = 0; i < patLength; i++) {
			MoveX[i] = 0;
		}
	} else if(letter == 'y'){
		for (i = 0; i < patLength; i++) {
			MoveY[i] = 0;
		}
	} else if(letter == 'z') {
		for (i = 0; i < patLength; i++) {
			MoveZ[i] = 0;
		}
	}
}
void pushMoveX(unsigned int val) {
	int i;
	for (i = patLength; i >= 0; i--)
	{
		MoveX[i+1] = MoveX[i];
	};

	MoveX[0] = val;

	return;
}

void pushMoveY(unsigned int val) {
	int i;
	for (i = patLength; i >= 0; i--)
	{
		MoveY[i+1] = MoveY[i];
	};

	MoveY[0] = val;

	return;
}

void pushMoveZ(unsigned int val) {
	int i;
	for (i = patLength; i >= 0; i--)
	{
		MoveZ[i+1] = MoveZ[i];
	};

	MoveZ[0] = val;

	return;
}

void detectPatX(int pat) {
	if (pat == patSeq) {
		if (timesX==10){
			timesX=0;
		}
		timesX++;
		LCDM6 = LCD_Char_Map[timesX];
		reset('x');
		return;
	}
	return;
}

void detectPatY(int pat) {
	if (pat == patSeq) {
		if (timesY==10){
			timesY=0;
		}
		timesY++;
		LCDM4 = LCD_Char_Map[timesY];
		reset('y');
		return;
	}
	return;
}

void detectPatZ(int pat) {
	if (pat == patSeq) {
		if (timesZ==10){
			timesZ=0;
		}
		timesZ++;
		LCDM2 = LCD_Char_Map[timesZ];
		reset('z');
		return;
	}
	return;
}

int checkArr(unsigned int arr[]) {
	int i, sameCount=0, prevNum=0;
	for (i = 0; i < patLength; i++) {
		if (prevNum == 1) {

			if (arr[i] == 1) {
				sameCount++;
			}
			else {
				sameCount = 0;
			}
		}
		prevNum = arr[i];
	}
	return (sameCount);
}


void display_acceleration()
{
	//    unsigned char *str;
	//    unsigned char percent[3];
	//unsigned short accel_data;

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

		//GET CURRENT ACCEL DATA
		valX = convert_acceleration_value_to_mgrav(sAccel.xyz[0]) / 10;
		valY = convert_acceleration_value_to_mgrav(sAccel.xyz[1]) / 10;
		valZ = convert_acceleration_value_to_mgrav(sAccel.xyz[2]) / 10;

		//CURRENT ACCEL DATA - PREVIOUS ACCEL DATA TO GET DIRECTION
		if (valX >= oldX){
			pushMoveX(1);
		} else {
			pushMoveX(0);
		}
		if (valY >= oldY){
			pushMoveY(1);
		} else {
			pushMoveY(0);
		}
		if (valZ >= oldZ){
			pushMoveZ(1);
		} else {
			pushMoveZ(0);
		}

		oldX = valX;
		oldY = valY;
		oldZ = valZ;

		detectPatX(checkArr(MoveX));
		detectPatY(checkArr(MoveY));
		detectPatZ(checkArr(MoveZ));


		// Filter acceleration
		//accel_data = valZ;
		//accel_data = (unsigned short) ((accel_data * 0.2) + (sAccel.data * 0.8));

		// Store average acceleration
		//sAccel.data = accel_data;

	}
}

