#ifndef _H_MEMTYPE_
#define _H_MEMTYPE_

#define P2MEMBLOCK(_addr_)			((struct MemoryBlock*)((char*)(_addr_)-sizeof(struct MemoryBlock)))
#define MEMBLOCK2P(_memblock_)			((char*)(_memblock_)+sizeof(struct MemoryBlock))
#define MEMBLOCK2MEMPAGE(_memblock_)		(struct MemoryPage*)((char*)(_memblock_)-(_memblock_)->addr_this_offset)

#define CHECK_MEMPAGE_MAGIC(_p_mempage_)	memcmp((char*)(_p_mempage_),"MP",2)
#define CHECK_MEMBLOCK_MAGIC(_p_memblock_)	memcmp((char*)(_p_memblock_),"MB",2)

#define ULSIZEOF(_type_)			((unsigned long)(sizeof(_type_)))

struct MemoryBlock
{
	char		magic[ 2 ];	/* MB */
	
	long		fileindex ;
	unsigned long	addr_this_offset ;
	unsigned long	addr_prev_offset ;
	unsigned long	addr_next_offset ;
	
	union
	{
		struct
		{
			unsigned long	dll_prev_offset ;
			unsigned long	dll_next_offset ;
		} dll ;
		struct
		{
			int		msg_type ;
			long		queue_prev_fileindex ;
			unsigned long	queue_prev_offset ;
			long		queue_next_fileindex ;
			unsigned long	queue_next_offset ;
		} queue ;
		struct
		{
			unsigned long	children_offset[ 4 ] ;
		} quarter_trie_tree ;
	} ds ;
	
	long		block_size ;
} ;

struct MemoryPage
{
	char		magic[ 2 ];	/* MP */
	
	unsigned long	totalsize ;
	                	
	unsigned long	block_count ;
	unsigned long	block_used_totalsize ;
	
	unsigned long	addr_first_offset ;
	unsigned long	addr_last_offset ;
	union
	{
		struct
		{
			unsigned long	dll_first_offset ;
			unsigned long	dll_last_offset ;
		} dll ;
		struct
		{
			long		queue_first_fileindex ;
			unsigned long	queue_first_offset ;
			long		queue_last_fileindex ;
			unsigned long	queue_last_offset ;
		} queue ;
		struct
		{
			unsigned long	root_offset ;
		} quarter_trie_tree ;
	} ds ;
} ;

#define MEMFILE_MAXLEN_FILENAME		256

struct MemoryFile
{
	char			pathfilename[ MEMFILE_MAXLEN_FILENAME + 1 ] ;
	struct MemoryPage	*p_mempage ;
} ;

struct MemoryFilesParameter
{
	char			prefix_pathfilename[ MEMFILE_MAXLEN_FILENAME + 1 ] ;
	char			postfix_pathfilename[ MEMFILE_MAXLEN_FILENAME + 1 ] ;
	long			max_file_count ;
	long			init_file_size ;
	long			increase_file_size ;
	long			max_file_size ;
} ;

struct MemoryFiles
{
	struct MemoryFilesParameter	memfilesparam ;
	
	char			pathdirname[ MEMFILE_MAXLEN_FILENAME + 1 ] ;
	struct MemoryFile	**memfiles_array ;
	long			memfiles_array_count ;
	long			memfile_current_index ;
	
	int			travel_fileindex ;
} ;

int MFReallocMemoryFileArray( struct MemoryFiles *p_memfiles );

#endif
