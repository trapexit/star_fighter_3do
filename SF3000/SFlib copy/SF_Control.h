/* File : SF_CONTROL -> Game control header file */

#ifndef __SF_CONTROL
#define __SF_CONTROL


// Includes

#include "SF_War.h"
#include "SF_Mission.h"
#include "SF_Status.h"
#include "SF_CelUtils.h"
#include "SF_Menu.h"
#include "SF_Parameters.h"
#include "SF_ARMAnim.h"
#include "SF_ARMCell.h"
#include "SF_Allocation.h"
#include "SF_Font.h"
#include "SF_Joystick.h"
#include "Ship_Struct.h"
#include "Camera_Struct.h"


// Definitions

#define	BRIEF_CONTINUE	1	// Continue with mission briefing
#define	BRIEF_START		2	// Start from mission briefing
#define	BRIEF_QUIT		4	// Quit from mission briefing

// External Variables

extern	game_configuration	configuration;
extern	game_parameters		parameters;
extern	game_status			status;	
extern	mission_data		mission;
extern	performance_data	player_performance;
extern	performance_data	player_resetperformance;
extern	ship_stack 			*players_ship;
extern	camera_data			camera [MAX_CAMERAS];
extern	cel_celdata			cel_quad;
extern	char				animate [2052];
extern	ship_list			ships;
extern	ship_sdb			pleb_special_data;
extern	formation_ship		formation [8];
extern	menu				menus [MAX_MENUS];
extern	TextCel				*cel_text [FONT_MAXMENU];
extern	long				cheat_feature2;
extern	BS_StickData 		joystick;

// Function Prototypes

void	control_initialisecameras (void);				// Initialise all game stuff on startup
void	control_startgame (long);						// Initialise a new game
void	control_startmission (void);					// Initialise a new mission

void	control_registerdeath (ship_stack*);			// A ship has died ! Do something
void	control_searchcamera (long, long);				// Set camera to an instance of a shiptype
void	control_changecamera (long);					// Change current camera
void	control_recheckcamera (ship_stack*);			// Recheck cameras when a ship is added
void	control_selectweapon (void);					// Select a new weapon for the player
void	control_gameover (void);						// Set gameover - display message etc.
void	control_retrack (void);							// Reset nearest / tracking objects if free format

void	control_setupfromconfigure (void);				// Sets up game from configuration file
long	control_saveconfigure (void);					// Saves configuration to NVRAM

long	control_showmission (long, long);				// Setup / Display mission brief
void	control_newlife (void);							// Adds a new life (checks for < 7 first)

#endif
