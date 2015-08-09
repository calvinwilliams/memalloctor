#include "memtype.h"
#include "memfiles.h"

struct MemoryFilesParameter *MFAllocMemoryFilesParameter5( char *prefix_pathfilename , long max_file_count , long init_file_size , long increase_file_size , long max_file_size )
{
	struct MemoryFilesParameter *p_memfileparam = NULL ;
	
	p_memfileparam = (struct MemoryFilesParameter *)malloc( sizeof(struct MemoryFilesParameter) ) ;
	if( p_memfileparam == NULL )
	{
		errno = ENOMEM ;
		return NULL;
	}
	memset( p_memfileparam , 0x00 , sizeof(struct MemoryFilesParameter) );
	
	strncpy( p_memfileparam->prefix_pathfilename , prefix_pathfilename , sizeof(p_memfileparam->prefix_pathfilename) );
	p_memfileparam->max_file_count = max_file_count ;
	p_memfileparam->init_file_size = init_file_size ;
	p_memfileparam->increase_file_size = increase_file_size ;
	p_memfileparam->max_file_size = max_file_size ;
	
	return p_memfileparam;
}

static struct MemoryFiles *_MFAllocMemoryFiles( char *pathdirname )
{
	struct MemoryFiles	*p_memfiles = NULL ;
	
	p_memfiles = (struct MemoryFiles *)malloc( sizeof(struct MemoryFiles) ) ;
	if( p_memfiles == NULL )
		return NULL;
	memset( p_memfiles , 0x00 , sizeof(struct MemoryFiles) );
	
	p_memfiles->memfilesparam.max_file_count = MEMFILES_MAX_FILE_COUNT ;
	p_memfiles->memfilesparam.init_file_size = MEMFILES_INIT_FILE_SIZE ;
	p_memfiles->memfilesparam.increase_file_size = MEMFILES_INCREASE_FILE_SIZE ;
	p_memfiles->memfilesparam.max_file_size = MEMFILES_MAX_FILE_SIZE ;
	
	strncpy( p_memfiles->pathdirname , pathdirname , sizeof(p_memfiles->pathdirname) );
	
	return p_memfiles;
}

static int _MFReallocMemoryFileArray( struct MemoryFiles *p_memfiles )
{
	if( p_memfiles->memfile_array == NULL )
	{
		p_memfiles->memfile_array = (struct MemoryFile **)malloc( sizeof(struct MemoryFile *) ) ;
		if( p_memfiles->memfile_array == NULL )
		{
			errno = ENOMEM ;
			return -1;
		}
		memset( p_memfiles->memfile_array , 0x00 , sizeof(struct MemoryFile *) );
		
		p_memfiles->memfile_current_ptr = p_memfiles->memfile_array[0] ;
		p_memfiles->memfile_array_count = 1 ;
	}
	else
	{
		struct MemoryFile	**tmp = NULL ;
		unsigned long		current_index ;
		
		if( p_memfiles->memfile_array_count + 1 > p_memfiles->memfilesparam.max_file_count )
		{
			errno = ENOSPC ;
			return -1;
		}
		
		current_index = p_memfiles->memfile_current_ptr - p_memfiles->memfile_array[0] ;
		tmp = (struct MemoryFile **)realloc( p_memfiles->memfile_array , sizeof(struct MemoryFiles*) * ( p_memfiles->memfile_array_count + 1 ) ) ;
		if( tmp == NULL )
		{
			errno = ENOMEM ;
			return -1;
		}
		memset( tmp + p_memfiles->memfile_array_count , 0x00 , sizeof(struct MemoryFile *) );
		p_memfiles->memfile_array = tmp ;
		p_memfiles->memfile_array_count++;
		p_memfiles->memfile_current_ptr = p_memfiles->memfile_array[current_index] ;
	}
	
	return 0;
}

struct MemoryFiles *MFLoadMemoryFiles( char *pathdirname , struct MemoryFilesParameter *p_memfilesparam )
{
	struct MemoryFiles	*p_memfiles = NULL ;
	
	char			pathfilename[ MEMFILE_MAXLEN_FILENAME + 1 ] ;
	struct stat		file_stat ;
	
