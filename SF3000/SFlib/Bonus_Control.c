#include "Bonus_Control.h"
#include "Ship_Control.h"
#include "Stdio.h"
#include "Collision_Update.h"
#include "Collision.h"
#include "Graphics_Set.h"
#include "Graphic_Struct.h"
#include "SF_ArmUtils.h"
#include "String.h"
#include "Graphics_Set.h"

//################################################
//#                                              #
//#     Bonus control and decision maker 	     #
//#                                              #
//################################################

void bonus_control( ship_stack * bonus )

{
long coll_check ;
long spin_rate = (128-bonus->counter) ;

if ( spin_rate < 16 ) spin_rate = 16 ;
spin_rate = spin_rate << 9 ;


// Spin on the x depending on type which way
if ( ((bonus->type)&1) == 0)
{
	bonus->x_rot = ((bonus->x_rot)-spin_rate)&ROT_LIMIT ;
}
else
{
	bonus->x_rot = ((bonus->x_rot)+spin_rate)&ROT_LIMIT ;
}

// add on velocitys
bonus->x_pos += bonus->x_vel ;
bonus->y_pos += bonus->y_vel ;
bonus->z_pos += bonus->z_vel ;

// slow down the velocitys quickly
bonus->x_vel -= (bonus->x_vel>>4) ;
bonus->y_vel -= (bonus->y_vel>>4) ;
bonus->z_vel -= (bonus->z_vel>>4) ;

// Dec counter
bonus->shields -= 1 ;

if (bonus->what_hit_me != 0) bonus_collision( bonus );

if (which_graphics_set != SPACE_GRAPHICS)
{
	// Rise slowly
	bonus->z_pos += (1<<18) ;

	coll_check = find_ground_height(bonus->x_pos,bonus->y_pos) ;
	if (coll_check>bonus->z_pos)
	{
		bonus->z_pos = coll_check ;
		if (bonus->z_vel < 0) bonus->z_vel = -bonus->z_vel ;
	}
}



}


void add_crystal_from_static( long x_grid , long y_grid , long chance )
{
long x_pos , y_pos , z_pos , type ;

graphics_details *details = (graphics_details*) static_graphics_adr ;

type = poly_map [ y_grid ] [ x_grid ] ;


x_pos = (unsigned) x_grid<<25 ;
y_pos = (unsigned) y_grid<<25 ;

z_pos = ((height_map [ (y_grid<<1) ] [ (x_grid<<1) ])-17)<<21 ;
	
if (z_pos < 0) z_pos = 0 ;
z_pos += ((details+type)->missile_aim) ;

	
switch ( chance )
{

case REF_STATIC_CRYSTAL_CHANCE_VERY_POOR :

if ( ((arm_random())&7) == 0 )
{
	add_bonus ( 	x_pos , y_pos , z_pos ,
					0 , 0 , (1<<22) ,
					((arm_random())&1)
					) ;
}

break ;

case REF_STATIC_CRYSTAL_CHANCE_POOR :

if ( ( ((arm_random())&3) + ((arm_random())&1) ) == 0 )
{
	add_bonus ( 	x_pos , y_pos , z_pos ,
					0 , 0 , (1<<22) ,
					((arm_random())&3)
					) ;
}

break ;



case REF_STATIC_CRYSTAL_CHANCE_AVERAGE :

if ( ((arm_random())&3) == 0 )
{
	add_bonus ( 	x_pos , y_pos , z_pos ,
					0 , 0 , (1<<22) ,
					( ((arm_random())&3) + ((arm_random())&1) )
					) ;
}

break ;

case REF_STATIC_CRYSTAL_CHANCE_GOOD :

if ( ((arm_random())&3) == 0 )
{
	add_bonus ( 	x_pos , y_pos , z_pos ,
					(1<<22) , 0 , (1<<22) ,
					((arm_random())&7)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					-(1<<22) , 0 , (1<<22) ,
					((arm_random())&7)
					) ;
}

break ;


case REF_STATIC_CRYSTAL_CHANCE_VERY_GOOD :
if ( ((arm_random())&1) == 0 )
{
	add_bonus ( 	x_pos , y_pos , z_pos ,
					(1<<22) , (1<<22) , (1<<22) ,
					((arm_random())&3)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					-(1<<22) , (1<<22) , (1<<22) ,
					((arm_random())&7)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					(1<<22) , -(1<<22) , (1<<22) ,
					((arm_random())&7)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					-(1<<22) , -(1<<22) , (1<<22) ,
					((arm_random())&3)
					) ;
}

break ;

case REF_STATIC_CRYSTAL_CHANCE_VERY_VERY_GOOD :

	add_bonus ( 	x_pos , y_pos , z_pos ,
					(1<<22) , (1<<22) , (1<<22) ,
					((arm_random())&7)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					-(1<<22) , (1<<22) , (1<<22) ,
					((arm_random())&7)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					(1<<22) , -(1<<22) , (1<<22) ,
					((arm_random())&7)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					-(1<<22) , -(1<<22) , (1<<22) ,
					((arm_random())&7)
					) ;
	add_bonus ( 	x_pos , y_pos , z_pos ,
					0 , 0 , (1<<22) ,
					((arm_random())&7)
					) ;
break ;


}

}

