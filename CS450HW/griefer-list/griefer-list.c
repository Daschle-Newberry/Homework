/*
 *  Please note that this assignment was created by combining my CS261
 *  scapegoat tree implementation with my vector implementation from high-scores. 
 *  These were done at different times, so the code quality/style may vary throughout.
 *
 * */

#include <time.h>
#include <stdint.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MAX(x,y) x < y ? y : x
#define LOG32(n) (log(n) / log(3.0/2.0))

typedef struct Int32Vector {
  size_t len;
  size_t cap;
  int32_t* data;
} Int32Vector;

typedef struct Griefer {
      char name[13];
      Int32Vector ban_list;
      time_t last_ban;
    } Griefer;

typedef struct SGTreeNode {
    Griefer griefer;            // Griefer metadata
    struct SGTreeNode* left;    //< Left child
    struct SGTreeNode* right;   //< Right child
} SGTreeNode;

typedef struct SGTree {
    int n;              //< Actual depth
    int q;              //< Overestimate of depth
    SGTreeNode* root;   //< Root node
} SGTree;

typedef enum {
    SGTREE_SUCCESS,
    SGTREE_NOT_FOUND,
    SGTREE_NOMEM,
    SGTREE_INVALID_STATE,
    SGTREE_INVALID_ARGUMENT
} SGTREE_ERR;


int i32vec_init(Int32Vector* vector, size_t capacity);
int i32vec_push_back(Int32Vector* vector, int32_t value);
int i32vec_search(Int32Vector* vector, int32_t value);
void i32vec_destroy(Int32Vector* vector);

SGTREE_ERR sg_init(SGTree** tree_ptr);
void sg_destroy(SGTree* tree);

SGTREE_ERR sg_insert(SGTree* tree, char* name, int32_t server_id, time_t time);
SGTreeNode* sg_search(SGTree* tree, char* name);

SGTREE_ERR sg_node_init(SGTreeNode** node_ptr, char* name, int32_t server_id, time_t ban_time);
void sg_node_destroy(SGTreeNode* node);

int sg_subtree_size(SGTreeNode* tree);
void sg_subtree_destroy(SGTreeNode* tree);
SGTreeNode* flatten(SGTreeNode* x, SGTreeNode* y);
SGTreeNode* build(SGTreeNode* x, int n);
SGTREE_ERR rebuild(SGTreeNode** new_root, SGTreeNode* sg, int n);

int main(int argc, char** argv) {
    if(argc < 2) {
      puts("Error: No input file");
      return 1;
    }

    FILE* file = fopen(argv[1], "r");
    
    if(file == NULL)
      err(1, "Error:");
   
    // Create scapegoat tree
    SGTree* tree;
    SGTREE_ERR err = sg_init(&tree);

    if(err != SGTREE_SUCCESS) {
      printf("Error: Scapegoat tree could not be created: %d", err);
      return 1;
    }
  
    char name[13];
    int32_t id;
    time_t time;
    
    // Insert ban data from file
    while(fscanf(file, "%s %d %lu", name, &id, &time) != -1) {
      sg_insert(tree, name, id, time);
    }
    
    // Search for each player in the input
    while(scanf("%s", name) != -1) {
      SGTreeNode* node = sg_search(tree, name);

      if(node == NULL)
        printf("%s is not currently banned from any servers.\n", name);
      else
        printf(
            "%s was banned from %lu servers. most recently on: %lu\n", 
            name, 
            node->griefer.ban_list.len, 
            node->griefer.last_ban
            );
    }

    sg_destroy(tree);
}

// Create vector with given capacity
int i32vec_init(Int32Vector* vector, size_t capacity) {
  if(!vector || capacity < 1)
    return -1;
  
  int32_t* data = malloc(sizeof(*vector->data) * capacity);
  
  if(!data)
    return -1;

  vector->len = 0;
  vector->cap = capacity;
  
  vector->data = data;
  return 0;
}

// Insert value into vector
int i32vec_push_back(Int32Vector* vector, int32_t value) {
  if(!vector)
    return 1;

  if(vector->len == vector->cap) {
    size_t new_cap = vector->cap * 2;
    int32_t* new_data = realloc(vector->data, new_cap * sizeof(*new_data));
    if(!new_data)
      return -1;
    
    vector->cap = new_cap;
    vector->data = new_data;
  }

  vector->data[vector->len] = value;
  vector->len++;

  return 0;
}

// Find value in vector, 1 if found 0 otherwise
int i32vec_search(Int32Vector* vector, int32_t value) {
  for(size_t i = 0; i < vector->len; i++) {
    if(vector->data[i] == value) return 1;
  }

  return 0;
}

// Free vector memory
void i32vec_destroy(Int32Vector* vector) {
  free(vector->data);
}

// Initialize a scapegoat tree node
SGTREE_ERR sg_node_init(SGTreeNode** node_ptr, char* name, int32_t server_id, time_t ban_time) {
    SGTreeNode* node = malloc(sizeof(SGTreeNode));

    if(!node) {
        return SGTREE_NOMEM;
    }
  
    strncpy(node->griefer.name, name, sizeof(node->griefer.name));

    int err = i32vec_init(&node->griefer.ban_list, 4);
    i32vec_push_back(&node->griefer.ban_list, server_id);

    if(err) {
      sg_node_destroy(node);
      return SGTREE_NOMEM;
    }

    node->griefer.last_ban = ban_time;

    node->left = NULL;
    node->right = NULL;

    *node_ptr = node;

    return SGTREE_SUCCESS;
}