	int			nret = 0 ;
	
	p_memfiles = _MFAllocMemoryFiles( pathdirname ) ;
	if( p_memfiles == NULL )
	{
		return NULL;
	}
	
	if( p_memfilesparam )
	{
		memcpy( & (p_memfiles->memfilesparam) , p_memfilesparam , sizeof(struct MemoryFilesParameter) );
		free( p_memfilesparam );
	}
	
	if( p_memfiles->memfilesparam.max_file_count == 0 || p_memfiles->memfilesparam.init_file_size == 0 || p_memfiles->memfilesparam.increase_file_size == 0 || p_memfiles->memfilesparam.max_file_size == 0 )
	{
		errno = EINVAL ;
		MFUnloadMemoryFiles( p_memfiles );
		return NULL;
	}
	
	while(1)
	{
		memset( pathfilename , 0x00 , sizeof(pathfilename) );
		snprintf( pathfilename , sizeof(pathfilename)-1 , "%s/%s%ld" , p_memfiles->pathdirname , p_memfiles->memfilesparam.prefix_pathfilename , p_memfiles->memfile_array_count );
		nret = stat( pathfilename , & file_stat ) ;
		if( nret )
			break;
		
		nret = _MFReallocMemoryFileArray( p_memfiles ) ;
		if( nret )
		{
			MFUnloadMemoryFiles( p_memfiles );
			return NULL;
		}
		
		p_memfiles->memfile_array[p_memfiles->memfile_array_count] = MFOpenMemoryFile( pathfilename ) ;
		if( nret )
		{
			MFUnloadMemoryFiles( p_memfiles );
			return NULL;
		}
	}
	
	return p_memfiles;
}

void MFUnloadMemoryFiles( struct MemoryFiles *p_memfiles )
{
	if( p_memfiles->memfile_array )
	{
		for( p_memfiles->memfile_array_count-- ; p_memfiles->memfile_array_count >= 0 ; p_memfiles->memfile_array_count-- )
		{
			if( p_memfiles->memfile_array[p_memfiles->memfile_array_count]->p_mempage )
			{
				MFCloseMemoryFile( p_memfiles->memfile_array[p_memfiles->memfile_array_count] );
			}
		}
		
		free( p_memfiles->memfile_array );
	}
	
	free( p_memfiles );
	
	return;
}

#if 0

