// File : SF_Bonus.c
// Bonus Award Control Routines

#include "SF_Bonus.h"
#include "SF_Status.h"
#include "SF_Message.h"
#include "SF_Palette.h"
#include "SF_Sound.h"
#include "SF_ARMUtils.h"
#include "Ship_Struct.h"
#include "Weapons.h"

#include <String.h>
#include <Stdio.h>

// External definitions

extern	game_status			status;	
char	crystal_string		[40];

/************************************************************************************************************************************
*													CRYSTAL AWARD DEFINITIONS														*
************************************************************************************************************************************/

// C_NONE		0
// C_RED		1
// C_GREEN		2
// C_YELLOW		3
// C_BLUE		4
// C_MAGENTA	5
// C_CYAN		6
// C_ORANGE		7
// C_DARK		8

// Definitions

#define	MAX_AWARDS 21
#define	MAX_NICEAWARDS 18

// Global variables

long	damage_variables [11] [2] =	{
									{WEAPON_LASER,			MTXT__LASER},
									{WEAPON_ATG,			MTXT__ATG},
									{WEAPON_ATA,			MTXT__ATA},
									{WEAPON_MEGA_BOMB,		MTXT__MEGABOMB},
									{WEAPON_BEAM_LASER,		MTXT__BEAM_LASER},
									{WEAPON_MULTI_MISSILE,	MTXT__MULTI_MISSILE},
									{WEAPON_MINE,			MTXT__MINES},
									{0,						MTXT__SHIELDS},
									{0,						MTXT__ENGINE},
									{0,						MTXT__CONTROL},
									{0,						MTXT__ECM}
									};
																		
crystal_award	crystal_awards [MAX_AWARDS] = 
{

// PERFORMANCE UPGRADES (NORMAL)

{0,			C_GREEN,	C_YELLOW,	C_GREEN,	bonus_wingpod,	1,						0,					0},		// Add 1 WINGPOD
{C_CYAN,	C_RED,		C_CYAN,		C_RED,		bonus_wingpod,	2,						0,					0},		// Add 2 WINGPODS

// WEAPONS UPGRADES (NORMAL)

{0,			0,			C_RED, 		C_RED,		bonus_weapon,	WEAPON_LASER,			MTXT__LASER,		1},		// Increase LASER
{0,			0,			C_BLUE, 	C_BLUE,		bonus_weapon,	WEAPON_ATG,				MTXT__ATG,			10},	// Increase ATG
{0,			0,			C_MAGENTA,	C_MAGENTA,	bonus_weapon,	WEAPON_ATA,				MTXT__ATA,			10},	// Increase ATA
{C_MAGENTA,	C_RED,		C_MAGENTA,	C_RED,		bonus_weapon,	WEAPON_MEGA_BOMB, 		MTXT__MEGABOMB,		15},	// Increase MEGABOMB
{C_MAGENTA,	C_GREEN,	C_MAGENTA,	C_GREEN,	bonus_weapon,	WEAPON_BEAM_LASER,		MTXT__BEAM_LASER,	250},	// Increase BEAM LASER
{C_MAGENTA,	C_BLUE,		C_MAGENTA,	C_BLUE,		bonus_weapon,	WEAPON_MULTI_MISSILE,	MTXT__MULTI_MISSILE,10},	// Increase MULTIMISSL
{C_MAGENTA,	C_ORANGE,	C_MAGENTA,	C_ORANGE,	bonus_weapon,	WEAPON_MINE,			MTXT__MINES,		10},	// Increase MINES
{0,			0,			C_BLUE,		C_GREEN,	bonus_weapon,	WEAPON_ECM,				MTXT__ECM,			5},		// Increase ECM's

// WEAPONS UPGRADES (ADDED)

{0,			0,			C_CYAN,		C_YELLOW,	bonus_weapon,	WEAPON_MULTI_MISSILE,	MTXT__MULTI_MISSILE,3},		// Increase MULTIMISSL
{C_CYAN,	C_GREEN,	C_CYAN,		C_BLUE,		bonus_weapon,	WEAPON_ECM,				MTXT__ECM,			50},	// Increase ECM's

// SHIP UPGRADES (NORMAL)

{0,			0,			C_YELLOW,	C_YELLOW,	bonus_performance,		1,				MTXT__SHIELDS,		0},		// Shields Upgrade
{0,			0,			C_BLUE,		C_YELLOW,	bonus_performance,		1,				MTXT__ENGINE,		0},		// Engine Upgrade
{0,			0,			C_RED,		C_YELLOW,	bonus_performance,		1,				MTXT__CONTROL,		0},		// Control Upgrade

// SPECIAL BONUSES

{0,			C_CYAN,		C_CYAN,		C_DARK,		bonus_fastcrystal,		10,				0,					0},		// Fast adder crystals
{C_DARK,	C_DARK,		C_DARK,		C_DARK,		bonus_fastcrystal,		20,				0,					0},		// Fast adder crystals
{C_DARK,	C_DARK,		C_DARK,		C_RED,		bonus_megasetup_start,	40,				0,					0},		// Megaship !

// SHIP DAMAGE (NORMAL)

{0,			C_MAGENTA,	C_YELLOW,	C_BLUE,		bonus_performance,		-1,				MTXT__SHIELDS,		0},		// Shields Damage
{0,			C_MAGENTA,	C_RED,		C_BLUE,		bonus_performance,		-1,				MTXT__ENGINE,		0},		// Engine Damage
{0,			C_MAGENTA,	C_GREEN,	C_BLUE,		bonus_performance,		-1,				MTXT__CONTROL,		0},		// Control Damage

};

