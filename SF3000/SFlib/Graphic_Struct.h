#ifndef __GRAPHIC_SET_H
#define __GRAPHIC_SET_H

// General structure to get addresses of data for a graphic

typedef struct graphics_details 
					{
					long clip_size ;		// byte 0
					long score ;			// byte 4
					long graphic_adr ;		// byte 8
					long explosion_adr ;	// byte 12
					
					long collision_adr ;	// byte 16
					long hits_counter ;		// byte 20
					long x_size ;			// byte 24
					long y_size ;			// byte 28
					
					long z_size ;			// byte 32
					long laser_data ;		// byte 36
					long missile_data ;		// byte 40
					long ship_data ;		// byte 44
					
					long smoke_data ;		// byte 48
					long thruster_data ;	// byte 52
					long missile_aim ;		// byte 56
					long extra_data ;		// byte 60
					
					} graphics_details
					;

// Explosion line data from graphic structure
// Used when a graphic is being blown up

typedef struct explosion_details_header 
					{
					long counter ;
					long type ;
					long free ;
					} explosion_details_header
					;

typedef struct explosion_details 
					{
					long x_pos ; long y_pos ; long z_pos ;
					long x_pos2 ; long y_pos2 ; long z_pos2 ;
					long colour1 ; long colour2 ;
					long bonus ;
					} explosion_details
					;

typedef struct collision_details_header 
					{
					long counter ;
					long free ;
					long free2 ;
					} collision_details_header
					;

typedef struct collision_details 
					{
					char type ; char colour ; char group ; char nowt ;
					long x_pos ; long y_pos ; long z_pos ;
					long x_pos2 ; long y_pos2 ; long z_pos2 ;
					} collision_details
					;

// Smoke data from graphic structure
// Used when a ship is damaged and smoking 
// or a static graphic with a smoking chimney

typedef struct smoke_details_header 
					{
					long counter ;
					} smoke_details_header
					;

typedef struct smoke_details 
					{
					long x_pos ;
					long y_pos ;
					long z_pos ;
					} smoke_details
					;

// Thruster data from graphic structure
// Used when a ship has its thruster on

typedef struct thruster_details_header 
					{
					long counter ;
					} thruster_details_header
					;

typedef struct thruster_details 
					{
					long x_pos ;
					long y_pos ;
					long z_pos ;
					} thruster_details
					;


// Laser data from graphic structure
// Used when a ship / static graphic fires its lasers

typedef struct laser_details_header 
					{
					long counter ;
					} laser_details_header
					;

typedef struct laser_details 
					{
					long x_pos ;
					long y_pos ;
					long z_pos ;
					} laser_details
					;

// Missile data from graphic structure
// Used when a ship / static graphic fires its missiles

typedef struct missile_details_header 
					{
					long counter ;
					} missile_details_header
					;

typedef struct missile_details 
					{
					long x_pos ;
					long y_pos ;
					long z_pos ;
					} missile_details
					;

// Ship data from graphic structure
// Used when a ship / static graphic launchs a ship

typedef struct ship_details 
					{
					long type ;
					long x_pos ;
					long y_pos ;
					long z_pos ;
					} ship_details
					;

#define SHIP_SOUTH_LAUNCH	0
#define SHIP_RUNWAY_TAKEOFF	1


// Explosion section data
typedef struct section_groups 
					{
					long group_0 ;
					long group_1 ;
					long group_2 ;
					long group_3 ;
					long group_4 ;
					long group_5 ;
					long group_6 ;
					} section_groups
					;

typedef struct section_details_header 
					{
					long counter ;
					} section_details_header
					;

typedef struct section_details
					{
					long type ;
					long x_pos ;
					long y_pos ;
					long z_pos ;
					long x_pos2 ;
					long y_pos2 ;
					long z_pos2 ;
					long x_vel ;
					long y_vel ;
					long z_vel ;
					} section_details
					;

#define SECTION_SMALL_THRUST	0
#define SECTION_SMALL_SMOKE		1
#define SECTION_BIG_THRUST		2
#define SECTION_BIG_SMOKE		3


#endif
					

extern long *static_graphics_adr ;
extern long *ships_adr ;
extern long *explosion_bits_adr ;
extern long graphics_data [ 19000 ] ;
extern char poly_map [ 128 ] [ 128 ] ;
extern char collision_map [ 128 ] [ 128 ] ;
extern char height_map [ 256 ] [ 256 ] ;