int MFAddMemoryBlock( struct MemoryFiles *p_memfiles , char *block_base , unsigned long block_size , struct MemoryPage **pp_mempage , struct MemoryBlock **pp_memblock )
{
	unsigned long		memfile_array_used_no ;
	
	char			pathfilename[ MEMFILE_MAXLEN_FILENAME + 1 ] ;
	struct stat		file_stat ;
	struct MemoryFile	*p_memfile = NULL ;
	unsigned long		file_size ;
	
	struct MemoryBlock	*p_memblock = NULL ;
	
	int			nret = 0 ;
	
	for( memfile_array_used_no = 0 ; p_memfiles->memfile_current_ptr && memfile_array_used_no < p_memfiles->memfile_array_used_count ; memfile_array_used_no++ )
	{
		nret = MAAddString( p_memfiles->memfile_current_ptr->p_mempage , block_base , block_size , pp_memblock ) ;
		if( nret == 0 )
			return 0;
		else if( nret != MEMALC_ERROR_NOT_ENOUGH_SPACE )
			return nret;
		
		p_memfiles->memfile_current_ptr++;
		if( p_memfiles->memfile_current_ptr > & (p_memfiles->memfile_array[p_memfiles->memfile_array_used_count]) )
			p_memfiles->memfile_current_ptr = & (p_memfiles->memfile_array[0]) ;
	}
	
	memset( pathfilename , 0x00 , sizeof(pathfilename) );
	snprintf( pathfilename , sizeof(pathfilename)-1 , "%s/%ld" , p_memfiles->pathdirname , p_memfiles->memfile_array_used_count );
	nret = stat( pathfilename , & file_stat ) ;
	if( nret == 0 )
		return MEMFILE_ERROR_FILE_EXIST;
	
	if( p_memfiles->memfile_array == NULL || p_memfiles->memfile_array_used_count + 1 > p_memfiles->memfile_array_total_count )
	{
		struct MemoryFile	*tmp = NULL ;
		unsigned long		memfile_array_new_count ;
		
		if( p_memfiles->memfile_array_total_count == 0 )
			memfile_array_new_count = 10 ;
		else
			memfile_array_new_count = p_memfiles->memfile_array_total_count + 10 ;
		
		tmp = (struct MemoryFile *)realloc( p_memfiles->memfile_array , sizeof(struct MemoryFile) * memfile_array_new_count ) ;
		if( tmp == NULL )
		{
			return MEMFILE_ERROR_ALLOC;
		}
		
		p_memfiles->memfile_array = tmp ;
		p_memfiles->memfile_array_total_count = memfile_array_new_count ;
	}
	
	if( p_memfiles->memfile_current_ptr == NULL )
		p_memfiles->memfile_current_ptr = p_memfiles->memfile_array ;
	
	p_memfile = p_memfiles->memfile_array + p_memfiles->memfile_array_used_count ;
	
	if( block_size > MEMFILE_DEFAULT_FILESIZE )
		file_size = block_size ;
	else
		file_size = MEMFILE_DEFAULT_FILESIZE * ( 2 ^ (p_memfile-p_memfiles->memfile_array) ) ;
	
	nret = MFCreateMemoryFile( pathfilename , file_size , p_memfile ) ;
	if( nret )
	{
		return nret;
	}
	
	p_memfiles->memfile_array_used_count++;
	
	nret = MAAddString( p_memfile->p_mempage , block_base , block_size , & p_memblock ) ;
	if( nret )
	{
		return nret;
	}
	
	if( pp_mempage )
		(*pp_mempage) = p_memfile->p_mempage ;
	if( pp_memblock )
		(*pp_memblock) = p_memblock ;
	
	return 0;
}

int MFRemoveMemoryBlock( struct MemoryFiles *p_memfiles , struct MemoryPage *p_mempage , struct MemoryBlock *p_memblock )
{
	return MARemoveString( p_mempage , p_memblock );
}

int MFTravelMemoryFile( struct MemoryFiles *p_memfiles , struct MemoryFile **pp_memfile )
{
	if( p_memfiles->memfile_array_used_count == 0 )
		return MEMALC_WARN_NO_BLOCK;
	
	if( (*pp_memfile) == NULL )
	{
		(*pp_memfile) = p_memfiles->memfile_array ;
	}
	else
	{
		(*pp_memfile)++;
	}
	
	if( (*pp_memfile) < p_memfiles->memfile_array + p_memfiles->memfile_array_used_count )
	{
		return 0;
	}
	else
	{
		return MEMFILE_WARN_NO_FILE;
	}
}

int MFTravelMemoryBlockByAddr( struct MemoryFiles *p_memfiles , struct MemoryFile **pp_memfile , struct MemoryBlock **pp_memblock )
{
	int			nret = 0 ;
	
	if( p_memfiles->memfile_array_used_count == 0 )
		return MEMALC_WARN_NO_BLOCK;
	
	if( (*pp_memfile) == NULL )
	{
		(*pp_memfile) = p_memfiles->memfile_array ;
		(*pp_memblock) = NULL ;
	}
	
	for( ; (*pp_memfile) < p_memfiles->memfile_array + p_memfiles->memfile_array_used_count ; (*pp_memfile)++ )
	{
		nret = MATravelMemoryBlockByAddr( (*pp_memfile)->p_mempage , pp_memblock ) ;
		if( nret == 0 )
			return 0;
		else if( nret == MEMALC_WARN_NO_BLOCK )
			continue;
		else
			return nret;
	}
	
	return MEMALC_WARN_NO_BLOCK;
}

#endif

#if 0

