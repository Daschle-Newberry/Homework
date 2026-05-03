#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "sgtree.h"

int main() {
    SGTree* tree;
    sg_init(&tree);
    
    char command;
    char char_arg;
    int num_arg;

    int should_exit = 0;
    while(!should_exit && scanf(" %c", &command) == 1) {

        if(strchr("sid",command)) { //< Command has a number following it
            scanf("%i",&num_arg);
        } else if(command == 't') {      //< Command has a char follwing it
            scanf(" %c",&char_arg);
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
                if(char_arg == 'i') sg_traverse_inorder(tree->root, 0);
                else if(char_arg == 'l') sg_traverse_preorder(tree->root, 0);
                else sg_traverse_postorder(tree->root, 0);
                puts(""); //< Newline (should have a better solution)
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
                sg_delete(tree, num_arg);  
                break;
            }
            case 's' : {
                SGTREE_ERR res = sg_search(tree, num_arg);
                if(res == SGTREE_SUCESS) {
                    printf("%d is present\n",num_arg);
                } else {
                    printf("%d is missing\n",num_arg);
                }
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