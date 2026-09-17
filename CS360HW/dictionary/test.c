#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include <string.h>

#include "lineNum.h"


int main() {
  assert(lineNum("tiny_9", "aardvark", 9) == 1);
  assert(lineNum("tiny_9", "bear", 9) == 2);
  assert(lineNum("tiny_9", "cat", 9) == 3);
  assert(lineNum("tiny_9", "dog", 9) == 4);
  assert(lineNum("tiny_9", "elephant", 9) == 5);
  assert(lineNum("tiny_9", "featherb", 9) == 6);
  assert(lineNum("tiny_9", "fi sh", 9) == 7);
  assert(lineNum("tiny_9", "guppie", 9) == 8);
  assert(lineNum("tiny_9", "horse", 9) == 9);
  assert(lineNum("tiny_9", "mellow", 9) == 10);

  assert(lineNum("tiny_9", "acid", 9) == -1);
  assert(lineNum("tiny_9", "a b c", 9) == -1);
  assert(lineNum("tiny_9", "youth", 9) == -10);
  assert(lineNum("tiny_9", "000", 9) == -1);

  assert(lineNum("webster_16", "a b c", 16) == 1);
  assert(lineNum("webster_16", "acquiesce", 16) == 180);
  assert(lineNum("webster_16", "algebraic", 16) == 500); 
  assert(lineNum("webster_16", "interior", 16) == 10000);
  assert(lineNum("webster_16", "zoo", 16) == 20422);
 
  return 0;
}
