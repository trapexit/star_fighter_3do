		EXPORT	arm_addpolycel16, arm_addpolycel32, arm_setpolycel32palette, arm_add4cel4, arm_addgamecel
		EXPORT	arm_celinitialisecreation, arm_generatemaps, arm_addcelfrom512map
		EXPORT	arm_addcelfrom128map, arm_addcelfrom32map, arm_initialisecache
		EXPORT	arm_updatecache, arm_addmonocel, arm_setcel_hw
		EXPORT	arm_interceptplot, arm_setgamecelpalette
		
		IMPORT	screen_intercept
	
		AREA |c$$code|,CODE,READONLY

cel_maxhold	EQU	1024		; Max number of temporary cels allowed - Also in Allocation.h - Also in ARMSky.h
MAX_SHADE	EQU 31			; Max shade available
MAX_SGAME	EQU 10			; Max shade available for game cels
MIN_SHADE	EQU 0

; ---------------------------------------------------------------------------------------------------

; SF_ARMCEL

; Routines for manipulating and plotting polygons / sprites

; All things plotted on the screen must be entered into a linked list of CCB's which are then
; plotted in one go ...

; Cels cen be added from the 	16x16 sprite list, (for landscape sprites)
;								32x32 sprite list, (for ship / object textures)
;								4x4 sprite list, (used to custom build a 16x16 sprite from the map)
;								game list (fixed game sprites, any size)
;								512x512 map, 128x128 map or 32x32 map
;

; For speed, the CCB temporary list is already pre-initialised with the correct flags, when a cel
; is added, only the relavent sections of the CCB are updated.
; Every time a cel is added to the temporary list, if the maximum is exceeded, the list is plotted
; and reset prior to adding the new one.


; arm_addpolycel16 -

; Passed a pointer to a cel-adder control block (co-ordinates etc.), a pointer to the 16x16
; sprite bit-map data, and a texture number, this routine creates a textured 4point polygon which
; can have a shade value of 0 (dark) to 7 (brightest)


; arm_addpolycel32 -

; As with arm_polycel16, this routine creates a 4point textured polygon, but is optimised for
; applying 32x32 textured sprite
; NOTE : If this routine is called with a texture number >160, then arm_addmonocel is called instead
; with 160 subtracted for colour lookup, R3 (the plot type) is set to opaque


; arm_addgamecel -

; Passed a constant game-sprite (number, x, y position, shade value and x&y scales in quad datablock),
; adds a constant game-sprite to the plotting list. All sprite references are offset through
; an indirection list to allow variable sized sprites.


; arm_setgamecelpalette -

; Passed a constant game-sprite palette - this sets the plutptr of the last added sprite to this plut
; PLUTpalettes occupy 1 sprite space as with normal gamecels, but have no source data


; arm_addmonocel -

; Passed a logical colour number in R2, adds a 4point polygon using a 2x2 colour texture from the
; palette table. R3 is plot type, 0-Opaque, 1-Transparent


; arm_add4cel4 -

; This routine is used for distant land plotting, where for speed 16 small landscape sprites are
; plotted in one go. Passed a pointer to the map and an x,y position, it first creates a 16x16
; uncoded 8bpp sprite containing 16 land sprites (4x4 pixels each), which is then added (as in
; arm_addpolycel16)

; NOTE - These are now cached since they tend to stay on the screen for a while, this saves
; building 40k of sprite data per frame


; arm_initialisecache -

; Sprite data created using arm_add4cel4 is now cached for speed, this routine should be called
; at the start of every mission to reset the building cache pointers


; arm_celinitialisecreation -

; When a cel is created using arm_add4cel4, the sprite data used for the texture is stored in
; 'cel_creation'. Each sprite occupies 264 bytes, 8 for the preamble words, and 256 for the sprite
; data itself. This routine accepts a pointer to this block of memory, and the number of areas
; to pre-initialise (defined as CEL_MAXCREATION in celutils.h). It then initialises the 2 preamble
; words at the start of each area, leaving the 256 bytes free to receive a bitmap.

;	Cel adder control block [14 words]
;	----------------------------------

; +0	cel_plot counter
; +4	x point 1			NOTE :	These 4 screen co-ordinates have their origin at 160,120
; +8	y point 1					(screen centre)				
; +12	x point 2
; +16	y point 2
; +20	x point 3
; +24	y point 3
; +28	x point 4
; +32	y point 4

; +36	address of cel temporary list
; +40	shading value (0 dark, to x bright)

; +44	address of 16bit uncoded palette (128 entries)
; +48	address of cel creation source block
; +52	address of 4x4 compact landscape sprites
; +56	address of 16x16 planet landscape texture sprites
; +60	address of 32x32 planet object texture sprites
; +64	address of fixed game sprites list
; +68	address of 512x512 area map
; +72	address of 128x128 area map
; +76	address of creation cache used block
; +80	address of creation cache free list
; +84	cache free counter

; +88	screen address 0
; +92	screen address 1
; +96	address of 32x32 area map
; +100	address of 32 entry coded6 plut
; +104	free

;	CCB structure : (17 Words MAX)
;	------------------------------

;	uint32 ccb_Flags;						-> Already set up in temporary CCB
;	struct CCB *ccb_NextPtr;				-> Set to point to the next temp cel
;	CelData    *ccb_SourcePtr;				-> Set to point to the pixel data
;	void       *ccb_PLUTPtr;				-> Set to point to the palette lookup

;	Coord  ccb_XPos;						-> These are set to wrap to a 4 point polygon
;	Coord  ccb_YPos;
;	int32  ccb_HDX;
;	int32  ccb_HDY;
;	int32  ccb_VDX;
;	int32  ccb_VDY;
;	int32  ccb_HDDX;
;	int32  ccb_HDDY;
	
;	uint32 ccb_PIXC;						-> Set for shading value
;	uint32 ccb_PRE0;						-> Ignored (already set in pixel source data)
;	uint32 ccb_PRE1;						-> Ignored (already set in pixel source data)
;	int32  ccb_Width;						-> IGNORED
;	int32  ccb_Height;						-> IGNORED

; ---------------------------------------------------------------------------------------------------

;*******************************
arm_addpolycel16			ROUT
;*******************************

; Receives :	R0 = address of cel adder control block
; 				R1 = texture number to apply

; Returns :		Nothing
; 				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	r0-r3

; ----------
	
	stmfd sp!,{r4-r11,r14}	; Store stack + registers							

; ----------
	
	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing

