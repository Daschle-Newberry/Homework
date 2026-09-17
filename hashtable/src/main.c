#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include "getWord.h"
#include "hash_table.h"
#include "linkedlist.h"
#include "crc64.h"

static int string_compare(const void* a, const void* b) {
  return strcmp((const char*) a, (const char*) b);
}

static hash64_t string_hash(const void* a) {
  return crc64((char*) a);
}

int static kv_compare(const void* a, const void* b) {
  const KVPair* p1 = (KVPair*) a;
  const KVPair* p2 = (KVPair*) b;

  return *(int*)p2->val - *(int*)p1->val;
}

int main(int argc, char **argv) {  
  // Argv lacks count, at least one file, or both
  if(argc < 2) { 
    fprintf(stderr, "pairsofwords: invalid set of arguments\n");
    return -1;
  }
  

  int first_file = 1;
  size_t count = ULONG_MAX;
  if(argv[1][0] == '-') {
    first_file = 2;
    char* end;
    count = strtoul(&argv[1][1], &end, 0);
    
    if(errno == ERANGE || count < 1) {
      fprintf(stderr, "pairsofwords: %lu: count must be within 1 and %lu\n", count, ULONG_MAX);
      return -1;
    }

    if(*end != '\0') {
      fprintf(stderr, "pairsofwords: %s: invalid count string", &argv[1][0]);
      return -1;
    }
  }

  HashTable tb;
  ht_init(&tb, &string_compare, &string_hash, &free, &free, 256);
  
  for(int i = first_file; i < argc; i++) {
    FILE* f = fopen(argv[i], "r");
    if(!f) {
      fprintf(stderr, "pairsofwords: %s: could not open file\n", argv[i]);
      continue;
    }
    
    char* last_word = getNextWord(f);
    char* curr_word;

    while((curr_word = getNextWord(f))) {
      int pair_len = strlen(last_word) + strlen(curr_word) + 2; // pair_len + ' ' + '\0';
      char* pair = malloc(pair_len);      
      snprintf(pair, pair_len, "%s %s", last_word, curr_word);
      
      int* old_c = (int*)ht_find(&tb, pair);
      
      int* c = malloc(sizeof(int));
      assert(c);
      
      // Count is either 1 or old count + 1;
      *c = old_c == NULL ? 1 : (*old_c) + 1;
      
      int err = ht_insert(&tb, pair, c);
      assert(!err);

      free(last_word);
      last_word = curr_word;
    }

    free(last_word);
    fclose(f);
  }

  size_t num_eles;
  KVPair* kvp = ht_to_array(&tb, &num_eles);
  assert(kvp);

  qsort(kvp, num_eles, sizeof(KVPair), &kv_compare);

  size_t n = (count < num_eles) ? count : num_eles;
  for(int i = 0; i < n; i++) {
    printf("%10d %s\n", *(int*) kvp[i].val, (char*) kvp[i].key);
  }

  free(kvp);
  ht_destroy(&tb);

	return 0;
}
