#   File:       PIX_RAW.make
#   Target:     PIX_RAW
#   Sources:    PIX_RAW.c
#   Created:    Thursday, September 14, 1995 7:13:03 pm


OBJECTS = PIX_RAW.c.o



PIX_RAW ÄÄ PIX_RAW.make {OBJECTS}
	Link -t APPL -c '????' ¶
		{OBJECTS} ¶
		"{CLibraries}"CSANELib.o ¶
		"{CLibraries}"Math.o ¶
		#"{CLibraries}"Complex.o ¶
		"{CLibraries}"StdClib.o ¶
		"{Libraries}"Runtime.o ¶
		"{Libraries}"Interface.o ¶
		-o PIX_RAW
PIX_RAW.c.o Ä PIX_RAW.make PIX_RAW.c
	 C -r  PIX_RAW.c
