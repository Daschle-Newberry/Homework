#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

void read_write_byte(int fd, char c) {
  if(read(fd, &c, 1)) {
  
    if(c == '\n')
      printf("\\n\n");
    else
      printf("%c", c);
    read_write_byte(fd, c);
  }
}

int main() {
  char* file = "test.txt";
  int fd = open(file, O_RDONLY);
  read_write_byte(fd, 0);
}
