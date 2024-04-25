// File : SF_Status
// In-Game Control & Status update Routines

#include "SF_ARMUtils.h"
#include "SF_Bonus.h"
#include "SF_Status.h"
#include "SF_Mission.h"
#include "SF_Message.h"
#include "SF_Palette.h"
#include "SF_Control.h"
#include "Weapons.h"
#include "Stdio.h"
#include "explosion.h"
#include "bonus_control.h"

/**************************************/

void	status_register	(long status_type, long status_set, long status_dec)

// Purpose : Sets a new game status (game over, player crashed etc.)
// Accepts : New Game Status
// Returns : Nothing

{
	// Filter out any 'not' allowed changes, like player dying when mission is completed etc.
	
	if (status.status == STATUS_MISSIONCOMPLETEDWAIT && status_type != STATUS_MISSIONCOMPLETED)
		return;
		
	if (status.status == STATUS_GAMEOVERWAIT && status_type != STATUS_GAMEOVER)
		return;
	
	// Set status
	
	status.status = status_type;				// Set type
	status.status_count = status_set;			// Set counter
	status.status_decrement = status_dec;		// Set decrement value	

	status.display_lives = 0;					// Turn lives display off
}

/**************************************/

void status_updatestatus (void)

// Purpose : Update status (ZOOM IN, MISSION COMPLETED, CRASHED, GAME OVER etc.)
// Accepts : nothing
// Returns : nothing

