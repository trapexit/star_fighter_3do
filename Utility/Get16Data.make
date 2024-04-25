#   File:       Get16Data.make
#   Target:     Get16Data
#   Sources:    Get16Data.c
#   Created:    Tuesday, April 11, 1995 10:57:40 am


OBJECTS = Get16Data.c.o



Get16Data ÄÄ Get16Data.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o Get16Data
Get16Data.c.o Ä Get16Data.make Get16Data.c
	 C -r  Get16Data.c
