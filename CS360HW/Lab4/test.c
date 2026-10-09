#include <linux/limits.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

int main() {
    
  DIR* dir = opendir("./tests/");

  if(!dir) {
    printf("%s\n", strerror(errno));
    return 0;
  }
  struct dirent* d;

  while((d = readdir(dir))) {
    char* type;

    switch(d->d_type) {
      case(DT_DIR): {type = "DIR"; break;}
      case(DT_REG): {type = "REG"; break;}
      default: {type = "UNK";}
    }

    printf("%lu %s %s\n", d->d_ino, type, d->d_name);

    struct stat s;

    char buf[PATH_MAX];
    sprintf(buf, "./tests/%s", d->d_name);

    int err = lstat(buf, &s);

    if(err) {
      printf("Error from lstat on '%s': %s\n", buf, strerror(errno));
    }
  
  }

  return 0;
}



