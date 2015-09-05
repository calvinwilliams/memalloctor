#include "memtype.h"
#include "memtree.h"

static int _AllocMemoryQuarterTrieTreePath( struct MemoryPage *p_mempage , char *key , void *value , unsigned long value_size , struct MemoryBlock *p_memblock_parent_branch , int bitpos , int replace_flag )
{
	unsigned char		uindex ;
	struct MemoryBlock	*p_memblock_branch_add = NULL ;
	void			*p_branch_add = NULL ;
	
	struct MemoryBlock	*p_memblock_leaf_add = NULL ;
	void			*p_leaf_add = NULL ;
	
	int			nret = 0 ;
	
	uindex = ((unsigned char)(*key)>>(6-bitpos))&0x03 ;
	
	if( *key )
	{
		if( p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] == 0 )
		{
			p_branch_add = MAAllocMemoryBlock( p_mempage , 0 );
			if( p_branch_add == NULL )
				return MEMTREE_ERROR_NOT_ENOUGH_SPACE;
			p_memblock_branch_add = P2MEMBLOCK(p_branch_add) ;
			p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] = p_memblock_branch_add->addr_this_offset ;
		}
		else
		{
			p_memblock_branch_add = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] ) ;
		}
		
		if( bitpos <= 4 )
			nret = _AllocMemoryQuarterTrieTreePath( p_mempage , key , value , value_size , p_memblock_branch_add , bitpos+2 , replace_flag ) ;
		else
			nret = _AllocMemoryQuarterTrieTreePath( p_mempage , key+1 , value , value_size , p_memblock_branch_add , 0 , replace_flag ) ;
		if( nret )
			return nret;
	}
	else
	{
		if( p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] && replace_flag == 0 )
			return MEMTREE_ERROR_KEY_EXIST;
		
		p_leaf_add = MAAllocMemoryBlock( p_mempage , value_size );
		if( p_leaf_add == NULL )
			return MEMTREE_ERROR_NOT_ENOUGH_SPACE;
		p_memblock_leaf_add = P2MEMBLOCK(p_leaf_add) ;
		p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] = p_memblock_leaf_add->addr_this_offset ;
		
		if( value && value_size > 0 )
		{
			memcpy( p_leaf_add , value , value_size );
		}
	}
	
	return 0;
}

static int _AllocMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key , void *value , unsigned long value_size , int replace_flag )
{
	struct MemoryBlock	*p_memblock_root_add = NULL ;
	void			*p_root_add = NULL ;
	
	int			nret = 0 ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) || key == NULL || value_size <= 0 )
	{
		errno = EINVAL ;
		return MEMTREE_ERROR_PARAMETER;
	}
	
	if( p_mempage->ds.quarter_trie_tree.root_offset == 0 )
	{
		p_root_add = MAAllocMemoryBlock( p_mempage , 0 );
		if( p_root_add == NULL )
			return MEMTREE_ERROR_NOT_ENOUGH_SPACE;
		p_memblock_root_add = P2MEMBLOCK(p_root_add) ;
		
		p_mempage->ds.quarter_trie_tree.root_offset = p_memblock_root_add->addr_this_offset ;
	}
	else
	{
		p_memblock_root_add = (struct MemoryBlock *)( (char*)p_mempage + p_mempage->ds.quarter_trie_tree.root_offset ) ;
	}
	
	nret = _AllocMemoryQuarterTrieTreePath( p_mempage , key , value , value_size , p_memblock_root_add , 0 , 0 ) ;
	if( nret )
		return nret;
	
	return 0;
}

int MAAddMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key , void *value , unsigned long value_size )
{
	return _AllocMemoryQuarterTrieTreeLeaf( p_mempage , key , value , value_size , 0 );
}

int MASetMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key , void *value , unsigned long value_size )
{
	return _AllocMemoryQuarterTrieTreeLeaf( p_mempage , key , value , value_size , 1 );
}

