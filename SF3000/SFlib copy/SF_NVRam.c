// File : NVRam.c
// NVRam management routines

// Includes

#include "SF_NVRam.h"
#include "SF_Message.h"
#include "String.h"
#include "directory.h"
#include "directoryfunctions.h"
#include "filestream.h"
#include "filestreamfunctions.h"
#include "filefunctions.h"
#include "BlockFile.h"

/**************************************/
					
long nvram_load (char* filename, char* gameprefix, char *addressofloadfile, long sizeofloadfile)

// Purpose : Loads a file from NVRam
// Accepts : Pointer to filename & Pointer to prefix to add to filename, load address, length of file to load
// Returns : (1) If loaded succesfully, (0) If not

{
IOInfo 	fileInfo;
Item 	ioReqItem;
Item	fileitem;
char	full_filename [40];
long	loop;

	// CREATE FULL PATHNAME
	sprintf(full_filename,"/NVRAM/%s%s",gameprefix,filename);		// Generate full filename and open it
	fileitem=OpenDiskFile(full_filename);
	if (fileitem<0)
		return (0);
	
	// LOAD IN THE FILE
	ioReqItem = CreateIOReq(NULL, 0, fileitem, 0);
	if (ioReqItem >= 0)
		{
		memset(&fileInfo, 0, sizeof(IOInfo));
		fileInfo.ioi_Command = CMD_READ;							// Set command to READ file
		fileInfo.ioi_Recv.iob_Buffer = addressofloadfile;			// Set load address
		fileInfo.ioi_Recv.iob_Len = sizeofloadfile;					// Set load length
		DoIO(ioReqItem, &fileInfo);									// Load the file
		DeleteIOReq(ioReqItem);										// Delete IO request
		CloseDiskFile(fileitem);
		return (1);													// Return 1 for success
		}
	return (0);														// Return 0 for Failure
}

/**************************************/

long nvram_save (char* filename, char* gameprefix, char* addressofsavefile, long sizeofsavefile)

// Purpose : Saves a file to NVRam AFTER ADDING 'GAMEPREFIX' TO IT
// Accepts : Pointer to filename, Pointer to prefix to add, Pointer to start of data, Length of data
// Returns : (1) If saved succesfully, (0) If not (Not enough memory)

