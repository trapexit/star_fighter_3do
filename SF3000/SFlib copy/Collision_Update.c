/* Include Headers */

#include "Update_Frame.h"
#include "SF_Io.h"
#include "Smoke_Control.h"
#include "Rotate_Land.h"
#include "Draw_Land.h"
#include "Plot_Graphic.h"
#include "Laser_Control.h"
#include "Bit_Control.h"
#include "SF_ArmLink.h"
#include "Explosion.h"
#include "Weapons.h"
#include "Collision.h"
#include "Maths_Stuff.h"
#include "Misc_Struct.h"
#include "Ground_Control.h"
#include "Bonus_Control.h"
#include "SF_Control.h"
#include "Stdio.h"
#include "SF_War.h"
#include "SF_Bonus.h"
#include "SF_ArmUtils.h"
#include "Sound_control.h"
#include "SF_Bonus.h"

#include <debug.h>

void collision_update (void)
{
long loop , coll_check ;
ship_stack* ship ;
ship_stack* temp_ship ;
ship_stack* other_ship ;

laser_stack *laser ;
long type ;
long *end_of_collision_stack ;
long *start_of_collision_stack = temp_store ;
static long bonus_shader = 1;



// Update tims stuff before any ships are deleted
// This is the high level ship controlling
// 'Mayday someone has shot me' etc......
war_updatemissionships();

// Check for any dead ships etc. and blow them up / delete them from t' list
ship=(ships.info).start_address ;

while ((ship->header).status==1)
{

	// Clear the collision check flag
	ship->who_hit_me = -1 ;
	ship->what_hit_me = 0 ;

	// Get the next adr as this ship may be deleted
	temp_ship=(ship->header).next_address ;

	// Are you a dead ship or wot?
	if (ship->shields<=0) 
	{
		// Was it a wing pod that just snuffed it ?
		if ( ship->type == WING_POD )
		{
			// If so then update the owners pointer things
			other_ship = (ship_stack*) ship->who_owns_me ;
			other_ship->ref = ((other_ship->ref)&(~ship->ref)) ;
		}
		
		explode_ship( ship );
	}
		
	ship=temp_ship ;
}



// Check for any ship v ship / ship v laser  collisions

// These sort in lists based on X position
armlink_resortlist ( &lasers );
armlink_resortlist ( &ships );

// Find any laser v ship collisions (stored in start_of_collision_start ----)
// Also sets ship->what_hit_me for ship v ship collisions
end_of_collision_stack = armcol_collisioncheck( start_of_collision_stack , &lasers , &ships ) ;


// Update any laser v ship collisions
// 0 laser hitting
// 1 which ship

while (start_of_collision_stack < end_of_collision_stack )
{

	//printf("Laser collision found\n");

	laser = (laser_stack*) start_of_collision_stack[0] ;
	ship = (ship_stack*) start_of_collision_stack[1] ;
		
	// Bonus crystals can't be hit by laser fire
	if ( ((ship->type)>>4) != BONUS )
	{
		
		if ((ship->type>>4) == BIG_SHIP || (ship->type>>4) == CAR ) 
		{
			coll_check = big_ship_collision_check( ship , (ship_stack*) laser ) ;
		}
		else
		{
			coll_check = 1 ;
		}
		
		if ( coll_check != 0 )
		{
			laser->counter = 0 ;
			ship->shields -= laser->type ;
			ship->z_roller = 32 * ( (arm_random()&2047) -1024 ) ;
			ship->who_hit_me = (long) laser->who_owns_me ;
			
			// Weapons blow up quicker (expect the players)
			if ( (ship->type>>4) == WEAPON && ( (ship_stack*) laser->who_owns_me ) != players_ship )
			{
				ship->shields -= (( 1 + laser->type)<<1) ;
			}
			else
			{
				ship->shields -= (( 1 + laser->type)) ;
			}
			
			// The players ship can survive death sometimes
			if (	((ship->type)>>4) == PLAYERS_SHIP &&
					ship->shields <= 0 &&
					ship->damage <= DAMAGE_MEDIUM_COUNTER &&
					(arm_random()&3) >= 2 ) 
			{
				ship->shields = 10 ;
				ship->damage = DAMAGE_BAD ;
				ship->damage_counter = DAMAGE_BAD_COUNTER ;	
				bonus_downgrade_ship( (arm_random()&3) );				
			}
			
			if ( (ship->type>>4) == PLAYERS_SHIP  && (arm_random()&31) == 0 && ship->shields < 32 )
			{
				bonus_downgrade_ship( 0 );
			}

			
			add_smoke( 	laser->x_pos , laser->y_pos , laser->z_pos ,
						ship->x_vel , ship->y_vel , ship->z_vel ,
						LASER_HIT_SMOKE ,
						0 );
		}
	}

	start_of_collision_stack += 2 ;

}

}



