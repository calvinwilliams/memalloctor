#include "memqueue.h"

static void TravelMemoryBlockByQueue( struct MemoryFiles *p_memfiles )
{
	unsigned long		filecount ;
	unsigned long		fileindex ;
	struct MemoryFile	*p_memfile = NULL ;
	struct MemoryPage	*p_mempage = NULL ;
	int			msg_type ;
	void			*p = NULL ;
	
	filecount = MFGetMemoryFilesCount( p_memfiles ) ;
	for( fileindex = 0 ; fileindex < filecount ; fileindex++ )
	{
		p_memfile = MFGetMemoryFile( p_memfiles , fileindex ) ;
		p_mempage = MFGetMemoryPage( p_memfile ) ;
		
		printf( "[%ld] TravelMemoryBlockByQueue\n" , fileindex );
		printf( "MEMORYPAGE addr[0x%p] - TotalSize[%ld]bytes BlockCount[%ld]s UsedTotalSize[%ld]bytes - MemoryPageHeader[%ld]bytes MemoryBlockHeader[%ld]bytes\n"
			, p_mempage
			, MAGetMemoryTotalSize(p_mempage) , MAGetMemoryBlockCount(p_mempage) , MAGetMemoryUsedTotalSize(p_mempage)
			, MASizeOfMemoryPageHeader() , MASizeOfMemoryBlockHeader() );
		
		while(1)
		{
			p = MATravelNextMemoryBlocks( p_mempage , p ) ;
			if( p == NULL )
				break;
			printf( "	MEMORYBLOCK - p offset[%ld] addr[%p] data[%ld][%s]\n" , LPTRDIFF(p,p_mempage) , p , MAGetMemoryBlockSize(p) , (char*)p );
		}
	}
	
	printf( "[*] MEMORYFILES QUEUE\n" );
	while(1)
	{
		p = MFTravelMemoryQueueMessages( p_memfiles , & msg_type , p ) ;
		if( p == NULL )
			break;
		printf( "	QUEUEMESSAGE - p fileindex[%ld] offset[%ld] addr[%p] msg_type[%d]data[%ld][%s]\n" , MFGetMemoryFileIndex(p_memfiles,MAGetMemoryPage(p)) , LPTRDIFF(p,MAGetMemoryPage(p)) , p , msg_type , MAGetMemoryBlockSize(p) , (char*)p );
	}
	
	printf( "[*] MEMORYFILES QUEUE REVERSE\n" );
	while(1)
	{
		p = MFTravelMemoryQueueMessagesReverse( p_memfiles , & msg_type , p ) ;
		if( p == NULL )
			break;
		printf( "	QUEUEMESSAGE - p fileindex[%ld] offset[%ld] addr[%p] msg_type[%d]data[%ld][%s]\n" , MFGetMemoryFileIndex(p_memfiles,MAGetMemoryPage(p)) , LPTRDIFF(p,MAGetMemoryPage(p)) , p , msg_type , MAGetMemoryBlockSize(p) , (char*)p );
	}
	
	printf( "\n" );
	
	return;
}

