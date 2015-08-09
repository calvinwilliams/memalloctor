#ifndef _H_MEMFILES_
#define _H_MEMFILES_

#include "memfile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>

#define MEMFILES_MAX_FILE_COUNT		1024
#define MEMFILES_INIT_FILE_SIZE		1024
#define MEMFILES_INCREASE_FILE_SIZE	-2
#define MEMFILES_MAX_FILE_SIZE		1024*1024*1024

struct MemoryFilesParameter *MFAllocMemoryFilesParameter5( char *prefix_pathfilename , long max_file_count , long init_file_size , long increase_file_size , long max_file_size );

struct MemoryFiles ;

struct MemoryFiles *MFLoadMemoryFiles( char *pathdirname , struct MemoryFilesParameter *p_memfileparam );
void MFUnloadMemoryFiles( struct MemoryFiles *p_memfiles );

#if 0

int MFAddMemoryBlock( struct MemoryFiles *p_memfiles , char *block_base , unsigned long block_size , struct MemoryPage **pp_mempage , struct MemoryBlock **pp_memblock );
int MFRemoveMemoryBlock( struct MemoryFiles *p_memfiles , struct MemoryPage *p_mempage , struct MemoryBlock *p_memblock );
int MFTravelMemoryFile( struct MemoryFiles *p_memfiles , struct MemoryFile **pp_memfile );
int MFTravelMemoryBlockByAddr( struct MemoryFiles *p_memfiles , struct MemoryFile **pp_memfile , struct MemoryBlock **pp_memblock );

#endif

#if 0

/* queue */

unsigned long MAPushMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int msg_type , char *msg_data , unsigned long msg_size );
unsigned long MAPopupMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char *msg_buf , unsigned long msg_bufsize );
unsigned long MAPopdmpMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char **pp_msg_data );

char *MATravelMemoryFilesQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *p_msg_data );

#endif

#endif

