#   File:       GetInfo.make
#   Target:     GetInfo
#   Sources:    GetInfo.c
#   Created:    Monday, June 19, 1995 8:24:47 pm


OBJECTS = GetInfo.c.o



GetInfo ÄÄ GetInfo.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o GetInfo
GetInfo.c.o Ä GetInfo.make GetInfo.c
	 C -r  GetInfo.c
