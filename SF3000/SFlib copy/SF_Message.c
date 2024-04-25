// File : SF_Message
// In-Game Message display routines

// Includes

#include "Stdio.h"
#include "SF_Message.h"
#include "SF_Palette.h"
#include "SF_CelUtils.h"
#include "SF_Sound.h"
#include "SF_Allocation.h"
#include "SF_Access.h"
#include "SF_Utility.h"
#include "SF_Control.h"
#include "SF_ARMCell.h"
#include "SF_ARMUtils.h"

// Global variables

message	messages [FONT_MAXMESSAGE];										// Messages status array for in game text

char	messages_gametext 			[4096];								// Arrays for game, menu & mission text lines
char	messages_menutext			[4096];
char	messages_missiontext		[2048];
char	message_missiontitle		[2048];

char	*game_message_pointers		[128];								// Arrays for pointers to game, menu & mission text lines
char	*menu_message_pointers		[160];
char	*mission_message_pointers	[128];
char	*mission_title_pointers		[80];

char	**message_pointerlist [4] =	{	game_message_pointers,			// List of pointer array starts
										menu_message_pointers,
										mission_message_pointers,
										mission_title_pointers};
										
char	languages [3] [] = {		"English",							// Names of languages supported
									"French",
									"German"	};
								
/************************************************************************************************************************************
*											GENERAL PURPOSE MESSAGE DISPLAY ROUTINES												*
************************************************************************************************************************************/

void	message_initialise (void)

// Purpose : Resets al message timers to zero
// Accepts : Nothing
// Returns : Nothing

{

long	message_type;

	for (message_type =0; message_type < FONT_MAXMESSAGE; message_type++)
		{
		messages [message_type].timer =0;
		SetTextCelSize (messages [message_type].textcel, 0, 0);				// Release text's memory
		(messages [message_type].textcel->tc_CCB) ->ccb_Flags |= CCB_SKIP;	// Make inactive
		}
		
	// Setup score & timer cel position
	
	SetTextCelCoords(messages [MESSAGE_SCORE].textcel, 24, 25);
	SetTextCelCoords(messages [MESSAGE_TIMER].textcel, 35, 202);
	
	// Make score cel active
	
	(messages [MESSAGE_SCORE].textcel->tc_CCB) ->ccb_Flags &= ~CCB_SKIP;	// Make active
}

/**************************************/

void	message_pilot (long pilot_name, long message_number, long colour, long flags, long timer, long cursor)

// Purpose : Adds message from a pilot
// Accepts : See message_text (except takes message number instead of pointer to string & pilot name)
// Returns : Nothing

{
char	pilot_message [40];

	sprintf (pilot_message, "%s : %s", message_decode (MTXT__MESSAGE_FROM, 0), pilot_names [pilot_name]);
	message_text (MESSAGE_NAME, pilot_message, WHITE_15, MESSAGE_FLASH, 64, MESSAGE_NOCURSOR);
	message_add (MESSAGE_BOTTOM, message_number, ORANGE_15, MESSAGE_SCROLL, 64, MESSAGE_CURSOR2);
}			
			
/**************************************/

void	message_add (long type, long message_number, long colour, long flags, long timer, long cursor)

// Purpose : Calls message_text with an decoded in-game message number (checks to see that it's not already displaying it)
// Accepts : See message_text (except takes message number instead of pointer to string)
// Returns : Nothing

{

	// If already displaying message, return
	
	if (messages [type].timer > 0 && messages [type].message_id == message_number)
		return;
	
	// Put in message id & Call message add with decoded message string
	
	messages [type].message_id = message_number;
	message_text (type, message_decode (message_number, 0), colour, flags, timer, cursor);
}

/**************************************/

void	message_text (long type, char* message, long colour, long flags, long timer, long cursor)

// Purpose : Sets up an in-game message for display, - picks X & Y position from type. Uses No delay, fixed scroll speed of 16
// Accepts : Pointer to message, type, colour, flags, timer, cursor
// Returns : Nothing

{

long	ypos=0,
		xpos=0;

	switch (type)
	{
		case MESSAGE_TOP :
			ypos = 36;
			xpos = -1;
			break;
		case MESSAGE_BOTTOM :
			ypos = 184;
			xpos = -1;
			break;
		case MESSAGE_BIG :
			ypos = 140;
			xpos = -1;
			break;
		case MESSAGE_NAME :
			ypos = 170;
			xpos = -1;
			break;
		case MESSAGE_CAM :
			ypos = 194;
			xpos = -1;
			break;
	}
	
	message_addxy (type, message, colour, 0, flags, timer, cursor, xpos, ypos, 0, 16);
}

