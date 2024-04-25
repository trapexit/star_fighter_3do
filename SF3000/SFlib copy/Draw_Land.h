#include "SF_Mission.h"

extern planet_data planet_info ;

extern long camera_x_position ;
extern long camera_y_position ;
extern long camera_z_position ;
extern long camera_x_rotation ;
extern long camera_y_rotation ;
extern long camera_z_rotation ;
extern long camera_x_velocity ;
extern long camera_y_velocity ;
extern long camera_z_velocity ;

extern long test_mode ;

extern ship_stack* ship_being_viewed ;

extern long land_sort_offset ;

extern long pex_table [ 32768 ];

extern long air_to_ground_scan_temp ;
extern long air_to_ground_scan ;
extern long air_to_ground_x ;
extern long air_to_ground_y ;
extern long air_to_ground_z ;
extern long atg_selected ;

extern long air_to_air_scan_temp ;
extern long air_to_air_scan ;
extern long air_to_air_x ;
extern long air_to_air_y ;
extern long air_to_air_z ;
extern long ata_selected ;

extern	char	sky [1024];
extern	CCB*	skycel;
extern	cel_celdata		cel_quad;

extern long which_graphics_set ;

extern void plot_stars(void) ;
extern void setup_rotations(void) ;
extern void machine_code_land_plot(void) ;
extern void rotate_sky(void*);
extern void plot_ship_graphic(void*);

extern char height_map [ 256 ] [ 256 ] ;

extern long* temp_store ;
extern ship_list ships ;

void draw_land( void ) ;
