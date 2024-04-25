//	File : SF_Screen_Utils
//	General screen utilities


// Includes

#include "SF_Screenutils.h"
#include "SF_ARMSky.h"
#include "SF_ARMCell.h"


// Defines

#define DISPLAY_FRAMERATE	3				// Max speed of frame update


// Global variables

long screen_swaptype;


/**************************************/

void screen_base (long updateflags)
{

// Purpose : Optionally prints image backdrop or SPORT clears screen
// Accepts : Update flags
// Returns : nothing
	
	// WAIT FOR FRAME DELAY ?

	if (updateflags & S_WAIT)
		WaitIO (VBLIOReq);
		
// DRAW BACKDROP IMAGE ?

	if (updateflags & S_IMAGE) 
		CopyVRAMPages(	VRAMIOReq, (screen->sc_Bitmaps [screen->sc_curScreen])->bm_Buffer,
						backdrop, screen->sc_nFrameBufferPages, -1 );
	
	if (updateflags & S_FLASH)
		SetVRAMPages(	VRAMIOReq, (screen->sc_Bitmaps [screen->sc_curScreen])->bm_Buffer,
						0, screen->sc_nFrameBufferPages, -1);
}

/**************************************/

void screen_update (long updateflags)
{

// Purpose : Screen display, optionally prints cel list, menu etc. & flips screens
// Accepts : Update flags
// Returns : nothing

long			temp_cels;

// WAIT FOR FRAME DELAY ?

	if ((updateflags & S_WAIT) || (updateflags & S_WAITFRAME))
		WaitIO (VBLIOReq);
			
// DRAW CEL_PLOTLIST ?
						
	if ((updateflags & S_CEL) && (cel_quad.temp_cels >0))
		{
		temp_cels=cel_quad.temp_cels-1;
		
		(*(cel_plotlist + temp_cels))->ccb_Flags |= CCB_LAST;
		DrawCels(screen->sc_BitmapItems[screen->sc_curScreen], *cel_plotlist);
		(*(cel_plotlist + temp_cels))->ccb_Flags &= ~CCB_LAST;
		}

// DRAW MENU ?

	if ((updateflags & S_MENU) && menu_status.hidden == 0)
		DrawCels(screen->sc_BitmapItems[screen->sc_curScreen], menu_status.cel_textccb);

// DRAW GAME INFO TEXT ? (If Also drawing menu, skip first 3 (Score, Weapon Count & Mission Timer)

	if (updateflags & S_INFO)
		{
		if (updateflags & S_MENU)
			DrawCels(screen->sc_BitmapItems[screen->sc_curScreen], messages[3].textcel->tc_CCB);
		else
			DrawCels(screen->sc_BitmapItems[screen->sc_curScreen], messages[0].textcel->tc_CCB);
		}

// FLIP SCREEN BANKS ?

	if (updateflags & S_BANK)
		{
		if (screen_swaptype != 0)
			screen_swap (screen_swaptype);								// Do pre-flip screenswap ?
				
		DisplayScreen(screen->sc_Screens[screen->sc_curScreen],0);		// Swap screen
		screen->sc_curScreen=1-screen->sc_curScreen;					// Swap screen
		
		if (screen_swaptype != 0)
			screen_postswap (screen_swaptype);							// Do post-flip screenswap ?
		
		}
				
// RESTART NEXT DELAY ?

	if (updateflags & S_WAIT)
		WaitVBLDefer(VBLIOReq, DISPLAY_FRAMERATE);
	
	if (updateflags & S_WAITFRAME)
		WaitVBLDefer(VBLIOReq, 1);
}


/**************************************/

void	screen_intercept (void)
{

// Purpose : Routine to draw cels and clear list (if temp list becomes full)
// Accepts : Nothing
// Returns : Nothing

long	temp_cels;

	// WAIT FOR DELAY
	
	WaitIO (VBLIOReq);
	
	// DRAW CURRENT (FULL) CEL LIST, RESET FOR NEXT BATCH
	
	if (cel_quad.temp_cels >0)
		{

		temp_cels=cel_quad.temp_cels-1;
		
		(*(cel_plotlist + temp_cels))->ccb_Flags |= CCB_LAST;
		DrawCels(screen->sc_BitmapItems[screen->sc_curScreen], *cel_plotlist);
		(*(cel_plotlist + temp_cels))->ccb_Flags &= ~CCB_LAST;
		
		cel_quad.temp_cels =0;
		}
}

/**************************************/

void screen_clear (long red, long green, long blue, long screen_type)

// Purpose : Clears the screen
// Accepts : Screen, RGB colour
// Returns : Nothing

{
	
Rect rectangle;
GrafCon GCon;
	
	SetFGPen( &GCon, MakeRGB15(red,green,blue));
		
	rectangle.rect_XLeft = 0;
	rectangle.rect_XRight = DISPLAY_WIDTH;
	rectangle.rect_YTop = 0;
	rectangle.rect_YBottom = DISPLAY_HEIGHT;
	
	if (screen_type == SCR_UPDATE)	
		FillRect(screen->sc_BitmapItems[screen->sc_curScreen ], &GCon, &rectangle );
	else
		FillRect(screen->sc_BitmapItems[1-screen->sc_curScreen], &GCon, &rectangle );
}

/**************************************/

void	screen_swap (long swap_type)

// Purpose : Calls relavent screenswap routine
// Accepts : Screenswap type
// Returns : Nothing

{

	switch (swap_type)
		{
		case 1 :	FadeToBlack(screen, 20);	// SCREENSWAP 1 ?
					break;
		case 2 : 	screenswap_blurscreen();	// SCREENSWAP 2 ?
					break;
		case 3 : 	screenswap_zoominbox();		// SCREENSWAP 3 ?
					break;
		}
}

