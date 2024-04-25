// File : SF_Sound
// Sound Player Routines

#include "SF_Sound.h"
#include "SF_Allocation.h"
#include "Audio.h"
#include "Stdio.h"

/**************************************/

// GLOBAL VARIABLES & DEFINITIONS

long	SAMPLER_INITIALISED = 0,							// Current status of sound player (1 = initialised & ready)
		SAMPLES_LOADED = 0,									// Current number of samples loaded
		SAMPLE_MASTERVOL = 128,								// Master volume level for SFX
		SAMPLE_MASTERSWITCH = 1;							// SFX are on
		
//	SAMPLE DEFINITIONS

#define MAX_CHANNELS		6								// Max number of channels allowed
#define MAX_SAMPLES			16								// Max number samples
#define	SOUND_MIXER			"$audio/dsp/mixer8x2.dsp"		// Name of mixer
#define SOUND_INSTRUMENT	"$audio/dsp/varmono8.dsp"		// Name of sample player 

//	KNOB REFERENCES (titter ye not)
							
static char *gainname_left [] = 
{
	"LeftGain0",
	"LeftGain1",
	"LeftGain2",
	"LeftGain3",
	"LeftGain4",
	"LeftGain5",
	"LeftGain6",
	"LeftGain7"
};

static char *gainname_right [] = 
{
	"RightGain0",
	"RightGain1",
	"RightGain2",
	"RightGain3",
	"RightGain4",
	"RightGain5",
	"RightGain6",
	"RightGain7"
};

static char *channelname [] =
{
	"Input0",
	"Input1",
	"Input2",
	"Input3",
	"Input4",
	"Input5",
	"Input6",
	"Input7"
};

//	ITEM DECLARATIONS

sample_info		samples [MAX_SAMPLES];			// Individual Samples
attachment_info	attachments [MAX_CHANNELS];		// Individual attachments

Item		mixer,								// Audio mixer instrument
			instruments [MAX_CHANNELS],			// Sample playing instruments
			mixer_leftgain [MAX_CHANNELS],		// Left gain knobs
			mixer_rightgain [MAX_CHANNELS];		// Right gain knobs


/************************************************************************************************************************************
*										       MAIN SOUND INIT / TERMINATE CONTROL CODE												*
************************************************************************************************************************************/

void	sound_loadsamples (void)

// Purpose : Loads in all game samples & mixer
// Accepts : Nothing
// Returns : Nothing

{
long		loop;
char		sample_file [64];
sample_list	sample_files [] = 	{
									{100,	"Laser.aiff"	},				// List of samples & priorities
									{100,	"Missile.aiff"	},
									{100,	"Thud.aiff"		},
									{150,	"Engine.aiff"	},
									{100,	"Beep.aiff"		},
									{100,	"Click.aiff"	},
									{100,	"Explosion.aiff"},
									{150,	"Wind.aiff"		},
									{100,	"Chime.aiff"	},
									{100,	"LockOn.aiff"	},
									{100,	"PickUp.aiff"	},
									{100,	"Incoming.aiff"	},
									{100,	"Alarm.aiff"	},
									{100,	"BeamLaser.aiff"},
									{0,		NULL			},
								};

									
	// LOAD IN AND INITIALISE THE MIXER, SET UP MIXER KNOBS
	
	mixer = LoadInstrument (SOUND_MIXER, 0, 100);							// Load in (x) channel mixer
	
	for (loop = 0; loop < MAX_CHANNELS; loop++)
		{
		mixer_leftgain [loop] = GrabKnob (mixer, gainname_left [loop]);		// Grab knobs
		mixer_rightgain [loop] = GrabKnob (mixer, gainname_right [loop]);
		}
		
	StartInstrument(mixer, NULL);											// Turn the mixer on
	
	// LOAD IN SAMPLES
	
	while (sample_files [SAMPLES_LOADED].name != NULL)						// Load in list
    	{
		
		sprintf (sample_file, "%s%s%s", RESOURCES_ROOT, SAMPLE_ROOT, sample_files [SAMPLES_LOADED].name);
		
		// LOAD INDIVIDUAL SAMPLES & STORE DATA

		samples [SAMPLES_LOADED].sample_item = LoadSample (sample_file);				// Load in laser sample & store item
		samples [SAMPLES_LOADED].priority  = sample_files [SAMPLES_LOADED].priority;	// Store priority of sample
		SAMPLES_LOADED++;
		}
}

/**************************************/

void	sound_unloadsamples (void)

// Purpose : Unloads all samples & mixer (for game termination)
// Accepts : Nothing
// Returns : Nothing

