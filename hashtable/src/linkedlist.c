#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "linkedlist.h"

int  ll_init(LinkedList* list, CmpFn cmp, DstrFn key_dstr, DstrFn val_dstr) {
  if(!list || !cmp || !key_dstr || !val_dstr)
    return -1;

  *list = (LinkedList) {
    .head = NULL,
    .len = 0,
    .cmp = cmp,
    .key_dstr = key_dstr,
    .val_dstr = val_dstr
  };
 
  return 0;
}

void ll_destroy(LinkedList* list) {
 if(!list)
   return;

  LLNode* cursor = list->head;
  while(cursor) {
    LLNode* next = cursor->next;
    llnode_destroy(cursor, list->key_dstr, list->val_dstr);
    cursor = next;
  }
}

void llnode_destroy(LLNode* node, DstrFn key_dstr, DstrFn val_dstr) {
  key_dstr(node->key);
  val_dstr(node->val);
  free(node);
}

LLNode* llnode_create(void* key, void* val) {
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
    list->key_dstr(key);
    list->val_dstr(prev->val);

    prev->val = val;
    return 0;
  }

  LLNode* node = llnode_create(key, val);
  
  if(!node)
    return -1;
  
  node->next = list->head;
  list->head = node;
  list->len++;

  return 0;
}


int ll_splice_node(LinkedList* list, LLNode* node) {
  if(list == NULL || node == NULL)
    return -1;

  node->next = list->head;
  list->head = node;

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


LLNode* ll_pop(LinkedList* list) {
  if(!list || !list->head)
    return NULL;

  LLNode* res = list->head;

  list->head = res->next;
  // So the caller doesn't mess with the internals of the list
  res->next = NULL;

  return res;
}