static int _FreeMemoryQuarterTrieTreePath( struct MemoryPage *p_mempage , char *key , struct MemoryBlock *p_memblock_parent_branch , int bitpos )
{
	unsigned char		uindex ;
	struct MemoryBlock	*p_memblock_remove = NULL ;
	
	int			nret = 0 ;
	
	uindex = ((unsigned char)(*key)>>(6-bitpos))&0x03 ;
	if( p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] == 0 )
	{
		return MEMTREE_ERROR_KEY_NOT_EXIST;
	}
	
	p_memblock_remove = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] ) ;
	
	if( *key )
	{
		if( bitpos <= 4 )
			nret = _FreeMemoryQuarterTrieTreePath( p_mempage , key , p_memblock_remove , bitpos+2 ) ;
		else
			nret = _FreeMemoryQuarterTrieTreePath( p_mempage , key+1 , p_memblock_remove , 0 ) ;
		if( nret )
			return nret;
		
		if(	p_memblock_remove->ds.quarter_trie_tree.children_offset[0] == 0
			&& p_memblock_remove->ds.quarter_trie_tree.children_offset[1] == 0
			&& p_memblock_remove->ds.quarter_trie_tree.children_offset[2] == 0
			&& p_memblock_remove->ds.quarter_trie_tree.children_offset[3] == 0
		)
		{
			MAFreeMemoryBlock( p_mempage , MEMBLOCK2P(p_memblock_remove) );
			p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] = 0 ;
		}
	}
	else
	{
		if(	p_memblock_remove->ds.quarter_trie_tree.children_offset[0] == 0
			&& p_memblock_remove->ds.quarter_trie_tree.children_offset[1] == 0
			&& p_memblock_remove->ds.quarter_trie_tree.children_offset[2] == 0
			&& p_memblock_remove->ds.quarter_trie_tree.children_offset[3] == 0
		)
		{
			MAFreeMemoryBlock( p_mempage , MEMBLOCK2P(p_memblock_remove) );
			p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] = 0 ;
		}
		else
		{
			p_memblock_remove->block_size = 0 ;
		}
	}
	
	return 0;
}

int MARemoveMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key )
{
	struct MemoryBlock	*p_memblock_root_remove = NULL ;
	
	int			nret = 0 ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) || key == NULL )
	{
		errno = EINVAL ;
		return MEMTREE_ERROR_PARAMETER;
	}
	
	if( p_mempage->ds.quarter_trie_tree.root_offset == 0 )
	{
		return MEMTREE_ERROR_KEY_NOT_EXIST;
	}
	else
	{
		p_memblock_root_remove = (struct MemoryBlock *)( (char*)p_mempage + p_mempage->ds.quarter_trie_tree.root_offset ) ;
	}
	
	nret = _FreeMemoryQuarterTrieTreePath( p_mempage , key , p_memblock_root_remove , 0 ) ;
	if( nret )
		return nret;
	
	if(	p_memblock_root_remove->ds.quarter_trie_tree.children_offset[0] == 0
		&& p_memblock_root_remove->ds.quarter_trie_tree.children_offset[1] == 0
		&& p_memblock_root_remove->ds.quarter_trie_tree.children_offset[2] == 0
		&& p_memblock_root_remove->ds.quarter_trie_tree.children_offset[3] == 0
	)
	{
		MAFreeMemoryBlock( p_mempage , MEMBLOCK2P(p_memblock_root_remove) );
		p_mempage->ds.quarter_trie_tree.root_offset = 0 ;
	}
	
	return 0;
}

static int _AccessMemoryQuarterTrieTreePath( struct MemoryPage *p_mempage , char *key , struct MemoryBlock *p_memblock_parent_branch , int bitpos , void **pp_value , unsigned long *p_value_size )
{
	unsigned char		uindex ;
	struct MemoryBlock	*p_memblock_access = NULL ;
	
	int			nret = 0 ;
	
	uindex = ((unsigned char)(*key)>>(6-bitpos))&0x03 ;
	if( p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] == 0 )
	{
		return MEMTREE_ERROR_KEY_NOT_EXIST;
	}
	
	p_memblock_access = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_parent_branch->ds.quarter_trie_tree.children_offset[uindex] ) ;
	
	if( *key )
	{
		if( bitpos <= 4 )
			nret = _AccessMemoryQuarterTrieTreePath( p_mempage , key , p_memblock_access , bitpos+2 , pp_value , p_value_size ) ;
		else
			nret = _AccessMemoryQuarterTrieTreePath( p_mempage , key+1 , p_memblock_access , 0 , pp_value , p_value_size ) ;
		if( nret )
			return nret;
	}
	else
	{
		if( p_memblock_access->block_size > 0 )
		{
			if( pp_value )
				(*pp_value) = MEMBLOCK2P(p_memblock_access) ;
			if( p_value_size )
				(*p_value_size) = p_memblock_access->block_size ;
		}
		else
		{
			if( pp_value )
				(*pp_value) = NULL ;
			if( p_value_size )
				(*p_value_size) = 0 ;
		}
	}
	
	return 0;
}

int MAQueryMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage , char *key , void **pp_value , unsigned long *p_value_size )
{
	struct MemoryBlock	*p_memblock_root_add = NULL ;
	
	int			nret = 0 ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) || key == NULL )
	{
		errno = EINVAL ;
		return MEMTREE_ERROR_PARAMETER;
	}
	
	if( p_mempage->ds.quarter_trie_tree.root_offset == 0 )
	{
		return MEMTREE_ERROR_KEY_NOT_EXIST;
	}
	else
	{
		p_memblock_root_add = (struct MemoryBlock *)( (char*)p_mempage + p_mempage->ds.quarter_trie_tree.root_offset ) ;
	}
	
	nret = _AccessMemoryQuarterTrieTreePath( p_mempage , key , p_memblock_root_add , 0 , pp_value , p_value_size ) ;
	if( nret )
		return nret;
	
	return 0;
}

static void _DebugMemoryQuarterTrieTreePath( struct MemoryPage *p_mempage , struct MemoryBlock *p_memblock_parent , char *key_base , char *key_ptr , int bitpos , int depth1 , int depth2 )
{
	unsigned char		uindex ;
	char			key_bak ;
	struct MemoryBlock	*p_memblock_branch = NULL ;
	int			i ;
	
	key_bak = key_ptr[0] ;
	for( uindex = 0 ; uindex <= 3 ; uindex++ )
	{
		if( p_memblock_parent->ds.quarter_trie_tree.children_offset[uindex] > 0 )
		{
			key_ptr[0] = ((unsigned char)key_bak<<2) + uindex ;
			key_ptr[1] = '\0' ;
			
			p_memblock_branch = (struct MemoryBlock *)( (char*)p_mempage + p_memblock_parent->ds.quarter_trie_tree.children_offset[uindex] ) ;
			
			for( i = 0 ; i < depth2 ; i++ )
			{
				printf( ".       " );
			}
			printf( "." );
			for( i = 0 ; i < depth1 ; i++ )
			{
				printf( " " );
			}
			printf( "[%ld] - [%d][%ld]" , p_memblock_parent->addr_this_offset , uindex , p_memblock_parent->ds.quarter_trie_tree.children_offset[uindex] );
			if( p_memblock_branch->block_size > 0 )
				printf( " ------ [%s][%s]" , key_base , MEMBLOCK2P(p_memblock_branch) );
			printf( "\n" );
			
			if( bitpos <= 4 )
				_DebugMemoryQuarterTrieTreePath( p_mempage , p_memblock_branch , key_base , key_ptr , bitpos+2 , depth1+1 , depth2 );
			else
				_DebugMemoryQuarterTrieTreePath( p_mempage , p_memblock_branch , key_base , key_ptr+1 , 0 , 0 , depth2+1 );
		}
	}
	
	return;
}

void MADebugMemoryQuarterTrieTreeLeaf( struct MemoryPage *p_mempage )
{
	struct MemoryBlock	*p_memblock_root = NULL ;
	
	char			key[ 1024 + 1 ] ;
	
	if( p_mempage == NULL || CHECK_MEMPAGE_MAGIC(p_mempage) )
	{
		errno = EINVAL ;
		return;
	}
	
	if( p_mempage->ds.quarter_trie_tree.root_offset == 0 )
	{
		return;
	}
	else
	{
		p_memblock_root = (struct MemoryBlock *)( (char*)p_mempage + p_mempage->ds.quarter_trie_tree.root_offset ) ;
	}
	
	memset( key , 0x00 , sizeof(key) );
	_DebugMemoryQuarterTrieTreePath( p_mempage , p_memblock_root , key , key , 0 , 0 , 0 );
	
	return;
}
