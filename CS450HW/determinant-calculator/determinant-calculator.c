/*
 * Author: Daschle Newberry
 *
 * Note: I did both Laplace expansion and row reduction. My Laplace 
 *       expansion implementation prevents data copies for each minor
 *       by copying only a small size N - 1 buffer of valid rows/cols.
 *
 *       However, Laplace expansion still (obviously) sucks. And I carried 
 *       over the row/col logic into my row reduction implementation, this is
 *       why you see me using mat_get and mat_set instead of inlining.
 * */



#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static char* MATRIX_ERROR_STRINGS[] = 
  { 
    "M_NONE", 
    "M_NOMEM",
    "M_OUTOFRANGE",
    "M_INVALIDARG"
  };

typedef enum {
  M_NONE,
  M_NOMEM,
  M_OUTOFRANGE,
  M_INVALIDARG
} MatrixError;


typedef struct {
  size_t refs;    //< Reference count
  size_t n;       //< Length of data
  double data[];  //< Data
} matstorage;

typedef struct {
  size_t r;             //< Rows
  size_t c;             //< Cols
  
  size_t original_c;   //< Original column count

  matstorage* storage; //< Shared storage

  size_t* rows;        //< The rows of the original matrix this matrix contains
  size_t* cols;        //< The cols of the original matrix tis matrix contains
} mat2d;

// @brief creates a matrix with no storage and uninitialized row/cols
mat2d* mat_create_empty(size_t n, size_t m, MatrixError* err) {
  mat2d* matrix = malloc(sizeof(*matrix) + ((n + m)) * sizeof(size_t));
  if(!matrix) {
    if(err) *err = M_NOMEM;
    return NULL;
  }

  *(matrix) = (mat2d) {
    .r = n,
    .c = m,
    .rows = (size_t*) (matrix + 1),
    .cols = (size_t*) (matrix + 1) + n
  };

  if(err) *err = M_NONE;
  return matrix;
}

// @brief creates a matrix size N x M with uninitialized data
mat2d* mat_create(size_t n, size_t m, MatrixError* err) { 
  mat2d* matrix = mat_create_empty(n, m, err);
  
  if(!matrix)
    return NULL;

  matstorage* storage = malloc(sizeof(*storage) + (n * m * sizeof(double)));
  
  if(!storage) {
    free(matrix);
    if(err) *err = M_NOMEM;
    return NULL;
  }

  // Initialize matrix to have all rows/cols
  for(size_t i = 0; i < n; i++) {
    matrix->rows[i] = i;
  }

  for(size_t j = 0; j < m; j++) {
    matrix->cols[j] = j;
  }

  storage->refs = 1;
  storage->n = n * m;
  matrix->storage = storage;
  matrix->original_c = m;

  return matrix;
}

// @brief creates a matrix size N x M and reads values from STDIN
mat2d* mat_read(size_t n, size_t m, MatrixError* err) {
  mat2d* matrix = mat_create(n, m, err);

  if(!matrix)
    return NULL;

  // Bad, no overflow checks
  for(size_t i = 0; i < (n*m); i++)
    scanf("%lf", &matrix->storage->data[i]);
 
  if(err) *err = M_NONE;
  return matrix;
}
// @brief creates a deepy copy of the given matrix   
mat2d* mat_deep_copy(const mat2d* matrix, MatrixError* err) {
  mat2d* copy = mat_create(matrix->r, matrix->c, err);

  if(!copy)
    return NULL;
  
  memcpy(copy->storage->data, matrix->storage->data, matrix->storage->n * sizeof(*matrix->storage->data));
  memcpy(copy->rows, matrix->rows, matrix->r);
  memcpy(copy->cols, matrix->cols, matrix->c);

  if(err) *err = M_NONE;
  return copy;
}

// @brief creates a new matrix that is the minor of original matrix given
//        we eliminate row and col
mat2d* mat_minor(const mat2d* matrix, size_t row, size_t col, MatrixError* err) {
  
  // Create empty matrix (no rows/cols, no storage)
  mat2d* minor = mat_create_empty(matrix->r - 1, matrix->c - 1, err);
  minor->storage = matrix->storage;
  minor->storage->refs++;
  minor->original_c = matrix->original_c;

  // Splice out row and column
  memcpy(minor->rows, matrix->rows, row * sizeof(*matrix->rows));
  memcpy(
      minor->rows + row, 
      matrix->rows + row + 1, 
      (matrix->r - row - 1) * sizeof(*matrix->rows)
      );


  memcpy(minor->cols, matrix->cols, col * sizeof(*matrix->cols));
  memcpy(
      minor->cols + col, 
      matrix->cols + col + 1, 
      (matrix->c - col - 1) * sizeof(*matrix->cols)
      );

  return minor;
}

// @brief frees the given matrix, freeing storage if it was the last reference
void mat_free(mat2d* matrix) {
  if(matrix->storage->refs == 1) {
    free(matrix->storage);
  } else {
    matrix->storage->refs--;
  }

  free(matrix);
}

