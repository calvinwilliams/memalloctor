#include "memtype.h"
#include "memalloctor.h"

struct MemoryPage *MAFormatMemoryPage( char *buf , unsigned long bufsize )
{
	struct MemoryPage	*p_mempage = (struct MemoryPage *) buf ;
	
	memset( buf , 0x00 , bufsize );
	
	memcpy( p_mempage->magic , "MP" , 2 );
	p_mempage->totalsize = bufsize ;
	p_mempage->block_count = 0 ;
	p_mempage->block_used_totalsize = ULSIZEOF(struct MemoryPage) ;
	
	return p_mempage;
}

void MACleanMemoryPage( struct MemoryPage *p_mempage )
{
	char		*p = NULL ;
	char		*p_next = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return;
	}
	
	p = MATravelNextMemoryBlocks( p_mempage , NULL ) ;
	while( p )
	{
		p_next = MATravelNextMemoryBlocks( p_mempage , p ) ;
		MARemoveMemoryBlock( p_mempage , p );
		p = p_next ;
	}
	
	return;
}

unsigned long MAGetMemoryTotalSize( struct MemoryPage *p_mempage )
{
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return 0;
	}
	
	return p_mempage->totalsize;
}

unsigned long MAGetMemoryBlockCount( struct MemoryPage *p_mempage )
{
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return 0;
	}
	
	return p_mempage->block_count;
}

unsigned long MAGetMemoryUsedTotalSize( struct MemoryPage *p_mempage )
{
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return 0;
	}
	
	return p_mempage->block_used_totalsize;
}

unsigned long MAGetMemoryUnusedTotalSize( struct MemoryPage *p_mempage )
{
	return MAGetMemoryTotalSize(p_mempage) - MAGetMemoryUsedTotalSize(p_mempage) ;
}

unsigned long MAGetMemoryBlockSize( void *p )
{
	if( p == NULL )
	{
		errno = EINVAL ;
		return 0;
	}
	
	return P2MEMBLOCK(p)->block_size;
}

unsigned long MASizeOfMemoryPageHeader()
{
	return sizeof(struct MemoryPage);
}

unsigned long MASizeOfMemoryBlockHeader()
{
	return sizeof(struct MemoryBlock);
}

struct MemoryPage *MAGetMemoryPage( void *p )
{
	return MEMBLOCK2MEMPAGE(P2MEMBLOCK(p));
}

void *MAAllocMemoryBlock( struct MemoryPage *p_mempage , long block_size )
{
	return MAAllocMemoryBlockEx( p_mempage , block_size , 0 );
}

