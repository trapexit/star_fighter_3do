	AREA |c$$code|,CODE,READONLY

	IMPORT	rotate_node_x_y_z_camera
	IMPORT	camera_x_position
	IMPORT	pex_table
	IMPORT	arm_addgamecel
	IMPORT	cel_quad

	EXPORT	plot_smoke


;smoke_stack structure - see file Smoke_Struct.h

smoke_header		EQU		12
smoke_x_pos			EQU		0
smoke_y_pos			EQU		4
smoke_z_pos			EQU		8
smoke_x_vel			EQU		12
smoke_y_vel			EQU		16
smoke_z_vel			EQU		20
smoke_counter		EQU		24
smoke_colour		EQU		28
smoke_type			EQU		32

;Smoke type defines - see file Smoke_Control.h

SMALL_THRUSTER_SMOKE	EQU		0 
EXPLOSION_SMOKE			EQU		1
MUSHROOM_SMOKE			EQU		2
BIG_MUSHROOM_CLOUD		EQU		3
SMALL_MUSHROOM_CLOUD	EQU		4
MISSILE_SMOKE			EQU		5
BIG_THRUSTER_SMOKE		EQU		6
BIG_DAMAGE_SMOKE		EQU		7
WING_POD_SMOKE			EQU		8

BASE_THRUSTER_COLOUR	EQU		78


perspective_table
	DCD pex_table
cel_quad_data
	DCD cel_quad

smoke_to_plot
	DCD 0
plot_smoke_type
	DCD 0
plot_smoke_colour
	DCD 0
plot_smoke_shade
	DCD 0

