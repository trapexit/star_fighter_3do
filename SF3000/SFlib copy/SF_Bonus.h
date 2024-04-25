// File : Bonus header file

#ifndef __SF_BONUS
#define __SF_BONUS

// Includes

#include "SF_Mission.h"
#include "ship_struct.h"

// Crystal Definitions

#define	C_NONE		0
#define C_RED		1
#define C_GREEN		2
#define C_YELLOW	3
#define C_BLUE		4
#define C_MAGENTA	5
#define C_CYAN		6
#define C_ORANGE	7
#define C_DARK		8

/**************************************/

// Structure definitions for award crystals

typedef	void	(*CrystalFunction) ();

typedef struct	crystal_award								// Definition of awards structure
				{
				char			award [4];
				CrystalFunction	function;
				long			parameter1;
				long			parameter2;
				long			parameter3;
				} crystal_award;


// External variables

extern	performance_data	player_performance;	
extern	performance_data	player_megaperformance;
extern	ship_stack 			*players_ship;
extern	ship_list			ships;

// Function Prototypes

void	bonus_parachute (long);								// Add a random parachute bonus
void	bonus_addpickup (long	pickup_type);				// Add a crystal pickup

// Function Prototypes for crystal bonus awards - All accept 1 long, 1 char, 1 long as a parameter (from award)

void	bonus_weapon (long, long, long);					// Award weapon upgrade
void	bonus_wingpod (long, long, long);					// Award wingpod
void	bonus_performance (long, long, long);				// Award engine / control / shields
void	bonus_fastcrystal (long, long, long);				// Award fast adder crystals

void	bonus_megasetup_start (long, long, long);			// Award megaship for a certain ammount of time
void	bonus_megasetup_cancel (void);						// Reset player from megaship
void	bonus_deletewingpods (void);						// Routine to delete all wingpods

void	bonus_downgrade_ship (long damage_rating);			// Picks a thing randomly & downgrades it, displays message etc.
#endif