void *MAAllocMemoryBlockEx( struct MemoryPage *p_mempage , long block_size , unsigned long fileindex )
{
	struct MemoryBlock	*p_memblock_add = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) ||  block_size < 0 )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	if( p_mempage->block_used_totalsize + block_size > p_mempage->totalsize )
	{
		errno = ENOMEM ;
		return NULL;
	}
	
	if( p_mempage->addr_first_offset == 0 )
	{
		if( ULSIZEOF(struct MemoryPage) + ULSIZEOF(struct MemoryBlock) + block_size <= p_mempage->totalsize )
		{
			/*
				|H|                  |
				|H|ADD|              |
			*/
			p_memblock_add = (struct MemoryBlock *)( (char*)p_mempage+ULSIZEOF(struct MemoryPage) );
			
			memcpy( p_memblock_add->magic , "MB" , 2 );
			p_memblock_add->fileindex = fileindex ;
			p_memblock_add->addr_this_offset = (char*)p_memblock_add - (char*)p_mempage ;
			p_memblock_add->addr_prev_offset = 0 ;
			p_memblock_add->addr_next_offset = 0 ;
			p_memblock_add->block_size = block_size ;
			p_mempage->addr_first_offset = p_memblock_add->addr_this_offset ;
			p_mempage->addr_last_offset = p_memblock_add->addr_this_offset ;
			
			p_mempage->block_count++;
			p_mempage->block_used_totalsize += ULSIZEOF(struct MemoryBlock) + block_size ;
			
			return MEMBLOCK2P(p_memblock_add);
		}
	}
	else
	{
		struct MemoryBlock	*p_memblock_travel = NULL ;
		struct MemoryBlock	*p_memblock_travel_next = NULL ;
		unsigned long		block_no ;
		
		for( p_memblock_travel = (struct MemoryBlock *)((char*)p_mempage+p_mempage->addr_last_offset) , block_no = 0
			; block_no < p_mempage->block_count
			; p_memblock_travel = (struct MemoryBlock *)( (char*)p_mempage+(p_memblock_travel->addr_next_offset?p_memblock_travel->addr_next_offset:p_mempage->addr_first_offset)) , block_no++ )
		{
			if(	p_memblock_travel->addr_next_offset == 0
				&&
				(char*)p_memblock_travel + ULSIZEOF(struct MemoryBlock) + p_memblock_travel->block_size + ULSIZEOF(struct MemoryBlock) + block_size <= (char*)p_mempage + p_mempage->totalsize
			)
			{
				/*
					|H|                  |
					|H|BLOCK|ADD|        |
				*/
				p_memblock_add = (struct MemoryBlock *)( (char*)p_memblock_travel + ULSIZEOF(struct MemoryBlock) + p_memblock_travel->block_size ) ;
				
				memcpy( p_memblock_add->magic , "MB" , 2 );
				p_memblock_add->fileindex = fileindex ;
				p_memblock_add->addr_this_offset = (char*)p_memblock_add - (char*)p_mempage ;
				p_memblock_add->addr_prev_offset = p_memblock_travel->addr_this_offset ;
				p_memblock_add->addr_next_offset = 0 ;
				p_memblock_add->block_size = block_size ;
				p_memblock_travel->addr_next_offset = p_memblock_add->addr_this_offset ;
				p_mempage->addr_last_offset = p_memblock_add->addr_this_offset ;
				
				p_mempage->block_count++;
				p_mempage->block_used_totalsize += ULSIZEOF(struct MemoryBlock) + block_size ;
				
				return MEMBLOCK2P(p_memblock_add);
			}
			else if(	p_memblock_travel->addr_prev_offset == 0
					&&
					(char*)p_mempage + ULSIZEOF(struct MemoryPage) + ULSIZEOF(struct MemoryBlock) + block_size <= (char*)(p_memblock_travel)
			)
			{
				/*
					|H|                  |
					|H|ADD|  |BLOCK|     |
				*/
				p_memblock_add = (struct MemoryBlock *)( (char*)p_mempage + ULSIZEOF(struct MemoryPage) ) ;
				
				memcpy( p_memblock_add->magic , "MB" , 2 );
				p_memblock_add->fileindex = fileindex ;
				p_memblock_add->addr_this_offset = (char*)p_memblock_add - (char*)p_mempage ;
				p_memblock_add->addr_prev_offset = 0 ;
				p_memblock_add->addr_next_offset = (char*)p_memblock_travel - (char*)p_mempage ;
				p_memblock_add->block_size = block_size ;
				p_memblock_travel->addr_prev_offset = p_memblock_add->addr_this_offset ;
				p_mempage->addr_first_offset = p_memblock_add->addr_this_offset ;
				
				p_mempage->block_count++;
				p_mempage->block_used_totalsize += ULSIZEOF(struct MemoryBlock) + block_size ;
				
				return MEMBLOCK2P(p_memblock_add);
			}
			else if( (char*)p_memblock_travel + ULSIZEOF(struct MemoryBlock) + p_memblock_travel->block_size + ULSIZEOF(struct MemoryBlock) + block_size <= (char*)p_mempage + p_memblock_travel->addr_next_offset )
			{
				/*
					|H|                      |
					|H|BLOCK|ADD|   |BLOCK|  |
				*/
				p_memblock_add = (struct MemoryBlock *)( (char*)p_memblock_travel + ULSIZEOF(struct MemoryBlock) + p_memblock_travel->block_size ) ;
				p_memblock_travel_next = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_travel->addr_next_offset ) ;
				
				memcpy( p_memblock_add->magic , "MB" , 2 );
				p_memblock_add->fileindex = fileindex ;
				p_memblock_add->addr_this_offset = (char*)p_memblock_add - (char*)p_mempage ;
				p_memblock_add->addr_prev_offset = (char*)p_memblock_travel - (char*)p_mempage ;
				p_memblock_add->addr_next_offset = p_memblock_travel_next->addr_this_offset ;
				p_memblock_add->block_size = block_size ;
				p_memblock_travel->addr_next_offset = p_memblock_add->addr_this_offset ;
				p_memblock_travel_next->addr_prev_offset = p_memblock_add->addr_this_offset ;
				
				p_mempage->block_count++;
				p_mempage->block_used_totalsize += ULSIZEOF(struct MemoryBlock) + block_size ;
				
				return MEMBLOCK2P(p_memblock_add);
			}
		}
	}
	
	errno = ENOMEM ;
	return NULL;
}

