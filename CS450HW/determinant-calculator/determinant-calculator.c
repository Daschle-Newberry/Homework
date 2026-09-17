#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct mat2d mat2d;
mat2d* get_matrix(size_t n, size_t m);

struct mat2d {
  size_t n;
  size_t m;
  double data[];
};

int main() {
  
  int n;
  scanf("%d", &n);
  
  mat2d* matrix = get_matrix(n, n);

  return 0;
}

mat2d* get_matrix(size_t n, size_t m) {
  mat2d* matrix = malloc(sizeof(mat2d) + (n * m) * sizeof(double));

  *(matrix) = (mat2d) {
    .n = n,
    .m = m,
  };
  
  // No oveflow checks
  for(size_t i = 0; i < (n*m); i++) {
    scanf("%lf", &matrix->data[i]);
  }

  return matrix;
}
