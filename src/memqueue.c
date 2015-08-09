#include "memtype.h"
#include "memqueue.h"

unsigned long MAPushMemoryQueueMessage( struct MemoryPage *p_mempage , int msg_type , char *msg_data , unsigned long msg_bufsize )
{
	void			*p_queue_add = NULL ;
	struct MemoryBlock	*p_memblock_queue_add = NULL ;

	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) || msg_bufsize <= 0 )
	{
		errno = EINVAL ;
		return 0;
	}
	
	p_queue_add = MAAllocMemoryBlock( p_mempage , msg_bufsize );
	if( p_queue_add == NULL )
		return 0;
	p_memblock_queue_add = P2MEMBLOCK(p_queue_add) ;
	
	if( p_mempage->ds.queue.queue_first_offset == 0 )
	{
		/*
			|H|        |
			|H|ADD     |
		*/
		
		p_memblock_queue_add->ds.queue.msg_type = msg_type ;
		p_memblock_queue_add->ds.queue.queue_prev_offset = 0 ;
		p_memblock_queue_add->ds.queue.queue_next_offset = 0 ;
		p_mempage->ds.queue.queue_first_offset = p_memblock_queue_add->addr_this_offset ;
		p_mempage->ds.queue.queue_last_offset = p_memblock_queue_add->addr_this_offset ;
	}
	else
	{
		/*
			|H|               |
			|H|BLOCK|ADD|     |
		*/
		
		struct MemoryBlock	*p_memblock_queue_last = (struct MemoryBlock *)((char*)p_mempage+p_mempage->ds.queue.queue_last_offset) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_last) )
		{
			MAFreeMemoryBlock( p_mempage , p_queue_add );
			return 0;
		}
		
		p_memblock_queue_add->ds.queue.msg_type = msg_type ;
		p_memblock_queue_add->ds.queue.queue_prev_offset = p_memblock_queue_last->addr_this_offset ;
		p_memblock_queue_add->ds.queue.queue_next_offset = 0 ;
		p_memblock_queue_last->ds.queue.queue_next_offset = p_memblock_queue_add->addr_this_offset ;
		p_mempage->ds.queue.queue_last_offset = p_memblock_queue_add->addr_this_offset ;
	}
	
	memcpy( p_queue_add , msg_data , msg_bufsize );
	
	return msg_bufsize;
}

static unsigned long _PopupMemoryBlockByQueue( struct MemoryPage *p_mempage , int *p_msg_type , char **pp_msg_data_remove , struct MemoryBlock **pp_memblock_queue_remove , unsigned long msg_bufsize )
{
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return 0;
	}
	
	if( p_mempage->ds.queue.queue_first_offset == 0 )
	{
		/*
			|H|                     |
			|H|                     |
		*/
		return 0;
	}
	
	while(1)
	{
		(*pp_msg_data_remove) = MATravelMemoryQueueMessage( p_mempage , NULL , (*pp_msg_data_remove) ) ;
		if( (*pp_msg_data_remove) == NULL )
			break;
		
		(*pp_memblock_queue_remove) = P2MEMBLOCK( (*pp_msg_data_remove) ) ;
		if( CHECK_MEMBLOCK_MAGIC(*pp_memblock_queue_remove) )
		{
			errno = EINVAL ;
			return 0;
		}
		
		if( (*p_msg_type) == 0 )
		{
			break;
		}
		else if( (*p_msg_type) > 0 && (*p_msg_type) == (*pp_memblock_queue_remove)->ds.queue.msg_type )
		{
			break;
		}
		else if( (*p_msg_type) < 0 && (*pp_memblock_queue_remove)->ds.queue.msg_type <= abs(*p_msg_type) )
		{
			break;
		}
	}
	if( (*pp_msg_data_remove) == NULL )
		return 0;
	
	if( msg_bufsize > 0 && msg_bufsize < (*pp_memblock_queue_remove)->block_size )
	{
		(*pp_msg_data_remove) = NULL ;
		return (*pp_memblock_queue_remove)->block_size;
	}
	
	if( (*pp_memblock_queue_remove)->ds.queue.queue_prev_offset == 0 && (*pp_memblock_queue_remove)->ds.queue.queue_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|              |
		*/
		p_mempage->ds.queue.queue_first_offset = 0 ;
		p_mempage->ds.queue.queue_last_offset = 0 ;
	}
	else if( (*pp_memblock_queue_remove)->ds.queue.queue_prev_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|  |BLOCK|     |
		*/
		struct MemoryBlock	*p_memblock_queue_next = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock_queue_remove)->ds.queue.queue_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_next) )
			return 0;
		
		p_mempage->ds.queue.queue_first_offset = p_memblock_queue_next->addr_this_offset ;
		p_memblock_queue_next->ds.queue.queue_prev_offset = 0 ;
	}
	else if( (*pp_memblock_queue_remove)->ds.queue.queue_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|BLOCK|  |REMOVE|     |
		*/
		
		struct MemoryBlock	*p_memblock_queue_prev = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock_queue_remove)->ds.queue.queue_prev_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_prev) )
			return 0;
		
		p_mempage->ds.queue.queue_first_offset = 0 ;
		p_memblock_queue_prev->ds.queue.queue_next_offset = 0 ;
	}
	else
	{
		/*
			|H|                         |
			|H|BLOCK|REMOVE|   |BLOCK|  |
		*/
		struct MemoryBlock	*p_memblock_queue_prev = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock_queue_remove)->ds.queue.queue_prev_offset ) ;
		struct MemoryBlock	*p_memblock_queue_next = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock_queue_remove)->ds.queue.queue_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_prev) )
			return 0;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_next) )
			return 0;
		
		p_memblock_queue_prev->ds.queue.queue_next_offset = p_memblock_queue_next->addr_this_offset ;
		p_memblock_queue_next->ds.queue.queue_prev_offset = p_memblock_queue_prev->addr_this_offset ;
	}
	
	(*p_msg_type) = (*pp_memblock_queue_remove)->ds.queue.msg_type ;
	
	return (*pp_memblock_queue_remove)->block_size;
}

