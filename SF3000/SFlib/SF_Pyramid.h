/* File : Pyramid header file */

#ifndef __SF_PYRAMID
#define __SF_PYRAMID

// Includes

#include "SF_CelUtils.h"

// Definitions

#define	MAX_MISSIONS		15					// Max number of missions per level
#define	MISSION_UNFINISHED	0					// Mission unfinished flag
#define	MISSION_FINISHED	1					// Mission finished flag

#define	PYRAMID_START		0					// Selection has been made
#define	PYRAMID_CONTINUE	1					// No Selection has been made yet
#define	PYRAMID_QUIT		2					// Player quitted pyramid

// Icon definitions for gameset 'Pyramid'

#define BASECEL_PYRAMID_PLANETS		0
#define	BASECEL_PYRAMID_POINTER		32
#define	BASECEL_PYRAMID_NUMBERS		36
#define	BASECEL_PYRAMID_COMPLETED	51
#define	BASECEL_PYRAMID_LEVEL		56

// Mission description structure definition
				
typedef struct	pyramid							// Pyramid structure data
				{
				long	sub_levels;				// Max no. of sub-levels in pyramid
				long	y_level;				// Current sub-level number
				long	x_level;				// Current mission in sub-level
				char	mission [MAX_MISSIONS];	// Array of missions in pyramid
				}	pyramid;


// External Definitions

extern cel_celdata			cel_quad;
extern long cosine_table	[2048];
extern long *sine_table;
extern long cheat_feature1;

// Function Prototypes

void	pyramid_initialiselevel (long);			// Clear a pyramid level
void	pyramid_initialisegame (void);			// Clear all pyramid levels
void	pyramid_initialiseselect (long);		// Initialise selection on a pyramid
long	pyramid_read (long, long);				// Update a selection on a pyramid
void	pyramid_showmission (void);				// Display mission description
long	pyramid_registercompleted (void);		// Register a mission as completed


#endif

