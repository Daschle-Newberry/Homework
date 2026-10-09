#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>

int print_error() {
  int err = errno;
  fprintf(stderr, "Error: %s\n", strerror(err));
  return err;
}

int readable_helper(char* path) {
  struct stat s;
  if(lstat(path, &s)) 
    return print_error();
  
  if(S_ISREG(s.st_mode))
    return access(path, R_OK) == 0;
  
  if(!S_ISDIR(s.st_mode))
    return 0;

  DIR* dir = opendir(path);

  if(!dir) {
    if(errno == EACCES)
      return 0;

    return print_error();
  }

  int err = 0;  
  int count = 0;
  struct dirent* d;
  while((d = readdir(dir))) {
    if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
      continue;

    char child[PATH_MAX];
    sprintf(child, "%s/%s", path, d->d_name);
    
    int res = readable_helper(child);

    if(res < 0) {
      err = res;
      break;
    }
    
    count += res;
  }
   
  closedir(dir);
  return err ? err : count;
}

int readable(char* inputPath) {
  char buf[256];
  if(!inputPath) {
    getcwd(buf, 256);
    inputPath = buf;
  }

  struct stat s;

  if(lstat(inputPath, &s)) {
    return -errno;
  }

  DIR* dir = opendir(inputPath);

  if(!dir && !(errno == ENOTDIR))
    return -errno;
  
  if(dir)
    closedir(dir);

  return readable_helper(inputPath);
}





