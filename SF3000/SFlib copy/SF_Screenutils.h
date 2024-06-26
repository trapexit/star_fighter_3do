/* File : Screen_Utils header file */

#ifndef __SF_SCREENUTILS
#define __SF_SCREENUTILS


// Includes

#include "graphics.h"
#include "Utils3DO.h"
#include "SF_Celutils.h"
#include "SF_Menu.h"
#include "SF_Message.h"

// Defines

#define DISPLAY_WIDTH		320 			// Global Screen Sizes
#define DISPLAY_HEIGHT		240

#define S_CEL		1						// Screen update options
#define S_BANK		2
#define S_IMAGE		4
#define S_MENU		8
#define	S_INFO		16
#define	S_WAIT		32
#define	S_FLASH 	64
#define	S_WAITFRAME	128

#define SCR_UPDATE	0						// Screen bank specifiers
#define SCR_DISPLAY 1


/* External Variables */

extern	ScreenContext*	screen;
extern	ubyte*			backdrop;
extern	Item			VBLIOReq;
extern	Item			VRAMIOReq;
extern	cel_celdata		cel_quad;
extern	menu_info		menu_status;
extern	menu			menus [MAX_MENUS];
extern	CCB*			cel_plotlist [CEL_MAXTEMP];
extern	char*			cel_plutdata;
extern	message			messages [FONT_MAXMESSAGE];	
extern	long			info;

/* Function Prototypes */

void	screen_base (long updateflags);									// Draw base screen backdrops
void 	screen_update (long);											// Draw rest of screen, flip etc.
void	screen_intercept (void);										// Draw temp plotting list if full
void 	screen_clear (long, long, long, long);							// Clear screen
void	screen_swap (long);												// Fade screenbanks
void	screen_postswap (long);											// Fade screenbanks after flip
void	screen_setswap (long);											// Set next type of screen swap
void	screenswap_zoominbox (void);									// SCREENSWAP ROUTINE 1
void	screenswap_blurscreen (void);									// SCREENSWAP ROUTINE 2

#endif
