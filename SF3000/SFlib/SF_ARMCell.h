//	File : SF_ArmCell (Header file for cel / polygon drawing & manipulation routines)

#ifndef __SF_ARMCELL
#define __SF_ARMCELL


// Definitions

#define	CHANGE_FLAG_CLEAR	0
#define	CHANGE_FLAG_SET		1
#define	CHANGE_PIXC_CLEAR	2
#define	CHANGE_PIXC_SET		3


// Function Prototypes

extern void arm_addpolycel16 (void* , long);				// Add a 16x16 textured polygon
extern void arm_addpolycel32 (void* , long);				// Add a 4x4, 8x8, 16x16, 32x32 textured polygon
extern void arm_setpolycel32palette (void*, long);			// Change palette ptr of last added cel to another 32's one
extern void	arm_addmonocel (void*, long, long, long);		// Add a monochrome polygon
extern void	arm_addgamecel (void*, long, long, long);		// Add a game sprite to list
extern void arm_setgamecelpalette (void*, long);			// Set last added gamesprite PLUTptr

extern void arm_add4cel4 (void*, void*, long, long);		// Create a 16x16 polygon from the map
extern void arm_celinitialisecreation (void*, long);		// Initialise preambles in 'created' cels
extern void arm_initialisecache (void*);					// Initialise cache
extern void arm_updatecache (void*);						// Update cache frame counters

extern void arm_generatemaps (void*, void*);				// Initialise the large scale maps
extern void arm_addcelfrom512map (void*, long, long, long);	// Add a sub-cel from the 512x512 map
extern void arm_addcelfrom128map (void*, long, long, long);	// Add a sub-cel from the 128x128 map
extern void arm_addcelfrom32map (void*, long, long, long);	// Add a sub-cel from the 32x32 map

extern void arm_setcel_hw (void*, long, long);				// Alter last added game cels height / width

#endif
