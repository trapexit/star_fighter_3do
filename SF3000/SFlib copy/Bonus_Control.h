#include "Ship_Struct.h"

extern ship_list ships ;
extern ship_stack* players_ship ;

extern long bonus_collision_this_frame ;

void bonus_control( ship_stack * ) ;

void add_bonus( long , long , long , long , long , long , long ) ;
void add_crystal_from_static( long , long , long ) ;

void start_bonus_crystal_adder( long , long ) ;
void update_bonus_crystal_adder( void ) ;

void add_bonus_from_collision_box( long , long ) ;

#include "Misc_Struct.h"

extern bonus_crystal_adder_struct bonus_crystal_adder ;