/************************************************************************************************************************************
*														CRYSTAL UPDATE CODE															*
************************************************************************************************************************************/

void	bonus_parachute (long display_message)

// Purpose : Picks a random bonus from the list
// Accepts : Flag to display message (1) or not
// Returns : Nothing

{

long	award = arm_randomvalue (MAX_NICEAWARDS-1);		// Get random nice award

	crystal_awards [award].function (crystal_awards [award].parameter1, crystal_awards [award].parameter2, crystal_awards [award].parameter3);
	
	if (display_message == 1)
		message_add (MESSAGE_BOTTOM, (MTXT__PARACHUTE_COLLECTED), CYAN_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
	
	message_addweapon ();
}

/**************************************/

void	bonus_addpickup (long	pickup_type)

// Purpose : Add a bonus pickup to the list
// Accepts : Pickup type
// Returns : Nothing


{
long	pickup_loop,
		award_check,
		pickup_status,
		crystal_shift;
		
	// SHIFT CRYSTALS DOWN, ADD NEW CRYSTAL AND SET TIMER
	
	for (pickup_loop=0; pickup_loop<3; pickup_loop++)
		status.pickups [pickup_loop] = status.pickups [pickup_loop+1];
	status.pickups [3] = (char) pickup_type;
	status.pickup_count = 16;

	// TEST FOR BONUS AWARD
	
	for (award_check=0; award_check < MAX_AWARDS; award_check++)
		{
		pickup_loop=3;
		pickup_status=0;
		
		// TEST FOR A PARTICULAR AWARD
		
		do
			{
			
			if (crystal_awards [award_check].award [pickup_loop] == 0)
				{
				pickup_status = 1;
				}			
			else
				{
				if (crystal_awards [award_check].award [pickup_loop] != status.pickups [pickup_loop])
					pickup_status=2;
				}
				
			if (--pickup_loop < 0 && pickup_status == 0)
				pickup_status=1;
			}
		while (pickup_status == 0); 


		if (pickup_status != 2)		// IS THIS OUR AWARD ?
			{
						
			// SHIFT UP CRYSTALS
			
			for (crystal_shift = 3; crystal_shift >= (3-(pickup_loop+1)); crystal_shift--)
				{
				status.pickups [crystal_shift] = status.pickups [crystal_shift - (2-pickup_loop)];
				}
			
			// DELETE BOTTOM CRYSTALS
			
			for (crystal_shift = 0; crystal_shift < (2-pickup_loop); crystal_shift++)
				{
				status.pickups [crystal_shift] = 0;
				}
				
			// CALL CRYSTAL AWARD FUNCTION AND CLEAR TIMER
			
			if (crystal_awards [award_check].function (	crystal_awards [award_check].parameter1, crystal_awards [award_check].parameter2,
													crystal_awards [award_check].parameter3) == 1)
													
				{
				message_add (MESSAGE_BOTTOM, (MTXT__CRYSTAL_AWARD), CYAN_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
				message_addweapon ();
				}
				
			return;
			}
			
		else
		
			{
			sound_playsample (SOUND_PICKUP, 60, 127, 0);	// Just do pickup sound
			}
		}
}

/************************************************************************************************************************************
*													CRYSTAL BONUS AWARD ROUTINES													*
************************************************************************************************************************************/

long	bonus_weapon (long type, long mttx_weapontextnumber, long value)

// Purpose : Increases players weapon power by value
// Accepts : Nothing
// Returns : Bonus added / not flag

{

	if (type == WEAPON_LASER && player_performance.weapons [type] >=7)
		return (0);
		
	sprintf (crystal_string, "%s %s : %d", message_decode(mttx_weapontextnumber, 0), message_decode (MTXT__UPGRADE, 0), value);
	message_text (MESSAGE_BIG, crystal_string, YELLOW_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
	player_performance.weapons [type]+=value;

	// CLIP LASERS TO MAX
	
	if (type == WEAPON_LASER && player_performance.weapons [type] >7)
		player_performance.weapons [type]=7;

	return (1);
}

/**************************************/

long	bonus_wingpod (long	value, long nothing1, long nothing2)

// Purpose : Increases players wingpods by value
// Accepts : Nothing
// Returns : Bonus added / not flag


{
long wing_pod_count = 0 ;
	
	if ( (players_ship->ref & POD_4) != 0 ) wing_pod_count += 1 ;		// Check how many of the pod slots are free
	if ( (players_ship->ref & POD_3) != 0 ) wing_pod_count += 1 ;
	if ( (players_ship->ref & POD_2) != 0 ) wing_pod_count += 1 ;
	if ( (players_ship->ref & POD_1) != 0 ) wing_pod_count += 1 ;

	if (wing_pod_count >=4)												// if wing_pod_count = 4 then no room for any more pods
		return (0);

	message_add (MESSAGE_BIG, (MTXT__WINGPOD_ADDED), YELLOW_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
	while (value-- > 0 && wing_pod_count <4)
		{
		add_wing_pod (players_ship);
		wing_pod_count++;
		}
		
	return (1);
}

/**************************************/

long	bonus_performance (long	value, long mtxt_bonustype, long nothing2)

// Purpose : Increases players engine / control / shields by value clipping at max / min
// Accepts : Nothing
// Returns : Nothing

{

long	*item_pointer;
char	item_message [32];

	// Decide which parameter to adjust
		
	switch (mtxt_bonustype)
		{
		case MTXT__ENGINE :
			item_pointer = &player_performance.engine;		// Get correct pointer for performance adjust
			break;
			
		case MTXT__CONTROL :
			item_pointer = &player_performance.control;
			break;
			
		case MTXT__SHIELDS :
			item_pointer = &player_performance.shields;
			break;
		}
		
	if (*item_pointer <=1 && value < 0)						// If we are reducing performance & already on min, return
		return (0);
	
	if (*item_pointer >=7 && value > 0)						// If we are increasing performance & already on max, return
		return (0);
		
	// Print message
	
	sprintf (item_message, "%s %s\n", message_decode (mtxt_bonustype, 0), (value < 0) ? message_decode (MTXT__DAMAGED, 0) : message_decode (MTXT__UPGRADE, 0));
	message_text (MESSAGE_BIG, item_message, YELLOW_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
	
	*item_pointer+=value;	// Adjust performance parameter
	
	if (*item_pointer <1)	// Check for min value
		*item_pointer = 1;
		
	if (*item_pointer >7)	// Check for max value
		*item_pointer = 7;

	if (players_ship->shields > (player_performance.shields << 5))		// Check for limiting shields
		players_ship->shields = (player_performance.shields << 5);

	return (1);
}

/**************************************/

long	bonus_fastcrystal (long	value, long nothing1, long nothing2)

// Purpose : Sets status to award 20 crystals randomly
// Accepts : Nothing
// Returns : Bonus Added


{
	status.fast_crystals = value;
	return (1);
}

/**************************************/

long	bonus_megasetup_start (long value, long nothing1, long nothing2)

// Purpose : Cancels megaship setup
// Accepts : Nothing
// Returns : Bonus Added

{
long		loop;
ship_stack	*ship = (ship_stack*) (ships.info).start_address;

	// printf("BONUS MEGASETUP START CALLED - ADDING %d\n", value);
	
	message_add (MESSAGE_BIG, (MTXT__MEGASHIP_AWARDED), YELLOW_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
	
	if (players_ship->performance == &player_performance)				// Only do this if using normal setup
		{
			
		players_ship->performance = &mission.performances [0];			// Use performance block in missions file (with megasetup)
		memcpy (&mission.performances [0], &player_megaperformance, sizeof (performance_data));		// Reset mega performance data
		
		player_performance.weapons [WEAPON_WING_POD] = 0;				// Count up how many wingpods the player had
		while ((ship->header).status == 1)								// Delete all players wingpods
			{
			if (ship->type == WING_POD)
				player_performance.weapons [WEAPON_WING_POD]+=1;
			ship = (ship_stack*) (ship->header).next_address;
			}
			
		for (loop=0; loop < 4; loop++)
			add_wing_pod (players_ship);										// Give 4 wingpods
		
		players_ship->shields = (players_ship->performance->shields) << 5;		// Give max shields to mega performance setting
		status.mega_ship = value;												// Set status mega counter to <value x 15frames> 
		}
		
	return (1);
}

/**************************************/

void	bonus_megasetup_cancel (void)

// Purpose : Cancels megaship setup
// Accepts : Nothing
// Returns : Nothing

{
long		loop;

	// printf("BONUS MEGASETUP CANCEL CALLED\n");
	
	players_ship->performance = &player_performance;							// Use proper performance datablock
	bonus_deletewingpods();														// Delete ships wingpods
	for (loop=0; loop < player_performance.weapons [WEAPON_WING_POD]; loop++)	// Put back original number of wingpods
		add_wing_pod(players_ship);
	players_ship->shields = (players_ship->performance->shields) << 5;			// Give max shields to original performance setting

	message_addweapon ();														// Restore weapon number display
}

/**************************************/

void	bonus_deletewingpods (void)

// Purpose : Deletes all wingpods from players ship
// Accepts : Nothing
// Returns : Nothing

{
ship_stack	*ship = (ship_stack*) (ships.info).start_address;
ship_stack	*ship_next,
			*other_ship;

	while ((ship->header).status == 1)											// Delete all players wingpods
		{
		if (ship->type == WING_POD)
			{
			
			other_ship = (ship_stack*) ship->who_owns_me ;						// If so then update the owners pointer things
			other_ship->ref = ((other_ship->ref)&(~ship->ref)) ;
		
			ship_next = (ship_stack*) (ship->header).next_address;
			armlink_deleteitem (ship, &ships);
			ship = ship_next;
			}
		else
			ship = (ship_stack*) (ship->header).next_address;
		}
}

/**************************************/

void	bonus_downgrade_ship (long damage_rating)

// Purpose : Causes damage to the players ship
// Accepts : Damage rating (0-3) - 3 is max damage
// Returns : Nothing

{

char	item_message [32];

long	stat_todamage,
		minimum,
		takeoff,
		original_value,
		*stat_pointer;


	stat_todamage = arm_randomvalue (10);		// Decide which to damage

	switch (stat_todamage)						// Set pointer to correct value to reduce
		{
		case 0 :
		case 1 :
		case 2 :
		case 3 :
		case 4 :
		case 5 :
		case 6 :
		case 7 :
			stat_pointer = &player_performance.weapons [damage_variables [stat_todamage] [0]];
			break;
		
		case 8 :
			stat_pointer = &player_performance.shields;
			break;
			
		case 9 :
			stat_pointer = &player_performance.engine;
			break;
			
		case 10 :
		default :
			stat_pointer = &player_performance.control;
			break;
		}
		
	original_value = *stat_pointer;
	minimum = (stat_todamage <=6) ? 0 : 1;
	takeoff = (stat_todamage <=6) ? arm_randomvalue ((*stat_pointer) >> (3-damage_rating)) : damage_rating>>1;
		
	if (takeoff <=0 || *stat_pointer <= minimum)	// Check we are actually taking something off & there's something to take off
		return;
			
	*stat_pointer -= takeoff;						// Reduce statistic
	if (*stat_pointer < minimum)					// Check for minumum
		*stat_pointer = minimum;
			
	if (*stat_pointer == original_value)			// If clipped & same as original value, return
		return;
		
	// Do damage text (only if still alivd)
	
	if (players_ship->shields > 0)
		{
		sprintf (item_message, "%s %s\n", message_decode (damage_variables [stat_todamage] [1], 0), message_decode (MTXT__DAMAGED, 0));
		message_text (MESSAGE_BIG, item_message, YELLOW_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
		}
		
	if (players_ship->shields > (player_performance.shields << 5))		// Check for limiting shields
		players_ship->shields = (player_performance.shields << 5);
}
