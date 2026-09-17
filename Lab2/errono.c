#include <stdio.h>
#include <errno.h>
#include <string.h>

int main() {
  FILE* f = fopen("hhhh.txt", "r");
  
  int err = errno;
  fprintf(stderr, "Error Number: %d, Error Message: %s\n", err, strerror(err));

  return errno;
}
