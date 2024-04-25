// File : 		SF_Music.c
// Purpose : 	Music Playing Routines

// Alter music filenames in *music_tracks array. Code assumes all music files are in the same
// Directory, (MUSIC_ROOT) this is defined in"SF_Allocation.h"

// Music commands are :	music_initialise ()				// MUST BE CALLED 1st TO INITIALISE PLAYER
														
//						music_play ()					// play a track
//						music_stop ()					// stop a track
//						music_pause ()					// pause track
//						music_resume ()					// resume track
//						music_terminate ()				// kill task

/**************************************/

#include "types.h"
#include "audio.h"
#include "debug3do.h"
#include "soundplayer.h"
#include "string.h"
#include "SF_Allocation.h"
#include "SF_Music.h"
#include "varargs.h"
#include "types.h"

/***************************************************************************************************/

#define	MUSIC_OUTPUTINSTRUMENT "directout.dsp"			// Type of output instrument

long	MUSIC_SAMPLERINSTRUMENTTYPE;
char	*MUSIC_SAMPLERINSTRUMENTS [2] = 	{
											"dcsqxdstereo.dsp",		// Sample player 1 (2:1 SQXD AIFC & 41100 hz) STEREO
											"dcsqxdmono.dsp"		// Sample player 2 (2:1 SQXD AIFC & 41100 hz) MONO
											};

char	*music_tracks [] =	{	"Obie130.aifc",			// TRACK LIST
								"Floating.aifc",
								"DeathBS.aifc",
								"PlanetMa.aifc",
								"Star1.aifc",
								"Starhed.aifc", 
								"FreeFall.aifc"
							};

/***************************************************************************************************/

// Music Status Definitions (Combinations of these can be set, ie. REPEAT + PLAY)

#define	PLAYER_PLAYING 			0x1					// Music Status Definitions (Combinations of these can be set, ie. REPEAT + PLAY)
#define	PLAYER_STOPPED 			0x2
#define	PLAYER_TERMINATED 		0x4

/**************************************/

Item		music_taskref;							// Task ref of player

SPPlayer	*music_player = NULL;					// Sample player
SPSound		*music_sample = NULL;					// Sample ref

int32		MUSIC_SIGNALPARENT;						// SIGNAL FROM TASK TO PARENT

int32		MUSIC_PLAY,								// SIGNALS FOR THE MUSIC PLAYING THREAD TO USE					
			MUSIC_REPEAT,
			MUSIC_STOP,
			MUSIC_TERMINATE,
			MUSIC_PAUSE,
			MUSIC_RESUME,
			MUSIC_COMMAND,
			MUSIC_MAKETIME,
			MUSIC_PLAYVOICE;
			
long		MUSIC_PLAYERTASKACTIVE=0,				// Current status of player
			MUSIC_MAKETIMEFLAG=0,					// Flag to wait for buffer fill ?
			MUSIC_TRACK,							// Track to play
			MUSIC_MASTERVOL=128,					// Music volume
			MUSIC_TRACKSLEFT,						// Tracks left to play
			player_status;							// Music player status

char		MUSIC_PROGRAM [MUSIC_PLAYLISTLENGTH];	// Music play list
char		MUSIC_PROGRAMLENGTH;					// Number of tracks in play list
char		MUSIC_VOICEOVERTOPLAY [128];			// Filename of voice over to play
long		MUSIC_BUFFERS,							// Memory / Buffer Allocation
			MUSIC_BUFFERSIZE;

Item		musicsampler_volume;					// Knob to set volume of sample playback
Item		music_sampler=0;						// Music sample player
Item		music_output=0;							// Music output instrument

/****************************************************************************************************
*									PARENT TASK CONTROL ROUTINES									*
****************************************************************************************************/

long music_initialise (long	sampleinstrumenttoplay)

// Purpose : Initialises music player task
// Accepts : nothing
// Returns : 1 for success, 0 for failure

