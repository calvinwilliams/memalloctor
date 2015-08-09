#ifndef _H_MEMALLOCTOR_
#define _H_MEMALLOCTOR_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define LPTRDIFF(_addr1_,_addr2_)		((long)((char*)(_addr1_)-(char*)(_addr2_)))

/* memory page */

struct MemoryPage ;

struct MemoryPage *MAFormatMemoryPage( char *buf , unsigned long totalsize );
void MACleanMemoryPage( struct MemoryPage *p_mempage );

unsigned long MAGetMemoryTotalSize( struct MemoryPage *p_mempage );

unsigned long MAGetMemoryBlockCount( struct MemoryPage *p_mempage );
unsigned long MAGetMemoryUsedTotalSize( struct MemoryPage *p_mempage );
unsigned long MAGetMemoryUnusedTotalSize( struct MemoryPage *p_mempage );

unsigned long MAGetMemoryBlockSize( void *p );

unsigned long MASizeOfMemoryPageHeader();
unsigned long MASizeOfMemoryBlockHeader();

/* memory block */

void *MAAllocMemoryBlock( struct MemoryPage *p_mempage , unsigned long block_size );
void MAFreeMemoryBlock( struct MemoryPage *p_mempage , void *p );

void *MAAddMemoryBlock( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size );
void MARemoveMemoryBlock( struct MemoryPage *p_mempage , void *p );

void *MATravelNextMemoryBlock( struct MemoryPage *p_mempage , void *p );
void *MATravelPrevMemoryBlock( struct MemoryPage *p_mempage , void *p );

#endif
