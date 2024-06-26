/* File : Mission structure header file */

#ifndef __SF_MISSION
#define __SF_MISSION

/* Status structure defines */

#define	GROUND_DESTROY 			0			// Definitions for ground object types
#define	GROUND_SWEAPONBONUS1	1
#define	GROUND_SWEAPONBONUS2	2
#define	GROUND_SWEAPONBONUS3	3
#define	GROUND_SWEAPONBONUS4	4
#define	GROUND_ADDTODEFENCETIME	5
#define	GROUND_BLOWUPPREVIOUS	6
#define	GROUND_NULL				7
#define	GROUND_SETSHIPSHIELD	8
#define	GROUND_CASHAWARD		9
#define	GROUND_TOGGLETIMER		10
#define	GROUND_HIT				11
#define	GROUND_HIDDENDESTROY	12

#define	KILL_NULL				0			// Definitions for ship kill status
#define KILL_MUSTDIE			1
#define	KILL_MUSTSURVUVE		2
#define KILL_MUSTSURVIVEFLIGHT	3
#define KILL_MUSTSTOPFLIGHT		4

/* Structure definitions */

typedef struct	groundobject				// Ground object identifiers x,y pos, bonus etc
				{
				char	xpos;
				char	ypos;
				char	type;
				char	var;
				}	groundobject;
				
typedef struct 	performance					// Performance rating of a given ship
				{
				long	lives;
				long	engine;
				long	control;
				long	shields;
				long	ata;
				long	atg;
				
				long	superweapon1;
				long	superweapon2;
				long	superweapon3;
				long	superweapon4;
				}	performance;
				
typedef struct 	mission						// Complete data set for active mission
				{

				// Mission start data
				
				long	x_startpos;
				long	y_startpos;
				long	z_startpos;
				long	start_xrotation;
				
				long	free1;
				long	free2;
				long	free3;
				long	free4;
				
				// Mission yes / no's		(0=no, 1=yes)
				
				char	starting_docked;
				char	dock_to_finish;
				char	special_setup;
				char	space_mission;
				char	scanner_bust;
				char	free5;
				char	free6;
				char	free7;
				
				char	free8;
				char	free9;
				char	free10;
				char	free11;
				
				// Mission timers
				
				long	missiontimer;		// How long to complete
				long	defencetimer;		// How long before defences active
				long	free12;
				long	free13;
				
				// Mission Difficulty stuff
				
				char	difficulty;			// How difficult ?
				char	hits_allowed;		// How many hits allowed ?
				char	free14;
				char	free15;
				
				// Starting setup of players ship
				
				performance	initial_setup;	// Initial performance setup
				
				long	ship_flying;		// Ship we are flying in this mission
				
				// Mission identification
				
				char	mission_number;		// 1 - 36 ?
				char	mission_level;		// 0 - 3 (Training, Easy, Medium, Hard)
				char	free16;
				char	free17;
				
				// Mission filenames
				
				char	planettype [16];	// Planet type, 'Earth', 'Chemical' etc
				char	skyfile	[16];		// Skyfile to use
				
				// Objects to destroy
				
				long	groundcount;				// Number of check objects to destroy
				groundobject	groundlist [128];	// List of upto 128 items
				
				} mission;


/* Function Prototypes */


#endif
