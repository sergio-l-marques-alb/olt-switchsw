/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2004-2007
 *
 * *********************************************************************
 *
 * @filename      dllqueue.c 
 *
 * @purpose       Double Linked List Queue implementation
 *
 * @component     Routing Utils Component
 *
 * @comments     
 * DLLQ_Init          Initialize queue
 * DLLQ_PutTail       Put an entry on the tail of the queue
 * DLLQ_PutHead       Put an entry on the head of the queue
 * DLLQ_GetTail       Get an entry from the tail of the queue
 * DLLQ_GetHead       Get an entry from the head of the queue
 * DLLQ_RemoveEntry   Remove an entry from the queue
 * DLLQ_InsertBefore  Insert entry before the current entry
 * DLLQ_InsertAfter   Insert entry after the current entry
 * DLLQ_Splice        Splice all entries from one queue to another
 * DLLQ_FreeList      Remove and free all entries on a queue
 *
 * @create    12/29/2004
 *
 * @author    Ed Vartanian
 *
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: %M% %I% %G%$";
#endif
#include <stdlib.h>
#include <stdio.h>
#include "std.h"
#include "dllqueue.h"
#include "dllqueue.ext"
#include "xx.ext"

#ifdef DEBUG
#define DEBUG_CHECK_LIST_CNT(q) check_list_cnt(q)
#else
#define DEBUG_CHECK_LIST_CNT(q)
#endif

