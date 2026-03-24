#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "sgtree.h"

#define MAX_INPUT_LENGTH 100
#define NUM_CHAR_START 48
#define NUM_CHAR_END 57

typedef enum {
    STRTOINT_SUCCESS,
    STRTOINT_OVERFLOW,
    STRTOINT_UNDERFLOW,
    STRTOINT_NONUM,
} STRTOINT_ERR;

static char* STRTOINT_ERR_STRINGS[] = {
    [STRTOINT_SUCCESS] = "STRTOINT_SUCCESS",
    [STRTOINT_OVERFLOW] = "STRTOINT_OVERFLOW",
    [STRTOINT_UNDERFLOW] = "STRTOINT_UNDERFLOW",
    [STRTOINT_NONUM] = "STRTOINT_NONUM"
};

int is_valid_num_char(char c) {
    return (NUM_CHAR_START <= c && c <= NUM_CHAR_END) || (c == '-');
}

int str_to_int(char* str, int* num) {
    char c = *str;
    while(!is_valid_num_char(c) && c != '\0') {
        c = *++str;
    }

    errno = 0;
    char* end;
    long l = strtol(str,&end, 0);
    
    //Error handling inspired by https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
    if(l > INT_MAX || (errno == ERANGE && l == LONG_MAX)) {
        return STRTOINT_OVERFLOW;
    }
    if(l < INT_MIN || (errno == ERANGE && l == LONG_MIN)) {
        return STRTOINT_UNDERFLOW;
    }
    if(end == str) {
        return STRTOINT_NONUM;
    }

    *num = (int) l;

    return STRTOINT_SUCCESS;
}

int main() {
    SGTree* tree;
    sg_init(&tree);
    
    int should_exit = 0;
    while(!should_exit) {
        char input[MAX_INPUT_LENGTH];
        char* result = fgets(input, sizeof(input), stdin);
        //Break on EOF
        if(result == NULL) {
            break;
        }

        char command = input[0];
        int num_arg;
        char char_arg;
        
        if(!strchr("metqids",command)) {
            printf("Invalid Command: %s\n", &command);
            continue;
        }

        if(strchr("ids",command) != NULL) {
            STRTOINT_ERR err = str_to_int(input, &num_arg);
            if(err != STRTOINT_SUCCESS) {
                printf("Invalid Number: %s\n", STRTOINT_ERR_STRINGS[err]);
                continue;
            }
        } else if(strchr("t",command)) {
            sscanf(input + 1, " %c",&char_arg);
            if(strchr("ilr",char_arg) == NULL) {
                printf("Unknown traversal specifier: %c\n",char_arg);
                continue;
            }
        }

        switch(command) {
            case 'e' : {
                sg_destroy(tree);
                sg_init(&tree);
                break;
            }
            case 'm' : {
                sg_metadata(tree);
                break;
            }
            case 't' : {
                if(char_arg == 'i') sg_traverse_inorder(tree->root);
                else if(char_arg == 'l') sg_traverse_preorder(tree->root);
                else sg_traverse_postorder(tree->root);
                break;                    
            }
            case 'q' : {
                should_exit = 1;
                continue;  
            }
            case 'i' : {
                sg_insert(tree, num_arg);
                break;
            }
            case 'd' : {
                //TO-DO: Implement this
                printf("Removing: %d\n", num_arg);
                break;
            }
            case 's' : {
                //TO-DO: Implement this
                printf("Searching: %d\n", num_arg);
                break;           
            }
            default : {
                printf("Unknown command: %c\n", command);
            }
        }
    }

    sg_destroy(tree);
 
    return 0;
}