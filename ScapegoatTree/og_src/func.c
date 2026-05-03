#include <stdlib.h>
#include "func.h"
#include <math.h>
#include <stdio.h>


ScapegoatTree* newTree(void);


static Node* _findSuccesor(Node* node) {
    Node* cursor;

    // The only node right is the right child or there is no further successor beyond right node
    if (node->right->size == 1 || node->right->left == NULL) { 
        return node->right;
    } else {
        cursor = node->right;
    }

    while(cursor->left != NULL) {
        cursor = cursor->left;
    }
    return cursor;

}

// x is the input root, y is a pointer to the reordered root, y is the final output of the recursion
static Node* _flatten(Node* x, Node* y) {
    if (x == NULL) {
        return y;
    }

    x->right = _flatten(x->right, y);
    if (x->right) {
        x->right->parent = x;   // update parent pointer
    }

    Node* result = _flatten(x->left, x);
    if (x->left) {
        x->left->parent = NULL; // x->left is moving up the chain, so no parent yet
    }

    return result;
}

static Node* _buildTree(int n, Node* x) {
    if (n == 0) {
        x->left = NULL;
        x->parent = NULL;
        return x;
    }

    int leftSize = (int)ceil((n - 1) / 2.0);
    int rightSize = (int)floor((n - 1) / 2.0);

    Node* r = _buildTree(leftSize, x);
    Node* s = _buildTree(rightSize, r->right);

    r->right = s->left;
    if (r->right) { r->right->parent = r; }  // update parent

    s->left = r;
    r->parent = s;  // update parent

    s->parent = NULL; // s becomes the new root of this subtree
    return s;
}

static Node* _rebuildTree(Node* scapegoat, int n) {
    Node w; // dummy node
    w.left = NULL;
    w.right = NULL;
    w.parent = NULL;

    Node* z = _flatten(scapegoat, &w);
    if (z) { z->parent = NULL; } // top of flattened list

    _buildTree(n, z);

    if (w.left) { w.left->parent = NULL; } // new root
    return w.left;
}


static void _walkTreeDelete(Node* root) {
    // A post order walk but instead of printing it frees the node
    if (root == NULL) { return; }
    _walkTreeDelete(root->left);
    _walkTreeDelete(root->right);
    free(root);
}

static int _recalculateSizes(Node* root) {
    int left = 0;
    int right = 0;

    if (root->left != NULL) {
        left = _recalculateSizes(root->left);
    }
    if (root->right != NULL) {
        right = _recalculateSizes(root->right);
    }
    if (root->left == NULL && root->right == NULL) {
        root->size = 1;
        return 1;
    }
    root->size = left + right + 1;
    return root->size;
}

ScapegoatTree* newTree(void) {
    ScapegoatTree* tree = malloc(sizeof(ScapegoatTree));
    tree->root = NULL;
    tree->n = 0;
    tree->q = 0;
    return tree;
}

Node* newNode(int value) {
    Node* node = malloc(sizeof(Node));
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    return node;
}


void insert(ScapegoatTree* tree, int value) {
    Node* cursor;
    int depth = 0;


    if (tree->root == NULL) {
        Node* node = newNode(value);
        node->size = 1;
        tree->root = node;
        tree->n++;
        tree->q++;
        return;
    } else { 
        cursor = tree->root;
    }

    int inserted = 0;
    while (1) {
        if (cursor->value == value) { return; } // No duplicate values 
        else if (cursor->value > value) {
            if (cursor->left == NULL) {
                Node* node = newNode(value);
                cursor->left = node;
                node->parent = cursor; 
                node->size = 1;
                Node* tmp = cursor;
                while (tmp != NULL) {
                    tmp->size++;
                    printf("Curr: %d\n",tmp->value);
                    tmp = tmp->parent;
                }
                tree->q++;
                inserted = 1;
                cursor = node;
                depth++;
                break;

            } else {
                cursor = cursor->left;
                depth++;
            }

        }
        else if (cursor->value < value) {
            if (cursor->right == NULL) {

                Node* node = newNode(value);
                cursor->right = node;
                node->parent = cursor; 
                node->size = 1;
                Node* tmp = cursor;
                while (tmp != NULL) {
                    tmp->size++;
                    tmp = tmp->parent;
                }
                tree->q++;
                inserted = 1;
                depth++;
                cursor = node;
                break;
            } else {
                cursor = cursor->right;
                depth++;
            }
        }
    }

    if (inserted) {
        double maxDepth = log((double)tree->q) / log(3.0/2.0);
        printf("#Max depth: %f\n", maxDepth);
        if (depth > maxDepth) {
            printf("#Max depth passed from value %d!\n", value);
            while (cursor != NULL) {
                if (cursor->parent == NULL || cursor->size > (2.0/3.0) * cursor->parent->size) {
                    
                    printf("#Scapegoat found: %d, size %d\n", cursor->value, cursor->size);

                    Node* scapegoat = cursor->parent;
                    Node* parent = scapegoat->parent;
                    int side;
                    if (parent->right == scapegoat) { side = 1; }
                    else if (parent->left == scapegoat) { side = -1; }
                    else { side = 0; };

                    Node* subRoot = _rebuildTree(scapegoat, scapegoat->size);

                    if (side ==1 ) {
                        parent->right = subRoot;
                        subRoot->parent = parent;
                    }
                    else if (side == -1) {
                        parent->left = subRoot;
                        subRoot->parent = parent;
                    } else {
                        tree->root = subRoot;
                    }
                    tree->n = _recalculateSizes(tree->root);
                    break;
                }
                cursor = cursor->parent;
            }
        }
    }

    printf("#Tree is now: ");
    preOrderTraversal(tree->root);
    printf("\n");

}

