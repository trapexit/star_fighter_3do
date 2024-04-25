//	File : SF_ARMLink (Header file for linked list routines)

#ifndef __SF_ARMLINK
#define __SF_ARMLINK

// Includes

#include "SF_Allocation.h"

// Structure definitions for general linked list routine

typedef struct 	linked_list
				{
				void*	start_address;
				void*	next_freeaddress;
				long	sortword;
				} linked_list;
				
typedef struct	link_header
				{
				void*	prev_address;
				void*	next_address;
				long	status;
				} link_header;

// Structure definitions for ZSort linked list routine

typedef struct	zsort_item
				{
				long	distance;
				void*	graphic_address;
				long	ref;
				void*	next_address;
				} zsort_item;
				
typedef struct	zsort_list
				{
				void*		offset [128];
				zsort_item	graphic [MAX_GRAPHICPLOT];
				} zsort_list;

// Function Prototypes

extern	void	armlink_initialise (long, void*, long, long);	// Setup an empty linked list
extern	void*	armlink_addtolist (void*);						// Add an item to the list
extern	void*	armlink_sorttolist (void*, long);				// Add an item to the list, sorted
extern	void	armlink_resortlist (void*);						// Resort a list into ascending order
extern	void	armlink_deleteitem (void*, void*);				// Delete an item from a list
extern	void	armlink_suspenditem (void*, void*);				// Suspend an item from the linked list
extern	void	armlink_releaseitem (void*, void*);				// Release an item from suspension
extern	void	armzsort_create (void*, long);					// Create a list
extern	void	armzsort_initialise (void);						// Initialise zsort list a frame start
extern	void	armzsort_add (long, void*, long);				// Add a graphic to the list
extern	void*	armcol_collisioncheck (void*, void*, void*);	// Collision Check
#endif
