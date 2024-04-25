#   File:       Get32.make
#   Target:     Get32
#   Sources:    Get32.c
#   Created:    Tuesday, April 11, 1995 8:27:59 pm


OBJECTS = Get32.c.o



Get32 ÄÄ Get32.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o Get32
Get32.c.o Ä Get32.make Get32.c
	 C -r  Get32.c
