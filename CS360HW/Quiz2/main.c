#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
int main() {
  int inputfd = open("in.txt", O_RDONLY, 0);
  int outputfd = open("out.txt", O_WRONLY, 0);

  off_t size = lseek(inputfd, 0, SEEK_END);
  
  for(off_t i = size - 1; i >= 0; i--) {
    lseek(inputfd, i, SEEK_SET);
    char c;
    read(inputfd, &c, 1);
    write(outputfd, &c, 1);
  }

  close(inputfd);
}
