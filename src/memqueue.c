#include "memtype.h"
#include "memqueue.h"

long MAPushMemoryQueueMessage( struct MemoryPage *p_mempage , int msg_type , char *msg_data , long msg_bufsize )
{
	void			*p_msg = NULL ;
	struct MemoryBlock	*p_memblock = NULL ;

	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) || msg_bufsize <= 0 )
	{
		errno = EINVAL ;
		return MEMQUEUE_ERROR_PARAMETER;
	}
	
	p_msg = MAAllocMemoryBlock( p_mempage , msg_bufsize );
	if( p_msg == NULL )
		return MEMQUEUE_ERROR_NOT_ENOUGH_SPACE;
	p_memblock = P2MEMBLOCK(p_msg) ;
	
	if( p_mempage->ds.queue.queue_first_offset == 0 )
	{
		/*
			|H|        |
			|H|ADD     |
		*/
		
		p_memblock->ds.queue.msg_type = msg_type ;
		p_memblock->ds.queue.queue_prev_offset = 0 ;
		p_memblock->ds.queue.queue_next_offset = 0 ;
		p_mempage->ds.queue.queue_first_offset = p_memblock->addr_this_offset ;
		p_mempage->ds.queue.queue_last_offset = p_memblock->addr_this_offset ;
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
			MAFreeMemoryBlock( p_mempage , p_msg );
			return MEMQUEUE_ERROR_INTERNAL;
		}
		
		p_memblock->ds.queue.msg_type = msg_type ;
		p_memblock->ds.queue.queue_prev_offset = p_memblock_queue_last->addr_this_offset ;
		p_memblock->ds.queue.queue_next_offset = 0 ;
		p_memblock_queue_last->ds.queue.queue_next_offset = p_memblock->addr_this_offset ;
		p_mempage->ds.queue.queue_last_offset = p_memblock->addr_this_offset ;
	}
	
	memcpy( p_msg , msg_data , msg_bufsize );
	
	return 0;
}

static long _MAPopupMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char **pp_msg , struct MemoryBlock **pp_memblock , long msg_bufsize )
{
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return MEMQUEUE_ERROR_PARAMETER;
	}
	
	if( p_mempage->ds.queue.queue_first_offset == 0 )
	{
		/*
			|H|                     |
			|H|                     |
		*/
		return MEMQUEUE_ERROR_MESSAGE_NOT_FOUND;
	}
	
	while(1)
	{
		(*pp_msg) = MATravelMemoryQueueMessages( p_mempage , NULL , (*pp_msg) ) ;
		if( (*pp_msg) == NULL )
			break;
		
		(*pp_memblock) = P2MEMBLOCK( (*pp_msg) ) ;
		if( CHECK_MEMBLOCK_MAGIC(*pp_memblock) )
		{
			errno = EINVAL ;
			return MEMQUEUE_ERROR_INTERNAL;
		}
		
		if( (*p_msg_type) == 0 )
		{
			break;
		}
		else if( (*p_msg_type) > 0 && (*p_msg_type) == (*pp_memblock)->ds.queue.msg_type )
		{
			break;
		}
		else if( (*p_msg_type) < 0 && (*pp_memblock)->ds.queue.msg_type <= abs(*p_msg_type) )
		{
			break;
		}
	}
	if( (*pp_msg) == NULL )
		return MEMQUEUE_ERROR_MESSAGE_NOT_FOUND;
	
	if( msg_bufsize > 0 && msg_bufsize < (*pp_memblock)->block_size )
	{
		(*pp_msg) = NULL ;
		return (*pp_memblock)->block_size;
	}
	
	if( (*pp_memblock)->ds.queue.queue_prev_offset == 0 && (*pp_memblock)->ds.queue.queue_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|              |
		*/
		p_mempage->ds.queue.queue_first_offset = 0 ;
		p_mempage->ds.queue.queue_last_offset = 0 ;
	}
	else if( (*pp_memblock)->ds.queue.queue_prev_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|  |BLOCK|     |
		*/
		struct MemoryBlock	*p_memblock_queue_next = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock)->ds.queue.queue_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_next) )
			return MEMQUEUE_ERROR_INTERNAL;
		
		p_mempage->ds.queue.queue_first_offset = p_memblock_queue_next->addr_this_offset ;
		p_memblock_queue_next->ds.queue.queue_prev_offset = 0 ;
	}
	else if( (*pp_memblock)->ds.queue.queue_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|BLOCK|  |REMOVE|     |
		*/
		
		struct MemoryBlock	*p_memblock_queue_prev = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock)->ds.queue.queue_prev_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_prev) )
			return MEMQUEUE_ERROR_INTERNAL;
		
		p_mempage->ds.queue.queue_last_offset = p_memblock_queue_prev->addr_this_offset ;
		p_memblock_queue_prev->ds.queue.queue_next_offset = 0 ;
	}
	else
	{
		/*
			|H|                         |
			|H|BLOCK|REMOVE|   |BLOCK|  |
		*/
		struct MemoryBlock	*p_memblock_queue_prev = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock)->ds.queue.queue_prev_offset ) ;
		struct MemoryBlock	*p_memblock_queue_next = (struct MemoryBlock *)( (char*)p_mempage + (*pp_memblock)->ds.queue.queue_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_prev) )
			return MEMQUEUE_ERROR_INTERNAL;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_next) )
			return MEMQUEUE_ERROR_INTERNAL;
		
		p_memblock_queue_prev->ds.queue.queue_next_offset = p_memblock_queue_next->addr_this_offset ;
		p_memblock_queue_next->ds.queue.queue_prev_offset = p_memblock_queue_prev->addr_this_offset ;
	}
	
	(*p_msg_type) = (*pp_memblock)->ds.queue.msg_type ;
	
	return 0;
}

long MAPopupMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *msg_buf , long msg_bufsize )
{
	char			*p = NULL ;
	struct MemoryBlock	*p_memblock_queue_remove = NULL ;
	
	long			lret = 0 ;
	
	lret = _MAPopupMemoryQueueMessage( p_mempage , p_msg_type , & p , & p_memblock_queue_remove , msg_bufsize ) ;
	if( lret )
		return lret;
	
	memcpy( msg_buf , p , p_memblock_queue_remove->block_size );
	
	MAFreeMemoryBlock( p_mempage , p );
	
	return 0;
}

long MAPopdmpMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char **pp_msg , long *p_msg_size )
{
	char			*p = NULL ;
	struct MemoryBlock	*p_memblock_queue_remove = NULL ;
	
	long			lret = 0 ;
	
	lret = _MAPopupMemoryQueueMessage( p_mempage , p_msg_type , & p , & p_memblock_queue_remove , 0 ) ;
	if( lret )
		return lret;
	
	(*pp_msg) = (char*)malloc( p_memblock_queue_remove->block_size ) ;
	if( (*pp_msg) == NULL )
		return MEMQUEUE_ERROR_ALLOC;
	memcpy( (*pp_msg) , p , p_memblock_queue_remove->block_size );
	if( p_msg_size )
		(*p_msg_size) = p_memblock_queue_remove->block_size ;
	
	MAFreeMemoryBlock( p_mempage , p );
	
	return 0;
}

char *MATravelMemoryQueueMessages( struct MemoryPage *p_mempage , int *p_msg_type , char *p_msg_data )
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

