#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "hash_table.h"
#include "linkedlist.h"

#define LOAD_FACTOR_THRESH .75
#define RESIZE_FACTOR 3

static LinkedList* get_bucket(const HashTable* table, const void* key) {
  hash64_t hash = table->hash(key);

  return &table->buckets[hash % table->num_buckets];
}

static inline double load_factor(size_t n, size_t m) {
  return (double) n / (double) m;
}

static int resize(HashTable* table, size_t factor) {
  if(factor <= 1 || table == NULL)
    return -1;

  size_t new_num_buckets = table->num_buckets * factor;

  // Overflow
  if(new_num_buckets < table->num_buckets)
    return -1;
  
  size_t old_bytes = table->num_buckets * sizeof(*table->buckets);
  size_t new_bytes = new_num_buckets * sizeof(*table->buckets);
  
  // Bytes Overflow
  if(old_bytes > new_bytes)
    return -1;

  LinkedList* new_buckets = malloc(new_bytes);
  
  if(!new_buckets)
    return -1;
 
  // Initialize new buckets
  for(size_t i = 0; i < new_num_buckets; i++)
    ll_init(&new_buckets[i], table->cmp, table->key_dstr, table->val_dstr);
  
  LinkedList* old_buckets = table->buckets;
  size_t old_num_buckets = table->num_buckets;

  table->buckets = new_buckets;
  table->num_buckets = new_num_buckets;

  // Rehash all existing elements
  for(size_t i = 0; i < old_num_buckets; i++) {
    LLNode* node;

    while((node = ll_pop(&old_buckets[i]))) {
      LinkedList* bucket = get_bucket(table, node->key);
      assert(bucket);
      
      ll_splice_node(bucket, node);
    }
  }

  free(old_buckets);

  return 0;
}

int ht_init(
    HashTable* table, 
    CmpFn cmp,
    HashFn hash, 
    DstrFn key_dstr, 
    DstrFn val_dstr,
    size_t start_buckets
    ) {
  if(!table || !cmp || !hash || !key_dstr || !val_dstr)
    return -1;

  *table = (HashTable) {
    .buckets = malloc(start_buckets * sizeof(LinkedList)),
    .num_buckets = start_buckets,
    .num_elements = 0,
    .hash = hash,
    .cmp = cmp,
    .key_dstr = key_dstr,
    .val_dstr = val_dstr
  };

  if(!table->buckets)
    return -1;

  for(int i = 0; i < start_buckets; i++) {
    // Note: this could potentially be optimized to prevent each linked list
    // from holding cmp, key_dstr, and val_dstr
    ll_init(&table->buckets[i], cmp, key_dstr, val_dstr);
  }
 
  return 0;
}

void ht_destroy(HashTable* table) {
  for(size_t i = 0; i < table->num_buckets; i++) {
    ll_destroy(&table->buckets[i]);
  }

  free(table->buckets);
}

int ht_insert(HashTable* table, void* key, void* val) {
  double lf = load_factor(table->num_elements, table->num_buckets);
 
  // #################################################################
  // Table resizes when the load factor is above .75, following Java's hashtable
  // implementation. 
  //
  // The load factor is computed as alpha = n/m, where n is the number of elements
  // and m is the number of buckets. We use load factor instead of probability
  // of collision because load factor represents the amount of work for collision resolution.
  //
  // #################################################################
  if(lf > LOAD_FACTOR_THRESH)
    resize(table, RESIZE_FACTOR);

  LinkedList* bucket = get_bucket(table, key);
  assert(bucket != NULL);
  
  size_t old_len = bucket->len;
  int err = ll_insert(bucket, key, val);

  // Increment the element count only when an insertion actually increases
  // the list length
  table->num_elements += (old_len != bucket->len);

  return err;
}

const void* ht_find(const HashTable* table, const void* key) {
  LinkedList* bucket = get_bucket(table, key);
  LLNode* node = ll_find(bucket, key);

  return node ? node->val : NULL;
}


KVPair* ht_to_array(const HashTable* table, size_t* count) {
  KVPair* arr = malloc(table->num_elements * sizeof(KVPair));
  
  if(!arr)
    return NULL;

  size_t c = 0;
  for(size_t i = 0; i < table->num_buckets; i++) {
    LinkedList* list = &table->buckets[i];
    
    for(LLNode* cursor = list->head; cursor; cursor = cursor->next, c++)
      arr[c] = (KVPair) { .key = cursor->key, .val = cursor->val };
  }

  *count = table->num_elements;
  return arr;
}
