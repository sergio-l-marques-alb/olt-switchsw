/*
 *
 * Copyright (C) 1992-2002 by SNMP Research, Incorporated.
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
 *  Function:  This file defines function constants, functions, and 
 *             prototypes for implementing general-purpose queues.
 *
 *  Notes:     The queues handle (void *) pointers, so pointers to any 
 *             type of data structure may be handled by these queues.
 *
 *             The queues are implemented using linked-lists so the
 *             queues do not have a fixed size.
 */

#ifndef SR_SR_QUEUE_H
#define SR_SR_QUEUE_H

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *  Convenience typedefs.
 */
typedef  struct _SRQEntry  SRQEntry;
typedef  struct _SRQueue   SRQueue;


/*
 *  Define the queue entry structure.
 */
struct _SRQEntry {
    void               *ptr;
    struct  _SRQEntry  *prev;
    struct  _SRQEntry  *next;
};


/*
 *  Define the queue structure.
 */
struct _SRQueue {
    volatile int lock;                /* The queue locked/unlocked flag */
    int      initialized;             /* Indicates the queue is initialized */
    int      count;                   /* The number of queue entries */
    int      (*SWaitFcn)(void *ptr);  /* Semaphore wait function pointer */
    int      (*SPostFcn)(void *ptr);  /* Semaphore post function pointer */
    void     *SData;                  /* Data to be passed to semaphore fcns */
    struct   _SRQEntry Head;          /* Sentinel at head of the queue */
    struct   _SRQEntry Tail;          /* Sentinel at tail of the queue */
};


/*
 *  Define the un/initialized values for a queue.  Where the un/initialized
 *  status is checked, the code returns if the queue initialization is not
 *  equal to SR_QUEUE_INITIALIZED.  This means that out of 2^32 possible ran-
 *  dom values for an int memory location, exactly one specifies the queue as
 *  initialized.  The value 0x55 is as good as the value 1, and perhaps a
 *  little more unique.  There is no special significance attached to 0x55.
 *  Note that if the operating system clears the memory before handing it
 *  to you, the value 0 means the queue is uninitialized.
 */
#define SR_QUEUE_UNINITIALIZED   0x00
#define SR_QUEUE_INITIALIZED     0x55


/*
 *  Define the un/locked values for a queue.  Where the un/locked status is
 *  checked, the code returns if the queue initialization is not equal to
 *  SR_QUEUE_UNLOCKED.  This means that out of 2^32 possible random values
 *  for an int memory location, exactly one specifies the queue as unlocked.
 *  The value 0xAA is as good as the value 1, and perhaps a little more uni-
 *  que.  There is no special significance attached to 0x55.  Note that if 
 *  the operating system clears the memory before handing it to you, the 
 *  value 0 means the queue is locked.
 */
#define SR_QUEUE_LOCKED     0x00
#define SR_QUEUE_UNLOCKED   0xAA


/*
 *  SRQMalloc()
 *
 *  Function:  Provides a convenience function for allocating and
 *             initializing a queue.
 *
 *  Inputs:    SWaitFcn - specifies an optional pointer for calling a sema-
 *                        phore wait function if the queue is locked.
 *             SPostFcn - specifies an optional pointer for calling a sema-
 *                        phore post function when the queue is unlocked.
 *             SData    - specifies data passed to the semaphore functions.
 *
 *
 *  Returns:   On success, returns a pointer to the queue.  
 *             On failure, returns NULL.
 *
 *  Notes:     This is a wrapper around malloc() and SRQInitialize().
 *
 *             It is the caller's responsibility to free the queue pointer
 *             returned by this function via SRQFree().
 */
SRQueue *
SRQMalloc(
    int     (*SWaitFcn)(void *ptr),
    int     (*SPostFcn)(void *ptr),
    void    *SData);


/*
 *  SRQFree()
 *
 *  Function:  Provides a convenience function for terminating and 
 *             freeing a queue.
 *
 *  Inputs:    queue - points to the queue to free.
 *             PtrFreeFcn - provides an optional function pointer for freeing
 *                          whatever object is stored in the pointer field of
 *                          the structure.
 *
 *  Returns:   On success, returns 0.  On failure, returns -1.
 *
 *  Notes:     This is a wrapper around SRQTerminate() and free().
 */
int
SRQFree(SRQueue *queue, void (*PtrFreeFcn)(void *ptr));


/*
 *  SRQInitialize()
 *
 *  Function:  Initializes a queue.
 *
 *  Inputs:    queue    - points to the queue.
 *             SWaitFcn - specifies an optional pointer for calling a sema-
 *                        phore wait function if the queue is locked.
 *             SPostFcn - specifies an optional pointer for calling a sema-
 *                        phore post function when the queue is unlocked.
 *             SData    - specifies data passed to the semaphore functions.
 *
 *  Returns:   On success, returns 0.  On failure, returns -1.
 *             (Success means the queue is initialized, ready for use.)
 *
 *  Notes:     This function does not malloc a queue structure because
 *             in many cases the user would want these statically defined.
 */
