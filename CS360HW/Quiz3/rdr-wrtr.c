#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <err.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(int argc, char** argv) {
   
  int shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0600);
  int* svals = shmat(shmid, NULL, 0);
  
  int* bal = svals;
  int* read_count = svals + 1;


  *bal = 0;
  *read_count = 0;

  int semid = semget(IPC_PRIVATE, 2, 0600);
  
  struct sembuf lock =   { 0, -1, 0 };
  struct sembuf unlock = { 0,  1, 0 };
  
  struct sembuf enter_room= { 1, -1, 0 };
  struct sembuf leave_room   = { 1, 1, 0 };
 
  union semun val;

  val.val = 1;
  semctl(semid, 0, SETVAL, val);
  semctl(semid, 1, SETVAL, val);
  int idx = 0;
  pid_t pid;
  for(; idx < 8 && (pid = fork()) != 0; idx++);
  

  if(pid == 0 && idx < 5) {
    while(1) {
      // Writers
      semop(semid, &enter_room, 1);
      printf("Writer setting bal\n");
      *bal = rand();
      semop(semid, &leave_room, 1);
    }
  }

  if(pid == 0 && idx >= 5) {
    // Readers
    while(1) {
      semop(semid, &lock, 1);
      
      *read_count = *read_count + 1;

      if(*read_count == 1)
        semop(semid, &enter_room, 1);
      
      semop(semid, &unlock, 1);

      printf("Reader %d, Balance: %d\n", idx, *bal);

      semop(semid, &lock, 1);
      *read_count = *read_count - 1;

      if(*read_count == 0)
        semop(semid, &leave_room, 1);
    
      semop(semid, &unlock, 1);
    }
  }

  
  if(pid != 0) {
    for(int i = 0; i < 8; i++) {
      wait(NULL);
    }
  }

  return 0;
}
