#include "Smoke_Struct.h"
#include "Ship_Struct.h"

extern smoke_list smokes ;

extern long cosine_table [ 2048 ] ;
extern long *sine_table ;

extern long which_graphics_set ;

void smoke_update(void);

void add_smoke( long , long , long , long , long , long , long , long );

void update_mushroom( smoke_stack* ) ;

void fighter_thrusters( ship_stack* ) ;

#define BASE_THRUSTER_COLOUR	78
#define BASE_EXPLOSION_COLOUR	16
#define BASE_MISSILE_COLOUR		20
#define BASE_DAMAGE_COLOUR		23
#define BASE_MUSHROOM_COLOUR	78
#define BASE_WING_POD_COLOUR	22
#define BASE_LASER_HIT_COLOUR	70

#define SMALL_THRUSTER_SMOKE	0 
#define EXPLOSION_SMOKE			1
#define MUSHROOM_SMOKE			2
#define BIG_MUSHROOM_CLOUD		3
#define SMALL_MUSHROOM_CLOUD	4
#define MISSILE_SMOKE			5
#define BIG_THRUSTER_SMOKE		6
#define BIG_DAMAGE_SMOKE		7
#define WING_POD_SMOKE			8
#define LASER_HIT_SMOKE			9
#define MEDIUM_MUSHROOM_CLOUD	10
#define BLACK_EXPLOSION_SMOKE	11
#define SMALL_EXPLOSION_SMOKE	12
#define BIG_EXPLOSION_SMOKE		13
#define SMALL_DAMAGE_SMOKE		14
#define SECTION_SMOKE			15
#define SECTION_SMOKE2			16
#define SECTION_SMOKE3			17
#define SECTION_SMOKE4			18















