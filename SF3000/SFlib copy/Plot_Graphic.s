	AREA |c$$code|,CODE,READONLY

	EXPORT		clip_3d_line
	EXPORT		plot_static_graphic
	EXPORT		machine_code_constants
	EXPORT		plot_stars
	EXPORT		rotate_sky
	EXPORT		setup_rotations
	EXPORT		camera_x_rotation
	EXPORT		camera_y_rotation
	EXPORT		camera_z_rotation
	EXPORT		camera_x_position
	EXPORT		camera_y_position
	EXPORT		camera_z_position
	EXPORT		plot_ship_graphic
	EXPORT		plot_bit_graphic
	EXPORT		plot_spinning_ship
	EXPORT		plot_static_from_grid
	EXPORT		air_to_ground_scan_temp
	EXPORT		air_to_ground_scan
	EXPORT		air_to_ground_x
	EXPORT		air_to_ground_y
	EXPORT		air_to_ground_z
	EXPORT		air_to_air_scan_temp
	EXPORT		air_to_air_scan
	EXPORT		air_to_air_x
	EXPORT		air_to_air_y
	EXPORT		air_to_air_z
	EXPORT		atg_selected
	EXPORT		ata_selected
	EXPORT		collision_box_colour_adder
	EXPORT		rotate_sky_node
	EXPORT		camera_x_velocity
	EXPORT		camera_y_velocity
	EXPORT		camera_z_velocity
	EXPORT		plot_space_stars
	EXPORT		planet_1_x_pos
	EXPORT		planet_1_y_pos
	EXPORT		planet_1_z_pos
	EXPORT		planet_2_x_pos
	EXPORT		planet_2_y_pos
	EXPORT		planet_2_z_pos
	EXPORT		plot_planets
	EXPORT		silly_x
	EXPORT		silly_y
	EXPORT		are_we_in_space_or_wot

	IMPORT		arm_addgamecel
	IMPORT		arm_addpolycel32
	IMPORT		graphic_screen_coords
	IMPORT		graphic_rotated_coords
	IMPORT		rotate_coords_x
	IMPORT		rotate_coords_y
	IMPORT		rotate_coords_z
	IMPORT		arm_addmonocel
	IMPORT		arm_setpolycel32palette
	IMPORT		divide
	IMPORT		rotate_node_x_y_z_camera
	IMPORT		setup_camera_rotations

;Game cel ref no for fluffy cloud bit - assumes base + (0-3) cloud sprites +(4-7) tree sprites
FLUFFY_CLOUD	EQU		58

POLY_CLIP_DIST	EQU		1280

;colour for monochrome cells
red				EQU		0
green			EQU		8
yellow			EQU		16
blue			EQU		24
magenta			EQU		32
cyan			EQU		40
white			EQU		48
	
;graphics_details structure - see file Graphic_Struct.h

clip_size		EQU		0
score			EQU		4
graphic_adr		EQU		8
explosion_adr	EQU		12
					
collision_adr	EQU		16
hits_counter	EQU		20
x_size			EQU		24
y_size			EQU		28
					
z_size			EQU		32
laser_data		EQU		36
missile_data	EQU		40
ship_data		EQU		44
					
smoke_data		EQU		48
thruster_data	EQU		52
missile_aim		EQU		56
ship_2_data		EQU		60

;ship_stack structure - see file Ship_Struct.h

ship_header				EQU		12
ship_x_pos				EQU		0
ship_y_pos				EQU		4
ship_z_pos				EQU		8
ship_x_rot				EQU		12
ship_y_rot				EQU		16
ship_z_rot				EQU		20
ship_type				EQU		24
ship_collision_size		EQU		28
ship_who_owns_me		EQU		32
ship_what_hit_me		EQU		36
ship_counter			EQU		40
ship_can_see			EQU		108
ship_special_data		EQU		112

;smoke_stack structure - see file Smoke_Struct.h

smoke_x_pos			EQU		0
smoke_y_pos			EQU		4
smoke_z_pos			EQU		8
smoke_x_vel			EQU		12
smoke_y_vel			EQU		16
smoke_z_vel			EQU		20
smoke_counter		EQU		24
smoke_colour		EQU		28
smoke_type			EQU		32

;bit_stack structure - see file Bit_Struct.h

bit_x_pos		EQU		0
bit_y_pos		EQU		4
bit_z_pos		EQU		8
bit_x_rot		EQU		12
bit_y_rot		EQU		16
bit_z_rot		EQU		20
bit_type		EQU		24
bit_colour1		EQU		28
bit_colour2		EQU		32
bit_counter		EQU		64

;Whats the max size of the planets
PLANET_SIZE		EQU		48

;Waht are the ship types - see Ship_Struct.h
PLAYERS_SHIP	EQU		0
SMALL_SHIP		EQU		1
BIG_SHIP		EQU		2
BONUS			EQU		3
PARACHUTE		EQU		4
SATELLITE		EQU		5
WEAPON			EQU		6
CAR				EQU		7

planet_1_x_pos
	DCD 0
planet_1_y_pos
	DCD 0
planet_1_z_pos
	DCD 0
planet_1_ref
	DCD 14

planet_2_x_pos
	DCD 0
planet_2_y_pos
	DCD 0
planet_2_z_pos
	DCD 0
planet_2_ref
	DCD 15

planet_shade
	DCD 0

