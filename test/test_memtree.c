#include "memtree.h"

void MADebugMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage );

static void TravelMemoryBlockByTrieTreeLeaf( struct MemoryPage *p_mempage )
{
	void		*p = NULL ;
	
	printf( "TravelMemoryBlockByTrieTreeLeaf\n" );
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
			printf( "		MEMORYBLOCK - p offset[%ld] addr[0x%p] data[%ld][%s]\n" , LPTRDIFF(p,p_mempage) , p , MAGetMemoryBlockSize(p) , MAGetMemoryBlockSize(p)?(char*)p:"" );
		}
	}
	
	MADebugMemoryQuarterTrieTreeLeaf( p_mempage );
	
	return;
}

int test_MAAddMemoryQuarterTrieTreeLeaf()
{
	char			buffer[ 1*1024*1024 ] ;
	struct MemoryPage	*p_mempage = NULL ;
	
	char			key[ 16 + 1 ] ;
	char			value[ 16 + 1 ] ;
	char			*p_value = NULL ;
	unsigned long		value_size ;
	
	int			nret = 0 ;
	
	p_mempage = MAFormatMemoryPage( buffer , sizeof(buffer) ) ;
	printf( "MAInitMemoryPage ok\n" );
	
	TravelMemoryBlockByTrieTreeLeaf( p_mempage );
	
	strcpy( key , "AAA" );
	strcpy( value , "111" );
	nret = MAAddMemoryQuarterTrieTreeLeaf( p_mempage , key , value , strlen(value)+1 ) ;
	if( nret )
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] failed[%d]\n" , key , value , nret );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] ok\n" , key , value );
	}
	
	TravelMemoryBlockByTrieTreeLeaf( p_mempage );
	
	strcpy( key , "BBB" );
	strcpy( value , "222" );
	nret = MAAddMemoryQuarterTrieTreeLeaf( p_mempage , key , value , strlen(value)+1 ) ;
	if( nret )
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] failed[%d]\n" , key , value , nret );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] ok\n" , key , value );
	}
	
	strcpy( key , "CCC" );
	strcpy( value , "333" );
	nret = MAAddMemoryQuarterTrieTreeLeaf( p_mempage , key , value , strlen(value)+1 ) ;
	if( nret )
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] failed[%d]\n" , key , value , nret );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] ok\n" , key , value );
	}
	
	strcpy( key , "BB" );
	strcpy( value , "22" );
	nret = MAAddMemoryQuarterTrieTreeLeaf( p_mempage , key , value , strlen(value)+1 ) ;
	if( nret )
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] failed[%d]\n" , key , value , nret );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] ok\n" , key , value );
	}
	
	strcpy( key , "BBBB" );
	strcpy( value , "2222" );
	nret = MAAddMemoryQuarterTrieTreeLeaf( p_mempage , key , value , strlen(value)+1 ) ;
	if( nret )
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] failed[%d]\n" , key , value , nret );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] ok\n" , key , value );
	}
	
	strcpy( key , "DDD" );
	strcpy( value , "444" );
	nret = MAAddMemoryQuarterTrieTreeLeaf( p_mempage , key , value , strlen(value)+1 ) ;
	if( nret )
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] failed[%d]\n" , key , value , nret );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] ok\n" , key , value );
	}
	
	strcpy( key , "DDD9" );
	strcpy( value , "4449" );
	nret = MAAddMemoryQuarterTrieTreeLeaf( p_mempage , key , value , strlen(value)+1 ) ;
	if( nret )
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] failed[%d]\n" , key , value , nret );
		return -1;
	}
	else
	{
		printf( "MAAddMemoryQuarterTrieTreeLeaf [%s][%s] ok\n" , key , value );
	}
	
	TravelMemoryBlockByTrieTreeLeaf( p_mempage );
	
	strcpy( key , "DDD" );
	nret = MAQueryMemoryQuarterTrieTreeLeaf( p_mempage , key , (void **) & p_value , & value_size ) ;
	if( nret )
	{
		printf( "MAQueryMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MAQueryMemoryQuarterTrieTreeLeaf [%s][%s][%ld] ok\n" , key , p_value , value_size );
	}
	
	strcpy( key , "DDD9" );
	nret = MAQueryMemoryQuarterTrieTreeLeaf( p_mempage , key , (void **) & p_value , & value_size ) ;
	if( nret )
	{
		printf( "MAQueryMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MAQueryMemoryQuarterTrieTreeLeaf [%s][%s][%ld] ok\n" , key , p_value , value_size );
	}
	
	TravelMemoryBlockByTrieTreeLeaf( p_mempage );
	
	strcpy( key , "AAA" );
	nret = MARemoveMemoryQuarterTrieTreeLeaf( p_mempage , key ) ;
	if( nret )
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] ok\n" , key );
	}
	
	strcpy( key , "DDD" );
	nret = MARemoveMemoryQuarterTrieTreeLeaf( p_mempage , key ) ;
	if( nret )
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] ok\n" , key );
	}
	
	strcpy( key , "DDD9" );
	nret = MARemoveMemoryQuarterTrieTreeLeaf( p_mempage , key ) ;
	if( nret )
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] ok\n" , key );
	}
	
	TravelMemoryBlockByTrieTreeLeaf( p_mempage );
	
	strcpy( key , "CCC" );
	nret = MARemoveMemoryQuarterTrieTreeLeaf( p_mempage , key ) ;
	if( nret )
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] ok\n" , key );
	}
	
	strcpy( key , "BBBB" );
	nret = MARemoveMemoryQuarterTrieTreeLeaf( p_mempage , key ) ;
	if( nret )
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] ok\n" , key );
	}
	
	strcpy( key , "BBB" );
	nret = MARemoveMemoryQuarterTrieTreeLeaf( p_mempage , key ) ;
	if( nret )
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] ok\n" , key );
	}
	
	strcpy( key , "BB" );
	nret = MARemoveMemoryQuarterTrieTreeLeaf( p_mempage , key ) ;
	if( nret )
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] failed[%d]\n" , key , nret );
		return -1;
	}
	else
	{
		printf( "MARemoveMemoryQuarterTrieTreeLeaf [%s] ok\n" , key );
	}
	
	TravelMemoryBlockByTrieTreeLeaf( p_mempage );
	
	return 0;
}

int main()
{
	int		nret = 0 ;
	
	nret = test_MAAddMemoryQuarterTrieTreeLeaf() ;
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