{

	// Check to see if player is already running, if so, just quit
	
	if (MUSIC_PLAYERTASKACTIVE == 1)
		return (1);
	
	// Set instrument type to play
	
	MUSIC_SAMPLERINSTRUMENTTYPE = sampleinstrumenttoplay;
	
	// Initialise variables
	
	MUSIC_BUFFERS = 2;								// Number of buffers [2]
	MUSIC_BUFFERSIZE = 32 * 2048;					// Buffer size [32 blocks of 2k each]
	MUSIC_SIGNALPARENT = AllocSignal (0);			// Allocate READY signal
	
	// Start player running
	
	music_taskref = CreateThread("music_player", 101, music_thread, 4096);
	
	// Wait for ready signal (if not failed)
	
	if (music_taskref >= 0)
		WaitSignal (MUSIC_SIGNALPARENT);
	
	// Deallocate signal & return success / failure flag
	
	FreeSignal (MUSIC_SIGNALPARENT);
	if (music_taskref >0)
		return (1);
	else
		return (0);
}

/**************************************/

void music_terminate (void)

// Purpose : Kills music player thread
// Accepts : nothing
// Returns : nothing

{
	// Only kill player if it is active
	
	if (MUSIC_PLAYERTASKACTIVE == 1)
		{
		MUSIC_SIGNALPARENT = AllocSignal (0);			// Allocate KILLED signal
		SendSignal(music_taskref, MUSIC_TERMINATE);		// Kill player
		WaitSignal (MUSIC_SIGNALPARENT);				// Wait for player to signal that it's thrown a 7
		FreeSignal (MUSIC_SIGNALPARENT);				// Deallocate signal
		}
}

/****************************************************************************************************
*										MUSIC CONTROL ROUTINES										*
****************************************************************************************************/

void music_maketime (void)

// Purpose : Waits until just after buffer full to make most loading time
// Accepts : nothing
// Returns : nothing

{
	// Only wait for message if active 
	
	if (MUSIC_PLAYERTASKACTIVE == 1)
		{
		MUSIC_SIGNALPARENT = AllocSignal (0);			// Allocate MAKETIME signal
		SendSignal(music_taskref, MUSIC_MAKETIME);		// Make it wait for signal
		WaitSignal (MUSIC_SIGNALPARENT);				// Wait for player to signal that it's thrown a 7
		FreeSignal (MUSIC_SIGNALPARENT);				// Deallocate signal
		}
}

/**************************************/

void music_resetplaylist (void)

// Purpose : Clears music play list
// Accepts : nothing
// Returns : nothing

{
	MUSIC_PROGRAMLENGTH = 0;
}

/**************************************/

void music_addtoplaylist (long track_to_add)

// Purpose : Adds a track to the music play list (if it's not already in the list)
// Accepts : Track to add
// Returns : Nothing

{
long	track_found = -1,
		loop;

	for (loop = 0; loop < MUSIC_PROGRAMLENGTH; loop++)
		if (MUSIC_PROGRAM [loop] == track_to_add)
			track_found = loop;
			
	if (track_found == -1)
		{
		if (MUSIC_PROGRAMLENGTH < MUSIC_PLAYLISTLENGTH)
			MUSIC_PROGRAM [MUSIC_PROGRAMLENGTH++] = (char) track_to_add;
		}
}

/**************************************/

void music_takefromplaylist (long track_to_takeout)

// Purpose : Takes a track out of the playlist
// Accepts : track to delete
// Returns : nothing

