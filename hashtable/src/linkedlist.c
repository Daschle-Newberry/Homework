#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include <assert.h>
#include "linkedlist.h"

LinkedList* ll_init() {
  LinkedList* list = malloc(sizeof (LinkedList));
  
  assert(list);

  if(!list)
    return NULL;

  list->head = NULL;
  list->len = 0;

  return list;
}

void ll_destroy(LinkedList* list) {
  LLNode* cursor = list->head;
  while(cursor) { 
    llnode_destroy(cursor);
    cursor = cursor->next;
  } 
  free(list);
}

void llnode_destroy(LLNode* node) {
  free(node->key);
  free(node);
}

LLNode* llnode_init(const char* key, int val) {
  LLNode* node = malloc(sizeof (LLNode));
 
  if(!node)
    return NULL;
  
  node->next = NULL;

  size_t  len = strlen((char*) key) + 1;
  node->key = (char*) malloc(len * sizeof (char));
  int n = snprintf(node->key, len, "%s", (char*) key);
  
  // Failure to copy entire string
  if(n < (len - 1)) {
    free(node);
    assert(0);
    return NULL;
  }

  node->val = val;
  
  return node;
}

int ll_insert(LinkedList* list, const void* key, const void* val) {
  if(list == NULL) 
    return -1;
  
  LLNode* prev;
  if((prev = ll_find(list, key))) {
    prev->val = *(int*) val;
    return 0;
  }

  LLNode* node = llnode_init((char*)key, *(int*)val);
  
  assert(node);
  if(!node)
    return -1;
  
  node->next = list->head;
  list->head = node;
  list->len++;

  return 0;
}

LLNode* ll_find(const LinkedList* list, const void* key) {
  if(!list | !key)
    return NULL;

  char* keyStr = (char*) key; 
  LLNode* cursor = list->head;
 
  for(LLNode* cursor = list->head; cursor != NULL; cursor = cursor->next) {
    if(strcmp(cursor->key, keyStr) == 0)
      return cursor;
  }
  
  return NULL;
}





