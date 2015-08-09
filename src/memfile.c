#include "memtype.h"
#include "memfile.h"

struct MemoryFile *MFCreateMemoryFile( char *pathfilename , unsigned long file_size )
{
	int			fd ;
	void			*paddr = NULL ;
	struct MemoryFile	*p_memfile = NULL ;
	
	fd = open( pathfilename , O_CREAT | O_EXCL | O_RDWR | O_TRUNC , S_IRWXU | S_IRWXG | S_IRWXO ) ;
	if( fd == -1 )
		return NULL;
	
	ftruncate( fd , file_size );
	
	paddr = mmap( NULL , file_size , PROT_READ | PROT_WRITE , MAP_SHARED , fd , 0 ) ;
	close( fd );
	if( paddr == NULL )
		return NULL;
	
	MAFormatMemoryPage( paddr , file_size ) ;
	
	p_memfile = (struct MemoryFile *)malloc( sizeof(struct MemoryFile) ) ;
	if( p_memfile == NULL )
	{
		munmap( paddr , file_size );
		return NULL;
	}
	memset( p_memfile , 0x00 , sizeof(struct MemoryFile) );
	strncpy( p_memfile->pathfilename , pathfilename , sizeof(p_memfile->pathfilename)-1 );
	p_memfile->p_mempage = paddr ;
	
	return p_memfile;
}

struct MemoryFile *MFOpenMemoryFile( char *pathfilename )
{
	int			fd ;
	struct stat		file_stat ;
	void			*paddr = NULL ;
	struct MemoryFile	*p_memfile = NULL ;
	
	fd = open( pathfilename , O_RDWR , S_IRWXU | S_IRWXG | S_IRWXO ) ;
	if( fd == -1 )
		return NULL;
	
	fstat( fd , & file_stat );
	
	paddr = mmap( NULL , file_stat.st_size , PROT_READ | PROT_WRITE , MAP_SHARED , fd , 0 ) ;
	close( fd );
	if( paddr == NULL )
		return NULL;
	
	p_memfile = (struct MemoryFile *)malloc( sizeof(struct MemoryFile) ) ;
	if( p_memfile == NULL )
	{
		munmap( paddr , file_stat.st_size );
		return NULL;
	}
	memset( p_memfile , 0x00 , sizeof(struct MemoryFile) );
	strncpy( p_memfile->pathfilename , pathfilename , sizeof(p_memfile->pathfilename)-1 );
	p_memfile->p_mempage = paddr ;
	
	return p_memfile;
}

void MFCloseMemoryFile( struct MemoryFile *p_memfile )
{
	struct MemoryPage	*p_mempage = p_memfile->p_mempage ;
	
	munmap( p_mempage , MAGetMemoryTotalSize(p_mempage) );
	
	free( p_memfile );
	
	return;
}

char *MFGetMemoryFilename( struct MemoryFile *p_memfile )
{
	return p_memfile->pathfilename;
}

struct MemoryPage *MFGetMemoryPage( struct MemoryFile *p_memfile )
{
	return p_memfile->p_mempage;
}

