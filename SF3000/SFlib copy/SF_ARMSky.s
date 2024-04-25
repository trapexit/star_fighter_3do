			EXPORT	arm_rendersky, arm_plotsky, skyfile
			IMPORT	arm_interceptplot
			IMPORT	divide
			IMPORT	arm_interceptplot
			AREA	|c$$code|,CODE,READONLY

; -----------------------------------------------------------------------------------------------------------------------------------
cel_maxhold	EQU	1024		; Max number of temporary cels allowed - Also in Allocation.h - Also in ARMCel.s

skyfile						; SKY Source File Area
	% 1024
	
armsky_renderdata			; Space for rendered sky
	% 400*8
	
; -----------------------------------------------------------------------------------------------------------------------------------
	
;*******************************
arm_rendersky				ROUT
;*******************************

; A new for '95 way of plotting the shaded sky. The left pixel strip of a sky is plotted directly
; into an unencoded 16bit cel, which is then stretched across the entire screen.

; NOTE : This sky-strip cel can now be plotted as with any quad-point polygon coz the sky can now rotate

; The routine draws a sky from the horizon upwards, given a screen offset to start drawing from.
; lines are clipped if the horizon is off the top of the screen (immediately thrown out), or if
; the horizon is off the bottom of the screen (in which case it calculates values until the bottom
; of the screen is reached and then starts normally).

; The skyfile contains a base height value (colours you would see if you were standing on the ground)
; and also a height value at which starts start to appear, (not used in this routine).

; NOTE :

; The cels used is 16bpp unencoded & unpacked, 2 pixels wide, upto 400 pixels high
; The format is 2 words per row : (Sprite is rendered as 1 pixel wide one to stop the cel engines stitching)

; [Word 0] 0rrrrrgggggbbbbb 0rrrrrgggggbbbbb	- pixels 0 & 1
; [Word 1] 0000000000000000 0000000000000000	- unused, has to be there because of cel engine pipelining

; ----------

; Receives :	R0 points to sky height scaling number
; 				R1 contains offset from start of screen	to start sky horizon,
;											[0 .. > 399, 0 is top of screen]

; Returns :		Nothing

; Corrupts :	r0-r3

; ----------

; Sky file format :

; SKYFILE + 0 -> Min height of sky
; SKYFILE + 4 -> Start height of star plot
; SKYFILE + 8 -> Start of sky colour data (This is new format, scaled 15 bit colours, no dithering)

; ----------

; Routine start

	cmp r1,#0
	strle r1,armsky_renderheight
	movles pc,r14			; If sky is off the top of the screen, return straight away
	
; We are at least drawing some sky, so ...
	
	stmfd sp!,{r4-r6,r14}	; Store stack + registers									

; ----------

; Initialise all sky drawing variables
	
	adrl r2,skyfile			; Get source address of skyfile
	ldr r4,[r2],#8			; Get minimum height of sky (lowest scale from ground view)
	
	mov r6,r0,asr#7			; Scale passed height value
	add r6,r6,r6,asr#1
	add r6,r6,#48
	cmp r6,#255
	movgt r6,#255
	
	add r0,r0,r4
	mov r4,#0
	
	cmp r1,#400				; Are we starting too low (ie. off end of CEL) ?
	bge armsky_clipsky		; Clip for bottom of screen
	
; ----------

armsky_startskydraw				; Start to draw sky
	
	str r1,armsky_renderheight
	
	adrl r3,armsky_renderdata	; Get address of sky datastrip
	add r1,r3,r1,asl #3			; Point to correct entry in CEL
	
