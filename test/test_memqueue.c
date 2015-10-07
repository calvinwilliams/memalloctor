#include "memqueue.h"

static void TravelMemoryBlockByQueue( struct MemoryPage *p_mempage )
{
	int		msg_type ;
	void		*p = NULL ;
	
	printf( "TravelMemoryBlockByQueue\n" );
	printf( "MEMORYPAGE addr[0x%p] - TotalSize[%ld]bytes BlockCount[%ld]s UsedTotalSize[%ld]bytes - MemoryPageHeader[%ld]bytes MemoryBlockHeader[%ld]bytes\n"
		, p_mempage
		, MAGetMemoryTotalSize(p_mempage) , MAGetMemoryBlockCount(p_mempage) , MAGetMemoryUsedTotalSize(p_mempage)
		, MASizeOfMemoryPageHeader() , MASizeOfMemoryBlockHeader() );
	
	if( MAGetMemoryBlockCount(p_mempage) == 0 )
	{
			printf( "	NO MEMORYBLOCK\n" );
	}
	else
	{
		printf( "	ADDR(NEXT)\n" );
		while(1)
		{
			p = MATravelNextMemoryBlocks( p_mempage , p ) ;
			if( p == NULL )
				break;
			printf( "		MEMORYBLOCK - p offset[%ld] addr[0x%p] data[%ld][%s]\n" , LPTRDIFF(p,p_mempage) , p , MAGetMemoryBlockSize(p) , (char*)p );
		}
		
		printf( "	QUEUE(NEXT)\n" );
		while(1)
		{
			p = MATravelMemoryQueueMessages( p_mempage , & msg_type , p ) ;
			if( p == NULL )
				break;
			printf( "		QUEUEMESSAGE - p offset[%ld] addr[0x%p] msg_type[%d]data[%ld][%s]\n" , LPTRDIFF(p,p_mempage) , p , msg_type , MAGetMemoryBlockSize(p) , (char*)p );
		}
	}
	
	return;
}

static int test_MAPushMemoryBlockByQueue()
{
	char			buffer[ 1024 ] ;
	int			msg_type ;
	char			msg_data[ 1024 ] ;
	long			msg_size ;
	struct MemoryPage	*p_mempage = NULL ;
	char			*p_msg_data = NULL ;
	
	long			lret = 0 ;
	
	p_mempage = MAFormatMemoryPage( buffer , MASizeOfMemoryPageHeader() + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 ) ;
	printf( "MAInitMemoryPage ok\n" );
	
	TravelMemoryBlockByQueue( p_mempage );
	
	memset( msg_data , 0x00 , sizeof(msg_data) );
	
	msg_type = 1 ;
	strcpy( msg_data , "m1" );
	lret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , 10 ) ;
	if( lret )
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] failed[%ld]\n" , msg_type , msg_data , lret );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	msg_type = 2 ;
	strcpy( msg_data , "m2" );
	lret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , 10 ) ;
	if( lret )
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] failed[%ld]\n" , msg_type , msg_data , lret );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	msg_type = 3 ;
	strcpy( msg_data , "m3" );
	lret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , 10 ) ;
	if( lret )
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] failed[%ld]\n" , msg_type , msg_data , lret );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	msg_type = 4 ;
	strcpy( msg_data , "m4" );
	lret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , 10 ) ;
	if( lret )
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] failed[%ld]\n" , msg_type , msg_data , lret );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	msg_type = 5 ;
	strcpy( msg_data , "m5" );
	lret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , 10 ) ;
	if( lret )
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] failed[%ld]\n" , msg_type , msg_data , lret );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	TravelMemoryBlockByQueue( p_mempage );
	
	msg_type = 2 ;
	lret = MAPopupMemoryQueueMessage( p_mempage , & msg_type , msg_data , sizeof(msg_data) ) ;
	if( lret )
	{
		printf( "MAPopupMemoryQueueMessage [%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MAPopupMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	msg_type = 3 ;
	lret = MAPopupMemoryQueueMessage( p_mempage , & msg_type , msg_data , 9 ) ;
	if( lret )
	{
		printf( "MAPopupMemoryQueueMessage [%d] failed[%ld]\n" , msg_type , lret );
	}
	else
	{
		printf( "MAPopupMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
		return -1;
	}
	
	msg_type = 33 ;
	lret = MAPopupMemoryQueueMessage( p_mempage , & msg_type , msg_data , sizeof(msg_data) ) ;
	if( lret )
	{
		printf( "MAPopupMemoryQueueMessage [%d] failed[%ld]\n" , msg_type , lret );
	}
	else
	{
		printf( "MAPopupMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
		return -1;
	}
	
	msg_type = 4 ;
	lret = MAPopupMemoryQueueMessage( p_mempage , & msg_type , msg_data , sizeof(msg_data) ) ;
	if( lret )
	{
		printf( "MAPopupMemoryQueueMessage [%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MAPopupMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	TravelMemoryBlockByQueue( p_mempage );
	
	msg_type = 2 ;
	strcpy( msg_data , "m2" );
	lret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , 10 ) ;
	if( lret )
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] failed[%ld]\n" , msg_type , msg_data , lret );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	msg_type = 4 ;
	strcpy( msg_data , "m4" );
	lret = MAPushMemoryQueueMessage( p_mempage , msg_type , msg_data , 10 ) ;
	if( lret )
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] failed[%ld]\n" , msg_type , msg_data , lret );
		return -1;
	}
	else
	{
		printf( "MAPushMemoryQueueMessage [%d][%s] ok\n" , msg_type , msg_data );
	}
	
	TravelMemoryBlockByQueue( p_mempage );
	
	msg_type = -2 ;
	lret = MAPopdmpMemoryQueueMessage( p_mempage , & msg_type , & p_msg_data , NULL ) ;
	if( lret )
	{
		printf( "MAPopdmpMemoryQueueMessage [%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MAPopdmpMemoryQueueMessage [%d][%s] ok\n" , msg_type , p_msg_data );
		free(p_msg_data);
	}
	
	msg_type = -2 ;
	lret = MAPopdmpMemoryQueueMessage( p_mempage , & msg_type , & p_msg_data , NULL ) ;
	if( lret )
	{
		printf( "MAPopdmpMemoryQueueMessage [%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MAPopdmpMemoryQueueMessage [%d][%s] ok\n" , msg_type , p_msg_data );
		free(p_msg_data);
	}
	
	TravelMemoryBlockByQueue( p_mempage );
	
	msg_type = -2 ;
	lret = MAPopdmpMemoryQueueMessage( p_mempage , & msg_type , & p_msg_data , & msg_size ) ;
	if( lret )
	{
		printf( "MAPopdmpMemoryQueueMessage [%d] failed[%ld]\n" , msg_type , lret );
	}
	else
	{
		printf( "MAPopdmpMemoryQueueMessage [%d][%s] ok , msg_size[%ld]\n" , msg_type , p_msg_data , msg_size );
		free(p_msg_data);
		return -1;
	}
	
	TravelMemoryBlockByQueue( p_mempage );
	
	MACleanMemoryPage( p_mempage );
	
	TravelMemoryBlockByQueue( p_mempage );
	
	return 0;
}

int main()
{
	int		nret = 0 ;
	
	nret = test_MAPushMemoryBlockByQueue() ;
	if( nret )
	{
		printf( "Testing failure\n" );
	}
	else
	{
		printf( "All is ok\n" );
	}
	
	return 0;
}
