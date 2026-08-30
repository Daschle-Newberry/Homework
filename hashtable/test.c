#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "linkedlist.h"

#define STR(x) #x

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


void test_ll_insert();
void test_ll_find();

int main() {
  test_ll_insert();
  test_ll_find();
  return 0;
}

void test_ll_insert() {
  puts("Testing ll_insert...");

  LinkedList list = { .head = NULL, .len = 0 };
  
  int val1 = 1;
  int insertEmptyResult = ll_insert(&list, "Hello", &val1);
  
  EXPECT_EQ("Insert into empty, returns zero", 0, insertEmptyResult);
  EXPECT_EQ("Inserted into empty, key is 'Hello'", 0, strcmp("Hello", list.head->key));
  EXPECT_EQ("Inserted into empty, value is 1", val1, list.head->val);

  LLNode* originalHead = list.head;
  int val2 = 2;
  int insertResult = ll_insert(&list, "World!", &val2);
  
  EXPECT_EQ("Insert into non-empty, returns zero", 0, insertResult);
  EXPECT_EQ("Insert into non-empty, key is equal to 'World!'", 0, strcmp("World!", list.head->key));
  EXPECT_EQ("Insert into non-empty, value is equal to 2", val2, list.head->val);
  EXPECT_EQ("Insert into non-empty, next pointer is equal to original head", (long)originalHead,(long)list.head->next);

  int val3 = 3;

  int replaceResult = ll_insert(&list, "Hello", &val3);

  EXPECT_EQ("Insert replace node, returns 0", 0, replaceResult);
  EXPECT_EQ("Insert replace node, original node value equal 3", 3, originalHead->val);
  EXPECT_EQ("Insert replace node, len is equal to 2", 2, list.len);

  llnode_destroy(originalHead);
  llnode_destroy(list.head);
}

void test_ll_find() {
  puts("Testing ll_find");

  LinkedList list = { .head = NULL, .len = 0 };

  LLNode* res = ll_find(&list, "Hi");

  EXPECT_EQ("Find on empty, returns null", (long) NULL, (long) res);
  

  LLNode* lastNode = llnode_init("0", 0);
  list.head = lastNode;

  for(int i = 0; i < 100; i++) {
    char key[4];
    snprintf(key, 4, "%d", i);
    
    LLNode* node = llnode_init(key, i);
    lastNode->next = node;
    lastNode = node;
  }
  
  res = ll_find(&list, "0");

  EXPECT_NEQ("Find first element, doesn't return NULL", (long) NULL, (long) res);
  EXPECT_EQ("Find first element, node has correct key", 0, strcmp("0", res->key));
  EXPECT_EQ("Find first element, node has correct value", 0, res->val);

  res = ll_find(&list, "49");
  EXPECT_NEQ("Find middle element, doesn't return NULL", (long) NULL, (long) res);
  EXPECT_EQ("Find middle element, node has correct key", 0, strcmp("49", res->key));
  EXPECT_EQ("Find middle element, node has correct value", 49, res->val);

  res = ll_find(&list, "99");
  EXPECT_NEQ("Find last element, doesn't return NULL", (long) NULL, (long) res);
  EXPECT_EQ("Find last element, node has correct key", 0, strcmp("99", res->key));
  EXPECT_EQ("Find last element, node has correct value", 99, res->val);


  res = ll_find(&list, "100");
  EXPECT_EQ("Find non-existant, returns NULL", (long) NULL, (long) res);

  LLNode* cursor = list.head;
  while(cursor) {
    LLNode* next = cursor->next;
    llnode_destroy(cursor);
    cursor = next;
  }
}

