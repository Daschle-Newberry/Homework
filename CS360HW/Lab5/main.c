#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main() {
  int fs[2];
  int sf[2];

  pipe(fs);
  pipe(sf);

  int rd1 = fs[0], wd1 = fs[1];
  int rd2 = sf[0], wd2 = sf[1];

  char* word = "cheese";

  int index = 0;
  
  
  // first branch -> print, then signal other process, then wait
  // second branch -> wait, then print then signal other process
  

  if(fork()) {
    for(int index = 0; index < strlen(word); index += 2) {
      printf("%c", word[index]);
      fflush(stdout);
      write(wd1, ".", 1);
      char c;
      read(rd2, &c, 1);
    }
  } else {
    for(int index = 1; index < strlen(word); index += 2) {
      char c;
      read(rd1, &c, 1);
      printf("%c", word[index]);
      fflush(stdout);
      write(wd2, &c, 1);
    }
  }
}
