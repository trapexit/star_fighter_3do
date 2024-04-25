//	File : SF_Cel_Utils
//	General cel utilities

//	Includes

#include "SF_CelUtils.h"
#include "SF_ARMSky.h"
#include "SF_Palette.h"
#include "SF_ARMCell.h"
#include "SF_ScreenUtils.h"
#include <string.h>

//	Definitions

#define	MAX_ALPHACHARS 120					// Max number of characters allowed in char map

//	Global variables

long		cel_alphacount,					// Total number of alpha-characters in char map
			cel_alphadelay,					// Delay before character updated
			cel_alphaaddmode,				// Type of add mode
			cel_alphaaddmodecount;			// Count for adder mode

alpha_char	cel_alphamap [MAX_ALPHACHARS];	// Character array map
alpha_char	cel_character;					// Cel Character for adding

char		alpha_widths [] = {8,6,8,8,8,8,8,8,8,8,11,10,10,10,9,9,11,10,3,7,11,9,12,10,12,9,12,10,10,9,10,10,14,10,10,9,11,11, 10};

/**************************************/

long cel_initialise(void)						

// Purpose : Allocates memory for list of temporary display cels and 16x16, 32x32 sprite areas etc.
// Accepts : external global variable pointers
// Returns : 0 if failed, or 1 if successful

{

long 	cel_loop;
CCB*	cel_temp;

// Allocate memory for fixed game cels, 32x32 and 16x16 landscape / polygon cels and creation cels

	cel_game = (char*) AllocMem (sizeof (char) *1024*CEL_MAXGAME, MEMTYPE_CEL);				// Game sprites area
	cel_creation = (char*) AllocMem (sizeof (char) *264*(CEL_MAXCREATION), MEMTYPE_CEL);	// Created sprites area
	cel_palette = (char*) AllocMem (sizeof (char) *1024, MEMTYPE_CEL);						// Monochrome polygon palette area
	cel_codedpalette = (char*) AllocMem (sizeof (char) *64, MEMTYPE_CEL);					// 32entry coded palette	
	cel_plutdata = (char*) AllocMem (sizeof (char) *64, MEMTYPE_CEL);						// Game plut area			
	cel_list32 = (char*) AllocMem (sizeof (char) *(1024*CEL_MAX32), MEMTYPE_CEL);			// Area of 32x32 textures
	cel_list16 = (char*) AllocMem (sizeof (char) *(1024*CEL_MAX16), MEMTYPE_CEL);			// Area of 16x16 textures
	
	map512 = (char*) AllocMem (sizeof (char) *(1024*192), MEMTYPE_CEL);						// Area for 512x512 map
	map128 = (char*) AllocMem (sizeof (char) *(1024*12), MEMTYPE_CEL);						// Area for 128x128 map
	map32 = (char*) AllocMem (sizeof (char) *(768), MEMTYPE_CEL);							// Area for 32x32 map
	
	// NOTE : map512 (Large Area Map) can also be used temporarily for the storage of gamecels
	
// Build temporary plotting list of CCB's

	for (cel_loop=0; cel_loop<CEL_MAXTEMP; cel_loop++)
	{
		cel_temp = (CCB *)ALLOCMEM(sizeof(CCB),MEMTYPE_CEL);
		if (cel_temp==NULL)
			return(0);
			
		*(cel_plotlist+cel_loop)=cel_temp;
		
		cel_temp->ccb_Flags=	CCB_NPABS |			// Absolute NEXTPTR
								CCB_SPABS |			// Absolute SOURCEPTR
								CCB_PPABS |			// Absolute PLUTPTR
								CCB_LDSIZE |		// Load in HDX, HDY, VDX, VDY
								CCB_LDPRS |			// Load in HDDX, HDDY
								CCB_LDPLUT |		// Load in PLUT
							 	(1<<24) |			// Load in PIXC word
								CCB_YOXY |			// Set x&y position
								CCB_ACW |			// Enable clockwise pixel rendering
								CCB_ACCW |			// Enable counter clockwise pixel rendering
								CCB_ACE |			// Enable 2nd cel engine
								(1<<8) |			// Use PMode 0
								CCB_BGND;			// pass 000rgb as colour - no transp
	}	


// Link all temp plotting cels together

	for (cel_loop=0; cel_loop<CEL_MAXTEMP-1; cel_loop++)
		(*(cel_plotlist+cel_loop))->ccb_NextPtr = *(cel_plotlist+cel_loop+1);


// Initialise cel data block

	cel_quad.plotlist = cel_plotlist;
	cel_quad.cel_creation = cel_creation;
	cel_quad.cel_palette = cel_palette;
	cel_quad.cel_codedpalette = cel_codedpalette;
	cel_quad.cel_list16 = cel_list16;
	cel_quad.cel_list32 = cel_list32;
	cel_quad.cels4x4 = cels4x4;
	cel_quad.cel_game = cel_game;
	cel_quad.cel_map512 = map512;
	cel_quad.cel_map128 = map128;
	cel_quad.cel_map32 = map32;
	cel_quad.cache_lookup = cache_lookup;
	cel_quad.cache_free = cache_free;
	
	cel_quad.screen_address0 = (char*) ((screen->sc_Bitmaps [0]) -> bm_Buffer);
	cel_quad.screen_address1 = (char*) ((screen->sc_Bitmaps [1]) -> bm_Buffer);
	
	return(1);
}