{
long	loop;

	// TERMINATE SOUND PLAYERS ACTIVE INSTRUMENTS & DISCONNECT ALL
	
	sound_terminate ();													// Terminate all instruments
	
	// UNLOAD SAMPLES
	
	for (loop=0; loop < SAMPLES_LOADED; loop++)
		UnloadSample (samples [loop].sample_item);						// Unload samples
	
	// DELETE MIXER KNOB HANDLES
	
	for (loop = 0; loop < MAX_CHANNELS; loop++)							// Delete mixer knobs
		{
		ReleaseKnob (mixer_leftgain [loop]);
		ReleaseKnob (mixer_rightgain [loop]);
		}
	
	UnloadInstrument (mixer);											// Delete mixer
}

/************************************************************************************************************************************
*										      SAMPLE PLAYING INIT / TERMINATE CONTROL CODE											*
************************************************************************************************************************************/


void	sound_initialise (void)

// Purpose : Loads and Initialises sample playing instruments
// Accepts : Nothing
// Returns : Nothing

{

long	loop;
								
	if (SAMPLER_INITIALISED == 1)
		return;
	
	// CREATE [MAX_CHANNELS] SAMPLE PLAYING INSTRUMENTS
	
	for (loop=0; loop < MAX_CHANNELS; loop++)
		{
		instruments [loop] = LoadInstrument (SOUND_INSTRUMENT, 0, 100);					// Load instrument
		ConnectInstruments (instruments [loop], "Output", mixer, channelname [loop]);	// Connect instrument to mixer
		attachments [loop].sample_playing = -1;											// Set attachment for channel to notplaying
		}
	
	SAMPLER_INITIALISED = 1;															// Set sound flag to READY
}
		
/**************************************/

void	sound_terminate (void)

// Purpose : UnLoads and deletes sample playing instruments + attachments (if they exist)
// Accepts : Nothing
// Returns : Nothing


{
long	loop;

	if (SAMPLER_INITIALISED == 0)
		return;

	// DELETE [MAX_CHANNELS] SAMPLE PLAYING INSTRUMENTS
	
	for (loop=0; loop < MAX_CHANNELS; loop++)
		{
		DisconnectInstruments (instruments [loop], "Output", mixer, channelname [loop]);	// Disconnect instrument
		UnloadInstrument (instruments [loop]);												// Delete instruments
		if (attachments [loop].sample_playing != -1)
			DetachSample (attachments [loop].attachment_item);								// Clear attachment ?
		}
	
	SAMPLER_INITIALISED = 0;																// Set sound flag to UNREADY
}

/************************************************************************************************************************************
*										 			      SAMPLE PLAYING CODE														*
************************************************************************************************************************************/

long	sound_playsample (long sample, long pitch, long volume, long stereo_pos)

// Purpose : Plays a sample
// Accepts : Sample Number to Play, Pitch and Volume
// Returns : Channel sound is being played on ( 0 .. <MAX_CHANNELS, -1 for sound not being played)

{

long	free_channel = -1;			// Free channel ?
long	channel_search = 0;			// Channel search variable
long	left_volume,
		right_volume;\
		
static TagArg Query_Tags [] =		{	
										{AF_TAG_STATUS,	0},										//	CURRENT PLAY STATUS OF INSTRUMENT
										{TAG_END,	0}											//	END OF LIST
									};

static TagArg Sample_Tags [] =		{
										{AF_TAG_VELOCITY, 0},									//	SET VOLUME
										{AF_TAG_PITCH, 0},										//	SET PITCH
										{TAG_END, 0}											//	END OF LIST
									};

static TagArg Attachment_Tags [] = 	{
										{AF_TAG_SET_FLAGS,	(void*) AF_ATTF_FATLADYSINGS},		//	STOP INSTRUMENT WHEN ATTACHMENT CLEAR
										{TAG_END, 0}											//	END OF LIST
									};
	
	// RETURN IF SFX ARE SWITCHED OFF
	
	if (SAMPLE_MASTERSWITCH == 0)
		return (-1);

	
	// FIND A FREE CHANNEL (CLEAR ANY THAT ARE STILL MARKED AS 'ACTIVE' BUT FINISHED)

	do
		{
		
		if (attachments [channel_search].sample_playing >=0)									// Is attachment playing a sample ?
			{
			GetAudioItemInfo (instruments [channel_search], &Query_Tags[0]);											
			if ((long) Query_Tags[0].ta_Arg == AF_STOPPED)
				{
				DetachSample (attachments [channel_search] . attachment_item);					// Finished ?	Yes - Delete item
				attachments [channel_search].sample_playing = -1;								//				Yes - set to nosamp
				}
			}
			
		if (attachments [channel_search].sample_playing == -1)									// Is attachment Not Playing ?
			free_channel = channel_search;														// Yes, this is our one
			
		channel_search+=1;																		// Look for next channel
		}
		
	while (free_channel == -1 && channel_search < MAX_CHANNELS);								// Until finished or channel found
	
	// ANY FREE CHANNELS ? IF NOT, SEARCH FOR ANY ACTIVE WITH LOWER PRIORITY
	
	if (free_channel == -1)
		return (-1);
		
	// CREATE & REGISTER ATTACHMENTS
			
	attachments [free_channel].attachment_item = AttachSample (instruments [free_channel], samples [sample].sample_item, NULL);
	attachments [free_channel].sample_playing = sample;
	SetAudioItemInfo (attachments [free_channel].attachment_item, &Attachment_Tags[0]);
	
	// SET STEREO POSITION & VOLUME
	
	left_volume = ((16384 - stereo_pos) * volume) >> 7;
	right_volume = ((16384 + stereo_pos) * volume) >> 7;
	
	TweakKnob (mixer_leftgain [free_channel], left_volume);
	TweakKnob (mixer_rightgain [free_channel], right_volume);
	
	// SET PITCH AND VOLUME
	
	Sample_Tags [0].ta_Arg = (void*) SAMPLE_MASTERVOL;	// Set to max volume, (volume is now adjusted using left & right gain scales)
	Sample_Tags [1].ta_Arg = (void*) pitch;				// Set pitch

	// START SAMPLE PLAYING
	
	BendInstrumentPitch (instruments [free_channel], 1<<16);
	StartInstrument (instruments [free_channel], &Sample_Tags [0]);
	return (free_channel);
}

