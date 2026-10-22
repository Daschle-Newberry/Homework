#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
//  Write long file_size(const char *path) using only open, lseek, and close. 
//  Return the size in bytes, or -1 on error. Don't use stat.

long file_size(const char* path) {
  int fd = open(path, O_RDONLY);

  if(fd == -1)
    return -1;

  off_t size = lseek(fd, 0, SEEK_END);
  if(size == -1)
    return -1;

  if(close(fd) == -1)
    return -1;

  return size;
}


int main() {


  return 0;
}
