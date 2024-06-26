#ifndef __BIT_SET_H
#define __BIT_SET_H

#include "SF_ArmLink.h"

#define MAX_BITS 64

typedef struct bit_stack 
					{	link_header header ;
						long x_pos ; long y_pos ; long z_pos ;		// 0-8
						long x_rot ; long y_rot ; long z_rot ;		// 12-20
						long type ;									// 24	
						long colour1 ; long who_owns_me ;				// 28-32
						long x_vel ; long y_vel ; long z_vel ;		// 36-44
						long x_r_vel ; long y_r_vel ; long z_r_vel ;// 48-56
						long style ; long counter ;					// 60-64
					} bit_stack
					;

typedef struct bit_list {
						linked_list info ;
						bit_stack bit_item [ MAX_BITS ] ;
						} bit_list
						;

#endif
