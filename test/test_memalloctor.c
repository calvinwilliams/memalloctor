#include "memalloctor.h"

static void TravelMemoryBlock( struct MemoryPage *p_mempage )
{
	void		*p = NULL ;
	
	printf( "TravelMemoryBlock\n" );
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
			printf( "		MEMORYBLOCK - p offset[%ld] addr[0x%p] data[%ld][%s]\n" , (unsigned long)((char*)p-(char*)p_mempage) , p , MAGetMemoryBlockSize(p) , (char*)p );
		}
	}
	
	return;
}

static int test_MAAllocMemoryBlock()
{
	char			buffer[ 1024 ] ;
	struct MemoryPage	*p_mempage = NULL ;
	void			*p1 = NULL ;
	void			*p2 = NULL ;
	void			*p3 = NULL ;
	void			*p4 = NULL ;
	void			*p5 = NULL ;
	
	p_mempage = MAFormatMemoryPage( buffer , sizeof(buffer) ) ;
	printf( "MAInitMemoryPage ok\n" );
	
	TravelMemoryBlock( p_mempage );
	
	p1 = MAAllocMemoryBlock( p_mempage , 10 ) ;
	if( p1 == NULL )
	{
		printf( "MAAllocMemoryBlock p1 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAllocMemoryBlock p1[%ld][%p] ok\n" , LPTRDIFF(p1,p_mempage) , p1 );
		strcpy( p1 , "p1" );
	}
	
	p2 = MAAllocMemoryBlock( p_mempage , 20 ) ;
	if( p2 == NULL )
	{
		printf( "MAAllocMemoryBlock p2 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAllocMemoryBlock p2[%ld][%p] ok\n" , LPTRDIFF(p2,p_mempage) , p2 );
		strcpy( p2 , "p2" );
	}
	
	p3 = MAAllocMemoryBlock( p_mempage , 30 ) ;
	if( p3 == NULL )
	{
		printf( "MAAllocMemoryBlock p3 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAllocMemoryBlock p3[%ld][%p] ok\n" , LPTRDIFF(p3,p_mempage) , p3 );
		strcpy( p3 , "p3" );
	}
	
	p4 = MAAllocMemoryBlock( p_mempage , 40 ) ;
	if( p4 == NULL )
	{
		printf( "MAAllocMemoryBlock p4 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAllocMemoryBlock p4[%ld][%p] ok\n" , LPTRDIFF(p4,p_mempage) , p4 );
		strcpy( p4 , "p4" );
	}
	
	TravelMemoryBlock( p_mempage );
	
	MAFreeMemoryBlock( p_mempage , p2 );
	printf( "MAFreeMemoryBlock p2\n" );
	
	MAFreeMemoryBlock( p_mempage , p1 );
	printf( "MAFreeMemoryBlock p1\n" );
	
	MAFreeMemoryBlock( p_mempage , p4 );
	printf( "MAFreeMemoryBlock p4\n" );
	
	TravelMemoryBlock( p_mempage );
	
	p5 = MAAllocMemoryBlock( p_mempage , 50 ) ;
	if( p5 == NULL )
	{
		printf( "MAAllocMemoryBlock p5 failed\n" );
		return -1;
	}
	else
	{
		printf( "MAAllocMemoryBlock p5[%ld][%p] ok\n" , LPTRDIFF(p5,p_mempage) , p5 );
		strcpy( p5 , "p5" );
	}
	
	TravelMemoryBlock( p_mempage );
	
	MAFreeMemoryBlock( p_mempage , p5 );
	printf( "MAFreeMemoryBlock p5\n" );
	
	TravelMemoryBlock( p_mempage );
	
	MAFreeMemoryBlock( p_mempage , p3 );
	printf( "MAFreeMemoryBlock p3\n" );
	
	TravelMemoryBlock( p_mempage );
	
	return 0;
}

int main()
{
	int		nret = 0 ;
	
	nret = test_MAAllocMemoryBlock() ;
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