// @brief returns the value at row and col in the given matrix
double mat_get(const mat2d* matrix, size_t row, size_t col, MatrixError* err) {
  size_t logical_r = matrix->rows[row];
  size_t logical_c = matrix->cols[col];
  
  size_t idx = logical_r * matrix->original_c + logical_c;
  if(idx >= matrix->storage->n) {
    if(err) *err = M_OUTOFRANGE;
    return 0;
  }

  return matrix->storage->data[idx];
}

// @brief sets the value at row and col in the given matrix
void mat_set(mat2d* matrix, double value, size_t row, size_t col, MatrixError* err) {
  size_t logical_r = matrix->rows[row];
  size_t logical_c = matrix->cols[col];
  
  size_t idx = logical_r * matrix->original_c + logical_c;
  if(idx >= matrix->storage->n) {
    if(err) *err = M_OUTOFRANGE;
    return;
  }

  matrix->storage->data[idx] = value;
}

// @brief prints the given matrix to STDOUT
void mat_print(const mat2d* matrix) {
  for(size_t r = 0; r < matrix->r; r++) {
    for(size_t c = 0; c < matrix->c; c++) {
      printf("%lf ", mat_get(matrix, r, c, NULL));
    }
    puts("");
  }
}

// @brief determinant calculation using laplace expansion, O(N!)
double mat_det_laplace_helper(const mat2d* matrix, MatrixError* err) {
  // Base case: 2x2 matrix
  if(matrix->r == 2) {
    return mat_get(matrix, 0, 0, NULL) * mat_get(matrix, 1, 1, NULL) - 
           mat_get(matrix, 0, 1, NULL) * mat_get(matrix, 1, 0, NULL);
  }

  double accum = 0;
  int sign = 1;
  
  // Each cofactor and minor combination
  for(int i = 0; i < matrix->c; i++) {
    mat2d* minor = mat_minor(matrix, i, 0, NULL);
    accum += sign * mat_get(matrix, i, 0, NULL) * mat_det_laplace_helper(minor, err);
    sign *= -1;
  }

  return accum;
}

// @brief computes the determinant of the given matrix using laplace expansion
double mat_det_laplace(const mat2d* matrix, MatrixError* err) {
  if(matrix->r != matrix->c) {
    if(err) *err = M_INVALIDARG;
    return 0;
  }

  return mat_det_laplace_helper(matrix, err);
}

// @brief swaps r1 with r2 in the given matrix
void mat_swap_row(mat2d* matrix, size_t r1, size_t r2, MatrixError* err) {
  if(r1 > matrix->r || r2 > matrix->r) {
    if(err) *err = M_INVALIDARG;
    return;
  }

  for(size_t c = 0; c < matrix->c; c++) {
    double v1 = mat_get(matrix, r1, c, NULL);
    double v2 = mat_get(matrix, r2, c, NULL);

    mat_set(matrix, v1, r2, c, NULL);
    mat_set(matrix, v2, r1, c, NULL);
  }
}
// @brief computes the determinant of the given matrix using row reduction
double mat_det_row_reduc(const mat2d* matrix, MatrixError* err) {
  if(matrix->r != matrix->c) {
    if(err) *err = M_INVALIDARG;
    return 0;
  }
  
  // Could technically be unneeded
  mat2d* m = mat_deep_copy(matrix, err);
  
  if(!m)
    return 0;

  size_t n = matrix->r;
  double accum = 1;
  for(int pivot = 0; pivot < n; pivot++) {
    double pivot_val = mat_get(m, pivot, pivot, NULL);
   
    // Swap row with first valid with valid pivot
    if(pivot_val == 0) {
      for(int row = pivot + 1; row < n; row++) {
        if((pivot_val = mat_get(m, row, pivot, NULL))) {
          mat_swap_row(m, pivot, row, NULL);
          accum *= -1;
          break;
        };
      }
      
      // No pivot found, det = 0
      if(pivot_val == 0) {
        accum = 0;
        break;
      }
    }

    accum *= pivot_val;

    // Row reduce
    for(int row = pivot + 1; row < n; row++) { 
      double current = mat_get(m, row, pivot, NULL);
      double scalar = current/pivot_val;

      for(int col = 0; col < n; col++) {
        double ad1 = -scalar * mat_get(m, pivot, col, NULL);
        double ad2 = mat_get(m, row, col, NULL);
        mat_set(m, ad1 + ad2, row, col, NULL);
      }
    }
  }

  mat_free(m);
  return accum;
}


int main() {
  int n;
  scanf("%d", &n);
  MatrixError err;
  mat2d* matrix = mat_read(n, n, &err);

  if(!matrix) {
    printf("%s\n", MATRIX_ERROR_STRINGS[err]);
    return -1;
  }
  printf("%lf\n", mat_det_row_reduc(matrix, NULL));

  return 0;
}
