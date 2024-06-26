// File : SF_Control
// Game Control Routines

#include "SF_Control.h"
#include "SF_Message.h"
#include "SF_Palette.h"
#include "SF_ARMUtils.h"
#include "SF_Sound.h"
#include "SF_Music.h"
#include "SF_ScreenUtils.h"
#include "SF_Io.h"
#include "SF_Utility.h"
#include "SF_Access.h"
#include "SF_NVRam.h"
#include "Ship_Struct.h"
#include "Setup_Tables.h"
#include "Weapons.h"
#include "Camera_Struct.h"
#include <String.h>
#include <Event.h>

/**************************************/

// Global variables

long	player_crashed_xpos,		// To remember where player was when they died / crashed (for reset in status.c)
		player_crashed_ypos,
		player_crashed_zpos;
		


char	weapon_names [] = {	MTXT__LASER,
							MTXT__ATG,
							MTXT__ATA,
							MTXT__MEGABOMB,
							MTXT__BEAM_LASER,
							MTXT__MULTI_MISSILE,
							MTXT__MINES
							};
							
/**************************************/

void control_initialisecameras (void)

// Purpose : Initialises camera settings when game first run
// Accepts : Nothing
// Returns : Nothing

{
	camera [1].x_rot=0;
	camera [1].y_rot=0;
	camera [1].zoom =1100;
	camera [1].type = CAMERA_NORMAL;
	camera [1].view = 0;
	camera [1].view_ship = NULL;
	
	camera [2].x_rot=0;
	camera [2].y_rot=0;
	camera [2].zoom =1100;
	camera [2].type = CAMERA_FLYBY;
	camera [2].view = 0;
	camera [2].view_ship = NULL;
}


/**************************************/

void	control_startgame (long nothing)

// Purpose : Initialises players status & performance at the start of a new game
// Accepts : Nothing
// Returns : Nothing

{
	memset (&status, 0, sizeof (game_status));												// Reset game status block
	memcpy (&player_performance, &player_resetperformance, sizeof (performance_data));		// Reset players performance data
	message_addscore (0);																	// Initialise score
	
	status.lives = (cheat_feature2 == 1) ? 99 : 3;											// Set lives																		// Set no. of lives to 3
	pyramid_initialisegame ();																// Reset pyramids
	parameters.level = 0;																	// Reset level
}

/**************************************/

void	control_startmission (long demo_mode)

// Purpose : Resest SOME players stats at the start of a new mission (position etc.)
// Accepts : Nothing
// Returns : Nothing

{
long	object_loop;

	// Initialise routines, tables etc. for mission

	arm_randominit ();										// Reset random
	//armanim_initialise (animate);							// Initialise animations for mission [TAKEN OUT]
	arm_initialisecache (&cel_quad);						// Initialise plotting cache
	message_initialise (demo_mode);							// Reset all messages
	
	status.flag_allshipdead =0;								// Reset mission flags & counters
	status.flag_defendland =0;
	status.flag_waitingtodock =0;
	status.friendly_fire =0;
	status.enemy_aircount = 0;
	status.enemy_groundcount = 0;
	status.hits_taken = 0;
	status.track_status = TRACKING_STATUS_FREE;				// Set tracking mode to zero
	status.track_type = TRACKING_NOTHING;
	status.clock3 = 0;
	status.formation_mode = SDB_FORMATION_NORMAL;			// Reset formation control mode to normal
	
	// Add all ships, including players (with required performance datablock)
	
	if (mission.special_setup == 0)
		war_addmissionships (&player_performance);			// Using normal setup ?
	else
		war_addmissionships (&mission.performances [0]);	// Or players setup ?

	// Initialise temp camera for zoom in
	
	camera [0].x_rot=0;
	camera [0].y_rot=0;
	camera [0].zoom =1100;
	camera [0].type = 0;
	camera [0].view = 0;
	camera [0].view_ship = players_ship;
	
	if (camera [1].view_ship == NULL)						// Check for unset cameras ? if so, look at players ship
		camera [1].view_ship = players_ship;
	
	if (camera [2].view_ship == NULL)
		camera [2].view_ship = players_ship;
	
	// Reset status block
	
	status.weapon = WEAPON_LASER;							// Reset selected weapon
	status.current_camera=0;								// Reset camera number
	status_register	(STATUS_ZOOMIN, 256, 8);				// Set game status to ZOOM IN
	
	// Total up ammount of ground objects to destroy (hidden and normal)
	
	status.flag_allgrounddead=0;
	for (object_loop =0 ; object_loop < mission.groundcount; object_loop++)
		if (mission.groundlist [object_loop].type == GROUND_DESTROY || mission.groundlist [object_loop].type == GROUND_HIDDENDESTROY)
			status.flag_allgrounddead+=1;
	
	// Just Docking ???
		
	if (status.flag_allgrounddead == 0 && status.flag_allshipdead == 0 && status.flag_defendland == 0)
		status.flag_waitingtodock =1;
	
	// Call Hutch's mission setup routine
	
	setup_tables ();													// Initialise game tables for mission
	
	for (object_loop = 1; object_loop < MAX_CAMERAS; object_loop++)		// Re-init cameras ?
		control_searchcamera (object_loop, camera [object_loop].instance);
}

