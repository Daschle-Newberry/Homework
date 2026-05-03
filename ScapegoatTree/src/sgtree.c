#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>

#include "sgtree.h"

#define MAX(x,y) x < y ? y : x
#define LOG32(n) (log(n) / log(3.0/2.0))

static SGTREE_ERR sg_node_init(SGTreeNode** node_ptr, int value) {
    SGTreeNode* node = malloc(sizeof(SGTreeNode));

    if(!node) {
        return SGTREE_NOMEM;
    }

    node->value = value;
    node->left = NULL;
    node->right = NULL;

    *node_ptr = node;

    return SGTREE_SUCESS;

}

static int sg_subtree_size(SGTreeNode* tree) {
    if(tree == NULL) return 0;

    return sg_subtree_size(tree->left) + sg_subtree_size(tree->right) + 1;
}

static void sg_subtree_destroy(SGTreeNode* tree) {
    if(tree == NULL) return;

    sg_subtree_destroy(tree->left);
    sg_subtree_destroy(tree->right);
    free(tree);
}


static SGTreeNode* flatten(SGTreeNode* x, SGTreeNode* y) {
    if(x == NULL) {
        return y;
    }

    x->right = flatten(x->right, y);
    SGTreeNode* tmp = flatten(x->left,x);

    return tmp;
}


static SGTreeNode* build(SGTreeNode* x, int n) {
    if(n == 0) {
        x->left = NULL;
        return x;
    }

    SGTreeNode* r = build(x,ceil((n - 1)/2.0f));          
    SGTreeNode* s = build(r->right,floor((n - 1)/2.0f));
    r->right = s->left;
    s->left = r;
    return s;
}

static SGTREE_ERR rebuild(SGTreeNode** new_root, SGTreeNode* sg, int n) {
    SGTreeNode* dummy;
    SGTREE_ERR err = sg_node_init(&dummy, 0);
    if (err != SGTREE_SUCESS) return err;

    SGTreeNode* flat_tree = flatten(sg,dummy);
    *new_root = build(flat_tree,n)->left;
    free(dummy);
    return SGTREE_SUCESS;
}

SGTREE_ERR sg_insert(SGTree* tree, int value) {
    if(tree == NULL) {
        return SGTREE_INVALID_ARGUMENT;
    }
    SGTreeNode* node;
    SGTREE_ERR alloc_err = sg_node_init(&node, value);

    if(alloc_err != SGTREE_SUCESS) return alloc_err;

    if(tree->root == NULL) { //< Root insertion
        tree->root = node;
        tree->n++;
        tree->q = MAX(tree->q, tree->n);
        return SGTREE_SUCESS;
    }


    // BST insertion, uses array to eep track of parent references
    SGTreeNode* current_node = tree->root;

    int max_path_length = ceil(LOG32(tree->q)) + 1;
    SGTreeNode* path[max_path_length];

    int d = 0;
    while(1){
        if(d > max_path_length) return SGTREE_INVALID_STATE; //< Previous insert did not uphold max path condition

        path[d++] = current_node;
        SGTreeNode* next_node = node->value < current_node->value ? current_node->left : current_node->right;

        if(next_node == NULL) break;
        if(next_node->value == value) { //< Replacement
            free(node);
            return SGTREE_SUCESS;
        }

        current_node = next_node;
    }

    if(node->value < current_node->value) {
        current_node->left = node;
    } else {
        current_node->right = node;
    }

    tree->n++;
    tree->q = MAX(tree->q, tree->n);
    //This should be a function... Or three...
    if(d > LOG32(tree->q)) {
        SGTreeNode* scapegoat = tree->root;
        int scapegoat_size = tree->n;

        SGTreeNode* scapegoat_parent = NULL;
        SGTreeNode* child = node;

        int child_size = 1;

        for(int i = d - 1; i >= 0; i--) {
            SGTreeNode* parent = path[i];
        
            int parent_size;
            if(child == parent->left) {
                parent_size = sg_subtree_size(parent->right);
            } else {
                parent_size = sg_subtree_size(parent->left);
            } 

            parent_size += child_size + 1;

            if(child_size > ((2.0/3.0) * parent_size)) {
                scapegoat = parent;
                //Case for scapegoat at root
                scapegoat_parent = (i > 0) ? path[i - 1] : NULL;
                scapegoat_size = parent_size;
                break;
            }

            child_size = parent_size;
            child = parent;
        }

        SGTreeNode* new;
        SGTREE_ERR rebuild_err = rebuild(&new,scapegoat,scapegoat_size);

        // Rebuild can only really fail because of a memory error. Still good to check though.
        if(rebuild_err != SGTREE_SUCESS) return rebuild_err;
        
        if(scapegoat_parent == NULL) {      //< Scapegoat is root
            tree->root = new;
        } else if(scapegoat == scapegoat_parent->left) {
            scapegoat_parent->left = new;
        } else if(scapegoat == scapegoat_parent->right) {
            scapegoat_parent->right = new;
        }
    }

    return SGTREE_SUCESS;
}

