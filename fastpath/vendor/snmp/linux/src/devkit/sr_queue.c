/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/*
 *  Function:  This file defines functions for implementing 
 *             general-purpose queues.
 */



#include "sr_conf.h"



#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <sys/time.h>

#include <sys/socket.h>

#include <string.h>


#include <malloc.h>

#include <netinet/in.h>

#include <errno.h>

#include <sys/ioctl.h>

#include <fcntl.h>


#include <stdarg.h>

#include "sr_proto.h"
#include "sr_type.h"
#include "sr_snmp.h"
#include "sr_trans.h"
#include "diag.h"
SR_FILENAME
#include "sr_queue.h"


/*
 *  SRQLock()
 *
 *  Function:  Locks and/or waits on a queue to become available.
 *
 *  Inputs:    queue - the queue to lock.
 *
 *  Outputs:   On success, returns 0.  On failure, returns -1.
 */
static int
SRQLock(SRQueue *queue)
{
    FNAME("SRQLock")

    /* --- Return if no queue was specified --- */
    if (queue == NULL) {
        return -1;
    }

    /* --- Return if the queue is not initialized --- */
    if (queue->initialized != SR_QUEUE_INITIALIZED) {
        return -1;
    }

    /* --- Use the semaphore if provided --- */
    if (queue->SWaitFcn != NULL) {
        if ((*(queue->SWaitFcn)) (queue->SData) != 0) {
            DPRINTF((APERROR, "%s:  semaphore wait function failed.\n", Fname));
            return -1;
        }
    }
    /* --- Return -1 if the queue is not unlocked --- */
    else {
        if (queue->lock != SR_QUEUE_UNLOCKED) {
           return -1;
        }
    }

    /* --- Mark the queue as locked and return --- */
    queue->lock = SR_QUEUE_LOCKED;
    return 0;
}


/*
 *  SRQUnlock()
 *
 *  Function:  Unlocks a queue and may post a semaphore.
 *
 *  Inputs:    queue - the queue to unlock.
 *
 *  Outputs:   On success, return 0, on failure, returns -1.
 *             (Success means the queue is unlocked.)
 */
static int
SRQUnlock(SRQueue *queue)
{
    FNAME("SRQUnlock")

    /* --- Return if no queue was specified --- */
    if (queue == NULL) {
        return -1;
    }

    /* --- Return if the queue is not initialized --- */
    if (queue->initialized != SR_QUEUE_INITIALIZED) {
        return -1;
    }

    queue->lock = SR_QUEUE_UNLOCKED;

    /* --- Return if no semaphore function provided --- */
    if (queue->SPostFcn == NULL) {
        return 0;
    }

    /* --- Post the semaphore --- */
    if ((*(queue->SPostFcn)) (queue->SData) != 0) {
        DPRINTF((APERROR, "%s:  semaphore post function failed.\n", Fname));
        return -1;
    }

    return 0;
}


/*
 *  SRQMalloc()
 *
 *  Function:  Provides a convenience function for allocating and
 *             initializing a queue.
 */
SRQueue *
SRQMalloc(
    int     (*SWaitFcn)(void *ptr), 
    int     (*SPostFcn)(void *ptr),
    void    *SData)
{
    SRQueue *queue = NULL;

    /* --- Allocate the queue --- */
    queue = (SRQueue *) malloc(sizeof(SRQueue));
    if (queue == NULL) {
        return NULL;
    }

    /* --- Initialize the queue --- */
    if (SRQInitialize(queue, SWaitFcn, SPostFcn, SData) == -1) {
        free(queue);
        return NULL;
    }

    return queue;
}


/*
 *  SRQFree()
 *
 *  Function:  Provides a convenience function for terminating and
 *             freeing a queue.
 */
int
SRQFree(SRQueue *queue, void (*PtrFreeFcn)(void *ptr))
{
    /* --- Return if no queue was specified --- */
    if (queue == NULL) {
        return 0;
    }

    /* --- Return if the queue is not initialized --- */
    if (queue->initialized != SR_QUEUE_INITIALIZED) {
        return 0;
    }

    /* --- Terminate the queue --- */
    if (SRQTerminate(queue, PtrFreeFcn) == -1) {
        return -1;
    }

    /* --- Free the queue --- */
    free(queue);

    return 0;
}


/*
 *  SRQInitialize()
 *
 *  Function:  Initializes a queue.
 */
