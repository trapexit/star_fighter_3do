/* File : SF_MENU -> Menus header file */

#ifndef __SF_MENU
#define __SF_MENU


/* Includes */

#include "graphics.h"
#include "TextLib.h"

#include "SF_CelUtils.h"
#include "SF_Allocation.h"
#include "SF_Status.h"
#include "SF_Parameters.h"
#include "SF_Music.h"
#include "SF_Pyramid.h"
#include "SF_Mission.h"
#include "SF_Joystick.h"

#include "Camera_Struct.h"
#include "Ship_Struct.h"

// Definitions for user messages

#define	USER_NVRAMFULL			1
#define	USER_DEFAULTCONFIG		2

// Definitions for easy access to menu variables
		
#define NO_MENU					-1
#define	MAIN_MENU				0
#define FORMATION_MENU			1
#define PAUSE_MENU				2
#define	PLAYERPOS_MENU			3
#define	GAMEPOS_MENU			4
#define	MISSION_MENU			5
#define	LEVEL_MENU				6
#define	SOUND_MENU				7
#define	JUKEBOX_MENU			8
#define	LOAD_MENU				9
#define	SAVE_MENU				10
#define	CONFIGURE_MENU			11
#define	CONTROLS_MENU			12
#define	DEBRIEF_MENU			13
#define	LANGUAGE_MENU			14
#define	HIGHSCORE_MENU			15
#define	FLIGHTCONTROL_MENU		16
#define	CALIBRATE_JOYSTICK_MENU	17


// Definitions for easy access to menu commands

#define	MAIN_NOTHING		0
#define	MAIN_START			1
#define	MAIN_LOADSTART		2

// Definitions for menu item status flags

#define ITEM_NOTOK			0		// Item is shaded and cannot be selected
#define ITEM_OK				1		// Item can be selected

#define	ITEM_TN				0		// Item cannot be ticked
#define	ITEM_TE				1		// Item can be exclusively ticked
#define	ITEM_TM_C			2		// Item can be ticked along with others, but is not currently
#define	ITEM_TM_S			3		// Item can be ticked along with others, and is set

#define	ITEM_PR				1		// Plot Icon on right
#define	ITEM_PL				2		// Plot Icon on left
#define	ITEM_PB				3		// Plot Icon on right and left
#define	ITEM_PA				4		// Always plot icon (Do not link to ticks)

/* Structure Definitions */

typedef struct	gamefile									// Save file structure
				{
				long				savefile_version;		// Current version of file
				long				savefile_level;			// Current level
				pyramid				savefile_pyramids [4];	// Pyramids status
				performance_data	savefile_performance;	// Players ship performance
				game_status			savefile_status;		// Players play status	
				camera_data			savefile_cameras [2];	// Players camera settings
				}	gamefile;
				
typedef	void	(*MenuFunction) ();

typedef struct	menu_info
				{
				long	current_menu,
						command,
						selected,
						hidden;
				CCB*	cel_textccb;
				} menu_info;
				
typedef struct 	menu_item
				{
				long	item_icon;
				long	item_iconwidth;
				long	item_plottype;
				long	tick_status;
				long	y_position;
				long	selectable;
				long	command;
				
				MenuFunction	function;
				long			function_value;
				
				long	max_selection;
				long	cur_selection;
				
				long	item_colour;
				long	string [4];
				TextCel	*tccbptr;
				} menu_item;

typedef	struct	menu_border
				{
				long	colour;
				char	option_start;
				char	option_end;
				} menu_border;
				
typedef struct	menu
				{
				long		title;
				long		reload_gamesprites;
				char		*gamesprites;
				long		quit;
				long		title_colour;
				long		tick_position;
				
				long		ypos;
				
				long		previous;
				long		border_count;
				menu_border	borders [4];
				long		max_option;
				long		cur_option;
				
				menu_item	items [9];
				} menu;


/* External definitions */

extern	long				first_frame;
extern	long				music_background;
extern	long				info;
extern	long				configure_waiting;

extern	menu_info			menu_status;
extern	menu				menus [MAX_MENUS];
extern	TextCel				*cel_text [FONT_MAXMENU];
extern	game_status			status;
extern	camera_data			camera [MAX_CAMERAS];
extern	ship_list			ships;
extern	ship_stack 			*players_ship;
extern	game_parameters		parameters;
extern	game_configuration	configuration;
extern	performance_data	player_performance;
extern	cel_celdata			cel_quad;
extern	pyramid				pyramids [4];

extern	char				MUSIC_PROGRAM [MUSIC_PLAYLISTLENGTH];
extern	char				MUSIC_PROGRAMLENGTH;
extern	long				cheat_feature3;
extern	stick_struct		pro_stick;
extern	BS_StickData 		joystick;

extern	long				cheat_feature1;
extern	long				cheat_feature2;
extern	long				cheat_feature3; 
extern	long				cheat_feature4;

/* Function Prototypes */

// Menu control functions

void	menu_moveup (long);					// Move to previous menu
void	menu_movedown (long);				// Move to sub menu
void	menu_enter (long, long);			// Set up a new menu
long	menu_update (long, long);			// Update menu
void	menu_display (long);				// Render menu info to screen
void	menu_resetmenutext (void);			// Clears all menu text cels

// Level menu functions

void	menu_setlevel (long);				// Select a level


// Game pause menu functions

void	menu_updateviewpresets (long, long);		// Update all camera menus
void	menu_updatecamerapos (long, long, long);	// Update camera positions
void	menu_quitgame (long);						// Return to main menu

// Formation control functions

long	menu_displayformation (long);		// Display / count formation on screen
void	menu_formationgroup (long);			// Re-group formation
void	menu_formationattack (long);		// Send formation into attack
void	menu_changeformmode (long);			// Change formation control mode

// Music control functions

void	menu_musicset (long);				// Set music mode
void	menu_soundset (long);				// Set sound mode
void	menu_trackset (long);				// Toggle track on / off
void	menu_setmusictracks (void);			// Setup music playlist from menu track selections
void	menu_displaymusic (void);			// Display sound volume info
void	menu_updatesoundmenu (long, long);	// Check for adjusting sound bars
void	menu_updatejukeboxmenu (long);		// Check for moving tracks
void	menu_displayjukebox (void);			// Display currently playing track

// NVRam gamefile access functions

void	menu_loadgame (long);				// Load a previously saved game
void	menu_savegamefile (long);			// Gets filename & saves (If no room, goes into replace menu)
void	menu_savegame (long);				// Save a game

// Configure functions

void	menu_setvideo (long);				// Toggles video on / off
void	menu_configdefault (long);			// Sets game to standard configuration
void	menu_setlanguage (long);			// Change language of game [CURRENTLY EUROPEAN ONLY]
void	menu_setcontrol (long);				// Sets new control method
void	menu_setupstick (long);				// Set up stick boundaries
void	menu_setflightc (long);				// Set buttons for game controls
void	menu_updateflightc (long);			// Update setflightcontrols menu
void	menu_saveflightc (void);			// Save config file & move up
long	menu_endflightc (long);				// Check for clashes in controls

// Get User Name functions

long	menu_getname (char*, long, char*, long);	// Get name / filename from user
void	menu_setpilotname (long);					// Gets default name / cheat

// User message display

void	menu_usermessage (long);					// Displays user interaction message

#endif
