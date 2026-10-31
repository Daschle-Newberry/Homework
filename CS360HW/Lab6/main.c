#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void print(char* str, int x) {
  int id = semget(IPC_PRIVATE, x, 0600);

  struct sembuf op = {0, 1, 0};
  semop(id, &op, 1);

  size_t s_len = strlen(str);
  int idx = 0;
  pid_t pid;

  for(; idx < x && (pid = fork()) != 0; idx++); 

  if(pid == 0) {
    for(; idx < s_len; idx += x) {
      op = (struct sembuf) { idx % x, -1, 0 };
      semop(id, &op, 1);
      putchar(str[idx]);
      fflush(stdout);
      op = (struct sembuf) { (idx + 1) % x, 1, 0 };
      semop(id, &op, 1);
    }
    exit(0);
  }

  // Parent
  while(wait(NULL) != -1);
}



int main(int argc, char** argv) {
  print("cheese", 3);
  return 0;
}