/**************************************/

void	cel_rectangle	(long xpos, long ypos, long xpos2, long ypos2, long colour, long type, long trans)

// Purpose : Draws a shaded rectangle from a (base) palette colour on the screen using
//			 (optionally) transparent monochrome cels. Screen co-ordinates are normal, 0-320, 0-240
//			 Coordinates are in bot left x,y then top right x,y
// Accepts : x,y pos, x,y width, rectangle, transparency ?
// Returns : Nothing

{

	// Draw borders ?
	
	if (type & REC_BORDER)
		{
		
		// DRAW TOP REC_BORDERSIZE
	
		cel_quad.x_pos0 = xpos-160;						cel_quad.y_pos0 = ypos2-120;
		cel_quad.x_pos1 = xpos2-160;					cel_quad.y_pos1 = ypos2-120;
		cel_quad.x_pos2 = (xpos2-REC_BORDERSIZE)-160;	cel_quad.y_pos2 = (ypos2+REC_BORDERSIZE)-120;
		cel_quad.x_pos3 = (xpos+REC_BORDERSIZE)-160;	cel_quad.y_pos3 = (ypos2+REC_BORDERSIZE)-120;
		cel_quad.shade 	= REC_SHADE_UP;
		arm_addmonocel (&cel_quad,0,colour, trans);
					
		// DRAW LEFT REC_BORDERSIZE
	
		cel_quad.x_pos0 = xpos-160;						cel_quad.y_pos0 = ypos2-120;
		cel_quad.x_pos1 = (xpos+REC_BORDERSIZE)-160;	cel_quad.y_pos1 = (ypos2+REC_BORDERSIZE)-120;
		cel_quad.x_pos2 = (xpos+REC_BORDERSIZE)-160;	cel_quad.y_pos2 = (ypos-REC_BORDERSIZE)-120;
		cel_quad.x_pos3 = xpos-160;						cel_quad.y_pos3 = ypos-120;
		cel_quad.shade 	= REC_SHADE_LEFT;
		arm_addmonocel (&cel_quad,0,colour, trans);
	
		// DRAW BOTTOM REC_BORDERSIZE
	
		cel_quad.x_pos0 = (xpos+REC_BORDERSIZE)-160;	cel_quad.y_pos0 = (ypos-REC_BORDERSIZE)-120;
		cel_quad.x_pos1 = (xpos2-REC_BORDERSIZE)-160;	cel_quad.y_pos1 = (ypos-REC_BORDERSIZE)-120;
		cel_quad.x_pos2 = xpos2-160;					cel_quad.y_pos2 = ypos-120;
		cel_quad.x_pos3 = xpos-160;						cel_quad.y_pos3 = ypos-120;
		cel_quad.shade 	= REC_SHADE_DOWN;
		arm_addmonocel (&cel_quad,0,colour, trans);
	
		// DRAW RIGHT REC_BORDERSIZE
	
		cel_quad.x_pos0 = (xpos2-REC_BORDERSIZE)-160;	cel_quad.y_pos0 = (ypos2+REC_BORDERSIZE)-120;
		cel_quad.x_pos1 = xpos2-160;					cel_quad.y_pos1 = ypos2-120;
		cel_quad.x_pos2 = xpos2-160;					cel_quad.y_pos2 = ypos-120;
		cel_quad.x_pos3 = (xpos2-REC_BORDERSIZE)-160;	cel_quad.y_pos3 = (ypos-REC_BORDERSIZE)-120;
		cel_quad.shade 	= REC_SHADE_LEFT;
		arm_addmonocel (&cel_quad,0,colour, trans);
		}
		
	// Draw box in the middle ?
	
	if (type & REC_BOX)
		{
		
		// DRAW MIDDLE
	
		cel_quad.x_pos0 = (xpos+REC_BORDERSIZE)-160;		cel_quad.y_pos0 = (ypos2+REC_BORDERSIZE)-120;
		cel_quad.x_pos1 = (xpos2-REC_BORDERSIZE)-160;		cel_quad.y_pos1 = (ypos2+REC_BORDERSIZE)-120;
		cel_quad.x_pos2 = (xpos2-REC_BORDERSIZE)-160;		cel_quad.y_pos2 = (ypos-REC_BORDERSIZE)-120;
		cel_quad.x_pos3 = (xpos+REC_BORDERSIZE)-160;		cel_quad.y_pos3 = (ypos-REC_BORDERSIZE)-120;
		cel_quad.shade 	= REC_SHADE_MIDDLE;
		arm_addmonocel (&cel_quad,0,colour, trans);
				
		}
}