/**************************************/

void	control_registerdeath (ship_stack *dead_ship)

// Purpose : A ship has died ! Is it the player ? Is it something we were looking at ?
// Accepts : Pointer to the dead ship
// Returns : Nothing

{

long	camloop;
char	killstatus;

		
		
		// WERE WE TRACKING THIS SHIP ? (FREE OR OTHERWISE ?)
		// IF SO, CANCEL TRACKING
		
		// IS THIS A DEMO MISSION & WAS IT A SMALL SHIP THAT THREW A 7 ?
		//		YES - RESET IT'S SHIELDS AND MOVE IT TO NEW LOCATION
		//		NO	- 
		//			IS IT THE PLAYERS SHIP ?
		//				YES - JUST SUSPEND IT FROM LIST
		//				NO	- DELETE IT FROM THE LINKED LIST
		
		// DID THE SHIP THAT DIED HAVE A SPECIAL DATA BLOCK - IF SO :
		
		// WAS IT FRIENDLY - DO DEATH MESSAGE IF SO
		
		//		DID IT HAVE A PLEB'S SDB ?
		//			1. PRINT 'ENEMY FIGHTER DESTROYED'
		
		//		DID IT HAVE A NORMAL SDB ?
		//			1. SET IT'S SDB FLAG TO DEAD
		//			2. IF IT WAS IN A FORMATION POSITION, CLEAR IT
		//			3. DO CHECKS TO SEE IF IT WAS ONE WE HAD TO PROTECT, OR HAD TO DIE, ETC.

	// ARE WE DOCKED IN THE SHIP THAT HAS JUST BLOWN UP ? IF SO, SET STATUS.DOCKED TO CLEAR
	
	if (status.docked == dead_ship)
		status.docked = NULL;
		
	// IS IT A SHIP ONE OF THE CAMERAS WAS LOOKING AT ?
	// IF SO, START LOOKING AT ANOTHER ONE (OR THE PLAYERS SHIP IF NONE LEFT OF THAT TYPE)
	
	for (camloop = 1; camloop < MAX_CAMERAS; camloop++)
		if (camera [camloop].view_ship == dead_ship)
			{
			camera [camloop] . counter2 = 37;
			camera [camloop] . x_pos = dead_ship->x_pos;
			camera [camloop] . y_pos = dead_ship->y_pos;
			camera [camloop] . z_pos = dead_ship->z_pos;
			}
	
	// WAS TRACKING LOOKING AT THIS SHIP ?
	
	if (status.track_type == TRACKING_SHIP)
		if (status.ship_tracking == dead_ship)
			{
			status.track_type = TRACKING_NOTHING;
			status.track_status = TRACKING_STATUS_FREE;
			status.ship_tracking = NULL;
			}

	// IF IT'S A SMALL SHIP & THIS IS DEMO, RE-INCARNATE IT
	
	if (mission.demo_mission == 1 && dead_ship->type >>4 == SMALL_SHIP)
		{
		dead_ship->shields = (dead_ship->performance->shields)<<5;	// Yes - reset shields
		dead_ship->damage = DAMAGE_NOTHING;							// Yes - reset damage
		dead_ship->damage_counter = DAMAGE_NOTHING_COUNTER;
		dead_ship->x_pos = arm_random();							// Yes - set new X pos
		dead_ship->y_pos = arm_random();							// Yes - set new Y pos
		dead_ship->z_pos = (50 + (arm_random() & 15)) << 24;		// Yes - set new Z pos												// Yes - return
		return;
		}

	else
	
		{
		
		// This is not a demo mission, so suspend or delete it
	
		if (dead_ship == players_ship)
			armlink_suspenditem (dead_ship, &ships);
		else
			armlink_deleteitem (dead_ship, &ships);
		}
		
	// Now do the rest
	
	if (dead_ship->special_data != NULL)
		{		

		if (dead_ship->special_data->side == SDB_SIDE_ENEMY)
			{
			status.enemy_aircount+=1;
			}
		else
			{
			if (dead_ship->special_data->pilot_name != 0)
				message_pilot (-1, -1, dead_ship->special_data->pilot_name);
			}
			
		// PLEB SHIP ?
		
		if (dead_ship->special_data->sdb_type == SDB_TYPE_PLEB)
			message_add (MESSAGE_TOP, (MTXT__ENEMY_FIGHTER_DESTROYED), YELLOW_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR1);
		
		// NORMAL MISSION SHIP ?
		
		if (dead_ship->special_data->sdb_type == SDB_TYPE_NORMAL)
			{

			dead_ship->special_data->alive = SDB_SHIP_DEAD;
			if (dead_ship->special_data->formation_position != 0)
				formation [dead_ship->special_data->formation_position].occupied = NULL;
			
			// WAS THE SHIP THAT DIED ONE WE HAD TO PROTECT AS PART OF THE MISSION ?
	
			killstatus = dead_ship->special_data->killstatus;			// Get ships kill status
		
			if (killstatus == SDB_KILL_MUSTSURVIVE || 
				killstatus == SDB_KILL_MUSTSURVIVEFLIGHT)
				{
				camera [status.current_camera].view_ship = dead_ship;	// Look at it if mission over
				
				if (dead_ship->type == MOTHER_SHIP)
					message_add (MESSAGE_TOP, (MTXT__MOTHERSHIP_DESTROYED), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR1);
				else
					message_add (MESSAGE_TOP, (MTXT__DEFENCE_TARGET_DESTROYED), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR1);
				
				control_gameover ();
				return;
				}
			
			// WAS THE SHIP THAT DIED ONE WE HAD TO KILL AS PART OF THE MISSION ?
			
			if (killstatus == SDB_KILL_MUSTDIE ||
				killstatus == SDB_KILL_MUSTSTOPFLIGHT)
				{
				message_add (MESSAGE_TOP, (MTXT__MISSION_SHIP_DESTROYED), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR1);
				status.flag_allshipdead-=1;
				}
			}
		}
			
	// WAS IT THE PLAYERS SHIP THAT DIED ? - MUST BE IN CONTINUE STATUS TO REGISTER DEATH
	
	if (dead_ship == players_ship)
		{
		player_crashed_xpos = dead_ship ->x_pos;				// Remember where player was when they died
		player_crashed_ypos = dead_ship ->y_pos;
		player_crashed_zpos = dead_ship ->z_pos;
				
		camera [0].type = CAMERA_TRACKING;						// Set the camera so it's tracking the dead player
		camera [0].view_ship = players_ship;
		players_ship->fire_request = WEAPON_NOTHING;
		status.current_camera=0;
		
		if (status.status == STATUS_CONTINUE)					// In normal mode ? Must be to lose a life etc.
			{
			if (--status.lives < 1)								// Lose a life
				control_gameover ();							// If no more lives, set to game over
			else
				status_register (STATUS_CRASHED, 64, 1);		// Else just set status to crashed
			}
		
		if (status.mega_ship > 0)								// Should we cancel megasetup ?
			{
			status.mega_ship = 0;
			bonus_megasetup_cancel ();
			}
			
		for (camloop = 0; camloop <=3; camloop++)				// Cancel all crystals picked up
			status.pickups [camloop] = 0;
		}
}