void bonus_collision( ship_stack* bonus )
{
ship_stack* other_ship = (ship_stack*) bonus->what_hit_me ;

// Check that it's a players ship before allowing the pickup
if ( ((other_ship->type)>>4) == PLAYERS_SHIP )
{
	bonus->shields = 0 ;
	bonus_addpickup( ((bonus->type)&15)+1 ) ;
}

// Check that its collision pointer points to you before clearing it
if ( ( (ship_stack*) other_ship->what_hit_me) == bonus)
{
	other_ship->what_hit_me = 0 ;
}

// Clear the bonus collision flag
bonus->what_hit_me = 0 ;

}



void parachute_collision( ship_stack* parachute )
{
ship_stack* other_ship = (ship_stack*) parachute->what_hit_me ;

// Check that it's a players ship before allowing the pickup
if ( ((other_ship->type)>>4) == PLAYERS_SHIP )
{
	parachute->shields = PARACHUTE_NO_EXPLODE ;
	bonus_parachute( 1 ) ;
}

// Check that its collision pointer points to you before clearing it
if ( ( (ship_stack*) other_ship->what_hit_me) == parachute)
{
	other_ship->what_hit_me = 0 ;
}

// Clear the bonus collision flag
parachute->what_hit_me = 0 ;

}



void missile_collision( ship_stack* missile )
{
ship_stack* other_ship = (ship_stack*) missile->what_hit_me ;


if ( ((other_ship->type)>>4) == BONUS || ((other_ship->type)>>4) == PARACHUTE ) return ;

// Chance of missing the player
if ( (other_ship->type>>4) == PLAYERS_SHIP && (arm_random()&1) == 0)
{
	missile->who_owns_me = (long) other_ship ;
	other_ship->what_hit_me = 0 ;
	missile->what_hit_me = 0 ;
	missile->shields = 4+(arm_random()&3) ;
	return;
}

// Stop yer pods from hitting the mothership and your friends
if ( missile->type == WING_POD && other_ship->special_data != NULL )
{
	if (other_ship->special_data->side != SDB_SIDE_ENEMY )
	{
		other_ship->what_hit_me = 0 ;
		missile->what_hit_me = 0 ;
		return ;
	}
}

if ( (other_ship->type>>4) == BIG_SHIP || (other_ship->type>>4) == CAR ) 
{
	if ( big_ship_collision_check( other_ship , missile ) == 0 )
	{
		other_ship->what_hit_me = 0 ;
		missile->what_hit_me = 0 ;
		return ;
	}
}

	missile->shields = 0 ;
	
	if ( ((other_ship->type)>>4) == PLAYERS_SHIP )
	{
		other_ship->shields -= 30 ;	
	}
	else
	{
		other_ship->shields -= 50 ;
	}

	// Roll the ship about
	other_ship->z_roller = 64 * ( (arm_random()&2047) -1024 ) ;

	// Random chance of mild damage after a missile hit
	if ( (arm_random()&7) == 0 )
	{
		other_ship->damage = DAMAGE_MEDIUM ;
		other_ship->damage_counter = DAMAGE_MEDIUM_COUNTER ;
		
		if ( (other_ship->type>>4) == PLAYERS_SHIP )
		{
			bonus_downgrade_ship( (arm_random()&1) );
		}
	}

	// The players ship can survive death sometimes
	if (	((other_ship->type)>>4) == PLAYERS_SHIP &&
			other_ship->shields <= 0 &&
			other_ship->damage <= DAMAGE_MEDIUM_COUNTER &&
			(arm_random()&3) >= 2 ) 
	{
		other_ship->shields = 10 ;
		other_ship->damage = DAMAGE_VERY_BAD ;
		other_ship->damage_counter = DAMAGE_VERY_BAD_COUNTER ;	
		bonus_downgrade_ship( (arm_random()&3) );
	}
			

//}

//printf("Player = %d\n",players_ship);
//printf("Missile owner = %d\n",missile->who_owns_me);
//printf("Ship owner = %d\n",other_ship->who_owns_me);


// Check that its collision pointer points to you before clearing it
if ( ( (ship_stack*) other_ship->what_hit_me) == missile)
{
	other_ship->what_hit_me = 0 ;
}

// Clear the bonus collision flag 
missile->what_hit_me = 0 ;

// Who shot the missile at me
other_ship->who_hit_me = missile->who_owns_me ;

}



