#include "Setup_Tables.h"
#include "Graphic_Struct.h"
#include "stdio.h"
#include "Collision.h"
#include "SF_ArmLink.h"
#include "Bit_Control.h"
#include "Smoke_Control.h"
#include "Laser_Control.h"
#include "Explosion.h"
#include "SF_ArmUtils.h"
#include "Graphics_Set.h"
#include "Sound_Control.h"

void setup_tables( void )

{

static long compiled = 0 ;

static graphics_details players_ship_graphic ;

long loop , loop2 , temp_long , temp_long2 ;
void *memptr [20];

graphics_details* reloc ;
graphics_details* details ;

long x_size , y_size ;
long type ;
long coll_loop ;
laser_stack *laser ;
long hangar_capicity = 4 ;

explosion_details *explosion_data ;
explosion_details_header *explosion_header ;

section_groups*				section_group_data ;
section_details_header*		section_data_header ;
section_details*			section_data ;

// Reset camera stuff
ship_viewed_last_frame = NULL ;
ship_viewed_last_frame_x_rot = 0 ;


test_mode = 0 ;

// Reset the ship sound pointers
sound_small_ship = NULL ;
sound_big_ship = NULL ;
sound_channel_small_ship = -1 ;
sound_channel_big_ship = -1 ;
sound_sample_small_ship = SMALL_SHIP_ENGINE_OFF ;


// Reset the bonus stuff
bonus_crystal_object.object_1 = OBJECT_NOTHING ;
bonus_crystal_object.object_2 = OBJECT_NOTHING ;
bonus_crystal_object.object_3 = OBJECT_NOTHING ;
bonus_crystal_object.object_4 = OBJECT_NOTHING ;

// Reset players control rates
players_x_control = 0 ;
players_y_control = 0 ;
players_thrust_control = 0 ;


ground_ship_rate	=	(mission.difficulty)>>4 ;

// ################################################
// ################################################
// THIS HAS CHANGED
ground_laser_rate	=	( mission.difficulty * LASER_FIRE_RATE )>>10 ;

// ################################################
// ################################################
// THIS HAS CHANGED
ground_sam_rate		= 	((mission.difficulty) * SAM_FIRE_RATE)>>12 ;

//printf("mission difficulty %d\n",mission.difficulty);

// mission.defence_timer > 0 dont fire
hangar_capicity = mission.hangar_capacity ;
// mission.commit_rate 

ground_ship_type 	=	(long) mission.shiplaunchtype_ground ;
ground_laser_type	=	(long) mission.ground_lasertype ;
air_ship_type		=	(long) mission.shiplaunchtype_air ;


camera_x_velocity = 0 ;
camera_y_velocity = 0 ;
camera_z_velocity = 0 ;


// Setup / clear these link lists
armlink_initialise( MAX_BITS , &bits , sizeof (bit_stack) , 0 ) ;
armlink_initialise( MAX_SMOKES , &smokes , sizeof (smoke_stack) , 0 ) ;
armlink_initialise( MAX_LASERS , &lasers , sizeof (laser_stack) , 0 ) ;
armlink_initialise( MAX_EXPLOSIONS , &explosions , sizeof (explosion_stack) , 0 ) ;
// Tim sets up the ship list in SF_War.c
// armlink_initialise( MAX_SHIPS , &ships , sizeof (ship_stack) , 0 ) ;


// Setup the docking details for the players ship
if (mission.starting_docked == 0)
{
	docked.docked_this_frame = 0 ;
	docked.ship = 0 ;
	docked.counter = 0 ;
	docked.status = DOCKING_OUT ;
}
else
{
	docked.docked_this_frame = 1 ;
	docked.ship = status.docked ;
	docked.counter = 0 ;
	docked.status = DOCKING_FIXED ;
}


// Grab some lasers for the beam laser
for (loop = 0 ; loop < 32 ; loop += 1 )
{

laser = armlink_sorttolist( &lasers , 0 ) ;

beam_lasers [ loop ] = laser ;

laser->type = BEAM_LASER ;
laser->x_vel = 0 ;
laser->y_vel = 0 ;
laser->z_vel = 0 ;
laser->who_owns_me = (long) players_ship ;

armlink_suspenditem( laser , &lasers );

}

// Reset the beam laser stuff
beam_laser.on_last_frame = 0 ;
beam_laser.x_rot_bend = 0 ;
beam_laser.y_rot_bend = 0 ;



// Has this graphics file been relocated yet or wot
if (graphics_data[3] != 193543)
{
	which_graphics_set = graphics_data[3] ;

	graphics_data[3] = 193543 ;
		
	if ( which_graphics_set == SPACE_GRAPHICS )
	{
		are_we_in_space_or_wot = 1 ;
	}
	else
	{
		are_we_in_space_or_wot = 0 ;
	}
	
	static_graphics_adr = (long*) (graphics_data) + ((graphics_data[0])>>2) ;
	ships_adr = (long*) (graphics_data) + ((graphics_data[1])>>2) ;
	explosion_bits_adr = (long*)  (graphics_data) + ((graphics_data[2])>>2) ;

	// Re-locate the pointers to graphics data
	// Keep em as null if not set
	reloc = (graphics_details*) static_graphics_adr ;
	for (loop = 0 ; loop < 64 ; loop += 1)
	{
		if(reloc->graphic_adr != 0)			reloc->graphic_adr += (long) (static_graphics_adr) ;
		if(reloc->explosion_adr != 0)		reloc->explosion_adr += (long) (static_graphics_adr) ;
		if(reloc->collision_adr != 0)		reloc->collision_adr += (long) (static_graphics_adr) ;
		if(reloc->laser_data != 0)			reloc->laser_data += (long) (static_graphics_adr) ;
		if(reloc->missile_data != 0)		reloc->missile_data += (long) (static_graphics_adr) ;
		if(reloc->ship_data != 0)			reloc->ship_data += (long) (static_graphics_adr) ;
		if(reloc->smoke_data != 0)			reloc->smoke_data += (long) (static_graphics_adr) ;
		if(reloc->thruster_data != 0)		reloc->thruster_data += (long) (static_graphics_adr) ;
		
		// Fix min height for missile aim
		if(reloc->missile_aim < (1<<22)) reloc->missile_aim = (1<<22) ;
		
		// Fix score
		if (reloc->score <= 0) reloc->score = (25*(arm_random()&255)) ;
		
		if(reloc->extra_data != 0)
		{
			reloc->extra_data += (long) (static_graphics_adr) ;
			
			section_group_data = (section_groups*) reloc->extra_data ;
			
			if (section_group_data->group_0 != 0) section_group_data->group_0 += (long) (static_graphics_adr) ;
			if (section_group_data->group_1 != 0) section_group_data->group_1 += (long) (static_graphics_adr) ;
			if (section_group_data->group_2 != 0) section_group_data->group_2 += (long) (static_graphics_adr) ;
			if (section_group_data->group_3 != 0) section_group_data->group_3 += (long) (static_graphics_adr) ;
			if (section_group_data->group_4 != 0) section_group_data->group_4 += (long) (static_graphics_adr) ;
			if (section_group_data->group_5 != 0) section_group_data->group_5 += (long) (static_graphics_adr) ;
			if (section_group_data->group_6 != 0) section_group_data->group_6 += (long) (static_graphics_adr) ;
		
		
			if (section_group_data->group_1 != 0)
			{
				section_data_header = (section_details_header*) section_group_data->group_1 ;
				
				//printf("Found section data for group 1\n");
				//printf("Counter set to %d\n",section_data_header->counter);
				
				section_data = (section_details*) (section_data_header+1) ;
				
				for ( loop2 = 0 ; loop2<=section_data_header->counter ; loop2 += 1)
				{
				//printf("Type %d\n",section_data->type);
				//printf("X pos %d\n",section_data->x_pos);
				//printf("Y pos %d\n",section_data->y_pos);
				//printf("Z pos %d\n",section_data->z_pos);
				//printf("X pos 2 %d\n",section_data->x_pos2);
				//printf("Y pos 2 %d\n",section_data->y_pos2);
				//printf("Z pos 2 %d\n",section_data->z_pos2);
				//printf("X vel %d\n",section_data->x_vel);
				//printf("Y vel %d\n",section_data->y_vel);
				//printf("Z vel %d\n",section_data->z_vel);
				
				section_data +=1 ;
				}
			}
		}
		
		reloc += 1;		// Next object
	}
	


	reloc = (graphics_details*) ships_adr ;
	for (loop = 0 ; loop < 128 ; loop += 1)
	{
		if(reloc->graphic_adr != 0)			reloc->graphic_adr += (long) (ships_adr) ;
		if(reloc->explosion_adr != 0)		reloc->explosion_adr += (long) (ships_adr) ;
		if(reloc->collision_adr != 0)		reloc->collision_adr += (long) (ships_adr) ;
		if(reloc->laser_data != 0)			reloc->laser_data += (long) (ships_adr) ;
		if(reloc->missile_data != 0)		reloc->missile_data += (long) (ships_adr) ;
		if(reloc->ship_data != 0)			reloc->ship_data += (long) (ships_adr) ;
		if(reloc->smoke_data != 0)			reloc->smoke_data += (long) (ships_adr) ;
		if(reloc->thruster_data != 0)		reloc->thruster_data += (long) (ships_adr) ;
		//if(reloc->ship_2_data != 0)			reloc->ship_2_data += (long) (ships_adr) ;

		// Fix score
		if (reloc->score <= 0) reloc->score = (25*(arm_random()&255)) ;

		if(reloc->extra_data != 0)
		{
			reloc->extra_data += (long) (ships_adr) ;
			
			section_group_data = (section_groups*) reloc->extra_data ;
			
			if (section_group_data->group_0 != 0) section_group_data->group_0 += (long) (ships_adr) ;
			if (section_group_data->group_1 != 0) section_group_data->group_1 += (long) (ships_adr) ;
			if (section_group_data->group_2 != 0) section_group_data->group_2 += (long) (ships_adr) ;
			if (section_group_data->group_3 != 0) section_group_data->group_3 += (long) (ships_adr) ;
			if (section_group_data->group_4 != 0) section_group_data->group_4 += (long) (ships_adr) ;
			if (section_group_data->group_5 != 0) section_group_data->group_5 += (long) (ships_adr) ;
			if (section_group_data->group_6 != 0) section_group_data->group_6 += (long) (ships_adr) ;
		}

		// Is it the players ship - yes grab its pointers and stash them
		if ( loop == 0 )
		{
			players_ship_graphic.graphic_adr		= reloc->graphic_adr ;
			players_ship_graphic.explosion_adr		= reloc->explosion_adr ;
			players_ship_graphic.collision_adr		= reloc->collision_adr ;
			players_ship_graphic.laser_data			= reloc->laser_data ;
			players_ship_graphic.missile_data		= reloc->missile_data ;
			players_ship_graphic.ship_data			= reloc->ship_data ;
			players_ship_graphic.smoke_data			= reloc->smoke_data ;
			players_ship_graphic.thruster_data		= reloc->thruster_data ;
		//	players_ship_graphic.ship_2_data		= reloc->ship_2_data ;

			players_ship_graphic.x_size				= reloc->x_size ;	
			players_ship_graphic.y_size				= reloc->y_size ;
			players_ship_graphic.z_size				= reloc->z_size ;
			players_ship_graphic.clip_size			= reloc->clip_size ;

		}

		reloc += 1;		// Next ship
	}

	reloc = (graphics_details*) explosion_bits_adr ;
	for (loop = 0 ; loop < 16 ; loop += 1)
	{
		if(reloc->graphic_adr != 0)			reloc->graphic_adr += (long) (explosion_bits_adr) ;
		if(reloc->explosion_adr != 0)		reloc->explosion_adr += (long) (explosion_bits_adr) ;
		if(reloc->collision_adr != 0)		reloc->collision_adr += (long) (explosion_bits_adr) ;
		if(reloc->laser_data != 0)			reloc->laser_data += (long) (explosion_bits_adr) ;
		if(reloc->missile_data != 0)		reloc->missile_data += (long) (explosion_bits_adr) ;
		if(reloc->ship_data != 0)			reloc->ship_data += (long) (explosion_bits_adr) ;
		if(reloc->smoke_data != 0)			reloc->smoke_data += (long) (explosion_bits_adr) ;
		if(reloc->thruster_data != 0)		reloc->thruster_data += (long) (explosion_bits_adr) ;
		//if(reloc->ship_2_data != 0)			reloc->ship_2_data += (long) (explosion_bits_adr) ;
		reloc += 1;		// Next explosion bit
	}
}





// mission.ship_flying = player = 0 1-4 fighter

// Address of the players ship details
details	= (graphics_details*) ships_adr ;

// What ship is the player flying on this mission
if ( mission.ship_flying == 0 )
{
	details->graphic_adr		= players_ship_graphic.graphic_adr ;
	details->explosion_adr		= players_ship_graphic.explosion_adr ;
	details->collision_adr		= players_ship_graphic.collision_adr ;
	details->laser_data			= players_ship_graphic.laser_data  ;
	details->missile_data		= players_ship_graphic.missile_data ;
	details->ship_data			= players_ship_graphic.ship_data ;
	details->smoke_data			= players_ship_graphic.smoke_data ;
	details->thruster_data		= players_ship_graphic.thruster_data ;
	//details->ship_2_data		= players_ship_graphic.ship_2_data ;

	details->x_size				= players_ship_graphic.x_size ;	
	details->y_size				= players_ship_graphic.y_size ;
	details->z_size				= players_ship_graphic.z_size ;
	details->clip_size			= players_ship_graphic.clip_size ;
}
else
{
	reloc = (graphics_details*) ships_adr ;
	reloc += (SMALL_SHIP<<4) + (mission.ship_flying-1) ;

	details->graphic_adr		= reloc->graphic_adr ;
	details->explosion_adr		= reloc->explosion_adr ;
	details->collision_adr		= reloc->collision_adr ;
	details->laser_data			= reloc->laser_data  ;
	details->missile_data		= reloc->missile_data ;
	details->ship_data			= reloc->ship_data ;
	details->smoke_data			= reloc->smoke_data ;
	details->thruster_data		= reloc->thruster_data ;
	//details->ship_2_data		= reloc->ship_2_data ;

	details->x_size				= reloc->x_size ;	
	details->y_size				= reloc->y_size ;
	details->z_size				= reloc->z_size ;
	details->clip_size			= reloc->clip_size ;
}


// Test set the players graphic to something else

//	reloc = (graphics_details*) ships_adr ;
//	reloc += (PARACHUTE<<4) ;

//	details->graphic_adr		= reloc->graphic_adr ;
//	details->explosion_adr		= reloc->explosion_adr ;
//	details->collision_adr		= reloc->collision_adr ;
//	details->laser_data			= reloc->laser_data  ;
//	details->missile_data		= reloc->missile_data ;
//	details->ship_data			= reloc->ship_data ;
//	details->smoke_data			= reloc->smoke_data ;
//	details->thruster_data		= reloc->thruster_data ;
//	details->ship_2_data		= reloc->ship_2_data ;

//	details->x_size				= reloc->x_size ;	
//	details->y_size				= reloc->y_size ;
//	details->z_size				= reloc->z_size ;
//	details->clip_size			= reloc->clip_size ;


setup_collision_constants( (long) planet_info.space_mission);

// Setup machine code with all the required constants
// To save setting up another array use cosine_table as a temp store

memptr [ 0 ] = &cosine_table ;
memptr [ 1 ] = &pex_table ;
memptr [ 2 ] = &rotated_coords ;
memptr [ 3 ] = &height_map ;
memptr [ 4 ] = &screen_coords ;
memptr [ 5 ] = &cel_quad ;

// These are already pointers

memptr [ 6 ] = 0 ; // &plot_graphic_delay ;
memptr [ 7 ] = 0 ; // &cel_list16;		Not reqd.
memptr [ 8 ] = &sprite_map ;
memptr [ 9 ] = 0 ; // &cel_list32;		Not reqd.

// Temp workspace for graphics
memptr [ 10 ] = &graphic_rotated_coords ;
memptr [ 11 ] = &graphic_screen_coords ;

// Various data
memptr [ 12 ] = &star_coords ;
memptr [ 13 ] = &poly_map ;
memptr [ 14 ] = &skyfile ;
memptr [ 15 ] = &graphics_data ;

machine_code_constants (&memptr) ;


memptr [ 0 ] = &rotated_coords ;
memptr [ 1 ] = &screen_coords ;
memptr [ 2 ] = &height_map ;
memptr [ 3 ] = &pex_table ;
memptr [ 4 ] = &quick_height_table ;
memptr [ 5 ] = &poly_map ;
memptr [ 6 ] = &sprite_map ;
memptr [ 7 ] = &cel_quad ;
memptr [ 8 ] = &cosine_table ;
memptr [ 9 ] = (void*) planet_info.space_mission ;

plot_land_constants (&memptr) ;



// Setup the perspective data table once only
if (compiled == 0)
{
	compiled = 1 ;

	for (loop = 0 ; loop < 16384 ; loop += 1)
	{
		if (loop < 16 )
		{
			temp_long=16;
		}
		else
		{
			temp_long= (loop<<1) ;
		}
			pex_table [ loop ] = ((2<<17)+(2<<16)) / temp_long ;
		
		// Near by perspective
		if ( loop < 2048 )
		{
			if (loop < 32 )
			{
				temp_long=32;
			}
			else
			{
				temp_long=loop ;
			}
			
			pex_table_near [ loop ] = ((2<<17)+(2<<16)) / temp_long ;
		}
	}
}


// Clear the collision map
for (loop = 0 ; loop < 128 ; loop += 1)
{
	for (loop2 = 0 ; loop2 < 128 ; loop2 += 1)
	{
		collision_map [ loop2 ] [ loop ] = 0 ; 
	}
}

// Setup map collision data pointers
// This is for the static object collision detection
// Collisions are found by looking at the poly_map data to see if a
// graphic is at a given x,y pos.
// Problems occur when the graphic is larger than 1 poly_map unit ( 1<<25 )
// To get round this big objects have pointers around them pointing to the
// object you may have collided with.

for (loop = 0 ; loop <128 ; loop +=1)
{
	for (loop2 = 0 ; loop2 < 128 ; loop2 +=1)
	{
		type = (long) poly_map [ loop2 ] [ loop ] ;
	
		// Is this a valid object for collision checking
		if (type != 0 && type < 64)
		{
			details = (graphics_details*) static_graphics_adr ;
			details += type ;
		
			// Set the hits counter for this object
			collision_map [ loop2 ] [ loop ] = (char) details->hits_counter ;
		
			// Get size in poly map units (1<<25)
			x_size = ( (details->x_size) + (1<<23) )>>25 ;
			y_size = ( (details->y_size) + (1<<23) )>>25 ;
			
			if ( x_size > 0 )
			{
				for (coll_loop = 1 ; coll_loop <= x_size ; coll_loop +=1)
				{
					poly_map [ loop2 ] [ ((loop-coll_loop)&127) ] = COLL_RIGHT ;
					poly_map [ loop2 ] [ ((loop+coll_loop)&127) ] = COLL_LEFT ;
				}
			}
		
			if ( y_size > 0 )
			{
				for (coll_loop = 1 ; coll_loop <= y_size ; coll_loop +=1 )
				{
				
					poly_map [ ((loop2+coll_loop)&127) ] [ loop ] = COLL_DOWN ;
					poly_map [ ((loop2-coll_loop)&127) ] [ loop ] = COLL_UP ;
		
					if ( x_size > 0 )
					{
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop-1)&127) ] = COLL_DOWN_RIGHT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop-1)&127) ] = COLL_UP_RIGHT ;
					
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop+1)&127) ] = COLL_DOWN_LEFT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop+1)&127) ] = COLL_UP_LEFT ;
					}
			
					if ( x_size > 1 )
					{
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop-2)&127) ] = COLL_DOWN_RIGHT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop-2)&127) ] = COLL_UP_RIGHT ;
					
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop+2)&127) ] = COLL_DOWN_LEFT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop+2)&127) ] = COLL_UP_LEFT ;
					}
					
					if ( x_size > 2 )
					{
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop-3)&127) ] = COLL_DOWN_RIGHT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop-3)&127) ] = COLL_UP_RIGHT ;
					
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop+3)&127) ] = COLL_DOWN_LEFT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop+3)&127) ] = COLL_UP_LEFT ;
					}
					
					if ( x_size > 3 )
					{
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop-4)&127) ] = COLL_DOWN_RIGHT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop-4)&127) ] = COLL_UP_RIGHT ;
					
						poly_map [ ((loop2+coll_loop)&127) ] [ ((loop+4)&127) ] = COLL_DOWN_LEFT ;
						poly_map [ ((loop2-coll_loop)&127) ] [ ((loop+4)&127) ] = COLL_UP_LEFT ;
					}					
				}
			}
		}
	}
}



