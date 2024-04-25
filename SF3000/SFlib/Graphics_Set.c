#include "Explosion.h"
#include "Bonus_Control.h"
#include "Graphics_Set.h"
#include "SF_Bonus.h"


long test_display_static_graphics( long test_graphic )
{

switch ( which_graphics_set )
{
	case	EARTH_LAND_BASED_GRAPHICS :
		test_graphic = test_display_earth_land_graphics( test_graphic ) ;
	break ;

	case	EARTH_SEA_BASED_GRAPHICS :
		test_graphic = test_display_earth_sea_graphics( test_graphic ) ;
	break ;

	case	CHEMICAL_GRAPHICS :
		test_graphic = test_display_chemical_graphics( test_graphic ) ;
	break ;

	case	THORG_GRAPHICS :
		test_graphic = test_display_thorg_graphics( test_graphic ) ;
	break ;

	case	SPACE_GRAPHICS :
		test_graphic = test_display_space_graphics( test_graphic ) ;
	break ;

	case	DEATH_STAR_GRAPHICS	:
		test_graphic = test_display_death_star_graphics( test_graphic ) ;
	break ;
}

return( test_graphic );

}

// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################

void explosion_check_this_graphic( long x_grid , long y_grid )
{

switch ( which_graphics_set )
{
	case	EARTH_LAND_BASED_GRAPHICS :
		explosion_check_earth_land_graphics( x_grid , y_grid ) ;
	break ;

	case	EARTH_SEA_BASED_GRAPHICS :
		explosion_check_earth_sea_graphics( x_grid , y_grid ) ;
	break ;

	case	CHEMICAL_GRAPHICS :
		explosion_check_chemical_graphics( x_grid , y_grid ) ;
	break ;

	case	THORG_GRAPHICS :
		explosion_check_thorg_graphics( x_grid , y_grid ) ;
	break ;

	case	SPACE_GRAPHICS :
		explosion_check_space_graphics( x_grid , y_grid ) ;
	break ;

	case	DEATH_STAR_GRAPHICS	:
		explosion_check_death_star_graphics( x_grid , y_grid ) ;
	break ;
}

}

// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################

// EARTH LAND BASED

