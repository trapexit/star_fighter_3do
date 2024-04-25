#ifndef __LASER_SET_H
#define __LASER_SET_H

#include "SF_ArmLink.h"

#define MAX_LASERS 256

typedef struct laser_stack 
					{	link_header header ;
						long x_pos ; long y_pos ; long z_pos ;
						long x_pos2 ; long y_pos2 ; long z_pos2 ;
						long type ;					// byte 24
						long counter ;				// byte 28
						long who_owns_me ;			// byte 32
						
						long x_vel ; long y_vel ; long z_vel ;
					} laser_stack
					;

typedef struct laser_list {
						linked_list info ;
						laser_stack laser_item [ MAX_LASERS ] ;
						} laser_list
						;

typedef struct beam_laser_data {
								long on_last_frame ;
								long x_rot_bend ;
								long y_rot_bend ;
								} beam_laser_data
								;



#endif
