#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	hdisk	"Moppit:"

char	string [128];

/***************************************************/

long	info_convert(char* cptr)

{
char	c1=*cptr,
		c2=*(cptr+1),
		c3=*(cptr+2);
	
	return ( ((c1-48)*100) + ((c2-48)*10) + (c3-48) );
}

/***************************************************/

void logit (char* log_message)
{
FILE*	file_error;
	
	file_error = fopen("Moppit:ErrorLog", "a");
	fputs(log_message, file_error);
	fclose (file_error);
}

/***************************************************/

char*	info_get (FILE* file_in, char* checkstring)
{

char	achar;
long	ccount,
		infolinefinished;

	do
		{
		ccount=0;
		infolinefinished=0;
		do
			{
			achar=fgetc (file_in);
			if (!feof (file_in))
				if (achar >=32)
					string[ccount++]=achar;
			if (feof (file_in))
				infolinefinished=1;
			}
		while (achar !=13 && infolinefinished==0);
	
		if (infolinefinished==0)
			{
			if (strncmp (checkstring, string, 3)==0)
				return (&string[3]);
			}
		}
	while (infolinefinished==0);
	return (NULL);
}
						
/***************************************************/

void	main()
{

char	filename_in [64],
		filename_out [64],
		master_filename [64],
		master_title [64],
		log	[256],
		mchar;

long	word_out;
char	*byte_out=(char*) &word_out;

char*	info_line;

long	buildfile [4096];
long	buildoffset [1024];

FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

long	get_source=0,
		finished_master=0;

long	explosion,
		explosion2,
		comet_rate;

char	height_check,
		flash_write,
		sprite_ref;
		
		
		logit ("Converting Info Files :\n\n");
		
// Open master files list
// ----------------------

	finished_master=0;
	
	sprintf(&master_filename,"%scoded8bpp:Masterlist",hdisk);
	file_master=fopen (master_filename,"rb");
	
// Main planet type search loop
// ----------------------------
	
	do
		{
		get_source=0;
	
// Read in name of planet type to convert
// --------------------------------------
	
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

// Convert info file for planet type
// ---------------------------------------

		if (finished_master==0)
			{

// Open source info file
			
			sprintf(&filename_in, "%scoded8bpp:%s16:Info",hdisk,master_title);
			file_in=fopen(&filename_in, "rb");

// Open group output file for planet
	
			sprintf(&filename_out,"%sCoded8bpp:Info:%s.info",hdisk,master_title);
			file_out=fopen(filename_out,"wb");

// Convert animation file for ground

			if (file_in != NULL)
				{
				sprintf(log, "Converting info file for ->%s\n",master_title);
				logit (log);
											
				// Get height check
				
				info_line = info_get (file_in, "HC ");
				if (info_line==NULL)	goto ended;
				height_check =info_convert(info_line);
				
				info_line = info_get (file_in, "FW ");
				if (info_line==NULL)	goto ended;
				flash_write =info_convert(info_line);
				
				info_line = info_get (file_in, "CR ");
				if (info_line==NULL)	goto ended;
				comet_rate = info_convert(info_line);
				
				// Loop to read 2 ecplosions
				
				for (explosion=0; explosion < 2; explosion++)
					for (explosion2=0; explosion2<8; explosion2++)
						{
						// Get individual explosion sprite reference
						
						info_line = info_get (file_in, "AN ");
						if (info_line==NULL)	goto ended;
						sprite_ref = (char) info_convert(info_line);
						fputc (sprite_ref, file_out); 
						}
				
				// Write header info
				
				fputc (height_check, file_out);
				fputc (flash_write, file_out);
				fputc (0, file_out);
				fputc (0, file_out);
				
				// Write comet rate
				
				word_out = comet_rate;
				
				fputc (*(byte_out+0), file_out);
				fputc (*(byte_out+1), file_out);
				fputc (*(byte_out+2), file_out);
				fputc (*(byte_out+3), file_out);
				}
ended:

			// Close output file
			
			fclose(file_out);
			fclose(file_in);
			}
		}
	while (finished_master == 0);	
	
	fclose (file_master);
	printf("Info files converted - See Log\n");
}
