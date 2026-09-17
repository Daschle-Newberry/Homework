#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stddef.h>

#include "types.h"

typedef struct LLNode LLNode;

typedef struct LinkedList {
  LLNode* head;
  size_t len;
  CmpFn cmp;
  DstrFn key_dstr, val_dstr;
} LinkedList;

struct LLNode {
  LLNode* next;
  void* key;
  void* val;      
};


/*
* @brief Initializes a new linked list 
* 
* @param list     The list to initialize
* @param cmp      A pointer to a comparison function
* @param key_dstr A pointer to a destructor function for the keys
* @param val_dstr A pointer to a destructor function for the values 
*
* @return 0 on success, -1 on failure
* */
int ll_init(LinkedList* list, CmpFn cmp, DstrFn key_dstr, DstrFn val_dstr);

/*
* @brief Destroys the given linked list, does not free the list itself

* @param node The list to be destroyed
* 
* */
void ll_destroy(LinkedList* list);

/*
 * @brief Creates a new LLNode 
 *
 * @note Upon successful insertion, ownership of key and val is transferred to the list node
 *
 * @param key The key for the node, assumed to be a pointer to a heap allocation
 * @param val The vaue for the node, assumed to be a poitner to a heap allocation
 * 
 * @return The new node, or NULL on failure
 * */
LLNode* llnode_create(void* key, void* val);

/*
* @brief Destroys the given node 
*
* @param node     The node to be destroyed
* @param key_dstr The destructor used for the key
* @param val_dstr The destructor used for the value
*
* */
void llnode_destroy(LLNode* node, DstrFn key_dstr, DstrFn val_dstr);

/*
 * @brief Inserts a new node into the linked list, overwriting val if key already exists
 *
 * @note Upon successful insertion, ownership of key and val is transferred to the list node
 *
 * @param head The head of the list
 * @param key  The key for the node, assumed to be a pointer to a heap allocation
 * @param val  The vaue for the node, assumed to be a poitner to a heap allocation
 * 
 * @return Whether the insertion was a success or failure
 * */
int ll_insert(LinkedList* list, void* key, void* val);

/*
 * @brief Inserts the given node into the list
 * 
 * @note This method does search the list to check for duplicates
 *
 * @param list The list to splice into
 * @param node The node to splic into
 *
 * @return 0 on successful splice, -1 otherwise
 *
 * */
int ll_splice_node(LinkedList* list, LLNode* node);

/*
 * @brief Finds the node with the given key
 *
 * @param list The list to serach
 * @param key  The key for the node
 *
 * @return The node with the given key, or NULL if the node doesn't exist
 * */
LLNode* ll_find(const LinkedList* list, const void* key);

/*
 * @brief Pops the head of the list
 *
 * @note The caller must free the popped node
 *
 * @param list The list to pop from
 *
 * @return The popped node
 * */
LLNode* ll_pop(LinkedList* list);
#endif

