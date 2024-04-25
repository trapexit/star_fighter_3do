/* File : SF_UTILITY -> General utilities header file */

#ifndef __SF_UTILITY
#define __SF_UTILITY


// Includes
	
#include "string.h"
#include "stdio.h"
#include "SF_Pyramid.h"


// External Variables

extern	Item		VBLIOReq;
extern	pyramid		pyramids [4];


// Function Prototypes

char	decode_level (long level);			// Turn a level number into a char
long	return_mission (pyramid*);			// Returns current mission number for a pyramid
long	return_missionpos (long, long);		// Return a mission number from a passed pyramid x&y position
long	total (long);						// Does things to a number (used to get offset for ypos)
void	delay_frames (long frame_count);	// Waits a given number of frames

#endif
