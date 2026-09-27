#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

void print_fork(const char* word) {
  for(int i = 0; i < strlen(word); fork() ? wait(NULL) : (putchar(word[i]) ? exit(0) : exit(0)), i++);
}


int main() {

  print_fork("cheese");
  return 0;
}