/**************************************/

void cel_paint (long xpos, long ypos, long shade, long cel)

// Purpose : Immediately draws a game cel onto the screen
// Accepts : xpos, ypos, shade value, cel number
// Returns : nothing

{
	cel_quad.temp_cels=0;
	
	cel_quad.x_pos0 = xpos-160;
	cel_quad.y_pos0 = ypos-120;
	cel_quad.shade = shade;
	arm_addgamecel (&cel_quad, cel, 1024, 1024);
	screen_update (S_CEL);
}

/**************************************/

void cel_setgamearea (long areatype)

// Purpose : Sets the pointer to the gamecels
// Accepts : (0) for standard, (1) for temporary super-large 192k 512x512 map area
// Returns : nothing

{
	if (areatype == 0)
		cel_quad.cel_game = cel_game;
	else
		cel_quad.cel_game = map512;
}

/**************************************/

void cel_resetcharactermap (void)

// Purpose : Clears active character list
// Accepts : nothing
// Returns : nothing

{
	cel_alphacount = 0;
	cel_alphadelay = 0;
}

/**************************************/

long cel_returnnextcharnumber (void)

// Purpose : Returns number of characters currently in
// Accepts : nothing
// Returns : frink

{
	return (cel_alphacount);
}

/**************************************/

void cel_movecharacterby (long char_to_move, long move_x, long move_y)

// Purpose : Moves a character's destination by X,Y pixels
// Accepts : Char to move, x & y pixel movement
// Returns : now't

{
	cel_alphamap [char_to_move].end_x_pos += move_x;	// Move destination points
	cel_alphamap [char_to_move].end_y_pos += move_y;

	cel_alphamap [char_to_move].x_dist -= move_x;		// Move offsets
	cel_alphamap [char_to_move].y_dist -= move_y;
}

/**************************************/

void cel_getalphastart (alpha_char *alpha_character)

// Purpose : Calculates a starting position & size for a character being added from a string, depends on add-mode & count
// Accepts : Pointer to alpha character block
// Returns : nothing

{

long	ang;

	switch (cel_alphaaddmode)
		{
		
		case ALPHA_NORMAL :								// Doing a normal add ? no moving about
			alpha_character->x_dist = 0;
			alpha_character->y_dist = 0;
			alpha_character->size_dist = 0;
			break;
			
		case ALPHA_TOP :								// Zoom from top of screen ?
			alpha_character->x_dist = ((cel_alphaaddmodecount*16) - alpha_character->end_x_pos);
			alpha_character->y_dist = ((-32) - alpha_character->end_y_pos);
			alpha_character->size_dist = (8192 - alpha_character->end_size);
			break;
			
		case ALPHA_BOTTOM :								// Zoom from bottom of screen ?
			alpha_character->x_dist = ((cel_alphaaddmodecount*16) - alpha_character->end_x_pos);
			alpha_character->y_dist = ((272) - alpha_character->end_y_pos);
			alpha_character->size_dist = (8192 - alpha_character->end_size);
			break;
			
		case ALPHA_CIRCLE :								// Arrange in a circle ?
			ang = ((cel_alphaaddmodecount<<4) & 1023);
			alpha_character->x_dist = ((((160*(*(sine_table + ang )))>>12)+160) - alpha_character->end_x_pos);
			alpha_character->y_dist = ((((120*cosine_table [ang] )>>12)+120) - alpha_character->end_y_pos);
			alpha_character->size_dist = (8192 - alpha_character->end_size);
			break;
			
		case ALPHA_BEHIND :								// Arrange from centre ?
			alpha_character->x_dist = ((160) - alpha_character->end_x_pos);
			alpha_character->y_dist = ((120) - alpha_character->end_y_pos);
			alpha_character->size_dist = (-alpha_character->end_size);
			break;
		}
		
	cel_alphaaddmodecount+=1;							// Inc add counter
}

/**************************************/

long cel_addalphastring (char *alpha_string, long alpha_xpos, long alpha_ypos, long alpha_size)

// Purpose : Adds a string at x,y,size on screen starting from char places dictated by alpha add mode
// Accepts : Pointer to string, string xpos, ypos and end size
// Returns : Number of next character