void delete(ScapegoatTree* tree, int value) {
    if (tree->root == NULL) { return; }

    printf("#Deleting %d current pre order: ", value);
    preOrderTraversal(tree->root);
    printf("\n");

    Node* cursor = tree->root;
    while(1) {
        if (cursor->value > value && cursor->left != NULL) {
            cursor = cursor->left;
            continue;
        }
        else if (cursor->value < value && cursor->right != NULL) {
            cursor = cursor->right;
            continue;
        }
        else if (cursor->value == value) {
            break;
        }
        return;
    }

    if (cursor->size == 1) { // Is a leaf
        if (cursor == tree->root) {
            tree->root = NULL;
            tree->n = 0;
            free(cursor);
            return;

        } else {
            if (cursor == cursor->parent->left) {
                cursor->parent->left = NULL;
            }
            else if (cursor == cursor->parent->right) {
                cursor->parent->right = NULL;
            }
        }
    }

    else if ((cursor->left != NULL) ^ (cursor->right != NULL)) { // If cursor only has one child
        if (cursor->left != NULL) { // Only has a left child
            if (cursor == tree->root) {
                tree->root = cursor->left;
                cursor->left->parent = NULL;
            } else {
                if (cursor->parent->left == cursor) {
                    cursor->parent->left = cursor->left;
                } else {
                    cursor->parent->right = cursor->left;
                }
                cursor->left->parent = cursor->parent;
            }
        }
        else if (cursor->right != NULL) { // Only has a right child
            if (cursor == tree->root) {
                tree->root = cursor->right;
                cursor->right->parent = NULL;
            } else {

                if (cursor->parent->left == cursor) {
                    cursor->parent->left = cursor->right;
                } else {
                    cursor->parent->right = cursor->right;
                }
                cursor->right->parent = cursor->parent;
            }
        }
    }

    else { // Cursor has two children
        Node* successor = _findSuccesor(cursor);

        if (successor->parent == cursor) { // Successor is a right child with no left subtree
            successor->left = cursor->left;
            cursor->left->parent = successor;
            if (cursor->parent == NULL) { // is the root
                tree->root = successor;
                successor->parent = NULL;
            } else {
                if (cursor->parent->left == cursor) {
                    cursor->parent->left = successor;
                } else {
                    cursor->parent->right = successor;
                }
            }
        } else { // Successor is not directly connected to cursor and has a right subtree

            successor->left = cursor->left;
            cursor->left->parent = successor;
            cursor->right->parent = successor;

            successor->parent->left = successor->right;
            
            if (successor->right != NULL) { successor->right->parent = successor->parent; }

            successor->right = cursor->right;

            if (cursor->parent == NULL) { // is the root
                tree->root = successor;
                successor->parent = NULL;
            } else {
                if (cursor->parent->left == cursor) {
                    cursor->parent->left = successor;
                } else {
                    cursor->parent->right = successor;
                }
            }

        }

    }

    free(cursor);
    tree->n = _recalculateSizes(tree->root);
    printf("#Deleted something, n is %d and q is %d\n", tree->n, tree->q);
    // reBuild tree as needed

    if (tree->n < tree->q/2.0) {
        Node* newRoot = _rebuildTree(tree->root, tree->root->size);

        tree->root = newRoot;
        tree->n = _recalculateSizes(tree->root);
        tree->q = tree->n;

        printf("#Reordered from delete: ");
        preOrderTraversal(tree->root);
        printf("\n");

    }

}

void search(ScapegoatTree* tree, int value) {
    if (tree->root == NULL) {
        printf("%d is missing", value);
        return;
    }
    Node* cursor = tree->root;
    while(1) {
        if (cursor == NULL ) { break; } // If it is a leaf and isn't the value being searched break
        if (cursor->value > value) {
            cursor = cursor->left;
            continue;
        }
        else if (cursor->value < value) {
            cursor = cursor->right;
            continue;
        }
        else if (cursor->value == value) {
            printf("%d is present", value);
            return;
        }
        
    }
    printf("%d is missing", value);
}

void emptyTree(ScapegoatTree* tree) {
    if (tree->root == NULL) { return; }
    _walkTreeDelete(tree->root);
    tree->root = NULL;
}

void inOrderTraversal(Node* root) {
    if (root == NULL) { return; }
    inOrderTraversal(root->left);
    printf("%d ", root->value);
    inOrderTraversal(root->right);
}

void preOrderTraversal(Node* root) {
    if (root == NULL) { return; }
    printf("%d ", root->value);
    preOrderTraversal(root->left);
    preOrderTraversal(root->right);
}

void postOrderTraversal(Node* root) {
    if (root == NULL) { return; }
    postOrderTraversal(root->left);
    postOrderTraversal(root->right);
    printf("%d ", root->value);
}


