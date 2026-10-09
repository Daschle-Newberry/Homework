#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "readable.h"

int main() {

  assert(readable("tests/dir1") == 1);
  assert(readable("tests/dir2") == 1);
  assert(readable("tests/dir3_readonly") < 0);
  assert(readable("tests") == 5);

  return 0;
}