void MAFreeMemoryBlock( struct MemoryPage *p_mempage , void *p )
{
	struct MemoryBlock	*p_memblock_remove = NULL ;
	
	if( p_mempage == NULL )
	{
		p_mempage = MEMBLOCK2MEMPAGE(P2MEMBLOCK(p)) ;
	}
	
	if( CHECK_MEMPAGE_MAGIC(p_mempage) || p == NULL )
	{
		errno = EINVAL ;
		return;
	}
	
	p_memblock_remove = P2MEMBLOCK( p ) ;
	if( CHECK_MEMBLOCK_MAGIC(p_memblock_remove) )
	{
		errno = EINVAL ;
		return;
	}
	
	if( p_memblock_remove->addr_this_offset == p_mempage->addr_first_offset && p_memblock_remove->addr_this_offset == p_mempage->addr_last_offset )
	{
		/*
			|H|                     |
			|H|REMOVE|              |
		*/
		p_mempage->addr_first_offset = 0 ;
		p_mempage->addr_last_offset = 0 ;
	}
	else if( p_memblock_remove->addr_this_offset == p_mempage->addr_first_offset )
	{
		/*
			|H|                     |
			|H|REMOVE|  |BLOCK|     |
		*/
		struct MemoryBlock	*p_memblock_addr_next = NULL ;
		
		p_memblock_addr_next = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_remove->addr_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_addr_next) )
		{
			errno = EINVAL ;
			return;
		}
		p_mempage->addr_first_offset = p_memblock_remove->addr_next_offset ;
		p_memblock_addr_next->addr_prev_offset = 0 ;
	}
	else if( p_memblock_remove->addr_this_offset == p_mempage->addr_last_offset )
	{
		/*
			|H|                     |
			|H|BLOCK|REMOVE|        |
		*/
		struct MemoryBlock	*p_memblock_addr_prev = NULL ;
		
		p_memblock_addr_prev = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_remove->addr_prev_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_addr_prev) )
		{
			errno = EINVAL ;
			return;
		}
		p_mempage->addr_last_offset = p_memblock_remove->addr_prev_offset ;
		p_memblock_addr_prev->addr_next_offset = 0 ;
	}
	else
	{
		/*
			|H|                         |
			|H|BLOCK|REMOVE|   |BLOCK|  |
		*/
		struct MemoryBlock	*p_memblock_addr_prev = NULL ;
		struct MemoryBlock	*p_memblock_addr_next = NULL ;
		
		p_memblock_addr_prev = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_remove->addr_prev_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_addr_prev) )
		{
			errno = EINVAL ;
			return;
		}
		p_memblock_addr_next = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_remove->addr_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_addr_next) )
		{
			errno = EINVAL ;
			return;
		}
		p_memblock_addr_prev->addr_next_offset = p_memblock_addr_next->addr_this_offset ;
		p_memblock_addr_next->addr_prev_offset = p_memblock_addr_prev->addr_this_offset ;
	}
	
	p_mempage->block_count--;
	p_mempage->block_used_totalsize -= ULSIZEOF(struct MemoryBlock) + p_memblock_remove->block_size ;
	
	memset( (char*)p_memblock_remove , 0x00 , ULSIZEOF(struct MemoryBlock) + p_memblock_remove->block_size );
	
	return;
}

void *MAAddMemoryBlock( struct MemoryPage *p_mempage , void *block_data , long block_size )
{
	return MAAddMemoryBlockEx( p_mempage , block_data , block_size , 0 );
}

void *MAAddMemoryBlockEx( struct MemoryPage *p_mempage , void *block_data , long block_size , unsigned long fileindex )
{
	void			*p_add = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	p_add = MAAllocMemoryBlockEx( p_mempage , block_size , fileindex ) ;
	if( p_add == NULL )
	{
		return NULL;
	}
	else
	{
		memcpy( p_add , block_data , block_size );
		return p_add;
	}
}

void MARemoveMemoryBlock( struct MemoryPage *p_mempage , void *p )
{
	MAFreeMemoryBlock( p_mempage , p );
	
	return;
}

void *MATravelNextMemoryBlocks( struct MemoryPage *p_mempage , void *p )
{
	struct MemoryBlock	*p_memblock = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	if( p == NULL )
	{
		if( p_mempage->addr_first_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P((char*)p_mempage+p_mempage->addr_first_offset);
	}
	else
	{
		p_memblock = P2MEMBLOCK(p) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock) )
			return NULL;
		
		if( p_memblock->addr_next_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P((char*)p_mempage+p_memblock->addr_next_offset);
	}
}

void *MATravelPrevMemoryBlocks( struct MemoryPage *p_mempage , void *p )
{
	struct MemoryBlock	*p_memblock = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	if( p == NULL )
	{
		if( p_mempage->addr_last_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P((char*)p_mempage+p_mempage->addr_last_offset);
	}
	else
	{
		p_memblock = P2MEMBLOCK(p) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock) )
			return NULL;
		
		if( p_memblock->addr_prev_offset == 0 )
			return NULL;
		else
			return MEMBLOCK2P((char*)p_mempage+p_memblock->addr_prev_offset);
	}
}