unsigned long MAPopupMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *msg_data , unsigned long msg_bufsize )
{
	char			*p_msg_data_remove = NULL ;
	struct MemoryBlock	*p_memblock_queue_remove = NULL ;
	
	unsigned long		ulret = 0 ;
	
	ulret = _PopupMemoryBlockByQueue( p_mempage , p_msg_type , & p_msg_data_remove , & p_memblock_queue_remove , msg_bufsize ) ;
	if( ulret == 0 || p_msg_data_remove == NULL )
		return 0;
	
	memcpy( msg_data , p_msg_data_remove , p_memblock_queue_remove->block_size );
	
	MAFreeMemoryBlock( p_mempage , p_msg_data_remove );
	
	return ulret;
}

unsigned long MAPopdmpMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char **pp_msg_data )
{
	char			*pp_msg_data_remove = NULL ;
	struct MemoryBlock	*p_memblock_queue_remove = NULL ;
	
	unsigned long		ulret = 0 ;
	
	ulret = _PopupMemoryBlockByQueue( p_mempage , p_msg_type , & pp_msg_data_remove , & p_memblock_queue_remove , 0 ) ;
	if( ulret == 0 || pp_msg_data_remove == NULL )
		return 0;
	
	(*pp_msg_data) = (char*)malloc( p_memblock_queue_remove->block_size ) ;
	if( (*pp_msg_data) == NULL )
		return 0;
	memcpy( (*pp_msg_data) , pp_msg_data_remove , p_memblock_queue_remove->block_size );
	
	MAFreeMemoryBlock( p_mempage , pp_msg_data_remove );
	
	return ulret;
}

char *MATravelMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *p_msg_data )
{
	struct MemoryBlock	*p_memblock_queue_travel = NULL ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return NULL;
	}
	
	if( p_msg_data == NULL )
	{
		if( p_mempage->ds.queue.queue_first_offset == 0 )
		{
			return NULL;
		}
		else
		{
			if( p_msg_type )
				(*p_msg_type) = ((struct MemoryBlock *)((char*)p_mempage+p_mempage->ds.queue.queue_first_offset))->ds.queue.msg_type ;
			return MEMBLOCK2P( (char*)p_mempage+p_mempage->ds.queue.queue_first_offset );
		}
	}
	else
	{
		p_memblock_queue_travel = P2MEMBLOCK(p_msg_data) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_travel) )
		{
			errno = EINVAL ;
			return NULL;
		}
		
		if( p_memblock_queue_travel->ds.queue.queue_next_offset == 0 )
		{
			return NULL;
		}
		else
		{
			if( p_msg_type )
				(*p_msg_type) = ((struct MemoryBlock *)((char*)p_mempage+p_memblock_queue_travel->ds.queue.queue_next_offset))->ds.queue.msg_type ;
			return MEMBLOCK2P( (char*)p_mempage+p_memblock_queue_travel->ds.queue.queue_next_offset );
		}
	}
}