{
long	track_found = -1,
		track_shift = 0,
		loop;

	// Check that there are some tracks in the play list !
	
	if (MUSIC_PROGRAMLENGTH <= 0)
		return;
		
	// Get current position of track in playlist
	
	for (loop = 0; loop < MUSIC_PROGRAMLENGTH; loop++)
		if (MUSIC_PROGRAM [loop] == track_to_takeout)
			track_found = loop;
	
	// Couldn't find track ? If so, return		
	
	if (track_found == -1)
		return;
	
	// Take track out
	
	for (loop = 0; loop < MUSIC_PROGRAMLENGTH; loop++)
	if (loop != track_found)
		{
		MUSIC_PROGRAM [track_shift] = MUSIC_PROGRAM [loop];
		track_shift++;
		}
	
	MUSIC_PROGRAMLENGTH-=1;
	
	// Have we deleted the one we were listening to ? If so, play next (same position in list)
	
	if (MUSIC_TRACK == track_found)
		music_play (MUSIC_MOVETRACK, 0, MUSIC_TRACKSLEFT);
		
	// Have we deleted a track prior to the one we are listening to ? If so, take 1 of playtrack number
	
	if (MUSIC_TRACK > track_found)
		MUSIC_TRACK-=1;
}

/**************************************/

void music_play (long music_setormove, long music_trackval, long trackstoplay)

// Purpose : Plays track list with optional repeat
// Accepts : Type of play (MOVE TRACK, OR PLAY FROM TRACK), Track value (START FROM OR MOVE BY)
// Returns : Nothing

{

	if (MUSIC_PROGRAMLENGTH > 0)							// Any tracks to play ?
		{
		if (music_setormove == MUSIC_PLAYFROMTRACK)			// ARE WE PLAYING FROM A SPECIFIED TRACK ?
			MUSIC_TRACK = music_trackval;					// Set current track to 1st in list
		else
			MUSIC_TRACK += music_trackval;
		
		MUSIC_TRACKSLEFT = trackstoplay;					// Set number of tracks left to play
		
		SendSignal(music_taskref, MUSIC_PLAY);				// Play tracks
		}
	else
		SendSignal(music_taskref, MUSIC_STOP);				// Stop if none
}

/**************************************/

void music_stop (void)

// Purpose : Stops a track from playing
// Accepts : nothing
// Returns : nothing

{
	if (player_status & PLAYER_PLAYING)
		SendSignal(music_taskref, MUSIC_STOP);				// Stop track playing
}

/**************************************/

void music_pause (void)

// Purpose : Pauses a track
// Accepts : nothing
// Returns : nothing

{
	if (player_status & PLAYER_PLAYING)
		SendSignal(music_taskref, MUSIC_PAUSE);			// Pause Track
}

/**************************************/

void music_resume (void)

// Purpose : Resumes a track if paused
// Accepts : nothing
// Returns : nothing

{
	if (player_status & PLAYER_PLAYING)
		SendSignal(music_taskref, MUSIC_RESUME);		// Resume Track
}


/**************************************/

long music_query (void)

// Purpose : Returns current player status
// Accepts : nothing
// Returns : current track being played (or -1 for no track)

{	
	if (player_status & PLAYER_PLAYING && MUSIC_TRACK >=0 && MUSIC_TRACK < MUSIC_PROGRAMLENGTH)
		return (MUSIC_PROGRAM [MUSIC_TRACK]);

	return (-1);
}

/**************************************/

void music_setmastervolume (long new_volume)

// Purpose : Sets new playback volume for music
// Accepts : new volume
// Returns : nothing

{
	if (MUSIC_PLAYERTASKACTIVE == 1)
		{
		MUSIC_MASTERVOL = new_volume;
		TweakKnob (musicsampler_volume, MUSIC_MASTERVOL<<8);
		}
}

/****************************************************************************************************
*									VOICE OVER CONTROL ROUTINES										*
****************************************************************************************************/

void music_playvoice (char *fmt,...)

// Purpose : Starts / Stops streaming of voice over
// Accepts : aifc filename as variable length arg list
// Returns : nothing

