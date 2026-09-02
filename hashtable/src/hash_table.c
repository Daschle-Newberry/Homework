#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "hash_table.h"
#include "linkedlist.h"

#define MIN_NUM_BUCKETS 10

int ht_init(HashTable* table, CmpFn cmp, HashFn hash, DstrFn key_dstr, DstrFn val_dstr) {
  if(!table || !cmp || !hash || !key_dstr || !val_dstr)
    return -1;

  table->buckets = malloc(MIN_NUM_BUCKETS * sizeof(LinkedList));
 
  for(int i = 0; i < MIN_NUM_BUCKETS; i++) {
    // Note: this could potentially be optimized to prevent each linked list
    // from holding cmp, key_dstr, and val_dstr
    ll_init(&table->buckets[i], cmp, key_dstr, val_dstr);
  }
  
  table->num_buckets = MIN_NUM_BUCKETS;
  table->hash = hash;

  return 0;
}

void ht_destroy(HashTable* table) {
  for(size_t i = 0; i < table->num_buckets; i++) {
    ll_destroy(&table->buckets[i]);
  }

  free(table->buckets);
}

static LinkedList* ht_get_bucket(const HashTable* table, const void* key) {
  hash64_t hash = table->hash(key);

  return &table->buckets[hash % table->num_buckets];
}

int ht_insert(HashTable* table, void* key, void* val) {
  LinkedList* bucket = ht_get_bucket(table, key);

  assert(bucket != NULL);

  return ll_insert(bucket, key, val);
}

const void* ht_find(const HashTable* table, const void* key) {
  LinkedList* bucket = ht_get_bucket(table, key);
  LLNode* node = ll_find(bucket, key);

  return node ? node->val : NULL;
}

