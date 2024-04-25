#ifndef __MISC_SET_H
#define __MISC_SET_H

#include "Ship_Struct.h"

// Structure for preparing the landscape node rotations
typedef struct rot_struct 
					{	long x_pos ; long y_pos ;
						long offset_x ;
						long offset_y ;
						long offset_z ;
					} rot_struct
					;

// Structure for finding target rotations based on position
// of item to fire from and item to fire at
typedef struct target_struct 
					{	long x_pos ; long y_pos ; long z_pos ;
						long x_aim ; long y_aim ; long z_aim ;
						long x_rot ;
						long y_rot ;
						long distance ;
					} target_struct
					;

// Structure for the players docking details
typedef struct docking_struct 
					{	long docked_this_frame ;
						ship_stack* ship ;
						long counter ;
						long status ;
					} docking_struct
					;

#define		DOCKING_OUT		0
#define		DOCKING_ENTER	1
#define		DOCKING_EXIT	2
#define		DOCKING_FIXED	3

// Structure for bonus crystal adding
typedef struct bonus_crystal_adder_struct
					{	long x_pos ;
						long y_pos ;
						long z_pos ;
						long counter ;
					} bonus_crystal_adder_struct
					;

// Structure for bonus crystal under hills
typedef struct bonus_crystal_hill_struct
					{	long x_grid_1 ;
						long y_grid_1 ;
						long x_grid_2 ;
						long y_grid_2 ;
						long x_grid_3 ;
						long y_grid_3 ;
						long x_grid_4 ;
						long y_grid_4 ;
					} bonus_crystal_hill_struct
					;

// Structure for bonus crystal object destroying
typedef struct bonus_crystal_object_struct
					{	long object_1 ;
						long object_2 ;
						long object_3 ;
						long object_4 ;
					} bonus_crystal_object_struct
					;

#define ROT_LIMIT	((1024*1024)-1)


#endif
