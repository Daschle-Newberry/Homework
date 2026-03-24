#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

#define MAX_INPUT_LENGTH 100
#define NUM_CHAR_START 48
#define NUM_CHAR_END 57

typedef struct SGTreeNode {
    int value;
    struct SGTreeNode* left;
    struct SGTreeNode* right;
} SGTreeNode;

typedef struct SGTree {
    int n;
    int q;
    SGTreeNode* root;
} SGTree;

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

typedef enum {
    SGTREE_SUCESS,
    SGTREE_NOMEM,

} SGTREE_ERR;
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

SGTREE_ERR sg_insert(SGTree* tree, int value) {
    SGTreeNode* node = malloc(sizeof(SGTreeNode));

    if(!node) {
        return SGTREE_NOMEM;
    }

    node->value = value;
    node->left = NULL;
    node->right = NULL;

    if(tree->root == NULL) {
        tree->root = node;
        return SGTREE_SUCESS;
    }

    SGTreeNode* parent = tree->root;
    
    do {
        SGTreeNode* child = node->value < parent->value ? parent->left : parent->right;
        if(child == NULL) {
            if(node->value < parent->value) {
                parent->left = node;
            } else {
                parent->right = node;
            }
            return SGTREE_SUCESS;
        }
        parent = child;
    } while(1);

}

void sg_traverse_inorder(SGTreeNode* tree) {
    if(tree == NULL) return;

    sg_traverse_inorder(tree->left);
    printf("Value: %d\n",tree->value);
    sg_traverse_inorder(tree->right);
}

void sg_traverse_preorder(SGTreeNode* tree) {

}

void sg_traverse_postorder(SGTreeNode* tree) {
    
}

SGTREE_ERR sg_init(SGTree** tree_ptr) {
    SGTree* ptr = malloc(sizeof(SGTree));
    if(!ptr) {
        free(ptr);
        return SGTREE_NOMEM;
    }
    ptr->n = 0;
    ptr->q = 0;
    ptr->root = NULL;
    *tree_ptr = ptr;
    
    return SGTREE_SUCESS;
}

int main() {
    SGTree* tree;
    sg_init(&tree);

    int should_exit = 0;
    while(!should_exit) {
        char input[MAX_INPUT_LENGTH];
        fgets(input, sizeof(input), stdin);

        char command = input[0];
        int num_arg;
        char char_arg;
        
        if(!strchr("etqids",command)) {
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
                //TO-DO: Implement this
                break;
            }
            case 't' : {
                sg_traverse_inorder(tree->root);
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
 
    return 0;
}
