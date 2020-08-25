/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2004-2007
 *
 * *********************************************************************
 *
 * @filename       avl.h
 *
 * @purpose        Double linked list management structures and macros
 *
 * @component      Routing Util Component
 *
 * @comments
 *
 * @create         12/29/2004
 *
 * @author         Ed Vartanian
 *
 * @end
 *
 * ********************************************************************/
#ifndef dllqueue_h
#define dllqueue_h

/*
 * Queue entry definitions
 */
typedef struct s_DllQueueEntry
{
    struct s_DllQueueEntry *next;
    struct s_DllQueueEntry *prev;
    struct s_DllQueueHdr   *hdr;
} t_DLLQEntry;

/*
 * Queue header definitions
 */
typedef struct s_DllQueueHdr
{
    struct s_DllQueueEntry *head;           /* head of DLL list */
    struct s_DllQueueEntry *tail;           /* tail of DLL list */
    int                     cnt;            /* count of entries on DLL list */
    unsigned short          offset;         /* hdr offset in entry structure */
    unsigned short          user_data;      /* user data */
} t_DLLQHdr;

/*****************************************************************************/
/* Related macros                                                            */
/*****************************************************************************/

/* peek at the head of the queue without removing the entry */
#define m_DLLQ_PEEK_FIRST(q) \
    ((void *)(((q)->head) ? (((char *)((q)->head)) - (q)->offset) : 0))

/* peek at the next element in the queue after the specified entry */
#define m_DLLQ_PEEK_NEXT(entry) \
    ((void *)(((entry)->next) ? \
              (((char *)(entry)->next) - (entry)->hdr->offset) : 0))

/* peek at the previous element in the queue after the specified entry */
#define m_DLLQ_PEEK_PREV(entry) \
    ((void *)(((entry)->prev) ? \
              (((char *)(entry)->prev) - (entry)->hdr->offset) : 0))

/* peek at the tail of the queue without removing the entry */
#define m_DLLQ_PEEK_LAST(q) \
    ((void *)(((q)->tail) ? \
              (((char *)((q)->tail)) - (q)->offset) : 0))

/* Is the list empty */
#define m_DLLQ_IS_EMPTY(q) (((q)->head) ? FALSE : TRUE)

/* How many elements in the queue */
#define m_DLLQ_GetEntriesNmb(q) ((q)->cnt)

/* return ptr to the queue header for the specified entry */
#define m_DLLQ_LIST_HEADER(entry) ((entry)->hdr)

/* Necessary for m_DLLQ_ENTRY_ON_QUEUE to work */
#define m_DLLQ_ENTRY_INIT(entry) \
    (entry)->next = NULL; (entry)->prev = NULL; (entry)->hdr = NULL

/* Is the entry on a queue? */
#define m_DLLQ_ENTRY_ON_QUEUE(entry) ((entry)->hdr != NULL)

/* Access user_data in header */
#define m_DLLQ_USER_DATA(entry) (entry)->hdr->user_data

#endif /* dllqueue_h */
