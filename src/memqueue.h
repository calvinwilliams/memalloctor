#ifndef _H_MEMQUEUE_
#define _H_MEMQUEUE_

#include "memalloctor.h"

/* queue */

unsigned long MAPushMemoryQueueMessage( struct MemoryPage *p_mempage , int msg_type , char *msg_data , unsigned long msg_size );
unsigned long MAPopupMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *msg_buf , unsigned long msg_bufsize );
unsigned long MAPopdmpMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char **pp_msg_data );

char *MATravelMemoryQueueMessage( struct MemoryPage *p_mempage , int *p_msg_type , char *p_msg_data );

#endif