/**************************************/

long	sound_stopsample (long channel_number)

// Purpose : Stops a sample being played on specified channel
// Accepts : Channel number to stop
// Returns : -1 always

{
static TagArg Query_Tags [] =		{	
										{AF_TAG_STATUS,	0},										//	CURRENT PLAY STATUS OF INSTRUMENT
										{TAG_END,	0}											//	END OF LIST
									};
									
	// Return immediately if no instrument registered as playing on this channel
	
	if (attachments [channel_number].sample_playing == -1)
		return (-1);
		
	// Check to see if the instrument has finished, but just not been de-linked in flags else Release it
	
	GetAudioItemInfo (instruments [channel_number], &Query_Tags[0]);											
	if ((long) Query_Tags[0].ta_Arg == AF_STOPPED)	
		{
		DetachSample (attachments [channel_number] . attachment_item);							// Finished ?	Yes - Delete item
		attachments [channel_number].sample_playing = -1;										// & Register channel as not playing
		}
	else			
		{
		StopInstrument (instruments [channel_number], NULL);									// If unfinished, release it
		// ReleaseInstrument  (instruments [channel_number], NULL);								// If unfinished, release it
		}
		
	return (-1);
}

/**************************************/

long	sound_pitchbend (long channel_number, long pitch_bend)

// Purpose : Sets the pitch bend value of instrument (<<16) - 65536 is NORMAL FREQUENCY
// Accepts : Channel number to bend pitch of, and new pitch bend value
// Returns : 0 if no sample was playing, otherwise 1

{

	// Check channel is playing
	
	if (attachments [channel_number].sample_playing == -1)
		return (0);
	
	// Set new pitch bend value
	
	BendInstrumentPitch (instruments [channel_number], (frac16) pitch_bend);
	return (1);
}

/**************************************/

void	sound_alter (long channel_number, long new_volume, long new_stereoposition)

// Purpose : Sets a new volume & stereo position for a sample playing on a channel
// Accepts : Channel number, volume& stereo position
// Returns : nothing

{

long	left_volume,
		right_volume;
		
	// SET STEREO POSITION & VOLUME
	
	left_volume = ((16384 - new_stereoposition) * new_volume) >> 7;
	right_volume = ((16384 + new_stereoposition) * new_volume) >> 7;
	
	TweakKnob (mixer_leftgain [channel_number], left_volume);
	TweakKnob (mixer_rightgain [channel_number], right_volume);
}

/**************************************/

void	sound_setmastervolume (long new_mastervolume)

// Purpose : Sets a new master volume setting for samples
// Accepts : New volume number (0 - 128)
// Returns : nothing

{
	SAMPLE_MASTERVOL = new_mastervolume;
}
						
/**************************************/

void	sound_switchonoroff (long new_switchstatus)

// Purpose : Switches sample player on / off
// Accepts : New status (0 = off, 1 = on)
// Returns : nothing

{
	SAMPLE_MASTERSWITCH = new_switchstatus;
}
