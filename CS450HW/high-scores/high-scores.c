#include <string.h>
#include <stdio.h>
#include <err.h>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define VECTOR_INIT_CAP 10280
#define NUM_SKILLS 5
#define NUM_LISTS 6
#define XP_THRESH 10000
#define XP_PROB .95

static char* LIST_NAMES[NUM_LISTS] = { 
    "SKILL_BREAKDANCING", 
    "SKILL_APICULTURE", 
    "SKILL_BASKET", 
    "SKILL_XBASKET", 
    "SKILL_SWORD",
    "TOTAL_XP"
};

typedef struct Int32Vector {
  size_t len;
  size_t cap;
  int32_t* data;
} Int32Vector;

typedef int (*I32VectorSorter)(Int32Vector* vector);
int i32vec_init(Int32Vector* vector, size_t capacity);
int i32vec_resize(Int32Vector* vector, size_t size);
int i32vec_push_back(Int32Vector* vector, int32_t value);
int i32_cmp(const void* a, const void* b);
int i32vec_sort_std(Int32Vector* vector);
int i32vec_sort_custom(Int32Vector* vector);
void cnsort_min_max(size_t n, const int32_t data[n], int32_t res[2]);
void cnsort(size_t n, int32_t data[n]);
void i32vec_print(const Int32Vector* vector, char delim);
void i32vec_destroy(Int32Vector* vector);

void nerr(int code, char* reason);
int init_lists(size_t n, Int32Vector lists[n]);
int populate_skills(size_t n, Int32Vector* lists);
int compute_total(size_t n, const Int32Vector lists[n], Int32Vector* total);

I32VectorSorter get_sorter(char* mode);
int sort_lists(size_t n, Int32Vector lists[n], I32VectorSorter sorter);
void print_lists(size_t n, const Int32Vector lists[n]);
void destroy_lists(size_t n, Int32Vector lists[n]);

int main(int argc, char** argv) {
  clock_t start = clock();
  setvbuf(stdin, NULL, _IOFBF, 0x100000);
  setvbuf(stdout, NULL, _IOFBF, 0x100000); 

  Int32Vector lists[NUM_LISTS];
  
  if(init_lists(NUM_LISTS, lists))
    nerr(1, "init_lists");

  if(populate_skills(NUM_SKILLS, lists))
    nerr(1, "populate_skills");

  if(compute_total(NUM_SKILLS, lists, &lists[NUM_SKILLS]))
    nerr(1, "compute_total");

  char* mode = argc == 2 ? argv[1] : "standard";
  I32VectorSorter sorter = get_sorter(mode);
  
  if(sort_lists(NUM_LISTS, lists, sorter))
    nerr(1, "sort_lists");

  print_lists(NUM_LISTS, lists);

  destroy_lists(NUM_LISTS, lists);

  clock_t end = clock();
  double time_elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;

  fprintf(stderr, "%s took %f seconds to execute.\n", mode, time_elapsed);
  return 0;
}

// Below are my sorting functions, put at the top for convenience 
int i32_cmp(const void* a, const void* b) {
  return *(int32_t*)b - *(int32_t*)a;
}

int i32vec_sort_std(Int32Vector* vector) {
  if(!vector)
    return 1;

  qsort(vector->data, vector->len, sizeof(*vector->data), i32_cmp);
  return 0;
}

int i32vec_sort_custom(Int32Vector* vector) {
  int status = 0;
  size_t bottom_size = vector->len * XP_PROB;
  size_t top_size = vector->len - bottom_size;

  // Users with XP < XP_THRESH
  Int32Vector bottom;

  // Users with XP > XP_THRESH
  Int32Vector top;

  if(i32vec_init(&bottom, bottom_size) || i32vec_init(&top, top_size))
    return 1;
    
  for(size_t i = 0; i < vector->len; i++) {
    Int32Vector* bucket = vector->data[i] > XP_THRESH ? &top : &bottom;
    
    if(i32vec_push_back(bucket, vector->data[i])) {
      status = 1;
      goto end;
    }        
  }

  cnsort(bottom.len, bottom.data);
  if(i32vec_sort_std(&top)) {
    status = 1;
    goto end;
  }
  
  for(size_t i = 0; i < top.len; i++) vector->data[i] = top.data[i];
  for(size_t i = 0; i < bottom.len; i++) vector->data[i + top.len] = bottom.data[i];

end:
  i32vec_destroy(&bottom);
  i32vec_destroy(&top);
  return status;
}

void cnsort_min_max(size_t n, const int32_t data[n], int32_t res[2]) {
  res[0] = data[0];
  res[1] = data[0];

  for(size_t i = 1; i < n; i++) {
    res[0] = res[0] < data[i] ? res[0] : data[i];
    res[1] = res[1] > data[i] ? res[1] : data[i];
  }
}

