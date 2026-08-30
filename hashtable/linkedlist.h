#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

typedef struct LLNode LLNode;

typedef struct LinkedList {
  LLNode* head;
  size_t len;
} LinkedList;

struct LLNode {
  LLNode* next;
  char* key;
  int val;      
};


/*
* @brief Initializes a new linked list 
* */
LinkedList* ll_init();

/*
* @brief Destroys the given linked list 
*
* @param node The list to be destroyed
* 
* */
void ll_destroy(LinkedList* list);

/*
 * @brief Initializes a new LLNode 
 *
 * @param key The string key for the node
 * @param val The int value of the node
 * 
 * @return The new node, or NULL on failure
 * */
LLNode* llnode_init(const char* key, int val);

/*
* @brief Destroys the given node 
*
* @param node The node to be destroyed
* 
* */
void llnode_destroy(LLNode* node);

/*
 * @brief Inserts a new node into the linked list, overwriting val if key already exists
 *
 * @param head The head of the list
 * @param key The key of the node, assumed to be char* within the scope of the project
 * @param val The value of the node, assumed to be int* within the scope of the project
 * 
 * @return Whether the insertion was a success or failure
 * */
int ll_insert(LinkedList* list, const void* key, const void* val);

/*
 * @brief Finds the node with the given key
 *
 * @param head The head of the list
 * @param key The key of the node, assumed to be char* within the scope of the project * 
 * @return The node with the given, or NULL if the node doesn't exist
 * */
LLNode* ll_find(const LinkedList* list, const void* key);

#endif

