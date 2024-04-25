/* File : Sound header file */

#ifndef __SF_SOUND
#define __SF_SOUND

// Includes

#include "types.h"

// Structure definitions

// DEFINITION FOR SAMPLE LOAD IN LIST

typedef struct sample_list	{
							long	priority;
							char	*name;
							}	sample_list;

// DEFINITION FOR SAMPLE INFORMATION

typedef struct sample_info	{
							Item	sample_item;
							long	priority;
							}	sample_info;

// DEFINITION FOR ATTACHMENT INFORMATION

typedef struct attachment_info	{
								Item	attachment_item;
								long	sample_playing;
								}	attachment_info;

// Definitions for update flags

#define UPDATE_PITCH	1
#define	UPDATE_VOLUME	2

// Definitions for Sounds

#define SOUND_LASER		0
#define SOUND_MISSILE	1
#define SOUND_THUD		2
#define	SOUND_ENGINE	3
#define	SOUND_BEEP		4
#define	SOUND_CLICK		5
#define SOUND_EXPLOSION	6
#define SOUND_WIND		7
#define	SOUND_CHIME		8
#define	SOUND_LOCKON	9
#define	SOUND_PICKUP	10
#define	SOUND_INCOMING	11
#define	SOUND_ALARM		12
#define	SOUND_BEAMLASER	13

// Function Prototypes

void	sound_loadsamples (void);						// Load in game samples & setup mixer [START OF GAME]
void	sound_unloadsamples (void);						// Deletes all samples & kills mixer [END OF GAME]

void	sound_initialise (void);						// Makes sample player active [MAKE SFX READY]
void	sound_terminate (void);							// Makes sample player inactive [SUSPEND SFX TO FREE DSP MEMORY]

long	sound_playsample (long, long, long, long);		// Play a sample
long	sound_stopsample (long);						// Stop a sample on specified channel
long	sound_pitchbend (long, long);					// Set pitch bend for a channel
void	sound_alter (long, long, long);					// Set new volume & stereo pos for a sample

void	sound_setmastervolume (long);					// Set new master volume setting
void	sound_switchonoroff (long);						// Switch sample player on / off

#endif

