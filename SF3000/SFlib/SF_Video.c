//	File : SF_Video
//	Video playing routines (just simple interface for platform swapping)

#include "SF_Video.h"
#include "SF_Allocation.h"
#include "SF_Io.h"
#include "SF_Music.h"
#include "SF_Sound.h"
#include "SF_ScreenUtils.h"

#include "umemory.h"
#include "Stdio.h"

/**************************************/

long	video_play (char *video_file)

// Purpose : Plays a video, terminating on user keypad press or end of movie
// Accepts : Pointer to movie file & Pointer to screen details
// Returns : (1) if correct keys were pressed to terminate player, or (0) if movie just finished

// NOTE : MUSIC & SAMPLE PLAYERS ARE TERMINATED TO FREE RESOURCES FOR DURATION OF VIDEO

{
long	status;
char	filename [128];
		
	// TERMINATE THE MUSIC/SAMPLE PLAYER
	
	music_pause ();
	sound_terminate ();
	
	// CLEAR BOTH SCREENBANKS
	
	screen_clear (0, 0, 0, SCR_UPDATE);
	screen_clear (0, 0, 0, SCR_DISPLAY);

	// STOP TIMEOUT TO VIDEO
	io_resetvideocounter (-1);
	
	// CREATE FULL PATHNAME & PLAY THE MOVIE FILE
	
	sprintf(filename,"%s%s%s",RESOURCES_ROOT, VIDEO_ROOT, video_file);
	status = PlayCPakStream (screen, filename, (PlayCPakUserFn) video_internal_interrupt, NULL );
	
	// DISPLAY CORRECT SCREENBANK AGAIN

	DisplayScreen(screen->sc_Screens[1- screen->sc_curScreen],0);
	
	// RESET TIMEOUT TO VIDEO & CLEAR NEXT SCREEN
	io_resetvideocounter (MAX_WAITBEFOREVIDEO);
	
	// REACTIVATE THE MUSIC/SAMPLE PLAYER & RETURN STATUS
	
	sound_initialise ();
	music_resume ();
	
	return (status);
}

/**************************************/

long video_internal_interrupt (PlayerPtr ctx )

// Purpose : Called by Cinepak main loop - Checks joypad & returns a 0 (no event) or 1 (Causes player to stop)
// Accepts : Pointer to Cinepak player
// Returns : (1) if correct keys were pressed to terminate player, or (0) if not

{

long	joybits;

	joybits = controlpad_read();		// Read joypad
	return ((joybits == 0) ? 0 : 1);	// Return Continue / Terminate flag
}
