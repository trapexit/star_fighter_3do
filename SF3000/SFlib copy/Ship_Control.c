#include "SF_Mission.h"
#include "Smoke_Control.h"
#include "Graphic_Struct.h"
#include "Plot_Graphic.h"
#include "Collision.h"
#include "Laser_Control.h"
#include "Explosion.h"
#include "SF_ArmLink.h"
#include "Stdio.h"
#include "Weapons.h"
#include "Sound_Control.h"
#include "Maths_Stuff.h"
#include "Misc_Struct.h"
#include "SF_Control.h"
#include "Ship_Control.h"
#include "Ship_Command.h"
#include "Collision_Update.h"
#include "SF_ArmUtils.h"
#include "SF_War.h"
#include "Graphics_Set.h"
#include "String.h"
#include "SF_Bonus.h"
#include "SF_Sound.h"
#include "SF_Message.h"
#include "SF_Palette.h"

void update_docking( ship_stack* ship )
{

ship_stack* mother = docked.ship ;
long counter ;
long x_rot , y_rot , z_rot ;
long x_pos , y_pos , z_pos ;
rotate_node node_data ;

// Offset to put players centre pos above the floor
node_data.x_pos = 0 ;
node_data.y_pos = (1<<21) ;
node_data.z_pos = (1<<21) ;
node_data.x_rot = mother->x_rot ;
node_data.y_rot = mother->y_rot ;
node_data.z_rot = mother->z_rot ;
rotate_node_from_c ( &node_data ) ;



// Have you just entered the docking bay
if (docked.status == DOCKING_OUT)
{
	docked.counter = 16 ;
	docked.status = DOCKING_ENTER ;
	return;
}

// Is the mother ship still here and alive
if ( ((mother->type)>>4) != BIG_SHIP || mother->shields <= 0)
{
	docked.status = DOCKING_OUT ;
	docked.counter = 0 ;
}


// Whats the docking status
switch ( docked.status )
{

case DOCKING_ENTER :

// Tell tim the adr of the mother ship
status.docked = mother ;

x_rot = ship->x_rot - mother->x_rot ;
if (x_rot > 512*1024) x_rot -= 1024*1024 ;
if (x_rot < -512*1024) x_rot += 1024*1024 ;

y_rot = ship->y_rot - mother->y_rot ;
if (y_rot > 512*1024) y_rot -= 1024*1024 ;
if (y_rot < -512*1024) y_rot += 1024*1024 ;

z_rot = ship->z_rot - mother->z_rot ;
if (z_rot > 512*1024) z_rot -= 1024*1024 ;
if (z_rot < -512*1024) z_rot += 1024*1024 ;

x_pos = ship->x_pos - (mother->x_pos + node_data.x_pos) ;
y_pos = ship->y_pos - (mother->y_pos + node_data.y_pos) ;
z_pos = ship->z_pos - (mother->z_pos + node_data.z_pos) ;

counter = 16 - docked.counter ;

ship->x_pos -= ((counter*x_pos)>>4) ;
ship->y_pos -= ((counter*y_pos)>>4) ;
ship->z_pos -= ((counter*z_pos)>>4) ;

ship->x_pos += ship->x_vel ;
ship->y_pos += ship->y_vel ;
ship->z_pos += ship->z_vel ;

ship->x_vel -= (ship->x_vel>>4) ;
ship->y_vel -= (ship->y_vel>>4) ;
ship->z_vel -= (ship->z_vel>>4) ;

ship->x_rot = (ship->x_rot - ((counter*x_rot)>>4) )&ROT_LIMIT;
ship->y_rot = (ship->y_rot - ((counter*y_rot)>>4) )&ROT_LIMIT;
ship->z_rot = (ship->z_rot - ((counter*z_rot)>>4) )&ROT_LIMIT;

if (docked.counter <= 0)
{
	docked.counter = 0 ;
	docked.status = DOCKING_FIXED ;
}

break ;


case DOCKING_EXIT :

if (docked.counter >= 16)
{
	docked.counter = 0 ;
	docked.status = DOCKING_OUT ;
	status.docked = 0 ;
}

docked.counter += 2 ;

ship->x_dir = mother->x_rot ;
ship->y_dir = mother->y_rot ;

ship->speed -= 8 ;

ship->x_vel =	-(
				(( sine_table [ (ship->x_dir)>>10 ] * 
				cosine_table [ (ship->y_dir)>>10 ] )>>12)
				* (ship->speed)
				) ;

ship->y_vel =	-(
				(( cosine_table [ (ship->x_dir)>>10 ] * 
				cosine_table [ (ship->y_dir)>>10 ] )>>12)
				* (ship->speed)
				) ;

ship->z_vel =	( sine_table [ (ship->y_dir)>>10 ] * (ship->speed) ) ;

ship->x_pos += ship->x_vel ;
ship->y_pos += ship->y_vel ;
ship->z_pos += ship->z_vel ;

break ;


case DOCKING_FIXED :

ship->x_pos = (mother->x_pos + node_data.x_pos) ;
ship->y_pos = (mother->y_pos + node_data.y_pos) ;
ship->z_pos = (mother->z_pos + node_data.z_pos) ;

ship->x_vel = mother->x_vel ;
ship->y_vel = mother->y_vel ;
ship->z_vel = mother->z_vel ;

ship->x_rot = mother->x_rot ;
ship->y_rot = mother->y_rot ;
ship->z_rot = mother->z_rot ;

ship->speed = mother->speed ;

if (ship->fire_request != WEAPON_NOTHING)
{
	docked.status = DOCKING_EXIT ;
	docked.counter = 0 ;
}

break ;


}

if (docked.counter > 0) docked.counter -= 1;

}

void ship_control( ship_stack * ship )

