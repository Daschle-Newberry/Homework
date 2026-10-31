#include <bits/time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <time.h>
#include <err.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

union semun {
    int val;                 
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;    
};

int randomGaussian(int mean, int stddev); 
int strtoplong(char* str, long* res); 
int sem_set_val(int semid, unsigned short val, long count); 
void tsleep(long seconds);
int simulate(
    long eat_time, 
    long rand_mean, 
    long rand_stddev, 
    long philo_count
);

int main(int argc, char *argv[]){
  if(argc < 5)
    errx(1, "Usage: <eat_time> <rand_mean> <rand_stddev> <philo_count> }");

  long largs[4];

  for(size_t i = 0; i < 4; i++) {
    if(strtoplong(argv[i + 1], &largs[i])) {
      err(1, "%s", argv[1]);
    }
  }

  long eat_time    = largs[0], 
       rand_mean   = largs[1], 
       rand_stddev = largs[2], 
       philo_count = largs[3];

  if(simulate(eat_time, rand_mean, rand_stddev, philo_count))
    err(1, " ");

	return 0;
}

/* successive calls to randomGaussian produce integer return values */
/* having a gaussian distribution with the given mean and standard  */
/* deviation.  Return values may be negative.                       */
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

// String to positive long
int strtoplong(char* str, long* res) {
    if(str == NULL || res == NULL) {
      errno = EINVAL;
      return -1;
    }

    char* endptr = NULL;
    long num = strtol(str, &endptr, 10);

    // Enforce positive long
    if(num == LONG_MAX || num < 0)
      return -1;
  
    // Enforce entire string to be included in conversion
    if((size_t)(endptr - str) != strlen(str)){
      errno = EINVAL;
      return -1;
    }
    
    *res = num;
    return 0;

}

// Set all values of the semaphores
int sem_set_val(int semid, unsigned short val, long count) {
  unsigned short vals[count];
  for(long i = 0; i < count; i++) vals[i] = val;

  union semun args;
  args.array = vals;

  if(semctl(semid, 0, SETALL, args) == -1)
    return -1;

  return 0;
}

// True sleep, ensures the process sleeps for the entire time
void tsleep(long seconds) {
  while(seconds > 0) {
    seconds = sleep(seconds);
  }
}

// Simulate the philospher problem
int simulate(
    long eat_time, 
    long rand_mean, 
    long rand_stddev, 
    long philo_count
) {
  int semid = semget(IPC_PRIVATE, philo_count, 0600);
  
  if(semid == -1)
    return -1;
  
  // Initialize all semaphores to 1
  if(sem_set_val(semid, 1, philo_count) == -1)
    return -1;

  struct sembuf c_grab[2] = {{0, -1, 0}, {1, -1, 0}};
  struct sembuf c_rel[2]  = {{0, 1, 0}, {1, 1, 0}};

  // Fork children
  long idx = 0;
  pid_t pid;
  for(; idx < philo_count && (pid = fork()) != 0; idx++); 

  // Seed random for children
  struct timespec t;
  if(clock_gettime(CLOCK_MONOTONIC, &t) == -1)
    exit(1);

  srand(t.tv_nsec);

  c_grab[0].sem_num = idx;
  c_grab[1].sem_num = (idx + 1) % philo_count;
 
  c_rel[0].sem_num =  idx;
  c_rel[1].sem_num =  (idx + 1) % philo_count;

  if(pid == 0) {
    long eat_remain = eat_time;

    while(eat_remain != 0) {
      
      int cur_think = randomGaussian(rand_mean, rand_stddev);
      int cur_eat   = randomGaussian(rand_mean, rand_stddev);

      cur_think = MAX(cur_think, 0);
      cur_eat   = MIN(MAX(cur_eat, 0), eat_remain);
      
      printf("philosopher %ld is thinking for %d seconds\n", idx, cur_think);
      
      tsleep(cur_think);

      // Wait for chopsticks to be available
      if(semop(semid, c_grab, 2) == -1) 
        exit(1);

      // CRITICAL SECTION - START
      printf("philosopher %ld is eating for %d seconds\n", idx, cur_eat);
      tsleep(cur_eat);
      // CRITICAL SECTION - END
      
      // Release chopsticks
      if(semop(semid, c_rel, 2))
        exit(1);

      eat_remain -= cur_eat;
    }
    printf("philosopher %ld ate for %ld seconds and is leaving the table\n", idx, eat_time);
    
    exit(0);
  }

  // Wait for children to exit and monitor return values
  int err = 0;
  int status;
  while(wait(&status) != -1) {
    err |= status != 0;
    if(status != 0)
      warn("Child process exited with non-zero exit code: %d\n", err);
  }

  // Release semaphores
  if(semctl(semid, 0, IPC_RMID) == -1)
    err = -1;

  return err;
}
