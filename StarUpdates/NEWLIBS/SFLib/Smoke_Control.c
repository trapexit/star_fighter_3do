#include "Smoke_Control.h"
#include "SF_ArmLink.h"
#include "Stdio.h"
#include "Collision.h"
#include "Maths_Stuff.h"
#include "Graphic_Struct.h"
#include "Plot_Graphic.h"
#include "SF_ArmUtils.h"
#include "Graphics_Set.h"

void smoke_update(void)

{

long coll_check ;
smoke_stack *smoke ;
smoke_stack *smoke_temp ;

smoke=(smokes.info).start_address ;

while ((smoke->header).status==1)
{

 // slow down the smokes velocity
 //smoke->x_vel -= ((smoke->x_vel)>>6) ; 
 //smoke->y_vel -= ((smoke->y_vel)>>6) ;
 //smoke->z_vel -= ((smoke->z_vel)>>6) ;

 // add velocity onto x ,y ,z positions
 //smoke->x_pos += (smoke->x_vel) ;
 //smoke->y_pos += (smoke->y_vel) ;
 //smoke->z_pos += (smoke->z_vel) ;

 // update the counter for the smoke when <0 then delete
 //smoke->counter -= 1 ;

// All the above is replaced in machine code
 mc_smoke_mover( smoke );


// Certain types of smoke add more smoke
if (	smoke->type==BIG_MUSHROOM_CLOUD ||
		smoke->type == SMALL_MUSHROOM_CLOUD
	) update_mushroom( smoke ) ;


 // Get the adr of the next before any deleting is done
 smoke_temp=(smoke->header).next_address ;

	if ( (smoke->counter) <= 0 )
	{
	// delete the smoke
	armlink_deleteitem(smoke,&smokes);
	}

// Has the smoke hit the ground - if so bounce off

if (which_graphics_set != SPACE_GRAPHICS)
{
	coll_check = find_ground_height(smoke->x_pos,smoke->y_pos) ;
	if (coll_check>smoke->z_pos)
	{
		smoke->z_pos = coll_check ;
		if ( smoke->z_vel < 0 ) smoke->z_vel = - smoke->z_vel ;
	}
}

 smoke=smoke_temp ;
}

}

void update_mushroom( smoke_stack *smoke )
{
long x_pos , y_pos , z_pos ;
long loop , loop2 ;
long size ;
long counter ;
long x_rot , x_rot_step ;

// Step though 2 sections at a time with the smoke counter as base

for (loop2 = ((7-smoke->counter)<<1) ; loop2 <= (((7-smoke->counter)<<1)+1) ; loop2 +=1 )
{
	// Reset the x rotation per new section
	x_rot = 0 ;

	if (loop2 < 8)
	{
	// Setup the details for the stem of the mushroom
		z_pos = (loop2 << 24) ;
		size = (1<<(24-12)) ;
		
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		// ####################  changed !!!!!!!!!!!!!!!!!!
		counter  = 2 ;
		x_rot_step = (1024/2) ;
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		// ##########################################	
	}
	else
	{
	// Setup the details for the cloud at top
		z_pos = (8<<24) + ((sine_table [ (loop2-8)<<7 ])<<12) ;
		size = (1<<(24-12)) + (sine_table [ (loop2-8)<<6 ]<<2) ;

		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		// ####################  changed !!!!!!!!!!!!!!!!!!
		counter = 8 ;
		x_rot_step = (1024/8) ;
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		//##################################################
		//##################################################		
		// ##########################################
	}

	if (smoke->type == SMALL_MUSHROOM_CLOUD)
	{
	z_pos = z_pos>>1 ;
	size = size>>1 ;
	counter = counter>>1 ;
	x_rot_step = x_rot_step<<1 ;
	}
	
	z_pos += smoke->z_pos ;

	for ( loop = 0 ; loop <= counter ; loop +=1 )
	{

		x_pos = smoke->x_pos + (size * cosine_table [ x_rot ]) ;
		y_pos = smoke->y_pos + (size * sine_table [ x_rot ]) ;

		add_smoke( x_pos , y_pos , z_pos ,
					0 ,0 ,0 ,
					MUSHROOM_SMOKE ,
					loop2>>1 ) ;

		x_rot += x_rot_step ;

	}

}


}



void add_smoke( long x_pos , long y_pos , long z_pos ,
				long x_vel , long y_vel , long z_vel ,
				long type , long colour)
				
