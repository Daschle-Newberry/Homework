

### Implementation Split
1. linkedlist.h/.c - The implementation of the linked list
```c
	typedef int (*CmpFunc)(void*, void*);
	
	struct LinkedList {
		LLNode* m_head;
		CmpFunc m_cmp;
	} LinkedList;
	
	struct LLNode;
	
	struct LLNode {
		LLNode* m_next;
		char* m_key;
		int m_value;
	} LLNode;
	
	
	LLNode* ll_find(LinkedList*, void* key);
	bool ll_insert(void* key, size_t key_size, void* val);
	
```
1.  hashtable.h/.c - The implementation of the hashtable
```c
	typedef crc64_t unsigned long long;
	typedef crc64_t (*HashFunc)(void *);
	typedef Bucket LInkedList;
	
	struct HashTable {
		HashFunc m_hash;
		Bucket* m_buckets;
		size_t m_count;
	} HashTable;
	
	
	```