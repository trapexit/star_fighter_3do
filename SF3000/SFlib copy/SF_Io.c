//	File : SF_io
//	General utilities to read control pad etc.

#include "SF_io.h"
#include "SF_Utility.h"
#include "SF_Video.h"
#include "SF_Joystick.h"

/**************************************/

// Global variables

long	keypad_iomap [7] = 	{
							ControlX,
							ControlStart,
							ControlA,
							ControlB,
							ControlC,
							ControlLeftShift,
							ControlRightShift,
							};
						
long	io_timetovideo;					// Timeout to video counter, value if -1 
long	io_keypad = 0,
		io_keypad_mask = 0,
		io_keypad_constant = 0;
		
/************************************************************************************************************************************
*													VIDEO PLAY ON TIMEOUT ROUTINES													*
************************************************************************************************************************************/


void io_resetvideocounter (long new_timeout)

// Purpose : Resets timeout to video
// Accepts : New number of frames (without user input) before timout video is displayed [ NOTE : -ve value is never do vid ]
// Returns : Nothing

{
	io_timetovideo = new_timeout;
}

/**************************************/

void io_playvideo (void)

// Purpose : Plays a video if no user input registered for a given time - timeout counter is automatically reset by video player
// Accepts : nothing
// Returns : nothing

{
	video_play ("Krisalis");
}


/************************************************************************************************************************************
*													CONTROL PAD IO ROUTINES															*
************************************************************************************************************************************/

long controlpad_read (void)

// Purpose : Immediately returns status of control pad
// Accepts : nothing
// Returns : Control pad words

{
ControlPadEventData		controlpad_data;
long					controlpad_result;

	// Read keypad
	
	controlpad_result = GetControlPad (1, FALSE, &controlpad_data);
	if (controlpad_result < 0)
		{
		printf(">COULDN'T READ CONTROLPAD\n");
		return -1;
		}
		
	// If using joystick control, set controlpad buttons from joystick
	
	if (configuration.control_method == 1)
	{
	
		// Set button bits from joystick position
		
		if (pro_stick.x < 80)			controlpad_data.cped_ButtonBits |= ControlLeft;
		if (pro_stick.x > 240)			controlpad_data.cped_ButtonBits |= ControlRight;
		if (pro_stick.y < 60)			controlpad_data.cped_ButtonBits |= ControlUp;
		if (pro_stick.y > 180)			controlpad_data.cped_ButtonBits |= ControlDown;
	
		// Set button bits from joystick buttons
		
		if (pro_stick.b & StickLeft)	controlpad_data.cped_ButtonBits |= ControlLeft;
		if (pro_stick.b & StickRight)	controlpad_data.cped_ButtonBits |= ControlRight;
		if (pro_stick.b & StickUp)		controlpad_data.cped_ButtonBits |= ControlUp;
		if (pro_stick.b & StickDown)	controlpad_data.cped_ButtonBits |= ControlDown;
		
		// Set control pad buttons A,B,C, Shift, Start & Stop
		
		if (pro_stick.b & StickFire)	controlpad_data.cped_ButtonBits |= ControlA;
		if (pro_stick.b & StickA) 		controlpad_data.cped_ButtonBits |= ControlA;
		if (pro_stick.b & StickB) 		controlpad_data.cped_ButtonBits |= ControlB;
		if (pro_stick.b & StickC) 		controlpad_data.cped_ButtonBits |= ControlC;
	
		if (pro_stick.b & StickLeftShift) controlpad_data.cped_ButtonBits |= ControlLeftShift;	
		if (pro_stick.b & StickRightShift) controlpad_data.cped_ButtonBits |= ControlRightShift;	
	
		if (pro_stick.b & StickPlay) controlpad_data.cped_ButtonBits |= ControlStart;
		if (pro_stick.b & StickStop) controlpad_data.cped_ButtonBits |= ControlX;		
	}
	
	// Timeout to video ?
		
	if (controlpad_data.cped_ButtonBits == 0)
		{
		if (io_timetovideo > 0)
			if (--io_timetovideo == 0)
				io_playvideo();
		}
	else
		io_timetovideo = MAX_WAITBEFOREVIDEO;
			
	// Return buttons
	
	return (controlpad_data.cped_ButtonBits);

}

/**************************************/

long controlpad_debounce (void)

// Purpose : Returns debounced keypad presses
// Accepts : nothing
// Returns : Debounced keypad presses

{
	io_keypad_constant = controlpad_read ();	// Read keypad
	io_keypad = io_keypad_constant;				// Copy into keypad
	io_keypad_mask &= io_keypad;				// Update mask
	io_keypad &= ~io_keypad_mask;				// Debounce keys
	io_keypad_mask |= io_keypad;				// Update mask again
	
	return (io_keypad);							// Return debounced keypad value
}

/**************************************/

long controlpad_constant (void)

// Purpose : Returns last registered keypresses from controlpad_debounce - These reading are not debounced
// Accepts : nothing
// Returns : Last keypad presses

{
	return (io_keypad_constant);				// Return dlast read keypad value
}