{
long coll_check ;
long temp_long ;
long loop , loop2 ;

long cosine_y , sine_y ;
long cosine_z , sine_z ;
target_struct target ;

ship_stack* temp_ship ;

long x_control , y_control , z_control ;

rotate_node node_data ;

graphics_details *details = (graphics_details*) ships_adr ;

smoke_details_header *smoke_header ;
smoke_details *smoke_data ;

thruster_details_header *thruster_header ;
thruster_details *thruster_data ;

laser_details_header *laser_header ;
laser_details *laser_data ;

// Check ship collisions
if ( ship->what_hit_me != 0 ) small_ship_collision( ship );

// Update the docking details for the players ship
if ( ((ship->type)>>4) == PLAYERS_SHIP &&
	( docked.docked_this_frame == 1 || docked.status != DOCKING_OUT ) )
{
	update_docking( ship );
	return ;
}

// x_control and y_control can be treated as rate of turn 
// these are set by the ship commands or the joypad key presses for the player

// Is this ship damaged
if (ship->damage != DAMAGE_NOTHING) small_ship_damaged( ship ) ;

// See what commands - if any this ship has
// if ( ship->command != COMMAND_NOTHING ) small_ship_command_update( ship );
if ( ((ship->type)>>4) != PLAYERS_SHIP ) small_ship_command_update( ship );

// Speed based control rate - the faster you go the slower the rate of control
temp_long = 1024 - ((ship->speed)>>2) ;
if (temp_long < 320) temp_long = 320 ;

// Use the performance data control rate to bump up the controls
x_control = ((ship->x_control + ((ship->x_control * ship->performance->control)>>3)  )*temp_long)>>11 ;
y_control = ((ship->y_control + ((ship->y_control * ship->performance->control)>>3)  )*temp_long)>>11 ;
z_control = ((ship->z_control + ((ship->z_control * ship->performance->control)>>3)  )*temp_long)>>11 ;

// Friction on speed based on x and y steering rates
// x_control - y_control

if (ship->x_control < 0)
{
	temp_long = -ship->x_control ;
}
else
{
	temp_long = ship->x_control ;
}

if (ship->y_control < 0)
{
	temp_long -= ship->y_control ;
}
else
{
	temp_long += ship->y_control ;
}

if (ship->z_control < 0)
{
	temp_long -= ship->z_control ;
}
else
{
	temp_long += ship->z_control ;
}

// friction on speed
ship->speed -= ( ((ship->speed)>>7) + ( ((ship->speed)*temp_long)>>16 ) ) ;
//ship->speed -=  ((ship->speed)>>6) ;

if (which_graphics_set != SPACE_GRAPHICS)
{
	// Gravity on speed
	ship->speed -= (( sine_table [ (ship->y_dir)>>10 ])>>8) ;
}

// check min speed limit and increase if ness
if ( (ship->speed) < 256 ) 
{
	ship->speed -= ((ship->speed)>>3) ;
	ship->speed += 64 ;
	if (ship->speed > 256) ship->speed = 256 ;
}


// change the ships rotation based on x , y control position

node_data.x_pos = z_control<<10 ;
node_data.y_pos = 1<<22 ;
node_data.z_pos = y_control<<10 ;
node_data.x_rot = ship->x_rot ;
node_data.y_rot = ship->y_rot ;
node_data.z_rot = ship->z_rot ;
rotate_node_from_c( &node_data ) ;

target.x_pos = 0 ;
target.y_pos = 0 ;
target.z_pos = 0 ;
target.x_aim = node_data.x_pos ;
target.y_aim = node_data.y_pos ;
target.z_aim = node_data.z_pos ;

target_finder( &target );

// There is a max change of x rot of 180 degrees when yer flip at the bottom / top
// the z roll must change to keep up
// How about z roll rate of change = x change * sine y rot ???

// Find rate of change of x rot
temp_long = (ship->x_rot - target.x_rot) ;
if (temp_long > 512*1024) temp_long -= 1024*1024 ;
if (temp_long < -512*1024) temp_long += 1024*1024 ;

// Watch out for overflow
temp_long = temp_long >> 2 ;

// Change z roll based on rate of change of x and sine y
ship->z_rot =  ( (ship->z_rot + ((sine_table [ ship->y_rot>>10 ] * temp_long )>>10) )&ROT_LIMIT);

ship->x_rot = target.x_rot ;
ship->y_rot = target.y_rot ;


// change x rot based sine z roll and cosine y pitch to help auto turn corners
ship->x_rot =	(ship->x_rot 

				- ( ( sine_table [ ship->z_rot>>10 ]
				* cosine_table [ ship->y_rot>>10 ] )>>12 )
				
				)
				&ROT_LIMIT ;


// change z rot based on x control and z_roller - (set by laser / missile hits)
ship->z_rot =	(ship->z_roller + ship->z_rot + ((x_control)<<6) )&ROT_LIMIT ;
// ship->z_rot =	(ship->z_rot + ((x_control)<<6) )&ROT_LIMIT ;

// Decrease damage roller
ship->z_roller -= ((ship->z_roller)>>2) ;


if ( (ship->damage & DAMAGE_HILL_BOUNCE_BIT) != DAMAGE_HILL_BOUNCE_BIT)
{
	// calculate x y directions based on air friction v rate of change
	temp_long = ship->x_dir - ship->x_rot ;
	if (temp_long > 512*1024) temp_long -= 1024*1024 ;
	if (temp_long < -512*1024) temp_long += 1024*1024 ;

	ship->x_dir = (ship->x_dir-(temp_long>>1))&ROT_LIMIT ;

	temp_long = ship->y_dir - ship->y_rot ;
	if (temp_long > 512*1024) temp_long -= 1024*1024 ;
	if (temp_long < -512*1024) temp_long += 1024*1024 ;

	ship->y_dir = (ship->y_dir-(temp_long>>1))&ROT_LIMIT ;
}
else
{
	// the ship has just bounced off a hill
	// calculate x y directions based slightly on ships rot
	temp_long = ship->x_dir - ship->x_rot ;
	if (temp_long > 512*1024) temp_long -= 1024*1024 ;
	if (temp_long < -512*1024) temp_long += 1024*1024 ;

	ship->x_dir = (ship->x_dir-(temp_long>>6))&ROT_LIMIT ;

	// Change the ships angle very slightly towards the direction angle
	ship->x_rot = (ship->x_rot+(temp_long>>4))&ROT_LIMIT ;

	temp_long = ship->y_dir - ship->y_rot ;
	if (temp_long > 512*1024) temp_long -= 1024*1024 ;
	if (temp_long < -512*1024) temp_long += 1024*1024 ;

	ship->y_dir = (ship->y_dir-(temp_long>>6))&ROT_LIMIT ;

	// Change the ships angle very slightly towards the direction angle
	ship->y_rot = (ship->y_rot+(temp_long>>4))&ROT_LIMIT ;
}

// calculate the x,y,z velocities based on x , y direction and speed
// 4096 speed = 1 sprite unit per frame
ship->x_vel =	-(
				(( sine_table [ (ship->x_dir)>>10 ] * 
				cosine_table [ (ship->y_dir)>>10 ] )>>12)
				* (ship->speed)
				) ;

ship->y_vel =	-(
				(( cosine_table [ (ship->x_dir)>>10 ] * 
				cosine_table [ (ship->y_dir)>>10 ] )>>12)
				* (ship->speed)
				) ;

ship->z_vel =	( sine_table [ (ship->y_dir)>>10 ] * (ship->speed) ) ;

// add velocity onto x ,y ,z positions
ship->x_pos += ship->x_vel ;
ship->y_pos += ship->y_vel ;
ship->z_pos += ship->z_vel ;

if (which_graphics_set != SPACE_GRAPHICS) check_shippy_collision( ship ) ;


// check for collisions with ground objects n' ground
coll_check = check_collision ( ship->x_pos , ship->y_pos , ship->z_pos ) ;
	
if (coll_check != 0 && coll_check != ship->who_owns_me )
{

	// Damage based on the ships speed	
	temp_long = (ship->speed)>>3 ;
	if (temp_long < 0) temp_long = -temp_long ;

	// Is it a collision with a static object
	// If so damage the object based on the speed of the collision
	if (coll_check > 0)
	{		
		static_explode ( coll_check , temp_long ) ;
	}
	else
	{
		dent_ground ( ship->x_pos , ship->y_pos , temp_long ) ;
	}
	
	//Damage the ship based on the impact speed
	ship->shields -= temp_long ;

	if ( (ship->type>>4) == PLAYERS_SHIP )
	{
		if ( temp_long>16 && (arm_random()&3) == 0 )
		{
			bonus_downgrade_ship( ((temp_long-16)>>4) ) ;
		}
	}

	// Reverse the ships velocity and speed
	ship->speed = -ship->speed ;
	ship->x_vel = -ship->x_vel ;
	ship->y_vel = -ship->y_vel ;
	ship->z_vel = -ship->z_vel ;
	
	// Take the ship back 2 frame steps
	ship->x_pos += ((ship->x_vel)<<1) ;
	ship->y_pos += ((ship->y_vel)<<1) ;
	ship->z_pos += ((ship->z_vel)<<1) ;

	make_sound( ship->x_pos , ship->y_pos , ship->z_pos , THUD_SOUND ) ;
}



// Max height limit
if (ship->z_pos > (100<<24) && which_graphics_set != SPACE_GRAPHICS )
{
	if ( ship->z_pos > (124<<24) ) ship->z_pos = (124<<24) ;
	
	ship->y_rot = ((ship->y_rot)-8192)&ROT_LIMIT ;
}


// is the thruster on
if ( ship->thrust_control>0 )
{
	//ship->speed += (32+( ((ship->performance)->engine)<<2 ) ) ;

	ship->speed += (( ( 20 + ((ship->performance)->engine << 2) ) * (ship->thrust_control) )>>10) ;

	// check if it can be seen n do thrusters if true
	if (ship->can_see) fighter_thrusters( ship ) ;
}
else
{
	// No thruster then dec colour counter
	if (ship->misc_counter > 0) ship->misc_counter -= 1;
}


// Dec the counter for can i fire - only when 0 can yer shoot
// This counter is set when you fire - therefore you can only shoot laser every other frame
// and missiles every 4 frames etc.......
if (ship->counter>0) ship->counter -= 1 ;

if ( ship->fire_request != WEAPON_NOTHING && ship->counter == 0 )
{
	// Check counters if its the players ship
	if (ship->fire_request != WEAPON_LASER && ((ship->type)>>4) == PLAYERS_SHIP )
	{
		
		// Check that the player has some of these weapons left
		if (ship->performance->weapons[ship->fire_request] <= 0)
		{
			ship->fire_request = WEAPON_NOTHING;
		}
		else
		{
			// If the weapon requires a lock (ie. missiles ) check its not null before allowing it
			if ( (ship->fire_request == WEAPON_ATA || ship->fire_request == WEAPON_ATG ) 
				&& ship->target == NULL )
			{
				ship->fire_request = WEAPON_NOTHING ;
			}
			else
			{
				if (ship->fire_request == WEAPON_MINE || ship->fire_request == WEAPON_MULTI_MISSILE)
				{
					if (ship_setmultipletargets( ship , __WAR_SR_LOCAL , (ship->special_data)->side )
						!= 0)
					{
						ship->performance->weapons[ship->fire_request] -= 1;
						message_addweapon();
					}
					else
					{
						ship->fire_request = WEAPON_NOTHING ;
					}

				}
				else
				{
					ship->performance->weapons[ship->fire_request] -= 1;
					message_addweapon();
				}
			}
		}
	}
	
	switch (ship->fire_request)
	{
	
	case WEAPON_LASER :
	small_fighter_lasers( ship );
	break ;
	
	case WEAPON_ATG :

	if ( ((ship->type)>>4) == PLAYERS_SHIP )
	make_sound ( ship->x_pos , ship->y_pos , ship->z_pos , MISSILE_SOUND ) ;
	
	add_air_to_ground( ship );
	break ;
	
	case WEAPON_ATA :

	if ( ((ship->type)>>4) == PLAYERS_SHIP )
	make_sound ( ship->x_pos , ship->y_pos , ship->z_pos , MISSILE_SOUND ) ;

	if ( (ship_stack*) ship->target == players_ship )
	{
		message_add( MESSAGE_TOP, MTXT__WARNING_INCOMING_ATAM , RED_15 , MESSAGE_FLASH , 24 , MESSAGE_NOCURSOR );
		sound_playsample( SOUND_INCOMING , 60 , 127 , 0 ) ;
	}

	// ata missiles can be treated as atg missiles when in space
	if ( (long) ship->target < 16384 )
	{
		add_air_to_ground( ship ) ;
	}
	else
	{
		add_air_to_air( ship , (ship_stack*) ship->target ) ;
	}
	
	break ;
	
	case WEAPON_MINE :
	add_mine( ship ) ;
	break ;
	
	case WEAPON_MEGA_BOMB :
	add_mega_bomb( ship ) ;
	break ;
	
	// Only available for the players ship
	case WEAPON_BEAM_LASER :
	if (beam_laser.on_last_frame == 0) start_beam_lasers() ;
	ship->counter =	0 ;
	update_beam_lasers() ;
	break ;
	
	case WEAPON_MULTI_MISSILE :
	
	// Fires three missiles at nearest 3 targets
	
	add_air_to_air( ship , (ship_stack*) ship->target ) ;

	// Is there a 2nd target - if not 1st target gets a double wammer
	if (ship->target2 != NULL)
	{
		add_air_to_air( ship , (ship_stack*) ship->target2 ) ;
	}
	else
	{
		add_air_to_air( ship , (ship_stack*) ship->target ) ;
	}
	
	// Is there a 3rd target - if not 1st target gets a triple wammer	
	if (ship->target3 != NULL)
	{
		add_air_to_air( ship , (ship_stack*) ship->target3 ) ;
	}
	else
	{
		add_air_to_air( ship , (ship_stack*) ship->target ) ;
	}
			
	break ;
	
	case WEAPON_ECM :
	temp_ship=(ships.info).start_address ;

	while ((temp_ship->header).status==1)
	{
		if ( (temp_ship->type>>4) == WEAPON && temp_ship->type != MEGA_BOMB && temp_ship->type != WING_POD )
		{
			temp_ship->shields = 0 ;
		}
	
		temp_ship=(temp_ship->header).next_address ;
	}

	for (loop = 0 ; loop < 64 ; loop += 1)
	{
		add_smoke( 	ship->x_pos , ship->y_pos , ship->z_pos ,
					ship->x_vel + ( (arm_random()&((1<<22)-1)) - (1<<21) ) ,
					ship->y_vel + ( (arm_random()&((1<<22)-1)) - (1<<21) ) ,
					ship->z_vel + ( (arm_random()&((1<<22)-1)) - (1<<21) ) ,
					MISSILE_SMOKE ,
					0 );
	}
	
	ship->counter = 8 ;
	
	break ;
	
	}
}

// Stop the beam lasers ?
if ( ((ship->type)>>4) == PLAYERS_SHIP &&
	ship->fire_request != WEAPON_BEAM_LASER && beam_laser.on_last_frame == 1
	) stop_beam_lasers() ;


}