void explosion_check_earth_land_graphics( long x_grid , long y_grid )
{

// Check object below
explosion_next_frame_earth_land_graphics( x_grid , ((y_grid-1)&127) ) ;

// Check object above
explosion_next_frame_earth_land_graphics( x_grid , ((y_grid+1)&127) ) ;

// Check object to left
explosion_next_frame_earth_land_graphics( ((x_grid-1)&127) , y_grid ) ;

// Check object to right
explosion_next_frame_earth_land_graphics( ((x_grid+1)&127) , y_grid ) ;

// What type of explosion is this object
switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	EL_GUN1 :
	EARTH_LAND_HIDDEN_BONUS
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;
	
case	EL_GUN2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_MUSHROOM
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;

case	EL_SUB :
	STATIC_CRYSTAL_CHANCE_GOOD
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
break ;

case	EL_SAM_BASE1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;
	
case	EL_SAM_BASE2 :
	STATIC_CRYSTAL_CHANCE_GOOD
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;
	
case	EL_SAM_BASE3 :
	STATIC_CRYSTAL_CHANCE_GOOD
	STATIC_EXPLODE_MEDIUM_FAST_MUSHROOM
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;

case	EL_HANGER :
	STATIC_CRYSTAL_CHANCE_VERY_VERY_GOOD
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			EL_DESTROYED_LONG ;
break ;

case	EL_TALL_HANG :
	STATIC_CRYSTAL_CHANCE_VERY_VERY_GOOD
	STATIC_EXPLODE_BIG_FAST_MUSHROOM

	//STATIC_EXPLODE_MEGATOWER_2_GROUPS
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;

case	EL_ARCH_TOWER :
	EARTH_LAND_HIDDEN_BONUS
	STATIC_CRYSTAL_CHANCE_GOOD
	STATIC_EXPLODE_BIG
break ;
		
case	EL_B_OFFICE :
	STATIC_CRYSTAL_CHANCE_VERY_GOOD
	STATIC_EXPLODE_BIG
	STATIC_REPLACE_WITH			EL_DESTROYED_LONG ;
break ;
	
case	EL_PYRAMID :
	STATIC_CRYSTAL_CHANCE_GOOD
	STATIC_EXPLODE_BIG_FAST
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;
	
case	EL_MEGATOWER2 :
	STATIC_CRYSTAL_CHANCE_VERY_GOOD
	STATIC_EXPLODE_MEGATOWER_2_GROUPS
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;

case	EL_T_OFFICE :
	STATIC_CRYSTAL_CHANCE_GOOD
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;
	
case	EL_MEGATOWER :
	STATIC_CRYSTAL_CHANCE_GOOD
	STATIC_EXPLODE_MEGATOWER_2_GROUPS
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;

case	EL_FACTORY1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;

case	EL_FACTORY2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;

case	EL_L_TOWER2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;
	
case	EL_TOWER1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;
	
case	EL_TOWER3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;
	
case	EL_LINK1 :
	STATIC_CRYSTAL_CHANCE_POOR
	STATIC_EXPLODE_MEDIUM_FAST
break ;

case	EL_LINK2 :
	STATIC_CRYSTAL_CHANCE_POOR
	STATIC_EXPLODE_MEDIUM_FAST
break ;
	
case	EL_DOME :
	STATIC_CRYSTAL_CHANCE_POOR
	STATIC_EXPLODE_BIG_FAST
	STATIC_REPLACE_WITH			EL_DESTROYED_BIG ;
break ;
	
case	EL_TREE :
	EARTH_LAND_HIDDEN_BONUS
	STATIC_CRYSTAL_CHANCE_VERY_POOR
	STATIC_EXPLODE_SMALL
break ;
	
case	EL_DOME_S :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;
	
case	EL_BIG_TREE :
	EARTH_LAND_HIDDEN_BONUS
	STATIC_CRYSTAL_CHANCE_POOR
	STATIC_EXPLODE_MEDIUM
break ;
	
case	EL_RADAR_BASE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_MEDIUM_FAST
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;
	
case	EL_X_PIPE :
	STATIC_CRYSTAL_CHANCE_VERY_VERY_GOOD
	STATIC_EXPLODE_SMALL
break ;

case	EL_B_PIPE1_1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	EL_B_PIPE1_2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	EL_PIPE1 :
	STATIC_CRYSTAL_CHANCE_VERY_POOR
	STATIC_EXPLODE_SMALL
break ;

case	EL_B_PIPE2_1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	EL_B_PIPE2_2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	EL_PIPE2 :
	STATIC_CRYSTAL_CHANCE_VERY_POOR
	STATIC_EXPLODE_SMALL
break ;

case	EL_TALL_TREE :
	EARTH_LAND_HIDDEN_BONUS
	STATIC_CRYSTAL_CHANCE_POOR
	STATIC_EXPLODE_MEDIUM
break ;

case	EL_DESTROYED_MEDIUM :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			EL_DESTROYED_SMALL ;
break ;

case	EL_DESTROYED_LONG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;

case	EL_DESTROYED_BIG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			EL_DESTROYED_MEDIUM ;
break ;

case	EL_DESTROYED_SMALL :
	STATIC_EXPLODE_SMALL
break ;

}

}

// ##################################################################

void explosion_next_frame_earth_land_graphics( long x_grid , long y_grid )
{

// Certain graphics which link onto others must die if they are no longer connected

switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	EL_LINK1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_LINK2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_X_PIPE :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_B_PIPE1_1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_B_PIPE1_2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_PIPE1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_B_PIPE2_1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_B_PIPE2_2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	EL_PIPE2 :
	DELAY_EXPLODE_THIS_GRAPHIC
}

}

// ##################################################################