static int test_MFPushMemoryQueueMessage()
{
	struct MemoryFilesParameter	*p_memfilesparam = NULL ;
	struct MemoryFiles		*p_memfiles = NULL ;
	char				msg_buf[ 1024 + 1 ] ;
	unsigned long			msg_len ;
	int				msg_type ;
	
	unsigned long			lret = 0 ;
	
	p_memfilesparam = MFAllocMemoryFilesParameter6( "memqueue_" , ".bin" , 10 , MASizeOfMemoryPageHeader() + (MASizeOfMemoryBlockHeader()+4) * 2 , 0 , -1 ) ;
	
	p_memfiles = MFLoadMemoryFiles( "memfiles" , p_memfilesparam ) ;
	if( p_memfiles == NULL )
	{
		printf( "MFLoadMemoryFiles failed\n" );
		return -1;
	}
	else
	{
		printf( "MFLoadMemoryFiles ok\n\n" );
	}
	
	TravelMemoryBlockByQueue( p_memfiles );
	
	msg_type = 111 ;
	strcpy( msg_buf , "AAA" );
	msg_len = strlen(msg_buf) + 1 ;
	lret = MFPushMemoryQueueMessage( p_memfiles , msg_type , msg_buf , msg_len ) ;
	if( lret )
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] failed[%ld]\n" , msg_type , msg_buf , lret );
		return -1;
	}
	else
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] ok\n\n" , msg_type , msg_buf );
	}
	
	msg_type = 222 ;
	strcpy( msg_buf , "BBB" );
	msg_len = strlen(msg_buf) + 1 ;
	lret = MFPushMemoryQueueMessage( p_memfiles , msg_type , msg_buf , msg_len ) ;
	if( lret )
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] failed[%ld]\n" , msg_type , msg_buf , lret );
		return -1;
	}
	else
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] ok\n\n" , msg_type , msg_buf );
	}
	
	msg_type = 333 ;
	strcpy( msg_buf , "CCC" );
	msg_len = strlen(msg_buf) + 1 ;
	lret = MFPushMemoryQueueMessage( p_memfiles , msg_type , msg_buf , msg_len ) ;
	if( lret )
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] failed[%ld]\n" , msg_type , msg_buf , lret );
		return -1;
	}
	else
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] ok\n\n" , msg_type , msg_buf );
	}
	
	msg_type = 444 ;
	strcpy( msg_buf , "DDD" );
	msg_len = strlen(msg_buf) + 1 ;
	lret = MFPushMemoryQueueMessage( p_memfiles , msg_type , msg_buf , msg_len ) ;
	if( lret )
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] failed[%ld]\n" , msg_type , msg_buf , lret );
		return -1;
	}
	else
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] ok\n\n" , msg_type , msg_buf );
	}
	
	TravelMemoryBlockByQueue( p_memfiles );
	
	msg_type = 222 ;
	memset( msg_buf , 0x00 , sizeof(msg_buf) );
	lret = MFPopupMemoryQueueMessage( p_memfiles , & msg_type , msg_buf , sizeof(msg_buf)-1 ) ;
	if( lret )
	{
		printf( "MFPopupMemoryQueueMessage[%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MFPopupMemoryQueueMessage[%d] ok , msg_buf[%s]\n\n" , msg_type , msg_buf );
	}
	
	TravelMemoryBlockByQueue( p_memfiles );
	
	msg_type = 111 ;
	memset( msg_buf , 0x00 , sizeof(msg_buf) );
	lret = MFPopupMemoryQueueMessage( p_memfiles , & msg_type , msg_buf , sizeof(msg_buf)-1 ) ;
	if( lret )
	{
		printf( "MFPopupMemoryQueueMessage[%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MFPopupMemoryQueueMessage[%d] ok , msg_buf[%s]\n\n" , msg_type , msg_buf );
	}
	
	TravelMemoryBlockByQueue( p_memfiles );
	
	msg_type = 444 ;
	memset( msg_buf , 0x00 , sizeof(msg_buf) );
	lret = MFPopupMemoryQueueMessage( p_memfiles , & msg_type , msg_buf , sizeof(msg_buf)-1 ) ;
	if( lret )
	{
		printf( "MFPopupMemoryQueueMessage[%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MFPopupMemoryQueueMessage[%d] ok , msg_buf[%s]\n\n" , msg_type , msg_buf );
	}
	
	TravelMemoryBlockByQueue( p_memfiles );
	
	msg_type = 333 ;
	memset( msg_buf , 0x00 , sizeof(msg_buf) );
	lret = MFPopupMemoryQueueMessage( p_memfiles , & msg_type , msg_buf , sizeof(msg_buf)-1 ) ;
	if( lret )
	{
		printf( "MFPopupMemoryQueueMessage[%d] failed[%ld]\n" , msg_type , lret );
		return -1;
	}
	else
	{
		printf( "MFPopupMemoryQueueMessage[%d] ok , msg_buf[%s]\n\n" , msg_type , msg_buf );
	}
	
	TravelMemoryBlockByQueue( p_memfiles );
	
	msg_type = 999 ;
	strcpy( msg_buf , "XXX" );
	msg_len = (MASizeOfMemoryBlockHeader()+4) * 3 ;
	lret = MFPushMemoryQueueMessage( p_memfiles , msg_type , msg_buf , msg_len ) ;
	if( lret )
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] failed[%ld]\n" , msg_type , msg_buf , lret );
		return -1;
	}
	else
	{
		printf( "MFPushMemoryQueueMessage[%d][%s] ok\n\n" , msg_type , msg_buf );
	}
	
	TravelMemoryBlockByQueue( p_memfiles );
	
	MFUnloadMemoryFiles( p_memfiles );
	printf( "MFUnloadMemoryFiles\n\n" );
	
	return 0;
}

int main()
{
	int		nret = 0 ;
	
	nret = test_MFPushMemoryQueueMessage() ;
	if( nret )
	{
		printf( "Testing failure\n" );
		return 1;
	}
	
	printf( "All is ok\n" );
	
	return 0;
}
