#include <pthread.h>
#include <stdio.h>

struct Counter {
  int val;
  pthread_mutex_t mutex;  
};


void* increment(void* counter) {
  struct Counter* c = (struct Counter*) counter;

  while(1) {
    pthread_mutex_lock(&c->mutex);
    
    if(c->val >= 1000) {
      pthread_mutex_unlock(&c->mutex);
      break;
    }

    printf("%ld starting increment (%d)\n", pthread_self(), c->val);
    c->val++;
    printf("%ld ending increment (%d)\n", pthread_self(), c->val);
    pthread_mutex_unlock(&c->mutex);
  }

  return NULL;
}



int main(int argc, char** argv) {
  struct Counter counter = {0, PTHREAD_MUTEX_INITIALIZER};
    
  pthread_t t1, t2, t3;

  pthread_create(&t1, NULL, increment, &counter);
  pthread_create(&t2, NULL, increment, &counter);
  pthread_create(&t3, NULL, increment, &counter);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  
  return 0;
}
