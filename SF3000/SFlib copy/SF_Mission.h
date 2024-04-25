/* File : Mission structure header file */

#ifndef __SF_MISSION
#define __SF_MISSION

/* Status structure defines */
	
#define	GROUND_SWEAPONBONUS1	1			// Definitions for ground & ship bonus/object types
#define	GROUND_SWEAPONBONUS2	2
#define	GROUND_SWEAPONBONUS3	3
#define	GROUND_SWEAPONBONUS4	4
#define	GROUND_ADDTODEFENCETIME	5
#define	GROUND_SETSHIPSHIELD	8
#define	GROUND_TOGGLETIMER		10
#define	GROUND_OBJECTJAMSCANNER	13

#define	GROUND_DESTROY 			0			// Definitions for just ground bonus/object types
#define	GROUND_BLOWUPPREVIOUS	6
#define	GROUND_NULL				7
#define	GROUND_RANDOMAWARD		9
#define	GROUND_FEDNETHIT		11
#define	GROUND_HIDDENDESTROY	12

/* Structure definitions */


typedef	struct	planet_data					// Info data for a planet type
				{
				long	explosion1_data1;
				long	explosion1_data2;
				long	explosion2_data1;
				long	explosion2_data2;
				char	explosion_heightcheck;
				char	space_mission;
				char	free1;
				char	free2;
				long	comet_rate;
				}	planet_data;

// --------------------

typedef struct	groundobject				// Ground object identifiers x,y pos, bonus etc
				{
				char	xpos;
				char	ypos;
				char	type;
				char	var;
				}	groundobject;

// --------------------

typedef	struct	ship_sdb					// Ship control structure
				{
				char	control_mode;		//	0
				char	alive;				//	1
				char	side;				//	2
				char	killstatus;			//	3
				
				char	flightpath_point;	//	4	
				char	command_override;	//	5	
				char	formation_position;	//	6
				char	fire_instruction;	//	7
				
				char	distance_check;		//	8	
				char	pilot_name;			//	9
				char	cloaking_device;	//	10
				char	start_direction;	//	11
				
				char	free1;				//	12
				char	bomb_todrop;		//	13	
				char	bomb_timer;			//	14
				char	ship_type;			//	15
				
				char	init_ref;			//	16	
				char	x_start;			//	17
				char	y_start;			//	18
				char	z_start;			//	19	
				
				char	sdb_type;			//	20
				char	target_type;		//	21
				char	target_counter;		//	22	
				char	max_canlaunch;		//	23
				
				void*	ship_address;		//	24
				void*	command_address;	//	28
				
				long	x_offset;			//	32	
				long	y_offset;			//	36	
				long	z_offset;			//	40	
				}	ship_sdb;

// --------------------

typedef	struct	flight_point						// Single flight-path point definition
				{
				char	x_pos;						// X position of flightpoint (0-255)
				char	y_pos;						// Y position of flightpoint (0-255)
				char	z_pos;						// Z position of flightpoint (0-255)
				char	command;					// Should we start dropping bombs at this flightpoint ?
				}	flight_point;
			
// --------------------

typedef	struct	flight_path							// flight path definition
				{
				long			flight_points;		// No. of flightpoints held in this flightpath
				flight_point	flight_data [64];	// Collection of flightpoints
				}	flight_path;
				
// --------------------
				
typedef struct 	performance_data					// Performance rating of a given ship
				{
				long	engine;						// Engine rating
				long	control;					// Control rating
				long	shields;					// Shields rating
				long	weapons [13];				// PLAYERS SHIP     /	COMPUTER SHIP | SATELLITE (Array of weapons in THIS ORDER)

													// LASER POWER		/	LASER POWER		1
													// NUM OF A-T-G		/	-------------	2
													// NUM OF A-T-A		/	-------------	3
													// MEGA-BOMBS		/	-------------	4
													// BEAM LASER		/	-------------	5
													// MULTI-MISSILES	/	-------------	6
													// MINES			/	-------------	7
													// FREE 1			/	-------------	8
													// FREE 2			/	-------------	9
													// FREE 3			/	-------------	10
													// FREE 4			/	-------------	11
													// WING PODS		/	-------------	12
													// -------------	/	MAX CAN LAUNCH	13

				long	laser_rate;					// How often to fire laser ?
				long	missile_rate;				// How often to fire missile ?
				long	launch_rate;				// How often to launch pleb ship ? (ALL out of 0-2047)

				}	performance_data;

// --------------------

typedef struct 	mission_data						// Complete data set for active mission
				{

				// Mission start data
				
				char	x_startpos;					// Start x pos (0-255)
				char	y_startpos;					// Start y pos (0-255)
				char	z_startpos;					// Start z pos (0-255)
				char	x_startrotation;			// Start rotation (0 - can't remember)
				
				// General freebies
				
				long	free1;
				long	free2;
				
				
				// Mission Yes / No's		(0=no, 1=yes)
				
				char	starting_docked;		// Do we start the mission docked ?
				char	dock_to_finish;			// Do we have to dock to finish the mission ?
				char	special_setup;			// Are we using a special ship setup for player ?
				char	scanner_bust;			// Is scanner (map) nackered ?
				char	demo_mission;			// Is this a demo mission ?
				
				char	free4;
				char	free5;
				char	free6;
				
				// Mission timers & Difficulty stuff
				
				long	mission_timer;			// How long to complete
				long	defence_timer;			// How long before defences active
				long	difficulty;				// Rate of ground fire / A-T-A rate / Ground Launch rate (0-1023)
				char	hangar_capacity;		// Number of ships in each hangar
				char	ground_lasertype;		// Type of laser fire from ground
				char	hits_allowed;			// How many hits allowed ?
				char	shiplaunchtype_ground;	// What type of ships are launched from the ground ?
				char	shiplaunchtype_air;		// What type of ships are launched from the air ?
				char	free14;
				char	free15;
				char	free16;	
								
				// Starting setup of players ship
				
				long	ship_flying;			// Ship we are flying in this mission
				
				performance_data	performances [10];	// Performances setup,
														// 0 .. Special setup for player
														// 1 .. 4 = small ships
														// 5 .. 6 = big ships
														// 7 .. Mothership
														// 8 .. Satellite
														// 9 .. Vehicles
				// Mission identification
				
				char	mission_number;					// 1 - 36 ?
				char	mission_level;					// 0 - 3 (Training, Easy, Medium, Hard)
				char	free17;
				char	free18;
				
				// Mission filenames
				
				char	planettype [16];				// Planet type, 'Earth', 'Chemical' etc
				char	skyfile	[16];					// Skyfile to use
				
				// Objects to destroy
				
				long			groundcount;			// Number of check objects to destroy
				groundobject	groundlist [128];		// List of upto 128 items
				
				// Flight path data
				
				long	flightpath_count;				// No. of flightpaths defined	
				flight_path	flight_paths [8];			// 8 flightpaths
				
				// Ship control data
				
				long	special_ships;					// No. of special mission ships to add
				ship_sdb ship_special [32];				// 32 special control datablocks
				
				// Mission title & texts
				
				char	title [32];						// Mission title
				char	location [20];					// Location
				char	variation [20];					// Variation
				} mission_data;
#endif