/**************************************/

void	message_addxy (long type, char* message, long colour, long back_colour, long flags, long timer, long cursor, long xpos, long ypos, long delay, long speed)

// Purpose : Displays an in game message
// Accepts : Message number, Message, colour, display flags, timer, cursor type and position on screen
// Returns : Nothing

{

long	width,
		height;
		
	SetTextCelColor(messages [type].textcel, back_colour, colour);		// Set text colour
	UpdateTextInCel (messages [type].textcel, TRUE, message);			// Render text to cel
	
	if (xpos < 0)
		{
		GetTextCelSize (messages [type].textcel, &width, &height);		// Get it's size
		xpos = 160-(width>>1);
		}
	
	SetTextCelCoords (messages [type].textcel, xpos, ypos);				// Set X&Y pos for message

	messages [type].timer = timer;										// Set message timer
	messages [type].flags = flags;										// Save message info
	messages [type].cursor = cursor;									// Save cursor type info
	messages [type].textcel->tc_CCB->ccb_Flags &= ~CCB_SKIP;			// Make Active
	messages [type].delay = delay;										// Ammount to delay by before displaying
	messages [type].scroll_speed = speed;								// Speed of scroll
	
	if (flags & MESSAGE_SCROLL)
		messages[type].textcel->tc_CCB->ccb_PRE1 &= (~1023);			// Set it's Width to zero (If we are scrolling it)
}

/**************************************/

void	message_update (void)

// Purpose : Updates all in game messages (top, bottom and big)
// Accepts : Nothing
// Returns : Nothing

{

long	message_type,
		message_xlength;

	// Update message timers
			
	for (message_type=3; message_type < FONT_MAXMESSAGE; message_type++)
		if (messages[message_type].delay > 0)
			messages[message_type].delay--;
		else
			{
			if (messages[message_type].timer >0)
				{
			
				// ARE WE SCROLLING THE MESSAGE ALONG ?
				
				if (messages [message_type].flags & MESSAGE_SCROLL)
					{
					message_xlength=(messages[message_type].textcel->tc_CCB->ccb_PRE1 & 1023)+messages[message_type].scroll_speed;
					if (message_xlength > messages[message_type].textcel->tc_CCB->ccb_Width)
						message_xlength = messages[message_type].textcel->tc_CCB->ccb_Width;
					else
						sound_playsample (SOUND_BEEP, 40 + (message_type<<2) + ((messages[message_type].timer & 1)<<3), 100, 0);
							
					messages[message_type].textcel->tc_CCB->ccb_PRE1 &= (~1023);
					messages[message_type].textcel->tc_CCB->ccb_PRE1 |= message_xlength;
					}
				
				// IS THE MESSAGE FLASHING A CURSOR ? - IF SO AND TIMER>>1&1 THEN PUT CURSOR ON SCREEN
				
				if (messages [message_type].cursor != 0)
					if (messages [message_type].timer & 2)
						{
						cel_quad.x_pos0 = (((messages[message_type].textcel->tc_CCB->ccb_XPos)>>16)-156) + (messages[message_type].textcel->tc_CCB->ccb_PRE1 & 1023);
						cel_quad.y_pos0 = ((messages[message_type].textcel->tc_CCB->ccb_YPos)>>16)-117;
						cel_quad.shade = 10;
						arm_addgamecel (&cel_quad, BASECEL_CURSOR + messages [message_type].cursor, 1024, 1024);
						}
					
				// IS THE MESSAGE FLASHING ON THE SCREEN ?
				
				if (messages [message_type].flags & MESSAGE_FLASH)
					{
					if (messages [message_type].timer & 2)
						{
						sound_playsample (SOUND_BEEP, 50, 127, 0);
						messages [message_type].textcel->tc_CCB->ccb_Flags |= CCB_SKIP;		// Hide text
						}
					else
						{
						messages [message_type].textcel->tc_CCB->ccb_Flags &= ~CCB_SKIP;	// Make text active again
						}
					}
				
				
				// HAS MESSAGE EXPIRED IT'S TIME LIMIT ?
				
				if ((--messages[message_type].timer) <=0)
					{
					SetTextCelSize (messages [message_type].textcel, 0, 0);					// Release text's memory
					(messages[message_type].textcel->tc_CCB) ->ccb_Flags |= CCB_SKIP;		// Make inactive
					}
				}
			}
}		

