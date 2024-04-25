#include "Graphic_Struct.h"
#include "SF_Status.h"
#include "SF_Message.h"
#include "SF_Status.h"
#include "Bit_Control.h"
#include "Smoke_Control.h"
#include "Ship_Control.h"
#include "SF_ArmLink.h"
#include "Explosion.h"
#include "Sound_Control.h"
#include "Explosion_Struct.h"
#include "SF_ArmLink.h"
#include "Stdio.h"
#include "Bonus_Control.h"
#include "SF_Control.h"
#include "SF_Armburn.h"
#include "Plot_Graphic.h"
#include "Graphics_Set.h"
#include "SF_ArmUtils.h"
#include "Weapons.h"
#include "String.h"

void static_explode( long grid_pos , long damage )
{

long x_grid , y_grid ;
long hits ;
long type ;

x_grid = grid_pos&127 ;
y_grid = grid_pos >> 7 ;

hits = (long) collision_map [ y_grid ] [ x_grid ] ;

hits -= damage ;

type = (long) poly_map [ y_grid ] [ x_grid ] ;

// Is this a valid object to blow up
// It seems that the odd cloud or non graphics seeks through to here
if (type >= 1 && type <= 63 )
{
	if (hits <= 0)
	{

		explosion_check_this_graphic( x_grid , y_grid ) ;
		
	}
	else
	{
		collision_map [ y_grid ] [ x_grid ] = (char) hits ;
	}
}

				
}
  
void explode_next_frame( long grid_ref )
{
// This will set a graphic to explode after a ramdom frame count between 2 - 9
explosion_stack *explosion ;

// Are we trying to blow up a non-graphic or a cloud
if (poly_map [ (grid_ref>>7) ] [ (grid_ref&127) ] == 0 ||
	poly_map [ (grid_ref>>7) ] [ (grid_ref&127) ] >= 64) return ;

explosion = (explosion_stack*) armlink_addtolist( &explosions );

if (explosion != NULL)
	{
	explosion->grid_ref = grid_ref ;
	explosion->counter = 2+(arm_random()&7) ;
	}

}

void explosion_update( void )
{
// This updates the list of graphics to explode and blows them up when counter reachs zero
explosion_stack *explosion ;
explosion_stack *explosion_temp ;

explosion = (explosion_stack*) (explosions.info).start_address ;

while ((explosion->header).status==1)
{

	// Get the adr of the next before any deleting is done
	explosion_temp = (explosion_stack*) (explosion->header).next_address ;
	
	explosion->counter -=1 ;
	
	if ( (explosion->counter) < 0 )
	{
		// delete the explosion and blow up this graphic
		static_explode( explosion->grid_ref , 256 ) ;
		armlink_deleteitem(explosion,&explosions);
	}

	explosion = explosion_temp ;
}

}


void explode_ship ( ship_stack *ship )
{
long loop ;
long size = 0 ;
long group_mask = 255 ; // Set all bits to explode all groups unless set otherwise
long centre_group ;
long x_start , y_start , z_start ;
long x_end , y_end , z_end ;
long style = EXPLOSION_STYLE_NORMAL ;

rotate_node node_data ;

graphics_details *details ;
explosion_details *explosion_data ;
explosion_details_header *explosion_header ;

long explosion_velocity = 8 ;

long explosion_counter = 0 ;
	
// Are we trying to blow up a bonus
if ( (ship->type)>>4 != BONUS && ship->shields != PARACHUTE_NO_EXPLODE )
{
	// Is it rearly a ship or a static graphic explosion section
	if ( ship->type >= SECTION )
	{
		// Get the start adr of the explosion data for this explosion section
		if (ship->type >= SHIP_SECTION)
		{
			details = (graphics_details*) ships_adr ;
			explosion_header = (explosion_details_header*) (details+( (ship->type)-SHIP_SECTION ) )->explosion_adr ;		
			//ship->type -= SHIP_SECTION ;
		}
		else
		{
			details = (graphics_details*) static_graphics_adr ;
			explosion_header = (explosion_details_header*) (details+( (ship->type)-SECTION ) )->explosion_adr ;
			//ship->type -= SECTION ;
		}
		
		group_mask = ~ship->group_mask ;
	}
	else
	{
		details = (graphics_details*) ships_adr ;
		// Get the start adr of the explosion data for this ship
		explosion_header = (explosion_details_header*) (details+(ship->type))->explosion_adr ;
	}
	
	// Get the start adr of the 1st explosion line
	explosion_data = (explosion_details *) (explosion_header + 1) ;

	switch ( ship->type )
	{
	
	case MEGA_BOMB :
		
		// Speed the bits in the direction of the bomb
		ship->x_vel = ((ship->x_vel)<<1) ;
		ship->y_vel = ((ship->y_vel)<<1) ;
		ship->z_vel = (-(ship->z_vel)>>1) ;
		explosion_velocity = 4 ;
		style = EXPLOSION_STYLE_MEGA_BOMB ;
		
		explosion_counter = 8 ;
		
	break ;
	
	}

	if (	((ship->type)>>4) == BIG_SHIP ||
			((ship->type)>>4) == PLAYERS_SHIP ||
			((ship->type)>>4) == SMALL_SHIP ||
			((ship->type)>>4) == SATELLITE )
	
	{
		if (	((ship->type)>>4) == PLAYERS_SHIP ||
				((ship->type)>>4) == SMALL_SHIP ||
				((ship->type)>>4) == SATELLITE )
		{
			if ( (ship->type>>4) == PLAYERS_SHIP )
			{
				explosion_counter = 4 ;
			}
			else
			{
				explosion_counter = 2 ;			
			}
			
			explosion_velocity = 8 ;
			style = EXPLOSION_STYLE_LOADS_OF_BITS ;		
		}		
		else
		{
			size = 6 ;
			explosion_velocity = 16 ;
		}

		if ( ((arm_random())&1) == 0)
		{
			// Release group 0 - centre bit
			
			// Explode the remaining groups
			group_mask = (group_mask & (GROUP_1 + GROUP_2 + GROUP_3 + GROUP_4 + GROUP_5) ) ;
			
			centre_group = 0 ;
			add_ship_explosion_section(	ship ,
										(GROUP_1 + GROUP_2 + GROUP_3 + GROUP_4 + GROUP_5) , // Don't release these groups
										centre_group ) ;
		}


		if ( ((arm_random())&1) == 0)
		{
			// Release group 1 - wing
			
			// Explode the remaining groups
			group_mask = (group_mask & (GROUP_0 + GROUP_2 + GROUP_3 + GROUP_4 + GROUP_5) ) ;
			
			centre_group = 1 ;
			add_ship_explosion_section(	ship ,
										(GROUP_0 + GROUP_2 + GROUP_3 + GROUP_4 + GROUP_5) , // Don't release these groups
										centre_group ) ;
		}

		if ( ((arm_random())&1) == 0)
		{
			// Release group 2 - another wing
			
			// Explode the remaining groups
			group_mask = (group_mask & (GROUP_0 + GROUP_1 + GROUP_3 + GROUP_4 + GROUP_5) ) ;
			
			centre_group = 2 ;
			add_ship_explosion_section(	ship ,
										(GROUP_0 + GROUP_1 + GROUP_3 + GROUP_4 + GROUP_5) , // Don't release these groups
										centre_group ) ;
		}
		
		if ( ((arm_random())&1) == 0)
		{
			// Release group 3 - yet another wing
			
			// Explode the remaining groups
			group_mask = (group_mask & (GROUP_0 + GROUP_1 + GROUP_2 + GROUP_4 + GROUP_5) ) ;
			
			centre_group = 3 ;
			add_ship_explosion_section(	ship ,
										(GROUP_0 + GROUP_1 + GROUP_2 + GROUP_4 + GROUP_5) , // Don't release these groups
										centre_group ) ;
		}

		if ( ((arm_random())&1) == 0)
		{
			// Release group 4 - nose cone or something
			
			// Explode the remaining groups
			group_mask = (group_mask & (GROUP_0 + GROUP_1 + GROUP_2 + GROUP_3 + GROUP_5) ) ;
			
			centre_group = 4 ;
			add_ship_explosion_section(	ship ,
										(GROUP_0 + GROUP_1 + GROUP_2 + GROUP_3 + GROUP_5) , // Don't release these groups
										centre_group ) ;
		}

		if (	((ship->type)>>4) == PLAYERS_SHIP ||
				((ship->type)>>4) == SMALL_SHIP ||
				((ship->type)>>4) == SATELLITE )
		{
			group_mask = 255 ;
		}

	}

	if ( ship->type >= SECTION && ship->type < SHIP_SECTION )
	{
		size = 8 ;
		explosion_velocity = 22 ;
	}


	if ( ship->type >= SHIP_SECTION )
	{
		if ( ((ship->type&255)>>4) == BIG_SHIP )
		{
			size = 6 ;
			explosion_velocity = 16 ;
		}
		else
		{
			explosion_velocity = 8 ;
			style = EXPLOSION_STYLE_LOADS_OF_BITS ;
		}
	}

	
	for ( loop=0 ; loop<=explosion_counter ; loop += 1 )
	{
		if ( (ship->type>>4) == PARACHUTE )
		{
			explode_ship_from_collision_box(	ship ,
												explosion_velocity ,
												-1 ,
												style ,
												group_mask , loop ) ;		
		}
		else
		{
			explode_ship_from_collision_box(	ship ,
												explosion_velocity ,
												(long) ship ,
												style ,
												group_mask , loop ) ;
		}
	}


	make_sound( ship->x_pos , ship->y_pos , ship->z_pos , SMALL_EXPLOSION_SOUND ) ;

	if ( (ship->type>>4) == BIG_SHIP || (ship->type>>4) == CAR || (ship->type>>4) == SMALL_SHIP )
	{
		message_addscore( (details + (ship->type) )->score );
	}

}

control_registerdeath(ship);
}


