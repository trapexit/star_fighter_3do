// File : Palette.h - Palette Declarations

#ifndef __SF_PALETTE
#define __SF_PALETTE


// Includes

#include "graphics.h"
#include "Utils3DO.h"

// External Variables

extern	ScreenContext*	screen;
extern	Item			VBLIOReq;


// Palette definitions


#define	RED_15			0x7c00		// Colour Definitions for 16bpp colour entries (0-31 for RGB, top bit left clear)
#define	GREEN_15		0x3e0
#define	DARKGREEN1_15	0x240
#define	DARKGREEN2_15	0x180
#define	YELLOW_15		0x7fe0
#define	BLUE_15			0x1f
#define	MAGENTA_15		0x7c1f
#define	CYAN_15			0x3ff
#define BLACK_15		1
#define LIGHTGREY_15	0x6318
#define GREY_15			0x5294
#define	DARK_15			0x294a
#define	WHITE_15		0x7fff
#define ORANGE_15		0x7de0

#define	RED_PAL		0				// Colour Definitions for internal plain palette
#define GREEN_PAL	8
#define	YELLOW_PAL	16
#define	BLUE_PAL	24
#define MAGENTA_PAL	32
#define CYAN_PAL	40
#define	WHITE_PAL	48
#define	GREY_PAL	51
#define	DARK_PAL	53

// Function prototypes

void palette_set (long p_red, long p_green, long p_blue);		// Set screen palette with component scales

#endif
	
