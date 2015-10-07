#include "memtype.h"
#include "memlist.h"

void *MAAddMemoryListNode( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size )
{
	void			*p_dll_add = NULL ;
	struct MemoryBlock	*p_memblock_dll_add = NULL ;

	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	p_dll_add = MAAllocMemoryBlock( p_mempage , block_size );
	if( p_dll_add == NULL )
		return NULL;
	p_memblock_dll_add = P2MEMBLOCK(p_dll_add) ;
	
	if( p_mempage->ds.dll.dll_first_offset == 0 )
	{
		/*
			|H|        |
			|H|ADD     |
		*/
		
		p_memblock_dll_add->ds.dll.dll_prev_offset = 0 ;
		p_memblock_dll_add->ds.dll.dll_next_offset = 0 ;
		p_mempage->ds.dll.dll_first_offset = p_memblock_dll_add->addr_this_offset ;
		p_mempage->ds.dll.dll_last_offset = p_memblock_dll_add->addr_this_offset ;
	}
	else
	{
		/*
			|H|               |
			|H|BLOCK|ADD|     |
		*/
		
		struct MemoryBlock	*p_memblock_dll_last = (struct MemoryBlock *)((char*)p_mempage+p_mempage->ds.dll.dll_last_offset) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_last) )
		{
			MAFreeMemoryBlock( p_mempage , p_dll_add );
			return NULL;
		}
		
		p_memblock_dll_add->ds.dll.dll_prev_offset = p_memblock_dll_last->addr_this_offset ;
		p_memblock_dll_add->ds.dll.dll_next_offset = 0 ;
		p_memblock_dll_last->ds.dll.dll_next_offset = p_memblock_dll_add->addr_this_offset ;
		p_mempage->ds.dll.dll_last_offset = p_memblock_dll_add->addr_this_offset ;
	}
	
	memcpy( p_dll_add , block_data , block_size );
	
	return p_dll_add;
}

void *MAInsertBeforeMemoryListNode( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size , void *p_before )
{
	struct MemoryBlock	*p_memblock_dll_before = NULL ;
	struct MemoryBlock	*p_memblock_dll_add = NULL ;
	void			*p_dll_add = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) || p_before == NULL )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	p_memblock_dll_before = P2MEMBLOCK( p_before ) ;
	if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_before) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	p_dll_add = MAAllocMemoryBlock( p_mempage , block_size );
	if( p_dll_add == NULL )
		return NULL;
	p_memblock_dll_add = P2MEMBLOCK(p_dll_add) ;
	
	if( p_memblock_dll_before->ds.dll.dll_prev_offset == 0 )
	{
		/*
			|H|              |
			|H|INSERT|BLOCK| |
		*/
		
		p_memblock_dll_add->ds.dll.dll_prev_offset = 0 ;
		p_memblock_dll_add->ds.dll.dll_next_offset = p_memblock_dll_before->addr_this_offset ;
		p_memblock_dll_before->ds.dll.dll_prev_offset = p_memblock_dll_add->addr_this_offset ;
		p_mempage->ds.dll.dll_first_offset = p_memblock_dll_add->addr_this_offset ;
	}
	else
	{
		/*
			|H|                  |
			|H|...|INSERT|BLOCK| |
		*/
		
		struct MemoryBlock	*p_memblock_dll_prev = (struct MemoryBlock *)((char*)p_mempage+p_memblock_dll_before->ds.dll.dll_prev_offset) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_prev) )
		{
			MAFreeMemoryBlock( p_mempage , p_dll_add );
			return NULL;
		}
		
		p_memblock_dll_add->ds.dll.dll_prev_offset = p_memblock_dll_prev->addr_this_offset ;
		p_memblock_dll_add->ds.dll.dll_next_offset = p_memblock_dll_before->addr_this_offset ;
		p_memblock_dll_prev->ds.dll.dll_next_offset = p_memblock_dll_add->addr_this_offset ;
		p_memblock_dll_before->ds.dll.dll_prev_offset = p_memblock_dll_add->addr_this_offset ;
	}
	
	memcpy( p_dll_add , block_data , block_size );
	
	return p_dll_add;
}

