#include "memlist.h"

static void TravelMemoryBlockByList( struct MemoryPage *p_mempage )
{
	void		*p = NULL ;
	
	printf( "TravelMemoryBlockByList\n" );
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
			p = MATravelNextMemoryBlock( p_mempage , p ) ;
			if( p == NULL )
				break;
			printf( "		MEMORYBLOCK - p offset[%ld] addr[0x%p] data[%ld][%s]\n" , LPTRDIFF(p,p_mempage) , p , MAGetMemoryBlockSize(p) , (char*)p );
		}
		
		printf( "	LIST(NEXT)\n" );
		while(1)
		{
			p = MATravelNextMemoryListNode( p_mempage , p ) ;
			if( p == NULL )
				break;
			printf( "		MEMORYBLOCK - p offset[%ld] addr[0x%p] data[%ld][%s]\n" , LPTRDIFF(p,p_mempage) , p , MAGetMemoryBlockSize(p) , (char*)p );
		}
		
		printf( "	LIST(PREV)\n" );
		while(1)
		{
			p = MATravelPrevMemoryListNode( p_mempage , p ) ;
			if( p == NULL )
				break;
			printf( "		MEMORYBLOCK - p offset[%ld] addr[0x%p] data[%ld][%s]\n" , LPTRDIFF(p,p_mempage) , p , MAGetMemoryBlockSize(p) , (char*)p );
		}
	}
	
	return;
}

