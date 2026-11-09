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
  
  int max_clients = 3;
  int* num_clients = shmat(shmid, NULL, 0);
  int* handled_clients = num_clients + 1;
  *num_clients = 0;
  *handled_clients = 0;


  int semid = semget(IPC_PRIVATE, 2, 0600);
  
  struct sembuf awake_barber = {0, 1, 0};
  struct sembuf barber_wakeup = { 0, -1, 0};

  struct sembuf lock = {1, -1 , 0};
  struct sembuf unlock = {1, 1, 0};

  int idx = 0;
  pid_t pid;
  for(;idx < 9 && (pid = fork()) != 0; idx++);


  if(idx == 0 && pid == 0) {
    // Barber
    
    while(1) {
      semop(semid, &barber_wakeup, 1);
      semop(semid, &lock, 1);
      
      if(*handled_clients == 8) {
        // Finished, go home
        printf("Barber is leaving...\n");
        break;
      }
      if(*num_clients == 0) {
        // No clients, go back to sleeping
        semop(semid, &unlock, 1);
        continue;
      }
      
      *num_clients = *num_clients + 1;
      *handled_clients = *handled_clients + 1;
      semop(semid, &unlock, 1);
  

      // Cut hair
      printf("Barber is cutting hair\n...");
      sleep(3);
      printf("Barber finished cutting hair\n");
    } 
  
    exit(0);
  }

  if(idx > 0 && pid == 0) {
    // Client
    semop(semid, &lock, 1);

    if(*num_clients == max_clients) {
      printf("Client sees barbershop is full, leaving...\n");
      *handled_clients = *handled_clients + 1;
      semop(semid, &unlock, 1);
      exit(0);
    }

    *num_clients = *num_clients + 1;
    semop(semid,)




  }


  for(int i = 0; i < 9; i++) {
    wait(NULL);
  }

}