/**************************************/

void	control_retrack (void)

// Purpose : Resets tracking status to nearest object
// Accepts : Nothing
// Returns : Nothing

{

long	object_loop,
		
		ground_xpos,
		ground_ypos,
		ground_zpos,
		
		ship_xpos,
		ship_ypos,
		ship_zpos,
		distance,
		nearest_distance = -1,
		new_tracking_type = TRACKING_NOTHING;
		
ship_stack	*ship_track = (ship_stack*) (ships.info).start_address;			// Ship Pointer - start of ship linked list
	
	// CHECK FOR NEAREST SHIP TO TRACK
	
	while ((ship_track->header).status == 1)								// Search for nearest ship
		{

		if (ship_track -> special_data != NULL)								// Ship has an SDB ?
			if (ship_track -> special_data -> side == SDB_SIDE_ENEMY)		// Is it an enemy ?
				{
				ship_xpos = (ship_track->x_pos) - (players_ship->x_pos);	// Get X,Y,Z pos of enemy target
				if (ship_xpos < 0)	ship_xpos = -ship_xpos;					
				ship_ypos = (ship_track->y_pos) - (players_ship->y_pos);
				if (ship_ypos < 0)	ship_xpos = -ship_ypos;
				ship_zpos = (ship_track->z_pos) - (players_ship->z_pos);
				if (ship_zpos < 0)	ship_xpos = -ship_zpos;
				
				distance = (ship_xpos > ship_ypos) ? ship_xpos : ship_ypos;
				if (ship_zpos > distance)
					distance = ship_zpos;
				
				if (nearest_distance == -1 || distance < nearest_distance)
					{
					status.ship_tracking = ship_track;						// Set to look at ship
					new_tracking_type = TRACKING_SHIP;
					nearest_distance = distance;
					}
				}
				
		ship_track = (ship_stack*) (ship_track->header).next_address;		// Get next ship in linked list
		}

		// CHECK FOR NEAREST GROUND MISSION TARGET TO TRACK
			
	for (object_loop =0 ; object_loop < mission.groundcount; object_loop++)
		if (mission.groundlist [object_loop].type == GROUND_DESTROY)
			{
			ground_xpos = ((mission.groundlist [object_loop].xpos)<<24) - players_ship->x_pos;
			if (ground_xpos < 0)	ground_xpos =-ground_xpos;
			ground_ypos = ((mission.groundlist [object_loop].ypos)<<24) - players_ship->y_pos;
			if (ground_ypos < 0)	ground_ypos =-ground_ypos;
			ground_zpos = players_ship->z_pos;
			if (ground_zpos < 0)	ground_zpos =-ground_zpos;
			
			distance = (ground_xpos > ground_ypos) ? ground_xpos : ground_ypos;
			if (ground_zpos > distance)
				distance = ground_zpos;
			
			if (nearest_distance == -1 || distance < nearest_distance)
				{
				status.track_xpos = mission.groundlist [object_loop].xpos;	// Set to look at ground object
				status.track_ypos = mission.groundlist [object_loop].ypos;
				new_tracking_type = TRACKING_GROUND;
				nearest_distance = distance;
				}
			}
			
	status.track_type = new_tracking_type;
}

