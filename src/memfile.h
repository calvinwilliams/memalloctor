#ifndef _H_MEMFILE_
#define _H_MEMFILE_

#include "memalloctor.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>

struct MemoryFile ;

struct MemoryFile *MFCreateMemoryFile( char *pathfilename , unsigned long file_size );
struct MemoryFile *MFOpenMemoryFile( char *pathfilename );
void MFCloseMemoryFile( struct MemoryFile *p_memfile );

char *MFGetMemoryFilename( struct MemoryFile *p_memfile );
struct MemoryPage *MFGetMemoryPage( struct MemoryFile *p_memfile );

#endif