{

char	char_toadd;

	while (*alpha_string != 0)
		{
		char_toadd = 255;
		
		if (*alpha_string == 'd')									// Delete icon ?
			char_toadd = 36;
			
		if (*alpha_string == 'a')									// Accept icon ?
			char_toadd = 37;
		
		if (*alpha_string == ' ')
			char_toadd = 38;
			
		if (*alpha_string >= '0' && *alpha_string <= '9')			// Rendering a number ?
			char_toadd = (*alpha_string) - '0';
			
		if (*alpha_string >= 'A' && *alpha_string <= 'Z')			// Rendering a character ?
			char_toadd = (*alpha_string) - 55;
		
		if (char_toadd != 255 && cel_alphacount < MAX_ALPHACHARS)	// Is char valid & room for it ?
			
			{
			
			cel_character.end_x_pos = alpha_xpos;					// Set end points for character
			cel_character.end_y_pos = alpha_ypos;
			cel_character.end_size = alpha_size;
	
			cel_getalphastart (&cel_character);						// Set start points for character, depending on add mode
			cel_character.character = char_toadd;					// Store char to add
			cel_character.delay = (cel_alphadelay++)>>1;			// Set delay prior to update
			memcpy (&cel_alphamap [cel_alphacount++], &cel_character, sizeof (alpha_char));	// Add character
			alpha_xpos+=alpha_widths [char_toadd]+2;
			}
		
		else
			{
			alpha_xpos+=8;											// Move to next space
			}
		alpha_string++;												// Get next character from list
		}
		
	return (cel_alphacount);										// Return number of next char to be added
}

/**************************************/

long cel_getalphastringlen (char *alpha_string)

// Purpose : Returns the length of a string in pixels
// Accepts : Pointer to string, string xpos, ypos and end size
// Returns : nothing

{

char	char_toadd;
long	pixel_length = 0;

	if (*alpha_string == 0)											// If no string, just return zero
		return (0);
		
	while (*alpha_string != 0)
		{
		char_toadd = 255;
		
		if (*alpha_string >= '0' && *alpha_string <= '9')			// Rendering a number ?
			char_toadd = (*alpha_string) - '0';
			
		if (*alpha_string >= 'A' && *alpha_string <= 'Z')			// Rendering a character ?
			char_toadd = (*alpha_string) - 55;
		
		if (char_toadd != 255)										// Is char valid ?
			pixel_length+=alpha_widths [char_toadd]+2;
		else
			pixel_length+=8;										// Add on length
			
		alpha_string++;												// Get next character from list
		}
	
	return (pixel_length)-2;
}

/**************************************/

void cel_setalphaaddmode (long addmodetype)

// Purpose : Sets alpha add mode type
// Accepts : New add type
// Returns : nothing

{
	cel_alphaaddmode = addmodetype;				// Type of add mode
	cel_alphaaddmodecount = 0;					// Count for adder mode
}

/**************************************/

void cel_addalphadelay (long alphadelay)

// Purpose : Increases alpha delay
// Accepts : Frames to increase by
// Returns : nothing

{
	cel_alphaaddmodecount += alphadelay;		// Count for adder mode
}


/**************************************/

void cel_deleteletters (long num_todelete)

// Purpose : Deletes last X letters added
// Accepts : No. of letters to delete
// Returns : nothing

{

	cel_alphacount-=num_todelete;
	
	if (cel_alphacount < 0)
		cel_alphacount = 0;
	
}

/**************************************/

void cel_rendercharactermap (long update)

// Purpose : Renders entire character map list to gamecels
// Accepts : Update pointer (0 = No update, just plot, 1 = Update sizes & positions)
// Returns : nothing

{

long	charloop;
long	val;

	for (charloop = 0; charloop < cel_alphacount; charloop++)
		{
		if (cel_alphamap [charloop].delay > 0)
			cel_alphamap [charloop].delay -=1;
		else
			{
			val = cel_alphamap [charloop].end_size + cel_alphamap [charloop].size_dist;
			
			cel_quad.x_pos0 = cel_alphamap [charloop].end_x_pos + cel_alphamap [charloop].x_dist -160;
			cel_quad.y_pos0 = cel_alphamap [charloop].end_y_pos + cel_alphamap [charloop].y_dist -120;
			cel_quad.shade = 11 + ((1024 - val)>>9);
			arm_addgamecel (&cel_quad, cel_alphamap [charloop].character, val, val);
		
			if (update == 1)
				{
			
				// UPDATE CHAR POSITIONS & CHARACTER SIZES
			
				cel_alphamap [charloop].x_dist = (cel_alphamap [charloop].x_dist*14) / 16 ;				// Update X Offset
				cel_alphamap [charloop].y_dist = (cel_alphamap [charloop].y_dist*14) / 16 ;				// Update Y Offset
				cel_alphamap [charloop].size_dist = (cel_alphamap [charloop].size_dist*14) / 16 ;		// Update Size Offset
				
				}
			}
		}
}	
