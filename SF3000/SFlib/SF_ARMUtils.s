			EXPORT	arm_randominit, arm_random, arm_randomvalue, arm_drawmaptargets
			IMPORT	arm_addgamecel
			IMPORT	height_map
			
			AREA	|c$$code|,CODE,READONLY


MAP_OBJECTSPRITE	EQU 145	; Sprite for ground object

; ---------------------------------------------------------------------------------------------------

; This library contains general utility routines that need to be fast

; A) arm_random****	- For resetting and reading random values

; ---------------------------------------------------------------------------------------------------
; arm_randominit

; Resets the random seed to it's default state - It accepts no parameters

; ----------
; arm_random

; Returns a random 32 bit value in R0 based on the contents of the 2 internal seed values. 

; ----------
; arm_randomvalue

; Returns a random number in r0 between 0 & the number passed in r0 (inclusive)

; ---------------------------------------------------------------------------------------------------

arm_randomseed1			; Random seeds
	DCD	1023

arm_randomseed2
	DCD 1023
	
; ---------------------------------------------------------------------------------------------------

;*******************************
arm_randominit				ROUT
;*******************************

; This routine resets the random seed values to default (1023, 1023)

; ----------

; Receives :	Nothing

; Returns :		Nothing

; Corrupts :	r0

; ----------

	mov r0,#1024
	sub r0,r0,#1

	str r0,arm_randomseed1
	str r0,arm_randomseed2

	movs pc,r14

;*******************************
arm_random					ROUT
;*******************************

; This routine reads the random values, re-altering seed values

; ----------

; Receives :	Nothing

; Returns :		r0 & r1 - 32 bit Random values

; Corrupts :	r0-r2

; ----------

	adr r2,arm_randomseed1
	ldmia r2,{r0,r1}
	eor r0,r0,r1, ror r1
	add r1,r1,r0
	stmia r2,{r0,r1}
	eor r0,r0,r1
	movs pc,r14

;*******************************
arm_randomvalue				ROUT
;*******************************

; This routine returns a random number between 0 and <= r0
; NOTE : Number will never be > 65536

; ----------

; Receives :	Random number limit

; Returns :		r0 - 32 bit Random value 1

; Corrupts :	r0-rx

; ----------

	stmfd sp!,{r14}
	mov r3,r0					; Get copy of max number
	bl arm_random				; Get random 32 bit number
	mov r2,r0					; Put it in r2
	
	mov r1,#1<<16
	mov r0,#0					; Our random number count value
	
; ----------

arm_randomvalueloop

	mov r1,r1, lsr#1			; Shift down adder
	cmp r1,#0					; Finished ?
	ldmeqfd sp!,{pc}^			; Return
	
	tst r2, r1
	beq arm_randomvalueloop		; Bit is not set, look at next
	
	add r0,r0,r1				; Add on this bit value
	cmp r0,r3					; Over limit ?
	subgt r0,r0,r1				; Yes, take it off again
	b arm_randomvalueloop		; Get next
	
; ----------

arm_crossshade
	DCD 0
arm_heightmapstart
	DCD height_map
	
;*******************************
arm_drawmaptargets			ROUT
;*******************************

; This routine draws all objects on the map (crosses)

; ----------

; Receives :	r0 = address of polygon map
;				r1 = map zoom
;				r2 = cel quad address
;				r3 = map offset [b15 .. 8 is Y offset (0-255), b7 .. 0 is X offset (0-255)]

; Returns :		Nothing

; Corrupts :	r0-r3

; ----------

	stmfd sp!,{r4-r12, r14}
	
	and r11,r3,#255				; R11 is X offset
	mov r12,r3, lsr#8			; R12 is Y offset
	mov r10,r2					; R10 is Copy of cel_quad sprite adder block
	mov r5, r1					; R5 is copy of map zoom for scaling heights
	add r9,r1, #20				; R9 is map zoom scalar
	mov r8,r0					; R8 is base address of polygon map
	
	mov r7,#16384				; Y loop
	
	ldr r14, arm_crossshade		; Inc shade counter value
	add r14, r14, #1
	and r14, r14, #255
	str r14, arm_crossshade
	
	tst r14, #4					; Set shade for objects
	moveq r14, #11
	movne r14, #8
	str r14, [r10, #40]			; Set shade in cel quad
	
	ldr r6, arm_heightmapstart	; Get start of height map
		
; ----------

arm_drawmaptargets_loop

	subs r7, r7, #1
	ldmmifd sp!,{r4-r12, pc}^	; Finished ?
	
	ldrb r0, [r8, r7]
	
	cmp r0,#0
	beq arm_drawmaptargets_loop			; No object here, ?
	
	cmp r0,#60
	bge arm_drawmaptargets_loop			; Not a valid object ?

; ----------

	and r0, r7, #127					; Get x pos
	mov r1, r7, lsr#7					; Get y pos
	
	mov r4, r1, asl#9
	add r4, r4, r0, asl#1				; Get map offset (255 , 255)
	
	mov r0, r0, asl#25					; Scale up x pos of object to ship co-ords
	mov r1, r1, asl#25					; Scale up y pos of object to ship co-ords

; ----------

	sub r0, r0, r11, asl#24				; Take off X map offset position
	sub r1, r1, r12, asl#24				; Take off Y map offset position

	mov r0, r0, asr r9					; Scale back down to zoom scale
	mov r1, r1, asr r9
	
	cmp r0,#-128
	cmpgt r1,#-128
	ble arm_drawmaptargets_loop			; Clip for screen co-ordinates
	
	cmp r0,#128
	cmplt r1,#128
	bge arm_drawmaptargets_loop			; Clip for screen co-ordinates
	
	rsb r1, r1, #0
	
	sub r0, r0, r1, asr#1				; Make oblique
	
	mov r0, r0, asr#1					; Convert from 256x256 to 192x192
	add r0, r0, r0, asr#1
	
	mov r1, r1, asr#1
	add r1, r1, r1, asr#1
	 
; ----------
		
	ldrb r4, [r6, r4]					; Get height at map position
	sub r1, r1, r4, asr r5				; Move up according to height & zoom
	
	;add r0, r0, #5-5					; Move for XMAPOFFSET SCREEN BASE POSITION [THIS IS NOW HARD-CODED TO SAVE REGISTERS]
	sub r1, r1, #10-3					; Move for YMAPOFFSET [IF CHANGED IN MAP.C (MAPBASEX, MAPBASEY - CHANGE HERE]
	
	str r0, [r10, #4]					; Store X pos
	str r1, [r10, #8]					; Store Y pos
	
arm_storefadedtarget

	mov r0, r10							; Draw object cross
	mov r1, #MAP_OBJECTSPRITE
	mov r2, #1024
	mov r3, #1024
	bl arm_addgamecel
			
	b arm_drawmaptargets_loop
	
	END
