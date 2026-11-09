#include "assignment7.h"
#include <stdio.h>
#include <sys/param.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <err.h>

#define SORT_THRESHOLD      40

typedef struct _sortParams {
    char** restrict array;
    int left;
    int right;
} SortParams;

static int maximumThreads = 1;              /* maximum # of threads to be used */

/* This is an implementation of insert sort, which although it is */
/* n-squared, is faster at sorting short lists than quick sort,   */
/* due to its lack of recursive procedure call overhead.          */
static void insertSort(char** array, int left, int right) {
    int i, j;
    for (i = left + 1; i <= right; i++) {
        char* pivot = array[i];
        j = i - 1;
        while (j >= left && (strcmp(array[j],pivot) > 0)) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = pivot;
    }
}

/* Recursive quick sort, but with a provision to use */
/* insert sort when the range gets small.            */
static void* quickSort(void* p) {
    SortParams* params = (SortParams*) p;
    char** array = params->array;
    int left = params->left;
    int right = params->right;
    int i = left, j = right;
    
    if (j - i < SORT_THRESHOLD) {
      insertSort(array, i , j);
      return NULL;
    }

    int m = (i + j) >> 1;               /* pick pivot as median of         */
    char* temp, *pivot;                 /* first, last and middle elements */
    if (strcmp(array[i],array[m]) > 0) {
        temp = array[i]; array[i] = array[m]; array[m] = temp;
    }
    if (strcmp(array[m],array[j]) > 0) {
        temp = array[m]; array[m] = array[j]; array[j] = temp;
        if (strcmp(array[i],array[m]) > 0) {
            temp = array[i]; array[i] = array[m]; array[m] = temp;
        }
    }
    pivot = array[m];

    for (;;) {
        while (strcmp(array[i],pivot) < 0) i++; /* move i down to first element greater than or equal to pivot */
        while (strcmp(array[j],pivot) > 0) j--; /* move j up to first element less than or equal to pivot      */
        if (i < j) {
            char* temp = array[i];      /* if i and j have not passed each other */
            array[i++] = array[j];      /* swap their respective elements and    */
            array[j--] = temp;          /* advance both i and j                  */
        } else if (i == j) {
            i++; j--;
        } else break;                   /* if i > j, this partitioning is done  */
    }
    
    SortParams first;  first.array = array; first.left = left; first.right = j;
    quickSort(&first);                  /* sort the left partition  */
    
    SortParams second; second.array = array; second.left = i; second.right = right;
    quickSort(&second);                 /* sort the right partition */
    return NULL;
}

/* user interface routine to set the number of threads sortT is permitted to use */

void setSortThreads(int count) {
    maximumThreads = count;
}

void merge(
    size_t ln, 
    size_t rn, 
    char* restrict left[ln], 
    char* restrict right[rn], 
    char* restrict out[ln + rn]) 
{
  size_t li = 0, ri = 0, idx = 0;
  while(li < ln && ri < rn) {
    if(strcmp(left[li], right[ri]) <= 0)
      out[idx] = left[li++];
    else 
      out[idx] = right[ri++];
    idx++;
  }
  
  while(li < ln)
    out[idx++] = left[li++];

  while(ri < rn)
    out[idx++] = right[ri++];
}


int kmerge(
    unsigned int n,
    char* array[restrict n], 
    unsigned int k, 
    unsigned int bounds[restrict k]
    ) 
{
  if(array == NULL || bounds == NULL) {
    errno = EINVAL;
    return 1;
  }

  if(k == 1)
    return 0;

  char** buff = malloc(sizeof(*buff) * n);
  char** src = array;
  char** dest = buff;
  
  if(dest == NULL) {
    return 1;
  }
  
  while(k > 1) {
    int i = 0;
    for(; i < k - 1; i += 2) {
      int s = bounds[i], m = bounds[i + 1], e = bounds[i + 2];
      merge(m - s, e - m, &src[s], &src[m], &dest[s]);
    }

    if(i == k - 1) {
      int s = bounds[i], e = bounds[i + 1];
      memcpy(&dest[s], &src[s], sizeof(*src) * (e - s));
    }
    
    unsigned int new_k = 0;
    for(unsigned int i = 0; i < k; i += 2) {
      bounds[new_k++] = bounds[i];  
    }
    bounds[new_k] = n;
    
    k = new_k;
    
    char** tmp = src;
    src = dest;
    dest = tmp;
  }

  if(src != array)
    memcpy(array, src, sizeof(*src) * n);

  free(buff);

  return 0;
}

/* user callable sort procedure, sorts array of count strings, beginning at address array */
void sortThreaded(char** array, unsigned int count) {
  pthread_t* threads = malloc(sizeof(*threads) * maximumThreads);
  SortParams* params = malloc(sizeof(*params) * maximumThreads);
  unsigned int* bounds = malloc(sizeof(*bounds) * count + 1);

  if(threads == NULL || params == NULL || bounds == NULL) {
    perror("malloc");
    goto error;
  }
  
  int num_threads = MIN(count, maximumThreads);
  int threads_created = 0;
  int err = 0;

  // Partition array k-ways and have each thread sort a subarray
  for(size_t i = 0; i < num_threads; i++) {
    params[i] = (SortParams) {
      .array = array,
      .left = i * count / num_threads,
      .right = (i + 1) * count / num_threads - 1
    };

    if((err = pthread_create(&threads[i], NULL, quickSort, &params[i])) != 0) {
      errno = err;
      perror("pthread_create");
      goto error;
    }
  }

  for(size_t i = 0; i < num_threads; i++) {
    int err = pthread_join(threads[i], NULL);
  }

  // Let all threads be joined before checking for a pthread_join error
  if(err) {
    errno = err;
    perror("pthread_join");
    goto error;
  }

  int partitions = num_threads;
  for(unsigned int i = 0; i < partitions; i++)
    bounds[i] = params[i].left;
  bounds[partitions] = count;

  if((err = kmerge(count, array, partitions, bounds))) {
    perror("kmerge");
    goto error;
  }

error:
  free(params);
  free(threads);
  free(bounds);
}