SGTREE_ERR sg_delete(SGTree* tree, int value) {
    if(tree == NULL) {
        return SGTREE_INVALID_ARGUMENT;
    }

    if(tree->root == NULL) {
        return SGTREE_NOT_FOUND;
    }  

    SGTreeNode* parent = NULL;
    SGTreeNode* current_node = tree->root;

    while(current_node->value != value){
        SGTreeNode* next_node = value < current_node->value ? current_node->left : current_node->right;
        if(next_node == NULL) return SGTREE_NOT_FOUND;

        parent = current_node;
        current_node = next_node;
    }

    tree->n--;

    SGTreeNode* successor = NULL;

    int left_exists = !(current_node->left == NULL);
    int right_exists = !(current_node->right == NULL);


    //Find BST Successor
    if(left_exists && !right_exists) {
       successor = current_node->left; 
    } else if(!left_exists && right_exists) {
        successor = current_node->right;
    } else if (left_exists && right_exists) {
        SGTreeNode* succesor_parent = current_node->right;
        successor = current_node->right;
        
        while(successor->left != NULL) {
            succesor_parent = successor;
            successor = successor->left;
        }

        if(!(succesor_parent == successor)) {
            succesor_parent->left = successor->right;
            successor->right = current_node->right;
        }

        successor->left = current_node->left;
    }  

    if(parent == NULL) { //< Successor is the root, node more work needed.
        free(current_node);
        tree->root = successor;
        return SGTREE_SUCESS;
    } else if(current_node == parent->left) {
        parent->left = successor;
    } else {
        parent->right = successor;
    }

    if(tree->n < tree->q/2.0) { //< Rebuild tree condition
        SGTreeNode* new_root;
        rebuild(&new_root, tree->root,tree->n);
        tree->q = tree->n;
        tree->root = new_root;
    }

    free(current_node);
    return SGTREE_SUCESS;
}

SGTREE_ERR sg_search(SGTree* tree, int value) {
    if(tree == NULL) return SGTREE_INVALID_ARGUMENT;
    if(tree->root == NULL) return SGTREE_NOT_FOUND;

    SGTreeNode* current_node = tree->root;
    while(current_node->value != value){
        SGTreeNode* next_node = value < current_node->value ? current_node->left : current_node->right;
        if(next_node == NULL) return SGTREE_NOT_FOUND;
        current_node = next_node;
    }

    return SGTREE_SUCESS;
}

SGTREE_ERR sg_init(SGTree** tree_ptr) { //< Uses SGTreee** to control allocation location (heap) and allow for error codes
    SGTree* tree = malloc(sizeof(SGTree));
    if(!tree) {
        free(tree);
        return SGTREE_NOMEM;
    }
    tree->n = 0;
    tree->q = 0;
    tree->root = NULL;
    *tree_ptr = tree;
    
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
void sg_traverse_inorder(SGTreeNode* tree, int d) {
    if(tree == NULL) return;

    sg_traverse_inorder(tree->left, d + 1);
    printf("%d ", tree->value);
    sg_traverse_inorder(tree->right, d + 1);
}

void sg_traverse_preorder(SGTreeNode* tree, int d) {
    if(tree == NULL) return;

    printf("%d ", tree->value);
    sg_traverse_preorder(tree->left, d + 1);
    sg_traverse_preorder(tree->right, d + 1);
}

void sg_traverse_postorder(SGTreeNode* tree, int d) {
    if(tree == NULL) return;

    sg_traverse_postorder(tree->left, d + 1);
    sg_traverse_postorder(tree->right, d + 1);
    printf("%d ", tree->value);
}