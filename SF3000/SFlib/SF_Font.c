// 	File : SF_Font
//	Font management routines

// Includes

#include "SF_Font.h"
#include "SF_Allocation.h"
#include "SF_Screenutils.h"

// Global variables

TextCel			*cel_text [FONT_MAXMENU];						// Cels for Menu entries
FontDescriptor	*font_list [FONT_MAXAVAILABLE];					// Array of Font handles
long			font_handles;									// No. of fonts currently held

/**************************************/

long font_loadin(void)

// Purpose : Loads in all fonts on game start
// Accepts : pointer to array of font handle pointers
// Returns : 0 if failed to load, 1 if success

{

char	font_file [128];
char	*font_files [] = 	{
							"Message",
							""
							};
	
	while (*font_files [font_handles] != 0)						// Load in list
    	{
		
		sprintf (font_file, "%s%s%s", RESOURCES_ROOT, FONT_ROOT, font_files [font_handles]);
		*(font_list + font_handles) = LoadFont(font_file, MEMTYPE_ANY);	
		
		if (*(font_list + font_handles) == NULL)				// File failed to load ? Return
			return (0);
			
		font_handles++;
		}
	font_handles-=1;
	return (1);
}

/**************************************/

void font_initialise(TextCel** cel_text)

// Purpose : Creates empty text cels for menu and game display
// Accepts : Pointer to font list, Pointer to cel_text list
// Returns : Nothing

{
long	font_loop;

	// Create menu entry cels
	
	for (font_loop=0; font_loop < FONT_MAXMENU; font_loop++)
		*(cel_text + font_loop) = CreateTextCel(*(font_list + MENU_FONT), 0,0,0);
	
	// Link menu cels together
	
	for (font_loop=0; font_loop < (FONT_MAXMENU-1); font_loop++)
		((*(cel_text+font_loop))->tc_CCB)->ccb_NextPtr = (*(cel_text+font_loop+1))->tc_CCB;
	
	// Create message cels
				
	for (font_loop=0; font_loop < FONT_MAXMESSAGE; font_loop++)
		messages [font_loop].textcel = CreateTextCel (*(font_list + MESSAGE_FONT), 0,0,0);

	// Link message cels together
	
	for (font_loop=0; font_loop < (FONT_MAXMESSAGE-1); font_loop++)
		{
		(messages[font_loop].textcel->tc_CCB)->ccb_NextPtr = messages[font_loop+1].textcel->tc_CCB;
		(messages[font_loop].textcel->tc_CCB)->ccb_Flags &= ~CCB_LAST;
		}
		
	(messages[FONT_MAXMESSAGE-1].textcel->tc_CCB)->ccb_Flags |= CCB_LAST;
		
}

/**************************************/

void font_terminate (void)

// Purpose : Unloads all fonts & free's memory
// Accepts : Pointer to font list, number of fonts currently loaded
// Returns : Nothing

{
long	font_loop;

	for (font_loop=0; font_loop < FONT_MAXMESSAGE; font_loop++)		// Delete message text cels
		DeleteTextCel (messages [font_loop].textcel);
		
	for (font_loop=0; font_loop < FONT_MAXMENU; font_loop++)		// Delete menu entry text cels
		DeleteTextCel (*(cel_text + font_loop));
	
	for (font_loop=font_handles-1; font_loop>=0; font_loop--)		// Delete fonts
		UnloadFont (*(font_list + font_loop));
	
		
}

/**************************************/

void font_paint(long font_bc, long font_fc, long font_xp, long font_yp, long font_num, long screen_type, char *font_str)
				
// Purpose : Paints a font immediately on the screen using GCON - THIS IS VERY SLOW
// Accepts : Stuff
// Returns : Nothing

{
GrafCon	gcon;
	
	gcon.gc_BGPen=font_bc;
	gcon.gc_FGPen=font_fc;
	gcon.gc_PenX=font_xp;
	gcon.gc_PenY=font_yp;

	if (screen_type == SCR_UPDATE)	
		DrawTextString(*(font_list + font_num), &gcon, screen->sc_BitmapItems[screen->sc_curScreen],font_str);
	else
		DrawTextString(*(font_list + font_num), &gcon, screen->sc_BitmapItems[1-screen->sc_curScreen],font_str);
}
