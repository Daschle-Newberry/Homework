#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "linkedlist.h"
#include "hash_table.h"
#include "crc64.h"

#define ASSERT_ON_FAIL

#ifdef ASSERT_ON_FAIL

#include <assert.h>

#define EXPECT_EQ(teststr, expected, actual) \
  do { \
    int _expected = (expected); \
    int _actual = (actual);     \
    if(_expected == _actual) \
      printf("  PASS: %s\n", teststr); \
    else  \
      printf("  FAIL: %s (expected %d, got %d)\n",teststr, _expected, _actual); \
    assert(_expected == _actual); \
  }while(0); \

#define EXPECT_NEQ(teststr, expected, actual) \
  do { \
    int _expected = (expected); \
    int _actual = (actual);     \
    if(_expected != _actual) \
      printf("  PASS: %s\n", teststr); \
    else \
      printf("  FAIL: %s (expected %d, got %d)\n",teststr, _expected, _actual); \
    assert(_expected != _actual); \
  }while(0); \


#else

#define EXPECT_EQ(teststr, expected, actual) \
  do { \
    int _expected = (expected); \
    int _actual = (actual);     \
    if(_expected == _actual) \
      printf("  PASS: %s\n", teststr); \
    else \
      printf("  FAIL: %s (expected %d, got %d)\n",teststr, _expected, _actual); \
  }while(0); \

#define EXPECT_NEQ(teststr, expected, actual) \
  do { \
    int _expected = (expected); \
    int _actual = (actual);     \
    if(_expected != _actual) \
      printf("  PASS: %s\n", teststr); \
    else \
      printf("  FAIL: %s (expected %d, got %d)\n",teststr, _expected, _actual); \
  }while(0); \

#endif

void test_ht_insert();
void test_ll_insert();
void test_ll_find();

int main() {
  test_ll_insert();
  test_ll_find();
  test_ht_insert();
  return 0;
}

int string_compare(const void* a, const void* b) {
  return strcmp((char*)a, (char*)b);
}

unsigned long long hash(const void* data) {
  return crc64((char*)data);
}


void test_ht_insert() {
  puts("Testing ht_insert...");
  HashTable table;

  ht_init(&table, &string_compare, &hash, &free, &free);
  
  int* v1 = malloc(sizeof(int));
  *v1 = 0;    

  EXPECT_EQ("Insert into empty, returns 0", 0, ht_insert(&table, strdup("Hello"), v1));
  EXPECT_EQ("Insert into empty, value is 0", *v1, *(int*)ht_find(&table, "Hello"));

  int* v2 = malloc(sizeof(int));
  *v2 = 1;
  EXPECT_EQ("Insert into non-empty, returns 0", 0, ht_insert(&table, strdup("World!"), v2));
  EXPECT_EQ("Insert into non-empty, value is 1", *v2, *(int*)ht_find(&table, "World!"));

  ht_destroy(&table);
}
// Suppress static analysis complaining about my unchecked use of malloc
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wanalyzer-possible-null-dereference"
void test_ll_insert() {
  puts("Testing ll_insert...");

  LinkedList list = {
    .head = NULL, 
    .len = 0, 
    .cmp = &string_compare, 
    .key_dstr = &free, 
    .val_dstr = &free
  };
  
  int* val1 = malloc(4);
  *val1 = 1;
  int insertEmptyResult = ll_insert(&list, strdup("Hello"), val1); 
  
  EXPECT_EQ("Insert into empty, returns zero", 0, insertEmptyResult);
  EXPECT_EQ("Inserted into empty, key is 'Hello'", 0, strcmp("Hello", list.head->key));
  EXPECT_EQ("Inserted into empty, value is 1",*val1, *(int*)list.head->val);

  LLNode* originalHead = list.head;
  int* val2 = malloc(4);
  *val2 = 1;
  int insertResult = ll_insert(&list, strdup("World!"), val2);
  
  EXPECT_EQ("Insert into non-empty, returns zero", 0, insertResult);
  EXPECT_EQ("Insert into non-empty, key is equal to 'World!'", 0, strcmp("World!", list.head->key));
  EXPECT_EQ("Insert into non-empty, value is equal to 2",*val2,*(int*)list.head->val);
  EXPECT_EQ("Insert into non-empty, next pointer is equal to original head", (long)originalHead,(long)list.head->next);

  int* val3 = malloc(4);
  *val3 = 3;

  int replaceResult = ll_insert(&list, strdup("Hello"), val3);

  EXPECT_EQ("Insert replace node, returns 0", 0, replaceResult);
  EXPECT_EQ("Insert replace node, original node value equal 3", *val3, *(int*)originalHead->val);
  EXPECT_EQ("Insert replace node, len is equal to 2", 2, list.len);

  llnode_destroy(originalHead, list.key_dstr, list.val_dstr);
  llnode_destroy(list.head, list.key_dstr, list.val_dstr);
}

void test_ll_find() {
  puts("Testing ll_find");

  LinkedList list = { 
    .head = NULL, 
    .len = 0, 
    .cmp = &string_compare,
    .key_dstr = &free,
    .val_dstr = &free
  };

  LLNode* res = ll_find(&list, "Hi");

  EXPECT_EQ("Find on empty, returns null", (long) NULL, (long) res);
  
  int* zero = malloc(4);
  *zero = 0;
  LLNode* lastNode = llnode_create(strdup("0"), zero);
  list.head = lastNode;

  for(int i = 0; i < 100; i++) {
    char* key = malloc(4);
    int* val = malloc(4);

    snprintf(key, 4, "%d", i);
    *val = i;

    LLNode* node = llnode_create(key, val);
    lastNode->next = node;
    lastNode = node;
  }
  
  res = ll_find(&list, "0");

  EXPECT_NEQ("Find first element, doesn't return NULL", (long) NULL, (long) res);
  EXPECT_EQ("Find first element, node has correct key", 0, strcmp("0", res->key));
  EXPECT_EQ("Find first element, node has correct value", 0,*(int*) res->val);

  res = ll_find(&list, "49");
  EXPECT_NEQ("Find middle element, doesn't return NULL", (long) NULL, (long) res);
  EXPECT_EQ("Find middle element, node has correct key", 0, strcmp("49", res->key));
  EXPECT_EQ("Find middle element, node has correct value", 49,*(int*) res->val);

  res = ll_find(&list, "99");
  EXPECT_NEQ("Find last element, doesn't return NULL", (long) NULL, (long) res);
  EXPECT_EQ("Find last element, node has correct key", 0, strcmp("99", res->key));
  EXPECT_EQ("Find last element, node has correct value", 99, *(int*)res->val);


  res = ll_find(&list, "100");
  EXPECT_EQ("Find non-existant, returns NULL", (long) NULL, (long) res);

  LLNode* cursor = list.head;
  while(cursor) {
    LLNode* next = cursor->next;
    llnode_destroy(cursor, list.key_dstr, list.val_dstr);
    cursor = next;
  }
}

#pragma GCC diagnostic pop
