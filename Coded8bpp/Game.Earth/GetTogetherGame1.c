#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	hdisk	"Moppit:"

/***************************************************/

void logit (char* log_message)
{
FILE*	file_error;
	
	file_error = fopen("Moppit:ErrorLog", "a");
	fputs(log_message, file_error);
	fclose (file_error);
}

/***************************************************/

int	main()
{
		
char	filename_in [64],
		filename_out [64],
		master_filename [64],
		master_title [64],
		gamemaster_title [64],
		mchar,
		log	[256];

char	buildfile [256*1024];
long	buildoffset [1024];

FILE*	file_gamemaster;
FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

char	*wordin;

long	finished=0;
long	packed;

long	p_load,
		p_entries,
		buildlen=0,
		buildptr=0,
		length,
		word,
		other_word,
		cel_counter=0,
		get_source=0,
		finished_master=0,
		game;

		wordin=(char*) &word;

// Main Game number conversion loop
// --------------------------------

	logit ("\n\nConversion of Game cels\n");
	
	sprintf(&master_filename,"%scoded8bpp:GameList",hdisk);
	file_gamemaster=fopen(master_filename,"rb");
	
	do
		{
	
		get_source=0;
		do
			{
			mchar=fgetc (file_gamemaster);
			if (feof (file_gamemaster))
				finished  = 1;
			else	
				if (mchar>=32)
					gamemaster_title[get_source++]=mchar;
			}
		while (mchar >=32 && finished==0);
		gamemaster_title[get_source]=0;
	
		if (finished==0)
		{
	
		sprintf(log, "\n    - Converting Gameset '%s'\n", gamemaster_title);
		logit (log);
		
		cel_counter=0;
		buildlen=0;
		buildptr=0;
		
// Open master files list
// ----------------------
	
		finished_master=0;
	
		sprintf(&master_filename,"%scoded8bpp:Game.%s:Files",hdisk,gamemaster_title);
		file_master=fopen (master_filename,"rb");
	
// Main sprite search loop
// -----------------------
	
		do
		{
	
		get_source=0;
	
// Read in name of sprite to convert
// ---------------------------------
	
			do
				{
				mchar=fgetc (file_master);
				
				if (mchar == EOF)
					finished_master=1;				// End of master file ? if so, set finishedmaster
				
				master_title[get_source]=mchar;
			
				if (mchar >=32)
					get_source++;
				}
			while (mchar >=32 && finished_master==0);
			master_title[get_source]=0;

			
// Open source cel file
// --------------------

			if (finished_master == 0)
				{
				sprintf(&filename_in, "%scoded8bpp:Game.%s:Packed:%s.cel", hdisk, gamemaster_title, &master_title[3]);
				file_in=fopen(&filename_in, "rb");
	
				if (file_in != NULL)
					{
					buildoffset [buildlen++] = buildptr;
			
					// Generate PIXC control, depending on translation command (a ... add shade, o-  ... opaque, m ... mix shading)
				
					switch (master_title[1])
						{
						case 'a' :
							word = (2<<6);
							break;							// Add shading
						case 'm' :
							word = (2<<6) | 1;
							break;							// Mix shading
						case 'o' :
							word = 0;
							break;							// Normal (Opaque shading)
						case 's' :
							word = (1<<15) | (3<<6) | 1<<1;	// Subtract shading
							break;
						default :
							word = 0;						// Default (As opaque)
							break;
						}
				
					word = word | (word<<16);						// Make for Pmode 1 as well
				
					if (master_title [1] != 'p')					// Don't put in PIXC if we are just doing PLUT
						{
						buildfile[buildptr++] = *(wordin+0);
						buildfile[buildptr++] = *(wordin+1);
						buildfile[buildptr++] = *(wordin+2);
						buildfile[buildptr++] = *(wordin+3);
						}
						
					// Clear flags word
		
					word = 0;
					
					// Set important bits of flags word
					
					if (master_title [0] == 'm')			// Set MARIA Flag ? (Optional)
						word |= 1<<12;						// Yes !
					
					if (master_title [1] == 's')			// USEAV bits of pixc word ?
						word |= 1<<10;						// Yes !
						
					word = word | 	(
									(1<<29) |				// Absolute NEXTPTR
									(1<<28) |				// Absolute SOURCEPTR
									(1<<27) |				// Absolute PLUTPTR
									(1<<26) |				// Load in HDX, HDY, VDX, VDY
									(1<<25) |				// Load in HDDX, HDDY
									(1<<24) |				// Load in PIXC word
									(1<<21) |				// Set x&y position
									(1<<19)	|				// Enable Cel line clipping !
									(1<<18) |				// Enable clockwise pixel rendering
									(1<<17) |				// Enable counter clockwise pixel rendering
									(1<<14) |				// Enable 2nd cel engine
									(1<<8)					// Use PMode 0
									);
					
					fseek (file_in, 12, SEEK_SET);
					fread (&other_word, sizeof (long), 1, file_in);		// Is cel packed ?
					packed = (other_word & (1<<9)) ? 1 : 0;				// 0 - no, 1 - Yes
					if (packed == 1)
						word |= (1 << 9);								// Put it in the flags
						
					if (other_word & (1<<23))
						word |= 1<<23;									// Load in PLUT ?
						
					if (master_title [1] != 'p')						// Don't put in FLAGS if we are just doing PLUT
						{				
						buildfile[buildptr++] = *(wordin+0);
						buildfile[buildptr++] = *(wordin+1);
						buildfile[buildptr++] = *(wordin+2);
						buildfile[buildptr++] = *(wordin+3);
						}
					
					// Get sprite details
			
					fseek (file_in, 84, SEEK_SET);
					fread (&length, sizeof(long), 1, file_in);			// Get length of sprite
					if (packed == 0)
						length-=4;
							
					// Should we be copying over palette data ?
					
					if ((word & (1<<23)) != 0 || master_title == 'p')
						{
						
						// Yes - find out PLUT data position and sprite length, then copy
						
						fseek (file_in, 84, SEEK_SET);
						fread (&word, sizeof(long), 1, file_in);			// Get Offset to PLUT
						word+=92;											// Start of PLUT
						if (packed == 0)
							word+=4;
							
						fseek (file_in, word, SEEK_SET);					// Point to plut data
						
						for (p_load = 0; p_load < 16; p_load++)
							{
							fread (&word, sizeof(long), 1, file_in);		// Get 2 palette entries
							buildfile[buildptr++] = *(wordin+0);
							buildfile[buildptr++] = *(wordin+1);
							buildfile[buildptr++] = *(wordin+2);
							buildfile[buildptr++] = *(wordin+3);			// Stored
							}
						}				
										
					if (master_title [1] != 'p')							// Don't put in PREAMBLES OR SOURCEDATA if just doing PLUT
						{
						fseek (file_in, 88, SEEK_SET);
						fread (&word, sizeof (long), 1, file_in);			// Copy Preamble 0
						buildfile [buildptr++] = *(wordin+0);
						buildfile [buildptr++] = *(wordin+1);
						buildfile [buildptr++] = *(wordin+2);
						buildfile [buildptr++] = *(wordin+3);			
					
						if (packed == 0)
							{
							fread (&word, sizeof (long), 1, file_in);		// Copy Preamble 1 (If sprite is not packed)
							buildfile [buildptr++] = *(wordin+0);
							buildfile [buildptr++] = *(wordin+1);
							buildfile [buildptr++] = *(wordin+2);
							buildfile [buildptr++] = *(wordin+3);			
							}
									
						for (get_source=0; get_source<(length-12); get_source++)			
							buildfile[buildptr++] = fgetc (file_in);
						}
						
					fclose(file_in);
					cel_counter++;
					}
				else
					{
					sprintf(log, "       - FILE NOT FOUND '%s'\n",filename_in);		
					logit (log);
					}
				}
			}
			while (finished_master == 0);
			
			// Open group output file for planet
	
			sprintf(&filename_out,"%sgroupcel:Game.%s", hdisk, gamemaster_title);
			file_out=fopen(filename_out,"wb");
			
			// Write output file
			
			for (get_source=0; get_source < buildlen; get_source++)
				{
				buildoffset[get_source]+=buildlen<<2;
				fwrite (&buildoffset[get_source], sizeof(long), 1, file_out);
				}
			
			for (get_source=0; get_source < buildptr; get_source++)
				fputc (buildfile[get_source],file_out);
			
			// Close output file
			
			fclose(file_out);
			sprintf(log, "       - %d files converted from Gameset\n", cel_counter);
			logit (log);		
			fclose (file_master);
		}
	}
	while (finished == 0);
	fclose (file_gamemaster);
}