{
Item		fileitem;
Item		ioReqItem;
IOReq		*ioReqP;
IOInfo		fileInfo;
char		full_filename [40];
long		loop;
uint32		numberOfBlocks, blockSize;
FileStatus file_status;
	
	// CREATE FULL PATHNAME
	
	sprintf(full_filename,"/NVRAM/%s%s",gameprefix,filename);		// Generate full filename and open it
	
	// CREATE DISK FILE
	
	nvram_delete (filename, gameprefix);
	fileitem=CreateFile(full_filename);
	
	if (fileitem < 0)
		{
		printf("NVRAM - COULD NOT CREATE DISK FILE - file must exist\n");
		return (0);
		}
	
	// 	OPEN THE DISK FILE
	
	fileitem = OpenDiskFile (full_filename);
	if (fileitem < 0)
		{
		printf("NVRAM - COULD NOT OPEN DISK FILE\n");
		return (0);
		}

	//	GET IO REQ ITEM FOR FILE
	
	ioReqItem = CreateIOReq(NULL, 0, fileitem, 0);
	if (ioReqItem < 0)
		{
		printf("NVRAM - Could not create IOREQ for blocksize set\n");
		return (0);
		}
			
	//	GET BLOCKSIZE OF FILE
	
	blockSize = 0;
	memset(&fileInfo, 0, sizeof(IOInfo));
	fileInfo.ioi_Command = CMD_STATUS;
	fileInfo.ioi_Recv.iob_Buffer = &file_status;
	fileInfo.ioi_Recv.iob_Len = sizeof(FileStatus);
	DoIO(ioReqItem, &fileInfo);
	blockSize = file_status.fs.ds_DeviceBlockSize;

	//	CALCULATE NUMBER OF BLOCKS REQUIRED
		
	numberOfBlocks = (sizeofsavefile / blockSize);
	if ((sizeofsavefile % blockSize) > 0)
		numberOfBlocks++;
			
	//	SET NUM OF BLOCKS FOR FILE
	
	memset(&fileInfo, 0, sizeof(IOInfo));
	fileInfo.ioi_Command = FILECMD_ALLOCBLOCKS;
	fileInfo.ioi_Offset = numberOfBlocks;
	ioReqP = (IOReq *) LookupItem(ioReqItem);
	DoIO(ioReqItem, &fileInfo);
					
	// WRITE OUTPUT FILE

	memset(&fileInfo, 0, sizeof(IOInfo));
	fileInfo.ioi_Command = CMD_WRITE;								// Set command to READ file
	fileInfo.ioi_Send.iob_Buffer = addressofsavefile;				// Set save address
	fileInfo.ioi_Send.iob_Len = (int)((sizeofsavefile+3) & ~3);		// Set file length (word aligned)
	DoIO(ioReqItem, &fileInfo);
	
	// SET END OF FILE

	memset(&fileInfo, 0, sizeof(IOInfo));
	fileInfo.ioi_Command = FILECMD_SETEOF;							// Set command to SET EOF
	fileInfo.ioi_Offset = sizeofsavefile;							// Set EOF length
	ioReqP = (IOReq*)LookupItem(ioReqItem);
	DoIO(ioReqItem, &fileInfo);

	// TIDY UP & RETURN SUCCESS

	DeleteIOReq(ioReqItem);
	CloseDiskFile(fileitem);
	return (1);
}

/**************************************/

long nvram_delete (char* filename, char* gameprefix)

// Purpose : Deletes a file from NVRam
// Accepts : Pointer to filename
// Returns : Success or Failure of DeleteFile

{
char	full_filename [40];

	sprintf(full_filename,"/NVRAM/%s%s", gameprefix, filename);		// Generate full NVRAM filename
	return (DeleteFile (full_filename));
}

/**************************************/

long nvram_directory (long menu_number, long menu_entry, long menu_maxentry, long set_itemok, char* gameprefix)

// Purpose : Reads directory of NVRAM  files into menu entries - Only those that start with 'gameprefix'
// Accepts : menu number, menu entry to start placing filenames in, entry to stop and default selectable status of entries
// Returns : Number of directory entries found

{

Directory		*directory;
DirectoryEntry	filename;
Item			ioReqItem;
Item			rootItem;
long			loop;
long			file_occurence = 0;

	// Clear current menu directory entries
	
	for (loop=menu_entry; loop <=menu_maxentry; loop++)
		{
		menus [menu_number].items[loop].selectable = set_itemok;
		sprintf (message_decode (menus [menu_number].items [loop].string [0], 1), "-");
		}
		
	// Open NVRam and IO requests
	
	rootItem = OpenDiskFile("/NVRAM");
	ioReqItem = CreateIOReq(NULL,0,rootItem,0);
	if (ioReqItem < 0)
		return (0);
		
	directory = OpenDirectoryItem(rootItem);
	
	// Read all files from directory, only register those starting with 'gameprefix'
	
	while(ReadDirectory(directory,&filename) >= 0 && menu_entry <=menu_maxentry)
		if (strncmp(filename.de_FileName, gameprefix, strlen (gameprefix))==0)
			{		
			file_occurence+=1;
			strcpy (message_decode (menus [menu_number].items [menu_entry].string [0], 1), &filename.de_FileName[0]+strlen (gameprefix));
			menus [menu_number].items [menu_entry++].selectable = ITEM_OK;
			}

	// Close NVRam and IO requests
	
	DeleteIOReq(ioReqItem);
	CloseDirectory(directory);
	CloseDiskFile(rootItem);

	return (file_occurence);
}
