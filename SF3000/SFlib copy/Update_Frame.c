
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
#include "Collision_Update.h"
#include "SF_Mission.h"
#include "Sound_Control.h"
#include "SF_ArmUtils.h"
#include "Bit_Control.h"
#include "Explosion.h"
#include "Graphics_Set.h"
#include "SF_Joystick.h"
#include "SF_Sound.h"
#include "Sound_Control.h"

#include "SF_Status.h"
#include "SF_Message.h"

extern game_status status ;

void update_frame (long keypad )

{
long loop , temp_long ;
ship_stack *ship ;
ship_stack *temp_ship ;
laser_stack *laser ;
long type ;
long *end_of_collision_stack ;
long *start_of_collision_stack = temp_store ;

static long test_graphic = 1 ;
static key_press = 0 ;
static view_wot = 0 ;

//####################################################################
//####################################################################
//####################################################################
//####################################################################
//####################################################################

if (test_mode == 1)
{

if ( keypad & ControlA )
{
	if (key_press == 0)
	{
		if (view_wot == 0)
		{
			test_graphic = test_display_static_graphics( test_graphic );
			key_press = 1 ;
		}
		else
		{
			test_graphic += 1 ;
			if (test_graphic>25) test_graphic = 1 ;
			key_press = 1 ;
		}
	}
}
else
{
	key_press = 0 ;
}

if ( (keypad & ControlRightShift) && (keypad & ControlLeftShift) )
{
	view_wot = 1-view_wot ;
	test_graphic = 1 ;
}

if (keypad & ControlRightShift)
{
	camera_y_position += (1<<22) ;
}

if (keypad & ControlLeftShift)
{
	camera_y_position -= (1<<22) ;
}

if (keypad & ControlDown)
{
	if ( keypad & ControlC )
	{
		camera_z_position += (1<<22) ;
	}
	else
	{
		players_ship->y_rot = (players_ship->y_rot-(16*1024))&ROT_LIMIT ;
	}
}

if (keypad & ControlUp)
{
	if ( keypad & ControlC )
	{
		camera_z_position -= (1<<22) ;
	}
	else
	{
		players_ship->y_rot = (players_ship->y_rot+(16*1024))&ROT_LIMIT ;
	}
}

if (keypad & ControlRight)
{
	if ( keypad & ControlC )
	{
		camera_x_position -= (1<<22) ;
	}
	else
	{
		players_ship->x_rot = (players_ship->x_rot-(16*1024))&ROT_LIMIT ;
	}
}

if (keypad & ControlLeft)
{
	if ( keypad & ControlC )
	{
		camera_x_position += (1<<22) ;
	}
	else
	{
		players_ship->x_rot = (players_ship->x_rot+(16*1024))&ROT_LIMIT ;
	}
}

if (view_wot == 0)
{
	players_ship->type = SECTION+test_graphic ;
}
else
{
	switch( test_graphic )
	{
	case 1 :
	players_ship->type = (PLAYERS_SHIP<<4) ;
	break ;
	case 2 :
	players_ship->type = (SMALL_SHIP<<4) ;
	break ;
	case 3 :
	players_ship->type = (SMALL_SHIP<<4)+1 ;
	break ;
	case 4 :
	players_ship->type = (SMALL_SHIP<<4)+2 ;
	break ;
	case 5 :
	players_ship->type = (SMALL_SHIP<<4)+3 ;
	break ;
	case 6 :
	players_ship->type = (BIG_SHIP<<4) ;
	break ;
	case 7 :
	players_ship->type = (BIG_SHIP<<4)+1 ;
	break ;
	case 8 :
	players_ship->type = (BIG_SHIP<<4)+2 ;
	break ;
	case 9 :
	players_ship->type = (WEAPON<<4) ;
	break ;
	case 10 :
	players_ship->type = (WEAPON<<4)+1 ;
	break ;
	case 11 :
	players_ship->type = (WEAPON<<4)+2 ;
	break ;
	case 12 :
	players_ship->type = (WEAPON<<4)+3 ;
	break ;
	case 13 :
	players_ship->type = (WEAPON<<4)+4 ;
	break ;
	case 14 :
	players_ship->type = (WEAPON<<4)+5 ;
	break ;
	case 15 :
	players_ship->type = (BONUS<<4) ;
	break ;
	case 16 :
	players_ship->type = (BONUS<<4)+1 ;
	break ;
	case 17 :
	players_ship->type = (BONUS<<4)+2 ;
	break ;
	case 18 :
	players_ship->type = (BONUS<<4)+3 ;
	break ;
	case 19 :
	players_ship->type = (BONUS<<4)+4 ;
	break ;
	case 20 :
	players_ship->type = (BONUS<<4)+5 ;
	break ;
	case 21 :
	players_ship->type = (BONUS<<4)+6 ;
	break ;
	case 22 :
	players_ship->type = (BONUS<<4)+7 ;
	break ;
	case 23 :
	players_ship->type = (PARACHUTE<<4) ;
	break ;
	case 24 :
	players_ship->type = (SATELLITE<<4) ;
	break ;
	case 25 :
	players_ship->type = (CAR<<4) ;
	break ;
	}
}

players_ship->counter = 0 ;


players_ship->x_pos = 0 ;
players_ship->y_pos = 0 ;
players_ship->z_pos = 0 ;


if (camera_y_position > 0 ) camera_y_position = 0 ;

if (camera_y_position < -(32<<24) ) camera_y_position = -(32<<24) ;

if (camera_x_position < -(8<<24) ) camera_x_position = -(8<<24) ;
if (camera_z_position < -(8<<24) ) camera_z_position = -(8<<24) ;
if (camera_x_position > (8<<24) ) camera_x_position = (8<<24) ;
if (camera_z_position > (8<<24) ) camera_z_position = (8<<24) ;

camera_x_rotation = 0 ;
camera_y_rotation = 0 ;
camera_z_rotation = 0 ;

return;

}

//####################################################################
//####################################################################
//####################################################################
//####################################################################
//####################################################################


// Enter test mode
if (	(keypad & ControlLeft) &&
		(keypad & ControlDown) &&
		(keypad & ControlLeftShift) &&
		(keypad & ControlRightShift) &&
		(keypad & ControlA) &&
		(keypad & ControlC) ) test_mode = 1 ;


//players_ship->shields = 128 ;


// Toggle this counter between 0 n 1 - used by ground laser bases
laser_counter = (1-laser_counter) ;

if (players_ship->shields > 0)
{
	// When the player is using a digital input (such a the 3DO joypad / keyboard etc.)
	// use the following code which will slowly build up the control rate and auto
	// centre in a nice kind of fashion

	// If a proportional joystick is being used then set the x_control and y_control to
	// the joystick position (+ve / -ve 1024) thrust_control (0-1024)

	// Are they using the control pad or pro stick
	if ( configuration.control_method == 0 )
	{
		// Check if the engine on key is being pressed
		if ( keypad & keypad_iomap[configuration.flight_controls[FLIGHT_THRUST]] )
		{
			// If the thruster is on then increase engine power and limit to max val.
			players_thrust_control += 256 ;
			if (players_thrust_control>1024) players_thrust_control = 1024 ;
		}
		else
		{
			// If the thruster is off then decrease power to min
			players_thrust_control -= ((players_thrust_control)>>2) ;
			players_thrust_control -= 128 ;
			if (players_thrust_control<0) players_thrust_control = 0 ;
		}
	
		if ( keypad & ControlRight ) 
		{
			// if the current roll is against the control then speed up the control rate
			// and auto centre the controls quicker
			if ((players_x_control)>0)
			{
				players_x_control -= 64 ;
				players_x_control -= ((players_x_control)>>1) ;
			}
			else
			{
				players_x_control -= 64 ;
				players_x_control += ((players_x_control)>>2) ;
			}
		}
	
		if ( keypad & ControlLeft )
		{
			if (players_x_control<0)
			{
				players_x_control += 64 ;
				players_x_control -= ((players_x_control)>>1) ;
			}
			else
			{
				players_x_control += 64 ;
				players_x_control += ((players_x_control)>>2) ;
			}
		}
	
		if ( keypad & ControlUp )
		{
			if ((players_y_control)>0)
			{
				players_y_control -= 64 ;
				players_y_control -= ((players_y_control)>>1) ;
			}
			else
			{
				players_y_control -= 64 ;
				players_y_control += ((players_y_control)>>2) ;
			}
		}
	
	
		if ( keypad & ControlDown )
		{
			if ((players_y_control)<0)
			{
				players_y_control += 64 ;
				players_y_control -= ((players_y_control)>>1) ;
			}
			else
			{
				players_y_control += 64 ;
				players_y_control += ((players_y_control)>>2) ;
			}
		}
	
		// auto centre control positions
		// Max dec (based on 1024) = 128
		players_x_control -= ((players_x_control)>>3) ;
		players_y_control -= ((players_y_control)>>3) ;
		players_z_control -= ((players_z_control)>>3) ;
	
		// Limit the control rates to max (+/-ve 1024)
		if (players_x_control>1024) players_x_control = 1024 ;
		if (players_y_control>1024) players_y_control = 1024 ;
		if (players_z_control>1024) players_z_control = 1024 ;
	
		if (players_x_control<-1024) players_x_control = -1024 ;
		if (players_y_control<-1024) players_y_control = -1024 ;
		if (players_z_control<-1024) players_z_control = -1024 ;
	
	}
	else
	{
		
		// pro_stick.x - / 0 = left / 160 = centre / 320 = right
		// pro_stick.y - / 0 = dive / 120 = centre / 240 = climb
		// pro_stick.z - 97 = no thrust / 0 = full thrust

		// Update the controls for a proportional joystick
		
		// Update the thrust
		temp_long = (11 * (97 - pro_stick.z) ) - players_thrust_control ;
		if ( temp_long > 256 ) temp_long = 256 ;
		if ( temp_long < -128 ) temp_long = -128 ;
		
		players_thrust_control += temp_long ;
		
		if (players_thrust_control>1024) players_thrust_control = 1024 ;
		if (players_thrust_control<0) players_thrust_control = 0 ;
	
		// Update left and right
		temp_long = (7 * (160 - pro_stick.x) ) - players_x_control ;
		if ( temp_long > 128 ) temp_long = 128 ;
		if ( temp_long < -128 ) temp_long = -128 ;
	
		players_x_control += temp_long ;
	
		// Update up and down
		temp_long = (9 * (pro_stick.y - 120) ) - players_y_control ;
		if ( temp_long > 128 ) temp_long = 128 ;
		if ( temp_long < -128 ) temp_long = -128 ;
	
		players_y_control += temp_long ;
	
		// Limit the control rates to max (+/-ve 1024)
		if (players_x_control>1024) players_x_control = 1024 ;
		if (players_y_control>1024) players_y_control = 1024 ;
	
		if (players_x_control<-1024) players_x_control = -1024 ;
		if (players_y_control<-1024) players_y_control = -1024 ;
	
	
		// Check the player fire key and set selected weapon to fire if required
		if (pro_stick.b & StickFire)
		{
			players_ship->fire_request = status.weapon ;
			players_ship->last_fire_request = status.weapon ; 
		}
	
	}

	// Check the player fire key and set selected weapon to fire if required
	if ( keypad & keypad_iomap[configuration.flight_controls[FLIGHT_FIRE_WEAPON]] )
	{
		players_ship->fire_request = status.weapon ;
		players_ship->last_fire_request = status.weapon ; 
	}

	// Check the player ecm and set to fire if required
	if ( keypad & keypad_iomap[configuration.flight_controls[FLIGHT_FIRE_ECM]] )
	{
		players_ship->fire_request = WEAPON_ECM ;
		players_ship->last_fire_request = WEAPON_ECM ; 
	}

	// Switch the engine sounds on
	engine_sounds_on_or_wot = 0 ;

	// Shields low alarm sound
	if (	(players_ship->shields < 32 || players_ship->damage != DAMAGE_NOTHING) )
	{
		if (sound_channel_shields_low == -1)
		{
			sound_channel_shields_low = sound_playsample( SOUND_ALARM , 60, 127, 0 ) ;
		}
	}
	else
	{
		if ( sound_channel_shields_low != -1 )
		{
			sound_channel_shields_low = sound_stopsample( sound_channel_shields_low ) ;
		}
	}


	// Switch the ground object target lock on or off
	if ( status.weapon == WEAPON_ATG || status.weapon == WEAPON_BEAM_LASER )
	{
		atg_selected = 1;
	}
	else
	{
		atg_selected = 0;
	}

	// Switch the air object target lock on or off
	if ( status.weapon == WEAPON_ATA || status.weapon == WEAPON_BEAM_LASER )
	{
		ata_selected = 1;
	}
	else
	{
		ata_selected = 0;
	}
}
else
{
	// The player is dead
	ata_selected = 0 ;
	atg_selected = 0 ;
	players_ship->fire_request = WEAPON_NOTHING ;

	if ( sound_channel_shields_low != -1 )
	{
		sound_channel_shields_low = sound_stopsample( sound_channel_shields_low ) ;
	}

	// Reset the players control rates
	players_x_control = 0 ;
	players_y_control = 0 ;
	players_thrust_control = 0 ;
}


// Set the players ships controls
players_ship->x_control = players_x_control ;

// Is it reverse controls
if ( configuration.flight_controls[ FLIGHT_FLIP ] == 0 )
{
	players_ship->y_control = players_y_control ;
}
else
{
	players_ship->y_control = -players_y_control ;
}

players_ship->z_control = players_z_control ;
players_ship->thrust_control = players_thrust_control ;



// Do the movement of these items
if (bonus_collision_this_frame>0) bonus_collision_this_frame -= 1 ;
update_bonus_crystal_adder();

smoke_update();

laser_update();

bit_update();

explosion_update();

if (mission.defence_timer <= 0 &&
	players_ship->shields > 0 &&
	players_ship->z_pos < (40<<24) ) 	
ground_update();


// Update the players ship first if it's alive
// This makes sure that any player dependent decissions are based on
// its correct position (i.e. Wing Pod positions)
// It also allows the player to have first choice of lasers and smoke free
// in the stack . Useful in big space battles where the laser stack may be
// nearly full and requests for lasers might be turned down

if (players_ship->shields > 0)
{
	ship_control( players_ship );
}
else
{
	// Kill the beam laser if the player is dead
	if (beam_laser.on_last_frame != 0) stop_beam_lasers() ;
	
	// Update the players dead movement for the benifit of the cameras
	players_ship->x_pos += players_ship->x_vel ;
	players_ship->y_pos += players_ship->y_vel ;
	players_ship->z_pos += players_ship->z_vel ;
	
	if ( which_graphics_set != SPACE_GRAPHICS )
	{
		players_ship->z_vel -= (1<<17) ;
		if ( players_ship->z_pos < find_ground_height( players_ship->x_pos , players_ship->y_pos ) )
		{
			players_ship->z_pos = find_ground_height( players_ship->x_pos , players_ship->y_pos ) ;
			if ( players_ship->z_vel < 0 )
			{
				players_ship->z_vel = (-players_ship->z_vel) ;
			}
		}
	}
	
	players_ship->x_vel -= (players_ship->x_vel>>6) ;
	players_ship->y_vel -= (players_ship->y_vel>>6) ;
	players_ship->z_vel -= (players_ship->z_vel>>6) ;
}

//if (arm_random()&255 == 1)
//{
//add_wing_pod( players_ship ) ;
//}

pod_counter = ((pod_counter+1)&3) ;


// Test 
if ( (arm_random()&63) == 0 )
{
	add_ship( arm_random(), arm_random() , (120<<24) , 0 , (PARACHUTE<<4) , 0 , 0 );
}



// Go through the ship link list and update any items found
// The list is sorted along the x position to help the collision
// detection. Therefore any objects found are in no particular
// order and should not be treated as such.
// The only problem is that objects can be added whilst the list
// is being updated - any objects added might or might not be updated
// this frame or the next. However I can't notice this and it only effects
// small things such as weapons / bonus crystals etc.

ship=(ships.info).start_address ;

while ((ship->header).status==1)
{
	type = (ship->type)>>4 ;

	switch( type )
	{
		
	case PLAYERS_SHIP :
	break ;
		
	case SMALL_SHIP :
		ship_control ( ship ) ;
	break ;	
	
	case BIG_SHIP :
		big_ship_control ( ship ) ;
	break ;
	
	case BONUS :
		bonus_control ( ship ) ;
	break ;
	
	case PARACHUTE :
		parachute_control( ship ) ;
	break ;
	
	case SATELLITE :
		satellite_control( ship ) ;
	break ;
	
	case WEAPON :
		weapon_control( ship ) ;
	break ;
	
	case CAR :
		car_control( ship ) ;
	break ;
		
	default :
		explosion_section_control( ship ) ;
		
	}
	
	ship=(ship->header).next_address ;
}


// Update the docking flags for the players ship
docked.docked_this_frame = 0 ;


// Update any laser or ship collisions

collision_update() ;

//make_sound ( players_ship->x_pos , players_ship->y_pos , players_ship->z_pos , ENGINE_SOUND ) ;


// random comets
if ( which_graphics_set != DEATH_STAR_GRAPHICS ) temp_long = (64<<24) ; else temp_long = 0 ;


if (	((arm_random())&1023) < planet_info.comet_rate &&
		camera_z_position > temp_long )
{

temp_long = camera_z_position + (20<<24) ;
if (temp_long > (120<<24) ) temp_long = (120<<24) ;

add_bit(	camera_x_position + ( ( arm_random() & ((64<<24)-1) ) - (32<<24) ) ,
			camera_y_position + ( ( arm_random() & ((64<<24)-1) ) - (32<<24) ) ,
			temp_long ,
			( arm_random() & ((1<<24)-1) ) - (1<<23) ,
			( arm_random() & ((1<<24)-1) ) - (1<<23) ,
			-(1<<18) ,
			2 , 2 ,
			1 , 0 ,
			0 , EXPLOSION_STYLE_COMET );
}

}