void small_ship_collision( ship_stack* ship )
{

long coll_check = 0 , temp_long ;
ship_stack *other_ship = (ship_stack*) ship->what_hit_me ;
long x_pos , y_pos , z_pos , size ;


if ( other_ship->type >= SECTION ) return ;


// Is it a bonus crystal situation ?
if ( ((other_ship->type)>>4) == BONUS)
{
	other_ship->what_hit_me = (long) ship ;
	bonus_collision( other_ship );
	return ;
}

// Is it a parachute situation ?
if ( ((other_ship->type)>>4) == PARACHUTE)
{
	other_ship->what_hit_me = (long) ship ;
	parachute_collision( other_ship );
	return ;
}

// Is it a weapon hit
if ( ((other_ship->type)>>4) == WEAPON)
{
	other_ship->what_hit_me = (long) ship ;
	missile_collision( other_ship );
	return ;
}


if ((other_ship->type>>4) == BIG_SHIP || (other_ship->type>>4) == CAR ) 
{
	coll_check = big_ship_collision_check( other_ship , ship );
}
else
{
	x_pos = ship->x_pos - other_ship->x_pos ;
	if (x_pos < 0) x_pos = -x_pos ;
	y_pos = ship->y_pos - other_ship->y_pos ;
	if (y_pos < 0) y_pos = -y_pos ;
	z_pos = ship->z_pos - other_ship->z_pos ;
	if (z_pos < 0) z_pos = -z_pos ;
	size = (ship->collision_size + other_ship->collision_size)>>1 ;
	
	if ( x_pos < size && y_pos < size && z_pos < size ) coll_check = 1 ;
}

// If the players ship is in the docking bay set the flag
if (coll_check == 2 && ((ship->type)>>4) == PLAYERS_SHIP && other_ship->special_data->side != SDB_SIDE_ENEMY )
{
	docked.docked_this_frame = 1 ;
	docked.ship = other_ship ;
}

if (coll_check != 0)
{

	ship->speed = -ship->speed ;

	if (ship != players_ship || other_ship->special_data->side == SDB_SIDE_ENEMY ||
		( docked.status == DOCKING_OUT && docked.docked_this_frame != 1 ) )
	{
		ship->who_hit_me = (long) ship->what_hit_me ;

		make_sound( ship->x_pos , ship->y_pos , ship->z_pos , THUD_SOUND ) ;

		if (ship->speed < 0)
		{
			temp_long = -(ship->speed>>4) ;
		}
		else
		{
			temp_long = (ship->speed>>4) ;
		}
		
		ship->shields -= temp_long ;
		other_ship->shields -= temp_long ;
		
		if ( ( (ship->type>>4) == PLAYERS_SHIP || (other_ship->type>>4) == PLAYERS_SHIP) && (arm_random()&3) == 0 )
		{
			bonus_downgrade_ship( (temp_long-16)>>4 );
		}
	}
}


ship->what_hit_me = 0 ;

}