void *MAInsertAfterMemoryListNode( struct MemoryPage *p_mempage , void *block_data , unsigned long block_size , void *p_after )
{
	struct MemoryBlock	*p_memblock_dll_after = NULL ;
	struct MemoryBlock	*p_memblock_dll_add = NULL ;
	void			*p_dll_add = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) || p_after == NULL )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	p_memblock_dll_after = P2MEMBLOCK( p_after ) ;
	if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_after) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	p_dll_add = MAAllocMemoryBlock( p_mempage , block_size );
	if( p_dll_add == NULL )
		return NULL;
	p_memblock_dll_add = P2MEMBLOCK(p_dll_add) ;
	
	if( p_memblock_dll_after->ds.dll.dll_next_offset == 0 )
	{
		/*
			|H|              |
			|H|BLOCK|INSERT| |
		*/
		
		p_memblock_dll_add->ds.dll.dll_prev_offset = p_memblock_dll_after->addr_this_offset ;
		p_memblock_dll_add->ds.dll.dll_next_offset = 0 ;
		p_memblock_dll_after->ds.dll.dll_next_offset = p_memblock_dll_add->addr_this_offset ;
		p_mempage->ds.dll.dll_last_offset = p_memblock_dll_add->addr_this_offset ;
	}
	else
	{
		/*
			|H|                  |
			|H|BLOCK|INSERT|...| |
		*/
		
		struct MemoryBlock	*p_memblock_dll_next = (struct MemoryBlock *)((char*)p_mempage+p_memblock_dll_after->ds.dll.dll_next_offset) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_next) )
		{
			MAFreeMemoryBlock( p_mempage , p_dll_add );
			return NULL;
		}
		
		p_memblock_dll_add->ds.dll.dll_prev_offset = p_memblock_dll_after->addr_this_offset ;
		p_memblock_dll_add->ds.dll.dll_next_offset = p_memblock_dll_next->addr_this_offset ;
		p_memblock_dll_after->ds.dll.dll_next_offset = p_memblock_dll_add->addr_this_offset ;
		p_memblock_dll_next->ds.dll.dll_prev_offset = p_memblock_dll_add->addr_this_offset ;
	}
	
	memcpy( p_dll_add , block_data , block_size );
	
	return p_dll_add;
}

void MARemoveMemoryListNode( struct MemoryPage *p_mempage , void *p )
{
	struct MemoryBlock	*p_memblock_dll_remove = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return;
	}
	
	p_memblock_dll_remove = P2MEMBLOCK( p ) ;
	if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_remove) )
	{
		errno = EINVAL ;
		return;
	}
	
	if( p_memblock_dll_remove->ds.dll.dll_prev_offset == 0 && p_memblock_dll_remove->ds.dll.dll_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|              |
		*/
		p_mempage->ds.dll.dll_first_offset = 0 ;
		p_mempage->ds.dll.dll_last_offset = 0 ;
	}
	else if( p_memblock_dll_remove->ds.dll.dll_prev_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|  |BLOCK|     |
		*/
		struct MemoryBlock	*p_memblock_dll_next = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_dll_remove->ds.dll.dll_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_next) )
			return;
		
		p_mempage->ds.dll.dll_first_offset = p_memblock_dll_next->addr_this_offset ;
		p_memblock_dll_next->ds.dll.dll_prev_offset = 0 ;
	}
	else if( p_memblock_dll_remove->ds.dll.dll_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|BLOCK|REMOVE|        |
		*/
		struct MemoryBlock	*p_memblock_dll_prev = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_dll_remove->ds.dll.dll_prev_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_prev) )
			return;
		
		p_mempage->ds.dll.dll_last_offset = p_memblock_dll_prev->addr_this_offset ;
		p_memblock_dll_prev->ds.dll.dll_next_offset = 0 ;
	}
	else
	{
		/*
			|H|                         |
			|H|BLOCK|REMOVE|   |BLOCK|  |
		*/
		struct MemoryBlock	*p_memblock_dll_prev = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_dll_remove->ds.dll.dll_prev_offset ) ;
		struct MemoryBlock	*p_memblock_dll_next = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_dll_remove->ds.dll.dll_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_prev) )
			return;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_next) )
			return;
		
		p_memblock_dll_prev->ds.dll.dll_next_offset = p_memblock_dll_next->addr_this_offset ;
		p_memblock_dll_next->ds.dll.dll_prev_offset = p_memblock_dll_prev->addr_this_offset ;
	}
	
	MAFreeMemoryBlock( p_mempage , p );
	
	return;
}

void *MATravelNextMemoryListNodes( struct MemoryPage *p_mempage , void *p )
{
	struct MemoryBlock	*p_memblock_dll_travel = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	if( p == NULL )
	{
		if( p_mempage->ds.dll.dll_first_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P( (char*)p_mempage+p_mempage->ds.dll.dll_first_offset );
	}
	else
	{
		p_memblock_dll_travel = P2MEMBLOCK(p) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_travel) )
		{
			errno = EINVAL ;
			return NULL;
		}
		
		if( p_memblock_dll_travel->ds.dll.dll_next_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P( (char*)p_mempage+p_memblock_dll_travel->ds.dll.dll_next_offset );
	}
}

void *MATravelPrevMemoryListNodes( struct MemoryPage *p_mempage , void *p )
{
	struct MemoryBlock	*p_memblock_dll_travel = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	if( p == NULL )
	{
		if( p_mempage->ds.dll.dll_last_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P( (char*)p_mempage+p_mempage->ds.dll.dll_last_offset );
	}
	else
	{
		p_memblock_dll_travel = P2MEMBLOCK(p) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_dll_travel) )
		{
			errno = EINVAL ;
			return NULL;
		}
		
		if( p_memblock_dll_travel->ds.dll.dll_prev_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P( (char*)p_mempage+p_memblock_dll_travel->ds.dll.dll_prev_offset );
	}
}

