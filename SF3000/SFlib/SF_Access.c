// File : SF_Access
// General File Access Routines

#include "SF_Access.h"
#include "SF_Utility.h"
#include "SF_Music.h"
#include "SF_ARMBurn.h"
#include "SF_ARMCell.h"
#include "SF_ARMAnim.h"
#include "SF_ARMUtils.h"
#include "varargs.h"
#include "types.h"
#include "blockfile.h"

/* Global variables */

long	file_return;
ubyte*	backdrop;


/**************************************/

long load_mission(long load_level, long load_mission)

// Purpose : Loads in the specified mission file & associated data files
// Accepts : pyramid level and mission number
// Returns : 0 if failed, 1 if successful
												
{
	
	// NOTE : WAIT HERE FOR MUSIC BUFFER FILL TO STOP GLITCHES IN PLAYBACK
	music_maketime ();
	
	// Load in mission file
	
	if (load_fileat(&mission, "%s%c/Miss_%d",MIS_ROOT,decode_level(load_level),load_mission) == NULL)
		return (0);
			
	return (1);
}

/**************************************/

long load_missionfiles (void)

// Purpose : Loads in the files associated with a mission
// Accepts : nothing
// Returns : 0 if failed, 1 if successful
												
{
static	long	cache_loadlevel=-1;
static	long	cache_loadmission=-1;
static	char	cache_planet [16] = "";
static	char	cache_sky [16] = "";

	// Load in polygon map file
	
	if (load_fileat(*poly_map, "%sMaps/%s/%s/P_Map",MIS_ROOT,mission.location, mission.variation) == NULL)
		return (0);

	// Only load in graphics and planet data if planet type is different from last time
	
	if (strcmp (mission.planettype, cache_planet) != 0)
		{
		strcpy (cache_planet, mission.planettype);
		
		if (load_fileat(cel_list32, "%s%s.32",CEL_ROOT,mission.planettype) == NULL)
			return (0);
		
		if (load_fileat(graphics_data, "%s%s",GRF_ROOT,mission.planettype) == NULL)
			return (0);

		if (load_fileat(&planet_info, "%s%s.info",INF_ROOT,mission.planettype) == NULL)
			return (0);
		
		if (load_fileat(animate_poly, "%s%s.anim",ANIM_ROOT,mission.planettype) == NULL)
			return (0);
			
		// Only load in these files and initialise if we are NOT in a space mission
		
		if (planet_info.space_mission == 0)
			{		
			if (load_fileat(cel_list16, "%s%s.16",CEL_ROOT,mission.planettype) == NULL)
				return (0);
	
			if (load_fileat(cels4x4, "%s%s.4",CEL_ROOT,mission.planettype) == NULL)
				return (0);	

			if (load_fileat(cel_codedpalette, "%s%s.pal",PAL_ROOT,mission.planettype) == NULL)
				return (0);
	
			armburn_initialise (&planet_info);									// Initialise new planet info
			}
			
		armtex_initialise (animate_poly, cel_list32);							// Initialise new planet texture animations
		armtex_reset (animate_poly);											// Reset all polygon animations
		}

	// ONLY LOAD IN THE FOLLOWING MISSION FILES IF THIS IS NOT A SPACE MISSION
	
	if (planet_info.space_mission == 0)
		{
		// Only load in animation if different mission from last time
	
		if (cache_loadlevel != mission.mission_level || cache_loadmission != mission.mission_number)
			{
			cache_loadlevel = mission.mission_level;
			cache_loadmission = mission.mission_number;

			//NOTE : GROUND ANIMATIONS TAKEN OUT 16/8/95
			
			//if (load_fileat(animate, "%sMaps/%s/%s/S_Anm",MIS_ROOT,mission.location, mission.variation) == NULL)
			//	return (0);
			}
		
		// Load in map for mission (Spritemap 'S_Map' and HeightMap 'H_Map')
	
		if (load_fileat(*sprite_map, "%sMaps/%s/%s/S_Map",MIS_ROOT,mission.location, mission.variation) == NULL)
			return (0);

		if (load_fileat(*height_map, "%sMaps/%s/%s/H_Map",MIS_ROOT,mission.location, mission.variation) == NULL)
			return (0);

		// Only load in sky file if it is different from last time
	
		if (strcmp (mission.skyfile, cache_sky) != 0)
			{
			strcpy (cache_sky, mission.skyfile);
			if (load_fileat(skyfile, "%s%s",SKY_ROOT,mission.skyfile) == NULL)
				return (0);
			}
		
		// Gerenate maps
		arm_generatemaps (&cel_quad, *sprite_map);
		}
		
	return (1);
}
	
