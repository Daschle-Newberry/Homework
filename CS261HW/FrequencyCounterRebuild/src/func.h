//Definitions for func.c

#ifndef FUNC_H
#define FUNC_H

#define VALID_CHAR_COUNT 26
#define DELIMITERS " \n"

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

typedef struct Heap{
    HeapNode** storage;
    int size;
    int max;
} Heap;

/**
 * Initializes trie node, with all bytes being zero, thin wrapper for calloc.
 * 
 * @return Pointer to heap allocated TrieNode
 * */
TrieNode* trie_node_init();


/** 
 * Frees provided Trie and all children recursively
 * 
 * @pararm n The trie to be destroyed
 * 
 * @return None
 * */
void trie_destroy(TrieNode* n);

/**
 * Inserts word into the provided trie
 * 
 * @param n Trie to insert into
 * @param word The word to insert, can be null-terminated or not
 * @param len The amount of characters to insert
 * 
 * @return None
 * */
void trie_insert_word(TrieNode* n, char* word, int len);

/**
 * Initialized a max heap
 * 
 * @param heap Pointer which the Maxheap will be allocated to
 * 
 * @return None
 * */
void heap_init(Heap* h);

/**
 * Destroys a heap node
 * 
 * @param node Pointer to node to destroyed
 * 
 * @return None
 * */
void heap_node_destroy(HeapNode* node);

/**
 * Destroys a heap and all nodes within that heap
 * 
 * @param h Heap (memory) allocated Heap (gotta fix these naming conventions) 
 * 
 * @return None
 * */
void heap_destroy(Heap* h);

/**
 * Inserts word into heap, treating it as a max heap with lexicographical tie breakers
 * 
 * @param h The heap to insert into
 * @param word The word to be copied an inserted
 * @param word_length The length of the word
 * @param word_count The frequency of the word
 * 
 * return None
 * */
void max_heap_insert(Heap* h, const char* word, int word_length, int word_count);

/**
 * Removes the max from a heap and uses max-heap sift down with lexicographical tie breakers
 * 
 * @param h Heap to remove the max from
 * 
 * @return HeapNode The max node
 * */
HeapNode* max_heap_remove_max(Heap* h);

/**
 * Converts a trie to a heap using depth first search
 *
 * @param n The trie to convert
 * @param h The heap to add to
 * @param word Buffer of size MAX_WORD_LENGTH to store words through calls
 * @param word_length Length of the current word
 * 
 * @return None
 * */
void trie_convert_to_heap(TrieNode* n, Heap* h, char* word, int word_length);

int isdelimiter(char c);

#endif