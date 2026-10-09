#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc < 3)
      return -1;

    char* src = argv[1];
    char* dest = argv[2];

    int rd, wr;

    if((rd = open(src, O_RDONLY)) == -1) {
      perror("");
      return 1;
    }

    if((wr = open(dest, O_WRONLY | O_CREAT, 0644)) == -1) {
      perror("");
      return 2;
    }

    char buf[4096];
    
    int bytes;
    while((bytes = read(rd, buf, 4096)) > 0) {
      if(write(wr, buf, bytes) == -1) {
        perror("");
        return 3;
      }
      printf("Wrote %d bytes", bytes);
  }

    if(bytes == -1)
      return 3;

  return 0;
}


