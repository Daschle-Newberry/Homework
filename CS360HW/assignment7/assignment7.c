#include "assignment7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <err.h>

#define SORT_THRESHOLD      40

typedef void* (*JobFunc)(void* args);

typedef struct _job {
  struct _job* next;
  void* args;
  JobFunc func;  
} Job;

typedef struct _job_queue {
  Job* head;
  Job* tail;
  size_t n; 
} JobQueue;

typedef struct _thread_pool {
  pthread_mutex_t mutex;
  pthread_cond_t  notify;
  JobQueue   job_queue;
  pthread_t* threads;
  size_t     n_threads;
  int       destroy;
} ThreadPool;

typedef struct _sortParams {
    char** array;
    int left;
    int right;
} SortParams;

static int maximumThreads;              /* maximum # of threads to be used */

static inline Job* jq_pop(JobQueue* jq) {
  if(jq->head == NULL)
    return NULL;

  Job* job = jq->head;
  jq->head = jq->head->next;
  
  if(job == jq->tail)
    jq->tail = jq->head;

  jq->n--;
  return job;
}

static inline void jq_push(JobQueue* jq, Job* job) {
  jq->tail->next = job;
  jq->tail = job;
}

static int tp_destroy(ThreadPool* tp) {
  tp->destroy = 1;
  pthread_cond_broadcast(&tp->notify);

  for(size_t i = 0; i < tp->n_threads; i++)
    pthread_join(tp->threads[i], NULL);

  free(tp->threads);
  pthread_mutex_destroy(&tp->mutex);
  pthread_cond_destroy(&tp->notify);
  return 0;
}

static void* thread_start(void* tp) {
  ThreadPool* thread_pool = (ThreadPool*) tp;
  JobQueue* job_queue = &thread_pool->job_queue;

  pthread_mutex_lock(&thread_pool->mutex);

  while(1) {
    while(thread_pool->job_queue.n == 0) {
      pthread_cond_wait(&thread_pool->notify, &thread_pool->mutex);
    } 

    if(thread_pool->destroy == 1)
      break;
    
    Job* job = jq_pop(job_queue);
    pthread_mutex_unlock(&thread_pool->mutex);
    printf("Thread receiving job\n");

    job->func(job->args);
  }

  return NULL;  
}

static int tp_create(ThreadPool* tp, size_t n) {
  if(tp == NULL) {
    errno = EINVAL;
    return 1;
  }
  
  pthread_t* threads = NULL;
  size_t threads_created = 0;
  int cond_init = 0, mutex_init = 0;
  int err = 0;

  if((err = pthread_cond_init(&tp->notify, 0)) != 0)
    goto error;
  cond_init = 1;

  if((err = pthread_mutex_init(&tp->mutex, 0)) != 0)
    goto error;
  mutex_init = 1;

  threads = malloc(sizeof(*threads) * n);

  if(threads == NULL) {
    errno = ENOMEM;
    goto error;
  }

  tp->job_queue = (JobQueue) {
    .head = NULL,
    .tail = NULL,
    .n = 0,
  };

  for(; threads_created < n; threads_created++) {
    int err = pthread_create(&threads[threads_created], NULL, thread_start, tp);
    
    // Signal all threads to exit
    if(err) {
      pthread_mutex_lock(&tp->mutex);
      tp->destroy = 1;
      pthread_cond_broadcast(&tp->notify);
      pthread_mutex_unlock(&tp->mutex);
      goto error;
    }
  }

  tp->threads = threads;
  tp->n_threads = threads_created;
  tp->destroy = 0;
  return 0;

error:
  for(size_t i = 0; i < threads_created; i++)
    pthread_join(tp->threads[i], NULL);

  free(threads);
  if(mutex_init) pthread_mutex_destroy(&tp->mutex);
  if(cond_init) pthread_cond_destroy(&tp->notify);
  errno = err;

  return 1;
}


void* thread_test(void* arg) {
  int* a = (int*)arg;

  printf("Thread found %d\n", *a);

  return NULL;
}

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
static void quickSort(void* p) {
    SortParams* params = (SortParams*) p;
    char** array = params->array;
    int left = params->left;
    int right = params->right;
    int i = left, j = right;
    
    if (j - i < SORT_THRESHOLD) {
      insertSort(array, i , j);
      return;
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
}

/* user interface routine to set the number of threads sortT is permitted to use */

void setSortThreads(int count) {
    maximumThreads = count;
}

/* user callable sort procedure, sorts array of count strings, beginning at address array */

void sortThreaded(char** array, unsigned int count) {
   // SortParams parameters;
   // parameters.array = array; parameters.left = 0; parameters.right = count - 1;
   // quickSort(&parameters);
   
    ThreadPool tp;
    tp_create(&tp, 8);

    Job* job = malloc(sizeof(*job));
    int id = 1;

    job->args = &id;
    job->func = thread_test;


    pthread_mutex_lock(&tp.mutex);
    jq_push(&tp.job_queue, job);
    pthread_mutex_unlock(&tp.mutex);

    while(tp.job_queue.n != 0) {
      pthread_cond_wait(&tp.notify, &tp.mutex);
    }

    tp_destroy(&tp);
}
