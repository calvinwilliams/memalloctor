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

#define MEMFILES_ERROR_ALLOC		-4
#define MEMFILES_ERROR_PARAMETER	-7
#define MEMFILES_ERROR_NOT_ENOUGH_SPACE	-16

struct MemoryFilesParameter *MFAllocMemoryFilesParameter6( char *prefix_pathfilename , char *postfix_pathfilename , long max_file_count , long init_file_size , long increase_file_size , long max_file_size );

struct MemoryFiles ;

struct MemoryFiles *MFLoadMemoryFiles( char *pathdirname , struct MemoryFilesParameter *p_memfilesparam );
void MFUnloadMemoryFiles( struct MemoryFiles *p_memfiles );

unsigned long MFGetMemoryFilesCount( struct MemoryFiles *p_memfiles );
unsigned long MFGetMemoryFileIndex( struct MemoryFiles *p_memfiles , struct MemoryPage *p_mempage );
struct MemoryFile *MFGetMemoryFile( struct MemoryFiles *p_memfiles , unsigned long fileindex );

#endif

