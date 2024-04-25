#ifndef __SMOKE_SET_H
#define __SMOKE_SET_H

#include "SF_ArmLink.h"

#define MAX_SMOKES 256

typedef struct smoke_stack 
					{	link_header header ;
						long x_pos ; long y_pos ; long z_pos ;	// bytes 0-11
						long x_vel ; long y_vel ; long z_vel ;	// bytes 12-23
						long counter ;							// bytes 24-27
						long colour ;							// bytes 28-31
						long type ;								// bytes 32-35
					} smoke_stack
					;

typedef struct smoke_list {
						linked_list info ;
						smoke_stack smoke_item [ MAX_SMOKES ] ;
						} smoke_list
						;

#endif
