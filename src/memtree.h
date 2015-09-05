#ifndef _H_MEMTREE_
#define _H_MEMTREE_

#include "memalloctor.h"

/* quarter-trie-tree */
#define MEMTREE_ERROR_PARAMETER			-7
#define MEMTREE_ERROR_NOT_ENOUGH_SPACE		-16
#define MEMTREE_ERROR_KEY_EXIST			-21
#define MEMTREE_ERROR_KEY_NOT_EXIST		-22

int MAAddMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key , void *value , unsigned long value_size );
int MARemoveMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key );
int MAQueryMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key , void **pp_value , unsigned long *p_value_size );

#endif
