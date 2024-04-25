	AREA |c$$code|,CODE,READONLY

	EXPORT		rotate_node_from_c
	EXPORT		rotate_land_node_from_c
	EXPORT		find_rotation
	EXPORT		rotate_coords_x
	EXPORT		rotate_coords_y
	EXPORT		rotate_coords_z
	EXPORT		divide
	EXPORT		target_finder
	EXPORT		mc_smoke_mover
	EXPORT		scan_poly_map
	EXPORT		scan_poly_map_2
	EXPORT		rotate_node_x_y_z_camera
	EXPORT		setup_camera_rotations
	EXPORT		find_2d_distance
	EXPORT		rotate_2d_node
	EXPORT		oppo_rotate_node_from_c

	IMPORT		cosine_table
	IMPORT		tangent_table
	IMPORT		poly_map
	IMPORT		camera_x_rotation
	IMPORT		camera_y_rotation
	IMPORT		camera_z_rotation
	

cosine_data
	DCD cosine_table

rotate_2d_node_data_pointer
	DCD 0

rotate_2d_node
	;r0 pointer to node data
	;0,4,8 x,y,z (z not used) coords
	;12,16,20 x,y,z (y,z not used) coords

	stmfd sp!,{r4-r12,r14}
	
	str r0,rotate_2d_node_data_pointer
	
	ldr r3,[r0,#12]		;get angle
	ldmia r0,{r0-r1}
	bl rotate_coords_x
	
	ldr r2,rotate_2d_node_data_pointer
	stmia r2,{r0-r1}
	
	ldmfd sp!,{r4-r12,pc}^

rotate_coords_x

; r0-r2 coords to be rotated
; r3 rotation value - 256 = 90 degrees
; uses r3 - r7
; updates r0 , r1 and keeps r2 intact

; get the cosine and sine values for this rotation
	ldr r6,cosine_data
	add r7,r6,#1024*3 ; sine table
	ldr r6,[r6,r3, asl #2]
	ldr r7,[r7,r3, asl #2]
	
; rotated the x and y coords 
; - note arm multiplies are quicker if the last operand is positive
; hence
	; x coord mul by cosine x
	movs r3,r0
	rsbmi r3,r3,#0
	mul r3,r6,r3
	rsbmi r3,r3,#0
	; y coord mul by sine x
	movs r4,r1
	rsbmi r4,r4,#0
	mul r4,r7,r4
	rsbmi r4,r4,#0
	; scale down values and add them for new x value in r5
	mov r3,r3, asr #12
	sub r5,r3,r4, asr #12
	
	; x coord mul by sine x
	movs r3,r0
	rsbmi r3,r3,#0
	mul r3,r7,r3
	rsbmi r3,r3,#0
	; y coord mul by cosine x
	movs r4,r1
	rsbmi r4,r4,#0
	mul r4,r6,r4
	rsbmi r4,r4,#0
	; scale down values and add them for new y value in r1
	mov r3,r3, asr #12
	add r1,r3,r4, asr #12

	; put new x value back into r0
	mov r0,r5
	
	; exit with x,y ( r0 , r1 ) rotated by r3
	mov pc,r14

rotate_coords_y

; r0-r2 coords to be rotated
; r3 rotation value - 256 = 90 degrees
; uses r3 - r7
; updates r1 , r2 and keeps r0 intact

; get the cosine and sine values for this rotation
	ldr r6,cosine_data
	add r7,r6,#1024*3 ; sine table
	ldr r6,[r6,r3, asl #2]
	ldr r7,[r7,r3, asl #2]
	
; rotated the y and z coords 
; - note arm multiplies are quicker if the last operand is positive
; hence
	; y coord mul by cosine y
	movs r3,r1
	rsbmi r3,r3,#0
	mul r3,r6,r3
	rsbmi r3,r3,#0
	; z coord mul by sine y
	movs r4,r2
	rsbmi r4,r4,#0
	mul r4,r7,r4
	rsbmi r4,r4,#0
	; scale down values and add them for new x value in r5
	mov r3,r3, asr #12
	sub r5,r3,r4, asr #12
	
	; y coord mul by sine y
	movs r3,r1
	rsbmi r3,r3,#0
	mul r3,r7,r3
	rsbmi r3,r3,#0
	; z coord mul by cosine y
	movs r4,r2
	rsbmi r4,r4,#0
	mul r4,r6,r4
	rsbmi r4,r4,#0
	; scale down values and add them for new y value in r1
	mov r3,r3, asr #12
	add r2,r3,r4, asr #12

	; put new y value back into r1
	mov r1,r5
	
	; exit with y,z ( r1 , r2 ) rotated by r3
	mov pc,r14


rotate_coords_z

; r0-r2 coords to be rotated
; r3 rotation value - 256 = 90 degrees
; uses r3 - r7
; updates r0 , r2 and keeps r1 intact

; get the cosine and sine values for this rotation
	ldr r6,cosine_data
	add r7,r6,#1024*3 ; sine table
	ldr r6,[r6,r3, asl #2]
	ldr r7,[r7,r3, asl #2]
	
; rotated the x and z coords 
; - note arm multiplies are quicker if the last operand is positive
; hence
	; x coord mul by cosine z
	movs r3,r0
	rsbmi r3,r3,#0
	mul r3,r6,r3
	rsbmi r3,r3,#0
	; z coord mul by sine z
	movs r4,r2
	rsbmi r4,r4,#0
	mul r4,r7,r4
	rsbmi r4,r4,#0
	; scale down values and add them for new x value in r5
	mov r3,r3, asr #12
	sub r5,r3,r4, asr #12
	
	; x coord mul by sine z
	movs r3,r0
	rsbmi r3,r3,#0
	mul r3,r7,r3
	rsbmi r3,r3,#0
	; z coord mul by cosine z
	movs r4,r2
	rsbmi r4,r4,#0
	mul r4,r6,r4
	rsbmi r4,r4,#0
	; scale down values and add them for new y value in r1
	mov r3,r3, asr #12
	add r2,r3,r4, asr #12

	; put new x value back into r0
	mov r0,r5
	
	; exit with x,z ( r0 , r2 ) rotated by r3
	mov pc,r14

setup_camera_rotations
	stmfd sp!,{r14}

	adr r12,camera_rotations
	
	mov r0,#4096
	mov r1,#0
	mov r2,#0

	adrl r3,camera_x_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_x

	adrl r3,camera_y_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_y

	adrl r3,camera_z_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_z
		
	stmia r12!,{r0-r2}
	
	mov r0,#0
	mov r1,#4096
	mov r2,#0

	adrl r3,camera_x_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_x

	adrl r3,camera_y_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_y

	adrl r3,camera_z_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_z
		
	stmia r12!,{r0-r2}

	mov r0,#0
	mov r1,#0
	mov r2,#4096

	adrl r3,camera_x_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_x

	adrl r3,camera_y_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_y

	adrl r3,camera_z_rotation
	ldr r3,[r3,#0]
	bl rotate_coords_z
	
	stmia r12!,{r0-r2}	
	
	ldmfd sp!,{pc}

camera_rotations
	DCD 0,0,0
	DCD 0,0,0
	DCD 0,0,0
	
rotate_node_x_y_z_camera	
	
	;input r0-r2 x,y,z
	;output rotated by camera view

	adr r9,camera_rotations

	;X rotations		
	ldmia r9!,{r6-r8}

	cmp r6,#0
	rsblt r6,r6,#0
	mul r3,r0,r6
	rsblt r3,r3,#0
	
	cmp r7,#0
	rsblt r7,r7,#0
	mul r4,r0,r7
	rsblt r4,r4,#0
	
	cmp r8,#0
	rsblt r8,r8,#0
	mul r5,r0,r8
	rsblt r5,r5,#0

	;Y rotations		
	ldmia r9!,{r6-r8}

	cmp r6,#0
	rsblt r6,r6,#0
	rsblt r1,r1,#0
	mla r3,r1,r6,r3
	rsblt r1,r1,#0
	
	cmp r7,#0
	rsblt r7,r7,#0
	rsblt r1,r1,#0
	mla r4,r1,r7,r4
	rsblt r1,r1,#0
	
	cmp r8,#0
	rsblt r8,r8,#0
	rsblt r1,r1,#0
	mla r5,r1,r8,r5

	;Z rotations		
	ldmia r9!,{r6-r8}

	cmp r6,#0
	rsblt r6,r6,#0
	rsblt r2,r2,#0
	mla r3,r2,r6,r3
	rsblt r2,r2,#0
	
	cmp r7,#0
	rsblt r7,r7,#0
	rsblt r2,r2,#0
	mla r4,r2,r7,r4
	rsblt r2,r2,#0
	
	cmp r8,#0
	rsblt r8,r8,#0
	rsblt r2,r2,#0
	mla r5,r2,r8,r5

	mov r0,r3, asr #12
	mov r1,r4, asr #12
	mov r2,r5, asr #12

	mov pc,r14


rotate_node_from_c
	stmfd sp!,{r4-r12,r14}
	
	;r0 - data pointer
	;	0,1,2 words x,y,z position of node
	;	3,4,5 words x,y,z rotation ( 1024 * 1024 = 360 degrees )
	
	;output rotated coords in word 0,1,2 of data block
	
	mov r12,r0
	ldmia r12,{r0-r2}
	
	mov r0,r0, asr #12		;scale down coords
	mov r1,r1, asr #12
	mov r2,r2, asr #12
	
	ldr r3,[r12,#20]
	mov r3,r3, asr #10
	rsb r3,r3,#1024			;flip the z rot
	bl rotate_coords_z
	
	ldr r3,[r12,#16]
	mov r3,r3, asr #10
	bl rotate_coords_y
	
	ldr r3,[r12,#12]
	mov r3,r3, asr #10
	bl rotate_coords_x
	
	mov r0,r0, asl #12		;scale up coords
	mov r1,r1, asl #12
	mov r2,r2, asl #12
	
	rsb r1,r1,#0			;flip the y
	
	stmia r12,{r0-r2}
	
	ldmfd sp!,{r4-r12,pc}^


oppo_rotate_node_from_c
	stmfd sp!,{r4-r12,r14}
	
	;r0 - data pointer
	;	0,1,2 words x,y,z position of node
	;	3,4,5 words x,y,z rotation ( 1024 * 1024 = 360 degrees )
	
	;output rotated coords in word 0,1,2 of data block
	
	mov r12,r0
	ldmia r12,{r0-r2}
	
	mov r0,r0, asr #12		;scale down coords
	mov r1,r1, asr #12
	mov r2,r2, asr #12
	
	ldr r3,[r12,#12]
	mov r3,r3, asr #10
	bl rotate_coords_x
	
	ldr r3,[r12,#16]
	mov r3,r3, asr #10
	bl rotate_coords_y
	
	ldr r3,[r12,#20]
	mov r3,r3, asr #10
	rsb r3,r3,#1024			;flip the z rot
	bl rotate_coords_z
	
	mov r0,r0, asl #12		;scale up coords
	mov r1,r1, asl #12
	mov r2,r2, asl #12
	
	rsb r1,r1,#0			;flip the y
	
	stmia r12,{r0-r2}
	
	ldmfd sp!,{r4-r12,pc}^


rotate_land_node_from_c
	stmfd sp!,{r4-r12,r14}
	
	;r0 - data pointer
	;	0,1,2 words x,y,z position of node
	;	3,4,5 words x,y,z rotation ( 1024 * 1024 = 360 degrees )
	
	;output rotated coords in word 0,1,2 of data block
	
	mov r12,r0
	ldmia r12,{r0-r2}
	
	ldr r3,[r12,#12]
	mov r3,r3, asr #10
	bl rotate_coords_x
	
	ldr r3,[r12,#16]
	mov r3,r3, asr #10
	bl rotate_coords_y

	ldr r3,[r12,#20]
	mov r3,r3, asr #10
	bl rotate_coords_z
		
	stmia r12,{r0-r2}
	
	ldmfd sp!,{r4-r12,pc}^


target_details
	DCD 0
target_distance
	DCD 0,0,0
target_x
	DCD 0
target_y
	DCD 0


target_finder
	;input - r0 pointer to target data
	; words 0 - 2 aim from position x,y,z
	; words 3 - 5 aim at position x,y,z
	; words 6 - 7 output x , y rotation required
	
	stmfd sp!,{r4-r12,r14}
	
	str r0,target_details
	
	;Find relative distances and stash them
	;Remember these are ship units so they can be big
	;scale em' down before using them
	ldmia r0,{r0-r5}
	rsb r0,r0,r3			;Flip the x distance
	sub r1,r1,r4
	sub r2,r2,r5
	adr r3,target_distance
	mov r0,r0, asr #14		;scale 'em down to a managerable size
	mov r1,r1, asr #14
	mov r2,r2, asr #14
	stmia r3,{r0-r2}
	
	;find the x rotation
	bl find_rotation
	str r0,target_x
	
	mov r0,r0, asr #10		;scale down the x rot for cosine table access
	
	;use the x rotation and the x n' y distance to find the
	;real distance along the x,y plane
	ldr r10,cosine_data
	add r11,r10,#1024*3			;get the start of the sine table
	ldr r10,[r10,r0, asl #2]	;get the cosine of x
	ldr r11,[r11,r0, asl #2]	;get the sine of x
	
	;Make these values unsigned
	cmp r10,#0
	rsblt r10,r10,#0
	cmp r11,#0
	rsblt r11,r11,#0
	
	;Get the relative distances
	adr r7,target_distance
	ldmia r7,{r7-r9}
	
	;Make the x,y distances unsigned
	cmp r7,#0
	rsblt r7,r7,#0
	cmp r8,#0
	rsblt r8,r8,#0
	
	;Mul the x distance by sine x rot and
	;add to the y distance mul cosine x rot
	mul r10,r8,r10
	mul r11,r7,r11
	;Add these values together and scale down
	add r1,r10,r11
	mov r1,r1, asr #12
	
	str r1,target_x_y_dist
	
	;Put the z distance in r0
	mov r0,r9
	
	;Find the y rotation
	bl find_rotation
	
	ldr r2,target_details
	str r0,[r2,#28]
	ldr r1,target_x
	str r1,[r2,#24]
	
	;Find the real distance based on x_y dist and z dist
	ldr r10,cosine_data
	add r11,r10,#1024*3			;get the start of the sine table
	mov r0,r0, asr #10
	ldr r10,[r10,r0, asl #2]	;get the cosine of x
	ldr r11,[r11,r0, asl #2]	;get the sine of x
	
	;Make these values unsigned
	cmp r10,#0
	rsblt r10,r10,#0
	cmp r11,#0
	rsblt r11,r11,#0
	
	ldr r8,target_x_y_dist
	ldr r9,target_distance+8
	cmp r9,#0
	rsblt r9,r9,#0

	mul r8,r10,r8
	mul r9,r11,r9
	
	add r9,r9,r8
	
	mov r9,r9, asl #2
	str r9,[r2,#32]
	
	ldmfd sp!,{r4-r12,pc}^

target_x_y_dist
	DCD 0


find_2d_distance_coords
	DCD 0,0

find_2d_distance
	;given r0,r1 x,y
	;output r0 - distance

	;Numbers must be kept smallish < 1<<18 ish

	stmfd sp!,{r4-r12,r14}
	
	adr r2,find_2d_distance_coords
	stmia r2,{r0-r1}
	
	bl find_rotation

	mov r0,r0, asr #10		;scale down the x rot for cosine table access
	
	;use the x rotation and the x n' y distance to find the
	;real distance along the x,y plane
	ldr r10,cosine_data
	add r11,r10,#1024*3			;get the start of the sine table
	ldr r10,[r10,r0, asl #2]	;get the cosine of x
	ldr r11,[r11,r0, asl #2]	;get the sine of x
	
	;Make these values unsigned
	cmp r10,#0
	rsblt r10,r10,#0
	cmp r11,#0
	rsblt r11,r11,#0
	
	;Get the relative distances
	adr r7,find_2d_distance_coords
	ldmia r7,{r7-r8}
	
	;Make the x,y distances unsigned
	cmp r7,#0
	rsblt r7,r7,#0
	cmp r8,#0
	rsblt r8,r8,#0
	
	;Mul the x distance by sine x rot and
	;add to the y distance mul cosine x rot
	mul r10,r8,r10
	mul r11,r7,r11
	;Add these values together and scale down
	add r1,r10,r11
	mov r0,r1, asr #12
	
	;output r0 - real dist between given coords
	
	ldmfd sp!,{r4-r12,pc}^

find_rot_vars
	DCD tangent_table


find_rotation

; Input r0,r1 x,y relative positions
; Use 4100 inverse tangent_table 
; output r0 - angle of rotation - 0 - 1024*1024

	;check that the values are sensible
	cmp r0,#0
	cmpeq r1,#0
	moveq pc,r14

	stmfd sp!,{r4-r12,r14}
	mov r11,r0
	mov r12,r1

	movs r9,r11
	rsbmi r9,r9,#0
	movs r10,r12
	rsbmi r10,r10,#0

	cmp r9,r10
	bgt East_West_find

	cmp r12,#0
	bgt North_found

; South Found

	mov r0,r9, asl #10
	mov r1,r10
	bl divide

	ldr r7,find_rot_vars
	ldr r6,[r7,r3, asl #2]

	cmp r11,#0
	addge r0,r6,#512*1024
	rsblt r0,r6,#512*1024
	ldmfd sp!,{r4-r12,pc}^

North_found

	mov r0,r9, asl #10
	mov r1,r10
	bl divide

	ldr r7,find_rot_vars
	ldr r6,[r7,r3, asl #2]

	cmp r11,#0
	movlt r0,r6
	rsbge r0,r6,#0

	cmp r0,#0
	addlt r0,r0,#1024*1024
	ldmfd sp!,{r4-r12,pc}^

East_West_find

	cmp r11,#0
	blt East_found

; West found

	mov r0,r10, asl #10
	mov r1,r9
	bl divide

	ldr r7,find_rot_vars
	ldr r6,[r7,r3, asl #2]
	cmp r12,#0
	addge r0,r6,#256*3*1024
	rsblt r0,r6,#256*3*1024
	ldmfd sp!,{r4-r12,pc}^

East_found

	mov r0,r10, asl #10
	mov r1,r9
	bl divide

	ldr r7,find_rot_vars
	ldr r6,[r7,r3, asl #2]
	cmp r12,#0
	addlt r0,r6,#256*1024
	rsbge r0,r6,#256*1024
	ldmfd sp!,{r4-r12,pc}^


divide

; Input x to be divided by y (r0 r1)

; Output answer and remainder (r3 r7)
; Uses r0 - r8

	mov R2,#1
	ands r4,r1,#1<<31
	rsbmi r1,r1,#0
	eor r5,r4,r0
	cmp r0,#0
	rsbmi r0,r0,#0
	cmp r1,#0
	mvneq r3,#&80000000
	beq divide_by_zero


divide_loop_1
	cmp r1,r0
	movcc r1,r1, asl #1
	movcc r2,r2, asl #1
	bcc divide_loop_1

	mov R3,#0

divide_loop_2
	cmp r0,r1
	subcs r0,r0,r1
	addcs r3,r3,r2
	movs r2,r2, lsr #1
	movne r1,r1, lsr #1
	bne divide_loop_2

divide_by_zero

	cmp r5,#0
	rsbmi r3,r3,#0

	mov pc,r14



;typedef struct smoke_stack 
;					{	link_header header ; 4*3 byte - 3 words
;						long x_pos ; long y_pos ; long z_pos ;
;						long x_vel ; long y_vel ; long z_vel ;
;						long counter ;
;						long colour ;
;						long type ;
;					} smoke_stack

mc_smoke_mover
	
	stmfd sp!,{r4-r7}
	
	;r0 - smoke adr
	add r0,r0,#12
	ldmia r0,{r1-r7}	;get data
	
	;r1-r3 x,y,z pos
	;r4-r6 x,y,z speed
	;r7 counter
	
	;slow down the smokes velocity
	sub r4,r4,r4, asr #6
	sub r5,r5,r5, asr #6
	sub r6,r6,r6, asr #6
	
	;add velocity onto x ,y ,z positions
	add r1,r1,r4
	add r2,r2,r5
	add r3,r3,r6
	
	;dec counter
	sub r7,r7,#1
	
	stmia r0,{r1-r7}	;stash data

	ldmfd sp!,{r4-r7}
	mov pc,r14


; scan_poly_map is called from C to find any active ground objects
; in a given area (Normally around the players ship) these ground objects
; are Ground Guns (ref 1-3) Sam bases (4-6) Ship hangers (7-8) and
; smoking chimneys (9-10) ?

poly_scan_map
	DCD poly_map

scan_poly_map

	stmfd sp!,{r4-r12,r14}
	
	;r0 x pos in grid
	;r1 y pos in grid
	;r2 objects found stack
	
	bic r0,r0,#3		;quad align x pos 
	
	ldr r3,poly_scan_map
	mov r11,#31		;x counter
	mov r12,#31		;y counter
	
poly_scan_loop

	add r4,r0,r1, asl #7	;pos in grid based on x + y*128
	ldr r5,[r3,r4]			;load 1 word at a time ( 4 object units)
	cmp r5,#0				;is there anything here?
	bne scan_found_object_in_word
continue_poly_scan
	
	;Update x counter and continue if still positive
	subs r11,r11,#4
	addpl r0,r0,#4
	andpl r0,r0,#127
	bpl poly_scan_loop
	
	;reset x counter and put x pointer to start of new line
	subs r0,r0,#28
	andmi r0,r0,#127
	mov r11,#31
	
	subs r12,r12,#1				;check vert counter and bail if neg
	
	addpl r1,r1,#1				;inc vert grid pos
	andpl r1,r1,#127
	bpl poly_scan_loop
	
end_of_poly_map_scan

	mvn r3,#0		;end the list with a neg number
	str r3,[r2,#0]
	
	ldmfd sp!,{r4-r12,pc}^

	
scan_found_object_in_word
	
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_1_failed
	cmp r5,#8
	strle r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_1_failed

	add r4,r4,#1		;skip next byte
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_2_failed

	cmp r5,#8
	strle r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_2_failed

	add r4,r4,#1		;next byte
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_3_failed
	cmp r5,#8
	strle r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_3_failed

	add r4,r4,#1		;next
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_4_failed
	cmp r5,#8
	strle r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_4_failed
	
	b continue_poly_scan


; scan_poly_map_2 is called from C to find any ground objects which
; require plotting in a given area (Normally around the camera pos)
; these ground objects are (ref 1-63) clouds (64-128) 
; Never try to plot object (ref 248+) as these are collision box 
; references

scan_poly_map_2

	stmfd sp!,{r4-r12,r14}
	
	;r0 x pos in grid
	;r1 y pos in grid
	;r2 objects found stack
	
	bic r0,r0,#3		;quad align x pos 
	
	ldr r3,poly_scan_map
	mov r11,#31		;x counter
	mov r12,#31		;y counter
	
poly_scan_loop_2

	add r4,r0,r1, asl #7	;pos in grid based on x + y*128
	ldr r5,[r3,r4]			;load 1 word at a time ( 4 object units)
	cmp r5,#0				;is there anything here?
	bne scan_found_object_in_word_2
continue_poly_scan_2
	
	;Update x counter and continue if still positive
	subs r11,r11,#4
	addpl r0,r0,#4
	andpl r0,r0,#127
	bpl poly_scan_loop_2
	
	;reset x counter and put x pointer to start of new line
	subs r0,r0,#28
	andmi r0,r0,#127
	mov r11,#31
	
	subs r12,r12,#1				;check vert counter and bail if neg
	
	addpl r1,r1,#1				;inc vert grid pos
	andpl r1,r1,#127
	bpl poly_scan_loop_2
 	
end_of_poly_map_scan_2

	mvn r3,#0		;end the list with a neg number
	str r3,[r2,#0]
	
	ldmfd sp!,{r4-r12,pc}

	
scan_found_object_in_word_2
	
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_1_failed_2
	cmp r5,#248
	strlt r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_1_failed_2

	add r4,r4,#1		;skip next byte
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_2_failed_2
	cmp r5,#248
	strlt r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_2_failed_2

	add r4,r4,#1		;next byte
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_3_failed_2
	cmp r5,#248
	strlt r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_3_failed_2
	
	add r4,r4,#1		;next
	ldrb r5,[r3,r4]		;is there a valid object in this byte
	cmp r5,#0
	beq scan_byte_4_failed_2
	cmp r5,#248
	strlt r4,[r2],#4	;yes store its ref in grid for C to deal with
scan_byte_4_failed_2
	
	b continue_poly_scan_2


	END