// Add arm_randomom clouds etc.
// And check for special objects - hangars etc.
for (loop = 0 ; loop <= 127 ; loop += 1 )
{
    for (loop2 = 0 ; loop2 <= 127 ; loop2 +=1 )
	{
		
		if ( poly_map [ loop2 ] [ loop ] == 7 || poly_map [ loop2 ] [ loop ] == 8 )
		{	
			collision_map [ loop2 ] [ (loop+1)&127 ] = hangar_capicity ;
		}
		
		
		// If we are not in space them add some clouds
		if ( which_graphics_set != DEATH_STAR_GRAPHICS && which_graphics_set != SPACE_GRAPHICS )
		{
			// Slight chance of clouds
			if ( poly_map [ loop2 ] [ loop ] == 0 && ((arm_random())&2047)==0 )
			{
				poly_map [ loop2 ] [ loop ] = 60 ;	
			}
		
			// If over sea then big chance of clouds
			if ( height_map [ loop2<<1 ] [ loop<<1 ] < 17 && ((arm_random())&127)==0 )
			{
				poly_map [ loop2 ] [ loop ] = 60 ;
			}
		}
		
		// Last minute new trees for graphics set earth - randomly replace big trees
		if (	which_graphics_set == EARTH_LAND_BASED_GRAPHICS &&
			 	poly_map [ loop2 ] [ loop ] == EL_BIG_TREE &&
				(arm_random()&1) == 0 )
		{
			poly_map [ loop2 ] [ loop ] = EL_TALL_TREE ;
		}
		
		// If we are in space add some rocks
		if ( planet_info.space_mission == 1)
		{
			// Space - add some rocks
			// 9 - 16 small rocks
			// 17 - 24 big rocks

			if ( poly_map [ loop2 ] [ loop ] == 0 )
			{
				if ( (arm_random()&7) == 1 )
				{
					temp_long = (arm_random()&7) ;
					
					// Big rock or small rock - remember the big rocks are slow to plot !
					if ( (arm_random()&3) == 1 )
					{
						// Complex astroid
						poly_map [ loop2 ] [ loop ] = SP_AST_1_H0 + temp_long ;
					}
					else
					{
						// Simple astroid
						poly_map [ loop2 ] [ loop ] = SP_AST_2_H0 + temp_long ;
					}
					
					// fixed rock height
					height_map [ loop2<<1 ] [ loop<<1 ] = 17 + (temp_long<<3) ;
				}
			}
			else
			{
				if ( poly_map [loop2] [loop] == SP_SPACE_GUNL || poly_map [loop2] [loop] == SP_SPACE_SAML )
				{
					height_map [ loop2<<1 ] [ loop<<1 ] = 17 ;
				}

				if ( poly_map [loop2] [loop] == SP_SPACE_GUNM || poly_map [loop2] [loop] == SP_SPACE_SAMM )
				{
					height_map [ loop2<<1 ] [ loop<<1 ] = 17+32 ;
				}

				if ( poly_map [loop2] [loop] == SP_SPACE_GUNH || poly_map [loop2] [loop] == SP_SPACE_SAMH )
				{
					height_map [ loop2<<1 ] [ loop<<1 ] = 17+64 ;
				}			
			}
		}
	}
}




