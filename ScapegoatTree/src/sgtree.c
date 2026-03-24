#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "sgtree.h"

#define MAX(x,y) x < y ? y : x
#define LOG32(n) (log(n) / log(3.0/2.0))


static int sg_subtree_size(SGTreeNode* tree) {
    if(tree == NULL) return 0;

    return sg_subtree_size(tree->left) + sg_subtree_size(tree->right);
}

static void sg_subtree_destroy(SGTreeNode* tree) {
    if(tree == NULL) return;

    sg_subtree_destroy(tree->left);
    sg_subtree_destroy(tree->right);
    free(tree);
}

SGTREE_ERR sg_insert(SGTree* tree, int value) {
    SGTreeNode* node = malloc(sizeof(SGTreeNode));

    if(!node) {
        return SGTREE_NOMEM;
    }

    node->value = value;
    node->d = 0;
    node->left = NULL;
    node->right = NULL;

    if(tree->root == NULL) {
        tree->root = node;
        tree->n++;
        tree->q = MAX(tree->q, tree->n);
        return SGTREE_SUCESS;
    }

    SGTreeNode* parent = tree->root;

    // SGTreeNode* path[max_path_length];
    // memset(path, 0, max_path_length);

    // int c = 0;
    // path[c] = parent;

    do {
        node->d++;
        SGTreeNode* child = node->value < parent->value ? parent->left : parent->right;
        
        if(child == NULL) {
            if(node->value < parent->value) {
                parent->left = node;
            } else if(node->value > parent-> value) {
                parent->right = node;
            }  else {
                //replacement
                free(node);
                break;
            }
            
            // path[++c] = child;
            // tree->n++;
            tree->q = MAX(tree->q, tree->n);
            break;
        }
        parent = child;
    } while(1);

    
    SGTreeNode* scapegoat = tree->root;

    // if(node->d > LOG32(tree->q)) {
    //     int last_size = 1;
    //     SGTreeNode* last_node = node;
    //     for(int i = c; i >= 1; i--) {
    //         SGTreeNode* current_node = path[i];
    //         int size = last_size;
    //         if(last_node == current_node->left) {
    //             size += sg_subtree_size(current_node->right);
    //         } else {
    //             size += sg_subtree_size(current_node->left);
    //         }

    //         if(last_size > (2/3) * size) {
    //             scapegoat = current_node;
    //             break;
    //         }

    //         last_size = size;
    //         last_node = current_node;
    //     }
    // }

    printf("Scapegoat: %d\n", scapegoat->value);

    return SGTREE_SUCESS;

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


void sg_destroy(SGTree* tree) {
    if(tree == NULL) return;

    sg_subtree_destroy(tree->root);
    free(tree);
}


void sg_metadata(SGTree* tree) {
    printf("Tree:\n n: %d\n q: %d\n", tree->n, tree->q);
}
void sg_traverse_inorder(SGTreeNode* tree) {
    if(tree == NULL) return;

    sg_traverse_inorder(tree->left);
    printf("Value: %d Depth: %d\n",tree->value, tree->d);
    sg_traverse_inorder(tree->right);
}

void sg_traverse_preorder(SGTreeNode* tree) {
    if(tree == NULL) return;

    printf("Value: %d Depth: %d\n",tree->value, tree->d);
    sg_traverse_inorder(tree->left);
    sg_traverse_inorder(tree->right);
}

void sg_traverse_postorder(SGTreeNode* tree) {
    if(tree == NULL) return;

    sg_traverse_inorder(tree->left);
    sg_traverse_inorder(tree->right);
    printf("Value: %d Depth: %d\n",tree->value, tree->d);

}


//**
//  FEATURES
//      - Traverse (DONE)
//      - Insert
//      - Delete        
// 
//  */