long MFPushMemoryQueueMessage( struct MemoryFiles *p_memfiles , int msg_type , char *msg_data , long msg_size )
{
	unsigned long		count ;
	struct MemoryPage	*p_root_mempage = NULL ;
	struct MemoryPage	*p_mempage = NULL ;
	struct MemoryBlock	*p_memblock = NULL ;
	void			*p_msg = NULL ;
	char			pathfilename[ MEMFILE_MAXLEN_FILENAME + 1 ] ;
	struct stat		file_stat ;
	unsigned long		file_size ;
	struct MemoryFile	*p_memfile = NULL ;
	
	int			nret = 0 ;
	
	if( p_memfiles == NULL || ( p_memfiles->memfilesparam.max_file_size > 0 && msg_size > p_memfiles->memfilesparam.max_file_size ) )
	{
		errno = EINVAL ;
		return MEMQUEUE_ERROR_PARAMETER;
	}
	
	while(1)
	{
		if( p_memfiles->memfiles_array_count > 0 )
		{
			p_root_mempage = p_memfiles->memfiles_array[0]->p_mempage ;
			
			for( count = 0 ; count < p_memfiles->memfiles_array_count ; count++ )
			{
				p_mempage = p_memfiles->memfiles_array[p_memfiles->memfile_current_index]->p_mempage ;
				
				p_msg = MAAllocMemoryBlockEx( p_mempage , msg_size , p_memfiles->memfile_current_index );
				if( p_msg )
				{
					p_memblock = P2MEMBLOCK(p_msg) ;
					
					if( p_root_mempage->ds.queue.queue_first_offset == 0 )
					{
						/*
							|H|        |
							|H|ADD     |
						*/
						
						p_memblock->ds.queue.msg_type = msg_type ;
						p_memblock->ds.queue.queue_prev_fileindex = 0 ;
						p_memblock->ds.queue.queue_prev_offset = 0 ;
						p_memblock->ds.queue.queue_next_fileindex = 0 ;
						p_memblock->ds.queue.queue_next_offset = 0 ;
						p_root_mempage->ds.queue.queue_first_fileindex = p_memfiles->memfile_current_index ;
						p_root_mempage->ds.queue.queue_first_offset = p_memblock->addr_this_offset ;
						p_root_mempage->ds.queue.queue_last_fileindex = p_memfiles->memfile_current_index ;
						p_root_mempage->ds.queue.queue_last_offset = p_memblock->addr_this_offset ;
					}
					else
					{
						/*
							|H|               |
							|H|BLOCK|ADD|     |
						*/
						
						struct MemoryBlock	*p_memblock_queue_last = (struct MemoryBlock *)((char*)(p_memfiles->memfiles_array[p_root_mempage->ds.queue.queue_last_fileindex]->p_mempage)+p_root_mempage->ds.queue.queue_last_offset) ;
						p_memblock->ds.queue.msg_type = msg_type ;
						p_memblock->ds.queue.queue_prev_fileindex = p_memblock_queue_last->fileindex ;
						p_memblock->ds.queue.queue_prev_offset = p_memblock_queue_last->addr_this_offset ;
						p_memblock->ds.queue.queue_next_fileindex = 0 ;
						p_memblock->ds.queue.queue_next_offset = 0 ;
						p_memblock_queue_last->ds.queue.queue_next_fileindex = p_memfiles->memfile_current_index ;
						p_memblock_queue_last->ds.queue.queue_next_offset = p_memblock->addr_this_offset ;
						p_root_mempage->ds.queue.queue_last_fileindex = p_memfiles->memfile_current_index ;
						p_root_mempage->ds.queue.queue_last_offset = p_memblock->addr_this_offset ;
					}
					
					memcpy( p_msg , msg_data , msg_size );
					
					return 0;
				}
				
				p_memfiles->memfile_current_index++;
				if( p_memfiles->memfile_current_index > p_memfiles->memfiles_array_count-1 )
				{
					p_memfiles->memfile_current_index = 0 ;
				}
			}
		}
		
		memset( pathfilename , 0x00 , sizeof(pathfilename) );
		snprintf( pathfilename , sizeof(pathfilename)-1 , "%s/%s%ld%s" , p_memfiles->pathdirname , p_memfiles->memfilesparam.prefix_pathfilename , p_memfiles->memfiles_array_count , p_memfiles->memfilesparam.postfix_pathfilename );
		nret = stat( pathfilename , & file_stat ) ;
		if( nret == 0 )
		{
			errno = EEXIST ;
			return MEMQUEUE_ERROR_FILE_NOT_FOUND;
		}
		
		nret = MFReallocMemoryFileArray( p_memfiles ) ;
		if( nret )
			return nret;
		
		if( p_memfiles->memfiles_array_count == 1 )
			file_size = p_memfiles->memfilesparam.init_file_size ;
		else if( p_memfiles->memfilesparam.increase_file_size >= 0 )
			file_size = p_memfiles->memfilesparam.init_file_size + p_memfiles->memfilesparam.increase_file_size * ( p_memfiles->memfiles_array_count - 1 ) ;
		else
			file_size = p_memfiles->memfilesparam.init_file_size * (-p_memfiles->memfilesparam.increase_file_size) * ( p_memfiles->memfiles_array_count - 1 ) ;
		
		if( file_size < MASizeOfMemoryPageHeader() + MASizeOfMemoryBlockHeader() + msg_size )
			file_size = MASizeOfMemoryPageHeader() + MASizeOfMemoryBlockHeader() + msg_size ;
		
		p_memfile = MFCreateMemoryFile( pathfilename , file_size ) ;
		if( p_memfile == NULL )
		{
			return MEMQUEUE_ERROR_ALLOC;
		}
		
		p_memfiles->memfiles_array[p_memfiles->memfiles_array_count-1] = p_memfile ;
		p_memfiles->memfile_current_index = p_memfiles->memfiles_array_count-1 ;
	}
}

