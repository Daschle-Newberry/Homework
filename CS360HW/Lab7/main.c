#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t* conds;
static int shared = 0;

void* print(void* b) {
  char* buff = (char*) b;
 
  int x = *(int*)(buff);
  int idx = *(int*)(buff + sizeof(int));
  char* str = *(char**)(buff + (2 * sizeof(int)));

  for(int i = idx; i < strlen(str); i += x) {
    if(i != 0)
      pthread_cond_wait(&conds[idx], &mutex);
    
    printf("%d is printing %d\n",idx,  i);
    printf("%c\n", str[i]);
  
    pthread_mutex_unlock(&mutex);

    pthread_cond_signal(&conds[(idx + 1) % x]);
    }
  pthread_exit(NULL);
}

void print_x(int x) {
  conds = malloc(sizeof(pthread_cond_t) * x);
  for(int i = 0; i < x; i++) {
    pthread_cond_init(&conds[i], NULL);
  }
  
  pthread_cond_signal(&conds[0]);
  pthread_t threads[x];

  char* str = "cheese";
  char* buffs = malloc((sizeof(int) + sizeof(int) + sizeof(char*)) * x);

  // [{int, char*}] = 8bytes
  for(int i = 0; i < x; i++) {
    int index = i * ((2 * (sizeof(int)) + sizeof(char*)));
    
    *(int*) (buffs + index) = x;
    *(int*) (buffs + index + sizeof(int)) = i;
    *(char**)(buffs + index + (2 * sizeof(int))) = str;
    pthread_create(&threads[i], NULL, print, &buffs[index]);
  }

  for(int i = 0; i < x; i++) {
    pthread_join(threads[i], NULL);
  }
}

int main(int argc, char** argv) {

  print_x(2);

  return 0;
} 