/**************************************/		

long load_gamedata (long load_spec)

// Purpose : Loads in the cosine / tangent data tables etc at game start.
// Accepts : (0) - Just load in configuration file, (1) - Load in all files, including configuration file
// Returns : 0 if failed, 1 if successful

{

	if (load_spec & DATA_CONFIGURE)
		{
		memset (&configuration, 0, sizeof (game_configuration));
		if (load_fileat (&configuration, "%sConfig",DAT_ROOT)==NULL)		// Load in default game configuration
			return (0);
		}
		
	if (load_spec & DATA_FILES)
		{
		if (load_fileat (cosine_table, "%sCosine",DAT_ROOT)==NULL)			// Load cosine table
		return (0);

		if (load_fileat (tangent_table, "%sTangent",DAT_ROOT)==NULL)		// Load tangent table
			return (0);
	
		if (load_fileat (cel_palette, "%sMonochrome.pal",PAL_ROOT)==NULL)	// Load in palette sprite block for monochrome cels
			return (0);
		}
	
	return (1);
}

/**************************************/		

long load_gamecels (char *filename)

// Purpose : Loads in a set of fixed game cels (but not if already loaded)
// Accepts : Filename to load in, defaults to loading in 'game.<planettype>' if NULL is passed
// Returns : 0 if failed, 1 if successful

{

static	char cache_loadgamecel [24]="";
char	cel_gamefile [24];

	// Loading in filename or planettype ?
	
	if (filename == NULL)
		strcpy (cel_gamefile, mission.planettype);
	else
		strcpy (cel_gamefile, filename);
			
	// Is file already in the cache ? - If not, load in and copy filename into cache	
				
	if (strcmp (cel_gamefile, cache_loadgamecel) != 0)
		{
		strcpy (cache_loadgamecel, cel_gamefile);
		
		// NOTE : WAIT HERE FOR MUSIC BUFFER FILL TO STOP GLITCHES IN PLAYBACK
		music_maketime ();
		
		if (load_fileat(cel_quad.cel_game, "%sGame.%s", CEL_ROOT, cel_gamefile) ==NULL)
			return (0);
		}
	return (1);
}

/**************************************/

void load_backdrop (char* backdrop_name, long load_over)

// Purpose : Loads in a backdrop image, putting load address in global variables 'backdrop' - Loads in 'menu[rnd]' if NULL passed
// Accepts : Filename to load in, flag to (0) ask for buffer (1) load raw image into previously allocated buffer
// Returns : Nothing

{
char	backdrop_file [48];

	if (backdrop_name == NULL)
		sprintf (backdrop_file, "%s%sMenu%d", RESOURCES_ROOT, IMG_ROOT, arm_random() & 7);	// Get random menu file ?
	else
		sprintf (backdrop_file, "%s%s%s", RESOURCES_ROOT, IMG_ROOT, backdrop_name);			// Or get passed image file ?
	
	// WAIT FOR MUSIC BUFFER FILL TO STOP GLITCHES, THEN LOAD FILE
	
	music_maketime ();
	
	if (load_over == 0)
		backdrop = LoadImage (backdrop_file, NULL, NULL, screen);							// Load in backdrop & allocate buffer
	else
		load_fileat (backdrop, backdrop_file);												// Or load raw image straight in ?
}

/**************************************/

