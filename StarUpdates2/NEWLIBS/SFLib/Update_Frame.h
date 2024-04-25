
#include "Ship_Struct.h"
#include "ship_control.h"
#include "SF_CelUtils.h"
//#include "graphics.h"
#include "camera_struct.h"
#include "SF_Status.h"

#include "SF_Parameters.h"
extern game_configuration configuration ;


#include "Misc_Struct.h"
extern docking_struct docked ;

#include "SF_Joystick.h"
extern stick_struct pro_stick ;

/* External Definitions */

// bugggggg fix
extern long laser_sound_counter ;

extern void rotate_node_from_c(void*);
extern long wave_counter ;
extern long wave_counter2 ;
extern long wave_counter3 ;
extern long camera_x_rotation ;
extern long camera_y_rotation ;
extern long camera_x_position ;
extern long camera_y_position ;
extern long camera_z_position ;
extern long test_grid_flip ;
extern struct camera_data camera [ 3 ] ;

extern game_status status ;
extern long pod_counter ;
extern long* temp_store ;

extern long laser_counter ;

#include "SF_Mission.h"
extern planet_data planet_info ;

extern long players_x_control ;
extern long players_y_control ;
extern long players_z_control ;
extern long players_thrust_control ;

extern long keypad_iomap[8] ;

extern long bonus_collision_this_frame ;

extern long test_mode ;

// Shitty get it ready for another show fix
extern long plot_clouds_or_wot ;

extern ship_stack *players_ship ;

/* Local Function prototypes */

void update_frame ( long ) ;
