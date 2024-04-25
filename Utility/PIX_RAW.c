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
		pix_filename [256],
		pix_filenameout [256],
		mchar;

FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

long	get_source=0,
		finished_master=0;

long	pix_number,
		x_size,
		y_size,
		total_pixels,
				
		zero,
		pixel_count,
		red,
		green,
		blue,
		rlc;
		
	logit ("\n\nConverting Silicon Graphics PIX files :\n\n");
		
// Open master files list
// ----------------------

	finished_master=0;
	
	sprintf(&master_filename,"Moppit:coded8bpp:Pixlist");
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
			
			sprintf(log, "\n    - Converting pix files > '%s'\n", master_title);
			logit (log);
			
			pix_number = 1;
			
			do
				{
				
				if (pix_number >= 100)
					sprintf (pix_filename, "%s.%d", master_title, pix_number);
				
				if (pix_number >= 10 && pix_number <= 99)
					sprintf (pix_filename, "%s.0%d", master_title, pix_number);
				
				if (pix_number <= 9)
					sprintf (pix_filename, "%s.00%d", master_title, pix_number);
				
				file_in=fopen (pix_filename, "rb");
				if (file_in != NULL)
					{
				
					sprintf (pix_filenameout, "%s#", pix_filename);
					file_out=fopen(pix_filenameout, "wb");
								
					// GET DETAILS OF PIX FILE
					
					x_size = ((fgetc (file_in)) << 8) + fgetc (file_in);
					y_size = ((fgetc (file_in)) << 8) + fgetc (file_in);
					total_pixels = x_size * y_size;
					
					sprintf(log, "    - Converting file '%s' [%d, %d]\n", pix_filename, x_size, y_size);
					logit (log);
				
					zero = fgetc (file_in);
					zero = fgetc (file_in);
					zero = fgetc (file_in);
					
					zero = fgetc (file_in);
					zero = fgetc (file_in);
					zero = fgetc (file_in);
					
					// CREATE HEADER DATA FOR RAW RGB
					
					fputc ('R', file_out);
					fputc ('A', file_out);
					fputc ('W', file_out);
					fputc (' ', file_out);
					fputc ('R', file_out);
					fputc ('G', file_out);
					fputc ('B', file_out);
					fputc (' ', file_out);
					
					fputc ((x_size>>8) & 255, file_out);
					fputc (x_size & 255, file_out);
					
					fputc ((y_size>>8) & 255, file_out);
					fputc (y_size & 255, file_out);
					
					pixel_count = 0;
					while (pixel_count < total_pixels)
						{
						rlc = fgetc (file_in);
						red = fgetc (file_in);
						green = fgetc (file_in);
						blue = fgetc (file_in);
						
												
						pixel_count+=rlc;
						
						for (; rlc > 0; rlc--)
							{
							fputc (blue, file_out);
							fputc (green, file_out);
							fputc (red, file_out);
							}
						}
					
					if (pixel_count != total_pixels)
						{
						sprintf (log, "    - FILE '%s' HAS CONVERTED BADLY\n", pix_filename);
						logit (log);
						}
						
					pix_number+=1;	
					fclose (file_out);
					}
					
				fclose (file_in);
				}
			while (file_in != NULL);
			
			sprintf (log, "    - %d files converted from %s\n", pix_number-1, master_title);
			}
		}
	while (finished_master == 0);	
	fclose (file_master);
	printf("Pix files converted - See Log\n");
}
