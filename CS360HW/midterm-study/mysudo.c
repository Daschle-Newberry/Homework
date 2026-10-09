#include <sys/types.h>
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>

int main(int argc, char** argv) {
  if(argc < 2)
    err(1, "args");


  uid_t ruid = getuid();
  struct passwd* pwd = getpwnam("daschle");
  
  if(pwd == NULL)
    err(2, "getpwdnam");

  if(pwd->pw_uid != ruid) {
    fprintf(stderr, "%s: not a valid sudoer", *argv);
    exit(2);
  }

  if(seteuid(0) == -1)
    err(2, "seteuid");
  
  execvp(argv[1], argv + 1);
  
  err(3, "execvp");
}
