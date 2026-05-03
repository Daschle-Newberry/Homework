#ifndef FUNC_H
#define FUNC_H

typedef struct Node {

    int value;
    int size;
    struct Node* left;
    struct Node* right;
    struct Node* parent;

}Node;



typedef struct ScapegoatTree {

    int n;
    int q;
    struct Node* root;

}ScapegoatTree;


void insert(ScapegoatTree* tree, int value);

void delete(ScapegoatTree* tree, int value);

void search(ScapegoatTree* tree, int value);

void emptyTree(ScapegoatTree* tree);

void inOrderTraversal(Node* tree);

void preOrderTraversal(Node* tree);

void postOrderTraversal(Node* tree);

ScapegoatTree* newTree(void);

Node* newNode(int value);



#endif

