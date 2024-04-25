#   File:       GetTogetherGame1.make
#   Target:     GetTogetherGame1
#   Sources:    GetTogetherGame1.c
#   Created:    Wednesday, March 15, 1995 3:24:10 pm


OBJECTS = GetTogetherGame1.c.o



GetTogetherGame1 ÄÄ GetTogetherGame1.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o GetTogetherGame1
GetTogetherGame1.c.o Ä GetTogetherGame1.make GetTogetherGame1.c
	 C -r  GetTogetherGame1.c