plot_smoke

	stmfd sp!,{r14}

	;r0 - pointer to smoke

	add r0,r0,#smoke_header		;skip over t' link list header

	str r0,smoke_to_plot
	ldr r11,[r0,#smoke_type]
	str r11,plot_smoke_type

	ldmia r0,{r0-r2}
	adrl r3,camera_x_position
	ldmia r3,{r3-r5}
	sub r0,r0,r3
	sub r1,r1,r4
	sub r2,r5,r2
	
	mov r0,r0, asr #12
	mov r1,r1, asr #12
	mov r2,r2, asr #12
		
	bl rotate_node_x_y_z_camera

	;r0-r2 x,y,z coords for smoke
	
	;is the smoke to near or behind the camera

	ldr r3,plot_smoke_type
	adr r4,smoke_clip_table
	ldr r3,[r4,r3, asl #2]

	cmp r1,r3
	ldmltfd sp!,{pc}^
	
	;check if smoke is out of 45 deg. check angle cone
	movs r3,r0		;get unsigned rotated x
	rsbmi r3,r3,#0
	cmp r3,r1		;check against y
	ldmgtfd sp!,{pc}^
	
	movs r3,r2		;get unsigned rotated z
	rsbmi r3,r3,#0
	cmp r3,r1		;check against y
	ldmgtfd sp!,{pc}^
	
	
	;get the perspective size of cloud based on y distance
	ldr r4,perspective_table
	bic r1,r1,#31
	ldr r1,[r4,r1, asr #3]
		
	mov r6,#128			;screen corner offset
	mov r7,r1, asr #1
		
	;Branch to the required smoke scaler
	adr r11,smoke_jump_table
	ldr r12,plot_smoke_type
	
	mov r14,pc
	ldr pc,[r11,r12, asl #2]
		
	;r1 - smoke scaler
	
	;1024 sub = scale asl #2
	
	sub r0,r0,r6
	sub r2,r2,r6
	
	;top left corner of cloud r4 - r5
	mul r4,r0,r1
	mov r4,r4, asr #16
	mul r5,r2,r1
	mov r5,r5, asr #16

	;Is the smoke on screen		
;	cmp r4,#120
;	ldmgtfd sp!,{pc}^
;	cmp r5,#80
;	ldmgtfd sp!,{pc}^
	
;	sub r6,r4,r7, asr #7
;	cmn r6,#120
;	ldmltfd sp!,{pc}^
;	sub r6,r5,r7, asr #7
;	cmn r6,#80
;	ldmltfd sp!,{pc}^
		
	;store coords in polygon data block
	ldr r0,cel_quad_data
	add r6,r0,#4
	stmia r6!,{r4-r5}
	
	;smoke shade
	ldr r6,plot_smoke_shade
	str r6,[r0,#40]
	
	;r0 - cel quad
	;r1 - cel type
	;r2 - x scale - 1024 = normal size
	;r3 - y scale 
		
	mov r2,r7	;scale
	mov r3,r7
	
	ldr r1,plot_smoke_colour
	
	;plot smoke
	bl arm_addgamecel
	
	ldmfd sp!,{pc}^

smoke_jump_table
	DCD small_thruster_smoke	;	SMALL_THRUSTER_SMOKE		0
	DCD explosion_smoke			;	EXPLOSION_SMOKE				1
	DCD explosion_smoke			;	MUSHROOM_SMOKE				2
	DCD explosion_smoke			;	BIG_MUSHROOM_CLOUD			3
	DCD explosion_smoke			;	SMALL_MUSHROOM_CLOUD		4
	DCD missile_thruster_smoke	;	MISSILE_SMOKE				5
	DCD big_thruster_smoke		;	BIG_THRUSTER_SMOKE			6
	DCD	big_damage_smoke		;	BIG_DAMAGE_SMOKE			7
	DCD missile_thruster_smoke	;	WING_POD_SMOKE				8
	DCD laser_hit_smoke			;	LASER_HIT_SMOKE				9
	DCD explosion_smoke			;	MEDIUM_MUSHROOM_CLOUD		10
	DCD black_explosion_smoke	;	BLACK_EXPLOSION_SMOKE		11
	DCD small_explosion_smoke	;	SMALL_EXPLOSION_SMOKE		12
	DCD big_explosion_smoke		;	BIG_EXPLOSION_SMOKE			13
	DCD	small_damage_smoke		;	SMALL_DAMAGE_SMOKE			14
	DCD	small_explosion_smoke	;	SECTION_SMOKE				15
	DCD	small_damage_smoke		;	SECTION_SMOKE2				16
	DCD	big_section_thrust		;	SECTION_SMOKE3				17
	DCD	big_section_smoke		;	SECTION_SMOKE4				18

smoke_clip_table
	DCD 1024		;0
	DCD 8192		;1
	DCD 8192		;2
	DCD 8192		;3
	DCD 8192		;4
	DCD 1024		;5
	DCD 8192		;6
	DCD 8192		;7
	DCD 1024		;8
	DCD 2048		;9
	DCD 8192		;10
	DCD 8192		;11
	DCD 6*1024		;12
	DCD 4096		;13
	DCD 1024		;14
	DCD 4096		;15
	DCD 4096		;16
	DCD 8192		;17
	DCD 8192		;18

	
laser_hit_smoke
	
	;r6,7 - smoke scaler value
	
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	ldr r12,[r10,#smoke_counter]
	rsb r12,r12,#7
	
	; Put me back in
	add r11,r11,r12
	
	str r11,plot_smoke_colour

	mov r12,#12
	str r12,plot_smoke_shade

	;Extra scale up cos this is a 32*32 sprite instead of 8*8
	mov r6,r6, asl #5	
	mov r7,r7, asl #3
	
	;mov r6,r6, asl #4	
	;mov r7,r7, asl #2
	
	
	mov pc,r14

small_thruster_smoke
	
	;r6,7 - smoke scaler value
	
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour

	ldr r12,[r10,#smoke_counter]		;counter
	
	sub r11,r11,#BASE_THRUSTER_COLOUR
	add r12,r12,r11, asr #1

	str r12,plot_smoke_shade

	mov pc,r14

		
missile_thruster_smoke
	
	;r6,7 - smoke scaler value
	
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour

	ldr r12,[r10,#smoke_counter]		;counter
	
	add r12,r12,#4

	str r12,plot_smoke_shade

	mov pc,r14
	
big_thruster_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	str r4,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#8
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8, asr #1
	mov r7,r9, asr #1
	
	mov pc,r14

big_damage_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	mov r5,r4, asr #3
	mov r5,#12
	str r5,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#8
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8, asr #1
	mov r7,r9, asr #1
	
	mov pc,r14


big_section_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	mov r5,r4, asr #3
	mov r5,#12
	str r5,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#8
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8;   , asr #1
	mov r7,r9;   , asr #1
	
	mov pc,r14


small_damage_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	mov r5,r4, asr #3
	mov r5,#12
	str r5,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#8
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8, asr #3
	mov r7,r9, asr #3
	
	mov pc,r14

explosion_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	str r4,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#16
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8, asr #1
	mov r7,r9, asr #1
	
	mov pc,r14

small_explosion_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	str r4,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#16
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8, asr #2
	mov r7,r9, asr #2
	
	mov pc,r14


big_explosion_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	str r4,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#16
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8;, asr #1
	mov r7,r9;, asr #1
	
	mov pc,r14

big_section_thrust

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	str r4,plot_smoke_shade
	
	mov r4,r4, asl #1
	add r4,r4,#16
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8, asl #1
	mov r7,r9, asl #1
	
	mov pc,r14

black_explosion_smoke

	;Scale the smoke based on its counter
	ldr r10,smoke_to_plot
	ldr r11,[r10,#smoke_colour]
	str r11,plot_smoke_colour
		
	ldr r4,[r10,#smoke_counter]		;counter	
	
	mov r5,#12
	str r5,plot_smoke_shade
	
	rsb r4,r4,#16
	mov r4,r4, asl #2
	add r4,r4,#32
	
	mul r8,r6,r4
	mul r9,r7,r4
	mov r6,r8, asr #1
	mov r7,r9, asr #1
	
	mov pc,r14

	END


