void check_shippy_collision( ship_stack* ship )
{

long height_temp , rot_temp ;
long temp_z_pos ;
long new_rot ;
long damage ;
long temp_long ;

// Is there a collision with t' ground
temp_z_pos = find_ground_height( ship->x_pos , ship->y_pos ) ;

// If not return
if ( temp_z_pos < ship->z_pos ) return ;


make_sound( ship->x_pos , ship->y_pos , ship->z_pos , THUD_SOUND ) ;


// Set the ships new height
ship->z_pos = temp_z_pos+(1<<18) ;

// Find the ground height in front of the object
height_temp = find_ground_height(	((ship->x_pos)-(sine_table [ (ship->x_rot)>>10 ]<<11)) , 
									((ship->y_pos)-(cosine_table [ (ship->x_rot)>>10 ]<<11)) );



// Find the relative height diff between height at front and middle
height_temp = (height_temp-temp_z_pos)>>10 ;

// Find the new y rotation based on the height and find the relative rot
new_rot = (( (find_rotation( -height_temp , (1<<13) ))<<1 )-ship->y_dir)&ROT_LIMIT ;

// Find the rate of change between new n' old angle
rot_temp = (new_rot - ship->y_dir)&ROT_LIMIT ;

// Set the rotation direction
ship->y_dir = new_rot ;

// Set the damage based on the cosine of the rate of change of angle + speed
damage = ( (-( (cosine_table [ rot_temp>>10 ] - 4096)) * ship->speed)>>14 ) ;
if (damage < 0) damage = -damage ;

//printf("shield damage %d\n",damage);
ship->shields -= damage ;


if ( (ship->type>>4) == PLAYERS_SHIP )
{
	if ( damage>16 && (arm_random()&3) == 0 )
	{
		bonus_downgrade_ship( ((damage-16)>>4) ) ;
	}
}


dent_ground ( ship->x_pos , ship->y_pos , (1+(damage>>6)) ) ;

damage = 1 + (damage>>7) ;

// printf("shield damage %d\n" , damage );

switch( damage )
{
case	DAMAGE_SLIGHT :
	ship->damage = DAMAGE_HILL_BOUNCE_BIT + DAMAGE_SLIGHT ;
	ship->damage_counter = DAMAGE_SLIGHT_COUNTER ;
break ;

case	DAMAGE_MEDIUM :
	ship->damage = DAMAGE_HILL_BOUNCE_BIT + DAMAGE_MEDIUM ;
	ship->damage_counter = DAMAGE_MEDIUM_COUNTER ;
break ;

case	DAMAGE_BAD :
	ship->damage = DAMAGE_HILL_BOUNCE_BIT + DAMAGE_BAD ;
	ship->damage_counter = DAMAGE_BAD_COUNTER ;
break ;

default :
	ship->damage = DAMAGE_HILL_BOUNCE_BIT + DAMAGE_VERY_BAD ;
	ship->damage_counter = DAMAGE_VERY_BAD_COUNTER ;
}


// the ship has just bounced off a hill
// calculate x y rotations abit based on ships direction
temp_long = ship->x_dir - ship->x_rot ;
if (temp_long > 512*1024) temp_long -= 1024*1024 ;
if (temp_long < -512*1024) temp_long += 1024*1024 ;

// Change the ships angle very slightly towards the direction angle
ship->x_rot = (ship->x_rot+(temp_long>>2))&ROT_LIMIT ;

temp_long = ship->y_dir - ship->y_rot ;
if (temp_long > 512*1024) temp_long -= 1024*1024 ;
if (temp_long < -512*1024) temp_long += 1024*1024 ;

// Change the ships angle very slightly towards the direction angle
ship->y_rot = (ship->y_rot+(temp_long>>2))&ROT_LIMIT ;

// Kill the speed abit
ship->speed = (ship->speed - (ship->speed>>3) ) ;

// calculate the x,y,z velocities based on x , y direction and speed
ship->x_vel =	-(
				(( sine_table [ (ship->x_dir)>>10 ] * 
				cosine_table [ (ship->y_dir)>>10 ] )>>12)
				* (ship->speed)
				) ;

ship->y_vel =	-(
				(( cosine_table [ (ship->x_dir)>>10 ] * 
				cosine_table [ (ship->y_dir)>>10 ] )>>12)
				* (ship->speed)
				) ;

ship->z_vel =	( sine_table [ (ship->y_dir)>>10 ] * (ship->speed) ) ;

}