void start_bonus_crystal_adder( long x_grid, long y_grid )
{

long height ;

height = ((height_map [ y_grid<<1 ] [ x_grid<<1 ])<<21)+(1<<24) ;

bonus_crystal_adder.x_pos = (unsigned) x_grid<<25 ;
bonus_crystal_adder.y_pos = (unsigned) y_grid<<25 ;
bonus_crystal_adder.z_pos = height ;
bonus_crystal_adder.counter = 1024 ;

poly_map [ y_grid ] [ x_grid ] = OBJECT_BONUS_CRYSTAL_ADDER ;


}

void update_bonus_crystal_adder( void )
{

if (bonus_crystal_adder.counter >= 0)
{
	if ( (bonus_crystal_adder.counter&31) == 0)
	{
		add_bonus ( 	bonus_crystal_adder.x_pos ,
						bonus_crystal_adder.y_pos ,
						bonus_crystal_adder.z_pos ,
						0 , 0 , (1<<22) ,
						((arm_random())&7)
						) ;
	}
	bonus_crystal_adder.counter -= 1 ;
	
	if (bonus_crystal_adder.counter == 0)
	{
		poly_map	[ (unsigned) bonus_crystal_adder.y_pos>>25 ]
					[ (unsigned) bonus_crystal_adder.x_pos>>25 ]
				= OBJECT_NOTHING ;
	}
}

}



void add_bonus ( 	long x_pos , long y_pos , long z_pos ,
					long x_vel , long y_vel , long z_vel ,
					long type
					)
	
{

ship_stack *bonus ;

bonus = armlink_sorttolist( &ships , x_pos ) ;
if (bonus != NULL)
{

memset( (void*) ( ((char*) bonus) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

bonus->x_pos = x_pos ;
bonus->y_pos = y_pos ;
bonus->z_pos = z_pos ;
bonus->x_vel = x_vel ;
bonus->y_vel = y_vel ;
bonus->z_vel = z_vel ;
bonus->x_rot = (arm_random())&ROT_LIMIT ;
bonus->y_rot = 0 ;
bonus->z_rot = 0 ;
bonus->type = (BONUS<<4)+type ;
bonus->shields = 128+((arm_random())&127) ;	// Life counter
bonus->collision_size = 1<<23 ;
bonus->what_hit_me = 0 ;
bonus->special_data = NULL ;
bonus->who_owns_me = 0 ;

}

}

void add_bonus_from_collision_box( long ref , long grid_ref )
{

long x_pos , y_pos , z_pos , type , loop ;
collision_details_header* coll_data_header ;
collision_details* coll_data ;
graphics_details *details = (graphics_details*) static_graphics_adr ;

static long toggle = 7 ;

if ( bonus_collision_this_frame > 0 ) return ;

bonus_collision_this_frame = 32 ;

x_pos = grid_ref&127 ;
y_pos = grid_ref >> 7 ;

// Get height of this object - less than 17 = sea
z_pos = (long) ( ( (height_map [ (y_pos<<1) ] [ (x_pos<<1) ])-17 )<<21 ) ;
if (z_pos<0) z_pos = 0;

type = (long) poly_map [ y_pos ] [ x_pos ] ;

x_pos = (unsigned) (x_pos<<25) ;
y_pos = (unsigned) (y_pos<<25) ;

coll_data_header = (collision_details_header*) (details+type)->collision_adr ;

coll_data = (collision_details *) (coll_data_header + 1) ;

for ( loop = 0 ; loop <= coll_data_header->counter ; loop +=1 )
{
	if ( coll_data->type == ref )
	{
		x_pos += ((coll_data->x_pos+coll_data->x_pos2)>>1) ;
		y_pos += ((coll_data->y_pos+coll_data->y_pos2)>>1) ;
		z_pos += ((coll_data->z_pos+coll_data->z_pos2)>>1) ;	
		break;
	}
	coll_data += 1 ;
}

add_bonus ( x_pos ,
			y_pos ,
			z_pos ,
			0 , 0 , (1<<22) ,
			((toggle-(ref-2))&7)
			) ;


toggle = 7 - toggle ;

}


