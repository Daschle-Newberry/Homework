#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <sys/param.h>

#define WEIGHT_CAP 1200
#define N_ITEM_CAP  128

typedef struct _Item {
  char name[129];
  uint32_t val;
  uint32_t weight;
} Item;


int knapsack(
    uint32_t n, 
    uint32_t capacity, 
    Item* restrict items, 
    uint32_t dp[N_ITEM_CAP + 1][WEIGHT_CAP + 1]
) {
  for(uint32_t i = 1; i <= n; i++) {
    for(uint32_t w = 1; w <= capacity; w++) {
      Item* item = &items[i - 1];
      if(item->weight > w)
        dp[i][w] = dp[i - 1][w];
      else
        dp[i][w] = MAX(dp[i - 1][w], dp[i - 1][w - item->weight] + item->val);
    }
  }  
  
  return dp[n][capacity];
}

int knapsack_traceback(
    int n, 
    int capacity, 
    Item* restrict items, 
    uint32_t dp[N_ITEM_CAP + 1][WEIGHT_CAP + 1]
) {
  if(n == 0 || capacity == 0)
    return 0;
  
  if(dp[n - 1][capacity] != dp[n][capacity]) {
    Item* item = &items[n - 1];
    int weight = knapsack_traceback(n - 1, capacity - item->weight, items, dp);
    printf("%s, %d, %d\n", item->name, item->weight, item->val);
    return weight + item->weight;
  } else {
    return knapsack_traceback(n -1, capacity, items, dp);
  }
}

int main(void) {
  Item items[N_ITEM_CAP];
  static uint32_t dp[N_ITEM_CAP + 1][WEIGHT_CAP + 1] = {{}};
  uint32_t capacity;

  if(scanf("%d\n", &capacity) != 1)
    errx(1, "No capacity value found in input");

  if(capacity > WEIGHT_CAP)
    errx(1, "Capacity must be at most %d", WEIGHT_CAP);

  int n = 0;
  int res;
  while( n < N_ITEM_CAP &&
       ( res = scanf("%128[^;];%d;%d\n", items[n].name, &items[n].weight, &items[n].val)) == 3)
  {
    n++;
  }

  if(ferror(stdin))
    err(1, "Error while parsing input");
  
  if(n < N_ITEM_CAP && res != EOF)
    errx(1, "Input number %d malformed\n", n + 1);
  
  int value = knapsack(n, capacity, items, dp);
  int weight = knapsack_traceback(n, capacity, items, dp);

  printf("final weight: %d\nfinal value: %d\n", weight, value);

}