//################################################
//#                                              #
//#     Satellite control and decision maker     #
//#                                              #
//################################################

void satellite_control( ship_stack * satellite )

{
long x_pos , y_pos , z_pos ;
long dist ;
ship_stack* target ;

// Fire at target if in range and arm_randomom fire = true
if ( satellite->fire_request != WEAPON_NOTHING )
{

	target = (ship_stack*) ( (ship_sdb*) satellite->special_data )->command_address ;

	// Find the relative dist between sate and target
	x_pos = satellite->x_pos - target->x_pos ;
	if ( x_pos < 0 ) x_pos = -x_pos ;

	y_pos = satellite->y_pos - target->y_pos ;
	if ( y_pos < 0 ) x_pos = -y_pos ;

	z_pos = satellite->z_pos - target->z_pos ;
	if ( z_pos < 0 ) z_pos = -z_pos ;

	// Find the greatest distance
	if ( x_pos > y_pos ) dist = x_pos ; else dist = y_pos ;
	if ( z_pos > dist ) dist = z_pos ;

	if ( dist < (32*(1<<24)) )
	{
		aim_laser( 	satellite->x_pos ,
					satellite->y_pos ,
					satellite->z_pos ,
					target->x_pos ,
					target->y_pos ,
					target->z_pos , 
					satellite->x_vel ,
					satellite->y_vel ,
					satellite->z_vel ,
					target->x_vel ,
					target->y_vel ,
					target->z_vel ,
					(satellite->performance)->weapons[ WEAPON_LASER ] ,
					(long) satellite ) ;
	}
}

// Spin on the x
satellite->x_rot = ((satellite->x_rot)+32*1024)&ROT_LIMIT ;

}



//################################################
//#                                              #
//#     Parachute control and decision maker     #
//#                                              #
//################################################

void parachute_control( ship_stack * parachute )

{

long coll_check ;

// Spin on the x
parachute->x_rot = ((parachute->x_rot)-16*1024)&ROT_LIMIT ;

// Fall slowly
parachute->z_pos -= (1<<21) ;
parachute->z_vel = -(1<<21) ;

// Check the collision flag 
if (parachute->what_hit_me != 0) parachute_collision( parachute ) ;

// check for collisions with ground objects n' ground
coll_check = check_collision ( parachute->x_pos , parachute->y_pos , parachute->z_pos ) ;

// if its hit anything then die	
if (coll_check != 0) parachute->shields = 0 ;

// If z_pos < 0 then die
if (parachute->z_pos < 0) parachute->shields = 0 ;


}