; ----------

	ldr r2,[r0,#56]			; Base address of 16x16 landscape sprites
	ldr r4,[r2,r1,asl#2]	; Get offset to spritedata
	add r3,r2,r4			; R3 now points to the base of our internal spritedata
	
	ldr r2,[r3],#4			; r2 are FLAGS
	ldmia r0,{r4-r12,r14}	; Load in cel setup data
	
; ----------

	ldr r14,[r14,r4,asl #2]	; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1			; Increment temp pointer
	str r4,[r0]				; Store temp pointer

; ----------

	ldr r4,[r0,#40]			; Shading value
	adr r1,arm_pixcshade_type1

	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,#MAX_SHADE
	movgt r4,#MAX_SHADE
	ldr r4,[r1, r4, asl#2]	; Get PIXC Shade
	
	str r4,[r14,#48]		; Store the PIXC Word
	str r2,[r14],#8			; Store the control flags
		
	add r2,r3,#32			; Move sourceptr over plot data
	stmia r14!,{r2, r3}		; Store SOURCEPTR then PLUTPTR

; ----------

; STAGE 1 - X,Y Start position

	mov r0,#160<<16			; Centre x value
	add r0,r0,r5,asl #16;
	
	mov r1,#120<<16			; Centre y value
	add r1,r1,r6,asl #16;

; ----------

; STAGE 2 - Calculate horizontal scale values
	
	sub r2,r7,r5
	mov r2,r2,asl #16
	
	sub r3,r8,r6
	mov r3,r3,asl #16
	
	stmia r14!,{r0-r3}

; ----------

; STAGE 3 - Calculate vertical scale values
	
	sub r0,r11,r5
	mov r0,r0,asl #12
	
	sub r1,r12,r6
	mov r1,r1,asl #12

; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values

	sub r2,r9,r11
	sub r2,r2,r7
	add r2,r2,r5
	mov r2,r2,asl #12
	
	sub r3,r10,r12
	sub r3,r3,r8
	add r3,r3,r6
	mov r3,r3,asl #12

	stmia r14!,{r0-r3}

; ----------

	ldmfd sp!,{r4-r11,pc}^	; Exit routine
				

;*******************************
arm_addpolycel32			ROUT
;*******************************
	
; Receives :	R0 = address of cel adder control block
; 				R1 = texture number to apply

; Returns :		Nothing
; 				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	r0-r3

; ----------
	
	stmfd sp!,{r4-r11,r14}		; Store stack + registers						

; ----------

	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing

; ----------

	ldr r2,[r0,#60]			; Base address of 32x32 object texture sprites
	ldr r4,[r2,r1,asl#2]	; Get offset to spritedata
	add r3,r2,r4			; R3 now points to the base of our internal spritedata
	ldmia r0,{r4-r12,r14}	; Load in cel setup data
	
; ----------

	ldr r14,[r14,r4,asl #2]	; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1			; Increment temp pointer
	str r4,[r0]				; Store temp pointer

; ----------
	
	ldr r4,[r0,#40]			; Get Shading value
	ldmia r3!,{r0,r2}		; R0 is size shift (in bits 0-7), and transparency in bits (15 - 8)
							; R2 is FLAGS for sprite
	
	mov r1,r0, lsr#8		; Get Transparency
	cmp r1,#0				; Are we using transparency ?
		
	moveq r1,#MAX_SHADE		; No  - Cap shade to 30 (max non transparent)
	movne r1,#MAX_SGAME		; Yes - Cap shade to 10 (max transparent)
	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,r1
	movgt r4,r1

	mov r1,r0, lsr#8		; Get Transparency
	cmp r1,#1				; Are we using transparency ?
	
	adrlt r1,arm_pixcshade_type1	; Get PIXC base for non transparent
	adreq r1,arm_pixcshade_type3	; Get PIXC base for transparent add shading
	adrgt r1,arm_pixcshade_type4	; Get PIXC base for transparent mix shading
	
	ldr r4,[r1, r4, asl#2]	; Get PIXC Shade
	
	str r4,[r14,#48]		; Store the PIXC Word
	str r2,[r14],#8			; Store the control flags
	add r2,r3,#32
	stmia r14!,{r2,r3}		; Store SOURCEPTR, then PLUTptr

; ----------

; STAGE 1 - Calculate horizontal scale values
	
	; Work out bitshift value for size
	
	and r0,r0,#255			; Get just bitsize in r0
	rsb r1,r0,#18
	
	sub r3,r7,r5
	mov r3,r3,asl r1
	
	sub r4,r8,r6
	mov r4,r4,asl r1

; ----------

; STAGE 2 - X,Y Start position

	mov r1,#160<<16			; Centre x value
	add r1,r1,r5,asl #16;
	
	mov r2,#120<<16			; Centre y value
	add r2,r2,r6,asl #16;

	stmia r14!,{r1-r4}

; ----------

; STAGE 3 - Calculate horizontal / vertical scale increment values

	; Work out bitshift value for size
	
	mov r1,r0, asl#1
	rsb r1,r1,#16
	
	sub r3,r9,r11
	sub r3,r3,r7
	add r3,r3,r5
	mov r3,r3,asl r1
	
	sub r4,r10,r12
	sub r4,r4,r8
	add r4,r4,r6
	mov r4,r4,asl r1

; ----------

; STAGE 4 - Calculate vertical scale values
	
	; Work out bitshift value for size
	
	rsb r0,r0,#14
	
	sub r1,r11,r5
	mov r1,r1,asl r0
	
	sub r2,r12,r6
	mov r2,r2,asl r0

	stmia r14!,{r1-r4}

; ----------

	ldmfd sp!,{r4-r11,pc}^	; Exit routine
	

;*******************************
arm_setpolycel32palette		ROUT
;*******************************

; Receives :	R0 = address of cel adder control block
; 				R1 = polycel32 sprite palette number

; Returns :		Nothing
; 				[ Last cel added has it's palette pointer changed to point to this one's palette pointer ]

; Corrupts :	r0-r3

; ----------
	
	ldr r2,[r0,#0]			; Get last cel added
	subs r2,r2,#1
	movmis pc,r14			; If none, return
	
; ----------

	ldr r3,[r0,#60]			; Base address of poly32 cels
	ldr r1,[r3,r1,asl #2]	; Get offset to actual sprite palette data
	add r3,r3,r1			; R3 now contains actual address of sprite plut data

; ----------

	ldr r1,[r0,#36]			; Address of temp plotting ccb list
	ldr r1,[r1,r2,asl #2]	; Get address of THIS temp (Dest) CCB 	(r1) - ccb we are writing to
	add r3,r3, #8			; Point at Plutdata of passed cel
	str r3, [r1, #12]		; Store new plutptr
	movs pc,r14
	
;*******************************
arm_addgamecel				ROUT
;*******************************

; Receives :	R0 = address of cel adder control block
; 				R1 = game sprite number to add to plotting list
;				R2 = X Scale value (1024 = Normal Scale)
;				R3 = Y Scale value (1024 = Normal Scale)

; Returns :		Nothing
; 				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	r0-r3

; ----------

	stmfd sp!,{r4-r11, r14}	; Store stack + registers								

; ----------
	
	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing

; ----------

	ldr r4,[r0,#64]			; Base address of constant game sprites
	ldr r8,[r4,r1,asl #2]	; Get offset to actual sprite data start
	add r8,r4,r8			; R8 now contains actual address of Flags control word
	ldmia r8!,{r6,r7}		; r6 = PIXC Control, r7 = Flags, R8 now points to start of sprite data (OR Plut)

; ----------

	ldr r4,[r0,#0]			; temp cel counter
	ldr r5,[r0,#36]			; address of temp plotting ccb list

	ldr r10,[r5,r4,asl #2]	; Get address of THIS temp (Dest) CCB 	(r5) - ccb we are writing to
	add r4,r4,#1			; Increment temp pointer
	str r4,[r0]				; Store temp pointer

; ----------

	ldr r4,[r0,#40]				; Shading value
	adr r1,arm_pixcshade_type2	; Shading PIXC list
	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,#MAX_SGAME
	movgt r4,#MAX_SGAME
	ldr r4,[r1, r4, asl#2]		; Get PIXC Shade
	orr r6,r6,r4				; Set bits in existing PIXC word
	
	str r6,[r10,#48]		; Store the PIXC Word
	str r7,[r10],#8			; Store Flags word
	tst r7,#1<<23			; Does this game cel want to load in a plut ?
	
	streq r8,[r10],#8		; Store SourcePtr - no plut load
	addne r1,r8,#64
	stmneia r10!,{r1, r8}	; Store SourcePtr and PlutPtr - we are loading both

; ----------

; STAGE 1 - X,Y Start position

	ldmib r0,{r0,r1}		; Get x and y position
	
	add r0,r0,#160			; Centre x value
	mov r0,r0,asl #16		; Scale up X position
	
	add r1,r1,#120			; Centre y value
	mov r1,r1,asl #16		; Scale up Y position

; ----------

; STAGE 2 - Calculate horizontal scale values
	
	mov r4,r2, asl#10
	mov r5,#0
	
; ----------

; STAGE 3 - Calculate vertical scale values

	mov r6,#0
	mov r7,r3,asl #6

; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values

	mov r8,#0
	mov r9,#0

	stmia r10!,{r0-r1,r4-r9}	

; ----------

	ldmfd sp!,{r4-r11,pc}^	; Exit routine

; ----------
99	
	DCD 0					; Stack store


;*******************************
arm_setgamecelpalette		ROUT
;*******************************

; Receives :	R0 = address of cel adder control block
; 				R1 = game sprite palette number

; Returns :		Nothing
; 				[ Last cel added has it's palette pointer changed to point to this one's palette pointer ]

; Corrupts :	r0-r3

; ----------
	
	ldr r2,[r0,#0]			; Get last cel added
	subs r2,r2,#1
	movmis pc,r14			; If none, return
	
; ----------

	ldr r3,[r0,#64]			; Base address of constant game sprites
	ldr r1,[r3,r1,asl #2]	; Get offset to actual sprite palette data
	add r3,r3,r1			; R3 now contains actual address of sprite plut data

; ----------

	ldr r1,[r0,#36]			; Address of temp plotting ccb list
	ldr r1,[r1,r2,asl #2]	; Get address of THIS temp (Dest) CCB 	(r1) - ccb we are writing to
	str r3, [r1, #12]		; Store new plutptr
	movs pc,r14


;*******************************
arm_addmonocel_opaque		ROUT
;*******************************

; This is a drop through to add monocel, setting opaque plotting (if called from arc_addcel32)

	mov r3,#0


;*******************************
arm_addmonocel				ROUT
;*******************************

; Receives :	R0 = address of cel adder control block
;				R1 = UNUSED
; 				R2 = Logical Colour Number (0-x)
;				R3 = Plot type, (0) - Opaque,
;								(1) - Transparent MIX Shading
;								(2) - Transparent ADD Shading

; Returns :		Nothing
; 				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	r0-r3

; ----------
	
	stmfd sp!,{r4-r11,r14}	; Store stack + registers						

; ----------

	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing

; ----------

	ldr r1,[r0,#44]			; Base address palette entries
	add r2,r1,r2,asl#3		; R2 now points to actual palette sprite pixel entry
	ldmia r0,{r4-r12,r14}	; Load in cel setup data

; ----------

	ldr r14,[r14,r4,asl #2]	; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1			; Increment temp pointer
	str r4,[r0]				; Store temp pointer
	
; ----------
	
	ldr r4,arm_cel_setccbflag_monochrome	; Get Flags
	str r4,[r14],#8							; Store flags
	str r2,[r14],#8							; Store SOURCEPTR

	cmp r3,#1						; Are we using transparency ?
	adrlt r3,arm_pixcshade_type1	; No - Get PIXC base for non transparent
	movlt r1,#MAX_SHADE				; No  - Cap shade to 30 (max non transparent)
	
	adreq r3,arm_pixcshade_type4	; Mix Transparency ? Yes - Get PIXC base for transparent
	adrgt r3,arm_pixcshade_type3	; Add Transparency ? Yes - Get PIXC base for transparent
	movge r1,#MAX_SGAME				; Either transparency ? Yes - Cap shade to 10 (max transparent)
	
	ldr r4,[r0,#40]					; Shading value
	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,r1
	movgt r4,r1

	ldr r4,[r3, r4, asl#2]			; Get PIXC Shade

	
; ----------

; STAGE 1 - X,Y Start position

	mov r0,#160<<16			; Centre x value
	add r0,r0,r5,asl #16;
	
	mov r1,#120<<16			; Centre y value
	add r1,r1,r6,asl #16;

; ----------

; STAGE 2 - Calculate horizontal scale values
	
	sub r2,r7,r5
	mov r2,r2,asl #20
	
	sub r3,r8,r6
	mov r3,r3,asl #20
	
	stmia r14!,{r0-r3}

; ----------

; STAGE 3 - Calculate vertical scale values
	
	sub r0,r11,r5
	mov r0,r0,asl #16
	
	sub r1,r12,r6
	mov r1,r1,asl #16

; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values

	sub r2,r9,r11
	sub r2,r2,r7
	add r2,r2,r5
	mov r2,r2,asl #20
	
	sub r3,r10,r12
	sub r3,r3,r8
	add r3,r3,r6
	mov r3,r3,asl #20

; ----------

; STAGE 5 - Create preamble 0 and preamble 1 words

	mov r5,#(1<<4)+(1<<3)+6	; UNCODED, REP8, 16BPP
	mov r6,#0
	
	stmia r14!,{r0-r6}

; ----------

	ldmfd sp!,{r4-r11,pc}^	; Exit routine

; ---------------------------------------------------------------------------------------------------

arm_cel_setccbflag_monochrome
	DCD	2_00111111011001100100010100110000

arm_cel_setccbflag_4x4
	DCD	2_00111111101001100100010000110000

arm_cel_setccbflag_frommap		 
	DCD	2_00111111111001100100010000110000

; ----------

arm_pixcshade_type4		; Shades from 0 to 10 for Pmodes 0 and 1
						; NOTE :	This has FIXED Mix-Shading using 2nd source
						
; Shading control values for P mode 0

    DCD 17+(1<<7) + (1<<23) + (3<<8) + (3<<24)							   ; 12.5%     (0)
    DCD 17+(1<<7) + (1<<23) + (2<<10) + (2<<26)+0                          ; 18.75%    (1)
    DCD 17+(1<<7) + (1<<23) + (1<<10) + (1<<26)+(3<<8) + (3<<24)           ; 25%       (2)
    DCD 17+(1<<7) + (1<<23) + (4<<10) + (4<<26)+0                          ; 31.25%    (3)
    DCD 17+(1<<7) + (1<<23) + (2<<10) + (2<<26)+(3<<8) + (3<<24)           ; 37.5%     (4)
    DCD 17+(1<<7) + (1<<23) + (6<<10) + (6<<26)+0                          ; 43.75%    (5)
    DCD 17+(1<<7) + (1<<23) + (3<<10) + (3<<26)+(3<<8) + (3<<24)           ; 50%       (6)
    DCD 17+(1<<7) + (1<<23) + (4<<10) + (4<<26)+(3<<8) + (3<<24)           ; 62.5%     (7)
    DCD 17+(1<<7) + (1<<23) + (5<<10) + (5<<26)+(3<<8) + (3<<24)           ; 75%       (8)
    DCD 17+(1<<7) + (1<<23) + (6<<10) + (6<<26)+(3<<8) + (3<<24)           ; 87.5%     (9)
    DCD 17+(1<<7) + (1<<23) + (7<<10) + (7<<26)+(3<<8) + (3<<24)           ; 100%      (10)
		
; ----------

arm_pixcshade_type3		; Shades from 0 to 10 for Pmodes 0 and 1
						; NOTE :	This has FIXED Add-Shading using 2nd source
						
; Shading control values for P mode 0

    DCD (1<<7) + (1<<23) + (3<<8) + (3<<24)								; 12.5%     (0)
    DCD (1<<7) + (1<<23) + (2<<10) + (2<<26)+0                          ; 18.75%    (1)
    DCD (1<<7) + (1<<23) + (1<<10) + (1<<26)+(3<<8) + (3<<24)           ; 25%       (2)
    DCD (1<<7) + (1<<23) + (4<<10) + (4<<26)+0                          ; 31.25%    (3)
    DCD (1<<7) + (1<<23) + (2<<10) + (2<<26)+(3<<8) + (3<<24)           ; 37.5%     (4)
    DCD (1<<7) + (1<<23) + (6<<10) + (6<<26)+0                          ; 43.75%    (5)
    DCD (1<<7) + (1<<23) + (3<<10) + (3<<26)+(3<<8) + (3<<24)           ; 50%       (6)
    DCD (1<<7) + (1<<23) + (4<<10) + (4<<26)+(3<<8) + (3<<24)           ; 62.5%     (7)
    DCD (1<<7) + (1<<23) + (5<<10) + (5<<26)+(3<<8) + (3<<24)           ; 75%       (8)
    DCD (1<<7) + (1<<23) + (6<<10) + (6<<26)+(3<<8) + (3<<24)           ; 87.5%     (9)
    DCD (1<<7) + (1<<23) + (7<<10) + (7<<26)+(3<<8) + (3<<24)           ; 100%      (10)
	
; ----------

arm_pixcshade_type2		; Shades from 0 to 10 for Pmodes 0 and 1
						; NOTE :	Does not use the 2nd source, this can still be used.
						
; Shading control values for P mode 0

    DCD 0+(3<<8) + (3<<24)                           ; 12.5%     (0)
    DCD (2<<10) + (2<<26)+0                          ; 18.75%    (1)
    DCD (1<<10) + (1<<26)+(3<<8) + (3<<24)           ; 25%       (2)
    DCD (4<<10) + (4<<26)+0                          ; 31.25%    (3)
    DCD (2<<10) + (2<<26)+(3<<8) + (3<<24)           ; 37.5%     (4)
    DCD (6<<10) + (6<<26)+0                          ; 43.75%    (5)
    DCD (3<<10) + (3<<26)+(3<<8) + (3<<24)           ; 50%       (6)
    DCD (4<<10) + (4<<26)+(3<<8) + (3<<24)           ; 62.5%     (7)
    DCD (5<<10) + (5<<26)+(3<<8) + (3<<24)           ; 75%       (8)
    DCD (6<<10) + (6<<26)+(3<<8) + (3<<24)           ; 87.5%     (9)
    DCD (7<<10) + (7<<26)+(3<<8) + (3<<24)           ; 100%      (10)

; ----------

arm_pixcshade_type1		; Shades from 0 to 35 using both P modes (P mode 1 is halfbrite)	- BEWARE OF COLOUR OVERFLOW ON >27 !
						; NOTE :	These shades use the 2nd source, No transparency is allowed,
						;			These are Complete PIXC descriptions

    DCD 2_00000011000000010001000000000001     ; P1 6%    P0 15%       (5)
    DCD 2_00001000000000010000101100000001     ; P1 9%    P0 18%       (6)
    DCD 2_00001000000000010001100000000001     ; P1 9%    P0 21%       (7)
    DCD 2_00000010000000010000000100000001     ; P1 12%   P0 25%       (8)
    DCD 2_00010000000000010000000011010001     ; P1 15%   P0 28%       (9)
    DCD 2_00010000000000010001001100000001     ; P1 15%   P0 31%       (10)
    DCD 2_00001011000000010000100011010001     ; P1 18%   P0 34%       (11)
    DCD 2_00001011000000010000000111100001     ; P1 18%   P0 37%       (12)
    DCD 2_00011000000000010001000011010001     ; P1 21%   P0 40%       (13)
    DCD 2_00011000000000010001101100000001     ; P1 21%   P0 43%       (14)
    DCD 2_00000001000000010001100011010001     ; P1 25%   P0 46%       (15)
    DCD 2_00000001000000010000000100000000     ; P1 25%   P0 50%       (16)
    DCD 2_00000001000000010000000011000001     ; P1 25%   P0 53%       (17)
    DCD 2_00000000110100010000001111000001     ; P1 28%   P0 56%       (18)
    DCD 2_00000000110100010000100011000001     ; P1 28%   P0 59%       (19)
    DCD 2_00010011000000010000010111100001     ; P1 31%   P0 62%       (20)
    DCD 2_00010011000000010001000011000001     ; P1 31%   P0 65%       (21)
    DCD 2_00001000110100010000101111000001     ; P1 34%   P0 68%       (22)
    DCD 2_00001000110100010001100011000001     ; P1 34%   P0 71%       (23)
    DCD 2_00000001111000010000000111000001     ; P1 37%   P0 75%       (24)
    DCD 2_00010000110100010001001111000001     ; P1 40%   P0 81%       (25)
    DCD 2_00011011000000010000100111100001     ; P1 43%   P0 87%       (26)
    DCD 2_00011000110100010001101111000001     ; P1 46%   P0 93%       (27)
    DCD 2_00000001000000000000010100000000     ; P1 50%   P0 100%      (28)
    DCD 2_00000000110000010000000011000000     ; P1 53%   P0 106%      (29)
    DCD 2_00000011110000010000110111100001     ; P1 56%   P0 112%      (30)
    DCD 2_00001000110000010000100011000000     ; P1 59%   P0 118%      (31)
    DCD 2_00000101111000010000100111000001     ; P1 62%   P0 125%      (32)
    DCD 2_00010000110000010001000011000000     ; P1 65%   P0 131%      (33)
    DCD 2_00001011110000010001000111100001     ; P1 68%   P0 137%      (34)
    DCD 2_00011000110000010001100011000000     ; P1 71%   P0 143%      (35)
    DCD 2_00000001110000010000000111000000     ; P1 75%   P0 150%      (36)



; ---------------------------------------------------------------------------------------------------

;*******************************
arm_add4cel4				ROUT
;*******************************

; Receives :	R0 = address of cel adder control block
; 				R1 = address of map
;				R2 = x position on map
;				R3 = y position on map

; Returns :		Nothing
; 				[ Next temporary cel to add is updated in cel adder control block ]
;				[ Next cel creation counter is updated]

; Corrupts :	r0-r3

; ----------
	
	stmfd sp!,{r4-r11,r14}	; Store stack + registers
	str sp,%99
	
; ----------

	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing
	
; ----------

	add r5,r0,#48			; Point to creation data in cel_quad block
	
	ldr r4,[r0,#76]			; Is the cel already in the cache ?
	
	mov r6,r2,asr #2		; x quad pos
	mov r7,r3,asr #2		; y quad pos
	add r4,r4,r6,asl #1
	add r4,r4,r7,asl #7		; r4 is cache datablock address
	ldrb r9,[r4,#0]			; Get the cache-frame counter - 
	
	cmp r9,#0
	beq addcel4x4cache_miss	; The sprite is not cached, build it
	
; ----------
	
	mov r6,#2				; The sprite is cached !
	strb r6,[r4,#0]			; Reset its frame - used counter
	ldrb r9,[r4,#1]			; r9 now holds the source number
	
	ldmia r5,{r6}
	
	add r2,r6,r9, asl#8			; Pointer to where the texture is
	add r2,r2,r9, asl#3
	b addcel4x4registercache	; We have got sprite from cache - just add it

; ----------

addcel4x4cache_miss				; We are now creating a sprite and putting it in the cache list
	
	ldr r9,[r0,#84]				; Get cache free count
	subs r9,r9,#1
	ldmmifd sp!,{r4-r11,pc}^	; No room in the cache - this should not happen, but just in case ..
	str r9,[r0,#84]
	
	mov r6,#2
	strb r6,[r4,#0]			; Mark cache lookup frame as used
	
	ldr r6,[r0,#80]			; Cache free address list
	ldrb r9,[r6,r9]			; Get free source number
	strb r9,[r4,#1]			; Store it in cache ref block
	
	ldmia r5,{r6,r7}
	add r6,r6,r9, asl#8		; Pointer to where the texture is going to be
	add r6,r6,r9, asl#3
	add r6,r6,#8
	
; ----------
	
	add r7,r7,#16			; Add offset to +1 sprites (for db-4 read)
	mov r8,#3				; r8 is y counter
	add r3,r2,r3, asl#8		; r3 is actual position in map
	add r3,r3,#768
	add r3,r1,r3			; r3 is not actual physical location on map (bottom left of sprite segment)
	
; ----------

addcel4x4yloop

	add r5,r6,r8,asl#5
	add r5,r5,r8,asl#4		; point to store address
	
	ldrb r9,[r3,#0]			; r9 is sprite in map xoffset 0
	ldrb r10,[r3,#1]		; r10 is sprite in map xoffset 1
	ldrb r11,[r3,#2]		; r11 is sprite in map xoffset 2
	ldrb r12,[r3,#3]		; r12 is sprite in map xoffset 3

	add r9,r7,r9, asl#4		; point to start address of 4x4 sprite (xoffset 0)
	add r10,r7,r10, asl#4	; point to start address of 4x4 sprite (xoffset 1)
	add r11,r7,r11, asl#4	; point to start address of 4x4 sprite (xoffset 2)
	add r12,r7,r12, asl#4	; point to start address of 4x4 sprite (xoffset 3)
	
	ldr r2,[r9,#-4]!
	ldr r13,[r10,#-4]!
	orr r2,r2,r13, lsr#24		; 1st word is now complete
	mov r13,r13,lsl#8
	ldr r14,[r11,#-4]!
	orr r13,r13, r14, lsr#16	; 2nd word is now complete
	mov r14,r14,lsl#16
	ldr r4,[r12,#-4]!
	orr r14,r14,r4,lsr#8		; 3rd word is now complete
	stmia r5!,{r2,r13,r14}		; STORE line of sprites

	ldr r2,[r9,#-4]!
	ldr r13,[r10,#-4]!
	orr r2,r2,r13, lsr#24		; 1st word is now complete
	mov r13,r13,lsl#8
	ldr r14,[r11,#-4]!
	orr r13,r13, r14, lsr#16	; 2nd word is now complete
	mov r14,r14,lsl#16
	ldr r4,[r12,#-4]!
	orr r14,r14,r4,lsr#8		; 3rd word is now complete
	stmia r5!,{r2,r13,r14}		; STORE line of sprites

	ldr r2,[r9,#-4]!
	ldr r13,[r10,#-4]!
	orr r2,r2,r13, lsr#24		; 1st word is now complete
	mov r13,r13,lsl#8
	ldr r14,[r11,#-4]!
	orr r13,r13, r14, lsr#16	; 2nd word is now complete
	mov r14,r14,lsl#16
	ldr r4,[r12,#-4]!
	orr r14,r14,r4,lsr#8		; 3rd word is now complete
	stmia r5!,{r2,r13,r14}		; STORE line of sprites

	ldr r2,[r9,#-4]!
	ldr r13,[r10,#-4]!
	orr r2,r2,r13, lsr#24		; 1st word is now complete
	mov r13,r13,lsl#8
	ldr r14,[r11,#-4]!
	orr r13,r13, r14, lsr#16	; 2nd word is now complete
	mov r14,r14,lsl#16
	ldr r4,[r12,#-4]!
	orr r14,r14,r4,lsr#8		; 3rd word is now complete
	stmia r5!,{r2,r13,r14}		; STORE line of sprites
	
	sub r3,r3,#256				; dec y position in map
	subs r8,r8,#1				; dec y
	bpl addcel4x4yloop			; still more to add ?
	
; ----------

addcel4x4register
	
	sub r2,r6,#8				; pointer to source data in r2

addcel4x4registercache

; ----------
	
	ldmia r0,{r4-r13}			; Load in cel setup data

	ldr r14,[r13,r4,asl #2]		; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1				; Increment temp pointer
	str r4,[r0]					; Store temp pointer

; ----------
	
	ldr r4,[r0,#40]			; Shading value
	adr r1,arm_pixcshade_type1

	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,#MAX_SHADE
	movgt r4,#MAX_SHADE
	ldr r4,[r1, r4, asl#2]			; Get PIXC Shade
	str r4,[r14,#48]				; Store the PIXC Word

	ldr r1,arm_cel_setccbflag_4x4	; Get Flags
	
	ldr r3,[r0,#100]				; Get address of plut data	
	str r1,[r14],#8					; Store FLAGS
	stmia r14!,{r2,r3}				; Store SOURCEPTR & PLUTPTR

; ----------

; STAGE 1 - X,Y Start position

	mov r0,#160<<16			; Centre x value
	add r0,r0,r5,asl #16;
	
	mov r1,#120<<16			; Centre y value
	add r1,r1,r6,asl #16;

; ----------

; STAGE 2 - Calculate horizontal scale values
	
	sub r2,r7,r5
	mov r2,r2,asl #16
	
	sub r3,r8,r6
	mov r3,r3,asl #16
	
	stmia r14!,{r0-r3}

; ----------

; STAGE 3 - Calculate vertical scale values
	
	sub r0,r11,r5
	mov r0,r0,asl #12
	
	sub r1,r12,r6
	mov r1,r1,asl #12

; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values

	sub r2,r9,r11
	sub r2,r2,r7
	add r2,r2,r5
	mov r2,r2,asl #12
	
	sub r3,r10,r12
	sub r3,r3,r8
	add r3,r3,r6
	mov r3,r3,asl #12

	stmia r14!,{r0-r3}

; ----------

	ldr sp,%99
	ldmfd sp!,{r4-r11,pc}^	; Exit routine

; ----------
99	
	DCD 0					; Stack store				


;*******************************
arm_addcelfrom512map		ROUT
;*******************************
	
; Adds a cel to temporary list clipped from the 512x512 map

; ----------

; Receives :	R0 = address of cel adder control block
;				R1 = x position in map (0-255)
;				R2 = y position in map (0-255)
;				R3 = size of block to render				(0) - 16x16 pixels
;															(1) - 32x32 pixels
;															(2) - 64x64 pixels
;															(3) - 128x128 pixels
;															(4) - 256x256 pixels
;															(5) - 512x512 pixels

; Returns :		Nothing
;				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	?

; ----------

	stmfd sp!,{r4-r11,r14}	; Store stack + registers
	str sp,%99 									

; ----------

	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing
	
; ----------
	
	ldr r4,[r0,#68]				; Find start address in base map
	
	mov r1,r1,asl#1				; X=X*2
	and r6,r1,#15				; Get SKIPX Value in R6
	str r6,arm_codedskipx

	mov r5, r1, asr#4			; Get Word Offset Value
	mov r7, r5, asl#3
	add r7, r7, r5, asl#2
	add r1,r4,r7				; Add on word offset value
	add r1,r1,r2, asl#9			; Add y*768
	add r1,r1,r2, asl#8

	ldmia r0,{r4-r13}			; Load in cel setup data
	
; ----------

	ldr r14,[r13,r4,asl #2]	; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1			; Increment temp pointer
	str r4,[r0]				; Store temp pointer

; ----------
	
	ldr r4,[r0,#100]				; Get address of plut data
	
	ldr r2,arm_cel_setccbflag_frommap		; Get Flags
	str r2,[r14],#8							; Store FLAGS
	stmia r14!,{r1,r4}						; Store SOURCEPTR & PLUTPTR

	ldr r4,[r0,#40]							; Shading value
	adr r1,arm_pixcshade_type1
	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,#MAX_SHADE
	movgt r4,#MAX_SHADE
	ldr r4,[r1, r4, asl#2]					; Get PIXC Shade
	str r4,[r14,#32]						; Store PICX control word
	
; ----------

; STAGE 1 - X,Y Start position

	mov r0,#160<<16			; Centre x value
	add r0,r0,r5,asl #16;
	
	mov r1,#120<<16			; Centre y value
	add r1,r1,r6,asl #16;

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 2 - Calculate horizontal scale values - (Depends on size of source)
	
	rsb r2,r3,#16
	
	sub r0,r7,r5
	mov r0,r0,asl r2
	
	sub r1,r8,r6
	mov r1,r1,asl r2
	
	stmia r14!,{r0-r1}

; ----------

; STAGE 3 - Calculate vertical scale values - (Depends on size of source)
	
	rsb r2,r3,#12
	
	sub r0,r11,r5
	mov r0,r0,asl r2
	
	sub r1,r12,r6
	mov r1,r1,asl r2

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values - (Depends on size of source)

	mov r2,r3,asl#1
	rsb r2,r2,#12
	
	sub r0,r9,r11
	sub r0,r0,r7
	add r0,r0,r5
	mov r0,r0,asl r2
	
	sub r1,r10,r12
	sub r1,r1,r8
	add r1,r1,r6
	mov r1,r1,asl r2

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 5 - Insert PIXC (Already Calculated) and Preamble words 0 and 1

	mov r6,#1					; Calculate x&y size of source subrectangle block (15, 31, 63, 127 etc.)
	add r5,r3,#4
	mov r6,r6,asl r5
	sub r6,r6,#1
	
	mov r5,#1<<30				; Calculate preamble word 0
	orr r5,r5,r6,lsl#6
	orr r5,r5,#4
	ldr r7,arm_codedskipx
	orr r5,r5,r7, lsl# 24		; Insert SKIPX value
	
	add r6,r6,r7				; Add skipx back on
	orr r6,r6,#94<<24			; Calculate preamble word 1
	stmib r14!,{r5-r6}			; Store preamble words
	
; ----------

	ldr sp,%99
	ldmfd sp!,{r4-r11,pc}^	; Exit routine

; ----------
99	
	DCD 0					; Stack store							


;*******************************
arm_addcelfrom128map		ROUT
;*******************************

; Adds a cel to temporary list clipped from the 128x128 map

; ----------

; Receives :	R0 = address of cel adder control block
;				R1 = x position in map (0-255)
;				R2 = y position in map (0-255)
;				R3 = size of block to render

; Returns :		Nothing
;				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	?

; ----------

	stmfd sp!,{r4-r11,r14}	; Store stack + registers
	str sp,%99 									

; ----------

	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing

; ----------

	ldr r4,[r0,#72]			; Find start address in base map
	
	mov r1,r1,asr#1				; X=X/2
	mov r2,r2,asr#1				; Y=Y/2
	
	and r6,r1,#15				; Get SKIPX Value in R6
	str r6,arm_codedskipx

	mov r5, r1, asr#4			; Get Word Offset Value
	mov r7, r5, asl#3
	add r7, r7, r5, asl#2

	add r1,r4,r7				; Add on word offset value
	add r1,r1,r2,asl#6			; add y*96
	add r1,r1,r2,asl#5	
	
	ldmia r0,{r4-r13}		; Load in cel setup data

; ----------

	ldr r14,[r13,r4,asl #2]	; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1			; Increment temp pointer
	str r4,[r0]				; Store temp pointer

; ----------
	
	ldr r4,[r0,#100]				; Get address of plut data
	
	ldr r2,arm_cel_setccbflag_frommap		; Get Flags
	
	str r2,[r14],#8							; Store FLAGS
	stmia r14!,{r1,r4}						; Store SOURCEPTR & PLUTPTR (IF LOADING IN PLUT)
	
	ldr r4,[r0,#40]							; Shading value
	adrl r1,arm_pixcshade_type1
	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,#MAX_SHADE
	movgt r4,#MAX_SHADE
	ldr r4,[r1, r4, asl#2]					; Get PIXC Shade
	str r4,[r14,#32]						; Store PICX control word
	
; ----------

; STAGE 1 - X,Y Start position

	mov r0,#160<<16			; Centre x value
	add r0,r0,r5,asl #16;
	
	mov r1,#120<<16			; Centre y value
	add r1,r1,r6,asl #16;

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 2 - Calculate horizontal scale values - (Depends on size of source)
	
	rsb r2,r3,#18
	
	sub r0,r7,r5
	mov r0,r0,asl r2
	
	sub r1,r8,r6
	mov r1,r1,asl r2
	
	stmia r14!,{r0-r1}

; ----------

; STAGE 3 - Calculate vertical scale values - (Depends on size of source)
	
	rsb r2,r3,#14
	
	sub r0,r11,r5
	mov r0,r0,asl r2
	
	sub r1,r12,r6
	mov r1,r1,asl r2

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values - (Depends on size of source)

	mov r2,r3,asl #1
	rsb r2,r2,#16
	
	sub r0,r9,r11
	sub r0,r0,r7
	add r0,r0,r5
	mov r0,r0,asl r2
	
	sub r1,r10,r12
	sub r1,r1,r8
	add r1,r1,r6
	mov r1,r1,asl r2

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 5 - Insert PIXC (Already Calculated) and Preamble words 0 and 1

	mov r6,#1					; Calculate x&y size of source subrectangle block (15, 31, 63, 127 etc.)
	add r5,r3,#2
	mov r6,r6,asl r5
	sub r6,r6,#1
	
	mov r5,#1<<30				; Calculate preamble word 0
	orr r5,r5,r6,asl#6
	orr r5,r5,#4
	ldr r7,arm_codedskipx
	orr r5,r5,r7, lsl# 24		; Insert SKIPX value
	
	add r6,r6,r7				; Add skipx back on
	orr r6,r6,#22<<24			; Calculate preamble word 1
	stmib r14!,{r5-r6}	; Store preamble words
	
; ----------

	ldr sp,%99
	ldmfd sp!,{r4-r11,pc}^	; Exit routine

; ----------
99	
	DCD 0					; Stack store

;*******************************
arm_addcelfrom32map			ROUT
;*******************************

; Adds a cel to temporary list (the 32x32 map)

; ----------

; Receives :	R0 = address of cel adder control block
; Note     :    THIS ROUTINE RENDERS THE ENTIRE MAP SPRITE

; Returns :		Nothing
;				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	?

; ----------

	stmfd sp!,{r4-r11,r14}	; Store stack + registers
	str sp,%99 									

; ----------

	ldr r4,[r0,#0]			; How many cels currently in ?
	cmp r4,#cel_maxhold		; Will this one exceed list space ?
	blge arm_interceptplot	; Yes, plot and reset list before continuing

; ----------

	ldr r1,[r0,#96]			; Find start address in base map
	ldmia r0,{r4-r13}		; Load in cel setup data

; ----------

	ldr r14,[r13,r4,asl #2]	; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1			; Increment temp pointer
	str r4,[r0]				; Store temp pointer

; ----------
	
	ldr r4,[r0,#100]				; Get address of plut data
	
	ldr r2,arm_cel_setccbflag_frommap		; Get Flags
	
	str r2,[r14],#8							; Store FLAGS
	stmia r14!,{r1,r4}						; Store SOURCEPTR & PLUTPTR
	
	ldr r4,[r0,#40]							; Shading value
	adrl r1,arm_pixcshade_type1
	cmp r4,#MIN_SHADE
	movlt r4,#MIN_SHADE
	cmp r4,#MAX_SHADE
	movgt r4,#MAX_SHADE
	ldr r4,[r1, r4, asl#2]					; Get PIXC Shade

; ----------

; STAGE 1 - X,Y Start position

	mov r0,#160<<16			; Centre x value
	add r0,r0,r5,asl #16;
	
	mov r1,#120<<16			; Centre y value
	add r1,r1,r6,asl #16;

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 2 - Calculate horizontal scale values - (Depends on size of source)
	
	sub r0,r7,r5
	mov r0,r0,asl #15
	
	sub r1,r8,r6
	mov r1,r1,asl #15
	
	stmia r14!,{r0-r1}

; ----------

; STAGE 3 - Calculate vertical scale values - (Depends on size of source)
	
	sub r0,r11,r5
	mov r0,r0,asl #11
	
	sub r1,r12,r6
	mov r1,r1,asl #11

	stmia r14!,{r0-r1}
	
; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values - (Depends on size of source)
	
	sub r0,r9,r11
	sub r0,r0,r7
	add r0,r0,r5
	mov r0,r0,asl #10
	
	sub r1,r10,r12
	sub r1,r1,r8
	add r1,r1,r6
	mov r1,r1,asl #10

; ----------

; STAGE 5 - Insert PIXC (Already Calculated) and Preamble words 0 and 1

	mov r5,#1<<30				; Calculate preamble word 0
	orr r5,r5,#31 << 6
	orr r5,r5,#4
	
	mov r6,#31
	orr r6,r6,#4<<24			; Calculate preamble word 1
	stmia r14!,{r0-r1, r4-r6}	; Store preamble words
	
; ----------

	ldr sp,%99
	ldmfd sp!,{r4-r11,pc}^	; Exit routine

; ----------
99	
	DCD 0					; Stack store

; ---------------------------------------------------------------------------------------------------
arm_codedskipx
	DCD 0
; ---------------------------------------------------------------------------------------------------

;*******************************
arm_celinitialisecreation	ROUT
;*******************************

; Receives :	R0 = address of creation sprite block
; 				R1 = Number of sprite blocks to initialise

; Returns :		Nothing

; Corrupts :	r0-r3
	
; ----------
	
	sub r1,r1,#1
	
	mov r2,#15 << 6			; Preamble word 0 (16x16 6bpp coded cel)
	orr r2,r2,#4
	
	mov r3,#1 << 24			; Preamble word 1 (16x16 6bpp coded cel)
	orr r3,r3,#1 << 12
	orr r3,r3,#15

; ----------

arm_initialisecreationloop

	subs r1,r1,#1
	movmis pc,r14

	stmia r0,{r2,r3}
	add r0,r0,#264
	b arm_initialisecreationloop


;*******************************
arm_initialisecache 		ROUT
;*******************************

; Receives :	R0 = Address of cel adder control block
	
; Returns :		Nothing

; Corrupts :	r0-r3
	
; ----------

	ldr r3,[r0,#76]			; Get address of cache_lookup block
	mov r2,#0
	mov r1,#0

; ----------				; Clear the cache lookup block

arm_initialisecacheblockloop

	str r2,[r3,r1]
	add r1,r1,#4
	cmp r1,#8192
	blt arm_initialisecacheblockloop

	mov r1,#255
	str r1,[r0,#84]			; Store number of free cel blocks
	ldr r3,[r0,#80]			; Get address of cache_free list
	
; ----------				; Clear the cache lookup block

arm_initialisecachefreeloop

	subs r1,r1,#1
	movmis pc,r14			; Exit
	
	strb r1,[r3,r1]			; Store free slot in free list
	b arm_initialisecachefreeloop
	

;*******************************
arm_updatecache 			ROUT
;*******************************

; Receives :	R0 = Address of cel adder control block
	
; Returns :		Nothing

; Corrupts :	r0-r3
	
; ----------

	stmfd sp!,{r4-r6}
	
	add r1,r0,#76
	ldmia r1,{r1,r2,r3}		; Get addresses
							; r1 = cache_lookup block address
							; r2 = cache_free list address
							; r3 = cache free count
							
	add r4,r1,#(64*64)*2	; point at end

arm_updatecacheloop

	cmp r4,r1
	blt arm_updatecacheexit;
	
; ----------

	ldr r5,[r4,#-4]!		; Get 2 cache entries
	cmp r5,#0
	beq arm_updatecacheloop	; They are both empty, carry on looking	
	
; ----------

	; Update timer entry for cache entry 1 (if valid)
	
	mov r6,r5, lsr#8
	and r6,r6,#255				; Get timer entry from word

	subs r6,r6,#1
	strgeb r6,[r4,#2]			; If still valid, store new counter value

	; Timer has expired (if equal to zero) for cache entry 1, delete it

	streqb r6,[r4,#3]			; Clear source entry
	streqb r5,[r2,r3]			; Put entry in free list
	addeq r3,r3,#1

; ----------

	; Update timer entry for cache entry 1 (if valid)
	
	mov r5,r5, lsr #16
	
	mov r6,r5, lsr#8
	and r6,r6,#255				; Get timer entry from word

	subs r6,r6,#1
	strgeb r6,[r4,#0]			; If still valid, store new counter value

	bne arm_updatecacheloop		; carry on search
	
	; Timer has expired (if equal to zero) for cache entry 2, delete it

	strb r6,[r4,#1]				; Clear source entry
	strb r5,[r2,r3]				; Put entry in free list
	add r3,r3,#1

	b arm_updatecacheloop		; carry on search

; ----------

arm_updatecacheexit

	str r3,[r0,#84]		; End of scan, put new free counter back
	ldmfd sp!,{r4-r6}
	movs pc,r14
	
;*******************************
arm_generatemaps			ROUT
;*******************************

; Receives :	R0 = address of cel data block
;				R1 = address of actual spritemap

; Returns :		Nothing
; 				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	r0-r3

; ----------

	stmfd sp!,{r4-r12,r14}
	
	; 3 stages :		- STAGE 1 : GENERATE 6BPPCODED 512X512 MAP
	;					- STAGE 2 : GENERATE 6BPPCODED 128X128 MAP
	;					- STAGE 3 : GENERATE 6BPPCODED 32X32 MAP
	
	
	ldr r12,[r0, #52]	; Load in address of 4x4 compact sprites

; ----------

; STAGE 1 - 512 x 512 Map
	
	ldr r11,[r0, #68]	; Load in address of 512x512 map
	mvn r2,#0

arm_generatemap_512yloop
	
	add r2,r2,#1
	cmp r2,#512
	bge arm_generatemap_128
	
	mvn r3,#0
	mov r9,#32			; Bit counter
	mov r10,#0			; Word value
	
arm_generatemap_512xloop
	
	add r3,r3,#1
	cmp r3,#256
	strge r10,[r11],#4
	bge arm_generatemap_512yloop
	
; ----------
	
	mov r5, r2, asr#1		; Get y position 0-255
	add r4, r3, r5, asl#8	; Get position in map

	; Get pixels 0 and 1 (from map pos)
	
	ldrb r5, [r1,r4]		; Sprite from map
	add r5, r12, r5, asl#4	; Point to 4x4 sprite address

	tst r2, #1				; Are we on an odd line (ie. get halfway through y 4x4 sprite ?) 

	ldreq r5, [r5,#8]		; No - Get line from sprite
	ldrne r5, [r5,#0]		; Yes - Get line from halfway through sprite
	
	mov r6, r5, lsr#26		; Get coded6 pixel1 in r6
	mov r7, r5, lsr#14
	and r7, r7, #63			; Get coded6 pixel2 in r7
	
; ----------

	; Write 6 bits of R6 pixel
	
	subs r9,r9,#6					; Will it all fit in this word ?
	orrpl r10,r10,r6, lsl r9 		; Yes - put it straight in and go to next pixel
	bpl arm_generatemap_512skip1	; Yes - do next
	
	rsb r8,r9,#0					; No - How many bits to shift down ?
	orr r10,r10,r6, lsr r8
	str r10,[r11],#4				; Store the word if it needs storing and set up next bit
	and r9,r9,#31
	mov r10, r6, lsl r9

arm_generatemap_512skip1

	; Write 6 bits of R7 pixel
	
	subs r9,r9,#6					; Will it all fit in this word ?
	orrpl r10,r10,r7, lsl r9 		; Yes - put it straight in and go to next pixel
	bpl arm_generatemap_512xloop	; Yes - do next
	
	rsb r8,r9,#0					; No - How many bits to shift down ?
	orr r10,r10,r7, lsr r8
	str r10,[r11],#4				; Store the word if it needs storing and set up next bit
	and r9,r9,#31
	mov r10, r7, lsl r9
	b arm_generatemap_512xloop

; ----------

; STAGE 2 - 128 x 128 Map

arm_generatemap_128

	ldr r11,[r0, #72]				; Load in address of 128x128 map
	mvn r2,#0

arm_generatemap_128yloop
	
	add r2,r2,#1
	cmp r2,#128
	bge arm_generatemap_32
	
	mvn r3,#0
	mov r9,#32						; Bit counter
	mov r10,#0						; Word value
	
arm_generatemap_128xloop
	
	add r3,r3,#1
	cmp r3,#128
	strge r10,[r11],#4
	bge arm_generatemap_128yloop
	
; ----------
	
	; Get pixel (from map pos)
		
	add r4, r1, r3, asl#1			; X*2 position on map
	ldrb r5, [r4, r2, asl#9]		; Sprite from map
	ldr r6, [r12, r5, asl#4]		; Get line from sprite
	mov r6,r6, lsr#26				; Get actual pixel
	
; ----------

	; Write 6 bits of R6 pixel
	
	subs r9,r9,#6					; Will it all fit in this word ?
	orrpl r10,r10,r6, lsl r9 		; Yes - put it straight in and go to next pixel
	bpl arm_generatemap_128xloop	; Yes - do next
	
	rsb r8,r9,#0					; No - How many bits to shift down ?
	orr r10,r10,r6, lsr r8
	str r10,[r11],#4				; Store the word if it needs storing and set up next bit
	and r9,r9,#31
	mov r10, r6, lsl r9
	b arm_generatemap_128xloop

; ----------

; STAGE 3 - 32 x 32 Map

arm_generatemap_32

	ldr r11,[r0, #96]	; Load in address of 32x32 map
	mvn r2,#0

arm_generatemap_32yloop
	
	add r2,r2,#1
	cmp r2,#32
	ldmgefd sp!,{r4-r12, pc}^
	
	mvn r3,#0
	mov r9,#32			; Bit counter
	mov r10,#0			; Word value
	
arm_generatemap_32xloop
	
	add r3,r3,#1
	cmp r3,#32
	strge r10,[r11],#4
	bge arm_generatemap_32yloop
	
; ----------
	
	; Get pixel (from map pos)
	
	add r4, r1, r3, asl#3		; X*8 position on map
	ldrb r5, [r4, r2, asl#11]	; Sprite from map
	ldr r6, [r12, r5, asl#4]	; Get line from sprite
	mov r6,r6, lsr#26			; Get actual pixel
		
; ----------

	; Write 6 bits of R6 pixel
	
	subs r9,r9,#6					; Will it all fit in this word ?
	orrpl r10,r10,r6, lsl r9 		; Yes - put it straight in and go to next pixel
	bpl arm_generatemap_32xloop		; Yes - do next
	
	rsb r8,r9,#0					; No - How many bits to shift down ?
	orr r10,r10,r6, lsr r8
	str r10,[r11],#4				; Store the word if it needs storing and set up next bit
	and r9,r9,#31
	mov r10, r6, lsl r9
	b arm_generatemap_32xloop


;*******************************
arm_interceptplot			ROUT
;*******************************

; Calls a C routine to plot the temporary list if it gets full

; ----------

; Receives :	R0 = address of cel adder control block

; Returns :		Nothing		[ Next temporary cel to add is cleared by C routine]

; Corrupts :	Nothing

	stmfd sp!,{r0-r3, r14}	; Store parameters
	bl screen_intercept		; Call plot routine
	ldmfd sp!,{r0-r3, pc}^	; Return


;*******************************
arm_setcel_hw				ROUT
;*******************************

; Receives :	R0 = address of cel data block
;				R1 = New width (< 0 ignored)
; 				R2 = New height  (< 0 ignored)

; Returns :		Nothing
; 				[Last cel added has it's width & height changed]
;
; NOTE :        CAN ONLY SET WIDTH ON UNPACKED CELS

; Corrupts :	r0-r3

; ----------
	
	ldr r3,[r0,#0]			; Get last cel added
	subs r3,r3,#1
	movmis pc,r14			; If none, return
	
; ----------

	stmfd sp!, {r14}		; Store Return Address
	
	ldr r0,[r0,#36]			; Address of temp plotting ccb list
	ldr r0,[r0,r3,asl #2]	; Get address of THIS temp (Dest) CCB
	ldr r0, [r0, #8]		; Get source data of cel
	
; ----------

	subs r1, r1, #1			; Are we setting new width (PL if yes)
	ldrpl r3, [r0, #4]		; GET PREAMBLE 1
	ldrpl r14, arm_sethw_mask1
	andpl r3, r3, r14
	orrpl r3, r3, r1		
	strpl r3, [r0, #4]		; Put in new width
	
; ----------

	subs r2, r2, #1			; Are we setting new height (PL if yes)
	ldrpl r3, [r0, #0]		; GET PREAMBLE 0
	ldrpl r14, arm_sethw_mask2	
	andpl r3, r3, r14
	orrpl r3, r3, r2, lsl #6
	strpl r3, [r0, #0]		; Put in new height
	
	ldmfd sp!, {pc}^		; Return

; ----------

arm_sethw_mask2
	DCD 2_11111111111111110000000000111111

arm_sethw_mask1
	DCD 2_11111111111111111111100000000000
	
; ----------

	END
