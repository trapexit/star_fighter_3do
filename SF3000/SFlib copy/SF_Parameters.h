/* File : Game Parameters header file */

#ifndef __SF_PARAMETERS
#define __SF_PARAMETERS

// Definitions

#define MENUSWITCH_TIMEOUT	2048

		
// Structure Definitions

typedef struct 	game_parameters
				{
				long	level;
				long	menu_display;
				long	display_timer;
				long	demo_counter;
				long	demo_or_credits;
				long	display_hiscoretable;
				} game_parameters;

typedef struct high_score			{
									char	name [20];
									long	score;
									}	high_score;
								
typedef struct game_configuration	{

									char		version;
									char		free1;
									char		free2;
									char		free3;
									
									char		language;
									char		control_method;
									char		music_volume;
									char		sound_volume;
									
									char		music_on;
									char		sound_on;
									char		free4;
									char		free5;
									
									char		flight_controls [12];
									
									long		stick_x_min;
									long		stick_x_max;
									long		stick_y_min;
									long		stick_y_max;
									long		stick_z_min;
									long		stick_z_max;
									
									char		music_tracks [8];
									char		pilot	[20];
									high_score	high_scores [4] [5];
									}	game_configuration;



#endif