unsigned long MAPushMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int msg_type , char *msg_data , unsigned long msg_size )
{
	unsigned long		current_index ;
	
	unsigned long		ulret = 0 ;
	
	if( msg_size > p_memfiles->memfilesparam.max_file_size )
	{
		errno = EINVAL ;
		return 0;
	}
	
	for( current_index = 0 ; p_memfiles->memfile_current_ptr && current_index < p_memfiles->memfile_array_count ; current_index++ )
	{
		ulret = MAPushMemoryQueueMessage( p_memfiles->memfile_current_ptr->p_mempage , msg_type , msg_data , msg_size ) ;
		if( ulret == 0 )
			return 0;
		
		p_memfiles->memfile_current_ptr++;
		if( p_memfiles->memfile_current_ptr > & (p_memfiles->memfile_array[p_memfiles->memfile_array_count]) )
			p_memfiles->memfile_current_ptr = p_memfiles->memfile_array ;
	}
	
_GOTO_EXPANSION :
	
	memset( pathfilename , 0x00 , sizeof(pathfilename) );
	snprintf( pathfilename , sizeof(pathfilename)-1 , "%s/%s%ld" , p_memfiles->pathdirname , p_memfiles->memfilesparam.prefix_pathfilename , p_memfiles->memfile_array_count );
	nret = stat( pathfilename , & file_stat ) ;
	if( nret == 0 )
	{
		errno = EEXIST ;
		return 0;
	}
	
	nret = _MFReallocMemoryFileArray( p_memfiles ) ;
	if( nret )
		return nret;
	p_memfiles->memfile_current_ptr = p_memfiles->memfile_array[p_memfiles->memfile_array_count-1] ;
	
	if( p_memfiles->memfilesparam.increase_file_size > 0 )
		file_size = p_memfiles->memfilesparam.init_file_size + p_memfiles->memfilesparam.increase_file_size * ( p_memfiles->memfile_array_count - 1 ) ;
	else
		file_size = p_memfiles->memfilesparam.init_file_size * (-p_memfiles->memfilesparam.increase_file_size) * ( p_memfiles->memfile_array_count - 1 ) ;
	
	nret = MFCreateMemoryFile( pathfilename , file_size , p_memfile ) ;
	if( nret )
	{
		return nret;
	}
	
	ulret = MAPushMemoryQueueMessage( p_memfiles->memfile_current_ptr->p_mempage , msg_type , msg_data , msg_size ) ;
	if( ulret == 0 )
		goto _GOTO_EXPANSION;
	
	return 0;
}

unsigned long MAPopupMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char *msg_buf , unsigned long msg_bufsize )
{
	unsigned long		current_index ;
	
	unsigned long		ulret = 0 ;
	
	for( current_index = 0 ; p_memfiles->memfile_current_ptr && current_index < p_memfiles->memfile_array_count ; current_index++ )
	{
		ulret = MAPopupMemoryQueueMessage( p_memfiles->memfile_current_ptr->p_mempage , p_msg_type , msg_buf , msg_bufsize ) ;
		if( ulret > 0 )
		{
			return ulret;
		}
		
		p_memfiles->memfile_current_ptr++;
		if( p_memfiles->memfile_current_ptr > & (p_memfiles->memfile_array[p_memfiles->memfile_array_count]) )
			p_memfiles->memfile_current_ptr = p_memfiles->memfile_array ;
	}
	
	return 0;
}

unsigned long MAPopdmpMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char **pp_msg_data )
{
	unsigned long		current_index ;
	
	unsigned long		ulret = 0 ;
	
	for( current_index = 0 ; p_memfiles->memfile_current_ptr && current_index < p_memfiles->memfile_array_count ; current_index++ )
	{
		ulret = MAPopupMemoryQueueMessage( p_memfiles->memfile_current_ptr->p_mempage , p_msg_type , pp_msg_data ) ;
		if( ulret > 0 )
		{
			return ulret;
		}
		
		p_memfiles->memfile_current_ptr++;
		if( p_memfiles->memfile_current_ptr > & (p_memfiles->memfile_array[p_memfiles->memfile_array_count]) )
			p_memfiles->memfile_current_ptr = p_memfiles->memfile_array ;
	}
	
	return 0;
}

char *MATravelMemoryFilesQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *p_msg_data )
{
}

#endif
