/*
 * stack.h
 *
 * Interface for a simple, dynamically allocated stack of ints
 * 
 * Users of this data structure are expected to keep a pointer to the
 * entry currently at the top of the stack, and pass it into stack functions.
 * 
 * A null top pointer is taken to mean the stack is empty,
 *
 * Written by Paul Bonamy - 21 February 2018
 */

#ifndef STACK_H
#define STACK_H

struct stackEntry {
    int value;
    struct stackEntry * next;     
};  

/* 
 * Push given value onto indicated stack
 * 
 * Parameters:
 * 		**top 	-- pointer to top pointer held by calling code.
 * 				   head pointer must point to a struct stackEnty or NULL.
 *                 
 *                 NOTE: Failure to initialize an empty list to NULL will
 *                 result in errors in the other functions.
 * 
 * Returns 1 on success, or 0 if memory was not allcated (no change on 0).
 * NOTE: Will modify top pointer in caller
 */
int stackPush(struct stackEntry **top, int val);

/*
 * Pop value off stack, returning it via passed pointer
 *  
 * Parameters:
 * 		**top 	-- pointer to top pointer held by calling code.
 * 				   head pointer must point to a struct stackEnty or NULL.
 * 		*val	-- Pointer to variable in which to store popped value
 * 
 * Returns 1 on success, or 0 if there was nothing to pop
 * NOTE: Will modify top pointer in caller
 */
int stackPop(struct stackEntry **top, int *val);

/*
 * Delete stack, leaving it ready to use again
 * 
 * Parameters:
 * 		**top 	-- pointer to top pointer held by calling code.
 * 				   head pointer must point to a struct stackEnty or NULL.
 * 
 * NOTE: Will modify top pointer in caller
 */
void stackDelete(struct stackEntry **top);

#endif
