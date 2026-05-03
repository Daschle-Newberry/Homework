/*
 * stack.c
 *
 * Implementation of a simple, dynamically allocated stack of ints
 * 
 * This implementation uses a basic singly linked list to manage values.
 *
 * Written by Paul Bonamy - 21 February 2018
 */

#include <stdio.h>
#include <stdlib.h>     
#include "stack.h"


/* simple enough -- we're always inserting at the head of a list */
int stackPush(struct stackEntry **top, int val)  {    
    struct stackEntry *newbie;
    
    /* we always need to allocate memory, so start there */
    newbie = (struct stackEntry *)malloc(sizeof(struct stackEntry));
    if(!newbie) {
        return 0;
    }
    
    // set value we're storing in the new entry
    newbie->value = val;

    // put this node at the start of the list
    newbie->next = *top;
    *top = newbie;
    return 1;
}


/* 
 * pop could just return what it pops, but then we have trouble telling
 * if the stack was empty. This way, we always know if the stack had 
 * something to pop after we finish the call
 */
int stackPop(struct stackEntry **top, int *val) {
    struct stackEntry *next;

    if(!(*top)) {
        // no values in stack
        return 0;
    } 

    // prepare the value for return
    *val = (*top)->value;

    // remove top element and free it
    next = (*top)->next;
    // BUG FOUND
    // Original code never freed top
    free(*top);
    *top = next;
    return 1;
}

/* just runs through the entire list freeing everything */
void stackDelete(struct stackEntry **top) {
    struct stackEntry *next;  
    while(*top != 0) {     
        next = (*top)->next;
        free(*top);            
        *top = next;         
    }
}
