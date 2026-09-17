/**
 *  Daschle Newberry CS 261 Program 2: Frequency Counter
 * 
 * 
 * This file implements the systems needed to build/use a trie of words and a Max heap of words/counts
 *
 * */

//For strndup 
#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdlib.h>
#include "func.h"

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
    
void trie_insert_word(TrieNode* n, char* word, int len){
    int offset = 0;
    while(offset != len){
        int c = (char) *(word + offset) - 'a';
        if(n->children[c] == 0){
            n->children[c] = trie_node_init();
        }
        n = n->children[c];
        offset++;
    }

    n->is_word = 1;
    n->count++;
}

void heap_init(Heap* h){
    h->storage = malloc(sizeof(HeapNode*));
    h->size = 0;
    h->max = 1;
}


void heap_node_destroy(HeapNode* node){
    free(node->word);
    free(node);
}

void heap_destroy(Heap* h){
    for(int i = 0; i < h->size; i++){
        heap_node_destroy(h->storage[i]);
    }
    free(h->storage);
    free(h);
}

static inline void heap_swap(Heap* h, int a_idx, int b_idx){
        HeapNode* tmp = h->storage[a_idx];
        h->storage[a_idx] = h->storage[b_idx];
        h->storage[b_idx] = tmp;
}


void max_heap_insert(Heap* h, const char* word, int word_length, int word_count){
    HeapNode* node = malloc(sizeof(HeapNode));
    node->word = strndup(word, word_length);
    node->count = word_count;

    if(h->size >= h->max){
        h->max*=2;
        h->storage = realloc(h->storage,h->max * sizeof(HeapNode*));
    }

    h->storage[h->size] = node;
    h->size++;

    int idx = h->size - 1;
    int pidx = (idx - 1) / 2;

    //These variables are just to make the loop a bit cleaner
    HeapNode* parent = h->storage[pidx];
    HeapNode* child = node;

    //Maxheap sift up with lexicographical fallback
    while(parent->count < child->count || ((parent->count == child->count) && strcmp(parent->word,child->word) > 0)){
        heap_swap(h,pidx,idx);

        idx = pidx;
        pidx = (pidx - 1) / 2;

        parent = h->storage[pidx];
        child = h->storage[idx];
    }
}

HeapNode* max_heap_remove_max(Heap* h){
    if(h->size < 1) return (HeapNode*) 0;
    HeapNode* max = h->storage[0];
    h->storage[0] = h->storage[h->size - 1];
    h->size--;
    int pidx = 0;

    //Maxheap sift down with lexicographical fallback
    while(1){
        int lidx = 2 * pidx + 1;
        int ridx = 2 * pidx + 2;
        int largest = pidx;

        if(lidx < h->size){
            HeapNode* left = h->storage[lidx];
            if(left->count > h->storage[largest]->count || ((h->storage[largest]->count == left->count) && strcmp(left->word,h->storage[largest]->word) < 0)){
                largest = lidx;
            }    
        }
        if(ridx < h->size){
            HeapNode* right = h->storage[ridx];
            if(right->count > h->storage[largest]->count || ((h->storage[largest]->count == right->count) && strcmp(right->word,h->storage[largest]->word) < 0)){
                largest = ridx;
            }
        }

        if(largest != pidx){
            heap_swap(h,pidx,largest);
            pidx = largest;
        }else break;
    }

    return max;
}

void trie_convert_to_heap(TrieNode* n, Heap* h, char* word, int word_length){
    if(n->is_word) max_heap_insert(h,word, word_length, n->count);
    for(int i = 0; i < VALID_CHAR_COUNT; i++){
        if(n->children[i] != 0){
            word[word_length] = i + 'a';
            trie_convert_to_heap(n->children[i],h,word,word_length + 1);
        }
    }
}

int isdelimiter(char c){
    return strchr(DELIMITERS,c) != NULL;
}