//#########################################################
//#     		                                          #
//#		Car / Truck / Tank control and decision maker     #
//#         		                                      #
//#########################################################

void car_control( ship_stack * car )

{

long height_temp , rot_temp ;
target_struct target ;
long x_aim , y_aim ;
ship_stack* ship ;
long x_pos , y_pos , z_pos , dist ;

if ( car->type == (CAR<<4) )
{

target.x_pos = car->x_pos ;
target.y_pos = car->y_pos ;
target.z_pos = car->z_pos ;
target.x_aim = car->goto_x ;
target.y_aim = car->goto_y ;
target.z_aim = car->z_pos ;// car->goto_z ;

target_finder( &target );

// Find the relative rotation between car and aim
x_aim = (target.x_rot) - (car->x_rot) ;
if (x_aim > 512*1024) x_aim -= 1024*1024 ;
if (x_aim < -512*1024) x_aim += 1024*1024 ;


car->x_rot = ((car->x_rot)+ (x_aim>>3) )&ROT_LIMIT ;

// Brake when near target
if (target.distance < (4<<24) )
{
	car->speed = target.distance>>16 ; // max speed 512
	if (car->speed < 512 ) car->speed = 512 ;
}
else
{
	car->speed += 32 ;
	if (car->speed > 1024) car->speed = 1024 ;
}

// No speed if not on line
if (x_aim<-(16<<10) || x_aim>(16<<10) )
{
	car->speed = 512 ;
}


// calculate the x,y,z velocities based on x , y direction and speed
car->x_vel =	-(
				(( sine_table [ (car->x_rot)>>10 ] * 
				cosine_table [ (car->y_rot)>>10 ] )>>12)
				* (car->speed)
				) ;

car->y_vel =	-(
				(( cosine_table [ (car->x_rot)>>10 ] * 
				cosine_table [ (car->y_rot)>>10 ] )>>12)
				* (car->speed)
				) ;

// add velocity onto x ,y positions
car->x_pos += car->x_vel ;
car->y_pos += car->y_vel ;

car->z_pos = find_ground_height(car->x_pos,car->y_pos) ;

// Find the ground height in front of the car
height_temp = find_ground_height( 	((car->x_pos)-(sine_table [ (car->x_rot)>>10 ]<<11)) , 
									((car->y_pos)-(cosine_table [ (car->x_rot)>>10 ]<<11)) );

// Find the relative height diff between height at front and middle
height_temp = (height_temp-car->z_pos)>>10 ;

// Find the y rotation based on the height and find the relative rot between real and ideal
rot_temp = (find_rotation( -height_temp , (1<<13) ))-car->y_rot ;
if(rot_temp>512*1024) rot_temp-=1024*1024 ;
if(rot_temp<-512*1024) rot_temp+=1024*1024 ;
// Change the y rotation by scale diff
car->y_rot = (car->y_rot+ (rot_temp>>2) )&ROT_LIMIT;


// Find the ground height at side of the car
height_temp = find_ground_height( 	((car->x_pos)-(cosine_table [ (car->x_rot)>>10 ]<<11)) , 
									((car->y_pos)+(sine_table [ (car->x_rot)>>10 ]<<11)) );

// Find the relative height diff between height at front and middle
height_temp = (height_temp-car->z_pos)>>10 ;

// Find the y rotation based on the height and find the relative rot between real and ideal
rot_temp = (find_rotation( -height_temp , (1<<13) ))-car->z_rot ;
if(rot_temp>512*1024) rot_temp-=1024*1024 ;
if(rot_temp<-512*1024) rot_temp+=1024*1024 ;
// Change the z rotation by scale diff
car->z_rot = (car->z_rot+ (rot_temp>>2) )&ROT_LIMIT;

}

else

{

target.x_pos = car->x_pos ;
target.y_pos = car->y_pos ;
target.z_pos = car->z_pos ;
target.x_aim = car->goto_x ;
target.y_aim = car->goto_y ;
target.z_aim = car->z_pos ;// car->goto_z ;

target_finder( &target );

// Find the relative rotation between car and aim
x_aim = (target.x_rot) - (car->x_rot) ;
if (x_aim > 512*1024) x_aim -= 1024*1024 ;
if (x_aim < -512*1024) x_aim += 1024*1024 ;


car->x_rot = ((car->x_rot)+ (x_aim>>3) )&ROT_LIMIT ;

	car->speed = 512 ;


// calculate the x,y,z velocities based on x , y direction and speed
car->x_vel =	-(
				(( sine_table [ (car->x_rot)>>10 ] * 
				cosine_table [ (car->y_rot)>>10 ] )>>12)
				* (car->speed)
				) ;

car->y_vel =	-(
				(( cosine_table [ (car->x_rot)>>10 ] * 
				cosine_table [ (car->y_rot)>>10 ] )>>12)
				* (car->speed)
				) ;

// add velocity onto x ,y positions
car->x_pos += car->x_vel ;
car->y_pos += car->y_vel ;

car->z_pos = find_ground_height(car->x_pos,car->y_pos) ;

// Find the ground height in front of the car
height_temp = find_ground_height( 	((car->x_pos)-(sine_table [ (car->x_rot)>>10 ]<<11)) , 
									((car->y_pos)-(cosine_table [ (car->x_rot)>>10 ]<<11)) );

// Find the relative height diff between height at front and middle
height_temp = (height_temp-car->z_pos)>>10 ;

// Find the y rotation based on the height and find the relative rot between real and ideal
rot_temp = (find_rotation( -height_temp , (1<<13) ))-car->y_rot ;
if(rot_temp>512*1024) rot_temp-=1024*1024 ;
if(rot_temp<-512*1024) rot_temp+=1024*1024 ;
// Change the y rotation by scale diff
car->y_rot = (car->y_rot+ (rot_temp>>2) )&ROT_LIMIT;


// Find the ground height at side of the car
height_temp = find_ground_height( 	((car->x_pos)-(cosine_table [ (car->x_rot)>>10 ]<<11)) , 
									((car->y_pos)+(sine_table [ (car->x_rot)>>10 ]<<11)) );

// Find the relative height diff between height at front and middle
height_temp = (height_temp-car->z_pos)>>10 ;

// Find the y rotation based on the height and find the relative rot between real and ideal
rot_temp = (find_rotation( -height_temp , (1<<13) ))-car->z_rot ;
if(rot_temp>512*1024) rot_temp-=1024*1024 ;
if(rot_temp<-512*1024) rot_temp+=1024*1024 ;
// Change the z rotation by scale diff
car->z_rot = (car->z_rot+ (rot_temp>>2) )&ROT_LIMIT;


// Fire at target if in range and arm_randomom fire = true
if ( car->fire_request != WEAPON_NOTHING )
{

	ship = (ship_stack*) ( (ship_sdb*) car->special_data )->command_address ;

	// Find the relative dist between sate and target
	x_pos = car->x_pos - ship->x_pos ;
	if ( x_pos < 0 ) x_pos = -x_pos ;

	y_pos = car->y_pos - ship->y_pos ;
	if ( y_pos < 0 ) x_pos = -y_pos ;

	z_pos = car->z_pos - ship->z_pos ;
	if ( z_pos < 0 ) z_pos = -z_pos ;

	// Find the greatest distance
	if ( x_pos > y_pos ) dist = x_pos ; else dist = y_pos ;
	if ( z_pos > dist ) dist = z_pos ;

	if ( dist < (32*(1<<24)) )
	{
		aim_laser( 	car->x_pos ,
					car->y_pos ,
					car->z_pos ,
					ship->x_pos ,
					ship->y_pos ,
					ship->z_pos , 
					car->x_vel ,
					car->y_vel ,
					car->z_vel ,
					ship->x_vel ,
					ship->y_vel ,
					ship->z_vel ,
					(car->performance)->weapons[ WEAPON_LASER ] ,
					(long) car ) ;
	}
}




}

}