/*****************************************************************************/
/* For DEBUG only: check the number of elements in the list vs the count.    */
/*****************************************************************************/
void check_list_cnt(t_DLLQHdr *q)
{
    int cnt;
    t_DLLQEntry *e;

    ASSERT(q->cnt >= 0);
    cnt = 0;
    e = q->head;
    while (e)
    {
        cnt++;
        ASSERT(e->hdr == q);
        if (e->next == NULL) ASSERT(q->tail == e);
        e = e->next;
    }
    ASSERT(cnt == q->cnt);
    cnt = 0;
    e = q->tail;
    while (e)
    {
        cnt++;
        ASSERT(e->hdr == q);
        if (e->prev == NULL) ASSERT(q->head == e);
        e = e->prev;
    }
    ASSERT(cnt == q->cnt);
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_Init
 *
 * DESCRIPTION:
 *    Initialize a double linked list (dll) queue.
 *
 * ARGUMENTS:
 *    q         - ptr to queue header
 *    offset    - offset from element's t_DLLQEntry to address of element.
 *    user_data - user defined information to save in header
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/

void DLLQ_Init(t_DLLQHdr *q, unsigned short offset, unsigned short user_data) 
{
    q->head = NULL;
    q->tail = NULL;
    q->cnt = 0;
    q->offset = offset;
    q->user_data = user_data;
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_PutTail
 *
 * DESCRIPTION:
 *    Add an entry to the tail of the queue.
 *
 * ARGUMENTS:
 *    q         - ptr to queue header
 *    newEntry  - ptr to new element to add to queue
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/

void DLLQ_PutTail(t_DLLQHdr *q, t_DLLQEntry *newEntry)
{
    XX_Freeze();
    if (q->tail)
    {
        q->tail->next = newEntry;
    }
    else
    {
        q->head = newEntry;
    }
    newEntry->next = NULL;
    newEntry->prev = q->tail;
    newEntry->hdr = q;
    q->tail = newEntry;
    q->cnt++;
    ASSERT(q->cnt > 0);
    ASSERT(q->head != NULL && q->tail != NULL);
    DEBUG_CHECK_LIST_CNT(q);
    XX_Thaw();
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_PutHead
 *
 * DESCRIPTION:
 *    Add an entry to the head of the queue.
 *
 * ARGUMENTS:
 *    q         - ptr to queue header
 *    newEntry  - ptr to new element to add to queue
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/
    
void DLLQ_PutHead(t_DLLQHdr *q, t_DLLQEntry *newEntry)
{
    XX_Freeze();
    if (q->head)
    {
        q->head->prev = newEntry;
    }
    else
    {
        q->tail = newEntry;
    }
    newEntry->next = q->head;
    newEntry->prev = NULL;
    newEntry->hdr = q;
    q->head = newEntry;
    q->cnt++;
    ASSERT(q->cnt > 0);
    ASSERT(q->head != NULL && q->tail != NULL);
    DEBUG_CHECK_LIST_CNT(q);
    XX_Thaw();
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_GetTail
 *
 * DESCRIPTION:
 *    Remove an entry from the tail of the queue.
 *
 * ARGUMENTS:
 *    q         - ptr to queue header
 *
 * RESULTS:
 *    ptr to entry removed from queue or NULL
 *--------------------------------------------------------------------*/

void *DLLQ_GetTail(t_DLLQHdr *q)
{
    char        *base = NULL;
    t_DLLQEntry *entry;

    XX_Freeze();
    entry = q->tail;
    if (entry)
    {
        q->cnt--;
        q->tail = entry->prev;
        if (q->tail)
        {
            q->tail->next = NULL;
        }
        else
        {
            q->head = NULL;
        }

        /* Mark entry as not being on the queue */
        m_DLLQ_ENTRY_INIT(entry);

        base = (char *)entry - q->offset;
        DEBUG_CHECK_LIST_CNT(q);
    }
    XX_Thaw();

    return (void *)base;
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_GetHead
 *
 * DESCRIPTION:
 *    Remove an entry from the heaad of the queue.
 *
 * ARGUMENTS:
 *    q         - ptr to queue header
 *
 * RESULTS:
 *    ptr to entry removed from queue or NULL
 *--------------------------------------------------------------------*/
    
void *DLLQ_GetHead(t_DLLQHdr *q)
{
    char        *base = NULL;
    t_DLLQEntry *entry;

    XX_Freeze();
    entry = q->head;
    if (entry)
    {
        q->cnt--;
        q->head = entry->next;
        if (q->head)
        {
            q->head->prev = NULL;
        }
        else
        {
            q->tail = NULL;
        }

        /* Mark entry as not being on the queue */
        m_DLLQ_ENTRY_INIT(entry);

        base = (char *)entry - q->offset;
        DEBUG_CHECK_LIST_CNT(q);
    }
    XX_Thaw();

    return (void *)base;
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_RemoveEntry
 *
 * DESCRIPTION:
 *    Remove the specified entry from the queue.  The entry may be
 *    any position in the queue.
 *
 * ARGUMENTS:
 *    entry     - ptr to entry to be removed
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/

void DLLQ_RemoveEntry(t_DLLQEntry *entry)
{
    t_DLLQHdr   *q;

    if (entry)
    {
        XX_Freeze();
        q = entry->hdr;
        ASSERT(q->cnt > 0);
        q->cnt--;
        if (entry->prev)
        {
            entry->prev->next = entry->next;
        }
        else
        {
            q->head = entry->next;
        }
        if (entry->next)
        {
            entry->next->prev = entry->prev;
        }
        else
        {
            q->tail = entry->prev;
        }

        /* Mark entry as not being on the queue */
        m_DLLQ_ENTRY_INIT(entry);
        DEBUG_CHECK_LIST_CNT(q);
        XX_Thaw();
    }
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_InsertBefore
 *
 * DESCRIPTION:
 *    Insert an entry before the specified entry which is already in the
 *    queue.
 *
 * ARGUMENTS:
 *    entry     - ptr to entry already in queue
 *    newEntry  - ptr to new entry to be added
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/
    
void DLLQ_InsertBefore(t_DLLQEntry *entry, t_DLLQEntry *newEntry)
{
    t_DLLQHdr *q;

    XX_Freeze();
    newEntry->next = entry;
    newEntry->prev = entry->prev;
    if (entry->prev)
    {
        entry->prev->next = newEntry;
    }
    entry->prev = newEntry;
    q = newEntry->hdr = entry->hdr;
    q->cnt++;
    if (q->head == entry)
    {
        q->head = newEntry;
    }
    DEBUG_CHECK_LIST_CNT(q);
    XX_Thaw();
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_InsertAfter
 *
 * DESCRIPTION:
 *    Insert an entry after the specified entry which is already in the
 *    queue.
 *
 * ARGUMENTS:
 *    entry     - ptr to entry already in queue
 *    newEntry  - ptr to new entry to be added
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/
    
void DLLQ_InsertAfter(t_DLLQEntry *entry, t_DLLQEntry *newEntry)
{
    t_DLLQHdr *q;

    XX_Freeze();
    newEntry->next = entry->next;
    newEntry->prev = entry;
    if (entry->next)
    {
        entry->next->prev = newEntry;
    }
    entry->next = newEntry;
    q = newEntry->hdr = entry->hdr;
    q->cnt++;
    if (q->tail == entry)
    {
        q->tail = newEntry;
    }
    DEBUG_CHECK_LIST_CNT(q);
    XX_Thaw();
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_Splice
 *
 * DESCRIPTION:
 *    Splice all entries on queue q on queue p.  Move the entire list
 *    of entries on q to the end of p.  Queue q is empty when finished.
 *
 * ARGUMENTS:
 *    p         - ptr to destination queue header
 *    q         - ptr to source queue header
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/

void DLLQ_Splice(t_DLLQHdr *p, t_DLLQHdr *q)
{
    t_DLLQEntry *ent;

    ASSERT(p->offset == q->offset); /* same type of queue */

    /* change all the hdr pointers to point to the new queue header */
    XX_Freeze();
    if ((ent = q->head) != NULL)
    {
        while (ent)
        {
            ent->hdr = p;
            ent = ent->next;
        }
    }
    if (p->tail)
    {
        p->tail->next = q->head;
        if (q->head)
        {
            q->head->prev = p->tail;
            /* Move p->tail only if q is not empty */
            p->tail = q->tail;
        }
    }
    else
    {
        p->head = q->head;
        p->tail = q->tail;
    }
    p->cnt += q->cnt;
    ASSERT(p->cnt >= 0);
    if (p->cnt > 0)
        ASSERT(p->head != NULL && p->tail != NULL);

    DLLQ_Init(q, q->offset, q->user_data);
    DEBUG_CHECK_LIST_CNT(p);
    XX_Thaw();
}

/*--------------------------------------------------------------------
 * ROUTINE:      DLLQ_Splice
 *
 * DESCRIPTION:
 *    Remove and free all entries on a queue.
 *
 * ARGUMENTS:
 *    p         - ptr to queue header
 *
 * RESULTS:
 *    None
 *--------------------------------------------------------------------*/

void DLLQ_FreeList(t_DLLQHdr *p)
{
    void *e;

    while ((e = DLLQ_GetHead(p)) != NULL)
    {
        XX_Free(e);
    }
}