{
	
va_list		variable_args;

	va_start(variable_args, fmt);
	vsprintf(MUSIC_VOICEOVERTOPLAY, fmt, variable_args)
	va_end(variable_args);
	MUSIC_TRACKSLEFT = 0;
	SendSignal(music_taskref, MUSIC_PLAYVOICE);
}

/****************************************************************************************************
*										MUSIC PLAYER (THREAD)										*
****************************************************************************************************/

void music_thread (void)

// Purpose : Music playing task
// Accepts : nothing
// Returns : nothing

{

int32		music_playersignal,
			music_signal,
			result;
	
// OPEN AUDIO FOLIO
	
	result = OpenAudioFolio();
	if (result < 0)
		return;
		
// LOAD INSTRUMENTS

    music_sampler = LoadInstrument (MUSIC_SAMPLERINSTRUMENTS [MUSIC_SAMPLERINSTRUMENTTYPE], 0, 100);	// Load sample player
	musicsampler_volume = GrabKnob (music_sampler, "Amplitude");										// Grab volume knob for sample player
	music_output = LoadInstrument (MUSIC_OUTPUTINSTRUMENT, 0, 100);										// Load output instrument
		
// CONNECT AND START INSTRUMENTS (DEPENDS ON SAMPLE PLAYER TYPE)

	if (MUSIC_SAMPLERINSTRUMENTTYPE == 0)
		{
		ConnectInstruments (music_sampler, "LeftOutput",  music_output, "InputLeft");
		ConnectInstruments (music_sampler, "RightOutput", music_output, "InputRight");
		}
	else
		{
		ConnectInstruments (music_sampler, "Output", music_output, "InputLeft");
		ConnectInstruments (music_sampler, "Output", music_output, "InputRight");
		}
		
	StartInstrument (music_output, NULL);
	
// SET UP A SOUND PLAYER

    spCreatePlayer (&music_player, music_sampler, MUSIC_BUFFERS, MUSIC_BUFFERSIZE, NULL);	// Create sample player
	music_playersignal = spGetPlayerSignalMask (music_player);								// Get wait signal for player


// INITIALISE TASK FLAGS
	
	MUSIC_PLAY 		= AllocSignal (0);					// Play music flag
	MUSIC_PAUSE		= AllocSignal (0);					// Pause music
	MUSIC_RESUME	= AllocSignal (0);					// Resume music
	MUSIC_STOP 		= AllocSignal (0);					// Stop music flag
	MUSIC_TERMINATE	= AllocSignal (0);					// Terminate task
	MUSIC_MAKETIME  = AllocSignal (0);					// Wait for buffer fill
	MUSIC_PLAYVOICE = AllocSignal (0);					// Play a voice over
	
	MUSIC_COMMAND = MUSIC_PLAY | MUSIC_PAUSE | MUSIC_RESUME | MUSIC_STOP | MUSIC_TERMINATE | MUSIC_MAKETIME | MUSIC_PLAYVOICE;

	MUSIC_PLAYERTASKACTIVE = 1;							// Set player flag to ACTIVE
	player_status = PLAYER_STOPPED;						// Set player status to stopped
	
	SendSignal (parent_taskref, MUSIC_SIGNALPARENT);	// Indicate to parent that we have setup
	
/****************************************************************************************************
*										Main Control Loop											*
****************************************************************************************************/

	do
		{
		
		// WAIT FOR COMMAND (PLAY, TERMINATE, MAKETIME OR PLAY VOICE)
		
		music_signal = WaitSignal (MUSIC_MAKETIME | MUSIC_PLAY | MUSIC_TERMINATE | MUSIC_PLAYVOICE);
		music_command (music_signal);
				
		// MAIN SAMPLE PLAYING LOOP

		while (player_status & PLAYER_PLAYING)
			{
			
			// WAIT FOR SIGNAL (EITHER FROM PLAYER, OR PARENT TASK), THEN CALL MUSIC COMMAND TO SEE IF IT'S A CONTROL COMMAND
			
			music_signal = WaitSignal (music_playersignal | MUSIC_COMMAND);
			if (music_signal & MUSIC_COMMAND)
				music_command (music_signal);

			// IF COMMAND TO SERVICE SOUND PLAYER, THEN UPDATE IT & CHECK SPOOLERS STATUS
			
			if ((player_status & PLAYER_PLAYING) && (music_signal & music_playersignal))
				{	
				spService (music_player, music_signal);									// Update spooler
				
				if (MUSIC_MAKETIMEFLAG == 1)											// Is parent waiting to load on buffer fill ?
					{																	// if so, signal them
					MUSIC_MAKETIMEFLAG = 0;												// Clear flag
					SendSignal (parent_taskref, MUSIC_SIGNALPARENT);					// Let parent carry on
					}
				
				if (!(spGetPlayerStatus(music_player) & SP_STATUS_F_BUFFER_ACTIVE))		// Has player finished track ?
					{
					spStop(music_player);												// (Music finished) - 
					spRemoveSound (music_sample);										// (Music finished) - Unload sound
					music_trackgetnext (1);												// (Music finished) - Get next track
					}
				}
			}
		}
		
	while (player_status != PLAYER_TERMINATED);										// Until player is terminated

	// TERMINATION OF MUSIC PLAYER
	
	spDeletePlayer (music_player);														// Delete music spooler
	ReleaseKnob (musicsampler_volume);													// Delete volume knob
	DisconnectInstruments (music_sampler, "LeftOutput", music_output, "InputLeft");		// Disconnect instruments
	DisconnectInstruments (music_sampler, "RightOutput", music_output, "InputRight");	// Disconnect instruments
	StopInstrument (music_output, NULL);												// Stop output instrument
	UnloadInstrument (music_output);													// Delete music output instrument
	UnloadInstrument (music_sampler);													// Delete sample player
	CloseAudioFolio();																	// Close audio folio
	FreeSignal(MUSIC_COMMAND);															// Free signals

	MUSIC_PLAYERTASKACTIVE = 0;															// Set player flag to INACTIVE
	SendSignal (parent_taskref, MUSIC_SIGNALPARENT);									// Indicate to parent that we have terminated
	return;
}

