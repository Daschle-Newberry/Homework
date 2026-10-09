/***********************************************************************
name:
	assignment4 -- acts as a pipe using ":" to seperate programs.
description:	
	See CS 360 Processes and Exec/Pipes lecture for helpful tips.
***********************************************************************/

/* Includes and definitions */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

void err(int code, char* prefix) {
  int err = errno;
  // Prefix not used here because autolab
  fprintf(stdout, "%s\n", strerror(errno));
  exit(code);
}

int main(int argc, char *argv[]){
  if(argc < 2) {
    fprintf(stdout, "Error: Usage: <proc1> : <proc>\n");
    return -1;
  }

  int colon = -1;
  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], ":") == 0) {
      colon = i;
      break;
    }
  }
  
  char** l_argv = NULL;
  char** r_argv = NULL;
  int l_argc = 0;
  int r_argc = 0;

  if(colon == -1) {
    l_argv = argv + 1;
    l_argc = argc - 1;
  } else {
    argv[colon] = NULL;
    l_argv = argv + 1;
    r_argv = argv + colon + 1;
    
    l_argc = colon - 1;
    r_argc = argc - colon - 1;
  }

  // No args
  if(l_argc == 0 && r_argc == 0) {
    return EXIT_SUCCESS;
  }

  // One program case
  if(l_argc == 0 || r_argc == 0) {
    char** p_argv = l_argc == 0 ? r_argv : l_argv;
    
    if(execvp(*p_argv, p_argv) == -1)
      err(EXIT_FAILURE, "execvp");
  }

  int fd[2];
  if(pipe(fd) == -1)
    err(EXIT_FAILURE, "pipe");

  int rd = fd[0], wr = fd[1];

  pid_t pid = fork();
  if(pid == -1)
    err(EXIT_FAILURE, "fork");

  if(pid) {
    // Consumer (parent)
    if(close(wr) == -1)
      err(EXIT_FAILURE, "close");

    if(dup2(rd, STDIN_FILENO) == -1)
      err(EXIT_FAILURE, "dup2");

    if(execvp(*r_argv, r_argv) == -1)
      err(EXIT_FAILURE, "execvp");

  } else {
    // Producer (child)
    if(close(rd) == -1)
      err(EXIT_FAILURE, "close");

    if(dup2(wr, STDOUT_FILENO) == -1)
      err(EXIT_FAILURE, "dup2");

    if(execvp(*l_argv, l_argv) == -1)
      err(EXIT_FAILURE, "execvp");
  }
}