plot_planets

	stmfd sp!,{r4-r12,r14}
	
	;Get the shade value of the planets 0-12 (0=faint 12=strong)
	;Base this on the sky file height offset relative to camera height
	ldr r0,sky_data
	ldr r0,[r0,#4]
	ldr r1,camera_z_position
	subs r0,r1,r0, asl #17
	mov r0,r0, asr #23
	cmp r0,#5	;min base shade
	movlt r0,#5
	mov r0,#12
	str r0,planet_shade
	
	adr r0,planet_1_x_pos
	bl plot_this_planet
	
	adr r0,planet_2_x_pos
	bl plot_this_planet
	
	ldmfd sp!,{r4-r12,pc}^
	
planet_to_plot
	DCD 0
	
plot_this_planet
	
	stmfd sp!,{r14}
	str r0,planet_to_plot
	
	ldmia r0,{r0-r2}
	bl rotate_node_x_y_z_camera
	
	cmp r1,#8192
	ldmltfd sp!,{pc}^

	ldr r10,perspective_table
	
	;get screen coords with perspective lookup table
	bic r1,r1,#31
	ldr r1,[r10,r1, asr #3]
	mul r3,r0,r1
	mul r4,r2,r1
	mov r3,r3, asr #16
	mov r4,r4, asr #16
	
	;check if planet is on screen
	cmp r3,#160+(PLANET_SIZE/2)
	ldmgtfd sp!,{pc}^
	cmn r3,#160+(PLANET_SIZE/2)
	ldmltfd sp!,{pc}^
	cmp r4,#120+(PLANET_SIZE/2)
	ldmgtfd sp!,{pc}^
	cmn r4,#120-(PLANET_SIZE/2)
	ldmltfd sp!,{pc}^
	
	;Centre the planet
	sub r3,r3,#(PLANET_SIZE/2)
	sub r4,r4,#(PLANET_SIZE/2)
	
	ldr r0,cel_quad
	
	;store the coords in cel data block
	add r1,r0,#4
	stmia r1,{r3-r4}
	
	;shade value
	ldr r5,planet_shade
	
	;shade
	str r5,[r0,#40]
		
	;r0 - cel quad
	;r1 - cel type
	;r2 - x scale - 1024 = normal size
	;r3 - y scale 
	
	ldr r1,planet_to_plot
	ldr r1,[r1,#12]
	
	mov r2,#1024
	mov r3,#1024
	
	;plot planet
	bl arm_addgamecel
		
	ldmfd sp!,{pc}^


distant_ship_to_plot
	DCD 0

plot_this_distant_ship
	
	stmfd sp!,{r14}
	str r0,distant_ship_to_plot
	
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
	
	cmp r1,#8192
	ldmltfd sp!,{pc}^

	ldr r10,perspective_table
	
	;get screen coords with perspective lookup table
	bic r1,r1,#31
	ldr r1,[r10,r1, asr #3]
	mul r3,r0,r1
	mul r4,r2,r1
	mov r3,r3, asr #16
	mov r4,r4, asr #16
	
	;check if planet is on screen
	cmp r3,#168
	ldmgtfd sp!,{pc}^
	cmn r3,#168
	ldmltfd sp!,{pc}^
	cmp r4,#128
	ldmgtfd sp!,{pc}^
	cmn r4,#128
	ldmltfd sp!,{pc}^
	
	;Centre the planet
	sub r3,r3,#4
	sub r4,r4,#4
	
	ldr r0,cel_quad
	
	;store the coords in cel data block
	add r1,r0,#4
	stmia r1,{r3-r4}
	
	;shade value
	mov r5,#12
	
	;shade
	str r5,[r0,#40]
		
	;r0 - cel quad
	;r1 - cel type
	;r2 - x scale - 1024 = normal size
	;r3 - y scale 
	
	ldr r1,distant_ship_to_plot
	ldr r1,[r1,#ship_type]
	
	mov r1,r1, asr #4
	
	cmp r1,#WEAPON
	moveq r1,#42
	beq dist_ship_type_found

	cmp r1,#PARACHUTE
	moveq r1,#43
	beq dist_ship_type_found

	ldr r1,distant_ship_to_plot
	ldr r1,[r1,#ship_special_data]
	ldrb r1,[r1,#2]
	cmp r1,#0
	moveq r1,#41
	movne r1,#40

dist_ship_type_found

	mov r2,#1024
	mov r3,#1024
	
	;plot ship
	bl arm_addgamecel
		
	ldmfd sp!,{pc}^


plot_space_stars

	;Are we high enought to see the stars
	ldr r0,sky_data
	ldr r0,[r0,#4]
	ldr r1,camera_z_position
	subs r0,r1,r0, asl #17
	;movmi pc,r14

	mov r0,r0, asr #24
	
	mov r0,#2048
	str r0,star_shade

	stmfd sp!,{r4-r12,r14}
	
	str sp,stack_pointer
	
	mov r0,#0
	str r0,stars_to_plot
	
	
	ldr r13,graphic_screen_coords_1
	str r13,star_stack
	
	mov r12,#127 ; loop counter
	ldr r11,star_data
	ldr r10,perspective_table
	
space_star_loop
	ldmia r11,{r0-r2,r13}

	adr r3,camera_x_velocity
	ldmia r3,{r3-r5}

	add r0,r0,r3, asr #14
	add r1,r1,r4, asr #14
	sub r2,r2,r5, asr #14

	mov r3,#1<<16
	sub r3,r3,#1

	and r0,r0,r3
	and r1,r1,r3
	and r2,r2,r3

	stmia r11,{r0-r2}
	add r11,r11,#16

	sub r0,r0,#1<<15
	sub r1,r1,#1<<15
	sub r2,r2,#1<<15

	bl rotate_node_x_y_z_camera
	
	;Is the star behind the camera view point
	cmp r1,#1024
	blt no_space_star_plot
	
	;get screen coords with perspective lookup table
	bic r1,r1,#31
	ldr r1,[r10,r1, asr #3]
	mul r3,r0,r1
	mul r4,r2,r1
	mov r3,r3, asr #16
	mov r4,r4, asr #16
	
	;check if star is on screen
	cmp r3,#180
	bgt no_space_star_plot
	cmn r3,#180
	blt no_space_star_plot
	cmp r4,#140
	bgt no_space_star_plot
	cmn r4,#140
	blt no_space_star_plot
	
	;put star in plot list and inc counter
		
	ldr r5,star_stack
	stmia r5!,{r3-r4,r13}
	str r5,star_stack
	
	ldr r5,stars_to_plot
	add r5,r5,#1
	str r5,stars_to_plot
	
no_space_star_plot
	subs r12,r12,#1
	bpl space_star_loop
	
	ldr sp,stack_pointer
	
	;now plot the stars in the list
	ldr r14,stars_to_plot
	subs r14,r14,#1
	ldmmifd sp!,{r4-r12,pc}^  ; there is no stars to plot
	
	ldr r12,graphic_screen_coords_1
	ldr r0,cel_quad
	
plot_the_space_stars_loop
	
	;get the x,y screen coords of the star to plot
	ldmia r12!,{r2-r4}
	
	;store the star plot vars
	stmfd sp!,{r0,r12,r14}
	
	;type of star to plot
	mov r1,r4, asr #24
	
	;shade offset value
	mov r4,r4, asr #16
	and r4,r4,#255
	ldr r5,star_shade
	sub r5,r5,r4
	
	mov r5,#12
	
	;shade
	str r5,[r0,#40]
	
	;store x and y pos in cel data block
	;(screen 0,0 = top left of t' screen)
	add r4,r0,#4
	stmia r4,{r2-r3}
	
	;r0 - cel quad
	;r1 - cel type
	;r2 - x scale - 1024 = normal size
	;r3 - y scale 
	
	mov r2,#1024
	mov r3,#1024
	
	;plot star
	bl arm_addgamecel
	
	ldmfd sp!,{r0,r12,r14}
	
	subs r14,r14,#1
	bpl plot_the_space_stars_loop
	
	ldmfd sp!,{r4-r12,pc}^



machine_code_constants

; This code is called once only when the game is setup
; The C code will pass a few pointers to save them being 
; passed when the game is running

; r0 - data pointer
	
	stmfd sp!,{r4-r12,r14}
	
	ldmia r0!,{r1-r12}
	adr r14,cosine_table
	stmia r14!,{r1-r12}
	ldmia r0!,{r1-r4}
	stmia r14!,{r1-r4}
		
	;setup start address for the 3 graphics types
	;0		offset from start to static graphics data
	;4		offset from start to ship graphics data
	;8		offset from start to explosion bits data
	
	ldr r0,graphics_data
	ldmia r0,{r1-r3}
	add r1,r1,r0
	str r1,static_graphics_data
	add r2,r2,r0
	str r2,ships_data
	add r3,r3,r0
	str r3,explosion_bits_data
	
	;the above pointers now contain the addresses of the start
	;of the data for these graphics
	;this data is in the format -
	
	;item 0
	
	;	byte	word
	;		0	0	off screen clip size ( max dist from centre)
	;		4	1	plot delay ( x,y size in sprite units )
	;		8	2	graphics plot data offset (from start of group list)
	;		12	3	explosion data offset
	;		16	4	collision data offset
	;		20	5	score for distruction
	;		24	6
	;		28	7
	
	;item 1 .....
	
	;each item is spaced by 32 bytes - item number << 5
	
	;there is a maximum of 64 static graphics
	
	;128 ships - split into 8 sub groups of
	
		;players ships			0-15
		;small ships			16-31
		;big ships				32-47
		;bonus coins			48-63
		;weapons				64-79
		;parachutes				80-95
		;satellites				96-111
		;docking bay / misc		112-127

	;there is a maximum of 16 items per ship sub group
	
	;when referencing item ship then
	
	;bits 4 - 6 which sub group
	;bits 0 - 3 which item in sub group
	
	;there is a maximum of 16 explosion bits 
	
	ldmfd sp!,{r4-r12,pc}^




static_graphics_data
	DCD 0
ships_data
	DCD 0
explosion_bits_data
	DCD 0


star_stack
	DCD 0
stars_to_plot
	DCD 0
star_shade
	DCD 0

camera_x_velocity
	DCD 0
camera_y_velocity
	DCD 0
camera_z_velocity
	DCD 0


plot_stars

	;Are we high enought to see the stars
	ldr r0,sky_data
	ldr r0,[r0,#4]
	ldr r1,camera_z_position
	subs r0,r1,r0, asl #17
	movmi pc,r14

	mov r0,r0, asr #24
	
	str r0,star_shade

	stmfd sp!,{r4-r12,r14}
	
	str sp,stack_pointer
	
	mov r0,#0
	str r0,stars_to_plot
	
	
	ldr r13,graphic_screen_coords_1
	str r13,star_stack
	
	mov r12,#127 ; loop counter
	ldr r11,star_data
	ldr r10,perspective_table
	
star_loop
	ldmia r11!,{r0-r2,r13}

	bl rotate_node_x_y_z_camera
	
	;Is the star behind the camera view point
	cmp r1,#1024
	blt no_star_plot
	
	;get screen coords with perspective lookup table
	bic r1,r1,#31
	ldr r1,[r10,r1, asr #3]
	mul r3,r0,r1
	mul r4,r2,r1
	mov r3,r3, asr #16
	mov r4,r4, asr #16
	
	;check if star is on screen
	cmp r3,#180
	bgt no_star_plot
	cmn r3,#180
	blt no_star_plot
	cmp r4,#140
	bgt no_star_plot
	cmn r4,#140
	blt no_star_plot
	
	;put star in plot list and inc counter
		
	ldr r5,star_stack
	stmia r5!,{r3-r4,r13}
	str r5,star_stack
	
	ldr r5,stars_to_plot
	add r5,r5,#1
	str r5,stars_to_plot
	
no_star_plot
	subs r12,r12,#1
	bpl star_loop
	
	ldr sp,stack_pointer
	
	;now plot the stars in the list
	ldr r14,stars_to_plot
	subs r14,r14,#1
	ldmmifd sp!,{r4-r12,pc}^  ; there is no stars to plot
	
	ldr r12,graphic_screen_coords_1
	ldr r0,cel_quad
	
plot_the_stars_loop
	
	;get the x,y screen coords of the star to plot
	ldmia r12!,{r2-r4}
	
	;store the star plot vars
	stmfd sp!,{r0,r12,r14}
	
	;type of star to plot
	mov r1,r4, asr #24
	
	;shade offset value
	mov r4,r4, asr #16
	and r4,r4,#255
	ldr r5,star_shade
	sub r5,r5,r4
	
	;shade
	str r5,[r0,#40]
	
	;store x and y pos in cel data block
	;(screen 0,0 = top left of t' screen)
	add r4,r0,#4
	stmia r4,{r2-r3}
	
	;r0 - cel quad
	;r1 - cel type
	;r2 - x scale - 1024 = normal size
	;r3 - y scale 
	
	mov r2,#1024
	mov r3,#1024
	
	;plot star
	bl arm_addgamecel
	
	ldmfd sp!,{r0,r12,r14}
	
	subs r14,r14,#1
	bpl plot_the_stars_loop
	
	ldmfd sp!,{r4-r12,pc}^
	

rotate_sky
	;r0 - pointer to data block
	stmfd sp!,{r4-r12,r14}
	mov r12,r0
	mov r0,#0
	mov r1,#16384
	mov r2,#0
	ldr r3,camera_y_rotation
	bl rotate_coords_y
	
	cmp r1,#2048
	movlt r1,#2048
	
	ldr r3,perspective_table
	bic r1,r1,#31
	ldr r1,[r3,r1, asr #3]
	mul r0,r2,r1
	mov r0,r0, asr #16
	add r0,r0,#150

	mov r0,#240
	
	str r0,[r12,#4]
	ldr r0,camera_z_position
	mov r0,r0, asr #17
	str r0,[r12,#0]
	
	ldmfd sp!,{r4-r12,pc}^

cosine_table			;0		1
	DCD 0
perspective_table		;4		2
	DCD 0
rotated_coords			;8		3
	DCD 0
landscape_heights		;12		4
	DCD 0
screen_coords			;16		5
	DCD 0
cel_quad				;20		6
	DCD 0
plot_graphic_delay		;24		7
	DCD 0
cel_masterlist			;28		8
	DCD 0
grid					;32		9
	DCD 0
cel_constantlist		;36		10
	DCD 0
; graphic_rotated_coords	40		11
	DCD 0
; graphic_screen_coords	44		12
	DCD 0
star_data				;48     1
	DCD 0
poly_map				;52		2
	DCD 0
sky_data				;56		3
	DCD 0
graphics_data			;60		4
	DCD 0

graphic_rotated_coords_1
	DCD graphic_rotated_coords
graphic_screen_coords_1
	DCD graphic_screen_coords

	

rotate_graphic

;this is the fast rotation graphics system which works by using
;3 sets of 3 movement coords (x,y,z) for the x,y,z planes
;these are added and subed off the centre point of the graphic
;to find the value of the next node in the graphics which is then
;stored .

;this can gain speed by reducing the number of muliplies
;required to find each node but it does have limitations

; r0 address of x,y,z movement in the x,y,z planes ( 9 words )

; r1 address of x,y,z centre point ( 3 words )


;data format - byte 40 number of nodes -1
;			 - byte 41 number of addition radar nodes
;	byte 42 ----
;              byte 1 - 3 x,y,z movement data
;              byte 4 - 6 ........
; per byte
; bits        7  set - sub else add
;             6  set - asr else asl
;             5  unused
;             4-0 value of shift
;
; if byte value = 0 then no movement
; for unshifted movement use bin 01000000 for positive
;                                11000000 for negative

	stmfd sp!,{r14}
	str sp,stack_pointer
	
	ldmia r0,{r2-r10}
	;r2-r4 x plane movement
	;r5-r7 y plane movement
	;r8-r10 z plane movement
	
	ldmia r1,{r11-r13}
	;r11-r13 x,y,z start coords

	ldr r0,graphic_start_adr
	
	ldrb r1,[r0,#33]		;node counter
	ldrb r14,[r0,#34]		;additional radar counter

	;mov r14,#0

	subs r14,r14,#1
	movmi r14,r1
	
	sub r14,r1,r14
	sub r1,r1,r14


	;get start adr of node data
	add r0,r0,#40

	; store loop counter
	str r14,radar_nodes		;additional nodes
	str r1,node_counter		;node counter
	
	;get total number of nodes and store
	add r14,r14,r1
	str r14,node_counter_2	;perspective loop counter
	
	ldr r1,graphic_rotated_coords_1
	;r1 - rotated coord store
	
node_loop

	;check x movement
	ldrb r14,[r0],#1
	cmp r14,#0
	beq quit_x_movement
	tst r14,#128
	bne negative_x_movement

	tst r14,#64
	and r14,r14,#31
	addne r11,r11,r2, asr r14
	addne r12,r12,r3, asr r14
	addne r13,r13,r4, asr r14
	addeq r11,r11,r2, asl r14
	addeq r12,r12,r3, asl r14
	addeq r13,r13,r4, asl r14
	b quit_x_movement

negative_x_movement	
	tst r14,#64
	and r14,r14,#31
	subne r11,r11,r2, asr r14
	subne r12,r12,r3, asr r14
	subne r13,r13,r4, asr r14
	subeq r11,r11,r2, asl r14
	subeq r12,r12,r3, asl r14
	subeq r13,r13,r4, asl r14	
quit_x_movement
	
	;check y movement
	ldrb r14,[r0],#1
	cmp r14,#0
	beq quit_y_movement
	tst r14,#128
	bne negative_y_movement
	
	tst r14,#64
	and r14,r14,#31
	addne r11,r11,r5, asr r14
	addne r12,r12,r6, asr r14
	addne r13,r13,r7, asr r14
	addeq r11,r11,r5, asl r14
	addeq r12,r12,r6, asl r14
	addeq r13,r13,r7, asl r14
	b quit_y_movement
	
negative_y_movement	
	tst r14,#64
	and r14,r14,#31
	subne r11,r11,r5, asr r14
	subne r12,r12,r6, asr r14
	subne r13,r13,r7, asr r14
	subeq r11,r11,r5, asl r14
	subeq r12,r12,r6, asl r14
	subeq r13,r13,r7, asl r14	
quit_y_movement
	
	;check z movement
	ldrb r14,[r0],#1
	cmp r14,#0
	beq quit_z_movement
	tst r14,#128
	bne negative_z_movement

	tst r14,#64
	and r14,r14,#31
	addne r11,r11,r8, asr r14
	addne r12,r12,r9, asr r14
	addne r13,r13,r10, asr r14
	addeq r11,r11,r8, asl r14
	addeq r12,r12,r9, asl r14
	addeq r13,r13,r10, asl r14
	b quit_z_movement

negative_z_movement	
	tst r14,#64
	and r14,r14,#31
	subne r11,r11,r8, asr r14
	subne r12,r12,r9, asr r14
	subne r13,r13,r10, asr r14
	subeq r11,r11,r8, asl r14
	subeq r12,r12,r9, asl r14
	subeq r13,r13,r10, asl r14	
quit_z_movement
	
	;store the new node
	stmia r1,{r11-r13}
	add r1,r1,#16		; store rotated coords in 16 byte spaces
						; to allow quicker random access
	
	;check loop counter and continue if ness
	ldr r14,node_counter
	subs r14,r14,#1
	strpl r14,node_counter
	bpl node_loop
	
	ldr r14,radar_nodes
	cmp r14,#0
	bgt radar_here
			
	ldr r0,graphic_rotated_coords_1
	ldr r1,perspective_table
	ldr r2,graphic_screen_coords_1
	ldr r3,node_counter_2
	
	bl do_perspective
	
	;restore stack adr
	ldr sp,stack_pointer	
	
	ldmfd sp!,{pc}^
		
node_counter
	DCD 0
node_counter_2
	DCD 0
radar_nodes
	DCD 0
	
stack_pointer
	DCD 0

radar_here
	str r14,node_counter
	adr r2,static_graphic_radar_rotation
	ldmia r2,{r2-r10}
	mov r14,#0
	str r14,radar_nodes
	b node_loop

node_data_store
	DCD 0

rotate_sky_node
	stmfd sp!,{r4-r12,r14}
	
	str r0,node_data_store
	ldmia r0,{r0-r2}
	
	ldr r3,camera_y_rotation

	;Limit sky y rot
;	cmp r3,#256*3
;	bgt low_y_limit
;	cmp r3,#256
;	blt low_y_limit
	
	;high y limit
;	cmp r3,#512-128
;	movlt r3,#512-128
	
;	cmp r3,#512+128
;	movgt r3,#512+128
	
;	mov r4,#1
;	str r4,sky_rot_flip_z
	
;	b finished_y_limit
	
;low_y_limit

;	mov r4,#0
;	str r4,sky_rot_flip_z
	
;	cmp r3,#512
;	bgt high_low_y_limit
	
;	cmp r3,#128
;	movgt r3,#128
;	b finished_y_limit	
	
;high_low_y_limit
;	cmp r3,#(256*3)+128
;	movlt r3,#(256*3)+128
;	b finished_y_limit
	
;finished_y_limit

	bl rotate_coords_y

	ldr r3,camera_z_rotation
;	ldr r4,sky_rot_flip_z
	
;	cmp r4,#0
	
;	addne r3,r3,#512
	
;	movne r5,#1024
;	subne r5,r5,#1
;	andne r3,r3,r5
	
	bl rotate_coords_z

	ldr r3,node_data_store
	stmia r3,{r0-r2}
	
	ldmfd sp!,{r4-r12,pc}

sky_rot_flip_z
	DCD 0

rotate_node_x_y_z_camera_plus_radar	
	stmfd sp!,{r14}

	;input r0-r2 x,y,z
	;output rotated by camera view

	ldr r3,camera_x_rotation
	ldr r4,radar_rotation
	subs r3,r3,r4
	addmi r3,r3,#1024
	
	bl rotate_coords_x

	ldr r3,camera_y_rotation
	bl rotate_coords_y
	
	ldr r3,camera_z_rotation
	bl rotate_coords_z
	
	ldmfd sp!,{pc}^

rotate_node_by_ship
	stmfd sp!,{r14}
	
	;r11 adr of ship
	
	ldr r3,[r11,#ship_z_rot]		;ships z rot
	mov r3,r3, asr #10
	
	bl rotate_coords_z
	
	ldr r3,[r11,#ship_y_rot]		;ships y rot
	mov r3,r3, asr #10
	
	bl rotate_coords_y
	
	ldr r3,[r11,#ship_x_rot]		;ship x rot combined with camera x rot
	
	ldr r4,camera_x_rotation		;get cam x rot
	subs r3,r4,r3, asr #10			;sub onto ships
	addmi r3,r3,#1024				;check for underflow 
	
	bl rotate_coords_x
	
	ldr r3,camera_y_rotation
	bl rotate_coords_y
	
	ldr r3,camera_z_rotation
	bl rotate_coords_z
	
	ldmfd sp!,{pc}^

setup_ship_rotations
	stmfd sp!,{r14}
	
	adr r12,ship_graphic_rotation
	ldr r11,ship_to_plot
	
	;get x plane movement and store it
	mov r0,#1024/2
	mov r1,#0
	mov r2,#0
	bl rotate_node_by_ship
	stmia r12!,{r0-r2}

	;get y plane movement and store it
	mov r0,#0
	mov r1,#1024/2
	mov r2,#0
	bl rotate_node_by_ship
	stmia r12!,{r0-r2}

	;get z plane movement and store it
	;mov r0,#0
	;mov r1,#0
	ldr r0,silly_y
	ldr r1,silly_x
	mov r2,#1024/2
	bl rotate_node_by_ship
	stmia r12!,{r0-r2}

	ldmfd sp!,{pc}^

setup_static_graphic_ship_rotations

	stmfd sp!,{r14}
	
	adr r12,ship_graphic_rotation
	ldr r11,ship_to_plot
	
	;get x plane movement and store it
	mov r0,#1024
	mov r1,#0
	mov r2,#0
	bl rotate_node_by_ship
	stmia r12!,{r0-r2}

	;get y plane movement and store it
	mov r0,#0
	mov r1,#1024
	mov r2,#0
	bl rotate_node_by_ship
	stmia r12!,{r0-r2}

	;get z plane movement and store it
	mov r0,#0
	mov r1,#0
	mov r2,#1024
	bl rotate_node_by_ship
	stmia r12!,{r0-r2}

	ldmfd sp!,{pc}^


setup_rotations
	stmfd sp!,{r4-r12,r14}
		
	;this routine is called once a frame to setup
	;rotated node movement for the static graphics
	;which are all at the same rotation - some
	;graphics have rotating radar on them , this
	;is also calculated and updated here

	bl setup_camera_rotations

	adr r12,static_graphic_rotation
	
	;get x plane movement and store it
	mov r0,#1024
	mov r1,#0
	mov r2,#0
	bl rotate_node_x_y_z_camera
	stmia r12!,{r0-r2}
	
	;get y plane movement and store it
	mov r0,#0
	mov r1,#1024
	mov r2,#0
	bl rotate_node_x_y_z_camera
	stmia r12!,{r0-r2}

	;get z plane movement and store it
	;mov r0,#0
	;mov r1,#0
	ldr r0,silly_x
	ldr r1,silly_y
	mov r2,#1024
	bl rotate_node_x_y_z_camera
	stmia r12!,{r0-r2}

	adr r12,static_graphic_radar_rotation
	
	;get x plane movement and store it
	mov r0,#1024
	mov r1,#0
	mov r2,#0
	bl rotate_node_x_y_z_camera_plus_radar
	stmia r12!,{r0-r2}
	
	;get y plane movement and store it
	mov r0,#0
	mov r1,#1024
	mov r2,#0
	bl rotate_node_x_y_z_camera_plus_radar
	stmia r12!,{r0-r2}

	;get z plane movement and store it
	;mov r0,#0
	;mov r1,#0
	ldr r0,silly_x
	ldr r1,silly_y
	mov r2,#1024
	bl rotate_node_x_y_z_camera_plus_radar
	stmia r12!,{r0-r2}


	;spin the radar
	ldr r0,radar_rotation
	subs r0,r0,#32
	addmi r0,r0,#1024
	str r0,radar_rotation
	
	ldmfd sp!,{r4-r12,pc}^

silly_x
	DCD 0
silly_y
	DCD 0

radar_rotation
	DCD 0

plot_graphic_x_pos
	DCD 0
plot_graphic_y_pos
	DCD 0
plot_graphic_z_pos
	DCD 0

camera_x_rotation
	DCD 0
camera_y_rotation
	DCD 0
camera_z_rotation
	DCD 0
camera_x_position
	DCD 0
camera_y_position
	DCD 0
camera_z_position
	DCD (1<<28)

plot_graphic_x_rot
	DCD 0
plot_graphic_y_rot
	DCD 0
plot_graphic_z_rot
	DCD 0

graphic_start_adr
	DCD 0


ship_graphic_rotation
	DCD 0,0,0,0,0,0,0,0,0

static_graphic_rotation
	DCD 0,0,0,0,0,0,0,0,0

static_graphic_radar_rotation
	DCD 0,0,0,0,0,0,0,0,0

ship_to_plot
	DCD 0

plot_ship_graphic
	;r0 - address of ship to plot
		;x_pos	0
		;y_pos	4
		;z_pos	8
		;x_rot	12
		;y_rot	16
		;z_rot	20
		;type	24

	stmfd sp!,{r14}
	add r0,r0,#ship_header
	
	ldr r1,[r0,#ship_can_see]
	cmp r1,#0
	beq far_plot_ship
	
	bl plot_ship
	b normal_plot_ship

far_plot_ship
	bl plot_this_distant_ship

normal_plot_ship	

	ldr r0,ata_selected
	cmp r0,#0
	ldmeqfd sp!,{pc}
	
	adr r0,plot_graphic_x_pos
	ldmia r0,{r0-r2}

	;Keep the x and z positive
	cmp r0,#0
	rsblt r0,r0,#0

	cmp r2,#0
	rsblt r2,r2,#0
	
	adr r11,air_to_air_x
	ldmia r11,{r3-r5}
	
	cmp r0,r3
	bgt air_to_air_check_this_ship
	
	;Is it a good lock
	;check x v' y
	cmp r1,r0, asl #2
	blt air_to_air_check_this_ship
	
	;check z v' y
	cmp r1,r2, asl #2
	blt air_to_air_check_this_ship
		
	ldr r4,ship_to_plot
	
	ldr r5,[r4,#ship_type]		;check type
	
	cmp r5,#256
	bge air_to_air_check_this_ship
	
	; Shift down the ship ref to compare with type
	mov r6,r5, asr #4
	
	cmp r6,#PLAYERS_SHIP

	cmpne r6,#BONUS
	
	cmpne r6,#WEAPON
	beq air_to_air_check_this_ship
		
	sub r6,r4,#ship_header
	str r6,air_to_air_scan_temp

	stmia r11,{r0-r2}
	
air_to_air_check_this_ship
	
	ldr r0,air_to_air_scan
	add r0,r0,#ship_header
	ldr r1,ship_to_plot
	cmp r1,r0
	ldmnefd sp!,{pc}
	
	str r0,ship_to_plot
	
	mov r0,#1		;set flag to plot ship
	b draw_collision_boxes

air_to_air_scan_temp
	DCD 0
air_to_air_scan
	DCD 0
air_to_air_x
	DCD 0
air_to_air_y
	DCD 0
air_to_air_z
	DCD 0
	
ata_selected	;If set to 0 then don't bother with highlight of ship graphics
	DCD 0

ship_clip_size
	DCD 0

plot_ship

	stmfd sp!,{r14}
	
	str r0,ship_to_plot


	ldrb r3,[r0,#ship_type]		;get type
	
	cmp r3,#255
	
	movge r4,#1<<26
	
	ldrlt r4,ships_data
	ldrlt r4,[r4,r3, asl #6]
	
	str r4,ship_clip_size


	;Rotation data for light shading
	ldr r1,[r0,#ship_y_rot]
	mov r1,r1, asr #12
	str r1,plot_graphic_y_rot
	ldr r1,[r0,#ship_z_rot]
	mov r1,r1, asr #12
	add r1,r1,#40
	str r1,plot_graphic_z_rot

	ldmia r0,{r0-r2}

	adr r3,camera_x_position
	ldmia r3,{r3-r5}
	sub r0,r0,r3
	sub r1,r1,r4
	sub r2,r5,r2
	
	mov r0,r0, asr #12
	mov r1,r1, asr #12
	mov r2,r2, asr #12
		
	bl rotate_node_x_y_z_camera
	
	adr r3,plot_graphic_x_pos
	
	stmia r3,{r0-r2}

	; Is the ship too far behind the camera
	ldr r4,ship_clip_size
	adds r4,r1,r4, asr #12
	ldmmifd sp!,{pc}
	
	;Is the ship off the left / right
	movs r5,r0
	rsbmi r5,r0,#0

	cmp r5,r4
	ldmgtfd sp!,{pc}
	
	;Is the ship off the top / bot
	movs r5,r2
	rsbmi r5,r2,#0
	
	cmp r5,r4
	ldmgtfd sp!,{pc}
	
	
	;Is this ship a static graphic bit or wot ?	
	ldr r10,ship_to_plot
	ldr r10,[r10,#ship_type]
	cmp r10,#255
	bgt this_ship_is_a_static_graphic
		
	bl setup_ship_rotations
	
	;Set the shade style
	mov r2,#1
	str r2,shade_style 	
	
	;Plot all the groups for this graphic
	mov r2,#0
	str r2,plot_group_mask
	
	ldr r1,ship_to_plot
	ldr r1,[r1,#ship_type]		;ship type
		
	ldr r0,ships_data	;ship data start
	add r0,r0,r1, asl #6
	ldr r0,[r0,#graphic_adr]
	
	str r0,graphic_start_adr
	
	adr r0,ship_graphic_rotation
	adr r1,plot_graphic_x_pos
	
	bl rotate_graphic
		
	ldr r0,graphic_start_adr
	
	adr r1,plot_style_list
	ldrb r0,[r0,#32]		;plot style for this graphic
	ldr pc,[r1,r0, asl #2]	;branch to plot routine for this graphic

;This is branched to from the ship plot bit
;for plotting sub groups of graphics for the explosions
this_ship_is_a_static_graphic
	
	cmp r10,#512
	bge group_setup_ship_rotations
				
	bl setup_static_graphic_ship_rotations
	
	b group_rotations_setup
	
group_setup_ship_rotations
	
	bl setup_ship_rotations

group_rotations_setup

	mov r2,#1
	str r2,shade_style 	

	ldr r0,ship_to_plot
	ldr r1,[r0,#ship_type]		;ship type

	;Is it a bit of a ship
	cmp r1,#512
	
	subge r1,r1,#512
	sublt r1,r1,#256		;ditch the bottom 8 bits used to ref ships

	;which groups of this object don't require plotting
	ldr r2,[r0,#ship_counter]
	str r2,plot_group_mask
		
	ldrlt r0,static_graphics_data		;ship data start
	ldrge r0,ships_data
	add r0,r0,r1, asl #6
	ldr r0,[r0,#graphic_adr]
	
	str r0,graphic_start_adr
	
	adr r0,ship_graphic_rotation
	adr r1,plot_graphic_x_pos
	
	bl rotate_graphic
		
	ldr r0,graphic_start_adr
	
	adr r1,plot_style_list
	ldrb r0,[r0,#32]		;plot style for this graphic
	ldr pc,[r1,r0, asl #2]	;branch to plot routine for this graphic



plot_bit_graphic
	;r0 - address of bit to plot
		;			Byte
		;x_pos		0
		;y_pos		4
		;z_pos		8
		;x_rot		12
		;y_rot		16
		;z_rot		20
		;type		24
		;colour 1	28
		;colour 2	32
		
	stmfd sp!,{r4-r12,r14}
	add r0,r0,#12			;add on header size
	bl plot_bit
	ldmfd sp!,{r4-r12,pc}^

plot_bit_palette
	DCD 3

plot_bit

	stmfd sp!,{r14}
	
	str r0,ship_to_plot

	;Rotation data for light shading
	ldr r1,[r0,#ship_y_rot]
	mov r1,r1, asr #12
	str r1,plot_graphic_y_rot
	ldr r1,[r0,#ship_z_rot]
	mov r1,r1, asr #12
	add r1,r1,#40
	str r1,plot_graphic_z_rot

	ldmia r0,{r0-r2}
	
	adr r3,camera_x_position
	ldmia r3,{r3-r5}
	sub r0,r0,r3
	sub r1,r1,r4
	sub r2,r5,r2
	
	mov r0,r0, asr #12
	mov r1,r1, asr #12
	mov r2,r2, asr #12
	
	bl rotate_node_x_y_z_camera
	
	; Is the bit behind the camera
	cmp r1,#0
	ldmltfd sp!,{pc}
	
	; Is the bit off the left or right
	cmp r0,#0
	rsblt r5,r0,#0
	movge r5,r0
	cmp r5,r1
	ldmgtfd sp!,{pc}	
	
	; Is the bit off the top or bot
	cmp r2,#0
	rsblt r5,r2,#0
	movge r5,r2
	cmp r5,r1
	ldmgtfd sp!,{pc}	
	
	
	adr r3,plot_graphic_x_pos
	
	stmia r3,{r0-r2}
	
	bl setup_ship_rotations
	
	ldr r1,ship_to_plot
	ldr r1,[r1,#bit_type]				;bit type
		
	ldr r0,explosion_bits_data	;bit data start

	add r0,r0,r1, asl #6
	ldr r0,[r0,#graphic_adr]

	str r0,graphic_start_adr
	
	adr r0,ship_graphic_rotation
	adr r1,plot_graphic_x_pos
	
	bl rotate_graphic
		
	ldr r0,graphic_start_adr

	;Set the bits colours
	ldr r1,[r0,#0]			;Adr offset for group 0 link data
	add r1,r1,r0
	;Get the bits colours
	ldr r2,ship_to_plot
	ldr r3,[r2,#bit_colour1]
	str r3,plot_bit_palette
	
	ldr r4,[r2,#bit_colour2]
	;Store the bits colours in the polygon link data
	
	;tim test masked bits thingy
	;ldr r3,[r2,#bit_counter]
	;rsbs r3,r3,#31
	;movmi r3,#0
	;mov r3,r3, asr #2
	;add r3,r3,#192
	;mov r4,r3

	;colour 1
	;strb r3,[r1,#20]
	;add r3,r3,#1
	;strb r3,[r1,#40]
	;add r3,r3,#1
	;strb r3,[r1,#60]

	;colour 2
	;strb r4,[r1,#80]
	;strb r4,[r1,#20]
	;add r4,r4,#1
	;strb r4,[r1,#100]
	;strb r4,[r1,#40]
	;add r4,r4,#1
	;strb r4,[r1,#120]
	;strb r4,[r1,#60]

	mov r1,#2
	str r1,shade_style 

	;Plot all the groups for this graphic
	mov r2,#0
	str r2,plot_group_mask

	adr r1,plot_style_list
	ldrb r0,[r0,#32]		;plot style for this graphic
	ldr pc,[r1,r0, asl #2]	;branch to plot routine for this graphic


static_object_details
	DCD 0,0,0	;un-rotated x,y,z
	DCD 0		;type

static_object_grid_ref
	DCD 0

static_clip_size
	DCD 0

air_to_ground_scan_temp
	DCD 0
air_to_ground_scan
	DCD 0
air_to_ground_x
	DCD 0
air_to_ground_y
	DCD 0
air_to_ground_z
	DCD 0

atg_selected	;If set to 0 then don't bother with highlight of ground graphics
	DCD 0

are_we_in_space_or_wot
	DCD 0

plot_static_from_grid
	;r0 poly map grid ref
		
	mov r10,r0
	str r10,static_object_grid_ref

	and r0,r10,#127			;find x grid pos
	mov r1,r10, asr #7		;find y grid pos
	
	ldr r11,landscape_heights
	ldr r12,poly_map
	
	ldrb r3,[r12,r10]		;get type
	
	ldr r4,static_graphics_data
	ldr r4,[r4,r3, asl #6]
	str r4,static_clip_size
	
	mov r2,r0, asl #1
	add r2,r2,r1, asl #9
	ldrb r2,[r11,r2]		;get height
	subs r2,r2,#17
	movmi r2,#0
	
	cmp r3,#60				;is it a cloud
	addeq r2,r2,#64			;if so extra height
	
	mov r2,r2, lsl #21		;scale up t' height
	
	mov r0,r0, lsl #25		;scale up t' grid pos to
	mov r1,r1, lsl #25		;get x,y pos of object

	stmfd sp!,{r3,r14}

	adrl r4,camera_x_position
	ldmia r4,{r4-r6}
	sub r0,r0,r4
	sub r1,r1,r5
	sub r2,r6,r2
	
	adr r7,static_object_details
	stmia r7,{r0-r3}
		
	mov r0,r0, asr #12
	mov r1,r1, asr #12
	mov r2,r2, asr #12
	
	bl rotate_node_x_y_z_camera

	;check if object is in view area
	
	ldr r5,static_clip_size
	add r5,r1,r5, asr #12
	
	;Is y in front of t' camera
	cmp r5,#0
	ldmltfd sp!,{r3,pc}
	
	;Is unsigned x greater than y
	movs r4,r0
	rsbmi r4,r4,#0
	cmp r4,r5
	ldmgtfd sp!,{r3,pc}
	
	;Is unsigned z greater than y
	movs r4,r2
	rsbmi r4,r4,#0
	cmp r4,r5
	ldmgtfd sp!,{r3,pc}


	ldmfd sp!,{r3,r14}

	; Are we in space with ata missiles selected 
	;- if so ata locks onto all ground objects and atg don't work
	
	ldr r4,are_we_in_space_or_wot
	cmp r4,#0
	beq atg_lock_not_in_space_as_normal
	
	ldr r4,ata_selected
	cmp r4,#0
	
	beq plot_static_graphic
	
	b ata_ground_lock_is_on
	
atg_lock_not_in_space_as_normal

	; Are the air to ground missiles on or wot ?
	ldr r4,atg_selected
	cmp r4,#0
	
	beq plot_static_graphic
	
ata_ground_lock_is_on
	
	; Lets find the best target for the air to ground 
	
	movs r4,r0
	rsbmi r4,r4,#0
	
	; Check that it's a good lock in the horizontal
	cmp r1,r4, asl #2
	blt plot_static_graphic
	
	; Check that it's a good lock in the vertical
	movs r5,r2
	rsbmi r5,r5,#0
	cmp r1,r5, asl #1
	blt plot_static_graphic
	
	ldr r5,air_to_ground_x
	cmp r4,r5
	bgt plot_static_graphic
	
	str r4,air_to_ground_x
	ldr r4,static_object_grid_ref
	str r4,air_to_ground_scan_temp
	
	; Is this the object we wish to highlight or wot ?
	ldr r5,air_to_ground_scan
	cmp r4,r5
	
	bne plot_static_graphic
	
	; Lets highlight this graphic
	stmfd sp!,{r14}
	bl plot_static_graphic

	mov r0,#0				;set flag to plot data for static graphic
	b draw_collision_boxes

	
	
plot_static_graphic
	;r0-r2 x,y,z pos
	;r3 graphic type

	stmfd sp!,{r14}

	;store the position
	adrl r4,plot_graphic_x_pos
	stmia r4,{r0-r2}

	ldr r0,static_graphics_data
	add r0,r0,r3, asl #6	;which graphic
	ldr r0,[r0,#graphic_adr]			;get the adr for this graphics
	
	;store the start address of this graphics data
	str r0,graphic_start_adr
	
	adrl r0,static_graphic_rotation
	adrl r1,plot_graphic_x_pos
		
	bl rotate_graphic
	
	ldr r0,graphic_start_adr
	
	mov r1,#0
	str r1,shade_style
	
	;Plot all the groups for this graphic
	str r1,plot_group_mask

	
	adr r1,plot_style_list
	ldrb r0,[r0,#32]		;plot style for this graphic
	ldr pc,[r1,r0, asl #2]	;branch to plot routine for this graphic


plot_style_list
	DCD plot_graphic_style_0
	DCD plot_graphic_style_1
	DCD plot_graphic_style_2
	DCD plot_graphic_style_3
	DCD plot_graphic_style_4
	DCD plot_graphic_style_5
	DCD plot_graphic_style_6
	DCD plot_graphic_style_7

vector_check
;Checks 3 pairs of x,y screen coords and returns pl or mi
;for clockwise / anticlockwise backface removal
;Input coords r0 - r5 kept intact
;Uses r6-r8
	
	sub r6,r2,r0
	sub r8,r4,r2
	subs r7,r5,r3
	rsbmi r7,r7,#0
	mul r7,r6,r7
	rsbmi r7,r7,#0
	subs r6,r3,r1
	rsbmi r6,r6,#0
	mul r6,r8,r6
	rsbmi r6,r6,#0
	subs r7,r7,r6

	mov pc,r14


plot_graphic_links
	;r0 - pointer to link data
	
	stmfd sp!,{r14}
	
	ldr r10,graphic_screen_coords_1
	ldr r11,[r0],#4	; loop counter
		
	ldr r9,graphic_rotated_coords_1
	add r9,r9,#4

	mov r12,r0		; data pointer
link_loop	
	
	ldrb r6,[r12,#16]
	cmp r6,#256-8
	bge little_fluffy_trees

	ldmia r12,{r5-r8} ; load 4 links
	
	;check to see if facet is in front of camera
	ldr r0,[r9,r5, asl #4]
	cmp r0,#POLY_CLIP_DIST
	blt link_data_behind_camera ;link_data_behind_camera

	ldr r0,[r9,r6, asl #4]
	cmp r0,#POLY_CLIP_DIST
	blt link_data_behind_camera ;link_data_behind_camera

	ldr r0,[r9,r7, asl #4]
	cmp r0,#POLY_CLIP_DIST
	blt link_data_behind_camera ;link_data_behind_camera

	ldr r0,[r9,r8, asl #4]
	cmp r0,#POLY_CLIP_DIST
	blt link_data_behind_camera ;link_data_behind_camera 
	
	;load 1st 3 screen coords and check sign of vector
	add r0,r10,r5, asl #3
	ldmia r0,{r0-r1}
	add r2,r10,r6, asl #3
	ldmia r2,{r2-r3}
	add r4,r10,r7, asl #3
	ldmia r4,{r4-r5}
	
	;r0-r5 1st 3 pairs of screen x,y coords

;Checks 3 pairs of x,y screen coords and returns pl or mi
;for clockwise / anticlockwise backface removal
;Input coords r0 - r5 kept intact
;Uses r6-r8
	
	sub r6,r2,r0
	sub r8,r4,r2
	subs r7,r5,r3
	rsbmi r7,r7,#0
	mul r7,r6,r7
	rsbmi r7,r7,#0
	subs r6,r3,r1
	rsbmi r6,r6,#0
	mul r6,r8,r6
	rsbmi r6,r6,#0
	subs r7,r7,r6

	bmi link_data_vector_fail
				
	;get last screen coord for this facet
	ldr r6,[r12,#12]
	add r6,r10,r6, asl #3
	ldmia r6,{r6-r7}
	
plot_clipped_poly
	
	;is it off the bottom
	cmp r1,#120
	cmpgt r3,#120
	cmpgt r5,#120
	cmpgt r7,#120
	bgt link_data_vector_fail
	
	;is it off the top
	cmn r1,#120
	cmnlt r3,#120
	cmnlt r5,#120
	cmnlt r7,#120
	blt link_data_vector_fail

	;is it off the left
	cmn r0,#160
	cmnlt r2,#160
	cmnlt r4,#160
	cmnlt r6,#160
	blt link_data_vector_fail
	
	;is it off the right
	cmp r0,#160
	cmpgt r2,#160
	cmpgt r4,#160
	cmpgt r6,#160
	bgt link_data_vector_fail

	;store these regs for next poly plot
	stmfd sp!,{r9-r12}

	;store coords in polygon data block
	ldr r8,cel_quad
	add r8,r8,#4
	stmia r8!,{r0-r7}
	
;Static graphics have a fixed shade value where as the
;Ship graphics shade based on their rotation plus constant

	ldr r0,shade_style
	cmp r0,#1
	bge ship_graphic_shader

	;get shade value for static ground objects	
	ldrb r0,[r12,#17]

	sub r0,r0,#17
	mov r0,r0, asl #1
	add r0,r0,#17

	;store shade value
	str r0,[r8,#4]

	;Plot the required polygon
	ldr r0,cel_quad
	ldrb r1,[r12,#16] ;bitmap for polygon
	
	bl arm_addpolycel32
	
	ldmfd sp!,{r9-r12}
	
link_data_vector_fail
	
	subs r11,r11,#1
	
	addpl r12,r12,#5*4	;next link data
	bpl link_loop
	
	ldmfd sp!,{pc}^

link_data_behind_camera

	;check to see if facet is in front of camera
	ldr r0,[r9,r5, asl #4]
	cmp r0,#POLY_CLIP_DIST
	bgt clip_this_polygon

	ldr r0,[r9,r6, asl #4]
	cmp r0,#POLY_CLIP_DIST
	bgt clip_this_polygon

	ldr r0,[r9,r7, asl #4]
	cmp r0,#POLY_CLIP_DIST
	bgt clip_this_polygon

	ldr r0,[r9,r8, asl #4]
	cmp r0,#POLY_CLIP_DIST
	bgt clip_this_polygon 

	b link_data_vector_fail

clip_poly_coords
	DCD 0,0,0
	DCD 0,0,0
	DCD 0,0,0
	DCD 0,0,0

clip_poly_screen_coords
	DCD 0,0
	DCD 0,0
	DCD 0,0
	DCD 0,0

clip_this_polygon
	
	stmfd sp!,{r9-r12}

	ldr r9,graphic_rotated_coords_1
	
	adr r10,clip_poly_coords
	
	add r0,r9,r5, asl #4	;get adr of 3d coords 1
	ldmia r0,{r0-r2}		;load em
	stmia r10!,{r0-r2}		;and stash em
	
	add r0,r9,r6, asl #4	;get adr of 3d coords 2
	ldmia r0,{r0-r2}		;load em
	stmia r10!,{r0-r2}		;and stash em
	
	add r0,r9,r7, asl #4	;get adr of 3d coords 3
	ldmia r0,{r0-r2}		;load em
	stmia r10!,{r0-r2}		;and stash em
	
	add r0,r9,r8, asl #4	;get adr of 3d coords 4
	ldmia r0,{r0-r2}		;load em
	stmia r10!,{r0-r2}		;and stash em
	
	adr r12,clip_poly_coords
	
	ldr r1,[r12,#4]
	cmp r1,#POLY_CLIP_DIST
	bgt clip_dist_ok_1
	
	ldmia r12,{r0-r2}	;get coords for this node
	
	ldr r4,[r12,#4+12]	;which node linked to this one is furthest away
	ldr r5,[r12,#4+36]	
	
	cmp r4,r5
	addgt r12,r12,#12
	addle r12,r12,#36
	
	ldmia r12,{r3-r5}
	bl clip_3d_line
	
	adr r12,clip_poly_coords
	stmia r12,{r0-r2}
	
clip_dist_ok_1
	
	adr r12,clip_poly_coords
	
	ldr r1,[r12,#12+4]
	cmp r1,#POLY_CLIP_DIST
	bgt clip_dist_ok_2
	
	add r11,r12,#12
	ldmia r11,{r0-r2}	;get coords for this node
	
	ldr r4,[r12,#4]	;which node linked to this one is furthest away
	ldr r5,[r12,#4+24]	
	
	cmp r4,r5
	addle r12,r12,#24
	
	ldmia r12,{r3-r5}
	bl clip_3d_line
	
	adr r12,clip_poly_coords+12
	stmia r12,{r0-r2}
	
clip_dist_ok_2
	
	adr r12,clip_poly_coords
	
	ldr r1,[r12,#24+4]
	cmp r1,#POLY_CLIP_DIST
	bgt clip_dist_ok_3
	
	add r11,r12,#24
	ldmia r11,{r0-r2}	;get coords for this node
	
	ldr r4,[r12,#4+12]	;which node linked to this one is furthest away
	ldr r5,[r12,#4+36]	
	
	cmp r4,r5
	addgt r12,r12,#12
	addle r12,r12,#36
	
	ldmia r12,{r3-r5}
	bl clip_3d_line
	
	adr r12,clip_poly_coords+24
	stmia r12,{r0-r2}
	
clip_dist_ok_3
	
	adr r12,clip_poly_coords
	
	ldr r1,[r12,#36+4]
	cmp r1,#POLY_CLIP_DIST
	bgt clip_dist_ok_4
	
	add r11,r12,#36
	ldmia r11,{r0-r2}	;get coords for this node
	
	ldr r4,[r12,#4+24]	;which node linked to this one is furthest away
	ldr r5,[r12,#4]	
	
	cmp r4,r5
	addgt r12,r12,#24
	
	ldmia r12,{r3-r5}
	bl clip_3d_line
	
	adr r12,clip_poly_coords+36
	stmia r12,{r0-r2}
	
clip_dist_ok_4

	adr r11,clip_poly_coords
	adr r12,clip_poly_screen_coords
	
	mov r10,#3	;counter
	
	ldr r9,perspective_table
	
clippy_poly_pex_loop
	
	ldmia r11!,{r0-r2}	;get 3d coords
	
	;get perspective scaler
	bic r1,r1,#31
	ldr r4,[r9,r1, asr #3]
	
	;scale the x,z
	mul r5,r0,r4
	mul r6,r2,r4
	mov r5,r5, asr #16
	mov r6,r6, asr #16
	
	stmia r12!,{r5-r6}
	
	subs r10,r10,#1
	bpl clippy_poly_pex_loop
	
	adr r14,clip_poly_screen_coords
	ldmia r14!,{r0-r5}

	ldmfd sp!,{r9-r12}
	
	;Check vector for clockwise
	sub r6,r2,r0
	sub r8,r4,r2
	subs r7,r5,r3
	rsbmi r7,r7,#0
	mul r7,r6,r7
	rsbmi r7,r7,#0
	subs r6,r3,r1
	rsbmi r6,r6,#0
	mul r6,r8,r6
	rsbmi r6,r6,#0
	subs r7,r7,r6
	
	;Is it a non plot polygon
	bmi link_data_vector_fail

	ldmia r14,{r6-r7}
	b plot_clipped_poly
	
	
clip_3d_line_coords
	DCD 0,0,0
	DCD 0,0,0
	
clip_3d_line
	
	;r0-r2 x,y,z 1
	;r3-r5 x,y,z 2

	stmfd sp!,{r14}

	cmp r1,r4
	blt clip_3d_no_swap
	
	mov r6,r0
	mov r7,r1
	mov r8,r2
	mov r0,r3
	mov r1,r4
	mov r2,r5
	mov r3,r6
	mov r4,r7
	mov r5,r8

clip_3d_no_swap

	adr r6,clip_3d_line_coords
	stmia r6,{r0-r5}
	
	;find require y dist
	sub r0,r4,#POLY_CLIP_DIST
	mov r0,r0, asl #10
	
	;find total dist
	sub r1,r4,r1
	
	bl divide
	
	;r3 - scaler
	
	adr r4,clip_3d_line_coords
	ldmia r4,{r5-r10}
	
	;r5 - x1
	;r6 - y1
	;r7 - z1
	;r8 - x2
	;r9 - y2
	;r10 - z2
	
	;Adjust x pos 1
	sub r5,r8,r5
	mul r11,r5,r3
	sub r5,r8,r11, asr #10
	
	;Adjust z pos 1
	sub r7,r10,r7
	mul r11,r7,r3
	sub r7,r10,r11, asr #10
	
	mov r6,#POLY_CLIP_DIST
	
	stmia r4,{r5-r7}	;store adjusted coords
	
	ldmia r4,{r0-r5}	;get new coords
	
	ldmfd sp!,{pc} ;branch back



ship_graphic_shader

	;0 - 256 (360 degrees)	
	ldr r6,cosine_table	
	add r7,r6,#1024*3		;start of sine table
	
	
	ldr r1,plot_graphic_z_rot	;Ships Z rot (0-256) = 360 degrees
	ldrb r2,[r12,#17]			;Pre calculated Z rot value 256 = 360 deg.
	sub r2,r1,r2				;Find relative rotation diff
	and r2,r2,#255				;Limit it to 360 deg.
	ldr r2,[r6,r2, asl #4]		;Get cosine value of this angle
	
	;r2 - shade value based on facets relative Z rot
	
	;As the facets pre calculated y reaches 90 deg. the Z shader
	;has less effect on the overall shade
	ldrb r3,[r12,#18]			;Facets y rot val.
	ldr r4,[r6,r3, asl #4]		;Cosine of y rot for facet
	cmp r4,#0
	rsblt r4,r4,#0				;Keep value positive
	mul r4,r2,r4				;Scale Z shader by cosine of facet y
	mov r2,r4, asr #12
	
	;Scale the shade based on relative y
	;ldr r4,plot_graphic_y_rot
	;sub r4,r4,r3
	;and r4,r4,#255
	;ldr r4,[r6,r4, asl #4]		;Cosine value of relative y
	;cmp r4,#0
	;rsblt r4,r4,#0
	;mul r5,r2,r4				;Scale shade by cosine of rel. Y
	;rsblt r4,r4,#0
	;rsblt r5,r5,#0
	;mov r2,r5, asr #12
	
	;add r2,r2,r4				;Add cosine rel. Y on shade
	
	;Scale the shade based on cosine y rot
	ldr r4,plot_graphic_y_rot
	ldr r4,[r6,r4, asl #4]		;Cosine value of y
	cmp r4,#0
	rsblt r4,r4,#0
	mul r5,r2,r4				;Scale shade by cosine of rel. Y
	rsblt r5,r5,#0
	mov r2,r5, asr #12

	mov r2,r2, asr #8			;Scale down shade value
	add r2,r2,#17				;Add on base mid shade

	
	;Plot the required polygon
	ldr r0,cel_quad
	ldrb r1,[r12,#16] ;bitmap for polygon
	
	;Is it a window 
	cmp r1,#31
	moveq r2,#4		;if so very transparant
	
	str r2,[r8,#4]				;Store shade val in poly data block
	
	bl arm_addpolycel32
	
	ldr r0,shade_style
	cmp r0,#2
	beq explosion_bit_get_palette
	
	ldmfd sp!,{r9-r12}

	subs r11,r11,#1
	
	addpl r12,r12,#5*4	;next link data
	bpl link_loop
	
	ldmfd sp!,{pc}^

explosion_bit_get_palette
	
	ldr r0,cel_quad
	ldr r1,plot_bit_palette
	
	bl arm_setpolycel32palette
	
	ldmfd sp!,{r9-r12}

	subs r11,r11,#1
	
	addpl r12,r12,#5*4	;next link data
	bpl link_loop
	
	ldmfd sp!,{pc}^
	
shade_style
	DCD 0
	
graphic_link_shade
	DCD 0
		
line_colour
	DCD 0
line_shader
	DCD 0

draw_line

	;Input	r0,r1	x , y start
	;		r2,r3	x , y end
	;		r4		colour

	;is it off the bottom
	cmp r1,#120
	cmpgt r3,#120
	movgt pc,r14
	
	;is it off the top
	cmn r1,#120
	cmnlt r3,#120
	movlt pc,r14

	;is it off the left
	cmn r0,#160
	cmnlt r2,#160
	movlt pc,r14
	
	;is it off the right
	cmp r0,#160
	cmpgt r2,#160
	movgt pc,r14

	;store link reg
	stmfd sp!,{r14}

	str r4,line_colour
	str r5,line_shader

	;find unsigned x n' y distance
	subs r5,r0,r2
	rsbmi r5,r5,#0
	subs r6,r1,r3
	rsbmi r6,r6,#0

	;Set hoz and vert line size
	mov r8,#0
	mov r9,#0
	cmp r5,r6		;is the line horizontal or vertical
	movgt r9,#1		;vert size = 1
	movle r8,#1		;hoz size = 1 

	;Add on hoz n' vert size
	add r4,r2,r8
	add r5,r3,r9
	add r6,r0,r8
	add r7,r1,r9

	;store coords in polygon data block
	ldr r8,cel_quad
	add r8,r8,#4
	stmia r8!,{r0-r7}
	
	ldr r0,line_shader
	;store shade value
	str r0,[r8,#4]

	;Plot the required polygon
	ldr r0,cel_quad
	ldr r2,line_colour				;bitmap for polygon
	mov r3,#2						;set to transparent
	
	bl arm_addmonocel
		
	ldmfd sp!,{pc}^

fluffy_type
	DCD 0

little_fluffy_trees

	;r12 data pointer
	;word 0 link ref for fluffy bit
	;r9 rotated data pointer (+ 4 offset ) (4 word spaced) asl 4
	;r6 type 
	
	; stash these
	stmfd sp!,{r9-r12}

	str r6,fluffy_type

	;get rotated node data
	
	sub r9,r9,#4				;get correct start adr
	
	ldr r0,[r12,#0]				;link ref
	add r0,r9,r0, asl #4		;rot data adr
	ldmia r0,{r0-r2}			;node coords

	cmp r1,#8192
	blt fluffy_behind_camera

	;get the perspective size of fluffy based on y distance
	
	;ldr r4,perspective_table
	adrl r4,perspective_table
	ldr r4,[r4,#0]
	
	bic r1,r1,#31
	ldr r1,[r4,r1, asr #3]

	ldr r6,fluffy_type
	cmp r6,#252

	add r7,r1,r1, asr #1 
	
	movge r6,#3*512
	movge r7,r7, asl #2
	
	movlt r6,#3*1024			;screen corner offset
	movlt r7,r7, asl #3
				
	;r1 - fluffy scaler
	
	sub r0,r0,r6
	sub r2,r2,r6
	
	;top left corner of fluffy r4 - r5
	mul r4,r0,r1
	mov r4,r4, asr #16
	mul r5,r2,r1
	mov r5,r5, asr #16
		
	;store coords in polygon data block
	;ldr r0,cel_quad
	adrl r0,cel_quad
	ldr r0,[r0,#0]
	
	add r6,r0,#4
	stmia r6!,{r4-r5}
	
	;r0 - cel quad
	;r1 - cel type
	;r2 - x scale - 1024 = normal size
	;r3 - y scale 
		
	mov r2,r7	;scale
	mov r3,r7
	
	;fluffy ref start at 58 (58-61 = clouds) (62-65 = trees)
	;Input 248-251 cloud		252-255 tree
	ldr r1,fluffy_type
	
	cmp r1,#252		;wot type of fluffy is it
	movlt r6,#4		;cloud shade
	movge r6,#17	;tree shade
		
	;fluffy shade
	str r6,[r0,#40]
	
	sub r1,r1,#248-FLUFFY_CLOUD
	
	;plot trees fluffy bit
	bl arm_addgamecel
	
fluffy_behind_camera
	
	ldmfd sp!,{r9-r12}
		
	subs r11,r11,#1
	
	addpl r12,r12,#5*4	;next link data
	bpl link_loop
	
	ldmfd sp!,{pc}^

plot_graphic_style_0
	
	ldr r0,graphic_start_adr
	ldr r12,[r0,#28] ; get start adr of vector checks
	add r12,r12,r0
	bl test_these_vectors
	
	ldr r0,graphic_start_adr
	
	tst r11,#2_1
	blne plot_poly_group_1
	tst r11,#2_10
	blne plot_poly_group_2
	
	bl plot_poly_group_0
	
	tst r11,#2_1
	bleq plot_poly_group_1
	tst r11,#2_10
	bleq plot_poly_group_2

	ldmfd sp!,{pc}^

plot_graphic_style_1
	
	ldr r0,graphic_start_adr
	ldr r12,[r0,#28] ; get start adr of vector checks
	add r12,r12,r0
	bl test_these_vectors
	
	ldr r0,graphic_start_adr
	
	tst r11,#2_1000
	blne plot_poly_group_4
	tst r11,#2_10
	blne plot_poly_group_2
	tst r11,#2_10000
	blne plot_poly_group_5
	tst r11,#2_100
	blne plot_poly_group_3
	tst r11,#2_1
	blne plot_poly_group_1
	
	bl plot_poly_group_0
	
	tst r11,#2_1
	bleq plot_poly_group_1
	tst r11,#2_100
	bleq plot_poly_group_3
	tst r11,#2_10
	bleq plot_poly_group_2
	tst r11,#2_1000
	bleq plot_poly_group_4
	tst r11,#2_10000
	bleq plot_poly_group_5
	
	ldmfd sp!,{pc}^
 
plot_graphic_style_2

	ldr r0,graphic_start_adr
 	bl plot_poly_group_0
	
	ldr r0,graphic_start_adr
 	bl plot_poly_group_1
	
	ldmfd sp!,{pc}^
 
plot_graphic_style_3
	
	ldr r0,graphic_start_adr
	ldr r12,[r0,#28] ; get start adr of vector checks
	add r12,r12,r0
	bl test_these_vectors
	
	ldr r0,graphic_start_adr
	
	tst r11,#2_1
	blne plot_poly_group_1
	
	bl plot_poly_group_0
	
	tst r11,#2_1
	bleq plot_poly_group_1
	
	ldmfd sp!,{pc}^
	
plot_graphic_style_4
	
	ldr r0,graphic_start_adr
	ldr r12,[r0,#28] ; get start adr of vector checks
	add r12,r12,r0
	bl test_these_vectors
	
	ldr r0,graphic_start_adr
	
	tst r11,#2_1000
	blne plot_poly_group_4
	tst r11,#2_1
	blne plot_poly_group_1
	tst r11,#2_10
	blne plot_poly_group_2
	tst r11,#2_100
	blne plot_poly_group_3
	
	bl plot_poly_group_0
	
	tst r11,#2_1
	bleq plot_poly_group_1
	tst r11,#2_10
	bleq plot_poly_group_2
	tst r11,#2_100
	bleq plot_poly_group_3
	tst r11,#2_1000
	bleq plot_poly_group_4
	
	ldmfd sp!,{pc}^
	
plot_graphic_style_5
	
	ldr r0,graphic_start_adr
	ldr r12,[r0,#28] ; get start adr of vector checks
	add r12,r12,r0
	bl test_these_vectors
	
	ldr r0,graphic_start_adr
	
	bl plot_poly_group_0
	
	tst r11,#2_100
	blne plot_poly_group_5
	tst r11,#2_1000
	blne plot_poly_group_6
	
	bl plot_poly_group_4
	
	tst r11,#2_100
	bleq plot_poly_group_5
	tst r11,#2_1000
	bleq plot_poly_group_6
	tst r11,#2_1
	bleq plot_poly_group_1
	tst r11,#2_10
	bleq plot_poly_group_2
	
	bl plot_poly_group_3
	
	ldmfd sp!,{pc}^
	
plot_graphic_style_6
	
	ldr r0,graphic_start_adr
	ldr r12,[r0,#28] ; get start adr of vector checks
	add r12,r12,r0
	bl test_these_vectors
	
	ldr r0,graphic_start_adr
	
	tst r11,#2_1
	bleq plot_poly_group_0
	tst r11,#2_10
	bleq plot_poly_group_1
	tst r11,#2_100
	blne plot_poly_group_3
	
	bl plot_poly_group_2
	
	tst r11,#2_10
	blne plot_poly_group_1
	tst r11,#2_100
	bleq plot_poly_group_3
	tst r11,#2_1
	blne plot_poly_group_0
	
	ldmfd sp!,{pc}^

plot_graphic_style_7
	
	ldr r0,graphic_start_adr
	ldr r12,[r0,#28] ; get start adr of vector checks
	add r12,r12,r0
	bl test_these_vectors
	
	ldr r0,graphic_start_adr
	
	tst r11,#2_1000
	blne plot_poly_group_4
	tst r11,#2_100
	blne plot_poly_group_3
	tst r11,#2_10
	blne plot_poly_group_2
	tst r11,#2_1
	blne plot_poly_group_1
	
	bl plot_poly_group_0
	
	tst r11,#2_1
	bleq plot_poly_group_1
	tst r11,#2_10
	bleq plot_poly_group_2
	tst r11,#2_100
	bleq plot_poly_group_3
	tst r11,#2_1000
	bleq plot_poly_group_4
	
	ldmfd sp!,{pc}^


plot_group_mask
	DCD 0
	
plot_poly_group_0
	
	ldr r1,plot_group_mask
	tst r1,#2_1
	movne pc,r14
	
	ldr r1,[r0,#0]	;get start adr for this group
	cmp r1,#0
	moveq pc,r14	;Are ther and polys here
	
	stmfd sp!,{r0,r11,r14}	
	add r0,r0,r1
	bl plot_graphic_links
	ldmfd sp!,{r0,r11,pc}^
	
plot_poly_group_1

	ldr r1,plot_group_mask
	tst r1,#2_10
	movne pc,r14

	ldr r1,[r0,#4]	;get start adr for this group
	cmp r1,#0
	moveq pc,r14	;Are ther and polys here

	stmfd sp!,{r0,r11,r14}
	add r0,r0,r1
	bl plot_graphic_links
	ldmfd sp!,{r0,r11,pc}^
	
plot_poly_group_2

	ldr r1,plot_group_mask
	tst r1,#2_100
	movne pc,r14

	ldr r1,[r0,#8]	;get start adr for this group
	cmp r1,#0
	moveq pc,r14	;Are ther and polys here

	stmfd sp!,{r0,r11,r14}
	add r0,r0,r1
	bl plot_graphic_links
	ldmfd sp!,{r0,r11,pc}^
	
plot_poly_group_3

	ldr r1,plot_group_mask
	tst r1,#2_1000
	movne pc,r14

	ldr r1,[r0,#12]	;get start adr for this group
	cmp r1,#0
	moveq pc,r14	;Are ther and polys here

	stmfd sp!,{r0,r11,r14}
	add r0,r0,r1
	bl plot_graphic_links
	ldmfd sp!,{r0,r11,pc}^
	
plot_poly_group_4

	ldr r1,plot_group_mask
	tst r1,#2_10000
	movne pc,r14

	ldr r1,[r0,#16]	;get start adr for this group
	cmp r1,#0
	moveq pc,r14	;Are ther and polys here

	stmfd sp!,{r0,r11,r14}
	add r0,r0,r1
	bl plot_graphic_links
	ldmfd sp!,{r0,r11,pc}^
	
plot_poly_group_5

	ldr r1,plot_group_mask
	tst r1,#2_100000
	movne pc,r14

	ldr r1,[r0,#20]	;get start adr for this group
	cmp r1,#0
	moveq pc,r14	;Are ther and polys here

	stmfd sp!,{r0,r11,r14}
	add r0,r0,r1
	bl plot_graphic_links
	ldmfd sp!,{r0,r11,pc}^
	
plot_poly_group_6

	ldr r1,plot_group_mask
	tst r1,#2_1000000
	movne pc,r14

	ldr r1,[r0,#24]	;get start adr for this group
	cmp r1,#0
	moveq pc,r14	;Are ther and polys here

	stmfd sp!,{r0,r11,r14}
	add r0,r0,r1
	bl plot_graphic_links
	ldmfd sp!,{r0,r11,pc}^


graphic_rotated_coords_2
	DCD graphic_rotated_coords
graphic_screen_coords_2
	DCD graphic_screen_coords
	
test_these_vectors
	
	;r12 - start of vector data - 	0 = number of vectors -1
	;								4,8,12 - vector link data
	
	stmfd sp!,{r14}
	
	mov r11,#0 ; bits set to true / false if vector fails / passes
	mov r10,#1 ; set which bit ?
	ldr r9,[r12],#4 ; number of check -1 
test_vectors_loop
	ldmia r12!,{r0,r2,r4} ; get 3 link refs
	
	;check that these are in front of the camera
	ldr r14,graphic_rotated_coords_2
	add r14,r14,#4
	ldr r1,[r14,r0, asl #4]
	cmp r1,#1280
	blt vector_check_behind_camera
	ldr r1,[r14,r2, asl #4]
	cmp r1,#1280
	blt vector_check_behind_camera
	ldr r1,[r14,r4, asl #4]
	cmp r1,#1280
	blt vector_check_behind_camera

	ldr r14,graphic_screen_coords_2
	add r0,r14,r0, asl #3
	ldmia r0,{r0-r1}
	add r2,r14,r2, asl #3
	ldmia r2,{r2-r3}
	add r4,r14,r4, asl #3
	ldmia r4,{r4-r5}
	bl vector_check
	
	;set this bit if it's a pass
	orrmi r11,r11,r10

vector_check_behind_camera

	;set the next bit if vector check is true
	mov r10,r10, asl #1

	subs r9,r9,#1
	bpl test_vectors_loop
	
	;returns r11 with bits set for the one that pass
	
	ldmfd sp!,{pc}^
	

	
do_perspective

;All the nodes have been rotated and stored in rotated_coords - now do the perspective
	; r0 rotated_coords [ xxx ] [ 4 ]
	; r1 pex_data [ xxx ] 
	; r2 screen_coords [ xxx ] [ 2 ]
	; r3 perspective loop counter
	

perspective_loop

;load in 3 sets of coords at a time
	ldmia r0,{r4-r6}
	add r0,r0,#16
	ldmia r0,{r7-r9}
	add r0,r0,#16
	ldmia r0,{r10-r12}
	add r0,r0,#16
	
	;r4-r6 1st x,y,z coords rotated 
	;r7-r9 2nd x,y,z 
	;r10-r12 3rd
	;calc perspective screen positions if the coords are in front of camera

;Is the 1st coord in front of the camera - if true calc screen pos
	cmp r5,#0
	ble fail_pex_1 ; Skip code if the point is behind the view point
	bic r5,r5,#31
	ldr r5,[r1,r5, asr #3] ; Load perspective scaler value
	mul r13,r4,r5
	mov r4,r13, asr #16 ; Scale the x
	mul r13,r6,r5
	mov r6,r13, asr #16 ; Scale the y
fail_pex_1

	subs r3,r3,#1
	bmi perspective_end

;2nd coord
	cmp r8,#0
	ble fail_pex_2
	bic r8,r8,#31
	ldr r8,[r1,r8, asr #3]
	mul r13,r7,r8
	mov r7,r13, asr #16
	mul r13,r9,r8
	mov r9,r13, asr #16
fail_pex_2

	subs r3,r3,#1
	bmi perspective_end

;3rd coord
	cmp r11,#0
	ble fail_pex_3
	bic r11,r11,#31
	ldr r11,[r1,r11, asr #3]
	mul r13,r10,r11
	mov r10,r13, asr #16
	mul r13,r12,r11
	mov r12,r13, asr #16
perspective_end
fail_pex_3

;store the screen coords
	stmia r2!,{r4,r6,r7,r9,r10,r12}

	subs r3,r3,#1
	bpl perspective_loop

	mov pc,r14


plot_spinning_ship
	stmfd sp!,{r4-r12,r14}

	mov r0,#1<<27
	sub r0,r0,#1<<25
	sub r0,r0,#1<<25 
	mov r1,#0
	mov r1,#(1<<5)+2	;t big ship like
	str r0,spinning_ship+4		;zoom
	str r1,spinning_ship+24		;type

	ldr r0,spinning_ship+12
	subs r0,r0,#256
	addmi r0,r0,#1024*1024
	str r0,spinning_ship+12
	
	ldr r0,spinning_ship+16
	subs r0,r0,#3*1024
	addmi r0,r0,#1024*1024
	str r0,spinning_ship+16
	
	ldr r0,spinning_ship+20
	subs r0,r0,#7*1024
	addmi r0,r0,#1024*1024
	str r0,spinning_ship+20

	;Reset camera x,y,z position and x,y rotation
	mov r0,#0
	mov r1,#0
	mov r2,#0
	adrl r3,camera_x_position
	stmia r3,{r0-r2}
	adrl r3,camera_x_rotation
	stmia r3,{r0-r2}

	adr r0,spinning_ship

	bl plot_ship
	
	ldmfd sp!,{r4-r12,pc}^

spinning_ship
	DCD 0,0,0,0,0,0,0



collision_box_type
	DCD 0
	
draw_collision_boxes
	
	;r0 	= 0 static ground graphic
	;		= 1 moving ship graphic
	str r0,collision_box_type
	
	cmp r0,#0
	bne draw_ships_collision_boxes
	
	;get the data for the static graphic
	;r0-r2 x,y,z centre point r3 type of graphic
	adrl r4,static_object_details
	ldmia r4,{r0-r3}
	
	;get start address of collision data
	adrl r4,static_graphics_data
	ldr r4,[r4,#0]

	b dont_draw_ships_collision_boxes
	
draw_ships_collision_boxes	

	;for the ships set centre to 0 as the coords are rotated by the ships rotation		
	mov r0,#0
	mov r1,#0
	mov r2,#0

	adrl r3,ship_to_plot
	ldr r3,[r3,#0]			;get adr of ship

	ldr r3,[r3,#ship_type]

	;Check type ? (last minute bug fix)
	cmp r3,#255
	ldmgefd sp!,{pc}

	;get start address of collision data
	adrl r4,ships_data
	ldr r4,[r4,#0]

dont_draw_ships_collision_boxes
	
	mov r0,#0
	mov r1,#0
	mov r2,#0
	
	;get the start of the collision data
	add r4,r4,r3, asl #6
	ldr r4,[r4,#collision_adr]

;Collision data format - r4
;word		byte		use
;0			0			number of boxes -1
;1			4			free
;2			8			free
;3			12			collision box ref	
;4			16			x pos corner 1
;5			20			y pos corner 1
;6			24			z pos corner 1
;7			28			x pos corner 2
;8			32			y pos corner 2
;9			36			z pos corner 2
;-----------------------next collision box
;10			40			collision box ref
;11			44			x pos corner 1
;12			48			y pos corner 2
; etc....................
;28 bytes per collision box

	ldr r5,[r4],#16		;get counter and put pointer to start of data

	;r0 - r2	x,y,z pos
	;r3			type
	;r4			data pointer
	;r5			counter

draw_collision_boxes_loop
	
	ldr r6,graphic_rotated_coords_2

	;Calc 1st coord	
	ldmia r4,{r7-r9}
	add r10,r0,r7
	add r11,r1,r8
	sub r12,r2,r9
	stmia r6,{r10-r12}
	add r6,r6,#16

	;Calc 2nd coord	
	ldr r7,[r4,#12]
	add r10,r0,r7
	stmia r6,{r10-r12}
	add r6,r6,#16
	
	;Calc 3rd coord	
	ldr r8,[r4,#16]
	add r11,r1,r8
	stmia r6,{r10-r12}
	add r6,r6,#16

	;Calc 4th coord	
	ldr r7,[r4,#0]
	add r10,r0,r7
	stmia r6,{r10-r12}
	add r6,r6,#16
	
	;Calc 5th coord	
	add r7,r4,#12
	ldmia r7,{r7-r9}
	add r10,r0,r7
	add r11,r1,r8
	sub r12,r2,r9
	stmia r6,{r10-r12}
	add r6,r6,#16

	;Calc 6th coord	
	ldr r7,[r4,#0]
	add r10,r0,r7
	stmia r6,{r10-r12}
	add r6,r6,#16
	
	;Calc 7th coord	
	ldr r8,[r4,#4]
	add r11,r1,r8
	stmia r6,{r10-r12}
	add r6,r6,#16

	;Calc 8th coord	
	ldr r7,[r4,#12]
	add r10,r0,r7
	stmia r6,{r10-r12}
	
	add r4,r4,#28	;skip over collision data
	
	stmfd sp!,{r0-r5}		;stash these for later
	
	ldr r11,graphic_rotated_coords_2	;pointer to t' data
	mov r12,#7							;counter
draw_collision_boxes_rotation_loop
	
	ldmia r11,{r0-r2}	;get un-rotated coords n' rotate 'em
	mov r0,r0, asr #12
	mov r1,r1, asr #12
	mov r2,r2, asr #12


	ldr r5,collision_box_colour_adder
	add r5,r5,#16
	mul r0,r5,r0
	mul r1,r5,r1
	mul r2,r5,r2
	mov r0,r0, asr #4
	mov r1,r1, asr #4
	mov r2,r2, asr #4
	
	; Is it a static graphic
	ldr r3,collision_box_type
	cmp r3,#0
	bleq rotate_node_x_y_z_camera

	; Is it a ship graphic
	ldr r3,collision_box_type
	cmp r3,#0
	beq dont_rotate_collision_box_by_ship
		
	stmfd sp!,{r11-r12}
	adrl r11,ship_to_plot
	ldr r11,[r11,#0]
	bl rotate_node_by_ship
	ldmfd sp!,{r11-r12}

dont_rotate_collision_box_by_ship

	adrl r3,plot_graphic_x_pos		;add rotated coords onto rotated ships position
	ldmia r3,{r3-r5}
	add r0,r0,r3
	add r1,r1,r4
	add r2,r2,r5
		
	stmia r11,{r0-r2}	;store rotated coords n' inc. pointer
	add r11,r11,#16
	
	subs r12,r12,#1		;check counter n' continue ?
	bpl draw_collision_boxes_rotation_loop
	
	ldr r0,graphic_rotated_coords_2
	adrl r1,perspective_table
	ldr r1,[r1,#0]
	;ldr r1,perspective_table
	ldr r2,graphic_screen_coords_2
	mov r3,#7						;counter
	
	str r13,r13_stash
	bl do_perspective
	ldr r13,r13_stash
	
	mov r9,#11	;counter
	adr r10,draw_collision_box_link_data
	ldr r11,graphic_rotated_coords_2
	ldr r12,graphic_screen_coords_2
	
	add r11,r11,#4		;offset this by 4

draw_collision_box_lines_loop
	
	ldmia r10!,{r7,r8}	;get link data
	
	;Is this line in front of the camera
	ldr r0,[r11,r7, asl #4]
	cmp r0,#2048						;check 1st point
	blt dont_draw_collision_box_line
	
	ldr r0,[r11,r8, asl #4]
	cmp r0,#2048						;check 2nd point
	blt dont_draw_collision_box_line
	
	stmfd sp!,{r9-r12}
	
	add r0,r12,r7, asl #3
	ldmia r0,{r0-r1}
	add r2,r12,r8, asl #3
	ldmia r2,{r2-r3}
	
	;what is the highlight colour
	ldr r4,collision_box_type
	cmp r4,#0
	moveq r4,#cyan		;for ground objects
	movne r4,#green		;for ships
	
	; If in space ground objects are treated as ships ie ata lock works on them
	adrl r5,are_we_in_space_or_wot
	ldr r5,[r5,#0]
	cmp r5,#1
	moveq r4,#green
	
	ldr r5,collision_box_colour_adder		;transparentcy value
	mov r5,r5, asr #1
	add r5,r5,#1
	
	bl draw_line
	
	ldmfd sp!,{r9-r12}
	
dont_draw_collision_box_line

	;cycle the colour adder
	;ldr r5,collision_box_colour_adder
	;subs r5,r5,#1
	;movmi r5,#7
	;str r5,collision_box_colour_adder

	subs r9,r9,#1
	bpl draw_collision_box_lines_loop
		
	ldmfd sp!,{r0-r5}		;get stash back
	
	subs r5,r5,#1			;check counter n' continue ?
	bpl draw_collision_boxes_loop
	
	ldmfd sp!,{pc}

r13_stash
	DCD 0

collision_box_colour_adder
	DCD 0
	
draw_collision_box_link_data
	DCD 0,1
	DCD 1,2
	DCD 2,3
	DCD 3,0

	DCD 0,6
	DCD 1,7
	DCD 2,4
	DCD 3,5

	DCD 4,5
	DCD 5,6
	DCD 6,7
	DCD 7,4


	END