int 
SRQInitialize(
    SRQueue *queue, 
    int     (*SWaitFcn)(void *ptr), 
    int     (*SPostFcn)(void *ptr),
    void    *SData)
{
    FNAME("SRQInitialize")

    /* --- Return if no queue was specified --- */
    if (queue == NULL) {
        return -1;
    }

    /*
     *  Lock the queue.  We cannot use the SRQLock() function because the
     *  queue (and function pointers) have not been initialized yet.
     */
    if (SWaitFcn != NULL) {
        if ((*(SWaitFcn))(SData) != 0) {
            DPRINTF((APERROR, "%s:  semaphore wait function failed.\n", Fname));
            return -1;
        }
    }

    /* --- Initialize the queue --- */
    queue->initialized = SR_QUEUE_UNINITIALIZED;
    queue->lock        = SR_QUEUE_LOCKED;
    queue->count       = 0;
    queue->SWaitFcn    = SWaitFcn;   /* Semaphore wait function */
    queue->SPostFcn    = SPostFcn;   /* Semaphore post function */
    queue->SData       = SData;      /* Data passed to semaphore functions */

    /* --- The head of the queue points to the tail --- */
    queue->Head.ptr   = NULL;
    queue->Head.prev  = NULL;
    queue->Head.next  = &(queue->Tail);

    /* --- The tail of the queue points back to the head --- */
    queue->Tail.ptr   = NULL;
    queue->Tail.prev  = &(queue->Head);
    queue->Tail.next  = NULL;

    /* --- Normal return --- */
    queue->initialized = SR_QUEUE_INITIALIZED;

    /*
     *  Unlock the queue.  We can use the SRQUnlock() function because the
     *  queue (and function pointers) have been initialized by this point.
     */
    SRQUnlock(queue);

    return 0;
}


/*
 *  SRQTerminate()
 *
 *  Function:  Terminates a queue.
 */
int SRQTerminate(SRQueue *queue, void (*PtrFreeFcn)(void *ptr))
{
    SRQEntry *head = NULL, *tail = NULL, *current = NULL, *temp = NULL;

    /* --- Return if no queue was specified --- */
    if (queue == NULL) {
        return 0;
    }

    /* --- Return if the queue is not initialized --- */
    if (queue->initialized != SR_QUEUE_INITIALIZED) {
        return 0;
    }

    /* --- Lock and/or wait on the queue --- */
    if (SRQLock(queue) == -1) {
        return -1;
    }

    /* --- Point to the sentinels --- */
    head = &(queue->Head);
    tail = &(queue->Tail);

    /* --- Loop through the queue freeing entries --- */
    for (current = head->next; current != tail; current = temp) {

        /* --- Get current out of queue --- */
        head->next = current->next;  /* Head points forward to next entry */
        current->next->prev = head;  /* Next entry points back to head */

        /* --- Call the free function to free the pointer --- */
        if ((current->ptr != NULL) && (PtrFreeFcn != NULL)) {
            (*(PtrFreeFcn)) (current->ptr);
        }

        /* --- Save the pointer to the next entry before free --- */
        temp = current->next;

        /* --- Free queue entry  --- */
        free(current);
    }

    /* --- Unlock the queue and call semaphore post function --- */
    SRQUnlock(queue);

    /* --- Mark the queue as uninitialized --- */
    queue->initialized = SR_QUEUE_UNINITIALIZED;

    return 0;
}


/*
 *  SRQEnqueue()
 *
 *  Function:  Puts an entry at the tail of a queue.
 */
int SRQEnqueue(SRQueue *queue, void *ptr)
{
    SRQEntry *new = NULL, *tail = NULL;

    /* --- Lock and/or wait on the queue --- */
    if (SRQLock(queue) == -1) {
        return -1;
    }

    /* --- Malloc a new queue entry --- */
    new = (SRQEntry *) malloc(sizeof(SRQEntry));
    if (new == NULL) {
        SRQUnlock(queue);
        return -1;
    }

    /* --- Point to queue tail --- */
    tail = &(queue->Tail);

    /* --- Populate the fields of the new queue entry --- */
    new->ptr   = ptr;
    new->prev  = tail->prev;   /* I point back to rest of queue */
    new->next  = tail;         /* I point forward to tail */

    /* --- Put me into the queue at the end  --- */
    tail->prev->next = new;   /* Put me after last entry */
    tail->prev = new;         /* Point tail back at me */

    /* --- Increment the count of queue entries --- */
    queue->count++;

    /* --- Unlock the queue and call semaphore post function --- */
    SRQUnlock(queue);

    return 0;
}


/*
 *  SRQEnqueueAtHead()
 *
 *  Function:  Puts an entry at the head of a queue.
 */
