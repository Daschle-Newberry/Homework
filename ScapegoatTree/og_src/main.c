#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "func.h"



int main(void) {

    int num = 0;
    char input[100];
    char command = '-';
    ScapegoatTree* tree = newTree();

    while (1) {

        scanf(" %c", &command);
        char c = command;
        if (command == 'q') {
            emptyTree(tree);
            free(tree);
            break;
        }

        else if (command == 'e') { 
            emptyTree(tree); 
            continue;
        }


        scanf(" %s", input);
        if(strchr("ids", command)) {
            num = strtol(input, NULL, 0);
            
            if (command == 'i') { insert(tree, num); }

            if (command == 'd') { delete(tree, num); }

            if (command == 's') { search(tree, num); }

            continue;
        }


        if (command == 't') {


            if (input[0] == 'i') { inOrderTraversal(tree->root); }

            if (input[0] == 'l') { preOrderTraversal(tree->root); }

            if (input[0] == 'r') { postOrderTraversal(tree->root); }

            printf("\n");

            continue;
        }
    }

}
