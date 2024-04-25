#   File:       Get4.make
#   Target:     Get4
#   Sources:    Get4.c
#   Created:    Tuesday, May 2, 1995 10:50:50 am


OBJECTS = Get4.c.o



Get4 ÄÄ Get4.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o Get4
Get4.c.o Ä Get4.make Get4.c
	 C -r  Get4.c