int SRQEnqueueAtHead(SRQueue *queue, void *ptr)
{
    SRQEntry *new = NULL, *head = NULL;

    /* --- Lock and/or wait on the queue --- */
    if (SRQLock(queue) == -1) {
        return -1;
    }

    /* --- Malloc a new queue entry --- */
    new = (SRQEntry *) malloc(sizeof(SRQEntry)); 
    if (new == NULL) {
        SRQUnlock(queue);
        return -1;
    }

    /* --- Point to queue head --- */
    head = &(queue->Head);

    /* --- Populate the fields of the new queue entry --- */
    new->ptr   = ptr;
    new->prev  = head;         /* I point back to head */
    new->next  = head->next;   /* I point forward to rest of queue */

    /* --- Put me into the queue at the beginning --- */
    head->next->prev = new;   /* Point second entry back at me */
    head->next = new;         /* Head now points to me */

    /* --- Increment the count of queue entries --- */
    queue->count++;

    /* --- Unlock the queue and call semaphore post function --- */
    SRQUnlock(queue);

    return 0;
}


/*
 *  SRQDequeue()
 *
 *
 *  Function:  Removes an entry from the head of the queue.
 */
int SRQDequeue(SRQueue *queue, void **ptr)
{
    SRQEntry *entry = NULL, *head = NULL;

    /* --- Lock and/or wait on the queue --- */
    if (SRQLock(queue) == -1) {
        return -1;
    }

    /* --- Get the head entry --- */
    head  = &(queue->Head);    /* Point to the head sentinel */
    entry = head->next;        /* The queue entry I want, not the sentinel */
   
    /* --- Fix up the queue head --- */
    head->next = entry->next;  /* Head points to entry after me */
   
    /* --- Fix up the entry after me --- */
    entry->next->prev = head;  /* Next entry points back to head */

    /* --- Get the stored objects, then free the SRQEntry --- */
    *ptr   = entry->ptr;      
    free(entry); 

    /* --- Decrement the count of queue entries --- */
    queue->count--;

    /* --- Unlock the queue and call semaphore post function --- */
    SRQUnlock(queue);

    return 0;
}


/*
 *  SRQEntryCount()
 *
 *  Function:  Returns the number of entries in a queue.
 */
int SRQEntryCount(SRQueue *queue)
{
    int count = -1;

    /* --- Lock the queue --- */
    if (SRQLock(queue) == -1) {
        return -1;
    }

    /* --- Get the count of queue entries --- */
    count = queue->count;

    /* --- Unlock the queue and call semaphore post function --- */
    SRQUnlock(queue);

    return count;
}


#ifdef SR_DEBUG
/*
 *  SRQDump()
 *
 *  Function:  Prints all entries in the TransportInfo queue.
 */
int SRQDump(SRQueue *queue, void (*PtrPrintFcn)(void *ptr))
{
    int count = 0;
    SRQEntry *current = NULL, *head = NULL, *tail = NULL;

    /* --- Return if no queue was specified --- */
    if (queue == NULL) {
        printf("SRQDump:  Queue is NULL.\n");
        return 0;
    }

    /* --- Return if the queue is not initialized --- */
    if (queue->initialized != SR_QUEUE_INITIALIZED) {
        printf("SRQDump:  Queue 0x%08lX is not initialized.\n", (unsigned long) queue);
        return 0;
    }

    /* --- Lock and/or wait on the queue --- */
    if (SRQLock(queue) == -1) {
        printf("SRQDump:  Queue 0x%08lX could not be locked.\n", (unsigned long) queue);
        return -1;
    }

    /* --- Point to the sentinels --- */
    head = &(queue->Head);
    tail = &(queue->Tail);

    printf("SRQDump:  Queue 0x%0lX:  %d entries, head=0x%0lX, tail=0x%0lX.\n", 
            (unsigned long) queue, (int) queue->count,
            (unsigned long) head, (unsigned long) tail);

    /* --- Loop through the queue printing entries --- */
    for (current = head; current != NULL; current = current->next) {

        /* --- Count the queue entry --- */
        count++;

        /* --- Skip the sentinel entries --- */
        if ((current == head) || (current == tail)) {
            continue;
        }

        /* --- Call the print function to print the pointer --- */
        if ((current->ptr != NULL) && (PtrPrintFcn != NULL)) {
            (*PtrPrintFcn)(current->ptr);
        }
    }

    /* --- Subtract head and tail sentinel entries --- */
    count -= 2;

    /* --- Print a message if the counts are wrong --- */
    if (queue->count != count) {
        printf("SRQDump:  Queue 0x0X thinks it has %d entries, I count %d entries.\n",
                queue->count, count);
    }
    printf("\n\n");

    /* --- Unlock the queue and call semaphore post function --- */
    SRQUnlock(queue);

    return 0;
}
#endif /* SR_DEBUG */
