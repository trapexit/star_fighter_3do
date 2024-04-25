#include <stdio.h>
#define	hdisk	"Moppit:"

/***************************************************/

void putrun (unsigned char* rle, long run_number, unsigned char run_char, long num_bits)

{
long	bits_count;

	for (bits_count = 0; bits_count < num_bits; bits_count++)
		if ((run_char & (1 << bits_count)) != 0)
			rle [run_number + ((num_bits-1)-bits_count)] = 1;
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

void	main()
{

long	last,
		word,
		
		packed_count=0,
		cel_counter=0,
		get_source=0,
		finished=0,
		read_in=0,
		
		x_spr,
		y_spr,
		x_pos,
		y_pos,
		ref,
		
		run_number,
		run_scramble,
		run_scrambleinsert,
		run_offset,
		
		palette_map [16],
		palette_entries,
		palette_found,
		palette_search,
		
		colours [16] [16],
		colour;

char	filename_in [128],
		filename_out [128],
		master_filename [128],
		master_title [128],
		log	[256],
		run_count,
		run_char,
		palette_overflow,
		template [16] [16];
		
FILE*	file_master;
FILE*	file_in;
FILE*	file_out;

unsigned char 	rle [32768];
unsigned char	chunk [163840];
unsigned char	mchar;
unsigned char	x_pixel;

char	*wordin=(char*) &word;


/***************************************************/
	
	logit ("\n\nConversion of Master 16 cels\n");
	
	sprintf(&master_filename,"%scoded8bpp:MasterList",hdisk);
	file_master=fopen(master_filename,"rb");
	
	do
	{
	
	get_source=0;
	do
		{
		mchar=fgetc (file_master);
		if (feof (file_master))
			finished  = 1;
		else	
			if (mchar>=32)
				master_title[get_source++]=mchar;
		}
	while (mchar >=32 && finished==0);
	master_title[get_source]=0;
	
	if (finished==0)
	{
	
	sprintf(log, "\n    - Conversion of %s16\n",master_title);
	logit (log);
	
	cel_counter=0;
	
	sprintf(&filename_in, "%scoded8bpp:%s16:%s16.cel",hdisk,master_title,master_title);
	file_in=fopen(&filename_in, "rb");
		
		// SHEET HAS BEEN OPENED FOR READ - GET ALL PIXEL DATA INTO 320 X 256 ARRAY
		// ------------------------------------------------------------------------
		
		if (file_in != NULL)
			{
			fseek (file_in, 88, SEEK_SET);
			
			for (get_source=0; get_source<163840; get_source++)
				chunk [get_source] = fgetc (file_in);
			fclose(file_in);
			
			cel_counter=0;
			packed_count=0;
			
			// READ SPRITE INTO 16 X 16 ARRAY
			// ------------------------------
			
			do
				{
				y_spr = cel_counter >> 4;
				x_spr =  cel_counter &15;
				
				last = 1;
				
				for (y_pos=0; y_pos<16; y_pos++)
					for (x_pos=0; x_pos<16; x_pos++)
						{
						ref=(153600 + (x_spr * 40) + (x_pos*2) - (y_spr * 12800) + (y_pos * 640));
						colours [x_pos] [y_pos] = ((chunk [ref] << 8) | chunk [ref+1]) & 32767;		// col long
						if (colours [x_pos] [y_pos] != 0)
							last = 0;
						}
				
				if (last == 0)
					{
					
					// Analyse palette entries for sprite
					// ----------------------------------
					
					for (get_source=0; get_source < 16; get_source++)
						palette_map [get_source]= 0;
					palette_entries = 0;
					
					palette_overflow = 0;
					for (y_pos=0; y_pos<16; y_pos++)
						for (x_pos=0; x_pos<16; x_pos++)
							{
							colour = colours [x_pos] [y_pos];						// Get pixel from sprite
							palette_found = -1;										// Is pixel already registered in the palette ?

							if (palette_entries > 0)
								for (palette_search = 0; palette_search < palette_entries; palette_search++)
									if (colour == palette_map [palette_search])
										palette_found = palette_search;				// Yes, palette entry has been found already
	
							if (palette_found == -1)
								{
								if (palette_entries < 16)
									{
									palette_found = palette_entries;
									palette_map [palette_entries++] = colour;		// Register in palette
									}
								else
									{
									palette_overflow = 1;							// Too many palette entries !!!
									palette_found = 0;								// Just put in colour 0 FOR THE MOMENT !!!!
									}
								}
							template [x_pos] [y_pos] = palette_found;				// Register colour entry in sprite
							}
					
					if (palette_overflow == 1)
						{
						sprintf(log, "    - ERROR : SPRITE %d Has too many colours !\n",cel_counter);
						logit (log);
						}
						
					for (get_source = 0; get_source < 32768; get_source++)			// Clear all commands
						rle [get_source] = 0;
					run_number = 0;
					
					// Is the cel smaller when packed - Encode it
					// ------------------------------------------
					
					for (y_pos=0; y_pos<16; y_pos++)								// Y line count
						{
						
						run_offset = run_number;
						run_number+=8;
						
						// Encode a single row
						// -------------------
						
						x_pixel =0;													// Set to first pixel
						run_scramble = 0;
						
						do
							{
							run_char = template [x_pixel] [y_pos];					// Get pixel
							run_count = 0;
							
							do
								{
								if (run_count++ >= 1 && run_scramble > 0)							// Is this the end of a literal ?
									{
									putrun (rle, run_scrambleinsert, (1 << 6) | (run_scramble-1), 8);	// Yes, insert literal com&count
									run_scramble=0;													// Clear literal count
									}
								x_pixel+=1;															// Move onto next pixel
								}
							while ((template [x_pixel] [y_pos] == run_char) && x_pixel < 16);		// Until not the same or EOL
							
							// End of repeat !

							if (run_count > 1)														// Was the run greater then 1 ?
								{
								putrun (rle, run_number, (3 << 6) | (run_count-1), 8);				// YES, put in repeat com&count
									run_number+=8;
								putrun (rle, run_number, run_char, 4);								// And the character
									run_number+=4;
								}
							else
								{
								if (run_scramble++ == 0)											// No, a literal - first one ?
									{
									run_scrambleinsert = run_number;								// Yes, mark position & move com
									run_number+=8;
									}
								putrun (rle, run_number, run_char, 4);								// Insert literal character
								run_number+=4;
								
								if (x_pixel >= 16 && run_scramble >= 1)
									putrun (rle, run_scrambleinsert, (1 << 6) | (run_scramble-1), 8);	// Insert literal com&count @ end
								}
							}
						while (x_pixel < 16);														// Until end
					
						putrun (rle, run_number, 0, 4);												// Put in EOL character
						run_number+=4;
						
						// Insert offset to next
						// ---------------------
						
						run_number = ((run_number + 31) & 524256);									// Make next word boundary
						if ((run_number - run_offset) < 64)
							run_number+=32;															// Make at least 2 words per
							
						putrun (rle, run_offset, (((run_number - run_offset)>>3)-8)>>2, 8);	// Put in offset to next
					}
					
					// Re-Encode packed cel data into bytes
					// ------------------------------------
					
					run_number = run_number >> 3;										// Put counter into bytes
					
					for (run_offset = 0; run_offset < run_number; run_offset++)
						{
						run_scramble = run_offset << 3;
						rle [run_offset] = (	(rle [run_scramble + 0] << 7) |
														(rle [run_scramble + 1] << 6) |
														(rle [run_scramble + 2] << 5) |
														(rle [run_scramble + 3] << 4) |
														(rle [run_scramble + 4] << 3) |
														(rle [run_scramble + 5] << 2) |
														(rle [run_scramble + 6] << 1) |
														(rle [run_scramble + 7] << 0) );
													}

					// Write to output file
					// --------------------
					
					sprintf(&filename_out,"%scoded8bpp:%s16:Processed:S_%d.data",hdisk,master_title, cel_counter);
					file_out=fopen(filename_out,"wb");
						
					if (run_number < 128)
						
						// Write packed data to coded4 cel file
						// ------------------------------------
						
						{
						
						packed_count+=1;
						
						// Set important bits of flags word
					
						word = 	(
							(1<<29) | (1<<28) |	(1<<27) | (1<<26) |	(1<<25) | (1<<24) |	(1<<23) | (1<<21) |	
							(1<<18) | (1<<17) |	(1<<14) | (1<<9) | (1<<8) | (1<<10) | (1<<5) | (1<<19)
							);
									
						// Write flags word
					
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PLUT Data
						
						for (get_source=0; get_source < 16; get_source++)
							{
							fputc (palette_map [get_source] >>8, file_out);
							fputc (palette_map [get_source] & 255, file_out);
							}
							
						// Write PREAMBLE WORD 0
						
						word = (1<<31) | (15<<6) | 3;                                             
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PACKED cel data
						
						for (get_source=0; get_source<run_number; get_source++)
							fputc (rle [get_source],file_out);
						}
						
						else

						// Write unpacked data to coded4 cel file
						// --------------------------------------
						
						{
						
						// Set important bits of flags word
					
						word = 	(
							(1<<29) | (1<<28) |	(1<<27) | (1<<26) |	(1<<25) | (1<<24) | (1<<23) | (1<<21) |
							(1<<18) | (1<<17) |	(1<<14) | (1<<8) | (1<<5) | (1<<19) | (1<<10)
							);
									
						// Write flags word
					
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PLUT Data
						
						for (get_source=0; get_source < 16; get_source++)
							{
							fputc (((palette_map [get_source]) >>8) & 255, file_out);
							fputc ((palette_map [get_source]) & 255, file_out);
							}
							
						// Write PREAMBLE WORD 0
						
						word = (15<<6)  | 3;
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write PREAMBLE WORD 1
						
						word = (1<<14) | 15;
						fputc (*(wordin+0), file_out);
						fputc (*(wordin+1), file_out);
						fputc (*(wordin+2), file_out);
						fputc (*(wordin+3), file_out);
						
						// Write Unpacked pixel data
							
						for (y_pos = 0; y_pos < 16; y_pos++)
							for (x_pos = 0; x_pos < 16; x_pos+=2)
								fputc (	(((template [x_pos] [y_pos])<<4) | template [x_pos+1] [y_pos]) ,file_out);
						}
					
					fclose(file_out);
					
					// Write to unpacked output file for 4x4 conversion
					// ------------------------------------------------
					
					sprintf(&filename_out,"%scoded8bpp:%s16:Unpacked:S_%d.data",hdisk,master_title, cel_counter);
					file_out=fopen(filename_out,"wb");
					for (y_pos = 0; y_pos < 16; y_pos++)
							for (x_pos = 0; x_pos < 16; x_pos++)
								{
								fputc (colours [x_pos] [y_pos] >> 8, file_out);
								fputc (colours [x_pos] [y_pos] & 255, file_out);
								}
					fclose(file_out);
						
					cel_counter ++;
					}
				}
			while (last == 0);

			sprintf(log, "    - %s16 Converted %d sprites, %d (%d%%) were packed\n", master_title, cel_counter, packed_count, ((100*packed_count)/cel_counter));
			logit (log);
	
			}
		}
	}
	while (finished==0);
	printf("Conversion finished - see log file\n");
	fclose (file_master);
	return;
}