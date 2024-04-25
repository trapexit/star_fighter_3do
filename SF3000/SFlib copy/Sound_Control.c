#include "SF_Sound.h"
#include "Plot_Graphic.h"
#include "Sound_Control.h"
#include "Maths_Stuff.h"
#include "Misc_Struct.h"

//#include "SF_Status.h"
//#include "SF_Message.h"
//extern long test_coll_count ;

void make_sound ( long x_pos , long y_pos , long z_pos , long type )
{
long pitch , sample ;
sound_details sound_data ;


get_sound_details( &sound_data , x_pos , y_pos , z_pos ) ;

// Samples currently available
// SOUND_LASER
// SOUND_MISSILE
// SOUND_THUD
// SOUND_EXPLOSION

switch( type )
{

	case LASER_SOUND :
	sample = SOUND_LASER ;
	pitch = 45 ;
	break ;

	case LASER_SOUND_1 :
	sample = SOUND_LASER ;
	pitch = 45 ;
	break ;

	case LASER_SOUND_2 :
	sample = SOUND_LASER ;
	pitch = 47 ;
	break ;

	case LASER_SOUND_3 :
	sample = SOUND_LASER ;
	pitch = 49 ;
	break ;

	case LASER_SOUND_4 :
	sample = SOUND_LASER ;
	pitch = 51 ;
	break ;

	case LASER_SOUND_5 :
	sample = SOUND_LASER ;
	pitch = 53 ;
	break ;

	case LASER_SOUND_6 :
	sample = SOUND_LASER ;
	pitch = 55 ;
	break ;

 	case LASER_SOUND_7 :
	sample = SOUND_LASER ;
	pitch = 57 ;
	break ;

	case LASER_SOUND_8 :
	sample = SOUND_LASER ;
	pitch = 60 ;
	break ;

	case MISSILE_SOUND :
	sample = SOUND_MISSILE ;
	pitch = 65 ;
	break ;

	case LASER_HIT_SOUND :
	sample = SOUND_THUD ;
	pitch = 50 ;
	break ;

	case BIG_EXPLOSION_SOUND :
	sample = SOUND_EXPLOSION ;
	pitch = 50 ;
	break ;

	case SMALL_EXPLOSION_SOUND :
	sample = SOUND_EXPLOSION ;
	pitch = 70 ;
	break ;

	case THUD_SOUND :
	sample = SOUND_THUD ;
	pitch = 70 ;
	break ;

}

// Make Sound						Volume (0-127)
sound_playsample( sample , pitch , sound_data.volume , sound_data.stereo_pos ) ;

}


void get_sound_details( sound_details* sound_data , long x_pos , long y_pos , long z_pos )
{
long dist ;
long stereo_pos , volume ;
long x_dist , y_dist , z_dist ;
rotate_node node_data ;
target_struct target ;

// Get the signed x,y,z distance
x_dist = (x_pos - camera_x_position) ;
y_dist = (y_pos - camera_y_position) ;
z_dist = (z_pos - camera_z_position) ;

// Find the relative rotated position
node_data.x_pos = (x_dist>>12) ;
node_data.y_pos = (y_dist>>12) ;
node_data.z_pos = (z_dist>>12) ;
node_data.x_rot = (camera_x_rotation<<10) ;
node_data.y_rot = (camera_y_rotation<<10) ;
node_data.z_rot = (camera_z_rotation<<10) ;
rotate_land_node_from_c( &node_data );

//printf("Rot node x %d\n",node_data.x_pos) ;
//printf("Rot node y %d\n",node_data.y_pos) ;
//printf("Rot node z %d\n\n",node_data.z_pos) ;

// Find the angle of the rotated relative position
target.x_pos = 0 ;
target.y_pos = 0 ;
target.z_pos = 0 ;
target.x_aim = (node_data.x_pos<<12) ;
target.y_aim = (node_data.y_pos<<12) ;
target.z_aim = (node_data.z_pos<<12) ;
target_finder( &target );

// Use the cosine of the relative x rot to get the stereo pos (+ve / -ve 16384)
stereo_pos = (-(sine_table [ (target.x_rot>>10) ]) << 2) ;

//printf("X rot %d\n",target.x_rot) ;
//printf("Y rot %d\n\n",target.y_rot) ;

// Find the real distance using the angles and relative dist
//x_dist = ((node_data.x_pos)*sine_table [ (target.x_rot>>10) ]) ;
//y_dist = ((node_data.y_pos)*cosine_table [ (target.x_rot>>10) ]) ;
//z_dist = ((node_data.z_pos)*cosine_table [ (target.y_rot>>10) ]) ;

//if (x_dist < 0) x_dist = -x_dist ;
//if (y_dist < 0) y_dist = -y_dist ;
//if (z_dist < 0) z_dist = -z_dist ;

//dist = (x_dist + y_dist + z_dist)>>14 ;

dist = target.distance ;

volume = dist>>14 ;

// Is this sound too far away to hear
if (volume>(64<<10)) volume = (64<<10) ;

// Is the sound too near
if ( volume < (1<<12) ) volume = (1<<12) ;

volume = ((127<<12)/volume) ;

//printf("Dist %d\n\n",dist) ;

//printf("Sound volume %d\n",volume);
//printf("Sound stereo %d\n\n",stereo_pos);

sound_data->stereo_pos = stereo_pos ;
sound_data->volume = volume ;
sound_data->distance = dist ;

}

