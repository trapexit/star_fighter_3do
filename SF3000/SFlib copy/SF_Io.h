/* File : SF_IO -> control pad reading etc. header file */

#ifndef __SF_IO
#define __SF_IO

//	includes

#include "SF_Io.h"
#include "SF_Joystick.h"
#include "SF_Parameters.h"
#include "Utils3DO.h"
#include "event.h"		

// Definitions

#define	MAX_WAITBEFOREVIDEO	10000			// Number of frames before video plays


#define	FLIGHTPAD_X				0
#define	FLIGHTPAD_P				1
#define	FLIGHTPAD_A				2
#define	FLIGHTPAD_B				3
#define	FLIGHTPAD_C				4
#define	FLIGHTPAD_LEFTSHIFT		5
#define	FLIGHTPAD_RIGHTSHIFT	6

#define	FLIGHT_FIRE_WEAPON		0
#define	FLIGHT_SELECT_WEAPON	1
#define	FLIGHT_THRUST			2
#define	FLIGHT_CHANGE_CAM		3
#define	FLIGHT_VIEW_MAP			4
#define	FLIGHT_FIRE_ECM			5
#define	FLIGHT_MENU				6
#define	FLIGHT_FLIP				7

// External variables

extern stick_struct pro_stick ;
extern game_configuration configuration ;

// Function Prototypes

void io_resetvideocounter (long);			// Sets number of frames without input prior to video, (-ve is never)
void io_playvideo (void);					// Play a video on timeout
long controlpad_read(void);					// Immediate status of pad

long controlpad_debounce (void);			// Read debounced keypad presses
long controlpad_constant (void);			// Return's last keypad registered in controlpad_debounce (undebounced)

#endif
