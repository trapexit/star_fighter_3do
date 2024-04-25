#   File:       Get32Data.make
#   Target:     Get32Data
#   Sources:    Get32Data.c
#   Created:    Wednesday, April 12, 1995 10:05:38 am


OBJECTS = Get32Data.c.o



Get32Data ÄÄ Get32Data.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o Get32Data
Get32Data.c.o Ä Get32Data.make Get32Data.c
	 C -r  Get32Data.c
