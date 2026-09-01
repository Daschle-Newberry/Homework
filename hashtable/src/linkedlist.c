#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "linkedlist.h"

LinkedList* ll_init(CmpFn cmp) {
  LinkedList* list = malloc(sizeof (LinkedList));

  if(!list)
    return NULL;

  list->cmp = cmp;
  list->head = NULL;
  list->len = 0;

  return list;
}

void ll_destroy(LinkedList* list) {
 if(!list)
   return;

  LLNode* cursor = list->head;
  while(cursor) {
    LLNode* next = cursor->next;
    llnode_destroy(cursor);
    cursor = next;
  }

  free(list);
}

void llnode_destroy(LLNode* node) {
  free(node->key);
  free(node->val);
  free(node);
}

LLNode* llnode_init(void* key, void* val) {
  LLNode* node = malloc(sizeof (LLNode));
 
  if(!node)
    return NULL;
  
  node->next = NULL;
  node->key = key;
  node->val = val;

  return node;
}

int ll_insert(LinkedList* list, void* key, void* val) {
  if(list == NULL) 
    return -1;

  LLNode* prev;
  if((prev = ll_find(list, key))) {
    // Free the replaced value and the inserted key to maintain ownership contract
    free(prev->val);
    free(key);

    prev->val = val;
    return 0;
  }

  LLNode* node = llnode_init(key, val);
  
  if(!node)
    return -1;
  
  node->next = list->head;
  list->head = node;
  list->len++;

  return 0;
}

LLNode* ll_find(const LinkedList* list, const void* key) {
  if(!list || !key)
    return NULL;
 
  for(LLNode* cursor = list->head; cursor != NULL; cursor = cursor->next) {  
    if(list->cmp(key, cursor->key) == 0)
      return cursor;
  }
  
  return NULL;
}





