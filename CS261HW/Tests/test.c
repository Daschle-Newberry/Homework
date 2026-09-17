#include <stdlib.h>
#include <stdio.h>

int* create_array(int n, int iValue) {
	if(n <= 0) return (void*) 0;

	int* arr = malloc(n * sizeof (int));
	if(!arr) return (void*) 0;

	for(int i = 0; i < n; arr[i] = iValue, i++);
	return arr;
}


int main() {
	int count = 15;
	int num = 123;
	int* array = create_array(count,num);
	
	for(int i = 0; i < count; i++) printf("%d ", array[i]);
	return 0;
}
