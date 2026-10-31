#include <bits/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <err.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

typedef struct _Table {
  pthread_mutex_t mutex;
  pthread_cond_t* can_eat;
  int*            chopsticks;
  long            n;
} Table;

typedef struct _Philospher {
  int eat_time;
  int rand_mean;
  int rand_stddev;
  int id;
  Table* table;

} Philospher;

int strtoi(char* str, int* res) {
    if(str == NULL || res == NULL) {
      errno = EINVAL;
      return -1;
    }

    char* endptr = NULL;
    long num = strtol(str, &endptr, 10);

    if(num > INT_MAX || num < INT_MIN) {
      errno = ERANGE;
      return -1;
    }
  
    // Enforce entire string to be included in conversion
    if((size_t)(endptr - str) != strlen(str)){
      errno = EINVAL;
      return -1;
    }
    
    *res = num;
    return 0;

}

int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}

// True sleep, ensures the process sleeps for the entire time
void tsleep(int seconds) {
  while(seconds > 0) {
    seconds = sleep(seconds);
  }
}


// Atomically claim both chopsticks
int claim_chopsticks(Philospher* p) {
  Table* t = p->table;
  int chop_left  = p->id;
  int chop_right = (p->id + 1) % t->n;

  int err;
  if((err = pthread_mutex_lock(&t->mutex)) != 0)
    return err;

  while(
      t->chopsticks[chop_left]  == 0 || 
      t->chopsticks[chop_right] == 0
      ) 
    {
      // Wait for a neighbor to notify
      if((err = pthread_cond_wait(&t->can_eat[p->id], &t->mutex)) != 0)
        return err;
    }
    
    // Claim both chopsticks
    t->chopsticks[chop_left]  = 0;
    t->chopsticks[chop_right] = 0;
  
    if((err = pthread_mutex_unlock(&t->mutex)) != 0)
      return err;

    return 0;
}

// Atomically release both chopsticks
int release_chopsticks(Philospher* p) {
  Table* t = p->table;
  int chop_left  = p->id;
  int chop_right = (p->id + 1) % t->n;

  int left_neighbor  = (p->id + t->n - 1) % t->n;
  int right_neighbor = chop_right;

  int err;
  if((err = pthread_mutex_lock(&t->mutex)) != 0)
    return err;

  t->chopsticks[chop_left]  = 1;
  t->chopsticks[chop_right] = 1;

  if((err = pthread_cond_signal(&t->can_eat[left_neighbor])) != 0)
    return err;
  if((err = pthread_cond_signal(&t->can_eat[right_neighbor])) != 0)
    return err;

  if((err = pthread_mutex_unlock(&t->mutex)) != 0)
    return err;

  return 0;
}

void* thread_simulate(void* philospher) {
  Philospher* p = (Philospher*) philospher;

  // Seed random for children
  struct timespec t;
  if(clock_gettime(CLOCK_MONOTONIC, &t) == -1) 
    errx(errno, "clock_gettime");

  srand(t.tv_nsec);
 
  int err;
  int eat_remain = p->eat_time;
  while(eat_remain > 0) {
    int cur_think = randomGaussian(p->rand_mean, p->rand_stddev);
    int cur_eat   = randomGaussian(p->rand_mean, p->rand_stddev);

    cur_think = MAX(cur_think, 0);
    cur_eat   = MIN(MAX(cur_eat, 0), eat_remain);

    printf("philosopher %d is thinking for %d seconds\n",p->id, cur_think);
    tsleep(cur_think);

    // Claim chopsticks to eat
    if((err = claim_chopsticks(p)))
      errx(err, "claim_chopsticks: %s\n", strerror(err));

    printf("philosopher %d is eating for %d seconds\n", p->id, cur_eat);
    tsleep(cur_eat);

    // Release chopsticks
    if((err = release_chopsticks(p)))
      errx(err, "release_chopsticks: %s\n", strerror(err));
    
    eat_remain -= cur_eat;
  }
  
  printf("philosopher %d ate for %d seconds and is leaving the table\n", p->id, p->eat_time);
  
  pthread_exit(NULL);
}

int simulate(
    int eat_time, 
    int rand_mean, 
    int rand_stddev, 
    int philo_count
) {
  pthread_t       threads[philo_count];
  pthread_cond_t  can_eat[philo_count];
  int             chopsticks[philo_count];
  Philospher      philosphers[philo_count];
  
  Table table = {
    .can_eat = can_eat,
    .chopsticks = chopsticks,
    .n = philo_count
  };
  
  int err;
  if((err = pthread_mutex_init(&table.mutex, 0)) != 0)
    return err;

  for(int i = 0; i < philo_count; i++) {
    if((err = pthread_cond_init(&table.can_eat[i], 0)) != 0)
        return err;

    chopsticks[i] = 1;
    philosphers[i] = (Philospher) {
      .eat_time    = eat_time,
      .rand_mean   = rand_mean,
      .rand_stddev = rand_stddev,
      .id          = i,
      .table       = &table
    };
  }
    
  for(int i = 0; i < philo_count; i++) {
    err = pthread_create(&threads[i], NULL, thread_simulate, &philosphers[i]);

    if(err != 0)
      errx(err, "pthread_create: %s", strerror(err));
  }

  for(int i = 0; i < philo_count; i++) {
    err = pthread_join(threads[i], NULL);  

    if(err != 0)
      errx(err, "pthread_join: %s", strerror(err));
  }

  return 0;
}

int main(int argc, char *argv[]){
  if(argc < 5)
    errx(1, "Usage: <eat_time> <rand_mean> <rand_stddev> <philo_count> }");

  int iargs[4];

  for(size_t i = 0; i < 4; i++) {
    if(strtoi(argv[i + 1], &iargs[i]))
      err(1, "%s", argv[i + 1]);

    if(iargs[i] < 0)
      errx(1, "%s: Value must be positive", argv[i + 1]);
  }

  int eat_time    = iargs[0], 
      rand_mean   = iargs[1], 
      rand_stddev = iargs[2], 
      philo_count = iargs[3];

  int err;
  if((err = simulate(eat_time, rand_mean, rand_stddev, philo_count)) != 0) {
    errx(err, "Ran into an unexpected error during simulation: %d", err);
  }

	return 0;
}
