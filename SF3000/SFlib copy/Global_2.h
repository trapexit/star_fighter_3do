
// Setup various structures
#include "Ship_Struct.h"
ship_list ships ;
ship_stack *players_ship ;

#include "Smoke_Struct.h"
smoke_list smokes ; 

#include "Camera_Struct.h"
camera_data camera [ MAX_CAMERAS ] ;

#include "Laser_Struct.h"
laser_list lasers ;

laser_stack *beam_lasers [ 32 ] ;
beam_laser_data beam_laser ;

#include "Bit_Struct.h"
bit_list bits ;

#include "Explosion_Struct.h"
explosion_list explosions ;

#include "Misc_Struct.h"

// Docking stuff for the players ship
docking_struct docked;

// Bonus crystal stuff
bonus_crystal_adder_struct bonus_crystal_adder ;
bonus_crystal_hill_struct bonus_crystal_hill ;
bonus_crystal_object_struct bonus_crystal_object ;
long bonus_collision_this_frame = 0 ;

// Players control rates
long players_x_control = 0 ;
long players_y_control = 0 ;
long players_z_control = 0 ;
long players_thrust_control = 0 ;

// Two nearest ships of this type make engine sounds
ship_stack* sound_big_ship ;
ship_stack* sound_small_ship ;
long sound_channel_small_ship = -1 ;
long sound_channel_big_ship = -1 ;
long sound_sample_small_ship = 0 ;
long sound_channel_shields_low  = -1 ;
long sound_channel_beam_laser  = -1 ;
long engine_sounds_on_or_wot = 0 ;

ship_stack* ship_being_viewed = 0 ;

// Rate of fire for ground objects - chances in 1024
long ground_laser_rate = 512 ;
long ground_sam_rate = 0 ;
long ground_ship_rate = 8 ;

long ground_laser_type = 2 ;
long ground_ship_type = 0 ;
long air_ship_type = 0 ;

// Toggle for the ground lasers
long laser_counter ;

long test_mode = 0 ;

// Misc counters etc.
long pod_counter = 0 ;

long which_graphics_set ;


// Setup data tables for quick reference look up
long cosine_table [ 2048 ] ;
long *sine_table = &cosine_table [ 256*3 ] ;
long pex_table [ 32768 ] ;
long quick_height_table [ 256 ] [ 4 ] ;
char tangent_table [ 4100 ] ;

// Collision table for static ground obejcts
char collision_map [ 128 ] [ 128 ] ;

// Setup data tables for the stars
long star_coords [ 128 ] [ 4 ] ;


// Setup temp workspace for rotated coords and screen coords

// For use by the landscape rotator
long rotated_coords [ 4096 ] [ 3 ] ;
long screen_coords [ 4096 ] [ 2 ] ;

// For use by the graphics - ships , static objects , explosion bits etc.
long graphic_rotated_coords [ 256 ] [ 4 ] ;
long graphic_screen_coords [ 256 ] [ 2 ] ;

// Temp store for misc use
long *temp_store = &graphic_rotated_coords [ 0 ] [ 0 ] ; 


// General store for all polygon graphics - ships , static objects , explosion bits
// This contains all coord data + polygon link data
// All explosion data and collision data
// Misc items such as score / hits counter etc.
long graphics_data [ 20000 ] ;

// Global pointers to items in the graphics table
long *static_graphics_adr ;
long *ships_adr ;
long *explosion_bits_adr ;


// Is the 3DO quicker than the arm tests
//mat33f16 matrix ;
//vec3f16 pre_rot [ 128 ] ;
//vec3f16 post_rot [ 128 ] ;
