#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#define MAX_INPUT_LENGTH 100
#define NUM_CHAR_START 48
#define NUM_CHAR_END 57


typedef enum {
    STRTOINT_SUCCESS,
    STRTOINT_OVERFLOW,
    STRTOINT_UNDERFLOW,
    STRTOINT_NONUM,
} strtoint_err;

int is_valid_char(char c) {
    return (NUM_CHAR_START < c && c < NUM_CHAR_END) || (c == '-');
}
int str_to_int(char* str, int* num) {
    char c = *str;
    while(!is_valid_char(c) && c != '\0') {
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
    while(1) {
        char input[MAX_INPUT_LENGTH];
        fgets(input, sizeof(input), stdin);

        char command = input[0];
        int num;
        strtoint_err err = str_to_int(input, &num);
        //Commands which do not take a number input
        switch(command) {
            case 'e' : {
                //TO-DO: Implement this
                puts("You are choosing to empty the tree");  
                break;
            }
            case 'q' : {
                //TO-DO: Implement this
                puts("You are choosing to exit");   
                break;  
            }
            case 'i' : {
                //TO-DO: Implement this
                printf("Adding: %d",num);
                break;
            }
            case 'd' : {
                //TO-DO: Implement this
                printf("Removing: %d", num);
                break;
            }
            case 's' : {
                //TO-DO: Implement this
                printf("Searching: %d", num);
                break;           
            }
            case 't' : {
                //TO-DO: Implement this
                puts("You are choosing to traverse the tree"); 
                break;                    
            }
            default : {
                printf("Unknown command: %c\n", command);
            }
        }
    }
 
    return 0;
}