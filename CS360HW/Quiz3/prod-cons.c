#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <err.h>
#include <sys/sem.h>

struct SBuffer {
  int buffer[5];
  int top;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(int argc, char** argv) {
  
  int shmid = shmget(IPC_PRIVATE, sizeof(struct SBuffer), IPC_CREAT | 0600);
  
  struct SBuffer* buff = shmat(shmid, NULL, 0);
  buff->top = 0;


  int semid = semget(IPC_PRIVATE, 3, 0600);
  struct sembuf wait_space = {0, -1, 0};
  struct sembuf wait_data = {1, -1, 0};
  struct sembuf lock = {2, -1, 0};

  struct sembuf signal_space = {0, 1, 0};
  struct sembuf signal_data = {1, 1, 0};
  struct sembuf unlock = {2, 1, 0};

  union semun val;
  
  unsigned short init_vals[] = {5, 0, 1};
  val.array = init_vals;
  semctl(semid, 0, SETALL, val);

  pid_t pid;
  int idx = 0;

  for(; idx < 20 && (pid = fork()) != 0; idx++);


  if(pid == 0 && idx < 10) {
    // Producer
    for(int i = 0; i < 20; i++) {
      semop(semid, &wait_space, 1);
      semop(semid, &lock, 1);
      
      buff->buffer[buff->top] = i;
      buff->top++;

      printf("Proc %d produced %d\n", idx, i);
      semop(semid, &signal_data, 1);
      semop(semid, &unlock, 1);
    }

    exit(0);
  }

  if(pid == 0 && idx >= 10) {
    for(int i = 0; i < 20; i++) {
      semop(semid, &wait_data, 1);
      semop(semid, &lock, 1);
      
      int i = buff->buffer[buff->top - 1];
      buff->top--;
      printf("Proc %d consumed %d\n", idx, i);

      semop(semid, &signal_space, 1);
      semop(semid, &unlock, 1);
    }

  }


  if(pid != 0) {if (pid > 0) {
    for (int i = 0; i < 20; i++)
        wait(NULL);
    }
  }

  return  0;
}