// sound_alter( channel_no , volume 0-127 , stereo_pos 0-16384 , 


void stop_engine_sounds( void )
{

// Is there a sample playing
if ( sound_channel_small_ship != -1 )
{
	sound_channel_small_ship = sound_stopsample ( sound_channel_small_ship ) ;
}

// Is there a sample playing
if ( sound_channel_big_ship != -1 )
{
	sound_channel_big_ship = sound_stopsample ( sound_channel_big_ship ) ;
}

// Is there a sample playing
if ( sound_channel_shields_low != -1 )
{
	sound_channel_shields_low = sound_stopsample ( sound_channel_shields_low ) ;
}

engine_sounds_on_or_wot = 1 ;

}

void update_engine_sounds( void )
{

long dist , last_dist ;
long pitch_bend ;
sound_details sound_data ;

// Are the engine sounds switched off
if ( engine_sounds_on_or_wot == 1 ) return ;


// Is there a small ship to make a sound
if ( sound_small_ship != NULL )
{
	// Is there a sample playing
	if ( sound_channel_small_ship == -1 )
	{
		// Which sample to start with (engine on / engine off)
		if ( sound_small_ship->thrust_control == 0 )
		{
			sound_channel_small_ship = sound_playsample( SOUND_WIND , 70, 127, 0 ) ;
			sound_sample_small_ship = SMALL_SHIP_ENGINE_OFF ;
		}
		else
		{
			sound_channel_small_ship = sound_playsample( SOUND_ENGINE, 70, 127, 0 ) ;
			sound_sample_small_ship = SMALL_SHIP_ENGINE_ON ;
		}
	}

	// Is there a change of state of engine sample (off >> on)
	if ( sound_small_ship->thrust_control != 0 && sound_sample_small_ship == SMALL_SHIP_ENGINE_OFF )
	{
		// The ship has changed from thruster off to thruster on
		
		// Stop the current engine off wind sound
		sound_channel_small_ship = sound_stopsample ( sound_channel_small_ship ) ;		
		
		// Start the thrusters on sound and set flag
		sound_channel_small_ship = sound_playsample( SOUND_ENGINE, 70, 127, 0 ) ;		
		sound_sample_small_ship = SMALL_SHIP_ENGINE_ON ;
	}

	// Is there a change of state of engine sample (on >> off)
	if ( sound_small_ship->thrust_control == 0 && sound_sample_small_ship == SMALL_SHIP_ENGINE_ON )
	{
		// The ship has changed from thruster on to thruster off
		
		// Stop the current engine thruster sound
		sound_channel_small_ship = sound_stopsample ( sound_channel_small_ship ) ;
		
		// Start the engine off wind sound and set flag
		sound_channel_small_ship = sound_playsample( SOUND_WIND , 70 , 127 , 0 ) ;		
		sound_sample_small_ship = SMALL_SHIP_ENGINE_OFF ;
	}

	get_sound_details(
						&sound_data ,
						sound_small_ship->x_pos ,
						sound_small_ship->y_pos ,
						sound_small_ship->z_pos
						) ;
	dist = sound_data.distance ;
	
	camera_x_position += camera_x_velocity ;
	camera_y_position += camera_y_velocity ;
	camera_z_position += camera_z_velocity ;
		
	get_sound_details(
						&sound_data ,
						sound_small_ship->x_pos - sound_small_ship->x_vel ,
						sound_small_ship->y_pos - sound_small_ship->y_vel ,
						sound_small_ship->z_pos - sound_small_ship->z_vel
						) ;
	last_dist = sound_data.distance ;

	camera_x_position -= camera_x_velocity ;
	camera_y_position -= camera_y_velocity ;
	camera_z_position -= camera_z_velocity ;

	pitch_bend = (last_dist-dist)>>9 ;

	//Test print up speed
	//status.score = 0 ;
	//message_addscore( test_coll_count ) ;

// sound_alter( channel_no , volume 0-127 , stereo_pos 0-16384 , 

	sound_alter( sound_channel_small_ship , sound_data.volume , sound_data.stereo_pos );

	if ( pitch_bend > (12*1024) ) pitch_bend = (12*1024) ;
	if ( pitch_bend < -(12*1024) ) pitch_bend = -(12*1024) ;
	
	if ( pitch_bend > (48*1024) || pitch_bend < -(48*1024) ) pitch_bend = 0 ;

	if ( sound_sample_small_ship == SMALL_SHIP_ENGINE_ON )
	{
		sound_pitchbend (	sound_channel_small_ship , 
							(9*1024) + (sound_small_ship->thrust_control<<2)
							+ (sound_small_ship->speed << 4) 
							+ pitch_bend ) ;
		sound_alter( sound_channel_small_ship , sound_data.volume , sound_data.stereo_pos );
	}
	else
	{
		sound_pitchbend (	sound_channel_small_ship , 
							(9*1024)
							+ (sound_small_ship->speed << 5) 
							+ pitch_bend ) ;	
		sound_alter( sound_channel_small_ship , sound_data.volume , sound_data.stereo_pos );
	}
}

// Is the small ship sound stopping 
if ( sound_small_ship == NULL && sound_channel_small_ship != -1 )
{
	sound_channel_small_ship = sound_stopsample( sound_channel_small_ship ) ;
}


// Is there a big ship to make a sound
if ( sound_big_ship != NULL )
{
	// Is there a sample playing
	if ( sound_channel_big_ship == -1 )
	{
		sound_channel_big_ship = sound_playsample( SOUND_ENGINE, 70, 127, 0 ) ;
	}

	get_sound_details(
						&sound_data ,
						sound_big_ship->x_pos ,
						sound_big_ship->y_pos ,
						sound_big_ship->z_pos
						) ;
	dist = sound_data.distance ;
	
	camera_x_position += camera_x_velocity ;
	camera_y_position += camera_y_velocity ;
	camera_z_position += camera_z_velocity ;
		
	get_sound_details(
						&sound_data ,
						sound_big_ship->x_pos - sound_big_ship->x_vel ,
						sound_big_ship->y_pos - sound_big_ship->y_vel ,
						sound_big_ship->z_pos - sound_big_ship->z_vel
						) ;
	last_dist = sound_data.distance ;

	camera_x_position -= camera_x_velocity ;
	camera_y_position -= camera_y_velocity ;
	camera_z_position -= camera_z_velocity ;

	pitch_bend = (last_dist-dist)>>11 ;

	sound_alter( sound_channel_big_ship , sound_data.volume , sound_data.stereo_pos );

	if ( pitch_bend > (12*1024) ) pitch_bend = (12*1024) ;
	if ( pitch_bend < -(12*1024) ) pitch_bend = -(12*1024) ;
	
	if ( pitch_bend > (48*1024) || pitch_bend < -(48*1024) ) pitch_bend = 0 ;

	sound_pitchbend (	sound_channel_big_ship , 
						(12*1024)
						+ (sound_big_ship->speed << 4)
						+ pitch_bend ) ;
}

// Is the big ship sound stopping 
if ( sound_big_ship == NULL && sound_channel_big_ship != -1 )
{
	sound_channel_big_ship = sound_stopsample( sound_channel_big_ship ) ;
}


}

