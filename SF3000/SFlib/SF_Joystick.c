// File : 		SF_Joystick.c
// Purpose : 	Joystick control routines

// NOTE : Only reads 1st joystick in chain

// Joystick commands are :	joystick_initialise ()		// MUST BE CALLED 1st TO INITIALISE READER
//							joystick_terminate ()		// MUST BE CALLED TO TERMINATE READER
//							joystick_read ()			// reads joystick buttons
//							joystick_registerports ()	// returns number of joysticks connected

/**************************************/	

#include "SF_Joystick.h"
#include "types.h"
#include "string.h"
#include "mem.h"
#include "utils3do.h"
#include "stdio.h"

/**************************************/

BS_StickData 	joystick;										// Data block for our joystick
BS_StickData	*joystick_pointers [JOYSTICK_MAXJOYSTICKS];		// Array of pointers to stick data
long			JOYSTICK_READERTASKACTIVE = 0;					// Current status of joystick reader task
Item			joystick_taskref;								// Task ref of joystick reader

long			JOYSTICK_SIGNALPARENT,							// GENERAL SIGNAL FROM TASK TO PARENT
				JOYSTICK_TERMINATE;

stick_struct	pro_stick;										// THIS STRUCTURE HOLDS ALL DATA FOR JOYSTICK

/****************************************************************************************************
*									PARENT TASK CONTROL ROUTINES									*
****************************************************************************************************/

long joystick_initialise (void)

// Purpose : Initialises joystick reader task
// Accepts : Nothing
// Returns : 1 for success, 0 for failure

{
	if (JOYSTICK_READERTASKACTIVE == 1)				// If reader already running, return success
		return (1);
	
	JOYSTICK_SIGNALPARENT = AllocSignal (0);										// Allocate continue signal
	joystick_taskref = CreateThread("joystick_reader", 101, joystick_thread, 4096);	// Start reader thread
	if (joystick_taskref >= 0)														// Wait for ready signal
		WaitSignal (JOYSTICK_SIGNALPARENT);
	
	FreeSignal (JOYSTICK_SIGNALPARENT);				// Deallocate signal

	if (joystick_taskref >0)
		return (1);									// Return success / failure
	
	return (0);
}

/**************************************/

void joystick_terminate (void)

// Purpose : Kills joystick reader thread (if it's active)
// Accepts : Nothing
// Returns : Nothing

{
	if (JOYSTICK_READERTASKACTIVE == 1)
		{
		JOYSTICK_SIGNALPARENT = AllocSignal (0);			// Allocate KILL signal
		SendSignal(joystick_taskref, JOYSTICK_TERMINATE);	// Kill player
		WaitSignal (JOYSTICK_SIGNALPARENT);					// Wait for reader to signal that it's thrown a 7
		FreeSignal (JOYSTICK_SIGNALPARENT);					// Deallocate signal
		}
}


/****************************************************************************************************
*										JOYSTICK READER (THREAD)									*
****************************************************************************************************/

void joystick_thread (void)
{

	long	eCode;											// Return code
	
	// Initialise joystick data & Connect to event broker
	
	memset(joystick_pointers,0,sizeof(joystick_pointers));	// Set all pointers to joysticks to null
	joystick_pointers [0] = &joystick;						// Set 1st pointer to point to our joystick data block		
	BS_InitStickData (&joystick);							// Initialise joystick datablock
	BS_ConnectEventBroker ();								// Connect event broker
	
	joystick_registerports();								// Set up watches for currently connected devices

	JOYSTICK_TERMINATE	= AllocSignal (0);					// Initialise Terminate task Flag
	JOYSTICK_READERTASKACTIVE = 1;							// Set reader flag to ACTIVE
	SendSignal (parent_taskref, JOYSTICK_SIGNALPARENT);		// Indicate to parent that we have setup
		
/****************************************************************************************************
*										Main Control Loop											*
****************************************************************************************************/
	
	while (JOYSTICK_READERTASKACTIVE == 1)
		{
		
		eCode = BS_NiceWaitEvent(joystick_pointers, NULL, NULL, NULL);		// Wait for event
		
		if (eCode != 0)
			{
			if(eCode&BS_OTHER_SIGNAL)
				joystick_command (BS_GetOtherSignals());					// Recieved another signal ?
				
			if(eCode&BS_PORT_CHANGE)
				joystick_registerports ();									// Has the device daisy chain changed ?
					
			BS_ProcessStickData (joystick_pointers [0], 0, 0);				// Process joystick data
					
			pro_stick.x = BS_GetStickX (&joystick);				// Get X pos of joystick
			pro_stick.y = BS_GetStickY (&joystick);				// Get Y pos of joystick
			pro_stick.z = BS_GetStickZ (&joystick);				// Get Z pos of joystick
			pro_stick.b = BS_GetStickButtons (&joystick);		// Get buttons 
			}
		}
		
	BS_DisconnectBroker();									// Disconnect event broker
	SendSignal (parent_taskref, JOYSTICK_SIGNALPARENT);		// Indicate to parent that we have terminated
}

/**************************************/

void joystick_command (long command)

// Purpose : Called to perform joystick processor commands
// Accepts : Nothing
// Returns : Nothing

{
	if (command & JOYSTICK_TERMINATE)						// Are we terminating joystick task ?
		{
		JOYSTICK_READERTASKACTIVE = 0;						// Set status flag to not-active
		}
}

/**************************************/

long joystick_registerports (void)

// Purpose : Called to reinitialise watching status depending on joystick count
// Accepts : Nothing
// Returns : Number of joysticks connected

{

long joystick_count;

	joystick_count = BS_GetPeripheralCount(BS_STICK);	// No. of joysticks currently connected
	if (joystick_count)									// At least one joystick connected ?
		{
		if (!BS_WatchingJoyStick())						// Yes - Are we not currently watching ?
			BS_WatchJoyStick(1);						// Yes - Watch joysticks with debounce
		}
	
	else												// No joysticks connected ?
	
		{
		if (BS_WatchingJoyStick())						// Yes - Are we currently watching ?
			BS_IgnoreJoyStick();						// Yes - Turn off watching
		}

	printf ("%d Joysticks connected\n", joystick_count);
	return (joystick_count);
}
