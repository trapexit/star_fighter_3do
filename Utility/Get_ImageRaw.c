#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/***************************************************/

void logit (char* log_message)
{
FILE*	file_error;
	
	file_error = fopen("Moppit:ErrorLog", "a");
	fputs(log_message, file_error);
	fclose (file_error);
}
						
/***************************************************/

void	main()
{

char	master_filename [128],
		master_title [128],
		log	[256],
		pix_filenameout [256],
		mchar;

FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

long	get_source=0,
		finished_master=0;

long	loop;

	logit ("\n\nConverting 3DO Image files to raw 3DO screen pictures :\n\n");
		
// Open master files list
// ----------------------

	finished_master=0;
	
	sprintf(&master_filename,"Moppit:coded8bpp:Imagelist");
	file_master=fopen (master_filename,"rb");
	
// Main music file search loop
// ---------------------------
	
	do
		{
		get_source=0;
	
// Read in directory name of pix files to convert
// ----------------------------------------------
	
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
		master_title [get_source] = 0;
		
// Fix output file
// ---------------

		if (finished_master==0 && master_title [0] != '|')
			{

// Open source info file
			
			sprintf(log, "\n    - Converting image file > '%s'\n", master_title);
			logit (log);
			
			file_in=fopen (master_title, "rb");
			
			if (file_in != NULL)
				{
				sprintf (pix_filenameout, "%s#", master_title);
				file_out=fopen(pix_filenameout, "wb");
				
				fseek (file_in, 36, SEEK_SET);
					
				for (loop = 0; loop < 153600; loop++)
					fputc (fgetc (file_in), file_out);
					
				fclose (file_out);
				}
			fclose (file_in);
			}
		}
	while (finished_master == 0);	
	fclose (file_master);
	printf("Image files converted - See Log\n");
}