/**************************************/

void music_trackgetnext (long track_mover)

// Purpose : Moves music track pointer & plays track
// Accepts : Ammount to move pointer by
// Returns : Player status (Just PLAYING or STOPPED)

{	
	if (MUSIC_PROGRAMLENGTH <= 0 || MUSIC_TRACKSLEFT == 0)	// If no tracks in play list, return
		{
		player_status = PLAYER_STOPPED;
		return;
		}
	
	if (MUSIC_TRACKSLEFT > 0)						// Take 1 off number of tracks left to play (if >0)
		MUSIC_TRACKSLEFT-=1;
		
	if (!(player_status & PLAYER_PLAYING))			// If player set to not playing, return
		{
		return;
		}
		
	MUSIC_TRACK += track_mover;						// Move track pointer
	
	while (MUSIC_TRACK < 0)							// If track pointer before start of list, wrap around from end
		{
		MUSIC_TRACK+=MUSIC_PROGRAMLENGTH;
		}
		
	while (MUSIC_TRACK >= MUSIC_PROGRAMLENGTH)		// If track pointer after end of list, wrap around from start
		{
		MUSIC_TRACK-=MUSIC_PROGRAMLENGTH;
		}
		
	music_loadin (0);								// Load in next track if we haven't stopped player
	return;
}


/**************************************/

void music_loadin (long play_voiceover)

// Purpose : Loads in a track from the music play list and starts it playing
// Accepts : Flag as to whether to play track (0) or voice over (1)
// Returns : nothing, sets player status to playing