int
SRQInitialize(SRQueue *queue,
        int     (*SWaitFcn)(void *ptr),
        int     (*SPostFcn)(void *ptr),
        void    *SData);


/*
 *  SRQTerminate()
 *
 *  Function:  Terminates a queue.
 *
 *  Inputs:    queue      - points to the queue.
 *             PtrFreeFcn - provides an optional function pointer for freeing 
 *                          whatever object is stored in the pointer field of
 *                          the structure.
 *
 *  Returns:   On success, returns 0.  On failure, returns -1.
 *
 *  Notes:     This function locks the queue, and leaves it locked.
 *
 *             If available, this function will call the semaphore wait 
 *             function, but will not call the semaphore post function.
 *
 *             Note that either  1) the queue should be empty when this
 *             function is called; or  2) you must provide a function to
 *             free the data structures stored in the queue.  Failure to
 *             to one or the other of these will result in leaked memory.
 *             The leaked memory will occur because this function frees
 *             the queue entries, so if it cannot also free the data 
 *             structures the queue entries point to, memory leaks will
 *             result.
 *
 *             Upon return from this function, the queue will be empty.
 *             It will be marked as uninitialized, ready for freeing.
 */
int 
SRQTerminate(SRQueue *queue, void (*PtrFreeFcn)(void *ptr));


/*
 *  SRQEnqueue()
 *
 *  Function:  Puts an entry at the tail of a queue.
 *
 *  Inputs:    queue - points to the queue.
 *             ptr   - Points to the data structure to be queued.
 *
 *  Returns:   On success, returns 0.  On failure, returns -1.
 *             (Success means the pointer was put in the queue.)
 *
 *  Notes:     This function does not clone the structure specified by
 *             ptr, it merely saves the pointer in the queue.
 *
 *             This function locks/unlocks the queue.  If the queue was
 *             initialized with semaphore function pointers, the sema-
 *             phore functions will be called.  If the queue was init-
 *             ialized without semaphore function pointers, this function
 *             will return -1 if the queue is locked.
 */
int 
SRQEnqueue(SRQueue *queue, void *ptr);


/*
 *  SRQEnqueueAtHead()
 *
 *  Function:  Puts an entry at the head of a queue.
 *
 *  Inputs:    queue - points to the queue.
 *             ptr   - Points to the data structure to be queued.
 *
 *  Returns:   On success, returns 0.  On failure, returns -1.
 *             (Success means the pointer was put in the queue.)
 *
 *  Notes:     This function does not clone the structure specified by
 *             ptr, it merely saves the pointer in the queue.
 *
 *             This function locks/unlocks the queue.  If the queue was
 *             initialized with semaphore function pointers, the sema-
 *             phore functions will be called.  If the queue was init-
 *             ialized without semaphore function pointers, this function
 *             will return -1 if the queue is locked.
 *
 *             Because structures are always removed from the head of the
 *             queue, this function provides a way of "boosting the priority"
 *             of structures when needed.  When a structure is queued by this
 *             function, that structure will be the next out of the queue.
 */
int 
SRQEnqueueAtHead(SRQueue *queue, void *ptr);


/*
 *  SRQDequeue()
 *
 *  Function:  Removes an entry from the head of the queue.
 *
 *  Inputs:    queue - points to the queue.
 *             ptr   - points to a void *ptr where the pointer to the
 *                     queued data structure can be returned.
 *
 *  Returns:   On failure, returns -1.  On success, returns 0 and sets the
 *             **ptr to with the values from the queue entry.
 *
 *  Notes:     This function locks/unlocks the queue.  If the queue was
 *             initialized with semaphore function pointers, the sema-
 *             phore functions will be called.  If the queue was init-
 *             ialized without semaphore function pointers, this function
 *             will return -1 if the queue is locked.
 */
int 
SRQDequeue(SRQueue *queue, void **ptr);


/*
 *  SRQEntryCount()
 *
 *  Function:  Returns the number of entries in a queue.
 *
 *  Inputs:    queue - points to the queue.
 *
 *  Returns:   On failure, returns -1.  On success, returns the
 *             number of entries in the queue.
 *
 *  Notes:     This function does not lock the queue.
 */
int 
SRQEntryCount(SRQueue *queue);


#ifdef SR_DEBUG
/*
 *  SRQDump()
 *
 *  Function:  Prints all entries in the queue.
 *
 *  Inputs:    queue       - points to the queue.
 *             PtrPrintFcn - points to a function to print the stored object.
 *
 *  Returns:   On success, returns 0.  On failure, returns -1.
 *
 *  Notes:     This function locks/unlocks the queue.
 */
int 
SRQDump(SRQueue *queue, void (*PtrPrintFcn)(void *ptr));
#endif /* SR_DEBUG */


#ifdef  __cplusplus
}
#endif  /* __cplusplus */

/* Do not put anything after this #endif */
#endif /* SR_SR_QUEUE_H */