/************************************************************************************************************************************
*								ROUTINES TO DISPLAY SCORE, WEAPON COUNT, MISSION TINER AND CAMERA									*
************************************************************************************************************************************/


void	message_addscore (long	score_adder)

// Purpose : Adds to players score, rendering to message cel 0
// Accepts : Integer to increment score by
// Returns : Nothing

{

char	score [16];
long	current_score = status.score;

	status.score += score_adder;
	sprintf(score, "%d", status.score);
	UpdateTextInCel(messages [MESSAGE_SCORE].textcel, TRUE, score);

	// Should we add extra life ? (100000, 250000, 350000)
	
	if (	(current_score < 50000 && status.score >= 50000) ||
			(current_score < 100000 && status.score >= 100000) ||
			(current_score < 250000 && status.score >= 250000) ||
			(current_score < 500000 && status.score >= 500000) ||
			(current_score < 1000000 && status.score >= 1000000))
			
			{
			control_newlife ();
			}
}

/**************************************/

void	message_addweapon (void)

// Purpose : Sets # of weapon (type) left, rendering to message cel 1
// Accepts : Nothing
// Returns : Nothing

{

long	weapon_type = status.weapon;												// Get type of current weapon
long 	weapon_num =  players_ship->performance->weapons [weapon_type];				// Get # left of weapon
char 	weapon_string [8];															// Destination string

	if (weapon_type == WEAPON_LASER || (weapon_type != WEAPON_LASER && weapon_num <=0))					// If laser or none left, do not print anything
		(messages [MESSAGE_WEAPON].textcel->tc_CCB) ->ccb_Flags |= CCB_SKIP;							// Make inactive
	else
		{

		(messages [MESSAGE_WEAPON].textcel->tc_CCB) ->ccb_Flags &= (~CCB_SKIP);							// Make Active
		sprintf(weapon_string, "%d", weapon_num);														// Render # to string
		SetTextCelColor(messages [MESSAGE_WEAPON].textcel, 0, weapon_num < 10 ? ORANGE_15 : WHITE_15);	// Set colour
		UpdateTextInCel(messages [MESSAGE_WEAPON].textcel, TRUE, weapon_string);						// Render text
		
		SetTextCelCoords(messages [MESSAGE_WEAPON].textcel, 299 - (((messages[MESSAGE_WEAPON].textcel->tc_CCB->ccb_Width)+2) & ~3), 29);	// Set pos									// Set text position
		}
}


/**************************************/

void	message_addtimer (void)

// Purpose : Displays current mission timer
// Accepts : Nothing
// Returns : Nothing

{

char	timer_string [8];

	messages [MESSAGE_TIMER].textcel->tc_CCB->ccb_Flags &= (~CCB_SKIP);											// Make Active
	sprintf (timer_string, "%d", mission.mission_timer);
	SetTextCelColor(messages [MESSAGE_TIMER].textcel, 0, mission.mission_timer < 30 ? ORANGE_15 : WHITE_15);	// Set colour
	UpdateTextInCel(messages [MESSAGE_TIMER].textcel, TRUE, timer_string);										// Render text
}

/**************************************/

void	message_addcamera (long	current_camera)

// Purpose : Displays current camera message
// Accepts : Current camera number
// Returns : Nothing

