// File : Global.h - Global Game Declarations

#ifndef __SF_GLOBAL
#define __SF_GLOBAL

// INCLUDES

#include "SF_Menu.h"
#include "SF_Pyramid.h"
#include "SF_Celutils.h"
#include "SF_Font.h"

// DEFINITIONS

#define	DISPLAY_NORMAL		0							// Don't do any altering
#define DISPLAY_MENU		1							// Just displaying normal menu
#define DISPLAY_HISCORES	2							// Displaying hi-score table ?

// EXTERNAL VARIABLES

extern	gamefile	save_gamefile;						// Game saver
extern	pyramid		pyramids [4];						// Pyramids array
extern	menu_info	menu_status;						// Menu status
extern	menu		menus [MAX_MENUS];					// Menus
extern	TextCel		*cel_text [FONT_MAXMENU];			// Cels for Menu entries
extern	message		messages [FONT_MAXMESSAGE];			// Text cels for In-game messages
extern	alpha_char	cel_character;						// For adding characters to char map
extern	long		cockpit_missiles;					// Number of missiles attacking player
extern	long		cockpit_enemybig;					// Number of enemy big ships
extern	long		cockpit_enemysmall;					// Number of enemy fighters attacking player
extern	long		keypad_iomap [8];					// Keypad control map

// LOCAL FUNCTION PROTOTYPES

void game_initialise (void);							// Initialise game at start
void game_end (void);									// Terminates program, quits
void game_intro (void);									// Displays fednet logo, start video etc.
long game_missionstart (long);							// Displays mission brief, calls game_play
void game_play (long);									// Main mission playing routine
void game_failed (void);								// Displays failure / hiscore check etc.
void game_debrief (void);								// Mission successful
void game_checkforhighscore (long);						// Check for a high score
void game_demo (void);									// Run demo mission
void menuswitch_update (long);							// Check for change of menu display
void menuswitch_next (long nextdisplaytype);			// Switches menu modes (hiscore - menu - demo - credits)
void game_pyramidcomplete (void);						// Registers a pyramid as completed

#endif
	