void explode_static_graphic ( long x_grid , long y_grid , long style )
{
long loop ;
long size = 4 ;
long type ;
long x_pos , y_pos , z_pos ;
long bit_mask = 255 ;		// Set a big bunch of bits to enable all explosion for all groups
							// unless set otherwise by special group releasing explosions
long group_number = 0 ;
long explosion_velocity ;

graphics_details *details = (graphics_details*) static_graphics_adr ;
explosion_details *explosion_data ;
explosion_details_header *explosion_header ;

type = (long) poly_map [ y_grid ] [ x_grid ] ;

x_pos = (unsigned) (x_grid<<25) ;
y_pos = (unsigned) (y_grid<<25) ;

// Get height of this object - less than 17 = sea
z_pos = (long) ( ( (height_map [ (y_grid<<1) ] [ (x_grid<<1) ])-17 )<<21 ) ;
if (z_pos<0) z_pos = 0;

switch ( style )
{

case	REF_STATIC_EXPLODE_SMALL :
	size = 2 ;
	explosion_velocity = 9 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , SMALL_EXPLOSION_SOUND ) ;
break ;

case	REF_STATIC_EXPLODE_SMALL_MUSHROOM :
	size = 2 ;
	explosion_velocity = 11 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , MEDIUM_EXPLOSION_SOUND ) ;
	add_smoke (	x_pos , y_pos , z_pos ,
				0 , 0 , 0 ,
				SMALL_MUSHROOM_CLOUD ,
				0 ) ;
break ;

case	REF_STATIC_EXPLODE_SMALL_FAST :
	size = 2 ;
	explosion_velocity = 13 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , SMALL_EXPLOSION_SOUND ) ;
break ;

case	REF_STATIC_EXPLODE_SMALL_FAST_MUSHROOM :
	size = 2 ;
	explosion_velocity = 15 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , MEDIUM_EXPLOSION_SOUND ) ;
	add_smoke (	x_pos , y_pos , z_pos ,
				0 , 0 , 0 ,
				SMALL_MUSHROOM_CLOUD ,
				0 ) ;
break ;

case	REF_STATIC_EXPLODE_MEDIUM :
	size = 6 ;
	explosion_velocity = 17 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , MEDIUM_EXPLOSION_SOUND ) ;
break ;

case	REF_STATIC_EXPLODE_MEDIUM_MUSHROOM :
	size = 6 ;
	explosion_velocity = 19 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;
	add_smoke (	x_pos , y_pos , z_pos ,
				0 , 0 , 0 ,
				MEDIUM_MUSHROOM_CLOUD ,
				0 ) ;
break ;

case	REF_STATIC_EXPLODE_MEDIUM_FAST :
	size = 6 ;
	explosion_velocity = 21 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , MEDIUM_EXPLOSION_SOUND ) ;
break ;

case	REF_STATIC_EXPLODE_MEDIUM_FAST_MUSHROOM :
	size = 6 ;
	explosion_velocity = 23 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;
	add_smoke (	x_pos , y_pos , z_pos ,
				0 , 0 , 0 ,
				MEDIUM_MUSHROOM_CLOUD ,
				0 ) ;
break ;

case	REF_STATIC_EXPLODE_BIG :
	size = 8 ;
	explosion_velocity = 25 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;
break ;

case	REF_STATIC_EXPLODE_BIG_MUSHROOM :
	size = 8 ;
	explosion_velocity = 27 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , VERY_BIG_EXPLOSION_SOUND ) ;
	add_smoke (	x_pos , y_pos , z_pos ,
				0 , 0 , 0 ,
				BIG_MUSHROOM_CLOUD ,
				0 ) ;
break ;

case	REF_STATIC_EXPLODE_BIG_FAST :
	size = 8 ;
	explosion_velocity = 29 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;
break ;

case	REF_STATIC_EXPLODE_BIG_FAST_MUSHROOM :
	size = 8 ;
	explosion_velocity = 29 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , VERY_BIG_EXPLOSION_SOUND ) ;	
	add_smoke (	x_pos , y_pos , z_pos ,
				0 , 0 , 0 ,
				BIG_MUSHROOM_CLOUD ,
				0 ) ;
break ;

case	REF_STATIC_EXPLODE_MEGATOWER_2_GROUPS :
	size = 6 ;
	explosion_velocity = 21 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;
	bit_mask = GROUP_1 ;			// Don't plot groups with this bit set
	group_number = 0 ;		// Centre round this group
	add_explosion_section( 	x_pos , y_pos , z_pos ,
							0 , 0 , (1<<22) , type , bit_mask , group_number );
break ;

case	REF_STATIC_EXPLODE_TOWER_1_GROUP :
	size = 8 ;
	explosion_velocity = 21 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;
	bit_mask = GROUP_0 ;			// Don't plot groups with this bit set - explode these groups now
	group_number = 1 ;		// Centre round this group
	add_explosion_section( 	x_pos , y_pos , z_pos ,
							0 , 0 , (1<<22) , type , bit_mask , group_number );
break ;

case	REF_STATIC_EXPLODE_STAR_TOWER_4_GROUPS :
	size = 6 ;
	explosion_velocity = 21 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;
	bit_mask = GROUP_1 ;			// Don't plot groups with this bit set
	group_number = 0 ;		// Centre round this group
	add_explosion_section( 	x_pos , y_pos , z_pos ,
							0 , 0 , (1<<22) , type , bit_mask , group_number );
break ;

case	REF_STATIC_EXPLODE_PALACE_REL_3_GROUPS :
	size = 6 ;
	explosion_velocity = 21 + ((arm_random())&3) ;
	make_sound( x_pos , y_pos , z_pos , BIG_EXPLOSION_SOUND ) ;

	bit_mask = GROUP_0+GROUP_2+GROUP_3 ;			// Don't plot groups with this bit set
	group_number = 1 ;		// Centre round this group
	add_explosion_section( 	x_pos , y_pos , z_pos ,
							0 , 0 , (1<<22) , type , bit_mask , group_number );

	bit_mask = GROUP_0+GROUP_1+GROUP_3 ;			// Don't plot groups with this bit set
	group_number = 2 ;		// Centre round this group
	add_explosion_section( 	x_pos , y_pos , z_pos ,
							0 , 0 , (1<<22) , type , bit_mask , group_number );

	bit_mask = GROUP_0+GROUP_1+GROUP_2 ;			// Don't plot groups with this bit set
	group_number = 0 ;		// Centre round this group
	add_explosion_section( 	x_pos , y_pos , z_pos ,
							0 , 0 , (1<<22) , type , bit_mask , group_number );

// dont think so ??????????
	//bit_mask = GROUP_1+GROUP_2+GROUP_3 ;			// Don't EXPLODE groups with this bit set
	
	bit_mask = GROUP_0 ;// explode this n
	
break ;



}

explosion_header = (explosion_details_header*) (details+type)->explosion_adr ;

explosion_data = (explosion_details *) (explosion_header + 1) ;

explode_static_from_collision_box(	x_grid+(y_grid<<7) , explosion_velocity ,
									x_grid+(y_grid<<7) , EXPLOSION_STYLE_NORMAL , bit_mask ) ;


//for ( loop = 0 ; loop <= explosion_header->counter ; loop +=1 )
//{
//	// Only explode the required groups
//	if ( (bit_mask&( 1<<(explosion_data->bonus) )) != 0 )
//	{
//		// printf( "blowing up group %d\n",explosion_data->bonus );
//		add_explosion_line (	0 , 0 , 0 ,
//								x_pos + explosion_data->x_pos ,
//								y_pos + explosion_data->y_pos ,
//								z_pos + explosion_data->z_pos ,
//								x_pos + explosion_data->x_pos2 ,
//								y_pos + explosion_data->y_pos2 ,
//								z_pos + explosion_data->z_pos2 ,
//								explosion_data->colour1 ,
//								explosion_data->colour2 ,
//								explosion_velocity ,
//								size ,
//								x_grid + (y_grid<<7) ,
//								EXPLOSION_STYLE_NORMAL
//								);
//	}

//	// Next explosion line data
//	explosion_data += 1;
//}


