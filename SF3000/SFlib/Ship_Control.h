#include "ship_struct.h"

#define C_LEFT		1
#define C_RIGHT		2
#define C_UP		4
#define C_DOWN		8
#define C_THRUST	16
#define C_FIRE		32
#define ROT_LIMIT	((1024*1024)-1)

extern long cosine_table [ 2048 ] ;
extern long *sine_table ;

#include "Misc_Struct.h"
extern docking_struct docked ;

#include "SF_Status.h"
extern game_status status ;

// Get these for the satellite update
extern long ground_laser_type ;
extern long ground_laser_rate ;

extern ship_list ships ;

extern ship_sdb pleb_special_data ;

extern ship_stack * players_ship ;

extern mission_data mission ;

void ship_control( ship_stack * );

void big_ship_control( ship_stack * );

void satellite_control( ship_stack * );

void parachute_control( ship_stack * );

void car_control( ship_stack * );

void check_shippy_collision( ship_stack* ) ;

void update_docking( ship_stack* );

ship_stack* add_ship ( 	long , long , long ,
						long , long ,
						ship_sdb* ,
						performance_data* ) ;

//command_override for big ships
#define BIG_SHIP_COMMAND_NORMAL			0
#define BIG_SHIP_COMMAND_ROTATE_Z		1
#define BIG_SHIP_COMMAND_ROTATE_X		2
#define BIG_SHIP_COMMAND_STATIC			3

#define BIG_SHIP_COMMAND_STATIC_FREE	4

