//	File : SF_ARMUtils (Header file for utility routines)

#ifndef __SF_ARMUTILS
#define __SF_ARMUTILS

// Function Prototypes

extern	void	arm_randominit (void);							// Reset random seed values
extern	long	arm_random (void);								// Get 32bit random value
extern	long	arm_randomvalue (long);							// Returns random number between 0 & passed value (inclusive)
extern	void	arm_drawmaptargets (void*, long, void*, long);	// Draws all map objects

#endif
