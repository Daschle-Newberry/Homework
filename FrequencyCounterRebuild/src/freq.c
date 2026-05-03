#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "func.h"

// Uses tries and maxheap to get word frequencies from input

#define MAX_WORD_LENGTH 20
#define MAX_COUNT 5


int main(){
    TrieNode* root = trie_node_init();

    char word_buf[MAX_WORD_LENGTH];
    int len = 0;
    int c;
    while((c = getchar()) != EOF){
        if(len > MAX_WORD_LENGTH) return -1;
        if(!(islower(c) || isdelimiter(c))) return -1;
        //End of word
        if(isdelimiter(c)){
            //Case where space occurs as first character in word
            if(len == 0) continue;

            if(!strncmp(word_buf,"zzzz",len)) break;

            trie_insert_word(root, word_buf, len);   
            len = 0; 
        }
        else{
            word_buf[len++] = (char) c;
        }
    }

    Heap* heap = malloc(sizeof(Heap));
    heap_init(heap);

    char word[MAX_WORD_LENGTH];
    trie_convert_to_heap(root,heap,word,0);

    HeapNode* max;
    int count = 0;
    puts("COUNT WORD\n===== ====");
    while(count != MAX_COUNT && (max = max_heap_remove_max(heap)) != (HeapNode*) 0){
        printf("%d %s\n",max->count,max->word);
        heap_node_destroy(max);
        count++;
    }

    heap_destroy(heap);
    trie_destroy(root);
    return 0;
}