void cnsort(size_t n, int32_t data[n]) {
  int32_t minmax[2];
  cnsort_min_max(n, data, minmax);
  
  int32_t min = minmax[0], max = minmax[1];
  int32_t range = max - min + 1;

  size_t counts[range];
  memset(counts, 0, sizeof(*counts) * range); 
  
  // Count
  for(size_t i = 0; i < n; i++) {
    int32_t idx = data[i] - min;
   
    assert(idx < range && idx >= 0);
    
    counts[idx]++;
  }
  // Fill
  size_t offset = 0;
  for(int32_t i = 0; i < range; i++) {
    for(size_t j = 0; j < counts[i]; j++) {
      data[offset + j] = i + min;
    }
    offset += counts[i];
  } 
}


int i32vec_init(Int32Vector* vector, size_t capacity) {
  if(!vector || capacity < 1)
    return -1;
  
  int32_t* data = malloc(sizeof(*vector->data) * capacity);
  
  if(!data)
    return -1;

  vector->len = 0;
  vector->cap = capacity;
  
  vector->data = data;
  return 0;
}

int i32vec_resize(Int32Vector* vector, size_t size) {
  if(!vector)
    return 1;
  
  if(size > vector->cap) {
    size_t new_cap = vector->cap;
    while(new_cap < size) new_cap *= 2;
    
    int32_t* new_data = realloc(vector->data, sizeof(*new_data) * new_cap);

    if(!new_data)
      return 1;

    vector->cap = new_cap;
    vector->data = new_data;
  }
  
  vector->len = size;

  return 0;
}

int i32vec_push_back(Int32Vector* vector, int32_t value) {
  if(!vector)
    return 1;

  if(vector->len == vector->cap) {
    size_t new_cap = vector->cap * 2;
    int32_t* new_data = realloc(vector->data, new_cap * sizeof(*new_data));
    if(!new_data)
      return -1;
    
    vector->cap = new_cap;
    vector->data = new_data;
  }

  vector->data[vector->len] = value;
  vector->len++;

  return 0;
}

void i32vec_print(const Int32Vector* vector, char delim) {
  for(size_t i = 0; i < vector->len; i++)
    printf("%d%c", vector->data[i], delim);
}

void i32vec_destroy(Int32Vector* vector) {
  free(vector->data);
}

// ============= HELPER FUNCTIONS ============= 
void nerr(int code, char* reason) {
  fprintf(stderr, "%s\n", reason);
  exit(code);
}

int init_lists(size_t n, Int32Vector lists[n]) {  
  if((n == 0) || !lists)
    return 1;

  for(size_t i = 0; i < n; i++) {
    if(i32vec_init(&lists[i], VECTOR_INIT_CAP))
      nerr(1, "i32vec_init");
  }

  return 0;
}

int populate_skills(size_t n, Int32Vector* lists) {
  if((n == 0) || !lists)
    return 1;

  int32_t val;
  size_t i = 0;
  while(scanf("%d", &val) == 1) {
    if(i32vec_push_back(&lists[i], val))
      nerr(1, "i32vec_push_back");
    
    i = (i + 1) % n;
  }

  return 0;
}

int compute_total(size_t n, const Int32Vector lists[n], Int32Vector* total) {
  if((n == 0) | !lists | !total)
    return 1;
  
  size_t num_entries = lists[0].len;

  if(i32vec_resize(total, num_entries))
    return 1;

  // Set to 0 beforehand so we can add each skills values 
  // at once for cache efficiency
  memset(total->data, 0, total->len * sizeof(*total->data));
  
  for(size_t l = 0; l < n; l++) {
    for(size_t i = 0; i < num_entries; i++)
     total->data[i] += lists[l].data[i];
  }

  return 0;
}

I32VectorSorter get_sorter(char* mode) {
  return strcmp(mode, "standard") ? i32vec_sort_custom : i32vec_sort_std;
}

int sort_lists(size_t n, Int32Vector lists[n], I32VectorSorter sorter) {
  for(size_t l = 0; l < n; l++)
    if(sorter(&lists[l]))
      return 1;
  return 0;
}

void print_lists(size_t n, const Int32Vector lists[n]) {
  for(size_t i = 0; i < n; i++) {
    printf("%s\n", LIST_NAMES[i]);
    i32vec_print(&lists[i], '\n');
    printf("\n");
  }
}

void destroy_lists(size_t n, Int32Vector lists[n]) {
  for(size_t l = 0; l < n; l++)
    i32vec_destroy(&lists[l]);
}