void check_bonus_earth_land_graphics( long x_grid , long y_grid )
{
long type ;

type = poly_map [ y_grid ] [ x_grid ] ;

if (type == EL_GUN1 && bonus_crystal_object.object_1 == OBJECT_NOTHING)
{
	//printf("Bonus object 1 found \n");
	bonus_crystal_object.object_1 = type ;
	return ;
}

if (type == EL_SAM_BASE3 && bonus_crystal_object.object_2 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 )
{
	//printf("Bonus object 3 found \n");
	bonus_crystal_object.object_2 = type ;
	return ;
}

if (type == EL_B_OFFICE && bonus_crystal_object.object_3 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_SAM_BASE3 )
{
	//printf("Bonus object 3 found \n");
	bonus_crystal_object.object_3 = type ;
	return ;
}

if (type == EL_T_OFFICE && bonus_crystal_object.object_4 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_SAM_BASE3 &&
	bonus_crystal_object.object_3 == EL_B_OFFICE )
{
	//printf("Bonus object 4 found \n");

	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;

	bonus_crystal_object.object_4 = type ;

	start_bonus_crystal_adder( x_grid , y_grid ) ;
	return ;
}


//printf("Bonus object lost \n");

bonus_crystal_object.object_1 = OBJECT_NOTHING ;
bonus_crystal_object.object_2 = OBJECT_NOTHING ;
bonus_crystal_object.object_3 = OBJECT_NOTHING ;
bonus_crystal_object.object_4 = OBJECT_NOTHING ;

}


long test_display_earth_land_graphics( long test_graphic )
{

test_graphic+=1;
if ( test_graphic > EL_MAX ) test_graphic = 1 ;

return( test_graphic );

}


// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################

// EARTH SEA BASED

void explosion_check_earth_sea_graphics( long x_grid , long y_grid )
{

// Check object below
explosion_next_frame_earth_sea_graphics( x_grid , ((y_grid-1)&127) ) ;

// Check object above
explosion_next_frame_earth_sea_graphics( x_grid , ((y_grid+1)&127) ) ;

// Check object to left
explosion_next_frame_earth_sea_graphics( ((x_grid-1)&127) , y_grid ) ;

// Check object to right
explosion_next_frame_earth_sea_graphics( ((x_grid+1)&127) , y_grid ) ;

// What type of explosion is this object
switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	ES_GUN1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_GUN2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_SUB :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
break ;

case	ES_SAM_BASE1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_SAM_BASE2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_SAM_BOAT :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
break ;

case	ES_HANGER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			ES_DESTROYED_MEDIUM ;
break ;

case	ES_CARRIER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
break ;

case	ES_CON_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_OFFICE_1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_LONG ;
break ;

case	ES_OFFICE_R :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_MEDIUM ;
break ;

case	ES_TOWER1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_FACTORY :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_MEDIUM ;
break ;

case	ES_OIL_RIG :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	ES_FACTORY2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			ES_DESTROYED_MEDIUM ;
break ;

case	ES_LIGHTHOUSE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_PIER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	ES_L_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_MEDIUM ;
break ;

case	ES_TOWER2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_SPACEWALL1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_SPACEWALL2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_LINK1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	ES_LINK2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	ES_TREE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	ES_BIG_TREE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	ES_DESTROYED_MEDIUM :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			ES_DESTROYED_SMALL ;
break ;

case	ES_DESTROYED_LONG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			ES_DESTROYED_MEDIUM ;
break ;

case	ES_DESTROYED_BIG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			ES_DESTROYED_MEDIUM ;	
break ;

case	ES_DESTROYED_SMALL :
	STATIC_EXPLODE_SMALL
break ;

}

}

// ##################################################################

void explosion_next_frame_earth_sea_graphics( long x_grid , long y_grid )
{

// Certain graphics which link onto others must die if they are no longer connected

switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	ES_SPACEWALL1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	ES_SPACEWALL2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	ES_LINK1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	ES_LINK2 :
	DELAY_EXPLODE_THIS_GRAPHIC
}

}

// ##################################################################

