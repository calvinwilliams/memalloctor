#include "memqueue.h"
#include "memfile.h"

#define TEST_MEMFILE_FILENAME	"memfile/test_memqueue.bin"
#define TEST_MEMFILE_FILESIZE	1024*1024

int test_memfile_create()
{
	struct MemoryFile	*p_memfile = NULL ;
	
	p_memfile = MFCreateMemoryFile( TEST_MEMFILE_FILENAME , TEST_MEMFILE_FILESIZE ) ;
	if( p_memfile == NULL )
	{
		printf( "MFCreateMemoryFile[%s][%d] failed\n" , TEST_MEMFILE_FILENAME , TEST_MEMFILE_FILESIZE );
		return -1;
	}
	else
	{
		printf( "MFCreateMemoryFile[%s][%d] ok\n" , TEST_MEMFILE_FILENAME , TEST_MEMFILE_FILESIZE );
	}
	
	MFCloseMemoryFile( p_memfile );
	printf( "MFCloseMemoryFile ok\n" );
	
	return 0;
}

int test_memfile_send( int msg_type , char *msg_data )
{
	struct MemoryFile	*p_memfile = NULL ;
	struct MemoryPage	*p_mempage = NULL ;
	
	unsigned long		ulret = 0 ;
	
	p_memfile = MFOpenMemoryFile( TEST_MEMFILE_FILENAME ) ;
	if( p_memfile == NULL )
	{
		printf( "MFOpenMemoryFile[%s] failed\n" , TEST_MEMFILE_FILENAME );
		return -1;
	}
	else
	{
		printf( "MFOpenMemoryFile[%s] ok\n" , TEST_MEMFILE_FILENAME );
		
		p_mempage = MFGetMemoryPage( p_memfile ) ;
		printf( "	MemoryTotalSize      [%ld]bytes\n" , MAGetMemoryTotalSize(p_mempage) );
		printf( "	MemoryBlockCount     [%ld]bytes\n" , MAGetMemoryBlockCount(p_mempage) );
		printf( "	MemoryUsedTotalSize  [%ld]bytes\n" , MAGetMemoryUsedTotalSize(p_mempage) );
		printf( "	MemoryUnusedTotalSize[%ld]bytes\n" , MAGetMemoryUnusedTotalSize(p_mempage) );
	}
	
	ulret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , strlen(msg_data)+1 ) ;
	if( ulret == 0 )
	{
		printf( "MAPushMemoryQueueMessage[%d][%s] failed\n" , msg_type , msg_data );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage[%d][%s] ok\n" , msg_type , msg_data );
	}
	
	MFCloseMemoryFile( p_memfile );
	printf( "MFCloseMemoryFile ok\n" );
	
	return 0;
}

int test_memfile_recv( int msg_type )
{
	struct MemoryFile	*p_memfile = NULL ;
	struct MemoryPage	*p_mempage = NULL ;
	char			*p_msg_data = NULL ;
	
	unsigned long		ulret = 0 ;
	
	p_memfile = MFOpenMemoryFile( TEST_MEMFILE_FILENAME ) ;
	if( p_memfile == NULL )
	{
		printf( "MFOpenMemoryFile[%s] failed\n" , TEST_MEMFILE_FILENAME );
		return -1;
	}
	else
	{
		printf( "MFOpenMemoryFile[%s] ok\n" , TEST_MEMFILE_FILENAME );
		
		p_mempage = MFGetMemoryPage( p_memfile ) ;
		printf( "	MemoryTotalSize      [%ld]bytes\n" , MAGetMemoryTotalSize(p_mempage) );
		printf( "	MemoryBlockCount     [%ld]bytes\n" , MAGetMemoryBlockCount(p_mempage) );
		printf( "	MemoryUsedTotalSize  [%ld]bytes\n" , MAGetMemoryUsedTotalSize(p_mempage) );
		printf( "	MemoryUnusedTotalSize[%ld]bytes\n" , MAGetMemoryUnusedTotalSize(p_mempage) );
	}
	
	ulret = MAPopdmpMemoryQueueMessage( p_mempage , & msg_type , & p_msg_data ) ;
	if( ulret == 0 )
	{
		printf( "MAPopmpMemoryQueueMessage[%d] failed[%ld]\n" , msg_type , ulret );
	}
	else
	{
		printf( "MAPopmpMemoryQueueMessage[%d][%s] ok\n" , msg_type , p_msg_data );
		free( p_msg_data );
		return -1;
	}
	
	MFCloseMemoryFile( p_memfile );
	printf( "MFCloseMemoryFile ok\n" );
	
	return 0;
}

int test_memfile_travel()
{
	struct MemoryFile	*p_memfile = NULL ;
	struct MemoryPage	*p_mempage = NULL ;
	int			msg_type ;
	void			*p_msg_data = NULL ;
	
	p_memfile = MFOpenMemoryFile( TEST_MEMFILE_FILENAME ) ;
	if( p_memfile == NULL )
	{
		printf( "MFOpenMemoryFile[%s] failed\n" , TEST_MEMFILE_FILENAME );
		return -1;
	}
	else
	{
		printf( "MFOpenMemoryFile[%s] ok\n" , TEST_MEMFILE_FILENAME );
		
		p_mempage = MFGetMemoryPage( p_memfile ) ;
		printf( "	MemoryTotalSize      [%ld]bytes\n" , MAGetMemoryTotalSize(p_mempage) );
		printf( "	MemoryBlockCount     [%ld]bytes\n" , MAGetMemoryBlockCount(p_mempage) );
		printf( "	MemoryUsedTotalSize  [%ld]bytes\n" , MAGetMemoryUsedTotalSize(p_mempage) );
		printf( "	MemoryUnusedTotalSize[%ld]bytes\n" , MAGetMemoryUnusedTotalSize(p_mempage) );
	}
	
	printf( "QUEUE(NEXT)\n" );
	while(1)
	{
		p_msg_data = MATravelMemoryQueueMessage( p_mempage , & msg_type , p_msg_data ) ;
		if( p_msg_data == NULL )
			break;
		printf( "	QUEUEMESSAGE - p offset[%ld] addr[0x%p] msg_type[%d]data[%ld][%s]\n" , LPTRDIFF(p_msg_data,p_mempage) , p_msg_data , msg_type , MAGetMemoryBlockSize(p_msg_data) , (char*)p_msg_data );
	}
	
	MFCloseMemoryFile( p_memfile );
	printf( "MFCloseMemoryFile ok\n" );
	
	return 0;
}

static void usage()
{
	printf( "USAGE : test_memfile [ create | send msgtype (...msg...) | recv msgtype  | travel ]\n" );
	return;
}

int main( int argc , char *argv[] )
{
	if( argc == 1 + 1 && strcmp( argv[1] , "create" ) == 0 )
	{
		return -test_memfile_create();
	}
	else if( argc == 1 + 3 && strcmp( argv[1] , "send" ) == 0 )
	{
		return -test_memfile_send( atol(argv[2]) , argv[3] );
	}
	else if( argc == 1 + 2 && strcmp( argv[1] , "recv" ) == 0 )
	{
		return -test_memfile_recv( atol(argv[2]) );
	}
	else if( argc == 1 + 1 && strcmp( argv[1] , "travel" ) == 0 )
	{
		return -test_memfile_travel();
	}
	else
	{
		usage();
		return 7;
	}
}

