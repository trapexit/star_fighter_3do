		EXPORT	armlink_initialise,armlink_addtolist,armlink_sorttolist, armlink_resortlist
		EXPORT	armlink_deleteitem, armlink_suspenditem, armlink_releaseitem, armzsort_create
		EXPORT	armzsort_initialise, armzsort_add, armcol_collisioncheck
		
		AREA	|c$$code|,CODE,READONLY

; ---------------------------------------------------------------------------------------------------

; I hate linked lists

; This library contains the 3 main sets of routines :

; A) armlink_****	- Main routines for managing linked lists throughout the game
; B) armzsort_****	- For fast building of discrete linked lists (Used for Zsorting - scrapped each frame)
; C) armcol_****	- Collision checking (lasers v ships)

; ---------------------------------------------------------------------------------------------------
; armlink_initialise -

; This routine accepts a pointer to a block of memory, the max number of items that can be
; stored in that block, the size of a data block, and the offset [in it] to a word that can
; be used for sorting. The block of memory is then completely pre-initialised with an empty
; linked list with all the pointers correctly set up, ready for use
						
; ----------
; armlink_addtolist -

; Used to add an item to the linked list. Calling this routine with R0 pointing to the start
; address of the list will either :	a) Return an address where the new data can be stored, OR
;									b) Return ZERO if there is no space in the list.

; ----------
; armlink_sorttolist -

; This routine is similar to 'armlink_addtolist' but will allocate a space in the list
; depending on a sort value passed in R1, The routine checks this sort value against all other
; items in the list using the 'word location of sorting value' which was passed in R3 when the list
; was set up.

; Values are always sorted in Ascending order (ie. Lowest first, Highest last)
; Returns same values as armlink_addtolist

; ----------
; armlink_resortlist -

; This routine only needs to be called if	a) The list NEEDS to be sorted, and
;											b) items were added 'unsorted', ie. using arm_addtolist
;											   instead of arm_sorttolist
; Pass the address if the list in R0, 

; ----------
; armlink_deleteitem -

; Deletes an item from the linked list.		r0 contains the start address of the item to delete,
; 											r1 contains the start address of the linked list

; Internal pointers are adjusted to remove the item from the list

; ----------
; armlink_suspenditem -

; Takes an item out of the linked list WITHOUT freeing the memory it uses (Useful for temporarily
; taking out the players ship etc).			r0 contains the start address of the item to delete,
; 											r1 contains the start address of the linked list

; ----------
; armlink_restoreitem -

; Restores an item to the START of the linked list (previously suspended)
; NOTE : THIS IS NOT QUICK - IT WILL NEED TO BE RESORTED FROM THE START OF THE LIST
;											r0 contains the start address of the item to delete,
; 											r1 contains the start address of the linked list

; ----------
; armzsort_initialise -

; Creates and sets up the zsort list.		r0 contains the address of the linklist data store
;											r1 contains the max ammount of plotting items

; ----------
; armzsort_create - 

; Creates and sets up the zsort list.		r0 contains the address of the linklist data store
;											r1 contains the max ammount of plotting items

; ----------
; armzsort_initialise -

; Initialises the linked list for a new frame of plotting data

; ----------
; armzsort_add -

; Adds a graphic item to the list.			r0 contains the Z Camera Distance
;											r1 contains the address of the graphic
;											r2 contains the graphic ref#

; ----------
; armcol_collisioncheck -

; Searches for possible collisions			r0 contains the laser v ship collision buffer
; (Ships v Ships , Lasers v Ships)			r1 contains pointer to laser list
;											r2 contains pointer to ship list


;****************************************************************************************************
;*										LINKED LIST ROUTINES										*
;****************************************************************************************************


;*******************************
armlink_initialise			ROUT
;*******************************

; Call this to initialise the linked list.

; ----------

; Receives :	r0 = Max number of items to store
;				r1 = Start address of linked list
;				r2 = Size of linked list data block (INCLUDING 3 header words)
;				r3 = Word location of sorting value (EXCLUDING 3 header words)

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

