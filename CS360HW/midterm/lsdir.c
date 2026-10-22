#include <err.h>
#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
  if(argc < 2) {
    fprintf(stderr, "%s: usage: lsdir [PATH]", *argv);
    return -1;
  }
  
  DIR* dir = opendir(argv[1]);

  if(dir == NULL)
    err(1, "opendir");

  char cwd[PATH_MAX];
  char* p_cwd = getcwd(cwd, PATH_MAX);

  if(p_cwd == NULL)
    err(1, "getcwd");

  char prefix[PATH_MAX];
  snprintf(prefix, PATH_MAX, "%s/%s", cwd, argv[1]);
  
  errno = 0;  
  struct dirent* entry;
  while((entry = readdir(dir))) {

    if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) 
      continue;

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/%s", prefix, entry->d_name);
    
    struct stat s;
    if(lstat(path, &s))
      printf("%s ??? %c\n", entry->d_name, entry->d_type);
    else 
      printf("%s %lu %c\n", entry->d_name, s.st_size, entry->d_type);
  }
  
  if(errno != 0)
    err(1, "readdir");

  if(closedir(dir))
    err(1, "closedir");
 
}
