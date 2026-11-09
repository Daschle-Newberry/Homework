#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "assignment7.h"

int vstrcmp(const void* a, const void* b) {
    const char* sa = *(const char* const*)a;
    const char* sb = *(const char* const*)b;
    return strcmp(sa, sb);
}

static int tests_run = 0;
static int tests_failed = 0;
 
#define RED   "\033[1;31m"
#define GREEN "\033[1;32m"
#define RESET "\033[0m"

#define BENCHMARK(func_call, elapsed) \
  do {                                \
    struct timespec start;            \
    struct timespec end;              \
    clock_gettime(CLOCK_MONOTONIC, &start); \
    func_call;                               \
    clock_gettime(CLOCK_MONOTONIC, &end); \
    elapsed = (long long)(end.tv_sec - start.tv_sec) * 1000000000LL \
                          + (end.tv_nsec - start.tv_nsec);           \
  }while(0)                           

#define ASSERT(cond) \
do { \
    if (!(cond)) { \
        fprintf(stderr, \
            "\e[1;31mFAIL\e[0m: %s\n    at %s:%d\n", \
             #cond, __FILE__, __LINE__); \
    } \
    else {\
        fprintf(stderr, \
            "\e[1;32mPASS\e[0m: %s\n", #cond); \
    }\
} while (0)

#define ASSERT_LT(a, b) do {                                           \
    long long _a = (a), _b = (b);                                      \
    tests_run++;                                                       \
    if (_a < _b) {                                                     \
        fprintf(stderr, GREEN "PASS" RESET ": %s < %s  (%lld < %lld)\n",\
                #a, #b, _a, _b);                                       \
    } else {                                                           \
        tests_failed++;                                                \
        fprintf(stderr, RED "FAIL" RESET ": %s < %s\n"                 \
                "    got %lld >= %lld\n    at %s:%d\n",                \
                #a, #b, _a, _b, __FILE__, __LINE__);                   \
    }                                                                  \
} while (0)

int char_arr_eq(char** a, char** b, int n) {
  for(int i = 0; i < n; i++) {
    if(strcmp(a[i], b[i]) != 0) {
      printf("%s != %s\n", a[i], b[i]);
      return 0;
    }
  }

  return 1;
}

void test_sort(int n_threads, char** input, int n) {
  printf("Testing with %d threads on %d words\n", n_threads, n);
  setSortThreads(n_threads);
  char** c1 = malloc(sizeof(*c1) * n);
  char** c2 = malloc(sizeof(*c2) * n);
  
  memcpy(c1, input, n * sizeof(*input));
  memcpy(c2, input, n * sizeof(input));

  long qsort_time;
  long sortThreaded_time;

  BENCHMARK(qsort(c1, n, sizeof(*c1), vstrcmp), qsort_time);
  BENCHMARK(sortThreaded(c2, n), sortThreaded_time);
  
  int is_sorted = char_arr_eq(c1, c2, n);

  ASSERT(is_sorted == 1);
  ASSERT_LT(sortThreaded_time, qsort_time);
  free(c1);
  free(c2);
}

int main(void) {
   int n = 0, cap = 1024, limit = -1;

  char** words = malloc(cap * sizeof(*words));
  char buf[128];
  while((n < limit || limit == -1) && scanf("%127s\n", buf) == 1) {
    if(n == cap) {
      cap *= 2;
      words = realloc(words, cap * sizeof(*words));
    }

    words[n++] = strdup(buf);
  }


  test_sort(1, words, n);
  test_sort(2, words, n);
  test_sort(4, words, n);
  test_sort(8, words, n);
  test_sort(16, words, n);

  for(int i = 0; i < n; i++) {
    free(words[i]);
  }

  free(words);
  return 1;
}
