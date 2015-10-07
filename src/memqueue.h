#ifndef _H_MEMQUEUE_
#define _H_MEMQUEUE_

#include "memalloctor.h"
#include "memfile.h"
#include "memfiles.h"

/* queue */
#define MEMQUEUE_ERROR_INTERNAL			-2
#define MEMQUEUE_ERROR_ALLOC			-4
#define MEMQUEUE_ERROR_FILE_NOT_FOUND		-6
#define MEMQUEUE_ERROR_PARAMETER		-7
#define MEMQUEUE_ERROR_NOT_ENOUGH_SPACE		-16
#define MEMQUEUE_ERROR_NOT_ENOUGH_BUFFER	-18
#define MEMQUEUE_ERROR_MESSAGE_NOT_FOUND	-21

long MAPushMemoryQueueMessage( struct MemoryPage *p_mempage , int msg_type , char *msg_data , long msg_size );
long MAPopupMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *msg_buf , long msg_bufsize );
long MAPopdmpMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char **pp_msg_data , long *p_msg_size );

char *MATravelMemoryQueueMessages( struct MemoryPage *p_mempage , int *p_msg_type , char *p_msg_data );

/* memory files queue */

struct MemoryFiles *p_memfiles ;

long MFPushMemoryQueueMessage( struct MemoryFiles *p_memfiles , int msg_type , char *msg_data , long msg_size );
long MFPopupMemoryQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char *msg_buf , long msg_bufsize );
long MAPopdmpMemoryFilesQueueMessage( struct MemoryFiles *p_memfiles , int *p_msg_type , char **pp_msg_data , long *p_msg_size );

char *MFTravelMemoryQueueMessages( struct MemoryFiles *p_memfiles , int *p_msg_type , char *p_msg_data );
char *MFTravelMemoryQueueMessagesReverse( struct MemoryFiles *p_memfiles , int *p_msg_type , char *p_msg_data );

#endif
