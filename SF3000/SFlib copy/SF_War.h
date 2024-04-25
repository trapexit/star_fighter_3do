/* File : War header file */

#ifndef __SF_WAR
#define __SF_WAR

// Includes

#include "SF_Mission.h"
#include "SF_Status.h"
#include "Ship_Struct.h"
#include "Ship_Control.h"
#include "Misc_Struct.h"

#include "Stdio.h"

/**************************************/

// Strategy Definitions

#define	__WAR_FS			22				// Scale value for formation distances from player
#define __WAR_MFF			15				// Max number of friendly fire's before mutiny
#define __WAR_TC			24				// No. of frames before multiple target re-allocation
#define __WAR_SR_LOCAL		40 << 24		// Max search distance (Local) for opponent targets (in any direction, +x, -x, etc.)
#define __WAR_SR_AREA		127 << 24		// Max search distance (Wide area) for opponent targets
#define	__WAR_MF			8				// Max number of people in formation (including null entry for player)
#define	__WAR_MDC_FRIEND	4				// Max search distance (in sprites) [against] player distance check	FRIENDLY
#define	__WAR_MDC_ENEMY		8				// Max search distance (in sprites) [against] player distance check	ENEMY

/**************************************/

// Structure definitions for formation

typedef struct	formation_ship					// Structure for an individual formation position
				{
				ship_stack	*occupied;			// Is this formation position occupied ? - if not NULL, then address of ship
				long		x_offset,			// X position from player
							y_offset,			// Y position from player
							z_offset;			// Z position from player
				} formation_ship;
				
/**************************************/

// Definitions for Ships Special DataBlock


#define	SDB_MODE_ATTACK				0			// Ship control modes
#define SDB_MODE_FLIGHTPATH			1
#define SDB_MODE_FORMATION 			2

#define SDB_SHIP_DEAD				0			// Ship alive/dead status
#define SDB_SHIP_ALIVE				1

#define SDB_SIDE_ENEMY				0
#define SDB_SIDE_FRIEND				1

#define	SDB_KILL_NULL				0			// Ship kill status
#define SDB_KILL_MUSTDIE			1
#define	SDB_KILL_MUSTSURVIVE		2
#define SDB_KILL_MUSTSURVIVEFLIGHT	3
#define SDB_KILL_MUSTSTOPFLIGHT		4

#define SDB_INCOMING_RESPOND		0			// Responses to incoming fire
#define	SDB_INCOMING_IGNORE			1
#define SDB_INCOMING_KEEPFLIGHTPATH	2

#define	SDB_NO_DISTANCECHECK		0			// Distance checks with other aircraft
#define	SDB_DO_DISTANCECHECK		1

#define SDB_CLOAKING_OFF			0			// Cloaking device
#define SDB_CLOAKING_ON				1

#define	SDB_TYPE_NORMAL				0			// Type of SDB
#define	SDB_TYPE_PLEB				1
#define	SDB_TYPE_PLAYER				2

#define SDB_TARGETTYPE_NORMAL		0			// Type of targeting system
#define	SDB_TARGETTYPE_MULTIPLE		1

#define	SDB_FORMATION_NORMAL		0			// Formation Control Modes
#define	SDB_FORMATION_AGGRESSIVE	1
#define	SDB_FORMATION_DEFENSIVE		2

// External Definitions

extern	ship_list		ships;
extern	ship_stack 		*players_ship ;
extern	mission_data	mission;
extern	game_status		status;	
extern	docking_struct	docked ;

// Function Prototypes

void	war_addmissionships (performance_data*);							// Add all ships on mission start
void	war_updatemissionships (void);										// Update all mission ships
void	war_updatemissionship (ship_stack*, ship_sdb*);						// Update a particular mission ship

void	war_updateship_attack (ship_stack*, ship_sdb*);						// Update a ship in ATTACK MODE
void	war_updateship_flightpath (ship_stack*, ship_sdb*);					// Update a ship in FLIGHTPATH MODE
void	war_updateship_formation (ship_stack*, ship_sdb*);					// Update a ship in FORMATION MODE

void	war_update_attacked_friend (ship_stack*, ship_sdb*);				// Update a friendly ship that has been fired upon
void	war_update_attacked_enemy (ship_stack*, ship_sdb*);					// Update an enemy ship that has been fired upon

long	war_update_defend_leader (ship_stack*, ship_sdb*, ship_stack*);		// See if a ship should defend it's formation leader
long	war_settoattack (ship_stack*, ship_stack*);							// Update status of a ship that has been fired upon
void	ship_settoflightpoint (ship_stack*, ship_sdb*);						// Set a ship's gotoxyz from it's flightpath point
void	war_update_multipleattackrequests (ship_stack*, ship_sdb*);			// Fire weapons / Acquire new targets for Multiple targetting ship
long	ship_setmultipletargets (ship_stack*, long, char);					// Acquire new target(s) for ship within search range

long	war_setformationposition (ship_stack*, char, long);					// Put a ship in formation with the player at a fixed pos
long	war_addshiptoformation (ship_stack *formation_ship, long);			// Put a ship in formation with the player at any pos
void	war_squashformationup (void);										// Squash to the players formation

void	war_distancecheckwithplayer (ship_stack*, ship_sdb*);				// Check distance of ship against player, take action if within range

#endif