{
long		camloop;
char		message_string [40];
ship_stack	*ship;

	// ARE WE DISPLAYING LIVES ON SCREEN ?
	
	if (status.display_lives > 0)
		status.display_lives-=1;
		
	// CHECK FOR DEAD CAM EXPIRY
	
	for (camloop = 1; camloop < MAX_CAMERAS; camloop++)	
		if (camera [camloop].counter2 > 0)
			if (--camera [camloop].counter2 == 0)
				control_searchcamera (camloop, camera [camloop].instance);
				
	// DECREMENT GAME STATUS COUNTERS
	
	if (status.status_count >= 0)						// Game status counter
		status.status_count-=status.status_decrement;

	// PICKUP COUNTER ?
	
	if (status.pickup_count > 0)
		status.pickup_count-=4;							// Crystal status counter
	
	// CHAIN REACTION ?
	
	if (status.chain_timer > 0)
		if (--status.chain_timer == 0)
			static_explode ((mission.groundlist [status.chain_next].xpos >> 1) + ((mission.groundlist [status.chain_next].ypos >> 1)<<7), 256);
		
	status.clock1 = (++status.clock1) & 3;				// Fast clock
	status.clock2 = (++status.clock2) & 31;				// Slow clock
	
	if (status.clock3 < 1023)
		status.clock3+=1;								// Frame clock upto 1023
		
	// DO ANY CHECKS ON 3FRAME COUNTER ?
	
	if (status.clock1 == 0)
		{
		if (status.fast_crystals > 0)					// Add a fast-adder crystal ?
			{
			status.fast_crystals-=1;
			bonus_addpickup (1+(arm_random() & 7));
			}
		}
		
	// DO ANY CHECKS ON 15FRAME COUNTER ?
	
	if (status.clock2 == 0)
		{
		
		if (mission.mission_timer >0)					// Has mission timer expired (and was previously set)
			{
			if (--mission.mission_timer <=0)
				{
				message_add (MESSAGE_TOP, (MTXT__YOU_HAVE_RUN_OUT_OF_TIME), WHITE_15, MESSAGE_FLASH, 32, MESSAGE_NOCURSOR);
				control_gameover();
				}
			message_addtimer();
			}
			
		if (status.mega_ship > 0)						// Putting a ship back to normal after megasetup ?
			if (--status.mega_ship <=0)
				bonus_megasetup_cancel ();
		}		

	
	// ADD A RANDOM PARACHUTE ?
	
	if (((arm_random() & 2047) == 2047) && planet_info.space_mission == 0)
		{
		message_add (MESSAGE_TOP, (MTXT__PARACHUTE_DROP_DETECTED), WHITE_15, MESSAGE_FLASH, 48, MESSAGE_NOCURSOR);
		ship = add_ship (	arm_random(),	// Xpos
							arm_random(),	// Ypos
							90<<24,			// Zpos (90 sprites up)
							0,				// Start dir
							PARACHUTE<<4,	// Type
							NULL,			// SDB
							NULL);			// PERFORMANCE
		}
			
	switch (status.status)
		{
		
		// STATUS - ZOOM IN AT START OF NEW GAME ?
		// ---------------------------------------
		
		case STATUS_ZOOMIN :										// INITIAL ZOOM IN OF CAMERA ?
			if (status.status_count<0)
				{
				status_register (STATUS_CONTINUE, 32, 1);
				status.display_lives = 32;
				if (status.current_camera == 0)
					control_changecamera (1);
				}
			else
				if (status.current_camera == 0)
					{
					camera [0].y_rot= -(status.status_count << 10);
					camera [0].zoom =1100 + (status.status_count << 6);
					}
			break;
		
		// STATUS - GAME / NEW LIFE JUST STARTED ?
		// ---------------------------------------
		
		case STATUS_CONTINUE :										// DO GENERAL IN-GAME CHECKS & UPDATES
				
			// HAS DEFENCE TIMER EXPIRED (AND WAS PREVIOUSLY SET)
			if (mission.defence_timer >0)
				if (--mission.defence_timer <=0)
					message_add (MESSAGE_BOTTOM, (MTXT__DEFENCES_ACTIVE), WHITE_15, MESSAGE_FLASH, 32, MESSAGE_NOCURSOR);
			
			// IS 'WAITING TO DOCK' FLAG SET - IF SO, CHECK FOR DOCKED, IF SO, MISSION IS FINISHED
			if (status.flag_waitingtodock == 1)
				if (status.docked != NULL)
					{
						message_add (MESSAGE_BIG, (MTXT__DOCKING_SUCCESSFUL), WHITE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
						message_add (MESSAGE_BOTTOM, (MTXT__MISSION_COMPLETED), WHITE_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
						status_register (STATUS_MISSIONCOMPLETEDWAIT, 64, 1);
					}
					
			// HAVE WE COMPLETED MISSION (WAITING TO DOCK) ?
			if (status.flag_allgrounddead == 0 && status.flag_allshipdead == 0 && status.flag_defendland == 0)
				{
				if (status.flag_waitingtodock == 0)
					{
					message_add (MESSAGE_BIG, (MTXT__MISSION_COMPLETED), WHITE_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
					if (mission.dock_to_finish == 1)
						{
						if (status.docked == NULL)
							message_add (MESSAGE_BOTTOM, (MTXT__RETURN_TO_MOTHERSHIP), WHITE_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
						status.flag_waitingtodock = 1;
						}
					else
						status_register (STATUS_MISSIONCOMPLETEDWAIT, 64, 1);
					}
				}				
		
			// ARE WE DOCKED ? IF SO, SET MOTHERSHIP BACK TO FRIEND
			
			if (status.docked != NULL)
				status.docked->special_data->side = SDB_SIDE_FRIEND;
				
			break;

		// STATUS - MISSION COMPLETED WAIT ?
		// ---------------------------------
		
		case STATUS_MISSIONCOMPLETEDWAIT :
			if (status.status_count < 0)
				status_register (STATUS_MISSIONCOMPLETED, 0,0);
			break;
			
		// STATUS - MISSION COMPLETED - PENDING NEW MISSION ?
		// --------------------------------------------------
		
		case STATUS_MISSIONCOMPLETED :								// PRINT UP MISSION COMPLETED ?
			break;
		
		// STATUS - PLAYER CRASHED - PENDING NEW LIFE ?
		// --------------------------------------------
		
		case STATUS_CRASHED :										// HAS PLAYER CRASHED ?
			if (status.status_count<0)
				{
				status.current_camera=1;
				status_register (STATUS_CONTINUE, 32, 1);
				status.display_lives = 32;
				
				armlink_releaseitem(players_ship , &ships);							// Release the ship for update !
				players_ship->shields=players_ship->performance->shields << 5;		// Reset Shields
				players_ship->damage = DAMAGE_NOTHING;
				players_ship->damage_counter = DAMAGE_NOTHING_COUNTER;
				players_ship->x_vel=0;												// Cancel all velocities
				players_ship->y_vel=0;
				players_ship->z_vel=0;
				players_ship->x_dir = players_ship->x_rot;							// Clear drift & speed
				players_ship->y_dir = 0;
				players_ship->x_control = 0;
				players_ship->y_control = 0;
				players_ship->z_roller = 0;
				players_ship->thrust_control = 0;
				players_ship->command = 0;
				players_ship->speed = 0;
				players_ship->y_rot = 0;											// Make ship level
				players_ship->z_rot = 0;
				players_ship->x_pos = player_crashed_xpos;							// Restore position
				players_ship->y_pos = player_crashed_ypos;
				players_ship->z_pos = player_crashed_zpos + (2<<24);				// Move it up a bit
											
				ship = (ship_stack*) (ships.info).start_address;					// Kill all non players A_T_A Missiles
				while ((ship->header).status == 1)
					{
					if (ship->type == AIR_TO_AIR && (ship_stack*) ship->who_owns_me != players_ship)
						ship->shields=0;
					ship = (ship_stack*) (ship->header).next_address;
					}
				
				control_recheckcamera (players_ship);								// Redo pending cameras ?
				}
			break;
		
		// STATUS - WAITING TO DO GAME OVER ?
		// ----------------------------------
		
		case STATUS_GAMEOVERWAIT :							// ARE WE WAITING TO DO GAME OVER ?
			if (status.status_count<0)
				{
				status.current_camera=1;
				status_register (STATUS_GAMEOVER, 0, 0);
				}
			break;
		}
}

/**************************************/

void	status_groundhit (long xpos, long ypos)

// Purpose : Called when a ground object has been destroyed, decided what to do / update
// Accepts : X & Y map positions
// Returns : Nothing

{

long		object_loop,
			object_type;
	
	// Register ground hit
	
	status.enemy_groundcount+=1;
			
	// Should we cancel a free track for this object ?
	
	if (status.track_type == TRACKING_GROUND)
		if (status.track_xpos == xpos && status.track_ypos == ypos)
			{
			status.track_type = TRACKING_NOTHING;	
			status.track_status = TRACKING_STATUS_FREE;
			}
			
	// Should we award a bonus for destroying this object ?
	
	for (object_loop =0 ; object_loop < mission.groundcount; object_loop++)
		{
		
		object_type = mission.groundlist [object_loop].type;
		if (object_type != GROUND_NULL)
			{
				
			if (mission.groundlist [object_loop].xpos == xpos && mission.groundlist [object_loop].ypos == ypos)
				{
				mission.groundlist [object_loop].type = GROUND_NULL;
				status_awardmissionbonus (object_type, mission.groundlist [object_loop].var, object_loop, 0, xpos, ypos);
				}
			}
		}
}
			
/**************************************/

void status_awardmissionbonus (long object_type, long object_var, long object_loop, long check_type, long gridx, long gridy)

// Purpose : Decodes and awards mission bonuses
// Accepts : Bonus type, Bonus value, whether it was from a ship / vehicle (1) OR ground object (0), xpos+ypos
// Returns : Nothing

{

ship_stack	*ship = (ship_stack*) (ships.info).start_address;
long	gridpos = (gridx + gridy) >> 1;

long	random_awards [8] [3] = {
								{WEAPON_LASER,			MTXT__LASER,			1},	
								{WEAPON_ATG,			MTXT__ATG,				5},
								{WEAPON_ATA,			MTXT__ATA,				5},
								{WEAPON_MEGA_BOMB,		MTXT__MEGABOMB,			5},
								{WEAPON_BEAM_LASER,		MTXT__BEAM_LASER,		25},
								{WEAPON_MULTI_MISSILE,	MTXT__MULTI_MISSILE,	5},
								{WEAPON_MINE,			MTXT__MINES,			5},
								{WEAPON_ECM,			MTXT__ECM,				5}
								};

	// THESE BONUSES ARE FOR GROUND OBJECTS & SHIPS
	
	switch (object_type)
		{
			
		case GROUND_SWEAPONBONUS1 :
			player_performance.weapons [WEAPON_MEGA_BOMB] +=object_var;
			message_add (MESSAGE_BIG, (MTXT__MEGABOMBS_AWARDED), YELLOW_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
			break;
			
		case GROUND_SWEAPONBONUS2 :
			player_performance.weapons [WEAPON_BEAM_LASER] +=object_var;
			message_add (MESSAGE_BIG, (MTXT__BEAM_LASERS_AWARDED), YELLOW_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
			break;
						
		case GROUND_SWEAPONBONUS3 :
			player_performance.weapons [WEAPON_MULTI_MISSILE] +=object_var;
			message_add (MESSAGE_BIG, (MTXT__MULTIMISSILE_AWARDED), YELLOW_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
			break;
						
		case GROUND_SWEAPONBONUS4 :
			player_performance.weapons [WEAPON_MINE] +=object_var;
			message_add (MESSAGE_BIG, (MTXT__MINES_AWARDED), YELLOW_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
			break;
			
		case GROUND_ADDTODEFENCETIME :
				if (object_var == 255)
					{
					mission.defence_timer = 0;
					message_add (MESSAGE_BOTTOM, (MTXT__DEFENCES_ACTIVE), WHITE_15, MESSAGE_FLASH, 32, MESSAGE_NOCURSOR);
					}
				else
					{
					mission.defence_timer+=(object_var*10);
					message_add (MESSAGE_BIG, (MTXT__ENEMY_DEFENCES_DOWN), CYAN_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
					}
				break;
		
		case GROUND_SETSHIPSHIELD :
			while ((ship->header).status == 1)
				{
				if (ship->type == object_var)
					ship->shields = 256;
				ship = (ship_stack*) (ship->header).next_address;
				}
		
		case GROUND_TOGGLETIMER :
			if (mission.mission_timer == 0)
				mission.mission_timer = object_var;
			else
				mission.mission_timer = 0;
						
			message_addtimer();
			break;
		}
		
	// THESE BONUSES ARE FOR JUST GROUND OBJECTS
		
	if (check_type == 0)
		{
		switch (object_type)
			{
		
			case GROUND_DESTROY :
				message_add (MESSAGE_TOP, (MTXT__MISSION_TARGET_DESTROYED), YELLOW_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
				status.flag_allgrounddead-=1;
				break;
		
			case GROUND_HIDDENDESTROY :
				message_add (MESSAGE_TOP, (MTXT__SECRET_MISSION_TARGET_DESTROYED), WHITE_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
				status.flag_allgrounddead-=1;
				break;
			
			case GROUND_BLOWUPPREVIOUS :
				if (object_loop > 0)
					{
					status.chain_next = object_loop-1;			// Set next explosion one to be next in loop
					status.chain_timer = object_var<<1;			// Frame timer before next destroyed
					}
				break;
			
			case GROUND_OBJECTJAMSCANNER :
				if (mission.scanner_bust == 1)
					{
					mission.scanner_bust = 0;
					message_add (MESSAGE_TOP, (MTXT__SCANNER_FREE), WHITE_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
					}
				break;
				
			case GROUND_RANDOMAWARD :
				if ((gridpos & 1) == 1)
					bonus_weapon (random_awards [gridpos & 7] [0], random_awards [gridpos & 7] [1], random_awards [gridpos & 7] [2]);
				else
					start_bonus_crystal_adder (gridx>>1, gridy>>1);
				break;
										
			case GROUND_FEDNETHIT :
				if (mission.hits_allowed >0)
					{
					message_add (MESSAGE_TOP, (MTXT__STRATEGIC_TARGET_DESTROYED), YELLOW_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
					mission.hits_allowed-=1;
					}
				else
					{
					message_add (MESSAGE_TOP, (MTXT__STRATEGIC_COMMAND_OVERRUN), RED_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR2);
					status_register (STATUS_GAMEOVERWAIT, 32, 1);
					}
				break;
			}
		}
}

/**************************************/

	//SetClipHeight (screen->sc_BitmapItems [0], 80);
	//SetClipHeight (screen->sc_BitmapItems [1], 80);
	//SetClipWidth (screen->sc_BitmapItems [0], 80);
	//SetClipWidth (screen->sc_BitmapItems [1], 80);
	//SetClipOrigin (screen->sc_BitmapItems [0], 160-40, 120-40);
	//SetClipOrigin (screen->sc_BitmapItems [1], 160-40, 120-40);