if ( poly_map [ y_grid ] [ x_grid ] != OBJECT_BONUS_CRYSTAL_ADDER )
{
	// Remove the object off the poly map
	poly_map [ y_grid ] [ x_grid ] = OBJECT_NOTHING ;
	collision_map [ y_grid ] [ x_grid ] = 0 ;
}

// Tell tim wots happened n' inc. score
status_groundhit( (x_grid<<1) , (y_grid<<1) );
message_addscore( (details+type)->score );

}


void add_explosion_line (	long x_vel , long y_vel , long z_vel ,
							long start_x , long start_y , long start_z ,
							long end_x , long end_y , long end_z ,
							long colour1 , long colour2 ,
							long explosion_velocity ,
							long size ,
							long owner ,
							long style )
{
static long x_rot = 0 ;
static long y_rot = 0 ;

long x_exp , y_exp , z_exp ;
long bit_size ;
long line_size ;
long temp_long ;
long counter = 1 ;
long loop , loop_2 ;
long x_size ;
long y_size ;
long z_size ;

// Calc start pos offsets
x_exp = (cosine_table [ x_rot ] * sine_table [ y_rot ]) ;
y_exp = (sine_table [ x_rot ] * sine_table [ y_rot ]) ;
z_exp = sine_table [ y_rot ] << 12 ;


// Calc size of explosion line 
x_size = end_x - start_x ;
y_size = end_y - start_y ;
z_size = end_z - start_z ;

// Find max x,y,z size
if ( x_size<0 ) line_size = -x_size ; else line_size = x_size ;
if ( y_size<0 ) temp_long = -y_size ; else temp_long = y_size ;
if ( line_size<temp_long ) line_size = temp_long ;
if ( z_size<0 ) temp_long = -z_size ; else temp_long = z_size ;
if ( line_size<temp_long ) line_size = temp_long ;

// Use max size to find step rate of line

// Is it a small line
if ( line_size>(2*(1<<24)) )
{
	counter = 2 ;
	x_size = x_size >> 1 ;
	y_size = y_size >> 1 ;
	z_size = z_size >> 1 ;
}

// Is it a medium line
if ( line_size>(4*(1<<24)) )
{
	counter = 4 ;
	x_size = x_size >> 2 ;
	y_size = y_size >> 2 ;
	z_size = z_size >> 2 ;
}

// Is it a big line
if ( line_size>(8*(1<<24)) )
{
	counter = 8 ;
	x_size = x_size >> 3 ;
	y_size = y_size >> 3 ;
	z_size = z_size >> 3 ;
}

if (style == EXPLOSION_STYLE_LOADS_OF_BITS)
{
	counter = (counter<<1) ;
	x_size = (x_size<<1) ;
	y_size = (y_size<<1) ;
	z_size = (z_size<<1) ;
}


for (loop = 0 ; loop <=counter ; loop += 1)
{

	// Add random explosion bits
	if (	(loop&1) == 0 
			|| style == EXPLOSION_STYLE_MEGA_BOMB 
			|| style == EXPLOSION_STYLE_LOADS_OF_BITS )
	{
		bit_size = size + (arm_random()&3) ;
		
		//colour1 = 192+(arm_random()&3) ;
		colour2 = 194+(arm_random()&3) ;

		add_bit ( 	start_x+x_exp , start_y+y_exp , start_z+z_exp ,
					x_vel , y_vel , z_vel ,
					colour1 , colour2 ,
					bit_size , explosion_velocity , owner , style ) ;
	}

	// Add some explosion smoke around the centre
	for (loop_2 = 0 ; loop_2 < 4 ; loop_2 += 1)
	{
		x_exp = (cosine_table [ x_rot ] * sine_table [ y_rot ]) ;
		y_exp = (sine_table [ x_rot ] * sine_table [ y_rot ]) ;
		z_exp = sine_table [ y_rot ] << 12 ;

		if ( size == 0 )
		{
			add_smoke (	start_x+(x_exp>>2) , start_y+(y_exp>>2) , start_z+(z_exp>>2) ,
						(x_vel>>1)+(x_exp>>3) , (y_vel>>1)+(y_exp>>3) , (z_vel>>1)+(z_exp>>3) ,
						SMALL_EXPLOSION_SMOKE ,
						0 ) ;		
		}
		else
		{
			if ( bit_size >= 6 ) 
			{
				add_smoke (	start_x+(x_exp) , start_y+(y_exp) , start_z+(z_exp) ,
							(x_vel>>1)+(x_exp>>1) , (y_vel>>1)+(y_exp>>1) , (z_vel>>1)+(z_exp>>1) ,
							BIG_EXPLOSION_SMOKE ,
							0 ) ;
			}
			else
			{
				add_smoke (	start_x+(x_exp>>1) , start_y+(y_exp>>1) , start_z+(z_exp>>1) ,
							(x_vel>>1)+(x_exp>>2) , (y_vel>>1)+(y_exp>>2) , (z_vel>>1)+(z_exp>>2) ,
							EXPLOSION_SMOKE ,
							0 ) ;
			}
		}
		
		x_rot = (x_rot+64+ (arm_random()&63) )&1023 ;
		y_rot = (y_rot+128+ (arm_random()&127) )&1023 ;
	}

	start_x += x_size ;
	start_y += y_size ;
	start_z += z_size ;

}

}