//################################################
//#                                              #
//#      Big ship control and decision maker     #
//#                                              #
//################################################

void big_ship_control( ship_stack * ship )

{

long loop ;

rotate_node node_data ;

graphics_details *details = (graphics_details*) ships_adr ;

smoke_details_header *smoke_header ;
smoke_details *smoke_data ;

thruster_details_header *thruster_header ;
thruster_details *thruster_data ;

laser_details_header *laser_header ;
laser_details *laser_data ;

ship_details *ship_data ;

missile_details_header *missile_header ;
missile_details *missile_data ;

ship_stack *pleb_ship ;

ship_stack *target_ship ;
ship_stack *temp_ship ;
long x_dist , y_dist , z_dist ;
long dist , temp_dist ;

target_struct target ;
long x_aim , y_aim ;

long laser_type ;

// Test collsion stuff
//ship_stack test_ship ;
//long y,z ;
//static long x = -8 ;


//command_override
//0 = normal
//1 = rotate z
//2 = rotate x
//3 = static
//#define BIG_SHIP_COMMAND_NORMAL			0
//#define BIG_SHIP_COMMAND_ROTATE_Z		1
//#define BIG_SHIP_COMMAND_ROTATE_X		2
//#define BIG_SHIP_COMMAND_STATIC			3


//if ( ship->can_see != 0)
//{
// Test collision
//for (x = -10 ; x <= 10 ; x+=1 )
//{
//for (y = -10 ; y <= 10 ; y+=1 )
//{
//for (z = -10 ; z <= 10 ; z+=1 )
//{

//	test_ship.x_pos = ship->x_pos + (x<<22) ;
//	test_ship.y_pos = ship->y_pos + (y<<22) ;
//	test_ship.z_pos = ship->z_pos + (z<<22) ;
	
//	if ( big_ship_collision_check( ship , &test_ship ) != 0)
//	{

//	add_smoke( 	test_ship.x_pos , test_ship.y_pos , test_ship.z_pos ,
//				ship->x_vel , ship->y_vel , ship->z_vel ,
//				LASER_HIT_SMOKE ,
//				0 );
//	}
//}
//}
//}
//}


//ship->fire_request = WEAPON_NOTHING ;


switch ( (ship->special_data)->command_override )
{

case BIG_SHIP_COMMAND_NORMAL :

	target.x_pos = ship->x_pos ;
	target.y_pos = ship->y_pos ;
	target.z_pos = ship->z_pos ;
	target.x_aim = ship->goto_x ;
	target.y_aim = ship->goto_y ;
	target.z_aim = ship->goto_z ;

	target_finder( &target );

	// Find the relative rotation between big ship and aim
	x_aim = (target.x_rot) - (ship->x_rot) ;
	y_aim = (target.y_rot) - (ship->y_rot) ;
	if (x_aim > 512*1024) x_aim -= 1024*1024 ;
	if (y_aim > 512*1024) y_aim -= 1024*1024 ;
	if (x_aim < -512*1024) x_aim += 1024*1024 ;
	if (y_aim < -512*1024) y_aim += 1024*1024 ;

	// Limit this value
	if (x_aim > 64*1024) x_aim = 64*1024 ;
	if (y_aim > 64*1024) y_aim = 64*1024 ;
	if (x_aim < -64*1024) x_aim = -64*1024 ;
	if (y_aim < -64*1024) y_aim = -64*1024 ;

	// Add these values onto the rate of change
	ship->x_control += (x_aim>>2) ;
	ship->y_control += (y_aim>>2) ;

	// Auto centre the rates of change
	ship->x_control -= ((ship->x_control)>>4) ;
	ship->y_control -= ((ship->y_control)>>4) ;

	// Turn the big ship
	ship->x_rot = ((ship->x_rot)+((ship->x_control)>>6) )&ROT_LIMIT ;
	ship->y_rot = ((ship->y_rot)+((ship->y_control)>>6) )&ROT_LIMIT ;

	// Roll abit based on x rate of change - just for effect
	ship->z_rot = (-(ship->x_control)>>2)&ROT_LIMIT ;

	// Test stuf - set big ship rot to player
	//ship->x_rot = players_ship->x_rot ;
	//ship->y_rot = players_ship->y_rot ;
	//ship->z_rot = players_ship->z_rot ;

	// Fix the speed
	ship->speed = 640 ;

	// calculate the x,y,z velocities based on x , y direction and speed
	ship->x_vel =	-(
					(( sine_table [ (ship->x_rot)>>10 ] * 
					cosine_table [ (ship->y_rot)>>10 ] )>>12)
					* (ship->speed)
					) ;

	ship->y_vel =	-(
					(( cosine_table [ (ship->x_rot)>>10 ] * 
					cosine_table [ (ship->y_rot)>>10 ] )>>12)
					* (ship->speed)
					) ;

	ship->z_vel =	( sine_table [ (ship->y_rot)>>10 ] * (ship->speed) ) ;



	// add velocity onto x ,y ,z positions
	ship->x_pos += ship->x_vel ;
	ship->y_pos += ship->y_vel ;
	ship->z_pos += ship->z_vel ;

break ;

case BIG_SHIP_COMMAND_ROTATE_Z :

	ship->z_rot = (ship->z_rot + 8*1024)&ROT_LIMIT ;
	ship->speed = 0 ;
	ship->x_vel = 0 ;
	ship->y_vel = 0 ;
	ship->z_vel = 0 ;

break ;


case BIG_SHIP_COMMAND_ROTATE_X :

	ship->x_rot = (ship->x_rot + 8*1024)&ROT_LIMIT ;
	ship->speed = 0 ;
	ship->x_vel = 0 ;
	ship->y_vel = 0 ;
	ship->z_vel = 0 ;

break ;

case BIG_SHIP_COMMAND_STATIC :

	ship->speed = 0 ;
	ship->x_vel = 0 ;
	ship->y_vel = 0 ;
	ship->z_vel = 0 ;

break ;

}

// Big ships always have their thrusters on
// check if it can be seen
if (ship->can_see)
{
	
	if ( ship->type != ((BIG_SHIP<<4)+2) )
	{
		
		thruster_header = (thruster_details_header*) (details+(ship->type))->thruster_data ;
		
		// Is there some thruster data here
		if (thruster_header != NULL)
		{
		thruster_data = (thruster_details*) ( thruster_header + 1 ) ;
			for (loop = 0 ; loop <= thruster_header->counter ; loop += 1)
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
							-ship->x_vel , -ship->y_vel , -ship->z_vel ,
							BIG_THRUSTER_SMOKE ,
							0 ) ;
				thruster_data += 1 ;
			}
		}
	}
	
	// Are the shields low on this big ship - if so smoke abit
	if ( ship->shields < 128 )
	{
		smoke_header = (smoke_details_header*) (details+(ship->type))->smoke_data ;
		// Is there some smoke data here
		if (smoke_header != NULL)
		{
		smoke_data = (smoke_details*) ( smoke_header + 1 ) ;
			for (loop = 0 ; loop <= smoke_header->counter ; loop += 1)
			{
				node_data.x_pos = (smoke_data->x_pos) ;
				node_data.y_pos = (smoke_data->y_pos) ;
				node_data.z_pos = (smoke_data->z_pos) ;
				node_data.x_rot = ship->x_rot ;
				node_data.y_rot = ship->y_rot ;
				node_data.z_rot = ship->z_rot ;
				rotate_node_from_c( &node_data );
				add_smoke( 	(ship->x_pos) + node_data.x_pos ,
							(ship->y_pos) + node_data.y_pos ,
							(ship->z_pos) + node_data.z_pos ,
							-ship->x_vel , -ship->y_vel , -ship->z_vel ,
							BIG_DAMAGE_SMOKE ,
							0 ) ;
				smoke_data += 1 ;
			}
		}
	}
}