void check_bonus_earth_sea_graphics( long x_grid , long y_grid )
{
long type ;

type = poly_map [ y_grid ] [ x_grid ] ;

if (type == ES_SUB && bonus_crystal_object.object_1 == OBJECT_NOTHING)
{
	bonus_crystal_object.object_1 = type ;
	return ;
}

if (type == ES_SAM_BOAT && bonus_crystal_object.object_2 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == ES_SUB )
{
	bonus_crystal_object.object_2 = type ;
	return ;
}

if (type == ES_CON_TOWER && bonus_crystal_object.object_3 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == ES_SUB &&
	bonus_crystal_object.object_2 == ES_SAM_BOAT )
{
	bonus_crystal_object.object_3 = type ;
	return ;
}

if (type == ES_LIGHTHOUSE && bonus_crystal_object.object_4 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == ES_SUB &&
	bonus_crystal_object.object_2 == ES_SAM_BOAT &&
	bonus_crystal_object.object_3 == ES_CON_TOWER )
{
	bonus_crystal_object.object_4 = type ;

	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;

	start_bonus_crystal_adder( x_grid , y_grid ) ;
	return ;
}

bonus_crystal_object.object_1 = OBJECT_NOTHING ;
bonus_crystal_object.object_2 = OBJECT_NOTHING ;
bonus_crystal_object.object_3 = OBJECT_NOTHING ;
bonus_crystal_object.object_4 = OBJECT_NOTHING ;

}

long test_display_earth_sea_graphics( long test_graphic )
{

test_graphic+=1;
if ( test_graphic > ES_MAX ) test_graphic = 1 ;

return( test_graphic );

}

// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################

// CHEMICAL / ICT

void explosion_check_chemical_graphics( long x_grid , long y_grid )
{

// Check object below
explosion_next_frame_chemical_graphics( x_grid , ((y_grid-1)&127) ) ;

// Check object above
explosion_next_frame_chemical_graphics( x_grid , ((y_grid+1)&127) ) ;

// Check object to left
explosion_next_frame_chemical_graphics( ((x_grid-1)&127) , y_grid ) ;

// Check object to right
explosion_next_frame_chemical_graphics( ((x_grid+1)&127) , y_grid ) ;

// What type of explosion is this object
switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	CM_GUN3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_GUN2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_GUN1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_SAM_BASE3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_SAM_BASE2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_SAM_BASE1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_HANG_2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			CM_DESTROYED_MEDIUM ;
break ;

case	CM_DEATH_HANG :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			CM_DESTROYED_MEDIUM ;
break ;

case	CM_FUEL_TANK :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_CHEM_1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_CHEM_2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_RADAR_BASE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_ROCKIT :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
break ;

case	CM_B_OFFICE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_LONG ;
break ;

case	CM_CON_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_COOL_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_NOFFICE_1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_LONG ;
break ;

case	CM_Nl_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_L_TOWER2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_MEDIUM ;
break ;

case	CM_NTOWER1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_NTOWER3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_NFACTORY2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			CM_DESTROYED_MEDIUM ;
break ;

case	CM_FACTORY1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_MEDIUM ;
break ;

case	CM_ARCH_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_LINK1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_LINK2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_SPACEWALL1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_SPACEWALL2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_CX_PIPE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_CB_PIPE1_1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_CB_PIPE1_2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_CPIPE1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_CB_PIPE2_1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_CB_PIPE2_2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_CPIPE2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	CM_DESTROYED_MEDIUM :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			CM_DESTROYED_SMALL ;
break ;

case	CM_DESTROYED_LONG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			CM_DESTROYED_MEDIUM ;
break ;

case	CM_DESTROYED_BIG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			CM_DESTROYED_MEDIUM ;	
break ;

case	CM_DESTROYED_SMALL :
	STATIC_EXPLODE_SMALL
break ;

}

}

// ##################################################################

void explosion_next_frame_chemical_graphics( long x_grid , long y_grid )
{

// Certain graphics which link onto others must die if they are no longer connected

switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	CM_LINK1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_LINK2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_SPACEWALL1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_SPACEWALL2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_CX_PIPE :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_CB_PIPE1_1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_CB_PIPE1_2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_CPIPE1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_CB_PIPE2_1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_CB_PIPE2_2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	CM_CPIPE2 :
	DELAY_EXPLODE_THIS_GRAPHIC
}

}

// ##################################################################

