#   File:       GetTogetherGameHW.make
#   Target:     GetTogetherGameHW
#   Sources:    GetTogetherGameHW.c
#   Created:    Thursday, September 28, 1995 5:20:51 pm


OBJECTS = GetTogetherGameHW.c.o



GetTogetherGameHW ÄÄ GetTogetherGameHW.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o GetTogetherGameHW
GetTogetherGameHW.c.o Ä GetTogetherGameHW.make GetTogetherGameHW.c
	 C -r  GetTogetherGameHW.c