/**************************************/

void	screen_postswap (long swap_type)

// Purpose : Calls relavent screenswap routine (after banks flipped)
// Accepts : Screenswap type
// Returns : Nothing

{

	switch (swap_type)
		{
		case 1 :	FadeFromBlack(screen, 20);	// SCREENSWAP 1 ?
					break;
		}
	
	screen_swaptype = 0;						// Clear global next-swaptype flag
}

/**************************************/

void screen_setswap (long swap_type)

// Purpose : Sets next type of screen swap when banks are flipped
// Accepts : Screenswap type
// Returns : Nothing

{
	screen_swaptype = swap_type;				// Set global next-swaptype flag
}


/**************************************/

void	screenswap_zoominbox (void)

// Purpose : SCREENSWAP1 - Zooms screen in a box
// Accepts : Nothing
// Returns : Nothing

{

CCB		screen_fade;
long	fade_loop,
		fade_xloop,
		fade_yloop;

	screen_fade.ccb_Flags =	CCB_LAST |			// This is the only cel
							CCB_SPABS |			// Absolute SOURCEPTR
							CCB_LDSIZE |		// Load in HDX, HDY, VDX, VDY
							CCB_LDPRS |			// Load in HDDX, HDDY
							(1<<24) |			// Load in PIXC word
							CCB_YOXY |			// Set x&y position
							CCB_ACW |			// Enable clockwise pixel rendering
							CCB_ACCW |			// Enable counter clockwise pixel rendering
							CCB_ACE |			// Enable 2nd cel engine
							CCB_CCBPRE |		// Preamble words in CCB
							(1<<8) |			// Use PMode 0
							CCB_USEAV |			// Use AV control bits
							CCB_BGND;			// pass 000rgb as colour - no transp
	
	
	screen_fade.ccb_HDY=0;
	screen_fade.ccb_VDX=0;
	screen_fade.ccb_HDDX=0;
	screen_fade.ccb_HDDY=0;	
	
	screen_fade.ccb_PRE0 =	(119<<6) |			// 120 pixels high !
							(1<<4) |			// uncoded cel
							6;					// 16bpp		
	
	screen_fade.ccb_PRE1 =	(318<<16) |			// 160 words long
							(1<<11)	|			// left / right memory format
							319;				// 320 Pixel per row
	

	screen_fade.ccb_SourcePtr = (CelData*) ((screen->sc_Bitmaps [screen->sc_curScreen]) -> bm_Buffer);
	for (fade_loop=0; fade_loop < 170; fade_loop+=8)
		{
		
		if (fade_loop <=160)
			fade_xloop = fade_loop;
		else
			fade_xloop = 160;
			
		fade_yloop=(long) ((fade_xloop*120) / 160);
		
		screen_fade.ccb_PIXC =	(((long) (fade_loop/23)) <<10) |			// PMV is 8
								(3<<8) |									// PDV is 8
								(1<<7) |									// 2nd source is frame buffer
								1;											// 2DV is 2
							
		screen_fade.ccb_XPos=(160-fade_xloop)<<16;
		screen_fade.ccb_YPos=(120-fade_yloop)<<16;
		screen_fade.ccb_HDX=(fade_xloop<<20)/160;
		screen_fade.ccb_VDY=(fade_yloop<<16)/120;
		DrawCels(screen->sc_BitmapItems[1-(screen->sc_curScreen)], &screen_fade);
		}
}


/**************************************/

void	screenswap_blurscreen (void)

// Purpose : SCREENSWAP2 - Blurs 2 screens together
// Accepts : Nothing
// Returns : Nothing

{

CCB		screen_fade;

	screen_fade.ccb_Flags =	CCB_LAST |			// This is the only cel
							CCB_SPABS |			// Absolute SOURCEPTR
							CCB_LDSIZE |		// Load in HDX, HDY, VDX, VDY
							CCB_LDPRS |			// Load in HDDX, HDDY
							(1<<24) |			// Load in PIXC word
							CCB_YOXY |			// Set x&y position
							CCB_ACW |			// Enable clockwise pixel rendering
							CCB_ACCW |			// Enable counter clockwise pixel rendering
							CCB_ACE |			// Enable 2nd cel engine
							CCB_CCBPRE |		// Preamble words in CCB
							(1<<8) |			// Use PMode 0
							CCB_USEAV |			// Use AV control bits
							CCB_BGND;			// pass 000rgb as colour - no transp
	
	screen_fade.ccb_SourcePtr = (CelData*) ((screen->sc_Bitmaps [1-screen->sc_curScreen]) -> bm_Buffer);
	screen_fade.ccb_HDY=0;
	screen_fade.ccb_VDX=0;
	screen_fade.ccb_HDDX=0;
	screen_fade.ccb_HDDY=0;	
	screen_fade.ccb_XPos=0;
	screen_fade.ccb_YPos=0;
	screen_fade.ccb_HDX=(1<<20);
	screen_fade.ccb_VDY=(1<<16);
	
	screen_fade.ccb_PRE0 =	(119<<6) |			// 120 pixels high !
							(1<<4) |			// uncoded cel
							6;					// 16bpp		
	screen_fade.ccb_PRE1 =	(318<<16) |			// 160 words long
							(1<<11)	|			// left / right memory format
							319;				// 320 Pixel per row
	
	screen_fade.ccb_PIXC =	(5<<10) |			// PMV is 3
							(3<<8) |			// PDV is 8
							(1<<7) |			// 2nd source is frame buffer
							0;					// 2DV is 2
							
	DrawCels(screen->sc_BitmapItems[screen->sc_curScreen], &screen_fade);
}


