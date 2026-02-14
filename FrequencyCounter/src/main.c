/**
 *  Daschle Newberry CS 261 Program 2: Frequency Counter
 * 
 * Implementation utilizes a trie to store counts of words efficiently and max-heap for max extraction. 
 * The same behavior could be heavily simplified by just reading input into an array and sorting it, but that's no fun. 
 * 
 * */

//For strndup 
#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#define MAX_WORD_LENGTH 100
#define VALID_CHAR_COUNT 26
#define MAX_COUNT 5

typedef struct TrieNode{
	struct TrieNode* children[VALID_CHAR_COUNT];
	int is_word;
	int count;
} TrieNode;

typedef struct HeapNode{
	char* word;
	int word_length;
	int count;
} HeapNode;

typedef struct MaxHeap{
	HeapNode** storage;
	int size;
	int max;
} MaxHeap;

TrieNode* trie_node_init(){
	//calloc is used to initialize the entire Trie node to 0. 
	//This is important for so the each entry of the array is 0 if not initialized
	return calloc(1,sizeof(TrieNode));
}

void trie_destroy(TrieNode* n){
	for(int i = 0; i < VALID_CHAR_COUNT; i++){
		if(n->children[i] != 0) trie_destroy(n->children[i]);
	}
	free(n); 
}

void trie_insert_word(TrieNode* n, char* word){
	char* c = word;
	while(*c !='\0'){
		if(n->children[(int)(*c - 'a')] == 0){
			n->children[(int)(*c - 'a')] = trie_node_init();
		}
		n = n->children[(int)(*c - 'a')];
		c++;
	}

	n->is_word = 1;
	n->count++;
}

void heap_init(MaxHeap* heap){
	heap->storage = malloc(sizeof(HeapNode*));
	heap->size = 0;
	heap->max = 1;
}

void heap_node_destroy(HeapNode* node){
	free(node->word);
	free(node);
}

void heap_destroy(MaxHeap* heap){
	for(int i = 0; i < heap->size; i++){
		heap_node_destroy(heap->storage[i]);
	}
	free(heap->storage);
	free(heap);
}

static inline void heap_swap(MaxHeap* heap, int a_idx, int b_idx){
		HeapNode* tmp = heap->storage[a_idx];
		heap->storage[a_idx] = heap->storage[b_idx];
		heap->storage[b_idx] = tmp;
}

void heap_insert(const char* word, int word_length, int word_count, MaxHeap* heap){
	HeapNode* node = malloc(sizeof(HeapNode));
	node->word = strndup(word, word_length);
	node->count = word_count;

	if(heap->size >= heap->max){
		heap->max*=2;
		heap->storage = realloc(heap->storage,heap->max * sizeof(HeapNode*));
	}

	heap->storage[heap->size] = node;
	heap->size++;

	int idx = heap->size - 1;
	int pidx = (idx - 1) / 2;

	//These variables are just to make the loop a bit cleaner
	HeapNode* parent = heap->storage[pidx];
	HeapNode* child = node;

	//Maxheap sift up with lexicographical fallback
	while(parent->count < child->count || ((parent->count == child->count) && strcmp(parent->word,child->word) > 0)){
		heap_swap(heap,pidx,idx);

		idx = pidx;
		pidx = (pidx - 1) / 2;

		parent = heap->storage[pidx];
		child = heap->storage[idx];
	}
}

HeapNode* heap_remove_max(MaxHeap* heap){
	if(heap->size < 1) return (HeapNode*) 0;
	HeapNode* max = heap->storage[0];
	heap->storage[0] = heap->storage[heap->size - 1];
	heap->size--;
	int pidx = 0;

	//Maxheap sift down with lexicographical fallback
	while(1){
		int lidx = 2 * pidx + 1;
		int ridx = 2 * pidx + 2;
		int largest = pidx;

		if(lidx < heap->size){
			HeapNode* left = heap->storage[lidx];
			if(left->count > heap->storage[largest]->count || ((heap->storage[largest]->count == left->count) && strcmp(left->word,heap->storage[largest]->word) < 0)){
				largest = lidx;
			}	
		}
		if(ridx < heap->size){
			HeapNode* right = heap->storage[ridx];
			if(right->count > heap->storage[largest]->count || ((heap->storage[largest]->count == right->count) && strcmp(right->word,heap->storage[largest]->word) < 0)){
				largest = ridx;
			}
		}

		if(largest != pidx){
			heap_swap(heap,pidx,largest);
			pidx = largest;
		}else break;
	}

	return max;
}

void trie_convert_to_heap(TrieNode* n, char* word, int word_length, MaxHeap* heap){
	if(n->is_word) heap_insert(word, word_length, n->count, heap);
	for(int i = 0; i < VALID_CHAR_COUNT; i++){
		if(n->children[i] != 0){
			word[word_length] = i + 'a';
			trie_convert_to_heap(n->children[i],word,word_length + 1, heap);
		}
	}
}

int main(){
	TrieNode* root = trie_node_init();

	char word_buf[MAX_WORD_LENGTH];
	size_t len = 0;

	int c;
	while((c = getchar()) != EOF){
		if(len > MAX_WORD_LENGTH) return -1;
		
		//End of word
		if(isspace(c)){
			word_buf[len] = '\0';

			if(!strcmp(word_buf,"zzzz")) break;

			//Case where space occurs as first character in word
			if(len == 0) continue;
			else{
				trie_insert_word(root, word_buf);	
				len = 0;
				continue;
			}
			
		}
		word_buf[len++] = (char)tolower(c);
	}


	MaxHeap* heap = malloc(sizeof(MaxHeap));
	heap_init(heap);

	char word[MAX_WORD_LENGTH];
	trie_convert_to_heap(root,word,0, heap);

	HeapNode* max;
	int count = 0;
	puts("COUNT WORD\n===== ====");
	while(count != MAX_COUNT && (max = heap_remove_max(heap)) != (HeapNode*) 0){
		printf("%d %s\n",max->count,max->word);
		heap_node_destroy(max);
		count++;
	}
	fclose(stdin);
	heap_destroy(heap);
	trie_destroy(root);
	return 0;
}