static int test_MAAddMemoryBlockByList()
{
	char			buffer[ 1024 ] ;
	char			block_data[ 1024 ] ;
	struct MemoryPage	*p_mempage = NULL ;
	void			*p1 = NULL ;
	void			*p2 = NULL ;
	void			*p3 = NULL ;
	void			*p4 = NULL ;
	void			*p5 = NULL ;
	
	p_mempage = MAFormatMemoryPage( buffer , sizeof(buffer) ) ;
	printf( "MAInitMemoryPage ok\n" );
	
	TravelMemoryBlockByList( p_mempage );
	
	memset( block_data , 0x00 , sizeof(block_data) );
	
	strcpy( block_data , "p1" );
	p1 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p1 == NULL )
	{
		printf( "MAAddMemoryListNode p1 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p1[%ld][%p] ok\n" , LPTRDIFF(p1,p_mempage) , p1 );
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	strcpy( block_data , "p2" );
	p2 = MAInsertBeforeMemoryListNode( p_mempage , block_data , 30 , p1 ) ;
	if( p2 == NULL )
	{
		printf( "MAInsertBeforeMemoryListNode p2 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAInsertBeforeMemoryListNode p2[%ld][%p] ok\n" , LPTRDIFF(p2,p_mempage) , p2 );
	}
	
	strcpy( block_data , "p3" );
	p3 = MAInsertBeforeMemoryListNode( p_mempage , block_data , 40 , p1 ) ;
	if( p3 == NULL )
	{
		printf( "MAInsertBeforeMemoryListNode p3 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAInsertBeforeMemoryListNode p3[%ld][%p] ok\n" , LPTRDIFF(p3,p_mempage) , p3 );
	}
	
	strcpy( block_data , "p4" );
	p4 = MAInsertAfterMemoryListNode( p_mempage , block_data , 50 , p1 ) ;
	if( p4 == NULL )
	{
		printf( "MAInsertBeforeMemoryListNode p4 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAInsertBeforeMemoryListNode p4[%ld][%p] ok\n" , LPTRDIFF(p4,p_mempage) , p4 );
	}
	
	strcpy( block_data , "p5" );
	p5 = MAInsertAfterMemoryListNode( p_mempage , block_data , 60 , p1 ) ;
	if( p5 == NULL )
	{
		printf( "MAInsertBeforeMemoryListNode p5 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAInsertBeforeMemoryListNode p5[%ld][%p] ok\n" , LPTRDIFF(p5,p_mempage) , p5 );
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	MARemoveMemoryListNode( p_mempage , p3 ) ;
	printf( "MARemoveMemoryListNode p3\n" );
	
	MARemoveMemoryListNode( p_mempage , p2 ) ;
	printf( "MARemoveMemoryListNode p2\n" );
	
	MARemoveMemoryListNode( p_mempage , p5 ) ;
	printf( "MARemoveMemoryListNode p5\n" );
	
	MARemoveMemoryListNode( p_mempage , p4 ) ;
	printf( "MARemoveMemoryListNode p4\n" );
	
	TravelMemoryBlockByList( p_mempage );
	
	MARemoveMemoryListNode( p_mempage , p1 ) ;
	printf( "MARemoveMemoryListNode p1\n" );
	
	TravelMemoryBlockByList( p_mempage );
	
	return 0;
}

static int maginaltest_MAAddMemoryBlockByList()
{
	char			buffer[ 1024 ] ;
	char			block_data[ 1024 ] ;
	struct MemoryPage	*p_mempage = NULL ;
	void			*p1 = NULL ;
	void			*p2 = NULL ;
	void			*p3 = NULL ;
	void			*p4 = NULL ;
	void			*p5 = NULL ;
	
	p_mempage = MAFormatMemoryPage( buffer , MASizeOfMemoryPageHeader() + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 + MASizeOfMemoryBlockHeader()+10 ) ;
	printf( "MAInitMemoryPage ok\n" );
	
	TravelMemoryBlockByList( p_mempage );
	
	memset( block_data , 0x00 , sizeof(block_data) );
	
	strcpy( block_data , "p1" );
	p1 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p1 == NULL )
	{
		printf( "MAAddMemoryListNode p1 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p1[%ld][%p] ok\n" , LPTRDIFF(p1,p_mempage) , p1 );
	}
	
	strcpy( block_data , "p2" );
	p2 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p2 == NULL )
	{
		printf( "MAAddMemoryListNode p2 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p2[%ld][%p] ok\n" , LPTRDIFF(p2,p_mempage) , p2 );
	}
	
	strcpy( block_data , "p3" );
	p3 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p3 == NULL )
	{
		printf( "MAAddMemoryListNode p3 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p3[%ld][%p] ok\n" , LPTRDIFF(p3,p_mempage) , p3 );
	}
	
	strcpy( block_data , "p4" );
	p4 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p4 == NULL )
	{
		printf( "MAAddMemoryListNode p4 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p4[%ld][%p] ok\n" , LPTRDIFF(p4,p_mempage) , p4 );
	}
	
	strcpy( block_data , "p5" );
	p5 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p5 == NULL )
	{
		printf( "MAAddMemoryListNode p5 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p5[%ld][%p] ok\n" , LPTRDIFF(p5,p_mempage) , p5 );
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	MARemoveMemoryListNode( p_mempage , p1 ) ;
	printf( "MARemoveMemoryListNode p1\n" );
	
	MARemoveMemoryListNode( p_mempage , p3 ) ;
	printf( "MARemoveMemoryListNode p3\n" );
	
	MARemoveMemoryListNode( p_mempage , p5 ) ;
	printf( "MARemoveMemoryListNode p5\n" );
	
	TravelMemoryBlockByList( p_mempage );
	
	strcpy( block_data , "p1" );
	p1 = MAAddMemoryListNode( p_mempage , block_data , 11 ) ;
	if( p1 == NULL )
	{
		printf( "MAAddMemoryListNode p1 failed\n" );
	}
	else
	{
		printf( "MAAddMemoryListNode p1[%ld][%p] ok\n" , LPTRDIFF(p1,p_mempage) , p1 );
		return -1;
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	strcpy( block_data , "p1" );
	p1 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p1 == NULL )
	{
		printf( "MAAddMemoryListNode p1 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p1[%ld][%p] ok\n" , LPTRDIFF(p1,p_mempage) , p1 );
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	strcpy( block_data , "p3" );
	p3 = MAAddMemoryListNode( p_mempage , block_data , 11 ) ;
	if( p3 == NULL )
	{
		printf( "MAAddMemoryListNode p3 failed\n" );
	}
	else
	{
		printf( "MAAddMemoryListNode p3[%ld][%p] ok\n" , LPTRDIFF(p3,p_mempage) , p3 );
		return -1;
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	strcpy( block_data , "p3" );
	p3 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p3 == NULL )
	{
		printf( "MAAddMemoryListNode p3 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p3[%ld][%p] ok\n" , LPTRDIFF(p3,p_mempage) , p3 );
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	strcpy( block_data , "p5" );
	p5 = MAAddMemoryListNode( p_mempage , block_data , 11 ) ;
	if( p5 == NULL )
	{
		printf( "MAAddMemoryListNode p5 failed\n" );
	}
	else
	{
		printf( "MAAddMemoryListNode p5[%ld][%p] ok\n" , LPTRDIFF(p5,p_mempage) , p5 );
		return -1;
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	strcpy( block_data , "p5" );
	p5 = MAAddMemoryListNode( p_mempage , block_data , 10 ) ;
	if( p5 == NULL )
	{
		printf( "MAAddMemoryListNode p5 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryListNode p5[%ld][%p] ok\n" , LPTRDIFF(p5,p_mempage) , p5 );
	}
	
	TravelMemoryBlockByList( p_mempage );
	
	MACleanMemoryPage( p_mempage );
	printf( "MACleanMemoryPage\n" );
	
	TravelMemoryBlockByList( p_mempage );
	
	return 0;
}

int main()
{
	int		nret = 0 ;
	
	nret = test_MAAddMemoryBlockByList() ;
	if( nret )
	{
		printf( "Testing failure\n" );
	}
	else
	{
		printf( "All is ok\n" );
	}
	
	nret = maginaltest_MAAddMemoryBlockByList() ;
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