/**************************************/

void	control_searchcamera (long	camera_set, long object_instance)

// Purpose : Sets a camera to view a particular instance of a ship type
// Accepts : Camera number to set and instance of object type to view (ie. 5th enemy ship, 3rd weapon etc.)
// Returns : Nothing

{

long		instance_count = -1;													// Set instance count
ship_stack	*ship = (ship_stack*) (ships.info).start_address,						// Ship Pointer - start of ship linked list
			*last_ship=NULL;
	
	camera [camera_set].counter2 = 0;												// Cancel DEADCAM
	
	if (object_instance < 0)														// Check for camera pending object
		object_instance = 255;														// Yes - look for last instance
	
	while ((ship->header).status == 1 && instance_count < object_instance)			// While active ship, search
		{
		switch (camera [camera_set].view)
			{
			
			// CHECK FOR PLAYERS SHIP VIEW
			
			case	0 :	if (((ship->type) >> 4) == PLAYERS_SHIP)
							{
							instance_count+=1;
							last_ship = ship;
							}
						break;

			// CHECK FOR FRIENDLY SHIP VIEW
			
			case	1 : if (ship->special_data != NULL && ship != players_ship)
							if (ship->special_data->side == SDB_SIDE_FRIEND)
								{
								instance_count+=1;
								last_ship = ship;
								}
						break;

			// CHECK FOR ENEMY SHIP VIEW
			
			case	2 : if (ship->special_data != NULL)
							if (ship->special_data->side == SDB_SIDE_ENEMY)
								{
								instance_count+=1;
								last_ship = ship;
								}
						break;
			
			// CHECK FOR WEAPON VIEW
			
			case	3 :	if (((ship->type) >> 4) == WEAPON && ship->type != WING_POD)
							if (ship->who_owns_me == (long) players_ship)
								{
								instance_count+=1;
								last_ship = ship;
								}
						break;		
						
			// CHECK FOR VEHICLE VIEW
			
			case	4 :	if (((ship->type) >> 4) == CAR)
							{
							instance_count+=1;
							last_ship = ship;
							}
						break;		
			
			}
		
		ship = (ship_stack*) (ship->header).next_address;				// Get next ship in linked list
		}
		
		if (instance_count == object_instance)							// DID WE FIND THE INSTANCE WE WANTED
			{
			camera [camera_set].instance = object_instance;				// YES - OBJECT INSTANCE FOUND, SET INSTANCE
			camera [camera_set].view_ship = last_ship;					// YES - SET ADDRESS OF OBJECT INSTANCE
			}
			
		if (instance_count < object_instance)							// WE DID NOT FIND THE INSTANCE WE WANTED !
			{
			if (instance_count >= 0)									// O.K. Did we find any of this type ?
				{
				control_searchcamera (camera_set, 0);						// YES - Call again to get 1st instance
				
				//camera [camera_set].instance = instance_count;			// YES - SET TO LAST ONE WE FOUND
				//camera [camera_set].view_ship = last_ship;				// YES - SET ADDRESS OF OBJECT INSTANCE
				}
			else
				if (camera [camera_set].view != 0)						// NO - SET TO -ve INSTANCE OF TYPE (Override)
					camera [camera_set].instance = -1;					// ONLY IF NOT LOOKING AT PLAYERS SHIP
			}
}

