#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void run_ls() {

  pid_t pid = fork();

  if(pid == -1) {
    perror("fork");
    return;
  }

  if(pid) {
    waitpid(pid, NULL, 0);
    printf("done\n");
  } else {
    if(execlp("ls", "ls", "-l", NULL)) {
      perror("exec");
      exit(-1);
    }
  }

}

int main() {

  run_ls();
  return 0;
}