{

char	music_file [128];
char	music_loadtrack = MUSIC_PROGRAM [MUSIC_TRACK];
int32	result;

// SET VOLUME FOR PLAY

	TweakKnob (musicsampler_volume, MUSIC_MASTERVOL<<8);
	
// LOAD SAMPLE READY FOR PLAYING

	if (play_voiceover == 0)
		sprintf (music_file, "%s%s%s", RESOURCES_ROOT, MUSIC_ROOT, music_tracks [music_loadtrack]);		// Set to play track ?
	else
		sprintf (music_file, "%s", MUSIC_VOICEOVERTOPLAY);												// Or voice over file ?
		
	result = spAddSoundFile (&music_sample, music_player, music_file);									// Creates sound
	
	if (result < 0)
		{
		printf("couldn't add sound file\n");
		return;
		}
		
// START READING AND PLAYING SOUND

	result = spStartReading (music_sample, SP_MARKER_NAME_BEGIN);				// Start reading
	result = spStartPlaying (music_player, NULL);								// Start playing
	
	return;
}

/**************************************/

void music_command (int32 command)

// Purpose : Accepts & processes commands from player [only if player active]
// Accepts : Signal command
// Returns : returns player status

{
long	result;

	if (MUSIC_PLAYERTASKACTIVE == 1)
		{
		
		//**************************************
		// STOP MUSIC OR TERMINATE MUSIC PLAYER ?
		//**************************************
		
		if (command & MUSIC_STOP || command & MUSIC_TERMINATE)								
			{
			if (player_status & PLAYER_PLAYING)												// Currently playing track ?
				{
				spStop(music_player);														// Yes - Stop sound playing
				spRemoveSound (music_sample);												// Yes - Remove sound
				}
			
			player_status = (command & MUSIC_STOP) ? PLAYER_STOPPED : PLAYER_TERMINATED;	// Set player status
			}
		
		//**************************************
		// PAUSE MUSIC ?
		//**************************************
		
		if (command & MUSIC_PAUSE)
			if (player_status & PLAYER_PLAYING)
				spPause (music_player);								// Pause music player

		//**************************************
		// RESUME MUSIC ?
		//**************************************
	
		if (command & MUSIC_RESUME)
			if (player_status & PLAYER_PLAYING)
				spResume (music_player);							// Resume music player
		
		//**********************************
		// PLAY MUSIC ?
		//**********************************
		
		if (command & MUSIC_PLAY)
			{											
			
			if (player_status & PLAYER_PLAYING)						// Is player currently playing ?
				{
				spStop(music_player);								// Yes - Stop sound playing
				spRemoveSound (music_sample);						// Yes - Remove sound
				}
			
			player_status = PLAYER_PLAYING;							// Set status to play
			music_trackgetnext (0);									// Start playing track
			}
				
		//**********************************
		// PLAY VOICE OVER ?
		//**********************************

		if (command & MUSIC_PLAYVOICE)
			{
			
			if (player_status & PLAYER_PLAYING)						// Is player currently playing ?
				{
				spStop (music_player);								// Yes - Stop sound playing
				spRemoveSound (music_sample);						// Yes - Remove sound
				}
			
			player_status = PLAYER_PLAYING;							// Set status to play
			music_loadin (1);										// Start playing voice over
			}
		}

	//*********************************************
	// DOES PARENT WANT TO GUARENTEE LOADING TIME ?
	//*********************************************
	
	if (command & MUSIC_MAKETIME)
		{
		result = spGetPlayerStatus (music_player);

		if ((player_status & PLAYER_PLAYING) && (result & SP_STATUS_F_PLAYING))		// Are we playing ?
			{
			if (result & SP_STATUS_F_PAUSED)
				SendSignal (parent_taskref, MUSIC_SIGNALPARENT);					// PAUSED ?
			else
				MUSIC_MAKETIMEFLAG = 1;												// PLAYING NORMALLY ?
			}
		else
			SendSignal (parent_taskref, MUSIC_SIGNALPARENT);						// NOT PLAYING ?
		}

}