/**************************************/

void control_changecamera (long new_camera)

// Purpose : Sets status camera to new camera number & Displays camera description message
// Accepts : New camera to view
// Returns : Nothing

{
	if (status.current_camera != new_camera)
		{
		status.current_camera = new_camera;
		message_addcamera (new_camera);
		}
}

/**************************************/

void control_recheckcamera (ship_stack*	ship)

// Purpose : Rechecks cameras when a new ship has been added - If we were pending view, watch it
// Accepts : Ship added
// Returns : Nothing

{

long	camloop;

	for (camloop = 0; camloop < MAX_CAMERAS; camloop++)					// Search all cameras
		{
		if (camera [camloop].instance == -1)							// Is this camera pending object ?
			switch (camera [camloop].view)
				{
				
				case 0 : if (((ship->type) >> 4) == PLAYERS_SHIP)		// Waiting for this type of ship [PLAYER]
					{
					camera [camloop].instance = 0;						// YES - Look at it
					camera [camloop].view_ship = ship;
					break;
					}
					
				case 2 : if (((ship->type) >> 4) == SMALL_SHIP)			// Waiting for this type of ship [ENEMY SHIP] ?
								if (ship->special_data != NULL)
									if (ship->special_data->side == SDB_SIDE_ENEMY)
										{
										camera [camloop].instance = 0;
										camera [camloop].view_ship = ship;
										}
						break;
				
				case 3 : if (((ship->type) >> 4) == WEAPON)					// Waiting for this type of ship [WEAPON] ?
							if (ship->who_owns_me == (long) players_ship)	// Yes - is it owned by the player ?
								{
								camera [camloop].instance = 0;			// YES - Look at it
								camera [camloop].view_ship = ship;
								break;
								}
				}
		}
}

/**************************************/

void control_selectweapon (void)

// Purpose : Selects a new weapon for the players ship, cycling round
// Accepts : Nothing
// Returns : Nothing

{
char	weapon_string [20];

	// Set the non fire counter to allow weapon locks to be selected
	players_ship->counter = 2 ;
	
	do
		{
		if (++status.weapon > WEAPON_MINE)					// Get next weapon
			status.weapon = WEAPON_LASER;					// Reached weapon limit ? Set back to laser
		}													// Else keep looking until we find a weapon with some left - OR LASER
	while (players_ship->performance->weapons [status.weapon] <= 0 && status.weapon != WEAPON_LASER);

	// Display weapon details
	
	sprintf (weapon_string, ">%s", message_decode (weapon_names [status.weapon], 0));
	message_text (MESSAGE_BOTTOM, weapon_string, ORANGE_15, MESSAGE_SCROLL, 32, MESSAGE_CURSOR1);
	message_addweapon();
}

/**************************************/

void control_gameover (void)

// Purpose : Display game over message, set status etc.
// Accepts : Nothing
// Returns : Nothing