{

long coll_check ;
long counter ;
smoke_stack *smoke ;
long size ;

smoke_stack* temp_smoke = NULL ;
smoke_stack* new_smoke = NULL ;
long new_smoke_val = (1<<20) ;




switch ( type )
{

case SMALL_THRUSTER_SMOKE :

counter = 8+(arm_random()&3) ;
colour += BASE_THRUSTER_COLOUR ;

break ;

case EXPLOSION_SMOKE :

counter = 8+(arm_random()&7) ;
colour = BASE_EXPLOSION_COLOUR + (arm_random()&3) ;

break ;

case BIG_MUSHROOM_CLOUD :

counter = 8 ;
colour = BASE_EXPLOSION_COLOUR ;

break ;

case SMALL_MUSHROOM_CLOUD :

counter = 8 ;
colour = BASE_EXPLOSION_COLOUR ;

break ;

case MUSHROOM_SMOKE :

counter = 16+(arm_random()&7) ;

if (colour>8) colour =58 ; else
colour += BASE_MUSHROOM_COLOUR ;

break ;

case MISSILE_SMOKE :

counter = 4+(arm_random()&3) ;
colour = BASE_MISSILE_COLOUR ;

break ;

case BIG_THRUSTER_SMOKE :

counter = 8+(arm_random()&3) ;
colour = (BASE_THRUSTER_COLOUR + 4) + (arm_random()&3) ;

break ;

case BIG_DAMAGE_SMOKE :

counter = 8+(arm_random()&3) ;
colour = BASE_DAMAGE_COLOUR ;

break ;

case WING_POD_SMOKE :

counter = 4+(arm_random()&3) ;
colour = BASE_WING_POD_COLOUR ;

break ;

case LASER_HIT_SMOKE :

counter = 7 ;
colour = BASE_LASER_HIT_COLOUR ;

break ;

case BLACK_EXPLOSION_SMOKE :

counter = 8+(arm_random()&7) ;
colour = BASE_DAMAGE_COLOUR ;

break ;

case SMALL_EXPLOSION_SMOKE :

counter = 8+(arm_random()&7) ;
colour = BASE_EXPLOSION_COLOUR + (arm_random()&3) ;

break ;

case BIG_EXPLOSION_SMOKE :

counter = 8+(arm_random()&7) ;
colour = BASE_EXPLOSION_COLOUR + (arm_random()&3) ;

break ;

case SMALL_DAMAGE_SMOKE :

counter = 8+(arm_random()&3) ;
colour = BASE_DAMAGE_COLOUR ;

break ;

// Small thrust
case SECTION_SMOKE :

counter = 4+(arm_random()&3) ;
colour = BASE_EXPLOSION_COLOUR + (arm_random()&3) ;

break ;

// Small smoke
case SECTION_SMOKE2 :

counter = 4+(arm_random()&3) ;
colour = BASE_DAMAGE_COLOUR ;

break ;

// Big thrust
case SECTION_SMOKE3 :

counter = 6+(arm_random()&3) ;
colour = BASE_EXPLOSION_COLOUR + (arm_random()&3) ;

break ;

// Big smoke
case SECTION_SMOKE4 :

counter = 6+(arm_random()&3) ;
colour = BASE_DAMAGE_COLOUR ;

break ;


}

smoke = armlink_addtolist( &smokes );


if (smoke == NULL)
{

temp_smoke=(smokes.info).start_address ;

while ((temp_smoke->header).status==1)
{

if ( (temp_smoke->counter) < new_smoke_val )
{
	new_smoke_val = temp_smoke->counter ;
	new_smoke = temp_smoke ;
}

temp_smoke=(temp_smoke->header).next_address ;
}

smoke = new_smoke ;

}



if (smoke != NULL)
	{

	smoke->x_pos = x_pos ;
	smoke->y_pos = y_pos ;
	smoke->z_pos = z_pos ;
	smoke->x_vel = x_vel ;
	smoke->y_vel = y_vel ;
	smoke->z_vel = z_vel ;
	smoke->colour = colour ;
	smoke->counter = counter ;
	smoke->type = type ;
	
	// Check the height of the smoke if not in space
	if (which_graphics_set != SPACE_GRAPHICS)
	{
		coll_check = find_ground_height(smoke->x_pos,smoke->y_pos) ;
		if (coll_check>smoke->z_pos )
		{
			smoke->z_pos = coll_check ;
		}
	}

	}

}

void fighter_thrusters( ship_stack* ship )
{

graphics_details *details = (graphics_details*) ships_adr ;

long loop ;
thruster_details_header*	thruster_header ;
thruster_details*			thruster_data ;
rotate_node node_data ;


thruster_header = (thruster_details_header*) (details+(ship->type))->thruster_data ;
thruster_data = (thruster_details*) ( thruster_header + 1 ) ;
		
for (loop = 0 ; loop <= thruster_header->counter ; loop += 1)
{
	// Thruster smoke is arm_randomom based on power
	if (ship->thrust_control > (arm_random()&1023) )
	{
		node_data.x_pos = (thruster_data->x_pos) ;
		node_data.y_pos = (thruster_data->y_pos) ;
		node_data.z_pos = (thruster_data->z_pos) ;
		node_data.x_rot = ship->x_rot ;
		node_data.y_rot = ship->y_rot ;
		node_data.z_rot = ship->z_rot ;
		rotate_node_from_c( &node_data );
		add_smoke( 	(ship->x_pos) + node_data.x_pos ,
					(ship->y_pos) + node_data.y_pos ,
					(ship->z_pos) + node_data.z_pos ,
					ship->x_vel , ship->y_vel , ship->z_vel ,
					SMALL_THRUSTER_SMOKE ,
					(ship->misc_counter)>>2 ) ;
	}
	thruster_data += 1 ;
}

// Inc smoke counter
//if (ship->misc_counter < ((ship->thrust_control)>>7) ) ship->misc_counter += 1 ;
if (ship->misc_counter < 31 ) ship->misc_counter += 1 ;

}



