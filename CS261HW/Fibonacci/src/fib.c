#include <stdio.h>
#include <string.h>

/**
 * Calculates the nth fibonnaci number using memoization
 * @param storage Integer array for storing the (n-i)th fibonacci number
 * @param n The nth fibonacci number to be computed
 * @return The nth fibonacci number
 */
int fib(int* storage, int n){
	// Base case
	if(n <= 1) return n;

	//Check and storage
	if(storage[n - 1] == -1){
		storage[n - 1] = fib(storage, n - 1);
	}

	if(storage[n - 2] == -1){
		storage[n - 2] = fib(storage, n - 2);
	}

	return storage[n - 1] + storage[n - 2];
	
}

int main(){
	while(1){
		int n;
		printf("Value: ");
		scanf("%d", &n);
		
		//Skip negative inputs
		if(n < 0) continue;

		//init storage and set it to -1
		int storage[n];
		memset(storage,-1,(n) * sizeof(int));
		int nth_fib = fib(storage,n);
		
		printf("Fibonacci number %d is %d\n",n, nth_fib);

		//Exit on 0. We could check before computing fib and initializing storage, but since n = 0 both of those operations are basically free
		if(n == 0) break;
	}

	return 0;
}