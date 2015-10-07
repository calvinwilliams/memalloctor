#include "memtype.h"
#include "memfiles.h"

struct MemoryFilesParameter *MFAllocMemoryFilesParameter6( char *prefix_pathfilename , char *postfix_pathfilename , long max_file_count , long init_file_size , long increase_file_size , long max_file_size )
{
	struct MemoryFilesParameter *p_memfilesparam = NULL ;
	
	p_memfilesparam = (struct MemoryFilesParameter *)malloc( sizeof(struct MemoryFilesParameter) ) ;
	if( p_memfilesparam == NULL )
	{
		errno = ENOMEM ;
		return NULL;
	}
	memset( p_memfilesparam , 0x00 , sizeof(struct MemoryFilesParameter) );
	
	if( prefix_pathfilename )
		strncpy( p_memfilesparam->prefix_pathfilename , prefix_pathfilename , sizeof(p_memfilesparam->prefix_pathfilename) );
	if( postfix_pathfilename )
		strncpy( p_memfilesparam->postfix_pathfilename , postfix_pathfilename , sizeof(p_memfilesparam->postfix_pathfilename) );
	p_memfilesparam->max_file_count = max_file_count ;
	p_memfilesparam->init_file_size = init_file_size ;
	p_memfilesparam->increase_file_size = increase_file_size ;
	p_memfilesparam->max_file_size = max_file_size ;
	
	return p_memfilesparam;
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

int MFReallocMemoryFileArray( struct MemoryFiles *p_memfiles )
{
	if( p_memfiles->memfiles_array == NULL )
	{
		p_memfiles->memfiles_array = (struct MemoryFile **)malloc( sizeof(struct MemoryFile *) ) ;
		if( p_memfiles->memfiles_array == NULL )
		{
			errno = ENOMEM ;
			return MEMFILES_ERROR_ALLOC;
		}
		memset( p_memfiles->memfiles_array , 0x00 , sizeof(struct MemoryFile *) );
		
		p_memfiles->memfiles_array_count = 1 ;
		p_memfiles->memfile_current_index = 0 ;
	}
	else
	{
		struct MemoryFile	**tmp = NULL ;
		
		if( p_memfiles->memfilesparam.max_file_count >= 0 && p_memfiles->memfiles_array_count + 1 > p_memfiles->memfilesparam.max_file_count )
		{
			errno = ENOSPC ;
			return MEMFILES_ERROR_NOT_ENOUGH_SPACE;
		}
		
		tmp = (struct MemoryFile **)realloc( p_memfiles->memfiles_array , sizeof(struct MemoryFiles*) * ( p_memfiles->memfiles_array_count + 1 ) ) ;
		if( tmp == NULL )
		{
			errno = ENOMEM ;
			return MEMFILES_ERROR_ALLOC;
		}
		memset( tmp + p_memfiles->memfiles_array_count , 0x00 , sizeof(struct MemoryFile *) );
		p_memfiles->memfiles_array = tmp ;
		p_memfiles->memfiles_array_count++;
		p_memfiles->memfile_current_index++;
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
	
	if( p_memfiles->memfilesparam.max_file_count == 0 || p_memfiles->memfilesparam.init_file_size == 0 || p_memfiles->memfilesparam.max_file_size == 0 )
	{
		errno = EINVAL ;
		MFUnloadMemoryFiles( p_memfiles );
		return NULL;
	}
	
	while(1)
	{
		memset( pathfilename , 0x00 , sizeof(pathfilename) );
		snprintf( pathfilename , sizeof(pathfilename)-1 , "%s/%s%ld%s" , p_memfiles->pathdirname , p_memfiles->memfilesparam.prefix_pathfilename , p_memfiles->memfiles_array_count , p_memfiles->memfilesparam.postfix_pathfilename );
		nret = stat( pathfilename , & file_stat ) ;
		if( nret )
			break;
		
		nret = MFReallocMemoryFileArray( p_memfiles ) ;
		if( nret )
		{
			MFUnloadMemoryFiles( p_memfiles );
			return NULL;
		}
		
		p_memfiles->memfiles_array[p_memfiles->memfiles_array_count-1] = MFOpenMemoryFile( pathfilename ) ;
		if( p_memfiles->memfiles_array[p_memfiles->memfiles_array_count-1] == NULL )
		{
			MFUnloadMemoryFiles( p_memfiles );
			return NULL;
		}
	}
	
	return p_memfiles;
}

void MFUnloadMemoryFiles( struct MemoryFiles *p_memfiles )
{
	if( p_memfiles->memfiles_array )
	{
		for( p_memfiles->memfiles_array_count-- ; p_memfiles->memfiles_array_count >= 0 ; p_memfiles->memfiles_array_count-- )
		{
			if( p_memfiles->memfiles_array[p_memfiles->memfiles_array_count]->p_mempage )
			{
				MFCloseMemoryFile( p_memfiles->memfiles_array[p_memfiles->memfiles_array_count] );
			}
		}
		
		free( p_memfiles->memfiles_array );
	}
	
	free( p_memfiles );
	
	return;
}

unsigned long MFGetMemoryFilesCount( struct MemoryFiles *p_memfiles )
{
	return p_memfiles->memfiles_array_count;
}

unsigned long MFGetMemoryFileIndex( struct MemoryFiles *p_memfiles , struct MemoryPage *p_mempage )
{
	long		fileindex ;
	
	for( fileindex = 0 ; fileindex < p_memfiles->memfiles_array_count ; fileindex++ )
	{
		if( p_memfiles->memfiles_array[fileindex]->p_mempage == p_mempage )
			return fileindex;
	}
	
	return MEMFILES_ERROR_PARAMETER;
}

struct MemoryFile *MFGetMemoryFile( struct MemoryFiles *p_memfiles , unsigned long fileindex )
{
	if( fileindex >= p_memfiles->memfiles_array_count )
		return NULL;
	
	return p_memfiles->memfiles_array[fileindex];
}