void* load_fileat (void *buffer, char *fmt,...)

// NOTE : 	THIS ROUTINE IS RIPPED FROM 3D0 LIBRARY
//			ALTERED TO LOAD CORRECT SIZE, REGARDLESS OF MEDIA BLOCKSIZE
//			ALWAYS LOADS FROM RESOURCES_ROOT (ie $boot/SF_Resources/)

// Purpose : File loader
// Accepts : Filename, Load address
// Returns : Load address or NULL if failed to load

{
long		rv,
			filesize,
			blocksize,
			remainder,
			bufsize;
	
Item		ioreqItem;
BlockFile	thefile;

char		*temp_buffer,
			*destination;

char		full_filename [128];
char		directory_filename [128];

va_list		variable_args;
thefile.fDevice = 0;
ioreqItem 		= 0;

	// CONSTRUCT FULL FILENAME
	
	va_start(variable_args,fmt);
	vsprintf(full_filename,fmt,variable_args)
	va_end(variable_args);

	sprintf (directory_filename, "%s%s", RESOURCES_ROOT, full_filename);
	
	// OPEN THE FILE TO LOAD								
	
	rv = OpenBlockFile(directory_filename, &thefile);
	if (rv < 0)
		goto ERROR_EXIT;

	// GET INFO ON FILE AND DEVICE
	
	filesize  = thefile.fStatus.fs_ByteCount;				// Size of the file to load
	blocksize = thefile.fStatus.fs.ds_DeviceBlockSize;		// Blocksize of device
	bufsize   = (filesize/blocksize)*blocksize;				// Number of bytes to nearest blocksize
	remainder = filesize-bufsize;							// Remainding bytes after blocksize

	// GET IOREQ ITEM TO LOAD

	ioreqItem = CreateBlockFileIOReq(thefile.fDevice, 0);
	if (ioreqItem < 0)
		goto ERROR_EXIT;
		
	// LOAD IN ALL BLOCKS (EXCEPT REMAINDER) DIRECTLY INTO MEMORY
		
	if (bufsize >0 )
		{

		// NOTE : WAIT HERE FOR MUSIC BUFFER FILL TO STOP GLITCHES IN PLAYBACK
		music_maketime ();
	
		AsynchReadBlockFile(&thefile, ioreqItem, buffer, bufsize, 0);
		rv = WaitReadDoneBlockFile(ioreqItem);
		if (rv !=0)
			goto ERROR_EXIT;
		}

	// LOAD IN REMAINDING BYTES INTO TEMP AREA, AND COPY TO COMPLETE REST OF FILE LOAD
	
	if (remainder >0 )
		{
		destination = (char*) buffer+bufsize;							// Adr to put remaining data
		temp_buffer =(char*) AllocMem(blocksize,MEMTYPE_ANY);			// Temp blocksize buffer

		// NOTE : WAIT HERE FOR MUSIC BUFFER FILL TO STOP GLITCHES IN PLAYBACK - ONLY IF ABSOLUTELY NECESSARY
		// music_maketime ();
	
		AsynchReadBlockFile(&thefile, ioreqItem, temp_buffer, blocksize, bufsize);
		rv = WaitReadDoneBlockFile(ioreqItem);
		if (rv !=0)
			goto ERROR_EXIT;

		memcpy(destination, temp_buffer, remainder);
		FreeMem(temp_buffer,blocksize);
		}
		
	// CLOSE THE FILE AND LEAVE
	
	if (ioreqItem > 0)			// Close IOREQ
		DeleteIOReq(ioreqItem);
	CloseBlockFile(&thefile);	// Close File
		
	return (buffer);			// Return with load address, success !
	
ERROR_EXIT :

	if (ioreqItem > 0)			// Close IOREQ
		DeleteIOReq(ioreqItem);
	CloseBlockFile(&thefile);	// Close File
	
	printf("LOAD ERROR : Can't load '%s'\n", directory_filename);			// Print Error Message
	
	return (0);					// Return ZERO for error
}