// Does the Big ship wish to fire its lasers
if (ship->fire_request == WEAPON_LASER && ship->counter == 0 && ship->target != NULL ) 
{
	laser_header = (laser_details_header*) (details+(ship->type))->laser_data ;
	laser_data = (laser_details*) ( laser_header + 1 ) ;

	make_sound( ship->x_pos , ship->y_pos , ship->z_pos , LASER_SOUND ) ;

	for (loop = 0 ; loop <= laser_header->counter ; loop += 1)
	{
		laser_type = ground_laser_type+loop ;
		
		if ( laser_type >= LASER_POWER_6 ) laser_type = LASER_POWER_6 ;
		
		node_data.x_pos = (laser_data->x_pos) ;
		node_data.y_pos = (laser_data->y_pos) ;
		node_data.z_pos = (laser_data->z_pos) ;
		node_data.x_rot = ship->x_rot ;
		node_data.y_rot = ship->y_rot ;
		node_data.z_rot = ship->z_rot ;
		rotate_node_from_c( &node_data );
		
		// Find the nearest ship to this laser
		
		// Assume that its the first target
		target_ship = (ship_stack*) ship->target ;
		x_dist = target_ship->x_pos - (ship->x_pos+node_data.x_pos) ;
		if (x_dist < 0) x_dist = -x_dist ;
		
		y_dist = target_ship->y_pos - (ship->y_pos+node_data.y_pos) ;
		if (y_dist < 0) y_dist = -y_dist ;
		
		z_dist = target_ship->z_pos - (ship->z_pos+node_data.z_pos) ;
		if (z_dist < 0) z_dist = -z_dist ;
		
		if (x_dist>y_dist) dist = x_dist ; else dist = y_dist ;
		if (z_dist>dist) dist = z_dist ;
		
		// See if the second target is a better option
		
		// Is there a second ship
		if (ship->target2 != NULL)
		{
			temp_ship = (ship_stack*) ship->target2 ;
			x_dist = temp_ship->x_pos - (ship->x_pos+node_data.x_pos) ;
			if (x_dist < 0) x_dist = -x_dist ;
		
			y_dist = temp_ship->y_pos - (ship->y_pos+node_data.y_pos) ;
			if (y_dist < 0) y_dist = -y_dist ;
		
			z_dist = temp_ship->z_pos - (ship->z_pos+node_data.z_pos) ;
			if (z_dist < 0) z_dist = -z_dist ;
		
			if (x_dist>y_dist) temp_dist = x_dist ; else temp_dist = y_dist ;
			if (z_dist>temp_dist) temp_dist = z_dist ;
		
			// Is it nearer than the current target
			if (temp_dist<dist)
			{
				dist = temp_dist ;
				target_ship = temp_ship ;
			}
		}
		
		// See if the third target is a better option
		
		// Is there a third ship
		if (ship->target3 != NULL)
		{
			temp_ship = (ship_stack*) ship->target3 ;
			x_dist = temp_ship->x_pos - (ship->x_pos+node_data.x_pos) ;
			if (x_dist < 0) x_dist = -x_dist ;
		
			y_dist = temp_ship->y_pos - (ship->y_pos+node_data.y_pos) ;
			if (y_dist < 0) y_dist = -y_dist ;
		
			z_dist = temp_ship->z_pos - (ship->z_pos+node_data.z_pos) ;
			if (z_dist < 0) z_dist = -z_dist ;
		
			if (x_dist>y_dist) temp_dist = x_dist ; else temp_dist = y_dist ;
			if (z_dist>temp_dist) temp_dist = z_dist ;
			
			// Is it nearer than the current target
			if (temp_dist<dist)	target_ship = temp_ship ;
		}

		
		
		aim_laser( 	ship->x_pos + node_data.x_pos ,
					ship->y_pos + node_data.y_pos ,
					ship->z_pos + node_data.z_pos ,
					target_ship->x_pos ,
					target_ship->y_pos ,
					target_ship->z_pos ,
					ship->x_vel ,
					ship->y_vel ,
					ship->z_vel ,
					target_ship->x_vel ,
					target_ship->y_vel ,
					target_ship->z_vel ,
					laser_type , (long) ship ) ;
		laser_data += 1 ;
	}
	ship->counter = 2 ;
}


// Does the Big ship wish to fire a missile
if (ship->fire_request == WEAPON_ATA && ship->counter == 0 && ship->target != NULL )
{
	missile_header = (missile_details_header*) (details+(ship->type))->missile_data ;
	if (missile_header != NULL)
	{
		missile_data = (missile_details*) ( missile_header + 1 ) ;
		for (loop = 0 ; loop <= missile_header->counter ; loop += 1)
		{
			node_data.x_pos = (missile_data->x_pos) ;
			node_data.y_pos = (missile_data->y_pos) ;
			node_data.z_pos = (missile_data->z_pos) ;
			node_data.x_rot = ship->x_rot ;
			node_data.y_rot = ship->y_rot ;
			node_data.z_rot = ship->z_rot ;
			rotate_node_from_c( &node_data );
			add_sam( 	ship->x_pos + node_data.x_pos ,
						ship->y_pos + node_data.y_pos ,
						ship->z_pos + node_data.z_pos ,
						(ship_stack*) ship->target ,
						(long) ship ) ;
			missile_data += 1 ;
		}
	
		if ( (ship_stack*) ship->target == players_ship )
		{
			message_add( MESSAGE_TOP, MTXT__WARNING_INCOMING_ATAM , RED_15 , MESSAGE_FLASH , 24 , MESSAGE_NOCURSOR );
			sound_playsample( SOUND_INCOMING , 60 , 127 , 0 ) ;
		}

		ship->counter = 4 ;
	}
}