{

char	cam_message [32];

	// DISPLAY CAMERA
	
	switch (current_camera)
		{
		case 0 :
			sprintf(cam_message, "DeadCam - ");
			break;
		case 1 :
			sprintf(cam_message, "PlayCam - ");
			break;
		case 2 :
			sprintf(cam_message, "GameCam - ");
			break;
		}

	if (camera [current_camera].instance <0)
		sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__NO, 0));
	else
		{
		
		// DISPLAY CAMERA TYPE

		switch (camera [current_camera].type)
			{
			case CAMERA_NORMAL :
				sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__VIEW, 0));
				break;
			case CAMERA_TRACKING :
				sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__TRACKING, 0));
				break;
			case CAMERA_FLYBY :
				sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__FLYBY, 0));
				break;
			case CAMERA_INTERNAL :
				sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__VIEW_FROM, 0));
				break;
			}
		}

	// DISPLAY VIEW TYPE
	
	switch (camera [current_camera].view)
	{
	case 0 :
		sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__PLAYER, 0));
		break;
	case 1 :
		sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__TEAM, 0));
		break;
	case 2 :
		sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__ENEMY, 0));
		break;
	case 3 :
		sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__WEAPON, 0));
		break;
	case 4 : 
		sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__VEHICLE, 0));
		break;
	}
	
	// DISPLAY INSTANCE
	
	if (camera [current_camera].instance <0)
		sprintf(cam_message,"%s %s", cam_message, message_decode (MTXT__AVAILABLE, 0));
	else
		{
		if (camera [current_camera].view != 0)
			sprintf(cam_message,"%s %d", cam_message, camera [current_camera].instance+1);
		}
			
	// ADD MESSAGE
	
	message_text (MESSAGE_CAM, cam_message, ORANGE_15, MESSAGE_SCROLL, 48, MESSAGE_CURSOR1);
}

/************************************************************************************************************************************
*									ROUTINES TO MANAGE INDIRECTED GAME MESSAGES FROM LANGUAGE FILE									*
************************************************************************************************************************************/

char*	message_decode (long message_number, long message_file)

// Purpose : Takes a string from the language file
// Accepts : Message number and message file id (0 = game message file, 1 = menu message file, 2 = mission message file)
// Returns : Pointer to string

{
	return (*(message_pointerlist [message_file] + message_number));
}

/**************************************/

void	message_initialiselanguagefiles (long language_id)

// Purpose : Loads and parses the game & menu message files
// Accepts : Language to load
// Returns : Nothing

{

	load_fileat(messages_gametext, "%s%s/Game", TEXT_ROOT, languages [language_id]);		// Load in game message file
	load_fileat(messages_menutext, "%s%s/Menu", TEXT_ROOT, languages [language_id]);		// Load in menu message file
	load_fileat(message_missiontitle, "%s%s/Title", TEXT_ROOT, languages [language_id]);	// Load in mission title file
	
	
	message_decodetextfile (&messages_gametext [0], game_message_pointers, 0);				// Decode game message file
	message_decodetextfile (&messages_menutext [0], menu_message_pointers, 0);			// Decode menu message file
	message_decodetextfile (&message_missiontitle [0], mission_title_pointers, 0);			// Decode mission title file
}

/**************************************/

long	message_initialisemissionfiles (long language_id, long load_level, long load_mission)

// Purpose : Loads and parses the mission texts / credits
// Accepts : Language number, level number, mission number - IF LEVEL NUMBER == -1, LOADS CREDITS INSTEAD
// Returns : Number of lines in mission text

{

	// Load in mission text file

	if (load_level != -1)
		load_fileat(messages_missiontext, "%s%s/%c/Brief_%d",TEXT_ROOT, languages [language_id], decode_level(load_level),load_mission);
	else
		load_fileat(messages_missiontext, "%s%s/Credits",TEXT_ROOT, languages [language_id]);
	
	return (message_decodetextfile (&messages_missiontext [0], mission_message_pointers, 1));
}

/**************************************/


long	message_decodetextfile (char *text_ptr, char *text_arrayptr [], long decode_type)

// Purpose : Parses a mac text file (strings terminated with &0d, replaces with &00 & places start entry into array of pointers
// Accepts : Pointer to start of text file, pointer to start of message array, type of decode (0 = use line numbers, 1 = auto)
// Returns : Number of lines read

{
long	text_message;
long	text_linecount = 0;
	
	while (*text_ptr != '#')
		{
				
		if (*text_ptr != '|')
			{
			
			// Read in message-id number (if not just counting sequentially)
		
			if (decode_type == 0)
				{
				text_message = (((*(text_ptr+0))-48)*100) + (((*(text_ptr+1))-48)*10) + (((*(text_ptr+2))-48)*1);
				if (text_message < 0 || text_message > 256)
					printf ("EITF !\n");
				text_ptr+=4;
				}
			else
				{
				text_message = text_linecount;
				}
				
			// Put in reference & inc. line count for file
			
			*(text_arrayptr + text_message) = text_ptr;
			text_linecount+=1;
			}
			
		// Scan line, marking end with zero, until end of text line
		
		do
			if (*text_ptr >= 32)
				text_ptr+=1;
		while (*text_ptr != 0x0d);
		*(text_ptr++) = 0;
		}
	
	return (text_linecount);
}
