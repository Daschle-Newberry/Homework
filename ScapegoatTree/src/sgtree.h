#ifndef SGTREE_H
#define SGTREE_H

typedef struct SGTreeNode {
    int value;                  //< Value contained
    struct SGTreeNode* left;    //< Left child
    struct SGTreeNode* right;   //< Right child
} SGTreeNode;

typedef struct SGTree {
    int n;              //< Actual depth
    int q;              //< Overestimate of depth
    SGTreeNode* root;   //< Root node
} SGTree;

typedef enum {
    SGTREE_SUCESS,
    SGTREE_NOT_FOUND,
    SGTREE_NOMEM,
    SGTREE_INVALID_STATE,
    SGTREE_INVALID_ARGUMENT
} SGTREE_ERR;

/**
 *  Insert into the given scapegoat tree
 *  @param tree_ptr Pointer to a tree pointer which will be reassigned to the new memory location
 *  @return Error code for initilization
 */

SGTREE_ERR sg_init(SGTree** tree_ptr);

/**
 *  Insert into the given scapegoat tree
 *  @param tree Pointer to tree which will be used for insertion
 *  @param value Value to be inserted
 *  @return Error code for insert
 */
SGTREE_ERR sg_insert(SGTree* tree, int value);



/**
 *  Delete node from given scapegoat tree
 *  @param tree Pointer to tree which will be used for deletion
 *  @param value Value to be deleted
 *  @return Error code for delete
 */
SGTREE_ERR sg_delete(SGTree* tree, int value);

SGTREE_ERR sg_search(SGTree* tree, int value);

/**
 * Destroy the tree and all nodes
 * @param tree The tree which will be destroyed   
 */
void sg_destroy(SGTree* tree);

/**
 * Prints the metadata (n and q values) for the provided tree
 * @param tree The tree to print
 */
void sg_metadata(SGTree* tree);

/**
 * Prints the in order traversal of the provided tree
 * @param tree The tree to traverse and print
 */
void sg_traverse_inorder(SGTreeNode* tree, int d);

/**
 * Prints the pre order traversal of the provided tree
 * @param tree The tree to traverse and print
 */
void sg_traverse_preorder(SGTreeNode* tree, int d);

/**
 * Prints the in post order traversal of the provided tree
 * @param tree The tree to traverse and print
 */
void sg_traverse_postorder(SGTreeNode* tree, int d);

#endif