/*
 * stackuser.c
 *
 * A simple, text-based interface to a stack. Commands are as follows:
 * 
 * u <val>  -- push integer value val onto the stack
 * o        -- pop top value off stack and print it, or print message for empty
 * e        -- empty the stack
 * q        -- quit the program
 *
 * To Compile:
 *      make
 *
 * To Run:
 *      make run
 *
 * Written by Paul Bonamy - 21 February 2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "stack.h"

int main (void) {
    // BUG FOUND
    // Original code left stackEntry unintialized, which means that when we delete the stack, we will never reach *top = 0. This is undefined behavior.
    struct stackEntry *stack = NULL;
    char cmd;           
    int val = 0;
    
    do {
        do {
            scanf("%c", &cmd);
        } while(isspace(cmd));
        
        switch (cmd) {          
            case 'u': // push a value
                // read value to push
                scanf("%d", &val);

                // push value and check for errors
                if(!stackPush(&stack, val)) {
                    printf("Failed to allocate memory. Exiting...\n");
                    stackDelete(&stack);
                    exit(1);
                }
                break;
            
            case 'o':  // pop a value and print it
                if(stackPop(&stack, &val)) {
                    printf("Popped %d\n", val);
                }
                else {
                    printf("Nothing to pop\n");
                }
                break;

            case 'e':  // empty the stack
                stackDelete(&stack);
                break;
            
            case 'q':
                // BUG FOUND
                // Original code never frees the stack on exit
                stackDelete(&stack);
                break;
                
            default:            
                printf("Oi! That's not a legal command\n");
        }
        
    } while (cmd != 'q');
    
    return 0;
}
