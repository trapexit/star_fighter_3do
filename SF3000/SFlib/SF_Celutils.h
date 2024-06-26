//	File : Cel_Utils header file

#ifndef __SF_CELUTILS
#define __SF_CELUTILS

//	includes

#include "UMemory.h"	
#include "Stdio.h"
#include "graphics.h"
#include "Utils3DO.h"
#include "SF_Allocation.h"

//	Definitions for text adder

#define	ALPHA_NORMAL		0
#define	ALPHA_TOP			1
#define	ALPHA_BOTTOM		2
#define	ALPHA_CIRCLE		3
#define	ALPHA_BEHIND		4

//	Definitions for rectangle drawer

#define REC_BORDERSIZE 		3
#define REC_SHADE_UP		10
#define REC_SHADE_LEFT		7
#define REC_SHADE_DOWN		1
#define REC_SHADE_RIGHT		3
#define REC_SHADE_MIDDLE	5
#define	REC_SHADE_LINE		17

#define	REC_BORDER			1
#define	REC_BOX				2

// Gameset 'Game.<planet>' Definitions

#define	BASECEL_SHIELD				4
#define	BASECEL_LIVES				8
#define	BASECEL_COCKPIT				9
#define	BASECEL_SMOKE				16
#define BASECEL_CURSOR				23
#define	BASECEL_CRYSTAL				28
#define	BASECEL_TIMER				36
#define	BASECEL_DISTANT				40
#define	BASECEL_WEAPON				44
#define	BASECEL_FORMATION			52
#define	BASECEL_TICK				66
#define	BASECEL_DIRECTION			86
#define	BASECEL_GRIDREF				94
#define	BASECEL_MAP_NUMBERS			94
#define	BASECEL_MAP_LETTERS			102
#define	BASECEL_GRIDLOCK			110
#define	BASECEL_GRIDSIGN			114
#define	BASECEL_PILOTS				116

#define	BASECEL_MAP_PALETTE			132
#define	BASECEL_MAP_TYPE_VEHICLE	135
#define	BASECEL_MAP_TYPE_MOTHERSHIP	136
#define	BASECEL_MAP_TYPE_SPACESHIP	137
#define	BASECEL_MAP_TYPE_PARACHUTE	138
#define	BASECEL_MAP_TYPE_MISSILE	139
#define	BASECEL_MAP_TYPE_FIGHTER	140
#define	BASECEL_MAP_TYPE_SATELLITE	143
#define	BASECEL_MAP_TARGET			144
#define	BASECEL_MAP_CHEAT			147
#define	BASECEL_MAP_BACKDROP		146
#define	BASECEL_MAP_ZOOM			148
#define	BASECEL_MAP_STATUS			151
#define	BASECEL_MAP_QUESTION		153
#define	BASECEL_MAP_CONTROL			155
#define	BASECEL_MAP_ENGINE			157
#define	BASECEL_MAP_SHIELDS			159
#define	BASECEL_MAP_LASERBAR		161
#define	BASECEL_MAP_GRAPH			162
#define	BASECEL_MAP_MODE			165
#define	BASECEL_MAP_HEIGHT			166
#define	BASECEL_MAP_KILL			167
			
// Structure definitions

typedef struct 	cel_celdata
				{
				long	temp_cels ;			// No. of temporary cels currently in plotting list
				
				long	x_pos0 ;			// 4 point co-ordinates for adding polygons
				long	y_pos0 ;
				long	x_pos1 ;
				long	y_pos1 ;
				long	x_pos2 ;
				long	y_pos2 ;
				long	x_pos3 ;
				long	y_pos3 ;
				
				CCB**	plotlist;			// Address of temporary ccb plot list
				long 	shade ;				// Shade value of polygon to add, 0-7
				char*	cel_palette;		// Address of palette entries for monochrome polygons			
				char*	cel_creation;		// Address of cel creation list
				char*	cels4x4;			// Address of 4x4 cels
				char*	cel_list16;			// Address of 16x16 cels
				char*	cel_list32;			// Address of 32x32 cels
				char*	cel_game;			// Address of internal game cels
				char*	cel_map512;			// Address of 512x512 map
				char*	cel_map128;			// Address of 128x128 map
				char*	cache_lookup;		// Cache data for created cels
				char*	cache_free;
				long	cache_freecount;
				
				char*	screen_address0;	// Screen addresses
				char*	screen_address1;
				char*	cel_map32;			// Address of 32x32 map
				char*	cel_codedpalette;	// Address of coded6 palette
				char*	free;				// Free
				
				} cel_celdata;


typedef struct alpha_char	{
							long	x_dist;
							long	end_x_pos;
							long	y_dist;
							long	end_y_pos;
							long	size_dist;
							long	end_size;
							char	character;
							char	delay;
							}	alpha_char;
							
// External variables

extern	char*			cel_game;
extern	char*			cel_list32;
extern	char*			cel_list16;
extern	char*			cel_creation;
extern	char*			cel_palette;
extern	char*			cel_codedpalette;
extern	char*			cel_plutdata;
extern	CCB*			cel_plut;
extern	CCB*			cel_plotlist [CEL_MAXTEMP];
extern	char			cels4x4 [16 * CEL_MAX4];
extern	cel_celdata		cel_quad;
extern	char			*map512;
extern	char			*map128;
extern	char			*map32;
extern	char			cache_lookup [64*64 *2];
extern	char			cache_free [CEL_MAXCREATION];
extern	ScreenContext*	screen;
extern	long cosine_table	[ 2048 ] ;
extern	long *sine_table ;

// Function Prototypes

long	cel_initialise(void);										// Initialise cel lists
void	cel_rectangle (long, long, long, long, long, long, long);	// Draw a shaded rectangle
void	cel_paint (long xpos, long ypos, long shade, long cel);		// Immediately draw a cel
void	cel_setgamearea (long);										// Sets memory area for gamecels

void	cel_resetcharactermap (void);								// Reset the char map list
void	cel_setalphaaddmode (long);									// Set new string add mode
void	cel_addalphadelay (long alphadelay);						// Increase delay for char update
long	cel_getalphastringlen (char*);								// Return length of string in pixels
long 	cel_addalphastring (char*, long, long, long);				// Add a string to the list
void	cel_getalphastart (alpha_char*);							// Set a chars start points depending on add mode
void	cel_deleteletters (long);									// Deletes last X chars added to alphalist
long	cel_returnnextcharnumber (void);							// Returns no. of chars in
void	cel_movecharacterby (long, long, long);						// Moves a character
void 	cel_rendercharactermap (long);								// Render char map to cel list

#endif