void check_bonus_chemical_graphics( long x_grid , long y_grid )
{
long type ;

type = poly_map [ y_grid ] [ x_grid ] ;

if (type == CM_FUEL_TANK && bonus_crystal_object.object_1 == OBJECT_NOTHING)
{
	bonus_crystal_object.object_1 = type ;
	return ;
}

if (type == CM_RADAR_BASE && bonus_crystal_object.object_2 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == CM_FUEL_TANK )
{
	bonus_crystal_object.object_2 = type ;
	return ;
}

if (type == CM_ROCKIT && bonus_crystal_object.object_3 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == CM_FUEL_TANK &&
	bonus_crystal_object.object_2 == CM_RADAR_BASE )
{
	bonus_crystal_object.object_3 = type ;
	return ;
}

if (type == CM_COOL_TOWER && bonus_crystal_object.object_4 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == CM_FUEL_TANK &&
	bonus_crystal_object.object_2 == CM_RADAR_BASE &&
	bonus_crystal_object.object_3 == CM_ROCKIT )
{
	bonus_crystal_object.object_4 = type ;
	
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	
	start_bonus_crystal_adder( x_grid , y_grid ) ;
	return ;
}


bonus_crystal_object.object_1 = OBJECT_NOTHING ;
bonus_crystal_object.object_2 = OBJECT_NOTHING ;
bonus_crystal_object.object_3 = OBJECT_NOTHING ;
bonus_crystal_object.object_4 = OBJECT_NOTHING ;

}

long test_display_chemical_graphics( long test_graphic )
{

test_graphic+=1;
if ( test_graphic > CM_MAX ) test_graphic = 1 ;

return( test_graphic );

}

// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################

// THORG / WARRIOR

void explosion_check_thorg_graphics( long x_grid , long y_grid )
{

// Check object below
explosion_next_frame_thorg_graphics( x_grid , ((y_grid-1)&127) ) ;

// Check object above
explosion_next_frame_thorg_graphics( x_grid , ((y_grid+1)&127) ) ;

// Check object to left
explosion_next_frame_thorg_graphics( ((x_grid-1)&127) , y_grid ) ;

// Check object to right
explosion_next_frame_thorg_graphics( ((x_grid+1)&127) , y_grid ) ;

// What type of explosion is this object
switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	TH_R_GUN1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_R_GUN2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_R_GUN3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_R_SAM1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_R_SAM2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_R_SAM3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_ROCK_HANG :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			TH_DESTROYED_LONG ;	
break ;

case	TH_T_R_HANG :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;	
break ;

case	TH_C_L_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;	
break ;

case	TH_CHURCH :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_DES_OFF :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;	
break ;

case	TH_HEAD :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_PALACE_R :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_PALACE_REL_3_GROUPS
	STATIC_REPLACE_WITH			TH_DESTROYED_BIG ;	
break ;

case	TH_ROCKHOUSE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_ROCKTOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
//	STATIC_EXPLODE_MEGATOWER_2_GROUPS
	STATIC_EXPLODE_SMALL	
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;	
break ;

case	TH_WATCHTOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;	
break ;

case	TH_CROSS :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_DES_HOUSE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_DES_TREE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	TH_GRAVE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	TH_WALL1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_WALL2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_C_LINK1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	TH_C_LINK2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	TH_RADAR_ROCK :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_C_TOWER2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;	
break ;

case	TH_WALL1_S :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_WALL2_S :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;	
break ;

case	TH_DESTROYED_MEDIUM :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			TH_DESTROYED_SMALL ;
break ;

case	TH_DESTROYED_LONG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;
break ;

case	TH_DESTROYED_BIG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			TH_DESTROYED_MEDIUM ;	
break ;

case	TH_DESTROYED_SMALL :
	STATIC_EXPLODE_SMALL
break ;

}


}

// ##################################################################

void explosion_next_frame_thorg_graphics( long x_grid , long y_grid )
{

// Certain graphics which link onto others must die if they are no longer connected

switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	TH_WALL1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	TH_WALL2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	TH_C_LINK1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	TH_C_LINK2 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	TH_WALL1_S :
	DELAY_EXPLODE_THIS_GRAPHIC

case	TH_WALL2_S :
	DELAY_EXPLODE_THIS_GRAPHIC
}

}

// ##################################################################