// Is it a mega bomb situation
if (ship->fire_request == WEAPON_MEGA_BOMB )
{
	add_mega_bomb( ship ) ;
}



// Does the Big ship wish to launch a pleb ship
if (ship->fire_request == WEAPON_LAUNCH_SHIP && ship->counter == 0)
//if ( (players_ship->fire_request)!=WEAPON_NOTHING )
{
//printf("Big ship launched pleb");

	ship_data = (ship_details*) (details+(ship->type))->ship_data ;
	// Is there any ship data here ?
	if (ship_data != NULL)
	{
		node_data.x_pos = (ship_data->x_pos) ;
		node_data.y_pos = (ship_data->y_pos) ;
		node_data.z_pos = (ship_data->z_pos) ;
		node_data.x_rot = ship->x_rot ;
		node_data.y_rot = ship->y_rot ;
		node_data.z_rot = ship->z_rot ;
		rotate_node_from_c( &node_data );
		pleb_ship = add_ship( 	ship->x_pos + node_data.x_pos ,
								ship->y_pos + node_data.y_pos ,
								ship->z_pos + node_data.z_pos ,
								((ship->x_rot)-512*1024)&ROT_LIMIT ,
								(SMALL_SHIP<<4)+3 ,
								&pleb_special_data ,
								&mission.performances[3+1]
							) ;
		
		// They belong to the big ship to avoid crashes on launch
		pleb_ship->who_owns_me = (long) ship ;
			
		// Set the rotations to the opposite of the big ship 
		pleb_ship->y_rot = (-(ship->y_rot))&ROT_LIMIT ;
		pleb_ship->z_rot = (-(ship->z_rot))&ROT_LIMIT ;
	
		// Set up the launch commands
		pleb_ship->command = COMMAND_TAKEOFF_CLIMB ;
		pleb_ship->command_counter = COMMAND_TAKEOFF_CLIMB_COUNTER ;
		
		// Set this pleb to attack the player
		war_settoattack( pleb_ship , players_ship ) ;

		sound_playsample( SOUND_INCOMING , 60 , 127 , 0 ) ;
	
		ship->counter = 8 ;
	}
}

if (ship->counter > 0) ship->counter -= 1 ;

}


//###########################################################
//#															#
//#															#
//#		All purpose ship add to stack routine 				#
//#															#
//#															#
//###########################################################


ship_stack*  add_ship ( long x_pos , long y_pos , long z_pos ,
						long x_rot , long type ,
						ship_sdb *special_data ,
						performance_data *performance )

{

ship_stack *ship ;

static test_the_fucking_fighters = 0 ;


//if (type!=(PLAYERS_SHIP<<4)) return( NULL ) ;

// Test
//if ( (type>>4) == CAR ) type += 1 ;


// It has been decided that all friendly small fighters are of type 3
// All enemy small fighters are of type 0,1,2
if (special_data != NULL && (type>>4) == SMALL_SHIP )
{
	if ( special_data->side == SDB_SIDE_FRIEND )
	{
		type = (SMALL_SHIP<<4) + 3 ;
	}
	else
	{
		if ( type == (SMALL_SHIP<<4)+3 )
		{
			type = (SMALL_SHIP<<4) + (arm_random()&1) + (arm_random()&1) ;
		}
	}
}


ship = (ship_stack*) armlink_sorttolist( &ships , x_pos ) ;

if (ship != NULL)
{

memset( (void*) ( ((char*) ship) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

ship->x_pos = x_pos ;
ship->y_pos = y_pos ;
ship->z_pos = z_pos ;
ship->x_rot = x_rot ;
ship->y_rot = 0 ;
ship->z_rot = 0 ;
ship->type = type ;
ship->x_dir = ship->x_rot ;
ship->y_dir = ship->y_rot ;
ship->thrust_control = 1024 ;
ship->x_control = 0 ;
ship->y_control = 0 ;
ship->speed = 0 ;
ship->x_vel = 0 ;
ship->y_vel = 0 ;
ship->z_vel = 0 ;
ship->performance = performance ;
ship->can_see = 0 ;
ship->special_data = special_data ;

if ( (type>>4) == SMALL_SHIP && test_the_fucking_fighters > 4)
{
//ship->z_pos = 1<<24 ;
//ship->speed = 8192 ;
//ship->y_rot = 3*256*1024 ;
}

if ( (type>>4) == SMALL_SHIP) test_the_fucking_fighters += 1 ;


ship->z_roller = 0 ;

// Misc counter used for thrusters on small ships 
// and number of plebs can launch from big ones

ship->misc_counter = 0 ;

if (performance != NULL)
{
	ship->shields = (performance->shields)<<5 ;
}
else
{
	ship->shields = 100 ;
}

ship->who_owns_me = (long) ship ;

ship->fire_request = WEAPON_NOTHING ; 

if ( (type>>4) == BIG_SHIP || (type>>4) == CAR )
{
	ship->collision_size = 1<<26 ;
}
else
{
	if ( (type>>4) == PLAYERS_SHIP )
	{
	ship->collision_size = 1<<22 ;	
	}
	else
	{
	ship->collision_size = 1<<23 ;
	}
}


//( (ship_sdb*) ship->specail_data)->control_mode 
// command_address = which ship is involved
//#define	SDB_MODE_ATTACK				0			// Ship control modes
//#define SDB_MODE_FLIGHTPATH			1
//#define SDB_MODE_FORMATION 			2

if (special_data != NULL )
{
	switch ( special_data->control_mode )
	{

	case SDB_MODE_ATTACK :

	ship->command = COMMAND_ATTACK_FAR ;

	break ;

	case SDB_MODE_FLIGHTPATH :

	ship->command = COMMAND_FLIGHT_PATH ;

	break ;

	case SDB_MODE_FORMATION :

	ship->command = COMMAND_FIXED_FORMATION ;
	
	break ;

	}
}

ship->command_counter = COMMAND_NOTHING_COUNTER ;

ship->aim_counter = arm_random()&7 ;

ship->aim_goto_x = 0 ;
ship->aim_goto_y = 0 ;
ship->aim_goto_z = 0 ;

ship->aim_x_vel = 0 ;
ship->aim_y_vel = 0 ;
ship->aim_z_vel = 0 ;

ship->aim_x_rot = ship->x_rot ;
ship->aim_y_rot = ship->y_rot ;
ship->aim_z_rot = ship->z_rot ;

ship->last_fire_request = -1 ;
ship->who_hit_me = -1 ;

control_recheckcamera( ship ) ;

}

return ( ship ) ;

}
