#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	hdisk	"Moppit:"


int	main()
{
		
char	filename_in [64],
		filename_out [64],
		master_filename [64],
		master_title [64],
		mchar;

char	buildfile [256*1024];
long	buildoffset [1024];

FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

char	*wordin,
		read;

long	buildlen=0,
		buildptr=0,
		word,
		cel_counter=0,
		get_source=0,
		finished_master=0;

		wordin=(char*) &word;

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

// Convert all S_xx.data files for planet type
// -------------------------------------------

		if (finished_master==0)
			{
			cel_counter=0;
	
			buildlen=0;
			buildptr=0;
			
// Main convert loop
			
			do
				{
			
// Open source cel file
			
				sprintf(&filename_in, "%scoded8bpp:%s16:Processed:S_%d.data",hdisk,master_title, cel_counter);
				file_in=fopen(&filename_in, "rb");
	
				if (file_in != NULL)
					{
			
					buildoffset [buildlen++] = buildptr;

					do
						{
						read = fgetc (file_in);
						if (!feof (file_in))
							buildfile [buildptr++] = read;
						}
					while (!feof (file_in));
					
					buildptr+=3;
					buildptr &= 262140;				// Word align for next
					fclose(file_in);
					}

				cel_counter++;
				}
			while (file_in != NULL);
			
			
			// Open group output file for planet
	
			sprintf(&filename_out,"%sgroupcel:%s.16",hdisk,master_title);
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
			printf("%d files read from %s\n",cel_counter-1,master_title);
			
			}
			
		}
	while (finished_master == 0);	
	fclose (file_master);
	
}