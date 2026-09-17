#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include "types.h"

typedef struct LinkedList LinkedList;

typedef struct HashTable {
  LinkedList* buckets;
  size_t num_buckets;
  size_t num_elements;
  HashFn hash;
  CmpFn cmp;
  DstrFn key_dstr;
  DstrFn val_dstr;
} HashTable;

typedef struct KVPair {
  const void* key;
  const void* val;
} KVPair;

/*
* @brief Initializes a new hash table 
* 
* @param table          The table to initialize
* @param cmp            A pointer to a comparison function
* @param hash           A pointer to a hash function
* @param key_dstr       A pointer to a destructor function for the keys
* @param val_dstr       A pointer to a destructor function for the values 
* @param start_buckets  The amount of buckets to start with
*
* @return 0 on success, -1 on failure
* */
int ht_init(
    HashTable* table, 
    CmpFn cmp, 
    HashFn hash, 
    DstrFn key_dstr, 
    DstrFn val_dstr, 
    size_t start_buckets
    );

/*
* @brief Destroys the given hashtable, does not free the table itself
*
* @param node The hashtable to be destroyed
* 
* */
void ht_destroy(HashTable* list);

/*
 * @brief Inserts a new item into the table, overwriting val if key already exists
 *
 * @note Upon successful insertion, ownership of key and val is transferred to the hashtable node
 *
 * @param table The hashtable to insert into
 * @param key   The key for the node, assumed to be a pointer to a heap allocation
 * @param val   The vaue for the node, assumed to be a poitner to a heap allocation
 * 
 * @return 0 on success, -1 otherwise
 * */
int ht_insert(HashTable* table, void* key, void* val);

/*
 * @brief Finds the given value associated with the given key
 *
 * @param table The table to search
 * @param key   The key for the value
 *
 * @return The value, or NULL if the node doesn't exist
 * */
 const void* ht_find(const HashTable* table, const void* key);

/*
 * @brief Creates an array for all key/val pairs in the table
 *
 * @param table The table to flatten
 * @count count Pointer to an integer where the array count will be stored
 *
 * @return A pointer to the flattened array
 */
 KVPair* ht_to_array(const HashTable* table, size_t* count);

#endif