armsky_dolineagain				; Main sky drawing loop

	mov r5,r4, asr#10			; Get correct sky colour band
	cmp r5,r6					; Are we further up that we are allowed ?
	
	ldrgt r5,[r2, r6, asl #2]	; Yes, clip to max
	ldrle r5,[r2, r5, asl #2]	; No, get colour

	str r5,[r1],#-8				; Store colour change word in CEL and move to previous
	cmp r1,r3					; Have we drawn to the top of the screen ?
	ldmlefd sp!,{r4-r6,pc}^		; Yes, leave
	
	add r4,r4,r0,asr #1			; Move up colour bands
	cmp r0,#128
	subgt r0,r0,r0,asr #5		; Scale down colour band adder
	
	b armsky_dolineagain		; Next line
	
; ----------

armsky_clipsky				; We are starting horizon off bottom of screen, loop to clip
							; sky directly of bottom
	add r4,r4,r0,asr #1
	subs r0,r0,r0,asr #5	; Move up colour bands
	sub r1,r1,#1			; Shift CEL pointer backwards

	cmp r1,#400				; Are we still of the end of the screen ?
	bge armsky_clipsky		; Yes, carry on clipping

	b armsky_startskydraw	; No, we are ready to start drawing to CEL

; -----------------------------------------------------------------------------------------------------------------------------------

arm_plotsky_pixc
	DCD	&1f001f00			; Pixc control word for display of skycel

arm_plotsky_flags			; Flags control word for display of skycel
	DCD 2_00111111011011100100000100100000
	
armsky_renderheight			; Height of last rendered sky
	DCD 0

armsky_pre1					; PREAMBLE WORD 1 (PREAMBLE WORD 0 IS CREATED)
	DCD	(1<<12)				; Blue lsb, 1 Pixel per row

armsky_r14					; Spare slot to store link register in
	DCD 0

; -----------------------------------------------------------------------------------------------------------------------------------

;*******************************
arm_plotsky				ROUT
;*******************************

; Receives :	R0 = address of cel adder control block

; Returns :		Nothing
; 				[ Next temporary cel to add is updated in cel adder control block ]

; Corrupts :	r0-r3

; ----------
	
	ldr r2,armsky_renderheight	; Is height of sky < 0, if so (off screen), then return
	cmp r2,#0
	movles pc,r14

; ----------

	stmfd sp!,{r4-r12, r14}		; Store stack + registers

	ldr r4,[r0,#0]				; How many cels currently in ?
	cmp r4,#cel_maxhold			; Will this one exceed list space ?
	blge arm_interceptplot		; Yes, plot and reset list before continuing
	
	ldmia r0,{r4-r12,r14}		; Load in cel setup data
	
; ----------

	ldr r14,[r14,r4,asl #2]		; Get address of THIS temp (Dest) CCB 	(r14) - ccb we are writing to
	add r4,r4,#1				; Increment temp pointer
	str r4,[r0]					; Store temp pointer
	
	ldr r4, arm_plotsky_flags
	str r4, [r14], #8			; Store FLAGS word
	adrl r1, armsky_renderdata
	str r1, [r14], #8			; Store SourcePtr

; ----------

; STAGE 1 - X,Y Start position

	mov r0,#160<<16				; Centre x value
	add r0,r0,r5,asl #16
	
	mov r1,#120<<16				; Centre y value
	add r1,r1,r6,asl #16

; ----------

; STAGE 2 - Calculate horizontal scale values
	
	sub r2,r7,r5
	mov r2,r2,asl #20			; #19
	
	sub r3,r8,r6
	mov r3,r3,asl #20			; #19
	
	stmia r14!,{r0-r3}

; ----------

; STAGE 3 - Calculate vertical scale values

	str r14,armsky_r14
	stmfd sp!,{r5-r8}
	sub r0,r11,r5
	mov r0,r0,asl #16
	ldr r1,armsky_renderheight
	bl divide
	
	ldmfd sp,{r5-r8}
	stmfd sp!,{r3}
	
	sub r1,r12,r6
	mov r0,r1,asl #16
	ldr r1,armsky_renderheight
	bl	divide

	ldmfd sp!,{r0}
	ldr r14,armsky_r14
	stmia r14!,{r0,r3}
	str r14,armsky_r14
		
; ----------

; STAGE 4 - Calculate horizontal / vertical scale increment values

	ldmfd sp,{r5-r8}
	
	sub r2,r9,r11
	sub r2,r2,r7
	add r2,r2,r5
	mov r0,r2,asl#20
	
	ldr r1,armsky_renderheight
	; mov r1,r1, asl#1				; 1 pixel now
	bl divide
	
	ldmfd sp!,{r5-r8}
	stmfd sp!,{r3}
	
	sub r3,r10,r12
	sub r3,r3,r8
	add r3,r3,r6
	mov r0,r3,asl #20
	
	ldr r1,armsky_renderheight
	; mov r1,r1, asl#1				; 1 pixel now
	bl divide
	
	ldmfd sp!,{r0}
	ldr r14,armsky_r14
	stmia r14!,{r0,r3}
	
; STAGE 5 - Fill in PIXC, PRE0 AND PRE1
	
	ldr r0,arm_plotsky_pixc		; PIXC control word
	ldr r2,armsky_pre1			; Get preamble word 1

	ldr r1,armsky_renderheight	; CREATE preable word 0
	sub r1,r1, #1
	mov r1,r1, lsl#6			; Get height -1
	orr r1,r1, #(1<<4)+6		; uncoded cel, 16bpp sprite
	
	stmia r14,{r0-r2}			; Store control words
	
; ----------

	ldmfd sp!,{r4-r12,pc}^	; Exit routine
				
; ----------
	
	END