// Free scapegoat node memory
void sg_node_destroy(SGTreeNode* node) {
  if(node == NULL)
    return;

  i32vec_destroy(&node->griefer.ban_list);
  free(node);
}

// Get number of nodes in a scapegoat tree root at "tree" 
int sg_subtree_size(SGTreeNode* tree) {
    if(tree == NULL) return 0;

    return sg_subtree_size(tree->left) + sg_subtree_size(tree->right) + 1;
}

// Recursively free all scapegoat nodes in a subtree
void sg_subtree_destroy(SGTreeNode* tree) {
    if(tree == NULL) return;

    sg_subtree_destroy(tree->left);
    sg_subtree_destroy(tree->right);
    sg_node_destroy(tree);
}

// Flatten a subtree into a linked list
SGTreeNode* flatten(SGTreeNode* x, SGTreeNode* y) {
    if(x == NULL) {
        return y;
    }

    x->right = flatten(x->right, y);
    SGTreeNode* tmp = flatten(x->left,x);

    return tmp;
}

// Build a flattened subtree into a balanced subtree
SGTreeNode* build(SGTreeNode* x, int n) {
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

// Rebuild a subtree
SGTREE_ERR rebuild(SGTreeNode** new_root, SGTreeNode* sg, int n) {
    SGTreeNode* dummy;
    SGTREE_ERR err = sg_node_init(&dummy, "", 0, 0);
    if (err != SGTREE_SUCCESS) return err;

    SGTreeNode* flat_tree = flatten(sg,dummy);
    *new_root = build(flat_tree,n)->left;
    sg_node_destroy(dummy);
    return SGTREE_SUCCESS;
}

// Insert a node into a scapegoat tree
SGTREE_ERR sg_insert(SGTree* tree, char* name, int32_t server_id, time_t ban_time) {
    if(tree == NULL) {
        return SGTREE_INVALID_ARGUMENT;
    }

    SGTreeNode* node;
    
    // Case where the new node is the root
    if(tree->root == NULL) {
      SGTREE_ERR alloc_err = sg_node_init(&node, name, server_id, ban_time);
      if(alloc_err != SGTREE_SUCCESS) return alloc_err;

      tree->root = node;
      tree->n++;
      tree->q = MAX(tree->q, tree->n);
      return SGTREE_SUCCESS;
    }


    // BST insertion, uses array to keep track of parent references
    SGTreeNode* current_node = tree->root;

    int max_path_length = ceil(LOG32(tree->q)) + 1;
    SGTreeNode* path[max_path_length];

    int d = 0;
    while(1){
        if(d > max_path_length) return SGTREE_INVALID_STATE; //< Previous insert did not uphold max path condition

        path[d++] = current_node;
        int cmp = strcmp(name, current_node->griefer.name);
        
        // Griefer is already in the scapegoat tree
        if(cmp == 0) {
          if(!i32vec_search(&current_node->griefer.ban_list, server_id)) {
            i32vec_push_back(&current_node->griefer.ban_list, server_id);
          }
          current_node->griefer.last_ban = MAX(
              current_node->griefer.last_ban, 
              ban_time
              );

          return SGTREE_SUCCESS;
        }

        SGTreeNode* next_node = cmp < 0 ? current_node->left : current_node->right;

        if(next_node == NULL) break;

        current_node = next_node;
    }

    SGTREE_ERR alloc_err = sg_node_init(&node, name, server_id, ban_time);
    if(alloc_err) {
      return SGTREE_NOMEM;
    }

    if(strcmp(node->griefer.name, current_node->griefer.name) < 0) {
        current_node->left = node;
    } else {
        current_node->right = node;
    }

    tree->n++;
    tree->q = MAX(tree->q, tree->n);
    // Find the scapegoat if the tree is too deep
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
        if(rebuild_err != SGTREE_SUCCESS) return rebuild_err;
        
        if(scapegoat_parent == NULL) {      //< Scapegoat is root
            tree->root = new;
        } else if(scapegoat == scapegoat_parent->left) {
            scapegoat_parent->left = new;
        } else if(scapegoat == scapegoat_parent->right) {
            scapegoat_parent->right = new;
        }
    }

    return SGTREE_SUCCESS;
}

// BST on the scapegoat tree
SGTreeNode* sg_search(SGTree* tree, char* name) {
    if(tree == NULL) return NULL;
    if(tree->root == NULL) return NULL;

    SGTreeNode* current_node = tree->root;
    int cmp;
    while((cmp = strcmp(current_node->griefer.name, name)) != 0){
        SGTreeNode* next_node = cmp < 0 ? current_node->right : current_node->left;
        if(next_node == NULL) return NULL;
        current_node = next_node;
    }

    return current_node;
}

// Initialize a scapegoat tree
SGTREE_ERR sg_init(SGTree** tree_ptr) {    
    SGTree* tree = malloc(sizeof(SGTree));
    if(!tree) {
        free(tree);
        return SGTREE_NOMEM;
    }
    tree->n = 0;
    tree->q = 0;
    tree->root = NULL;
    *tree_ptr = tree;
    
    return SGTREE_SUCCESS;
}

// Free all scapegoat tree memory
void sg_destroy(SGTree* tree) {
    if(tree == NULL) return;

    sg_subtree_destroy(tree->root);
    free(tree);
}

