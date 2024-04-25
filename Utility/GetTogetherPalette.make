#   File:       GetTogetherPalette.make
#   Target:     GetTogetherPalette
#   Sources:    GetTogetherPalette.c
#   Created:    Wednesday, March 1, 1995 11:28:10 am


OBJECTS = GetTogetherPalette.c.o



GetTogetherPalette ÄÄ GetTogetherPalette.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o GetTogetherPalette
GetTogetherPalette.c.o Ä GetTogetherPalette.make GetTogetherPalette.c
	 C -r  GetTogetherPalette.c