; list_startaddress is				[r1 +0]
; list_nextaddressavailable is		[r1 +4]
; list starts at					[r1 +8]

	subs r0,r0,#1			; Take 1 off no. of items for loop
	movmis pc,r14			; No items to initialise ! Return
	
	stmfd sp!,{r4}
	
	add r1,r1,#12			; move start of list up by 12 for initial pointers
	add r3,r3,#12			; move sorting word up by 12 for initial pointers
	
	str r1,[r1,#-12]		; Set list_startaddress
	str r1,[r1,#-8]			; Set list_nextaddressavailable
	str r3,[r1,#-4]			; Store word location of sorting value.

	mov r4,#0
	str r4,[r1]
	str r4,[r1,#8]
	add r3,r1,r2
	str r3,[r1,#4]

; ----------

armlink_initialiseloop
	subs r0,r0,#1
	bmi armlink_leaveinitialiseloop

	str r1,[r3,#0]
	add r1,r1,r2
	str r4,[r1,#8]
	add r3,r1,r2
	str r3,[r1,#4]
	b armlink_initialiseloop

; ----------

armlink_leaveinitialiseloop
	str r4,[r1,#4]
	mov r4,#2
	str r4,[r1,#8]
	ldmfd sp!,{r4}
	movs pc,r14


;*******************************
armlink_addtolist			ROUT
;*******************************

; Call this to add an item to the list

; ----------

; Receives :	r0 = Start address of linked list

; Returns :		r0 = Address to store data OR zero if no space

; Corrupts : 	r0,r1,r2

; ----------

	ldr r1,[r0,#4]			; get item_nextaddressavailable
	ldr r2,[r1,#8]			; is it at the end of the list ? - IF SO, DO NOT ADD ANYTHING, RETURN 0
	cmp r2,#2

	movne r2,#1
	strne r2,[r1,#8]		; Set it's status to active

	ldrne r2,[r1,#4]		; Make next point to next in list
	strne r2,[r0,#4]

	movne r0,r1				; Put start address of structure in r0 for return
	moveq r0,#0
	movs pc,r14


;*******************************
armlink_sorttolist			ROUT
;*******************************

; Call this to add an item to the list, with its position SORTED, depending on the value
; of a word specifying where that word can be found in other already stored items for comparison.

; ----------

; Receives :	r0 = Start address of list
; 				r1 = Word sort value of this item

; Returns :		r0 = Address to store data OR zero if no space

; Corrupts : 	r0,r1,r2,r3

; ----------

	stmfd sp!,{r4-r7,r14}
	
	ldr r2,[r0,#8]			; get word location of sort value.
	mov r7,r0				; Store start of linked list

	ldr r3,[r0,#4]			; get item_nextaddressavailable
	ldr r4,[r3,#8]			; is it at the end of the list ?
	cmp r4,#2
	moveq r0,#0
	ldmeqfd sp!,{r4-r7,pc}^	; We have no room for next thing

	mov r4,#1
	str r4,[r3,#8]			; Set this to active

	ldmia r3,{r5,r6}		; Get nextaddresses old previous (R5)
							; Make next point to next in list (R6)
	str r6,[r0,#4]

; Now, find the item which has the next lowest (or equivalent) value

	mov r0,r3

	ldr r0,[r0,#0]			; get the previous one if not !
	cmp r0,#0				; Are we at start of linked list ?
	ldrne r4,[r0,r2]		; get its value
	moveq r4,#&80000000

	cmp r4,r1				; Is it lower (or equal) ?
	movle r0,r3				; Yes, Put start address in r0
	ldmlefd sp!,{r4-r7,pc}^	; Yes, Return with address		

; ----------

armlink_findnextlowest		; Now we are searching back ...

	ldr r0,[r0,#0]			; get one previous if not !
	cmp r0,#0				; Are we at start of linked list ?
	ldrne r4,[r0,r2]		; get its value
	moveq r4,#&80000000

	cmp r4,r1					; Is it lower (or equal) to our item ?
	bgt armlink_findnextlowest	; No, find next lowest

; Make it point to ours (only if its not the pre - first in the list)

	cmp r0,#0						; Was it pre-start of list ?
	beq armlink_sortitemprelist		; YES !

	str r0,[r3,#0]			; Store its pointer in our previous

	ldr r1,[r0,#4]			; its next pointer
	str r3,[r0,#4]			; Store our pointer in its next
	str r1,[r3,#4]			; Store its next in our next
	str r3,[r1,#0]			; Store our pointer in old nexts previous
	str r6,[r5,#4]			; Make nextaddresses old previous point to new
	str r5,[r6,#0]			; Make nextaddresses previous old previous

	mov r0,r3				; Put start address in r0
	ldmfd sp!,{r4-r7,pc}^	; Return with address		

; ----------

armlink_sortitemprelist

	ldmia r3,{r2, r4}
	str r2,[r4,#0]
	str r4,[r2,#4]

	str r0,[r3,#0]			; Store its pointer in our previous

	ldr r1,[r7]

	str r3,[r7]				; Make this new start of list
	str r3,[r1,#0]			; Make old starts previous point to ours
	str r1,[r3,#4]			; Make our next point to it
	
	mov r0,r3				; Put start address in r0
	ldmfd sp!,{r4-r7,pc}^	; Return with address		


;*******************************
armlink_resortlist			ROUT
;*******************************

; Use this routine to re-sort a list after altering Items.
							
; ----------

; Receives :	r0 = Start address of list

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	stmfd r13!,{r4-r8,r14}
	
	mov r7,r0

	ldr r1,[r0,#8]			; get word loc of sort value
	ldr r0,[r0,#0]			; get start location of list data
	mov r3,#&80000000		; Max lowest value

; ----------

armlink_findnextvalue

	ldr r5,[r0,#8]			; Status ?
	cmp r5,#1
	ldmnefd sp!,{r4-r8,pc}^

	ldr r2,[r0,r1]			; get sort value
	cmp r2,r3				; Is it higher or equal ?
	movgt r3,r2
	ldrge r0,[r0,#4]		; get next and loop back if so.
	bge armlink_findnextvalue

; ----------

	; Now we need to swap an out of order value
	; Should we swap it just with the one previous to it ?

	ldr r5,[r0,#0]			; Previous
	ldr r4,[r5,#0]			; Previous << 2

	cmp r4,#0				; Are we back past start of list ?
	ldrne r8,[r4,r1]		; get its value (if not start of list)
	moveq r8,#&80000000		; If first in list set to max lowest

	cmp r2,r8
	blt armlink_searchback

; ----------

	; RIGHT - We are just swapping the current with its previous,

	; r5 Points to the previous one (WITH THE NEXT LOWEST VALUE)
	; r4 Points to its previous
	; r0 Points to the current one

	ldr r8,[r0,#4]			; Current items next pointer

	stmia r0,{r4,r5}		; Make current (next) point to previous
							; Make current (prev) point to prev previous

	stmia r5,{r0,r8}		; Make previous (prev) point to current
							; Make previous (next) point to currents old next

	cmp r4,#0				; Was prev previous past start of list ?

	strne r0,[r4,#4]		; No - Make prev previous point to current (in its next)
	streq r0,[r7,#0]		; Yes - Make this the new start of the list

	str r5,[r8,#0]			; link to previous (prev)

	mov r3,r2
	b armlink_findnextvalue	; Swap performed, return to sort

; ----------

armlink_searchback

	mov r5,r4

	ldr r4,[r5,#0]			; get previous
	cmp r4,#0
	ldrne r8,[r4,r1]		; get value
	moveq r8,#&80000000

	cmp r2,r8
	blt armlink_searchback

	; r5 Points to the previous one (WITH THE NEXT LOWEST VALUE)
	; r4 Points to its previous
	; r0 Points to the current one

	ldmia r0,{r6,r8}		; Current items prev pointer (R6)
							; Current items next pointer (R8)

	stmia r0,{r4,r5}		; Make current (next) point to previous (R4)
							; Make current (prev) point to prev previous (R5)

	str r0,[r5,#0]			; Make previous (prev) point to current
	str r8,[r6,#4]			; Make direct previous (next) point to currents old next

	cmp r4,#0				; Was prev previous past start of list ?

	strne r0,[r4,#4]		; No - Make prev previous point to current (in its next)
	streq r0,[r7,#0]		; Yes - Make this the new start of the list

	str r6,[r8,#0] 			; link to previous (prev)

	mov r3,r2
	b armlink_findnextvalue	; Swap performed, return to sort


;*******************************
armlink_deleteitem			ROUT
;*******************************

; Deletes an item from the list

; ----------

; Receives :	r0 Address of the items data block
; 				r1 Address of the linked list

; Returns :		Nothing

; Corrupts : 	r1,r2,r3,r4

; ----------

; Make its previous skip over it, and its next point back to its previous.

	stmfd sp!,{r4}
	
	ldr r3,[r1]
	cmp r0,r3				; Is it at the start of the list ?

	ldmia r0,{r2,r4}		; get current ones previous (R2)
							; get current ones next (R4)

	strne r4,[r2,#4]		; Make currents previous skip current
	strne r2,[r4,#0]		; Make nexts previous previous to current

	streq r2,[r0,#0]
	ldreq r3,[r4,#8]		; Is its next one active ?
	cmpeq r3,#1				; Only move start address on if it is

	streq r2,[r4,#0]
	streq r4,[r1,#0]		; list_startaddress

; Put empty space before nextaddressavailable

	ldr r3,[r1,#4]			; list_nextaddressavailable
	str r0,[r1,#4]			; list_nextaddressavailable

	ldr r2,[r3,#0]			; get old next address's previous pointer
	str r0,[r2,#4]			; Make it point to this one
	stmia r0,{r2,r3}		; Make new next address's previous point to it
							; Make new next address's next point to old next adr
	str r0,[r3,#0]			; Make old next address's previous new next adr

	mov r3,#0
	str r3,[r0,#8]			; Set its status to zero

	ldr r1,[r1,#0]
	str r3,[r1,#0]

	ldmfd sp!,{r4}
	movs pc,r14
	
	
;*******************************
armlink_suspenditem			ROUT
;*******************************

; Deletes an item from the list (WITHOUT RELEASING IT'S MEMORY ADDRESS TO THE LIST - SO IT CAN BE PUT BACK IN LATER)

; ----------

; Receives :	r0 Address of the items data block
; 				r1 Address of the linked list

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	ldmia r0,{r2,r3}
	str r2,[r3,#0]			; Make next item's (previous pointer) point to previous item - OR ZERO If this was 1st in list
	
	cmp r2,#0
	strne r3,[r2,#4]		; If there is a previous item (ie. not zero) make it point to next
	streq r3,[r1,#0]		; ELSE make the next item the start of the list
	movs pc,r14
	

;*******************************
armlink_releaseitem			ROUT
;*******************************

; Restores an item back to the start of the list (Must have been suspended with armlink_suspenditem)
; NOTE : This is NOT QUICK (It will have to be re-sorted to god knows where when armlink_resortlist is next called)

; ----------

; Receives :	r0 Address of the items data block
; 				r1 Address of the linked list

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

; Make the nextaddressavailable pointer point directly to the item just deleted

	ldr r3,[r1,#0]			; Get start of linked list
	str r0,[r1,#0]			; Make start of linked list point to our ship
	
	mov r2,#0
	stmia r0,{r2,r3}		; Make our (previous pointer) zero (ie. start of list) (R2)
							; Make our (next pointer) point to the old start of the list
	str r0,[r3,#0]			; Make old start of list item's (previous pointer) point to our item
	movs pc,r14				; Return

;****************************************************************************************************
;*										ARMZSORT ROUTINES											*
;****************************************************************************************************

; This routine uses an indirection table, and is written for making fast sorted entries to multiple
; linked lists which are scrapped at the start of every frame (used to sort objects into a plotting order)

;*******************************
armzsort_create				ROUT
;*******************************

; Call this to create the list

; ----------

; Receives :	r0 = Address of linked list
;				r1 = Max number of graphics allowed in datatable
				
; Returns :		Nothing

; Corrupts : 	r0,r1,r2

; ----------

	str r0,armzsort_listaddress		; Store start address of linked list
	str r1,armzsort_maxreset		; Store max number allowed
	
	mov r1,#0
	mov r2,#128
	
armzsort_createloop					; Clear the indirection list

	subs r2,r2,#1
	movmis pc,r14
	
	str r1,[r0,r2,asl#2]
	b armzsort_createloop

;*******************************
armzsort_initialise			ROUT
;*******************************

; Call this to reset the list, ready for adding things to it

; ----------

; Receives :	Nothing

; Returns :		Nothing

; Corrupts : 	r0

; ----------

	ldr r0,armzsort_listaddress
	add r0,r0,#128 << 2
	str r0,armzsort_nextaddress
	
	ldr r0,armzsort_maxreset
	str r0,armzsort_count

	movs pc,r14
	
	
;*******************************
armzsort_add				ROUT
;*******************************

; Call this to add an item to the list

; ----------

; Receives :	r0 - Z Sort Value (bit 31 should NOT be set, otherwise it's bad news bears)	- DW 0
;				r1 - Address of graphic														- DW 1
;				r2 - Ref#																	- DW 2

; Returns :		Nothing

; Corrupts : 	r0,r1,r2,r3

; ----------

	ldr r3,armzsort_count			; Is their any room left in the table ?
	subs r3,r3,#1
	movmis pc,r14					; No, quit
	
	str r3,armzsort_count			; Yes, add it
	stmfd sp!,{r4-r6}
	
	mov r6,r0, asr#24				; Index offset
	
	ldr r3, armzsort_listaddress
	ldr r4,[r3, r6, asl#2]			; Get address from lookup table
	cmp r4,#0						; Is an entry registered in the table ?
	beq armzsort_addempty			; No, just put it straight in
	
	ldr r5,[r4, #0]					; Get ZSort value of the 1st item in group list
	cmp r0,r5						; Is our new one higher or equal to it ?
	bge	armzsort_addprior			; Yes, add it straight in and leave

; -----------

armzsort_loop

	mov r6,r4						; Put this entry address in R6
	ldr r4,[r4, #12]				; Get next entry in R4
	cmp r4,#0						; End of the list ?
	beq armzsort_addlast			; Yes, insert at end of list
	
	ldr r5,[r4, #0]					; Get ZSort value of the this item in group list
	cmp r0,r5						; Is our new one higher or equal to it ?
	blt armzsort_loop				; No, carry on searching
	
; ----------

	ldr r5, [r6,#12]				; Last item's NEXT pointer
	
	ldr r4,armzsort_nextaddress		; Get next free slot
	str r4, [r6,#12]				; Make previous entry point to it

	stmia r4!,{r0-r2, r5}			; Add entry to linked list, with POINTER TO NEXT ITEM
	str r4,armzsort_nextaddress		; Store new next address

	ldmfd sp!,{r4-r6}				; Leave
	movs pc,r14

; ----------

armzsort_addempty

	ldr r4,armzsort_nextaddress		; Get next free slot
	str r4,[r3, r6, asl#2]			; Register entry
	
	mov r3,#0
	stmia r4!,{r0-r3}				; Add entry to linked list, with ZERO TERMINATOR
	str r4,armzsort_nextaddress		; Store new next address

	ldmfd sp!,{r4-r6}				; Leave
	movs pc,r14
	
; ----------

armzsort_addprior

	ldr r5,armzsort_nextaddress		; Get next free slot
	str r5,[r3, r6, asl#2]			; Register entry
	
	stmia r5!,{r0-r2, r4}			; Add entry to linked list, with POINTER TO NEXT ITEM
	str r5,armzsort_nextaddress		; Store new next address

	ldmfd sp!,{r4-r6}				; Leave
	movs pc,r14

; ----------

armzsort_addlast

	ldr r4,armzsort_nextaddress		; Get next free slot
	str r4,[r6, #12]				; Make previous entry point to it
	
	mov r3,#0
	stmia r4!,{r0-r3}				; Add entry to linked list, with ZERO TERMINATOR
	str r4,armzsort_nextaddress		; Store new next address

	ldmfd sp!,{r4-r6}				; Leave
	movs pc,r14
	
; ----------

armzsort_listaddress
	DCD	0
	
armzsort_nextaddress
	DCD 0

armzsort_maxreset
	DCD 0
	
armzsort_count
	DCD	0

;*******************************
armcol_collisioncheck		ROUT
;*******************************

; Call this to generate list of all possible collisions between lasers&ships and ships&ships
; Version 2000000000004783.b

; Just a quick note to anyone who ever looks at this code - If your bad and go to hell when you die,
; this piece of code will be waiting for you when you get there ...

; The basic idea is that two pointers waddle through the X Sorted linked lists (ship & laser) leading each
; other on, only checking for the minimum of possible collisions. It saves checking every item against every
; other item and seems to be pretty good idea, but then again, they said that about the channel tunnel.

; Ship v Ship collisions are ignored if :	a) Ship A is OWNED by Ship B
;											b) Ship B is OWNED by Ship A
;											c) Ship A ARE Ship B are BOTH OWNED by the same ship
; Laser v Ship collisions are ignored if :	a) Laser is OWNED by the ship
;											b) Laser is OWNED by the ship's owner

; These checks prevent missiles shooting themselves down, players shooting themselves, wing pods blowing
; themselves (or the player) up etc.
; Collisions between ships are put in the individual ships data block, laser v ship collisions are entered
; into a list [LASER,SHIP] [LASER,SHIP] (the address being provided in r0 when the routine is called)

; ----------

; Receives :	r0 - Address of possible collision list for laser v ship
;				r1 - Address of laser list
;				r2 - Address of ship list

				
; Returns :		r0 - Next update address in collision list

; Corrupts : 	r0,r1,r2,r3

; ----------

col_linkhdsz				EQU 12					; Size of linkdata headers

collision_shipx				EQU	col_linkhdsz + 0	; Offsets in ship block
collision_shipy				EQU	col_linkhdsz + 4
collision_shipz				EQU	col_linkhdsz + 8
collision_shipcatch			EQU col_linkhdsz + 28
collision_shipwho_ownsme	EQU col_linkhdsz + 32
collision_shipwhat_hitme	EQU	col_linkhdsz + 36

collision_laserx			EQU col_linkhdsz + 0	; Offsets in laser block
collision_lasery			EQU col_linkhdsz + 4
collision_laserz			EQU col_linkhdsz + 8
collision_laserowner		EQU col_linkhdsz + 32

; ----------

	stmfd sp!,{r4 - r12, r14}
		
	ldr r1,[r1]							; Get 1st laser
	ldr r2,[r2]  						; Get 1st ship

; ----------

armcol_newship							; CHECK THIS SHIP IS ACTIVE, IF NOT, LEAVE
										; OTHERWISE, GET ITS DETAILS
	ldr r12,[r2,#8]
	cmp r12,#1
	ldmnefd sp!,{r4 - r12, pc}^			; Quit

	ldr r11,[r2,#4]						; Get next ship in list in R11
	
	add r3,r2,#collision_shipx			; Get ships X,Y,Z co-ordinates in r3,r4,r5
	ldmia r3,{r3,r4,r5}
	ldr r6,[r2,#collision_shipcatch]	; Get catchment area of ship in r6

; ----------

armcol_newshipcheck						; IS NEXT SHIP ACTIVE ? IF SO, CHECK AGAINST THIS ONE

	ldr r12,[r11,#8]       				; Is it active ?, if not, leave this loop and check
	cmp r12,#1							; against lasers for a bit
	bne armcol_newlaser
										; YES IT IS ACTIVE, CHECK IT AGAINST ORIGINAL SHIP
										
	ldr r10,[r11,#collision_shipcatch] 	; Get catchment area for next ship in r10
	add r10,r10,r6        				; Total Catchment area to check for

	ldr r12,[r11,#collision_shipx]   	; CHECK X DISTANCE
	
	subs r12,r12,r3						; Calculate X distance
	rsbmi r12,r12,#0					; +ve dist between ship X centres
	cmp r12,r10							; Is it within catchment area ?
	bgt armcol_newlaser					; No, leave the loop & check lasers instead

	ldr r12,[r11,#collision_shipy]   	; CHECK Y DISTANCE
	
	subs r12,r12,r4						; Calculate Y distance
	rsbmi r12,r12,#0					; +ve dist between ship Y centres
	cmp r12,r10							; Is it within catchment area ?
	ldrgt r11,[r11,#4]
	bgt armcol_newshipcheck				; No, check against next ship

	ldr r12,[r11,#collision_shipz]   	; CHECK Z DISTANCE
	
	subs r12,r12,r5						; Calculate Z distance
	rsbmi r12,r12,#0					; +ve dist between ship Z centres
	cmp r12,r10							; Is it within catchment area ?
	ldrgt r11,[r11,#4]
	bgt armcol_newshipcheck				; No, check against next ship

; ----------								; REGISTER A COLLISION BETWEEN TWO SHIPS - ONLY IF ONE IS NOT OWNED BY THE OTHER

	ldr r10, [r11,#collision_shipwho_ownsme]	; Get the owner of our ship
	ldr r12, [r2,#collision_shipwho_ownsme]		; Get the owner of other ship
	cmp r10,r12									; Do both have the same owner ?
	cmpne r10,r2								; NO - Is the owner of the 1st the second ship ?
	cmpne r12,r11								; NO - Is the owner of the 2ns the first ship ?
		
	strne r2,[r11,#collision_shipwhat_hitme]	; NO - Store this ships ID in nexts
	strne r11,[r2,#collision_shipwhat_hitme]	; NO - Store next ships ID in this

	ldr r11,[r11,#4]							; Get next next ship
	b armcol_newshipcheck

; ----------

armcol_newlaserrestorenext

	mov r3,r7							; Restore Current ships X
	mov r4,r8							; Restore Current ships Y
	mov r5,r9							; Restore Current ships Z
	mov r6,r14							; Restore Current ships (Original) colsize
	ldr r1,[r1,#4]						; Get next laser

; ----------

armcol_newlaser

	ldr r12,[r1,#8]						; Is this laser active ? (ie. eol ?)
	cmp r12,#1
	ldrne r2,[r2,#4]
	bne armcol_newship					; No, carry on with ship search
	
; ----------

	ldr r11,[r1,#collision_laserx]		; CHECK X POSITION OF THIS LASER
	
	subs r12, r3, r11					; distance between laser & ship
	rsbmi r12, r12, #0
	
	cmp r12, r6                         ; Is it within collision box ?
	ble armcol_laserwithin				; Yes - look at it
	
	cmp r11, r3							; No - is the laser too far right (ie. get next ship ?)
	ldrgt r2, [r2, #4]
	bgt armcol_newship					;		Yes - get next ship
	
	ldrlt r1, [r1, #4]
	blt armcol_newlaser					; 		No  - laser is too far left, get next laser
	
armcol_laserwithin
	
; ----------							; LASER IS WITHIN X CATCHMENT OF SHIP, CHECK IT OUT

	mov r10,r2							; Put ships pointer in r10
	mov r7,r3							; Preserve Current ships X
	mov r8,r4							; Preserve Current ships Y
	mov r9,r5							; Preserve Current ships Z
	mov r14,r6							; Preserve Current ships (Original) colsize

; ----------

armcol_checknext

; Check this laser against ship (X)

	subs r12,r11,r3						; Get X dist of laser from ship
	rsbmi r12,r12,#0
	cmp r12,r6							; Is it within X catchment area ?
	bgt armcol_newlaserrestorenext		; Laser is outside ships X catchment Area, return.
	
; Check this laser against ship (Y)

	ldr r12,[r1,#collision_lasery]		; Y position of laser
	subs r12,r12,r4						; Get Y dist of laser from ship
	rsbmi r12,r12,#0
	cmp r12,r6							; Is it within Y catchment area ?
	bgt armcol_next						; Laser is outside ships Y catchment area, next
	
; Check this laser against ship (Z)

	ldr r12,[r1,#collision_laserz]		; Z position of laser
	subs r12,r12,r5						; get Z dist of laser from ship
	rsbmi r12,r12,#0
	cmp r12,r6							; Is it within Z catchment area ?
	bgt armcol_next						; Laser is outside ships Z catchment area, next

; Register the laser v ship collision - only if the laser is NOT owned by this ship

	ldr r12,[r1, #collision_laserowner]			; Get the owner of the laser
	cmp r12,#16384								; Is it a ground object ?
	ble armcol_registercollision				; YES !
	
	cmp r10, r12								; Is the owner of the laser, the ship that's been hit ? 
	beq armcol_next								; YES !

	ldr r6, [r10, #collision_shipwho_ownsme]	; Is Laser owner & ship owner same (ie. Wingpod hit by player ?)
	cmp r6, r12
	beq armcol_next
	
	ldr r12, [r12, #collision_shipwho_ownsme]	; Get owner of ship firing
	cmp r10, r12								; Are they the same ? 
	beq armcol_next								; YES !
	
armcol_registercollision

	stmia r0!,{r1,r10}							; Register a collision between a laser & ship
	
; ----------

armcol_next

	ldr r10,[r10,#4]					; Get next ship
	ldr r12,[r10,#8]
	cmp r12,#1							; Are we out of ships ?
	bne armcol_newlaserrestorenext		; Yes, Branch back and get next next laser.

; ----------

	ldr r6,[r10,#collision_shipcatch]	; Catchment area of ship

	add r3,r10,#collision_shipx			; Get next ships X,Y,Z co-ordinates in r3,r4,r5
	ldmia r3,{r3,r4,r5}
	b armcol_checknext					; Branch back and try next ship.

; ----------

armcol_r14								; Return Address
	DCD	0
	
	END