static long _MFPopupMemoryQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char **pp_msg , struct MemoryBlock **pp_memblock , long msg_bufsize )
{
	struct MemoryPage	*p_root_mempage = NULL ;
	
	if( p_memfiles->memfiles_array_count == 0 )
		return MEMQUEUE_ERROR_MESSAGE_NOT_FOUND;
	
	while(1)
	{
		(*pp_msg) = MFTravelMemoryQueueMessages( p_memfiles , NULL , (*pp_msg) ) ;
		if( (*pp_msg) == NULL )
			break;
		
		(*pp_memblock) = P2MEMBLOCK( (*pp_msg) ) ;
		if( CHECK_MEMBLOCK_MAGIC(*pp_memblock) )
		{
			errno = EINVAL ;
			return MEMQUEUE_ERROR_INTERNAL;
		}
		
		if( (*p_msg_type) == 0 )
		{
			break;
		}
		else if( (*p_msg_type) > 0 && (*p_msg_type) == (*pp_memblock)->ds.queue.msg_type )
		{
			break;
		}
		else if( (*p_msg_type) < 0 && (*pp_memblock)->ds.queue.msg_type <= abs(*p_msg_type) )
		{
			break;
		}
	}
	if( (*pp_msg) == NULL )
		return MEMQUEUE_ERROR_MESSAGE_NOT_FOUND;
	
	p_root_mempage = p_memfiles->memfiles_array[0]->p_mempage ;
	
	if( msg_bufsize > 0 && msg_bufsize < (*pp_memblock)->block_size )
	{
		(*pp_msg) = NULL ;
		return (*pp_memblock)->block_size;
	}
	
	if( (*pp_memblock)->ds.queue.queue_prev_offset == 0 && (*pp_memblock)->ds.queue.queue_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|              |
		*/
		p_root_mempage->ds.queue.queue_first_fileindex = 0 ;
		p_root_mempage->ds.queue.queue_first_offset = 0 ;
		p_root_mempage->ds.queue.queue_last_fileindex = 0 ;
		p_root_mempage->ds.queue.queue_last_offset = 0 ;
	}
	else if( (*pp_memblock)->ds.queue.queue_prev_offset == 0 )
	{
		/*
			|H|                     |
			|H|REMOVE|  |BLOCK|     |
		*/
		struct MemoryBlock	*p_memblock_queue_next = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[(*pp_memblock)->ds.queue.queue_next_fileindex]->p_mempage) + (*pp_memblock)->ds.queue.queue_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_next) )
			return MEMQUEUE_ERROR_INTERNAL;
		
		p_memblock_queue_next->ds.queue.queue_prev_fileindex = 0 ;
		p_memblock_queue_next->ds.queue.queue_prev_offset = 0 ;
		p_root_mempage->ds.queue.queue_first_fileindex = p_memblock_queue_next->fileindex ;
		p_root_mempage->ds.queue.queue_first_offset = p_memblock_queue_next->addr_this_offset ;
	}
	else if( (*pp_memblock)->ds.queue.queue_next_offset == 0 )
	{
		/*
			|H|                     |
			|H|BLOCK|  |REMOVE|     |
		*/
		
		struct MemoryBlock	*p_memblock_queue_prev = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[(*pp_memblock)->ds.queue.queue_prev_fileindex]->p_mempage) + (*pp_memblock)->ds.queue.queue_prev_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_prev) )
			return MEMQUEUE_ERROR_INTERNAL;
		
		p_memblock_queue_prev->ds.queue.queue_next_fileindex = 0 ;
		p_memblock_queue_prev->ds.queue.queue_next_offset = 0 ;
		p_root_mempage->ds.queue.queue_last_fileindex = p_memblock_queue_prev->fileindex ;
		p_root_mempage->ds.queue.queue_last_offset = p_memblock_queue_prev->addr_this_offset ;
	}
	else
	{
		/*
			|H|                         |
			|H|BLOCK|REMOVE|   |BLOCK|  |
		*/
		struct MemoryBlock	*p_memblock_queue_prev = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[(*pp_memblock)->ds.queue.queue_prev_fileindex]->p_mempage) + (*pp_memblock)->ds.queue.queue_prev_offset ) ;
		struct MemoryBlock	*p_memblock_queue_next = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[(*pp_memblock)->ds.queue.queue_next_fileindex]->p_mempage) + (*pp_memblock)->ds.queue.queue_next_offset ) ;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_prev) )
			return MEMQUEUE_ERROR_INTERNAL;
		if( CHECK_MEMBLOCK_MAGIC(p_memblock_queue_next) )
			return MEMQUEUE_ERROR_INTERNAL;
		
		p_memblock_queue_prev->ds.queue.queue_next_fileindex = p_memblock_queue_next->fileindex ;
		p_memblock_queue_prev->ds.queue.queue_next_offset = p_memblock_queue_next->addr_this_offset ;
		p_memblock_queue_next->ds.queue.queue_prev_fileindex = p_memblock_queue_prev->fileindex ;
		p_memblock_queue_next->ds.queue.queue_prev_offset = p_memblock_queue_prev->addr_this_offset ;
	}
	
	(*p_msg_type) = (*pp_memblock)->ds.queue.msg_type ;
	
	return 0;
}

long MFPopupMemoryQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char *msg_buf , long msg_bufsize )
{
	char			*p = NULL ;
	struct MemoryBlock	*p_memblock_queue_remove = NULL ;
	
	long			lret = 0 ;
	
	lret = _MFPopupMemoryQueueMessage( p_memfiles , p_msg_type , & p , & p_memblock_queue_remove , msg_bufsize ) ;
	if( lret )
		return 0;
	
	memcpy( msg_buf , p , p_memblock_queue_remove->block_size );
	
	MAFreeMemoryBlock( MEMBLOCK2MEMPAGE(p_memblock_queue_remove) , p );
	
	return 0;
}

long MAPopdmpMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char **pp_msg , long *p_msg_size )
{
	char			*p = NULL ;
	struct MemoryBlock	*p_memblock_queue_remove = NULL ;
	
	long			lret = 0 ;
	
	lret = _MFPopupMemoryQueueMessage( p_memfiles , p_msg_type , & p , & p_memblock_queue_remove , 0 ) ;
	if( lret )
		return 0;
	
	(*pp_msg) = (char*)malloc( p_memblock_queue_remove->block_size ) ;
	if( (*pp_msg) == NULL )
		return 0;
	memcpy( (*pp_msg) , p , p_memblock_queue_remove->block_size );
	if( p_msg_size )
		(*p_msg_size) = p_memblock_queue_remove->block_size ;
	
	MAFreeMemoryBlock( MEMBLOCK2MEMPAGE(p_memblock_queue_remove) , p );
	
	return 0;
}

char *MFTravelMemoryQueueMessages( struct MemoryFiles *p_memfiles , int *p_msg_type , char *p_msg_data )
{
	struct MemoryPage	*p_root_mempage = NULL ;
	struct MemoryBlock	*p_memblock = NULL ;
	
	if( p_msg_data == NULL )
	{
		if( p_memfiles->memfiles_array_count == 0 )
			return NULL;
		
		p_root_mempage = p_memfiles->memfiles_array[0]->p_mempage ;
		if( p_root_mempage->ds.queue.queue_first_offset == 0 )
			return NULL;
		
		p_memblock = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[p_root_mempage->ds.queue.queue_first_fileindex]->p_mempage) + p_root_mempage->ds.queue.queue_first_offset ) ;
		if( p_msg_type )
			(*p_msg_type) = p_memblock->ds.queue.msg_type ;
		return MEMBLOCK2P(p_memblock);
	}
	else
	{
		
		p_memblock = P2MEMBLOCK(p_msg_data) ;
		if( p_memblock->ds.queue.queue_next_offset == 0 )
			return NULL;
		
		p_memblock = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[p_memblock->ds.queue.queue_next_fileindex]->p_mempage) + p_memblock->ds.queue.queue_next_offset ) ;
		if( p_msg_type )
			(*p_msg_type) = p_memblock->ds.queue.msg_type ;
		return MEMBLOCK2P(p_memblock);
	}
}

char *MFTravelMemoryQueueMessagesReverse( struct MemoryFiles *p_memfiles , int *p_msg_type , char *p_msg_data )
{
	struct MemoryPage	*p_root_mempage = NULL ;
	struct MemoryBlock	*p_memblock = NULL ;
	
	if( p_msg_data == NULL )
	{
		if( p_memfiles->memfiles_array_count == 0 )
			return NULL;
		
		p_root_mempage = p_memfiles->memfiles_array[0]->p_mempage ;
		if( p_root_mempage->ds.queue.queue_last_offset == 0 )
			return NULL;
		
		p_memblock = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[p_root_mempage->ds.queue.queue_last_fileindex]->p_mempage) + p_root_mempage->ds.queue.queue_last_offset ) ;
		if( p_msg_type )
			(*p_msg_type) = p_memblock->ds.queue.msg_type ;
		return MEMBLOCK2P(p_memblock);
	}
	else
	{
		
		p_memblock = P2MEMBLOCK(p_msg_data) ;
		if( p_memblock->ds.queue.queue_prev_offset == 0 )
			return NULL;
		
		p_memblock = (struct MemoryBlock *)( (char*)(p_memfiles->memfiles_array[p_memblock->ds.queue.queue_prev_fileindex]->p_mempage) + p_memblock->ds.queue.queue_prev_offset ) ;
		if( p_msg_type )
			(*p_msg_type) = p_memblock->ds.queue.msg_type ;
		return MEMBLOCK2P(p_memblock);
	}
}

