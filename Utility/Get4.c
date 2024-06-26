#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	hdisk	"Moppit:"

/***************************************************/

long pixel_read (long xl, long yl, FILE* file_read)
{

unsigned long pe;
long	xp,
		yp,
		dv=0,
		av_red=0,
		av_green=0,
		av_blue=0;
		
	
	for (yp = yl<<2; yp < (yl<<2)+2; yp+=4) 
		for (xp = xl<<2; xp < (xl<<2)+2; xp+=4)
			{
			fseek (file_read, (xp + (yp<<4)) << 1, SEEK_SET);
			pe = (fgetc (file_read)<<8) | fgetc (file_read);	// Get 16bit pixel
			av_blue+= pe & 31;									// add up instances of rgb components
			av_green+= (pe >> 5) & 31;
			av_red+= (pe >> 10) & 31;
			dv +=1;
			}

	av_red = av_red / dv;										// Average out RGB components
	av_green = av_green / dv;
	av_blue = av_blue / dv;
	
	return ((av_red<<10) | (av_green<<5) | av_blue);
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
			
void main()
{
		
char	filename_in [64],
		filename_out [64],
		master_filename [64],
		master_title [64],
		log [128],
		mchar;

unsigned char	byte1;
unsigned char	byte2;
unsigned char	x_line [4];
unsigned long	word_out;
unsigned char	*byte_out=(char*) &word_out;

long	palette_counter [32] [32] [32];
long	palette_list [32768] [2];

long	swap1,
		swap2,
		p1,
		p2,
		p3,
		p_entry;

FILE*	file_master;
FILE*	file_in;
FILE*	file_out;
FILE*	palette_out;

long	distance,
		dist_red,
		dist_green,
		dist_blue,
		max_dist,
		pixel,
		p_mode=0,
		
		cel_counter=0,
		get_source,
		finished=0,
		x_pos,
		y_pos,
		xloop,
		yloop,
		copy,
		
		palette_entry,
		palette_red,
		palette_green,
		palette_blue,
		
		red_value [32],
		green_value [32],
		blue_value [32];
		
/***************************************************/

	// Open the master file list
	// -------------------------
		
	logit ("\n4X4 Landscape Conversion\n\n");
	
	sprintf(&master_filename,"%scoded8bpp:MasterList",hdisk);
	file_master=fopen(master_filename,"rb");
	
	do
	{
	
	get_source=0;
	do
		{
		mchar=fgetc (file_master);
		if (feof (file_master))
			finished=1;
		else	
			if (mchar >=32)
				master_title[get_source++]=mchar;
		
		}
	while (mchar >=32 && finished==0);
	master_title[get_source]=0;
	
	if (finished==0)
	{

	sprintf(log, "\n    - Converting %s16\n", master_title);
	logit (log);
			
	// CONVERT PLANET'S PALETTE FILE
	// -----------------------------
	
	cel_counter=0;
	logit ("	- ANALYSING PALETTE ENTRIES\n");
	
	// Reset palette
	
	for (p1 = 0; p1 < 16; p1++)
		for (p2 = 0; p2 < 16; p2++)
			for (p3 = 0; p3 < 16; p3++)
				palette_counter [p1] [p2] [p3] = 0;
				
	// Total up all seperate references to palette colours
	
	do
		{
		sprintf(&filename_in, "%scoded8bpp:%s16:Unpacked:S_%d.data",hdisk,master_title,cel_counter);
		file_in=fopen(&filename_in, "rb");
			
		if (file_in != NULL)
			{
			for (y_pos =0; y_pos < 4; y_pos++)
				{
				for (x_pos = 0; x_pos < 4; x_pos++)
					{
					palette_entry = pixel_read(x_pos, y_pos, file_in);
					
					palette_blue = (palette_entry & 31)>>1;				// Get RGB components (cut off bottom bit)
					palette_green = ((palette_entry >> 5) & 31)>>1;
					palette_red = ((palette_entry >> 10) & 31)>>1;
					
					if (palette_blue <=7 && palette_green <=7 && palette_red <=7)
						palette_counter [palette_red<<1] [palette_green<<1] [palette_blue<<1] +=1;		// Reg halfbrite
					else
						palette_counter [palette_red] [palette_green] [palette_blue] +=1;				// Can't - Reg single
					}
				}
			fclose (file_in);
			cel_counter+=1;
			}
		}
	while (file_in != NULL);
	
	// Copy all palette counts into 1 linear list
	
	p_entry = 0;
	
	for (p1 = 0; p1 < 16; p1++)
		for (p2 = 0; p2 < 16; p2++)
			for (p3 = 0; p3 < 16; p3++)
				{
				palette_list [p_entry] [0] = (p1<<10) + (p2<<5) + p3;			// Copy over colour !
				palette_list [p_entry] [1] = palette_counter [p1] [p2] [p3];	// Copy over counter for colour
				p_entry+=1;
				}
	
	printf("%d colour entries sorted\n", p_entry);
	
	// Bubble sort list to get most common entries at the bottom
	
	p_entry = 0;
	
	do
		{
		if (palette_list [p_entry] [1] < palette_list [p_entry+1] [1])
			{
			swap1 = palette_list [p_entry] [0];
			swap2 = palette_list [p_entry] [1];
		
			palette_list [p_entry] [0] = palette_list [p_entry+1] [0];
			palette_list [p_entry] [1] = palette_list [p_entry+1] [1];
		
			palette_list [p_entry+1] [0] = swap1;
			palette_list [p_entry+1] [1] = swap2;
		
			if (p_entry > 0)
				p_entry-=1;
			}
		else
			p_entry+=1;
		}
	while (p_entry < 4095);
	
	// Palette list has now been sorted, copy over into actual palette
	
	logit ("	- PALETTE SORTED - COPYING AND SAVING\n");
	
	sprintf(&filename_out, "%sCoded8bpp:Palettes:%s.pal", hdisk,master_title);
	palette_out=fopen (filename_out,"wb");
	
	if (palette_out != NULL)
		{
			
		for (p_entry = 0; p_entry < 32; p_entry++)
			{
			red_value [p_entry] = (((palette_list [p_entry] [0])>>10) &31)<<1;
			green_value [p_entry] = (((palette_list [p_entry] [0])>>5) &31)<<1;
			blue_value [p_entry] = ((palette_list [p_entry] [0]) &31)<<1;
		
			sprintf (log, "        Palette entry %d has count of %d, RGB IS (%d, %d, %d)\n", p_entry, palette_list [p_entry] [1], red_value [p_entry], green_value [p_entry], blue_value [p_entry]);
			logit (log);
			
			palette_entry = (red_value [p_entry] << 10) | (green_value [p_entry] << 5) | blue_value [p_entry];
			byte1 = palette_entry>>8;
			byte2 = palette_entry & 255;
		
			fputc (byte1, palette_out);
			fputc (byte2, palette_out);
			}
		fclose (palette_out);
		logit("	- PALETTE COPIED AND SAVED\n");
		}
		
		
	// CONVERT PLANETS SPRITE FILES
	// ----------------------------
	
	cel_counter=0;
	
	sprintf(&filename_out,"%sgroupcel:%s.4",hdisk,master_title);
	file_out=fopen(filename_out,"wb");
	
	logit ("4x4 sprites\n");
	
	do
	{
		sprintf(&filename_in, "%scoded8bpp:%s16:Unpacked:S_%d.data",hdisk,master_title,cel_counter);
		file_in=fopen(&filename_in, "rb");
			
		if (file_in != NULL)
			{
			for (yloop=0; yloop<4; yloop+=1)
				{
				for (xloop=0; xloop<4; xloop+=1)
					{
					palette_entry = pixel_read(xloop, yloop, file_in);
					
					palette_blue = palette_entry & 31;						// Get RGB components
					palette_green = (palette_entry >> 5) & 31;
					palette_red = (palette_entry >> 10) & 31;
					
					// Find this pixels nearest equivalent colour in palette (including half brite)
					
					pixel = 0;
					max_dist = -1;
					
					for (copy = 0; copy < 32; copy++)
						{
						dist_red = palette_red - red_value [copy];
						dist_green = palette_green - green_value [copy];
						dist_blue = palette_blue - blue_value [copy];
						
						dist_red = (dist_red * dist_red);
						dist_green = (dist_green * dist_green);
						dist_blue = (dist_blue * dist_blue);
						
						distance = dist_red + dist_green + dist_blue;
						
						if (max_dist == -1 || distance < max_dist)
							{
							pixel = copy;
							p_mode = 0;
							max_dist = distance;
							}
						
						dist_red = palette_red - (red_value [copy] >> 1);
						dist_green = palette_green - (green_value [copy] >> 1);
						dist_blue = palette_blue - (blue_value [copy] >> 1);
						
						dist_red = (dist_red * dist_red);
						dist_green = (dist_green * dist_green);
						dist_blue = (dist_blue * dist_blue);
						
						distance = dist_red + dist_green + dist_blue;
						
						if (distance < max_dist)
							{
							pixel = copy;
							p_mode = 1;
							max_dist = distance;
							}
						
						}
							
					x_line [xloop] = pixel + (p_mode << 5);
					}
				
				// Write 4 pixels (6bpp each) to file (3 bytes in total)
				
				word_out = (
							x_line [0] << 26 |
							x_line [1] << 20 |
							x_line [2] << 14 |
							x_line [3] << 8
							);
				
				fputc (*(byte_out+0), file_out);
				fputc (*(byte_out+1), file_out);
				fputc (*(byte_out+2), file_out);
				fputc (0, file_out); 				// - We do not need to store the low byte
				}
					
			fclose(file_in);
			cel_counter++;
			}
		}
	while (file_in != NULL);

	fclose(file_out);
	sprintf(log, "    - %d sprites converted from %s16\n", cel_counter, master_title);
	logit (log);
	}
	}
	while (finished==0);
	
	printf("Conversion complete - See log\n");
	fclose (file_master);
}