void check_bonus_thorg_graphics( long x_grid , long y_grid )
{
long type ;

type = poly_map [ y_grid ] [ x_grid ] ;

if (type == EL_GUN1 && bonus_crystal_object.object_1 == OBJECT_NOTHING)
{
	bonus_crystal_object.object_1 = type ;
	return ;
}

if (type == EL_ARCH_TOWER && bonus_crystal_object.object_2 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 )
{
	bonus_crystal_object.object_2 = type ;
	return ;
}

if (type == EL_TREE && bonus_crystal_object.object_3 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_ARCH_TOWER )
{
	bonus_crystal_object.object_3 = type ;
	return ;
}

if (type == EL_BIG_TREE && bonus_crystal_object.object_4 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_ARCH_TOWER &&
	bonus_crystal_object.object_3 == EL_TREE )
{
	bonus_crystal_object.object_4 = type ;

	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;

	start_bonus_crystal_adder( x_grid , y_grid ) ;
	return ;
}

bonus_crystal_object.object_1 = OBJECT_NOTHING ;
bonus_crystal_object.object_2 = OBJECT_NOTHING ;
bonus_crystal_object.object_3 = OBJECT_NOTHING ;
bonus_crystal_object.object_4 = OBJECT_NOTHING ;

}

long test_display_thorg_graphics( long test_graphic )
{

test_graphic+=1;
if ( test_graphic > TH_MAX ) test_graphic = 1 ;

return( test_graphic );

}


// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################

// SPACE THE FINAL FRONT EAR

void explosion_check_space_graphics( long x_grid , long y_grid )
{

// What type of explosion is this object
switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	SP_SPACE_GUNL :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_SPACE_GUNM :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_SPACE_GUNH :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_SPACE_SAML :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_SPACE_SAMM :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_SPACE_SAMH :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_FREE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_FREE2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H0 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H4 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H5 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H6 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_2_H7 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H0 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H4 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H5 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H6 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	SP_AST_1_H7 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;
}

}


// ##################################################################

void check_bonus_space_graphics( long x_grid , long y_grid )
{
long type ;

return;

type = poly_map [ y_grid ] [ x_grid ] ;

if (type == EL_GUN1 && bonus_crystal_object.object_1 == OBJECT_NOTHING)
{
	bonus_crystal_object.object_1 = type ;
	return ;
}

if (type == EL_ARCH_TOWER && bonus_crystal_object.object_2 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 )
{
	bonus_crystal_object.object_2 = type ;
	return ;
}

if (type == EL_TREE && bonus_crystal_object.object_3 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_ARCH_TOWER )
{
	bonus_crystal_object.object_3 = type ;
	return ;
}

if (type == EL_BIG_TREE && bonus_crystal_object.object_4 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_ARCH_TOWER &&
	bonus_crystal_object.object_3 == EL_TREE )
{
	bonus_crystal_object.object_4 = type ;
	start_bonus_crystal_adder( x_grid , y_grid ) ;
	return ;
}


bonus_crystal_object.object_1 = OBJECT_NOTHING ;
bonus_crystal_object.object_2 = OBJECT_NOTHING ;
bonus_crystal_object.object_3 = OBJECT_NOTHING ;
bonus_crystal_object.object_4 = OBJECT_NOTHING ;

}

long test_display_space_graphics( long test_graphic )
{

test_graphic+=1;
if ( test_graphic > SP_MAX ) test_graphic = 1 ;

return( test_graphic );

}

// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################

// DEATH STAR

