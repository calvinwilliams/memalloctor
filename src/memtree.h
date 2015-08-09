#ifndef _H_MEMTREE_
#define _H_MEMTREE_

#include "memalloctor.h"

/* red-block tree */

void *MAAddMemoryRBTreeRoot( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size );
void *MAAddMemoryRBTreeLeftChild( struct MemoryPage *p_mempage , void *parent , void *block_data , unsigned long block_size );
void *MAAddMemoryRBTreeRightChild( struct MemoryPage *p_mempage , void *parent , void *block_data , unsigned long block_size );
void *MAInsertMemoryRBTreeNode( struct MemoryPage *p_mempage , void *from , void *block_data , unsigned long block_size );
void MARemoveMemoryRBTreeNode( struct MemoryPage *p_mempage , void *p );

void *MAGetMemoryRBTreeParent( struct MemoryPage *p_mempage , void *p );
void *MAGetMemoryRBTreeLeftChild( struct MemoryPage *p_mempage , void *p );
void *MAGetMemoryRBTreeRightChild( struct MemoryPage *p_mempage , void *p );

void *MATravelMemoryRBTreePreOrder( struct MemoryPage *p_mempage , void *p );
void *MATravelMemoryRBTreeInOrder( struct MemoryPage *p_mempage , void *p );
void *MATravelMemoryRBTreePostOrder( struct MemoryPage *p_mempage , void *p );

#endif
