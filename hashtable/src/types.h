#ifndef TYPES_H
#define TYPES_H

typedef unsigned long long hash64_t;
typedef hash64_t (*HashFn)(const void* data);

typedef int (*CmpFn)(const void* a, const void* b);
typedef void (*DstrFn)(void* a);

#endif
