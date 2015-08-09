#ifndef _H_MEMLIST_
#define _H_MEMLIST_

#include "memalloctor.h"

/* double linked list */

void *MAAddMemoryListNode( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size );
void *MAInsertBeforeMemoryListNode( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size , void *p );
void *MAInsertAfterMemoryListNode( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size , void *p );
void MARemoveMemoryListNode( struct MemoryPage *p_mempage , void *p );

void *MATravelNextMemoryListNode( struct MemoryPage *p_mempage , void *p );
void *MATravelPrevMemoryListNode( struct MemoryPage *p_mempage , void *p );


#endif
