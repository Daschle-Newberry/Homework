#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

typedef struct LLNode LLNode;
typedef int (*CmpFn)(const void* a, const void* b);

typedef struct LinkedList {
  LLNode* head;
  CmpFn cmp;
  size_t len;
} LinkedList;

struct LLNode {
  LLNode* next;
  void* key;
  void* val;      
};


/*
* @brief Initializes a new linked list 
* */
LinkedList* ll_init(CmpFn cmp);

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
 * @note Upon successful insertion, ownership of key and val is transferred to the list node
 *
 * @param key The key for the node, assumed to be a pointer to a heap allocation
 * @param val The vaue for the node, assumed to be a poitner to a heap allocation
 * 
 * @return The new node, or NULL on failure
 * */
LLNode* llnode_init(void* key, void* val);

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
 * @note Upon successful insertion, ownership of key and val is transferred to the list node
 *
 * @param head The head of the list
 * @param key The key for the node, assumed to be a pointer to a heap allocation
 * @param val The vaue for the node, assumed to be a poitner to a heap allocation
 * 
 * @return Whether the insertion was a success or failure
 * */
int ll_insert(LinkedList* list, void* key, void* val);

/*
 * @brief Finds the node with the given key
 *
 * @param list The list to serach
 * @param key The key for the node
 *
 * @return The node with the given key, or NULL if the node doesn't exist
 * */
LLNode* ll_find(const LinkedList* list, const void* key);

#endif