// Setup star_coords with random x,y,z coords 
for ( loop = 0 ; loop <= 127 ; loop += 1 )
{
	temp_long = (arm_random())&1023;
	temp_long2 = (arm_random())&511;

	if (loop <= 1) temp_long2 = ((temp_long2)&127)+32 ;

	// Set the x,y,z coords for the star

	if (planet_info.space_mission == 1)
	{
		star_coords [loop] [0] = (arm_random()&((1<<16)-1)) ;
		star_coords [loop] [1] = (arm_random()&((1<<16)-1)) ;
		star_coords [loop] [2] = (arm_random()&((1<<16)-1)) ;
	}
	else
	{
		star_coords [loop] [0]= (((cosine_table[temp_long])*(cosine_table[temp_long2])))>>10;
		star_coords [loop] [1]= (((sine_table[temp_long])*(cosine_table[temp_long2])))>>10;
		star_coords [loop] [2]= -(sine_table[temp_long2])<<2;
	}

	// Set the value of the type of star to plot
	* ((char*) &star_coords [loop] [3] +0) = (arm_random())&3 ;

	// Set the value of the shade offset for the star
	* ((char*) &star_coords [loop] [3] +1) = (arm_random())&15 ;
}

// Planet ref 12 , 13
// Setup the random planet pos
temp_long = ((arm_random())&1023) ;
temp_long2 = (temp_long + 512 + (arm_random()&511))&1023 ;

planet_1_x_pos = (cosine_table [ temp_long ])<<2 ;
planet_1_y_pos = (sine_table [ temp_long ])<<2 ;
planet_1_z_pos = -(sine_table [ (32+((arm_random())&63)) ]<<2) ;

planet_2_x_pos = (cosine_table [ temp_long2 ])<<2 ;
planet_2_y_pos = (sine_table [ temp_long2 ])<<2 ;
planet_2_z_pos = -(sine_table [ (32+((arm_random())&63)) ]<<2) ;

/* Exit with cosine + perspective table setup + temp test hills */	
}