{

	if (status.status == STATUS_MISSIONCOMPLETEDWAIT || status.status == STATUS_MISSIONCOMPLETED)
		return;
		
	message_add (MESSAGE_BIG, (MTXT__YOU_HAVE_FAILED_YOUR_MISSION), ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR1);
	message_add (MESSAGE_BOTTOM, (MTXT__GAME_OVER), YELLOW_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
	status_register (STATUS_GAMEOVERWAIT, 64, 1);
}

/**************************************/

void control_setupfromconfigure (void)

// Purpose : Sets up all game parameters from configuration file
// Accepts : Nothing
// Returns : Nothing

{
	message_initialiselanguagefiles (configuration.language);			// Initialise correct message files
	sound_switchonoroff (configuration.sound_on);						// Set SFX to on / off
	sound_setmastervolume ((configuration.sound_volume+1)*16);			// Set master volume level for SFX
	music_setmastervolume ((configuration.music_volume+1)*16);			// Set master volume level for MUSIC

	BS_SetStickMinX (&joystick, (short) configuration.stick_x_min);		// Set up joystick boundaries
	BS_SetStickMinY (&joystick, (short) configuration.stick_x_max);
	BS_SetStickMinZ (&joystick, (short) configuration.stick_y_min);
	BS_SetStickMaxX (&joystick, (short) configuration.stick_y_max);
	BS_SetStickMaxY (&joystick, (short) configuration.stick_z_min);
	BS_SetStickMaxZ (&joystick, (short) configuration.stick_z_max);

}

/**************************************/

long control_saveconfigure (void)

// Purpose : Trys to save the config file to NVRAM after deleting old one
// Accepts : Nothing
// Returns : success (1) or failure (0) of operation

{
	nvram_delete ("StarFighter.Config", "SFC:");
	return (nvram_save ("StarFighter.Config", "SFC:", (char*) &configuration, sizeof (game_configuration)));
}

/**************************************/

long control_showmission (long show_mode, long show_var)

// Purpose : Prints up mission brief for current mission or credits file
// Accepts : Mode Variable : 	0 - Setup for brief
//								1 - Setup for credits
//								2 - Display brief
//								3 - Display credits (no user interaction except terminate)

//			 Second var is		Setup 	: Number of mission lines in brief (if setting up) - only set if >=0
//								Display : Draw backdrop flag

// Returns : Continue / Quit / Start status

{

#define	BRIEF_ADDRATE	16					// Text add rate
#define	BRIEF_ADD		0x1
#define	BRIEF_ENTER		0x2

static	long	mission_lines;					// Number of text lines in brief
static	long	textcel_addrate;				// Counter before add next text line at top of screen
static	long	textcels_added;					// Next line to add at top of screen
static	long	text_topcheck;					// Top check limit for text
static	long	lastlineoffthetop;				// Last line that went off the top of the screen
static	long	text_overflow;					// Allow overflow of text
static	long	text_multipleadder;				// Initialise 1st frame with text page
static	long	textcel_fore;					// Current flip colour of text
static	long	textcel_lastcolouroffthetop;	// Colour of last item off the top of the screen
static	long	textcel_lastcolouroffthebot;	// Colour of last item off the bot of the screen

char	mission_line [40];
long	textcel,
		cel_width,
		cel_height,
		keypad,
		keypad_constant,
		textcel_case,
		text_bottomcheck = 240<<16,
		textcel_newypos = 0,
		text_direction,
		textcel_newline,
		mission_linecolour,
		textcel_slottouse = 0,
		mission_linetouse = 0,
		message_xlength;
		
	// ------------------------------------------------- SETTING UP FOR CREDITS OR BRIEF ?
	
	if (show_mode < 2)
		{
		if (show_var >= 0)
			mission_lines = show_var-1;		// Set number of mission lines ?
			
		textcels_added	 			= 0;	// Reset text adder counters
		textcel_addrate				= 0;
		lastlineoffthetop			= 0;
		textcel_lastcolouroffthetop = -1;
		textcel_lastcolouroffthebot = -1;
		text_topcheck 	    		= -12<<16;
		text_overflow				= 0;
		text_multipleadder			= 0;
		textcel_fore				= DARKGREEN1_15;
		
		// DISPLAY MISSION DETAILS, ENTER MENU & START STREAMING OF VOICE OVER [IF BRIEF]
	
		if (show_mode == 0)
			{
			text_topcheck = 12<<16;
			text_multipleadder = 205;
			
			menu_enter (MISSION_MENU, 0);
			
			if (configuration.sound_on == 1)
				{
				music_terminate ();
				music_initialise (1);
				music_playvoice ("%s%s%c/Miss_%d", RESOURCES_ROOT, VOICE_ROOT, decode_level(parameters.level), 1+return_mission (&pyramids [parameters.level]));
				}
			}
			
		// CLEAR TEXT CELS
		
		for (textcel = 1; textcel < FONT_MAXMENU; textcel++)
			{
			SetTextCelSize (cel_text [textcel], 0, 0);
			((cel_text [textcel] -> tc_CCB) -> ccb_Flags) |= CCB_SKIP;
			((cel_text [textcel] -> tc_CCB) -> ccb_Flags) &= ~CCB_LAST;
			}
		((cel_text [FONT_MAXMENU-1] -> tc_CCB) -> ccb_Flags) |= CCB_LAST;
		
		return (BRIEF_CONTINUE);
		}
		
	// ------------------------------------------------- DISPLAYING CREDITS OR BRIEF
	
	keypad = controlpad_debounce ();
	keypad_constant = controlpad_constant ();
	
	// CHECK FOR DIRECTION CONTROL FROM USER (IF BRIEF, ELSE ALWAYS UP)
	
	text_direction = 0;
	
	if (show_mode == 2)
		{
		if (keypad_constant & ControlUp || keypad_constant & ControlLeftShift)		text_direction = -1;	// Up ?
		if (keypad_constant & ControlDown || keypad_constant & ControlRightShift)	text_direction = +1;	// Down ?
		}
		
	else
		text_direction = -1;									// Force up for credits
		
	if (text_multipleadder > 0)									// Force up for 1st frame page fill ?
		text_direction = -1;
	
	do
		{
		
		// SHOULD WE ADD A LINE FROM THE MISSIONS FILE ?
		
		textcel_newline = 0;
		
		if (text_direction < 0)									// Are we scrolling text up ?
			{
			if (++textcel_addrate >= BRIEF_ADDRATE)				// Add a line from the text file ?
				{
				
				if (text_overflow <= -8)
					{
					textcel_addrate = BRIEF_ADDRATE-1;
					text_direction = 0;
					if (show_mode == 3)
						return (BRIEF_QUIT);
					}
				else
					{
					textcel_newline |= BRIEF_ENTER;					// Set new line flag
					textcel_addrate -= BRIEF_ADDRATE;				// Yes - Reset adder count
					textcel_newypos = text_bottomcheck >> 16;		// Set ypos of text
					mission_linetouse = ++textcels_added;			// Move mission line to next
					
					if (mission_linetouse > mission_lines || mission_linetouse < 1)
						{
						mission_linetouse = -1;						// If end of brief, do ''
						text_overflow-=1;
						}
						
					if (textcel_lastcolouroffthebot != -1)
						{
						textcel_fore = textcel_lastcolouroffthebot;
						textcel_lastcolouroffthebot = -1;
						}
					}
				}
			}
	
		if (text_direction > 0)									// Are we scrolling text down ?
			{
			if (--textcel_addrate < 0)							// Add a line from the text file ?
				{
				
				if (text_overflow >=8)
					{
					textcel_addrate = 0;
					text_direction = 0;
					}
				else
					{
					textcel_addrate += BRIEF_ADDRATE;				// Yes - Reset adder count
					textcel_newline |= BRIEF_ENTER;					// Set new line flag
					textcel_newypos = text_topcheck >> 16;			// Set ypos of text
					mission_linetouse = lastlineoffthetop--;		// Move mission line to next
					
					if (mission_linetouse > mission_lines || mission_linetouse < 1)
						{
						mission_linetouse = -1;						// If end of brief, do ''
						text_overflow+=1;
						}
					
					if (textcel_lastcolouroffthetop != -1)			// Get last colour that went off the top
						{
						textcel_fore = textcel_lastcolouroffthetop;
						textcel_lastcolouroffthetop = -1;
						}
					}
				}
			}
					
		// ARE WE ADDING A LINE OF TEXT ?
		
		if (textcel_newline & BRIEF_ENTER)
			{
			textcel_case = (mission_linetouse == -1) ? -1 : (show_mode == 3) ? 0 : mission_linetouse;
			
			// Find an empty slot to use for text
			
			textcel_slottouse = -1;
			textcel = 1;
			do
				{
				if ((((cel_text [textcel] -> tc_CCB) -> ccb_Flags) & CCB_SKIP))		// Is cel inactive ?
					textcel_slottouse = textcel;									// Yes - get it's number
				textcel+=1;
				}
			while (textcel < FONT_MAXMENU && textcel_slottouse == -1);
			
			// print warning & set to 0 if couldn't find ! (THIS SHOULD NEVER HAPPEN)
			
			if (textcel_slottouse == -1)
				{
				printf ("COULD NOT ALLOCATE FREE TEXTSLOT !!!!!!!!!!!!!!!\n");
				textcel_slottouse = 0;
				}
				
			switch (textcel_case)
				{
				case -1 :
					sprintf (mission_line, "");
					mission_linecolour = WHITE_15;
					break;	
				case 0 :
					sprintf (mission_line, "%s", message_decode (mission_linetouse, 2));
					mission_linecolour = WHITE_15;
					if (show_mode == 3 && mission_line [0] == '-')
						mission_linecolour = YELLOW_15;
					break;
				case 1 :
					sprintf (mission_line, "Location : '%s'", message_decode (mission_linetouse, 2));
					mission_linecolour = RED_15;
					break;
				case 2 :
					sprintf (mission_line, "Date : %s A.D", message_decode (mission_linetouse, 2));
					mission_linecolour = RED_15;
					break;
				default :
					sprintf (mission_line, "%s", message_decode (mission_linetouse, 2));
					if (*(message_decode (mission_linetouse, 2)) == 0)
						{
						textcel_fore = (textcel_fore == DARKGREEN1_15) ? DARKGREEN2_15 : DARKGREEN1_15;
						}
					mission_linecolour = textcel_fore;
					break;
				}
			
			SetTextCelColor(cel_text [textcel_slottouse], 0, mission_linecolour);					// Set colour
			UpdateTextInCel(cel_text [textcel_slottouse], TRUE, mission_line);						// Render text cel
			GetTextCelSize (cel_text [textcel_slottouse], &cel_width, &cel_height);					// Get it's size
			SetTextCelCoords(cel_text [textcel_slottouse], 160-(cel_width>>1), textcel_newypos);	// Set it's position
			((cel_text [textcel_slottouse] -> tc_CCB) -> ccb_PRE1) &= (~1023);						// Set it's Width to zero (If we are scrolling it)
			((cel_text [textcel_slottouse] -> tc_CCB) -> ccb_Flags) &= ~CCB_SKIP;					// Make active
			cel_text [textcel_slottouse] -> tc_userData = (void*) mission_linecolour;				// Remember colour
			}
			
		// ARE WE MOVING TEXT CELS UP OR DOWN ?
				
		for (textcel = 1; textcel < FONT_MAXMENU; textcel++)
			if (!(((cel_text [textcel] -> tc_CCB) -> ccb_Flags) & CCB_SKIP))				// Is cel active ?
				{
				
				if (text_multipleadder == 0)
					{
					message_xlength = (((cel_text [textcel] -> tc_CCB) -> ccb_PRE1) & 1023);
					message_xlength += ((show_mode == 2) ? 3 : 2);
					
					if (message_xlength > ((cel_text [textcel] -> tc_CCB) -> ccb_Width))
						message_xlength = (cel_text [textcel] -> tc_CCB) -> ccb_Width;
					else
						if (arm_random () & 64)
							sound_playsample (SOUND_BEEP, 60, 10, 0);
					
					(cel_text [textcel] -> tc_CCB) -> ccb_PRE1 &= (~1023);
					(cel_text [textcel] -> tc_CCB) -> ccb_PRE1 |= message_xlength;
					}
					
				if (text_direction != 0)
					{
					
					((cel_text [textcel] -> tc_CCB) -> ccb_YPos)+=(text_direction<<16);		// Yes, move it
					
					if ((cel_text [textcel] -> tc_CCB) -> ccb_YPos <= text_topcheck) 		// Off top of screen ?
						{
						((cel_text [textcel] -> tc_CCB) -> ccb_Flags) |= CCB_SKIP;				// Yes, take it out
						textcel_lastcolouroffthetop = (long) cel_text [textcel] -> tc_userData;	// Remember colour !
						lastlineoffthetop+=1;													// Increase 1 off the top counter
						
						if (text_overflow > 0)
							text_overflow-=1;
						}
						
					if ((cel_text [textcel] -> tc_CCB) -> ccb_YPos >= text_bottomcheck) 		// Off bottom of screen ?
						{
						
						((cel_text [textcel] -> tc_CCB) -> ccb_Flags) |= CCB_SKIP;				// Yes, take it out
						textcel_lastcolouroffthebot = (long) cel_text [textcel] -> tc_userData;	// Remember colour !
						textcels_added-=1;													// Move mission line pointer back 1
						
						if (text_overflow < 0)
							text_overflow+=1;
						}
					}
				}
						
		// REDUCE MULTIPLE ADDER PAGE ?
		
		if (text_multipleadder > 0)
			text_multipleadder-=1;
			
		}
	while (text_multipleadder > 0);
	
	// RENDER GRAPHICS TO SCREEN
		
	message_update ();											// Update Text Messages
	menu_status.cel_textccb = cel_text [1] -> tc_CCB;			// Set menutext to start display at mission brief
	screen_update (S_CEL | S_MENU);								// Draw text & flip screens
	
	cel_quad.temp_cels=0;										// Reset plotting list
	
	if (show_mode == 2)
		{
		menu_status.cel_textccb = cel_text [0] -> tc_CCB;		// Set menutext to start display at menu title
		menu_display (RED_PAL);									// Display title
		}
	
	if (show_mode == 3 && keypad != 0)							// If doing credits & keypress, return quit
		return (BRIEF_QUIT);
		
	if (keypad & (ControlX))									// Check for quit
		{
		return (BRIEF_QUIT);								
		music_pause ();
		}
	
	if (keypad & (ControlStart | ControlA))						// Check for start / a
		{
		return (BRIEF_START);								
		music_pause ();
		}
	
	return (BRIEF_CONTINUE);
}

/**************************************/

void control_newlife (void)

// Purpose : Adds a new life (if < 7)
// Accepts : Nothing
// Returns : Nothing

{

	if (status.lives < 7)
		{
		message_add (MESSAGE_BIG, MTXT__EXTRA_LIFE, WHITE_15, MESSAGE_SCROLL, 32, MESSAGE_NOCURSOR);
		status.lives+=1;
		status.display_lives = 32;
		}
}
