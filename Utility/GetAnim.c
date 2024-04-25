#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	hdisk	"Moppit:"

char	string [128];

/***************************************************/

long	anim_convert(char* cptr)

{
char	c1=*cptr,
		c2=*(cptr+1);
		
	if (c2>='0' && c2<='9')
		return (((c1-48)*10)+(c2-48));
	else
		return (c1-48);
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

char*	anim_get (FILE* file_in, char* checkstring)
{

char	achar;
long	ccount,
		animlinefinished;

	do
		{
		ccount=0;
		animlinefinished=0;
		do
			{
			achar=fgetc (file_in);
			if (!feof (file_in))
				if (achar >=32)
					string[ccount++]=achar;
			if (feof (file_in))
				animlinefinished=1;
			}
		while (achar !=13 && animlinefinished==0);
	
		if (animlinefinished==0)
			{
			if (strncmp (checkstring, string, 3)==0)
				return (&string[3]);
			}
		}
	while (animlinefinished==0);
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

char*	anim_line;

long	buildfile [4096];
long	buildoffset [1024];

FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

long	buildlen=0,
		buildptr=0,
		get_source=0,
		finished_master=0;

long	act0,
		act1,
		act2,
		animation_status,
		total_animations,
		animation_frames,
		number_sprites,
		repeat_frames,
		repeat_loop,
		sp_sheet,
		sp_row,
		sp_xpos,
		sprite;
		
		logit ("Converting Animation Files :\n\n");
		
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

// Convert animations file for planet type
// ---------------------------------------

		if (finished_master==0)
			{

// Open source animations file
			
			sprintf(&filename_in, "%scoded8bpp:%s32:Animation",hdisk,master_title);
			file_in=fopen(&filename_in, "rb");
	
			if (file_in != NULL)
				{
				sprintf(log, "Converting animations for ->%s\n",master_title);
				logit (log);
			
				buildlen=0;
				buildptr=0;
				
				anim_line = anim_get (file_in, "AA ");
				if (anim_line==NULL)	goto ended;
				total_animations=anim_convert(anim_line);
				buildoffset [buildlen++] = total_animations;
				
				for (act0=0; act0 < total_animations; act0++)
					{
					
					sprintf(log, "     - Converting animations %d\n",act0);
					logit (log);
				
					// MAIN ANIMATION FETCH LOOP
					
					buildoffset [buildlen++] = buildptr<<2;
					
					// Get Start Status of animation
					anim_line = anim_get (file_in, "SS ");
					if (anim_line==NULL)	goto ended;
					animation_status=anim_convert(anim_line);
					
					// Get Frame rate of animation
					anim_line = anim_get (file_in, "FR ");
					if (anim_line==NULL)	goto ended;
					animation_frames=anim_convert(anim_line);
				
					// Get Number of Sprites to Animate
					anim_line = anim_get (file_in, "NS ");
					if (anim_line==NULL)	goto ended;
					number_sprites=anim_convert(anim_line);
				
					// Get Number of Animation Repeat Frames
					anim_line = anim_get (file_in, "AF ");
					if (anim_line==NULL)	goto ended;
					repeat_frames=anim_convert(anim_line);
					
					// Get Animation Loop Point
					anim_line = anim_get (file_in, "AL ");
					if (anim_line==NULL)	goto ended;
					repeat_loop=anim_convert(anim_line);
							
					buildfile [buildptr++] = animation_status;		// WRITE STATUS
					buildfile [buildptr++] = 0;						// WRITE FRAME COUNT
					buildfile [buildptr++] = animation_frames-1;	// WRITE FRAME RESTORE
					buildfile [buildptr++] = number_sprites;		// WRITE NUMBER OF SPRITES
					buildfile [buildptr++] = 0;						// WRITE IN-SPRITE-ANIMCOUNT
					buildfile [buildptr++] = repeat_frames;			// WRITE IN-SPRITE-ANIMRESTORE
					buildfile [buildptr++] = repeat_loop;			// WRITE LOOP POINT
					
					// Read in animations data
					
					for (act1=0; act1<number_sprites; act1++)
						{
							
						// Get SpriteNumber to Animate
						
						anim_line = anim_get (file_in, "SP ");
						if (anim_line==NULL)	goto ended;
						sp_sheet=anim_convert(anim_line);
						sp_row=anim_convert(anim_line+2);
						sp_xpos=anim_convert(anim_line+4);
						sprite=(sp_sheet*40)+(sp_row*8)+sp_xpos;
						buildfile [buildptr++] = sprite;			// WRITE SPRITE TO ANIMATE
						
						for (act2=0; act2<repeat_frames; act2++)
							{
							// Get SpriteNumber to Animate To
							
							anim_line = anim_get (file_in, "AN ");
							if (anim_line==NULL)	goto ended;
							sp_sheet=anim_convert(anim_line);
							sp_row=anim_convert(anim_line+2);
							sp_xpos=anim_convert(anim_line+4);
							sprite=(sp_sheet*40)+(sp_row*8)+sp_xpos;
							buildfile [buildptr++] = sprite;		// WRITE SPRITE TO ANIMATE TO
							}
						}
					}
ended:

			// Open group output file for planet
	
			sprintf(&filename_out,"%sCoded8bpp:Animations:%s.anim",hdisk,master_title);
			file_out=fopen(filename_out,"wb");
			
			// Write output file
			
			for (get_source=1; get_source < buildlen; get_source++)
				buildoffset[get_source]+=(buildlen-1)<<2;
				
			for (get_source=0; get_source < buildlen; get_source++)
				fwrite (&buildoffset[get_source], sizeof(long), 1, file_out);
				
			for (get_source=0; get_source < buildptr; get_source++)
				fwrite (&buildfile[get_source], sizeof(long), 1, file_out);
			
			// Close output file
			fclose(file_out);
			fclose(file_in);
			}
		}
	}
	while (finished_master == 0);	
	fclose (file_master);
	printf("Animation files converted - See Log\n");
	
}