void dent_ground( long x_pos , long y_pos , long damage )
{

// Makes dents in the ground to the depth of damage at given x,y

long new_height ;

armburn_addexplosion ( (unsigned) x_pos>>24 , (unsigned) y_pos>>24 , arm_random()&3 );

x_pos = (unsigned) (x_pos + (1<<23) )>>24 ;
y_pos = (unsigned) (y_pos + (1<<23) )>>24 ;

//printf( "X pos %d\n" , x_pos );
//printf( "Y pos %d\n" , y_pos );

// If there is a graphic at the change of height point then explode it
if ( poly_map [ y_pos>>1 ] [ x_pos>>1 ] != 0 
	&& height_map [ y_pos ] [ x_pos ] > 17 ) static_explode( (x_pos>>1) + ((y_pos>>1)<<7) , 255 ) ;

if ( height_map [ y_pos ] [ x_pos ] < 17 ) return ;

new_height = ( (long) height_map [ y_pos ] [ x_pos ] ) - damage ;

// Check min height
if (new_height < 17) new_height = 17 ;

height_map [ y_pos ] [ x_pos ] = (char) new_height ;

// Check rate of change of height in surrounding area

// check left
if ( height_map [ y_pos ] [ (x_pos-1)&255 ] - new_height > 16 )
{

dent_ground(	(long)  (( ( (unsigned) (x_pos-1)&255)<<24 ) -(1<<23)) ,
				(long)  ( ( (unsigned) y_pos<<24)-(1<<23) ) ,
				(long) (height_map [ y_pos ] [ (x_pos-1)&255 ] - new_height)-16 );
}

// check right
if ( height_map [ y_pos ] [ (x_pos+1)&255 ] - new_height > 16 )
{

dent_ground(	(long) (( (unsigned) ((x_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) ( ( (unsigned) y_pos<<24)-(1<<23) ) ,
				(long) (height_map [ y_pos ] [ (x_pos+1)&255 ] - new_height)-16 );
}

// check above
if ( height_map [ (y_pos-1)&255 ] [ x_pos ] - new_height > 16 )
{

dent_ground(	(long) (unsigned) ( (x_pos<<24)-(1<<23) ) ,
				(long) (unsigned) (( ((y_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (height_map [ (y_pos-1)&255 ] [ x_pos ] - new_height)-16 );
}

// check below
if ( height_map [ (y_pos+1)&255 ] [ x_pos ] - new_height > 16 )
{

dent_ground(	(long) (unsigned) ( (x_pos<<24)-(1<<23) ) ,
				(long) (unsigned) (( ((y_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (height_map [ (y_pos+1)&255 ] [ x_pos ] - new_height)-16 );
}

// check below left
if ( height_map [ (y_pos+1)&255 ] [ (x_pos-1)&255 ] - new_height > 16 )
{

dent_ground(	(long) (unsigned) (( ((x_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (height_map [ (y_pos+1)&255 ] [ (x_pos-1)&255 ] - new_height)-16 );
}

// check below right
if ( height_map [ (y_pos+1)&255 ] [ (x_pos+1)&255 ] - new_height > 16 )
{

dent_ground(	(long) (unsigned) (( ((x_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (height_map [ (y_pos+1)&255 ] [ (x_pos+1)&255 ] - new_height)-16 );
}

// check above left
if ( height_map [ (y_pos-1)&255 ] [ (x_pos-1)&255 ] - new_height > 16 )
{

dent_ground(	(long) (unsigned) (( ((x_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (height_map [ (y_pos-1)&255 ] [ (x_pos-1)&255 ] - new_height)-16 );
}

// check above right
if ( height_map [ (y_pos-1)&255 ] [ (x_pos+1)&255 ] - new_height > 16 )
{

dent_ground(	(long) (unsigned) (( ((x_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (height_map [ (y_pos-1)&255 ] [ (x_pos+1)&255 ] - new_height)-16 );
}

}


void undent_ground( long x_pos , long y_pos , long damage )
{

// Makes hills in the ground to the height of damage at given x,y

long new_height ;

//armburn_addexplosion ( (unsigned) x_pos>>24 , (unsigned) y_pos>>24 , arm_random()&3 );

x_pos = (unsigned) (x_pos + (1<<23) )>>24 ;
y_pos = (unsigned) (y_pos + (1<<23) )>>24 ;

//printf( "X pos %d\n" , x_pos );
//printf( "Y pos %d\n" , y_pos );

if ( poly_map [ y_pos>>1 ] [ x_pos>>1 ] != 0 ) static_explode( (x_pos>>1) + ((y_pos>>1)<<7) , 255 ) ;

new_height = ( (long) height_map [ y_pos ] [ x_pos ] ) + damage ;

// Check max height
if (new_height > 255) new_height = 255 ;

height_map [ y_pos ] [ x_pos ] = (char) new_height ;

// Check rate of change of height in surrounding area

// check left
if ( height_map [ y_pos ] [ (x_pos-1)&255 ] - new_height < -16 )
{

undent_ground(	(long)  (( ( (unsigned) (x_pos-1)&255)<<24 ) -(1<<23)) ,
				(long)  ( ( (unsigned) y_pos<<24)-(1<<23) ) ,
				(long) (new_height - height_map [ y_pos ] [ (x_pos-1)&255 ] )-16 );
}

// check right
if ( height_map [ y_pos ] [ (x_pos+1)&255 ] - new_height < -16 )
{

undent_ground(	(long) (( (unsigned) ((x_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) ( ( (unsigned) y_pos<<24)-(1<<23) ) ,
				(long) (new_height - height_map [ y_pos ] [ (x_pos+1)&255 ] )-16 );
}

// check above
if ( height_map [ (y_pos-1)&255 ] [ x_pos ] - new_height < -16 )
{

undent_ground(	(long) (unsigned) ( (x_pos<<24)-(1<<23) ) ,
				(long) (unsigned) (( ((y_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (new_height - height_map [ (y_pos-1)&255 ] [ x_pos ])-16 );
}

// check below
if ( height_map [ (y_pos+1)&255 ] [ x_pos ] - new_height < -16 )
{

undent_ground(	(long) (unsigned) ( (x_pos<<24)-(1<<23) ) ,
				(long) (unsigned) (( ((y_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (new_height - height_map [ (y_pos+1)&255 ] [ x_pos ])-16 );
}

// check below left
if ( height_map [ (y_pos+1)&255 ] [ (x_pos-1)&255 ] - new_height < -16 )
{

undent_ground(	(long) (unsigned) (( ((x_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (new_height - height_map [ (y_pos+1)&255 ] [ (x_pos-1)&255 ])-16 );
}

// check below right
if ( height_map [ (y_pos+1)&255 ] [ (x_pos+1)&255 ] - new_height < -16 )
{

undent_ground(	(long) (unsigned) (( ((x_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (new_height - height_map [ (y_pos+1)&255 ] [ (x_pos+1)&255 ])-16 );
}

// check above left
if ( height_map [ (y_pos-1)&255 ] [ (x_pos-1)&255 ] - new_height < -16 )
{

undent_ground(	(long) (unsigned) (( ((x_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (new_height - height_map [ (y_pos-1)&255 ] [ (x_pos-1)&255 ])-16 );
}

// check above right
if ( height_map [ (y_pos-1)&255 ] [ (x_pos+1)&255 ] - new_height < -16 )
{

undent_ground(	(long) (unsigned) (( ((x_pos+1)&255)<<24 ) -(1<<23)) ,
				(long) (unsigned) (( ((y_pos-1)&255)<<24 ) -(1<<23)) ,
				(long) (new_height - height_map [ (y_pos-1)&255 ] [ (x_pos+1)&255 ])-16 );
}

}

ship_stack* add_explosion_section(	long x_pos , long y_pos , long z_pos ,
									long x_vel , long y_vel , long z_vel ,
									long type , long group_mask , long centre_group )
{

// Adds bits of static objects into the ship stack for the explosions
// group mask - bits set mean don't plot this group

rotate_node node_data ;

ship_stack* section ;
collision_details_header* collision_header ;
graphics_details *details = (graphics_details*) static_graphics_adr ;

collision_header = (collision_details_header*) (details+type)->collision_adr ;

section = (ship_stack*) armlink_sorttolist( &ships , x_pos ) ;
if (section != NULL)
{

memset( (void*) ( ((char*) section) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

find_group_centre_point(	collision_header ,
							section ,
							centre_group ) ;

// Assume these are all 0 as it may be a static grqphic
// Otherwise let the routine that called me set it up
section->x_rot = 0 ;
section->y_rot = 0 ;
section->z_rot = 0 ;

// Rotate centre point offsets to find centre of rotaion
node_data.x_pos = -section->aim_goto_x ;
node_data.y_pos = -section->aim_goto_y ;
node_data.z_pos = -section->aim_goto_z ;
node_data.x_rot = section->x_rot ;
node_data.y_rot = section->y_rot ;
node_data.z_rot = section->z_rot ;
rotate_node_from_c( &node_data );

// Get centre point of rotation for this section
section->goto_x = x_pos + node_data.x_pos ;
section->goto_y = y_pos + node_data.y_pos ;
section->goto_z = z_pos + node_data.z_pos ;

// Get real centre point for this section
node_data.x_pos = section->aim_goto_x ;
node_data.y_pos = section->aim_goto_y ;
node_data.z_pos = section->aim_goto_z ;
node_data.x_rot = section->x_rot ;
node_data.y_rot = section->y_rot ;
node_data.z_rot = section->z_rot ;
rotate_node_from_c( &node_data );

section->x_pos = section->goto_x + node_data.x_pos ;
section->y_pos = section->goto_y + node_data.y_pos ;
section->z_pos = section->goto_z + node_data.z_pos ;


section->type = SECTION+type ;

// Base velocity in x,y with random +ve -ve adder
section->x_vel = x_vel + ( ( ((arm_random())&255) * (1<<14) ) - (1<<21) ) ;
section->y_vel = y_vel + ( ( ((arm_random())&255) * (1<<14) ) - (1<<21) ) ;

// Base z velocity + rand up movement
section->z_vel = z_vel + ( ((arm_random())&255) * (1<<13) ) ;

section->can_see = 0 ;

section->who_owns_me = (long) section ;

// Which poly groups should be plotted
section->group_mask = group_mask ;

section->misc_counter = centre_group ;

section->collision_size = -(1<<26) ;

section->x_control = 32*((arm_random()&1023)-512) ;
section->y_control = 32*((arm_random()&1023)-512) ;

section->shields = 40 ;

section->special_data = NULL ;
section->performance = NULL ;

}

return ( section ) ;

}


void explosion_section_control( ship_stack* section )
{

graphics_details *details = (graphics_details*) static_graphics_adr ;
collision_details_header* collision_header ;
collision_details* collision_data  ;
long loop , coll_check ;
rotate_node node_data ;
long x_size , y_size , z_size ;
section_groups*				section_group_data ;
section_details_header*		section_data_header ;
section_details*			section_data ;
long x_pos , y_pos , z_pos , x_pos2 , y_pos2 , z_pos2 , x_vel , y_vel , z_vel ;

// Is it a bit of a ship
if (section->type >= SHIP_SECTION)
{
	ship_explosion_section_control( section ) ;
	return ;
}

collision_header = (collision_details_header*) (details + (section->type-SECTION) )->collision_adr ;
collision_data = (collision_details*) (collision_header + 1) ;
section_group_data = (section_groups*) (details + (section->type-SECTION) )->extra_data ;


// Update the movement of this section
section->goto_x += section->x_vel ;
section->goto_y += section->y_vel ;
section->goto_z += section->z_vel ;

// Get real centre point for this section
node_data.x_pos = section->aim_goto_x ;
node_data.y_pos = section->aim_goto_y ;
node_data.z_pos = section->aim_goto_z ;
node_data.x_rot = section->x_rot ;
node_data.y_rot = section->y_rot ;
node_data.z_rot = section->z_rot ;
rotate_node_from_c( &node_data );

section->x_pos = section->goto_x + node_data.x_pos ;
section->y_pos = section->goto_y + node_data.y_pos ;
section->z_pos = section->goto_z + node_data.z_pos ;


// Gravity - but not in space
if ( which_graphics_set != SPACE_GRAPHICS )
{
	section->z_vel -= (1<<19) ;
}

// Friction
section->x_vel -= (section->x_vel>>6) ;
section->y_vel -= (section->y_vel>>6) ;
section->z_vel -= (section->z_vel>>6) ;

section->y_rot = ((section->y_rot+section->x_control)&ROT_LIMIT);
section->z_rot = ((section->z_rot+section->y_control)&ROT_LIMIT);

section->shields -= 1 ;

if ( section_group_data != NULL )
{
	// Section smoke
	switch( section->misc_counter )
	{
		case 0 :
		section_data_header = (section_details_header*) section_group_data->group_0 ;
		break ;
		case 1 :
		section_data_header = (section_details_header*) section_group_data->group_1 ;
		break ;
		case 2 :
		section_data_header = (section_details_header*) section_group_data->group_2 ;
		break ;
		case 3 :
		section_data_header = (section_details_header*) section_group_data->group_3 ;
		break ;
		case 4 :
		section_data_header = (section_details_header*) section_group_data->group_4 ;
		break ;
		case 5 :
		section_data_header = (section_details_header*) section_group_data->group_5 ;
		break ;
		case 6 :
		section_data_header = (section_details_header*) section_group_data->group_6 ;
		break ;
	}
	
	if ( section_data_header != NULL )
	{
		node_data.x_rot = section->x_rot ;
		node_data.y_rot = section->y_rot ;
		node_data.z_rot = section->z_rot ;
			
		section_data = (section_details*) (section_data_header+1) ;
		for (loop = 0 ; loop <= section_data_header->counter ; loop +=1)
		{
			node_data.x_pos = section_data->x_pos ;
			node_data.y_pos = section_data->y_pos ;
			node_data.z_pos = section_data->z_pos ;
			rotate_node_from_c( &node_data );
			x_pos = section->x_pos + node_data.x_pos ;
			y_pos = section->y_pos + node_data.y_pos ;
			z_pos = section->z_pos + node_data.z_pos ;
			
			node_data.x_pos = section_data->x_pos2 ;
			node_data.y_pos = section_data->y_pos2 ;
			node_data.z_pos = section_data->z_pos2 ;
			rotate_node_from_c( &node_data );
			x_pos2 = section->x_pos + node_data.x_pos ;
			y_pos2 = section->y_pos + node_data.y_pos ;
			z_pos2 = section->z_pos + node_data.z_pos ;
			
			node_data.x_pos = (section_data->x_vel>>2) ;
			node_data.y_pos = (section_data->y_vel>>2) ;
			node_data.z_pos = (section_data->z_vel>>2) ;
			rotate_node_from_c( &node_data );
			x_vel = section->x_vel - node_data.x_pos ;
			y_vel = section->y_vel - node_data.y_pos ;
			z_vel = section->z_vel - node_data.z_pos ;
			
			add_section_smoke(	x_pos , y_pos , z_pos ,
								x_pos2 , y_pos2 , z_pos2 ,
								x_vel , y_vel , z_vel , section_data->type ) ;
			
			section_data+=1;
		}
	}
}


// Check the collision
for (loop = 0 ; loop <= collision_header->counter ; loop += 1)
{

//printf("Section bm %d\n",section->counter);
//printf("Exp bm %d\n",explosion_data->bonus);


if ( (section->group_mask&( 1<<(collision_data->group) )) == 0)
{
	// printf("section coll check grop %d\n",explosion_data->bonus);
	node_data.x_pos = collision_data->x_pos ;
	node_data.y_pos = collision_data->y_pos ;
	node_data.z_pos = collision_data->z_pos ;
	node_data.x_rot = section->x_rot ;
	node_data.y_rot = section->y_rot ;
	node_data.z_rot = section->z_rot ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;
	if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
	node_data.x_pos = collision_data->x_pos2 ;
	node_data.y_pos = collision_data->y_pos ;
	node_data.z_pos = collision_data->z_pos ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;
	if (coll_check > 0) static_explode ( coll_check , 256 ) ;

	node_data.x_pos = collision_data->x_pos2 ;
	node_data.y_pos = collision_data->y_pos2 ;
	node_data.z_pos = collision_data->z_pos ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;
	if (coll_check > 0) static_explode ( coll_check , 256 ) ;

	node_data.x_pos = collision_data->x_pos ;
	node_data.y_pos = collision_data->y_pos2 ;
	node_data.z_pos = collision_data->z_pos ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;

	if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	node_data.x_pos = collision_data->x_pos ;
	node_data.y_pos = collision_data->y_pos ;
	node_data.z_pos = collision_data->z_pos2 ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;
	if (coll_check > 0) static_explode ( coll_check , 256 ) ;


	if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	node_data.x_pos = collision_data->x_pos2 ;
	node_data.y_pos = collision_data->y_pos ;
	node_data.z_pos = collision_data->z_pos2 ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;
	if (coll_check > 0) static_explode ( coll_check , 256 ) ;

	node_data.x_pos = collision_data->x_pos2 ;
	node_data.y_pos = collision_data->y_pos2 ;
	node_data.z_pos = collision_data->z_pos2 ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;
	if (coll_check > 0) static_explode ( coll_check , 256 ) ;

	node_data.x_pos = collision_data->x_pos ;
	node_data.y_pos = collision_data->y_pos2 ;
	node_data.z_pos = collision_data->z_pos2 ;
	rotate_node_from_c( &node_data );
	coll_check = check_collision(	section->x_pos + node_data.x_pos ,
									section->y_pos + node_data.y_pos ,
									section->z_pos + node_data.z_pos );
	if (coll_check != 0) section->shields = 0 ;
	if (coll_check > 0) static_explode ( coll_check , 256 ) ;

}

collision_data += 1 ;
}


}

ship_stack* add_ship_explosion_section(	ship_stack* ship ,
										long group_mask ,
										long centre_group )
{

// Adds bits of static objects into the ship stack for the explosions
// group mask - bits set mean don't plot this group

ship_stack* section ;
collision_details_header* collision_header ;
graphics_details *details = (graphics_details*) ships_adr ;
rotate_node node_data ;

collision_header = (collision_details_header*) (details+ship->type)->collision_adr ;

section = (ship_stack*) armlink_sorttolist( &ships , ship->x_pos ) ;
if (section != NULL)
{

memset( (void*) ( ((char*) section) + (sizeof(link_header)) ) , 0 , (sizeof(ship_stack))-(sizeof(link_header)) ) ; 

find_group_centre_point(	collision_header ,
							section ,
							centre_group ) ;

section->x_rot = ship->x_rot ;
section->y_rot = ship->y_rot ;
section->z_rot = ship->z_rot ;

// Rotate centre point offsets to find centre of rotaion
node_data.x_pos = -section->aim_goto_x ;
node_data.y_pos = -section->aim_goto_y ;
node_data.z_pos = -section->aim_goto_z ;
node_data.x_rot = section->x_rot ;
node_data.y_rot = section->y_rot ;
node_data.z_rot = section->z_rot ;
rotate_node_from_c( &node_data );

// Get centre point of rotation for this section
section->goto_x = ship->x_pos + node_data.x_pos ;
section->goto_y = ship->y_pos + node_data.y_pos ;
section->goto_z = ship->z_pos + node_data.z_pos ;

// Get real centre point for this section
node_data.x_pos = section->aim_goto_x ;
node_data.y_pos = section->aim_goto_y ;
node_data.z_pos = section->aim_goto_z ;
node_data.x_rot = section->x_rot ;
node_data.y_rot = section->y_rot ;
node_data.z_rot = section->z_rot ;
rotate_node_from_c( &node_data );

section->x_pos = section->goto_x + node_data.x_pos ;
section->y_pos = section->goto_y + node_data.y_pos ;
section->z_pos = section->goto_z + node_data.z_pos ;

section->type = SHIP_SECTION+ship->type ;

section->x_vel = ship->x_vel + ( ( ((arm_random())&255) * (1<<14) ) - (1<<21) ) ;
section->y_vel = ship->y_vel + ( ( ((arm_random())&255) * (1<<14) ) - (1<<21) ) ;
section->z_vel = ship->z_vel + ( ( ((arm_random())&255) * (1<<14) ) - (1<<21) ) ;

section->can_see = 0 ;

section->shields = 300 ;

section->who_owns_me = (long) section ;

section->misc_counter = centre_group ;

// Which poly groups should be plotted
section->group_mask = group_mask ;

section->collision_size = -(1<<26) ;

section->x_control = 32*((arm_random()&1023)-512) ;
section->y_control = 32*((arm_random()&1023)-512) ;

section->special_data = NULL ;
section->performance = NULL ;

}

return ( section ) ;

}


void ship_explosion_section_control( ship_stack* section )
{

graphics_details *details = (graphics_details*) ships_adr ;
collision_details_header* collision_header ;
collision_details* collision_data  ;
long loop , coll_check ;
rotate_node node_data ;
section_groups*				section_group_data ;
section_details_header*		section_data_header ;
section_details*			section_data ;
long x_pos , y_pos , z_pos , x_pos2 , y_pos2 , z_pos2 , x_vel , y_vel , z_vel ;

collision_header = (collision_details_header*) (details + (section->type-SHIP_SECTION) )->collision_adr ;
collision_data = (collision_details*) (collision_header + 1) ;
section_group_data = (section_groups*) (details + (section->type-SHIP_SECTION) )->extra_data ;

// Update the movement of this section
section->goto_x += section->x_vel ;
section->goto_y += section->y_vel ;
section->goto_z += section->z_vel ;

// Get real centre point for this section
node_data.x_pos = section->aim_goto_x ;
node_data.y_pos = section->aim_goto_y ;
node_data.z_pos = section->aim_goto_z ;
node_data.x_rot = section->x_rot ;
node_data.y_rot = section->y_rot ;
node_data.z_rot = section->z_rot ;
rotate_node_from_c( &node_data );

section->x_pos = section->goto_x + node_data.x_pos ;
section->y_pos = section->goto_y + node_data.y_pos ;
section->z_pos = section->goto_z + node_data.z_pos ;



// Gravity - but not in space
if ( which_graphics_set != SPACE_GRAPHICS )
{
	section->z_vel -= (1<<17) ;
}

// Friction
section->x_vel -= (section->x_vel>>6) ;
section->y_vel -= (section->y_vel>>6) ;
section->z_vel -= (section->z_vel>>6) ;

section->y_rot = ((section->y_rot+section->x_control)&ROT_LIMIT);
section->z_rot = ((section->z_rot+section->y_control)&ROT_LIMIT);

section->shields -= 1 ;

if ( section_group_data != NULL )
{
	// Section smoke
	switch( section->misc_counter )
	{
		case 0 :
		section_data_header = (section_details_header*) section_group_data->group_0 ;
		break ;
		case 1 :
		section_data_header = (section_details_header*) section_group_data->group_1 ;
		break ;
		case 2 :
		section_data_header = (section_details_header*) section_group_data->group_2 ;
		break ;
		case 3 :
		section_data_header = (section_details_header*) section_group_data->group_3 ;
		break ;
		case 4 :
		section_data_header = (section_details_header*) section_group_data->group_4 ;
		break ;
		case 5 :
		section_data_header = (section_details_header*) section_group_data->group_5 ;
		break ;
		case 6 :
		section_data_header = (section_details_header*) section_group_data->group_6 ;
		break ;
	}
	
	if ( section_data_header != NULL)
	{
		
		node_data.x_rot = section->x_rot ;
		node_data.y_rot = section->y_rot ;
		node_data.z_rot = section->z_rot ;
		
		section_data = (section_details*) (section_data_header+1) ;
		for (loop = 0 ; loop <= section_data_header->counter ; loop +=1)
		{
			node_data.x_pos = section_data->x_pos ;
			node_data.y_pos = section_data->y_pos ;
			node_data.z_pos = section_data->z_pos ;
			rotate_node_from_c( &node_data );
			x_pos = section->x_pos + node_data.x_pos ;
			y_pos = section->y_pos + node_data.y_pos ;
			z_pos = section->z_pos + node_data.z_pos ;
			
			node_data.x_pos = section_data->x_pos2 ;
			node_data.y_pos = section_data->y_pos2 ;
			node_data.z_pos = section_data->z_pos2 ;
			rotate_node_from_c( &node_data );
			x_pos2 = section->x_pos + node_data.x_pos ;
			y_pos2 = section->y_pos + node_data.y_pos ;
			z_pos2 = section->z_pos + node_data.z_pos ;
			
			node_data.x_pos = (section_data->x_vel>>2) ;
			node_data.y_pos = (section_data->y_vel>>2) ;
			node_data.z_pos = (section_data->z_vel>>2) ;
			rotate_node_from_c( &node_data );
			x_vel = section->x_vel - node_data.x_pos ;
			y_vel = section->y_vel - node_data.y_pos ;
			z_vel = section->z_vel - node_data.z_pos ;
			
			add_section_smoke(	x_pos , y_pos , z_pos ,
								x_pos2 , y_pos2 , z_pos2 ,
								x_vel , y_vel , z_vel , section_data->type ) ;
			
			section_data+=1;
		}
	}
}


// Check the collision
for (loop = 0 ; loop <= collision_header->counter ; loop += 1)
{

//printf("Section bm %d\n",section->counter);
//printf("Exp bm %d\n",explosion_data->bonus);

	if ( (section->group_mask&( 1<<(collision_data->group) )) == 0 || ((section->type-SHIP_SECTION)>>4) != BIG_SHIP )
	{
		// printf("section coll check grop %d\n",explosion_data->bonus);
		node_data.x_pos = collision_data->x_pos ;
		node_data.y_pos = collision_data->y_pos ;
		node_data.z_pos = collision_data->z_pos ;
		node_data.x_rot = section->x_rot ;
		node_data.y_rot = section->y_rot ;
		node_data.z_rot = section->z_rot ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields = 0 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
		
		node_data.x_pos = collision_data->x_pos2 ;
		node_data.y_pos = collision_data->y_pos ;
		node_data.z_pos = collision_data->z_pos ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields -= 50 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
		node_data.x_pos = collision_data->x_pos2 ;
		node_data.y_pos = collision_data->y_pos2 ;
		node_data.z_pos = collision_data->z_pos ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields -= 50 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
		node_data.x_pos = collision_data->x_pos ;
		node_data.y_pos = collision_data->y_pos2 ;
		node_data.z_pos = collision_data->z_pos ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields -= 50 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
	
		node_data.x_pos = collision_data->x_pos ;
		node_data.y_pos = collision_data->y_pos ;
		node_data.z_pos = collision_data->z_pos2 ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields -= 50 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
		node_data.x_pos = collision_data->x_pos2 ;
		node_data.y_pos = collision_data->y_pos ;
		node_data.z_pos = collision_data->z_pos2 ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields -= 50 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
		node_data.x_pos = collision_data->x_pos2 ;
		node_data.y_pos = collision_data->y_pos2 ;
		node_data.z_pos = collision_data->z_pos2 ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields -= 50 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
		node_data.x_pos = collision_data->x_pos ;
		node_data.y_pos = collision_data->y_pos2 ;
		node_data.z_pos = collision_data->z_pos2 ;
		rotate_node_from_c( &node_data );
		coll_check = check_collision(	section->x_pos + node_data.x_pos ,
										section->y_pos + node_data.y_pos ,
										section->z_pos + node_data.z_pos );
		if (coll_check != 0) section->shields -= 50 ;
		if (coll_check > 0) static_explode ( coll_check , 256 ) ;
	
	}
	
	collision_data += 1 ;
}


}




void find_group_centre_point(	collision_details_header* coll_header ,
								ship_stack* section ,
								long which_group )
{
collision_details* coll_data = (collision_details*) (coll_header + 1) ;
long loop ;
long x_pos = 0 , y_pos = 0 , z_pos = 0 ;
long counter = 0 ;


//printf("Searching for group %d\n",which_group);

for (loop = 0 ; loop <= coll_header->counter ; loop += 1)
{

//printf("exp ref %d\n",explosion_data->bonus);
//printf("exp adr %d\n",explosion_data);
	if (coll_data->group == which_group)
	{
		//printf("Found a exp group\n");
		counter += 2 ;
		
		x_pos += (coll_data->x_pos + coll_data->x_pos2) ;
		y_pos += (coll_data->y_pos + coll_data->y_pos2) ;
		z_pos += (coll_data->z_pos + coll_data->z_pos2) ;
	//	printf("Found one\n");
	}

coll_data += 1 ;

}

//printf("End of search\n");

if ( counter == 0)
{
	//printf("No group found\n");
	section->aim_goto_x = 0 ;
	section->aim_goto_y = 0 ;
	section->aim_goto_z = 0 ;
}
else
{
	section->aim_goto_x = -(x_pos / counter) ;
	section->aim_goto_y = -(y_pos / counter) ;
	section->aim_goto_z = -(z_pos / counter) ;
}


//printf("x %d\n",section->aim_goto_x);
//printf("y %d\n",section->aim_goto_y);
//printf("z %d\n",section->aim_goto_z);


}


void add_section_smoke(	long x_pos , long y_pos , long z_pos ,
						long x_pos2 , long y_pos2 , long z_pos2 ,
						long x_vel , long y_vel , long z_vel , long type )
{

long x , y , z ;
long r ;

x = x_pos2 - x_pos ;
y = y_pos2 - y_pos ;
z = z_pos2 - z_pos ;

r = (arm_random()&31) ;

x_pos = ((x_pos + ((x*r)>>5) ));
y_pos = ((y_pos + ((y*r)>>5) ));
z_pos = ((z_pos + ((z*r)>>5) ));

add_smoke (	x_pos , y_pos , z_pos ,
			x_vel , y_vel , z_vel ,
			SECTION_SMOKE + type , // BIG_EXPLOSION_SMOKE ,
			0 ) ;

//add_smoke (	x_pos2 , y_pos2 , z_pos2 ,
//			x_vel , y_vel , z_vel ,
//			SECTION_SMOKE + type , // BIG_EXPLOSION_SMOKE ,
//			0 ) ;

}


void explode_static_from_collision_box( long grid_ref , long  explosion_velocity , long owner , long style , long bit_mask )
{

long x_pos , y_pos , z_pos , type , loop ;
collision_details_header* coll_data_header ;
collision_details* coll_data ;
explosion_details_header* explosion_header ;
explosion_details* explosion_data ;
graphics_details *details = (graphics_details*) static_graphics_adr ;
long x_size , y_size , z_size , size = 0 ;
long bit_size , line_size ;
long temp_long ;
long too_many ;

long x_start , y_start , z_start ;
long x_pos_exp , y_pos_exp , z_pos_exp ;
long x_counter , y_counter , z_counter ;
long x_vel , y_vel , z_vel ;
long x_loop , y_loop , z_loop ;

long x_exp , y_exp , z_exp ;
long x_rot , y_rot , loop_2 ;

x_rot = (arm_random()&1023) ;
y_rot = (arm_random()&1023) ;

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

  if ( (bit_mask&( 1<<(coll_data->group) )) != 0 )
  {

	x_size = coll_data->x_pos2 - coll_data->x_pos ;
	y_size = coll_data->y_pos2 - coll_data->y_pos ;	
	z_size = coll_data->z_pos2 - coll_data->z_pos ;	
	
	if ( x_size > 0 ) size = x_size ; else size = -x_size ;
	if ( y_size > 0 ) size += y_size ; else size -= y_size ;	
	if ( z_size > 0 ) size += z_size ; else size -= z_size ;	

	bit_size = 12 ;
	line_size = ((8<<24)/9) ;


	if ( size < (8<<24) )
	{
		bit_size = 8 ;
		line_size = ((6<<24)/9) ;
	}

	if ( size < (4<<24) )
	{
		bit_size = 4 ;
		line_size = ((4<<24)/9) ;
	}

	if ( size < (2<<24) )
	{
		bit_size = 0 ;
		line_size = ((3<<24)/9) ;
	}

	
	x_counter = x_size/line_size ;
	if (x_counter<0) x_counter = -x_counter ;
	if (x_counter==0) x_counter = 1 ;	
	x_start = coll_data->x_pos + ((x_size/x_counter)>>1) ;
	x_vel = (x_size/x_counter) ;
	
	y_counter = y_size/line_size ;	
	if (y_counter<0) y_counter = -y_counter ;
	if (y_counter==0) y_counter = 1 ;	
	y_start = coll_data->y_pos + ((y_size/y_counter)>>1) ;
	y_vel = (y_size/y_counter) ;

	z_counter = z_size/line_size ;	
	if (z_counter<0) z_counter = -z_counter ;
	if (z_counter==0) z_counter = 1 ;	
	z_start = coll_data->z_pos + ((z_size/z_counter)>>1) ;
	z_vel = (z_size/z_counter) ;
	
	too_many = (z_counter*x_counter*y_counter)+(coll_data_header->counter<<2) ;
	
	if ( too_many > 48 ) too_many = 48 ;
	
	x_pos_exp = x_start ;
	for (x_loop = 1 ; x_loop <= x_counter ; x_loop += 1 )
	{
		y_pos_exp = y_start ;
		for (y_loop = 1 ; y_loop <= y_counter ; y_loop += 1 )
		{
			z_pos_exp = z_start ;
			
			for (z_loop = 1 ; z_loop <= z_counter ; z_loop += 1 )
			{
				if ( (arm_random()&63) > too_many )
				{
					add_bit ( 	x_pos + x_pos_exp , y_pos + y_pos_exp , z_pos + z_pos_exp ,
								(x_pos_exp>>4) , (y_pos_exp>>4) , (z_pos_exp>>4) ,  //x_vel , y_vel , z_vel ,
								coll_data->colour , 0 , //  colour1 , colour2 ,
								bit_size + (arm_random()&3) , (explosion_velocity>>1) , owner , style ) ;
				}
				
				if ( (arm_random()&63) > too_many )
				{
					// Add some explosion smoke around the centre
					for (loop_2 = 0 ; loop_2 < 2 ; loop_2 += 1)
					{
						x_exp = (cosine_table [ x_rot ] * sine_table [ y_rot ]) ;
						y_exp = (sine_table [ x_rot ] * sine_table [ y_rot ]) ;
						z_exp = sine_table [ y_rot ] << 12 ;
	
						if ( size == 0 )
						{
							add_smoke (	x_pos + x_pos_exp+(x_exp>>2) , y_pos + y_pos_exp+(y_exp>>2) , z_pos + z_pos_exp+(z_exp>>2) ,
										(x_pos_exp>>3)+(x_exp>>3) , (y_pos_exp>>3)+(y_exp>>3) , (z_pos_exp>>3)+(z_exp>>3) ,
										SMALL_EXPLOSION_SMOKE ,
										0 ) ;		
						}
						else
						{
							if ( bit_size >= 6 ) 
							{
								add_smoke (	x_pos + x_pos_exp+(x_exp) , y_pos + y_pos_exp+(y_exp) , z_pos + z_pos_exp+(z_exp) ,
											(x_pos_exp>>3)+(x_exp>>1) , (y_pos_exp>>3)+(y_exp>>1) , (z_pos_exp>>3)+(z_exp>>1) ,
											BIG_EXPLOSION_SMOKE ,
											0 ) ;
							}
							else
							{
								add_smoke (	x_pos + x_pos_exp+(x_exp>>1) , y_pos + y_pos_exp+(y_exp>>1) , z_pos + z_pos_exp+(z_exp>>1) ,
											(x_pos_exp>>3)+(x_exp>>2) , (y_pos_exp>>3)+(y_exp>>2) , (z_pos_exp>>3)+(z_exp>>2) ,
											EXPLOSION_SMOKE ,
											0 ) ;
							}
						}
			
						x_rot = (x_rot+64+ (arm_random()&63) )&1023 ;	
						y_rot = (y_rot+128+ (arm_random()&127) )&1023 ;
					}
				}
				z_pos_exp += z_vel ;
			}
			
			y_pos_exp += y_vel ;
		}	
	
		x_pos_exp += z_vel ;
	}
  }
coll_data += 1 ;
}

// Add bonus crystals
explosion_header = (explosion_details_header*) (details+type)->explosion_adr ;
explosion_data = (explosion_details *) (explosion_header + 1) ;

for ( loop = 0 ; loop <= explosion_header->counter ; loop +=1 )
{
	if ( explosion_data->colour2 == 8 )
	{
		if ( (arm_random()&7) == 0 )
		{
			add_bonus ( x_pos+((explosion_data->x_pos+explosion_data->x_pos)>>1) ,
						y_pos+((explosion_data->y_pos+explosion_data->y_pos)>>1) , 
						z_pos+((explosion_data->z_pos+explosion_data->z_pos)>>1) ,
						((explosion_data->x_pos+explosion_data->x_pos)>>5) ,
						((explosion_data->y_pos+explosion_data->y_pos)>>5) ,
						((explosion_data->z_pos+explosion_data->z_pos)>>7) ,
						(arm_random()&7)
						) ;
		}
	}
	else
	{	
		add_bonus ( x_pos+((explosion_data->x_pos+explosion_data->x_pos)>>1) ,
					y_pos+((explosion_data->y_pos+explosion_data->y_pos)>>1) , 
					z_pos+((explosion_data->z_pos+explosion_data->z_pos)>>1) ,
					((explosion_data->x_pos+explosion_data->x_pos)>>5) ,
					((explosion_data->y_pos+explosion_data->y_pos)>>5) ,
					((explosion_data->z_pos+explosion_data->z_pos)>>7) ,
					((explosion_data->colour2)&7)
					) ;
	}
	explosion_data += 1 ;
}

}



void explode_ship_from_collision_box( ship_stack* ship , long  explosion_velocity , long owner , long style , long bit_mask , long bonus_counter )
{

long x_pos , y_pos , z_pos , type , loop ;
collision_details_header* coll_data_header ;
collision_details* coll_data ;
explosion_details_header* explosion_header ;
explosion_details* explosion_data ;
graphics_details *details = (graphics_details*) ships_adr ;
long x_size , y_size , z_size , size = 0 ;
long bit_size , line_size ;
long temp_long ;
long too_many ;
rotate_node node_data ;

long x_start , y_start , z_start ;
long x_pos_exp , y_pos_exp , z_pos_exp ;
long x_pos_rot_exp , y_pos_rot_exp , z_pos_rot_exp ;
long x_counter , y_counter , z_counter ;
long x_vel , y_vel , z_vel ;
long x_loop , y_loop , z_loop ;

long x_exp , y_exp , z_exp ;
long x_rot , y_rot , loop_2 ;
long section_randy_crystals = 0 ;

x_rot = (arm_random()&1023) ;
y_rot = (arm_random()&1023) ;

type = ship->type ;

if ( type >= SHIP_SECTION )
{
	details = (graphics_details*) ships_adr ;
	type = type - SHIP_SECTION ;
	section_randy_crystals = 1 ;
}

if ( type >= SECTION )
{
	details = (graphics_details*) static_graphics_adr ;
	type = type - SECTION ;
	section_randy_crystals = 1 ;
}


x_pos = ship->x_pos ;
y_pos = ship->y_pos ;
z_pos = ship->z_pos ;

coll_data_header = (collision_details_header*) (details+type)->collision_adr ;
coll_data = (collision_details *) (coll_data_header + 1) ;

//printf("Exploding ship with - counter = %d\n",coll_data_header->counter);

for ( loop = 0 ; loop <= coll_data_header->counter ; loop +=1 )
{

  if ( (bit_mask&( 1<<(coll_data->group) )) != 0 )
  {

//printf("Found ship expl group %d\n",coll_data->group);

	x_size = coll_data->x_pos2 - coll_data->x_pos ;
	y_size = coll_data->y_pos2 - coll_data->y_pos ;	
	z_size = coll_data->z_pos2 - coll_data->z_pos ;	
	
	if ( x_size > 0 ) size = x_size ; else size = -x_size ;
	if ( y_size > 0 ) size += y_size ; else size -= y_size ;	
	if ( z_size > 0 ) size += z_size ; else size -= z_size ;	

	bit_size = 12 ;
	line_size = ((8<<24)/9) ;


	if ( size < (8<<24) )
	{
		bit_size = 8 ;
		line_size = ((6<<24)/9) ;
	}

	if ( size < (4<<24) )
	{
		bit_size = 4 ;
		line_size = ((4<<24)/9) ;
	}

	if ( size < (2<<24) )
	{
		bit_size = 0 ;
		line_size = ((2<<24)/9) ; // was - ((3<<24)/9) ;
	}

	
	x_counter = x_size/line_size ;
	if (x_counter<0) x_counter = -x_counter ;
	if (x_counter==0) x_counter = 1 ;	
	x_start = coll_data->x_pos + ((x_size/x_counter)>>1) ;
	x_vel = (x_size/x_counter) ;
	
	y_counter = y_size/line_size ;	
	if (y_counter<0) y_counter = -y_counter ;
	if (y_counter==0) y_counter = 1 ;	
	y_start = coll_data->y_pos + ((y_size/y_counter)>>1) ;
	y_vel = (y_size/y_counter) ;

	z_counter = z_size/line_size ;	
	if (z_counter<0) z_counter = -z_counter ;
	if (z_counter==0) z_counter = 1 ;	
	z_start = coll_data->z_pos + ((z_size/z_counter)>>1) ;
	z_vel = (z_size/z_counter) ;
	
	too_many = (z_counter*x_counter*y_counter); //+(coll_data_header->counter<<2) ;

	if ( too_many > 48 ) too_many = 48 ;
	
	node_data.x_rot = ship->x_rot ;
	node_data.y_rot = ship->y_rot ;	
	node_data.z_rot = ship->z_rot ;
	
	x_pos_exp = x_start ;
	for (x_loop = 1 ; x_loop <= x_counter ; x_loop += 1 )
	{
		y_pos_exp = y_start ;
		for (y_loop = 1 ; y_loop <= y_counter ; y_loop += 1 )
		{
			z_pos_exp = z_start ;
			
			for (z_loop = 1 ; z_loop <= z_counter ; z_loop += 1 )
			{
				if ( (arm_random()&63) > too_many )
				{
					node_data.x_pos = x_pos_exp ;
					node_data.y_pos = y_pos_exp ;
					node_data.z_pos = z_pos_exp ;
					rotate_node_from_c( &node_data ) ;
					
					add_bit ( 	x_pos + node_data.x_pos ,
								y_pos + node_data.y_pos ,
								z_pos + node_data.z_pos ,
								ship->x_vel + (node_data.x_pos>>4) ,
								ship->y_vel + (node_data.y_pos>>4) ,
								ship->z_vel + (node_data.z_pos>>4) ,
								coll_data->colour , 0 , //  colour1 , colour2 ,
								bit_size + (arm_random()&3) , (explosion_velocity>>1) , owner , style ) ;
				}

				if ( section_randy_crystals == 1 && (arm_random()&15) == 0 )
				{
					add_bonus ( x_pos + node_data.x_pos ,
								y_pos + node_data.y_pos , 
								z_pos + node_data.z_pos ,
								ship->x_vel , ship->y_vel , ship->z_vel + (1<<22) ,
								(arm_random()&7)
								) ;
				}
				
				if ( (arm_random()&63) > too_many )
				{
					// Add some explosion smoke around the centre
					for (loop_2 = 0 ; loop_2 < 2 ; loop_2 += 1)
					{
						x_exp = (cosine_table [ x_rot ] * sine_table [ y_rot ]) ;
						y_exp = (sine_table [ x_rot ] * sine_table [ y_rot ]) ;
						z_exp = sine_table [ y_rot ] << 12 ;
	
						if ( size == 0 )
						{
							add_smoke (	x_pos + node_data.x_pos + (x_exp>>2) ,
										y_pos + node_data.y_pos + (y_exp>>2) ,
										z_pos + node_data.z_pos + (z_exp>>2) ,
										ship->x_vel + (node_data.x_pos>>3) + (x_exp>>3) ,
										ship->y_vel + (node_data.y_pos>>3) + (y_exp>>3) ,
										ship->z_vel + (node_data.z_pos>>3) + (z_exp>>3) ,
										SMALL_EXPLOSION_SMOKE ,
										0 ) ;
						}
						else
						{
							if ( bit_size >= 6 ) 
							{
								add_smoke (	x_pos + node_data.x_pos + x_exp ,
											y_pos + node_data.y_pos + y_exp ,
											z_pos + node_data.z_pos + z_exp ,
											ship->x_vel + (node_data.x_pos>>3) + (x_exp>>1) ,
											ship->y_vel + (node_data.y_pos>>3) + (y_exp>>1) ,
											ship->z_vel + (node_data.z_pos>>3) + (z_exp>>1) ,
											BIG_EXPLOSION_SMOKE ,
											0 ) ;
							}
							else
							{
								add_smoke (	x_pos + node_data.x_pos + (x_exp>>1) ,
											y_pos + node_data.y_pos + (y_exp>>1) ,
											z_pos + node_data.z_pos + (z_exp>>1) ,
											ship->x_vel + (node_data.x_pos>>3) + (x_exp>>2) ,
											ship->y_vel + (node_data.y_pos>>3) + (y_exp>>2) ,
											ship->z_vel + (node_data.z_pos>>3) + (z_exp>>2) ,
											BIG_EXPLOSION_SMOKE ,
											0 ) ;
							}
						}
			
						x_rot = (x_rot+64+ (arm_random()&63) )&1023 ;	
						y_rot = (y_rot+128+ (arm_random()&127) )&1023 ;
					}
				}
				z_pos_exp += z_vel ;
			}
			y_pos_exp += y_vel ;
		}	
		x_pos_exp += z_vel ;
	}
  }
coll_data += 1 ;
}

if (section_randy_crystals == 0)
{
	// Add bonus crystals
	explosion_header = (explosion_details_header*) (details+type)->explosion_adr ;
	explosion_data = (explosion_details *) (explosion_header + 1) ;
	
	if ( explosion_header->counter >= 0 && bonus_counter == 0 && (ship->type>>4) != WEAPON )
	{
		for ( loop = 0 ; loop <= explosion_header->counter ; loop +=1 )
		{
			node_data.x_pos = ((explosion_data->x_pos+explosion_data->x_pos)>>1) ;
			node_data.y_pos = ((explosion_data->y_pos+explosion_data->y_pos)>>1) ;
			node_data.z_pos = ((explosion_data->z_pos+explosion_data->z_pos)>>1) ;
		
			if ( explosion_data->colour2 == 8 )
			{
				if ( (arm_random()&7) == 0 )
				{
					if ( (ship->type>>4) == BIG_SHIP )
					{
						add_bonus ( x_pos + node_data.x_pos ,
									y_pos + node_data.y_pos , 
									z_pos + node_data.z_pos ,
									ship->x_vel + (node_data.x_pos>>3) ,
									ship->y_vel ,//+ (node_data.y_pos>>5) ,
									ship->z_vel + (node_data.z_pos>>3) ,
									(arm_random()&7)
									) ;
					}
					else
					{
						add_bonus ( x_pos + (node_data.x_pos<<1) ,
									y_pos + (node_data.y_pos<<1) , 
									z_pos + (node_data.z_pos<<1) ,
									ship->x_vel + node_data.x_pos ,
									ship->y_vel ,//+ node_data.y_pos ,
									ship->z_vel + node_data.z_pos ,
									(arm_random()&7)
									) ;
					}				
				}
			}
			else
			{	
				if ( (ship->type>>4) == BIG_SHIP )
				{
					add_bonus ( x_pos + node_data.x_pos ,
								y_pos + node_data.y_pos , 
								z_pos + node_data.z_pos ,
								ship->x_vel + (node_data.x_pos>>3) ,
								ship->y_vel ,//+ (node_data.y_pos>>5) ,
								ship->z_vel + (node_data.z_pos>>3) ,
								((explosion_data->colour2)&7)
								) ;
				}
				else
				{
					add_bonus ( x_pos + (node_data.x_pos<<1) ,
								y_pos + (node_data.y_pos<<1) , 
								z_pos + (node_data.z_pos<<1) ,
								ship->x_vel + node_data.x_pos ,
								ship->y_vel ,//+ node_data.y_pos ,
								ship->z_vel + node_data.z_pos ,
								((explosion_data->colour2)&7)
								) ;
				}
			}
			explosion_data += 1 ;
		}
	}
}

}