void explosion_check_death_star_graphics( long x_grid , long y_grid )
{

// Check object below
explosion_next_frame_death_star_graphics( x_grid , ((y_grid-1)&127) ) ;

// Check object above
explosion_next_frame_death_star_graphics( x_grid , ((y_grid+1)&127) ) ;

// Check object to left
explosion_next_frame_death_star_graphics( ((x_grid-1)&127) , y_grid ) ;

// Check object to right
explosion_next_frame_death_star_graphics( ((x_grid+1)&127) , y_grid ) ;

// What type of explosion is this object
switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	DS_GUN_F :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_GUN_F2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_GUN_F3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_SAM_F :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_SAM_F2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_SAM_F3 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_DEATH_HANG :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			DS_DESTROYED_MEDIUM ;
break ;

case	DS_FREE :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	DS_TOWER1 :
	STATIC_EXPLODE_NOT

case	DS_TOWER2 :
	STATIC_EXPLODE_NOT

case	DS_TOWER3 :
	STATIC_EXPLODE_NOT

case	DS_TOWER4 :
	STATIC_EXPLODE_NOT

case	DS_TOWER5 :
	STATIC_EXPLODE_NOT

case	DS_TOWER6 :
	STATIC_EXPLODE_NOT

case	DS_TOWER7 :
	STATIC_EXPLODE_NOT

case	DS_TOWER8 :
	STATIC_EXPLODE_NOT

case	DS_DEATH_DOME :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_BIG_FAST_MUSHROOM
	STATIC_REPLACE_WITH			DS_DESTROYED_BIG ;
break ;

case	DS_STAR_TOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_STAR_TOWER_4_GROUPS
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_SPACETOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_TOWER_1_GROUP
	STATIC_REPLACE_WITH			DS_DESTROYED_MEDIUM ;
break ;

case	DS_DEATH_DOM2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL_FAST_MUSHROOM
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_DEATH_ARCH :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	DS_COOL_DEATH :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_NMEGATOWER :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_MEGATOWER_2_GROUPS
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_ROCKIT :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
break ;

case	DS_SPACEWALL1 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_SPACEWALL2 :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_DL_LINK1 :
	STATIC_EXPLODE_NOT

case	DS_DL_LINK2 :
	STATIC_EXPLODE_NOT

case	DS_DH_LINK1 :
	STATIC_EXPLODE_NOT

case	DS_DH_LINK2 :
	STATIC_EXPLODE_NOT

case	DS_DEATH_PYR :
	STATIC_CRYSTAL_CHANCE_AVERAGE
	STATIC_EXPLODE_SMALL
	STATIC_REPLACE_WITH			DS_DESTROYED_MEDIUM ;	
break ;

case	DS_DESTROYED_MEDIUM :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			DS_DESTROYED_SMALL ;
break ;

case	DS_DESTROYED_LONG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			DS_DESTROYED_MEDIUM ;
break ;

case	DS_DESTROYED_BIG :
	STATIC_EXPLODE_MEDIUM
	STATIC_REPLACE_WITH			DS_DESTROYED_MEDIUM ;	
break ;

case	DS_DESTROYED_SMALL :
	STATIC_EXPLODE_SMALL
break ;

}

}

// ##################################################################

void explosion_next_frame_death_star_graphics( long x_grid , long y_grid )
{

// Certain graphics which link onto others must die if they are no longer connected

switch ( poly_map [ y_grid ] [ x_grid ] )
{
case	DS_SPACEWALL1 :
	DELAY_EXPLODE_THIS_GRAPHIC

case	DS_SPACEWALL2 :
	DELAY_EXPLODE_THIS_GRAPHIC
}

}

// ##################################################################

void check_bonus_death_star_graphics( long x_grid , long y_grid )
{
long type ;

type = poly_map [ y_grid ] [ x_grid ] ;

if (type == EL_GUN1 && bonus_crystal_object.object_1 == OBJECT_NOTHING)
{
	bonus_crystal_object.object_1 = type ;
	return ;
}

if (type == EL_ARCH_TOWER && bonus_crystal_object.object_2 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 )
{
	bonus_crystal_object.object_2 = type ;
	return ;
}

if (type == EL_TREE && bonus_crystal_object.object_3 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_ARCH_TOWER )
{
	bonus_crystal_object.object_3 = type ;
	return ;
}

if (type == EL_BIG_TREE && bonus_crystal_object.object_4 == OBJECT_NOTHING &&
	bonus_crystal_object.object_1 == EL_GUN1 &&
	bonus_crystal_object.object_2 == EL_ARCH_TOWER &&
	bonus_crystal_object.object_3 == EL_TREE )
{
	bonus_crystal_object.object_4 = type ;
	start_bonus_crystal_adder( x_grid , y_grid ) ;

	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;
	bonus_parachute( 0 ) ;

	return ;
}


bonus_crystal_object.object_1 = OBJECT_NOTHING ;
bonus_crystal_object.object_2 = OBJECT_NOTHING ;
bonus_crystal_object.object_3 = OBJECT_NOTHING ;
bonus_crystal_object.object_4 = OBJECT_NOTHING ;

}

long test_display_death_star_graphics( long test_graphic )
{

test_graphic+=1;
if ( test_graphic > DS_MAX ) test_graphic = 1 ;

return( test_graphic );

}

// ##################################################################
// ##################################################################
// ##################################################################
// ##################################################################
