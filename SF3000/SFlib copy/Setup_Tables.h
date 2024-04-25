#include "Ship_Struct.h"
#include "SF_CelUtils.h"
#include "Camera_Struct.h"
#include "SF_Mission.h"

#define COLL_UP_RIGHT	248
#define COLL_UP			249
#define COLL_UP_LEFT	250
#define COLL_LEFT		251
#define COLL_DOWN_LEFT	252
#define COLL_DOWN		253
#define COLL_DOWN_RIGHT	254
#define COLL_RIGHT		255

extern long cosine_table [ 2048 ];
extern long pex_table [ 32768 ];
extern long rotated_coords [ 4096 ] [ 3 ];

extern char height_map [ 256 ] [ 256 ];
extern char sprite_map [ 256 ] [ 256 ];
extern char poly_map [ 128 ] [ 128 ];

extern long screen_coords [ 4096 ] [ 2 ];
extern long graphic_rotated_coords [ 256 ] [ 4 ] ;
extern long graphic_screen_coords [ 256 ] [ 2 ] ;
//extern long plot_graphic_delay [ 8 ] [ 128 ] ;
extern long quick_height_table [ 256 ] [ 4 ] ;
extern char collision_map [ 128 ] [ 128 ] ;

extern long camera_x_velocity ;
extern long camera_y_velocity ;
extern long camera_z_velocity ;

#include "Misc_Struct.h"
extern docking_struct docked ;

#include "SF_Mission.h"
extern mission_data mission ;

#include "SF_Status.h"
extern game_status status ;

extern long star_coords [ 128 ] [ 4 ] ;

extern long *sine_table ;

extern ship_stack *players_ship ;

extern cel_celdata cel_quad ;
extern char skyfile [ 1024 ] ;

extern ship_list ships ;
extern camera_data camera [ MAX_CAMERAS ] ;

extern void machine_code_constants(void*);
extern void plot_land_constants(void*);

void setup_tables( void );

extern planet_data planet_info ;

extern long ground_ship_rate ;
extern long ground_sam_rate ; 
extern long ground_laser_rate ;
extern long ground_ship_type ;
extern long ground_laser_type ;
extern long air_ship_type ;

extern long test_mode ;

extern long are_we_in_space_or_wot ;

extern long planet_1_x_pos ;
extern long planet_1_y_pos ;
extern long planet_1_z_pos ;

extern long planet_2_x_pos ;
extern long planet_2_y_pos ;
extern long planet_2_z_pos ;

extern bonus_crystal_object_struct bonus_crystal_object ;

extern long which_graphics_set ;

extern long players_x_control ;
extern long players_y_control ;
extern long players_thrust_control ;


