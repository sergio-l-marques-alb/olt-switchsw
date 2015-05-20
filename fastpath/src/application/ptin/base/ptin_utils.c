/**
 * ptin_utils.c 
 *  
 * Implements general library functions (linked lists, bitmaps, etc) 
 *
 * Created on: 2011/07/19
 * Author: Alexandre Santos (alexandre-r-santos@ptinovacao.pt)
 */

#include <stdio.h>
#include "ptin_utils.h"


void hexdump (void *buf, size_t length, const char *startLine, const char *endLine)
{
  int i;
  unsigned char *buffer = (unsigned char *)buf;
  
  {//<print_header_table>
     if ( NULL != startLine ){ printf("%s", startLine); }
     printf("     ");
     for ( i = 0; 32 > i; i++ ) printf ("%2d ", i + 1);
     if ( NULL != endLine ){ printf("%s", endLine); }
     printf("\n");
  }
  for ( i = 0; i < length; i++ )
  {
     if ( 0 == (i % 32) )
     {
        if ( NULL != startLine ){ printf("%s", startLine); }
        printf("%3d: ", (i / 32) + 1);
     }
     printf("%02X ", buffer[i]);
     if ( (i == (length - 1)) && (0 != (length%32)) )
     {
        printf("%*.*s", (32 - (length%32))*3, (32 - (length%32))*3, "");
        if ( NULL != endLine ){ printf("%s", endLine); }
        printf("\n");
     }
     else if ( 0 == ((i + 1)%32) )
     {
        if ( NULL != endLine ){ printf("%s", endLine); }
        printf("\n");
     }
  }
}//hexdump


/****************************************************************************** 
 * DOUBLE-LINKED QUEUES MANIPULATION
 ******************************************************************************/

/**
 * Double-linked queue init
 * 
 * @param queue Pointer to the queue
 * 
 * @return RET_CODE_t NOERR/INVPARAM
 */
RET_CODE_t dl_queue_init(dl_queue_t *queue)
{
    if (queue == NULL)
        return INVPARAM;

    queue->n_elems = 0;
    queue->head    = NULL;
    queue->tail    = NULL;

    return NOERR;
}

/**
 * Add an element to the double-linked queue (tail)
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM
 */
RET_CODE_t dl_queue_add(dl_queue_t *queue, dl_queue_elem_t *elem)
{
    return dl_queue_add_tail(queue, elem);
}

/**
 * Add an element to the head of a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM
 */
RET_CODE_t dl_queue_add_head(dl_queue_t *queue, dl_queue_elem_t *elem)
{
    if (queue == NULL || elem == NULL)
        return INVPARAM;

    if (queue->n_elems == 0) {
        queue->head = elem;
        queue->tail = elem;
        elem->next = NULL;
        elem->prev = NULL;
    }
    else {
        queue->head->prev = elem;
        elem->prev = NULL;
        elem->next = queue->head;
        queue->head = elem;
    }

    queue->n_elems++;

    return NOERR;
}

/**
 * Add an element to the tail of a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM
 */
RET_CODE_t dl_queue_add_tail(dl_queue_t *queue, dl_queue_elem_t *elem)
{
    if (queue == NULL || elem == NULL)
        return INVPARAM;

    if (queue->n_elems == 0) {
        queue->head = elem;
        queue->tail = elem;
        elem->next = NULL;
        elem->prev = NULL;
    }
    else {
        queue->tail->next = elem;
        elem->prev = queue->tail;
        elem->next = NULL;
        queue->tail = elem;
    }

    queue->n_elems++;

    return NOERR;
}

/**
 * Remove an element from a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
RET_CODE_t dl_queue_remove(dl_queue_t *queue, dl_queue_elem_t *elem)
{
    if (queue == NULL || elem == NULL)
        return INVPARAM;

    if (queue->n_elems == 0)
        return EMPTYQUEUE;

    if (queue->n_elems == 1) {
        queue->head = NULL;
        queue->tail = NULL;
    }
    else {
        if (queue->head == elem) {
            queue->head = queue->head->next;
            queue->head->prev = NULL;
        }
        else if (queue->tail == elem) {
            queue->tail = queue->tail->prev;
            queue->tail->next = NULL;
        }
        else {
            elem->prev->next = elem->next;
            elem->next->prev = elem->prev;
        }
    }

    elem->next = NULL;
    elem->prev = NULL;
    queue->n_elems--;

    return NOERR;
}

/**
 * Remove the first element from a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the removed element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
RET_CODE_t dl_queue_remove_head(dl_queue_t *queue, dl_queue_elem_t **elem)
{
    if (queue == NULL || elem == NULL)
        return INVPARAM;

    if (queue->n_elems == 0)
        return EMPTYQUEUE;

    if (queue->n_elems == 1) {
        *elem = queue->head;
        queue->head = NULL;
        queue->tail = NULL;
    }
    else {
        *elem = queue->head;
        queue->head = queue->head->next;
        queue->head->prev = NULL;

        (*elem)->next = NULL;
        (*elem)->prev = NULL;
    }

    queue->n_elems--;

    return NOERR;
}

/**
 * Remove the last element from a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the removed element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
RET_CODE_t dl_queue_remove_tail(dl_queue_t *queue, dl_queue_elem_t **elem)
{
    if (queue == NULL || elem == NULL)
        return INVPARAM;

    if (queue->n_elems == 0)
        return EMPTYQUEUE;

    if (queue->n_elems == 1) {
        *elem = queue->tail;
        queue->head = NULL;
        queue->tail = NULL;
    }
    else {
        *elem = queue->tail;
        queue->tail = queue->tail->prev;
        queue->tail->next = NULL;

        (*elem)->next = NULL;
        (*elem)->prev = NULL;
    }

    queue->n_elems--;

    return NOERR;
}

/**
 * Get the first element from a double-linked queue (do not remove it)
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to a pointer to the read element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
RET_CODE_t dl_queue_get_head(dl_queue_t *queue, dl_queue_elem_t **elem)
{
    if (queue == NULL || elem == NULL)
        return INVPARAM;

    if (queue->n_elems == 0)
        return EMPTYQUEUE;

    *elem = queue->head;

    return NOERR;
}

/**
 * Get the last element from a double-linked queue (do not remove it)
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to a pointer to the read element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
RET_CODE_t dl_queue_get_tail(dl_queue_t *queue, dl_queue_elem_t **elem)
{
    if (queue == NULL || elem == NULL)
        return INVPARAM;

    if (queue->n_elems == 0)
        return EMPTYQUEUE;

    *elem = queue->tail;

    return NOERR;
}

/**
 * Get the next element (of a given element)
 *  
 * @param queue Pointer to the queue
 * @param elem Reference element
 * 
 * @return dl_queue_elem_t* Pointer to the next element (NULL if none)
 */
dl_queue_elem_t * dl_queue_get_next(dl_queue_t *queue, dl_queue_elem_t *elem)
{
    if (queue == NULL || elem == NULL)
        return NULL;

    if (queue->n_elems == 0)
        return NULL;

    return elem->next;
}


/****************************************************************************** 
 * BITMAPS MANIPULATION
 ******************************************************************************/

inline int _bmp_set(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit)
{
    bmp_cell_t   cellpos;
    unsigned int bitpos;

#ifdef BITMAP_VALIDATE_SIZE
    if (bit >= (sizeof(bmp_cell_t) * sizeof_bmp * 8)) {
        printf("%s(%d) *** CRITICAL *** Bit %u is out of range! Bitmap size = %u bits\n",
               __FUNCTION__, __LINE__, bit, (sizeof(bmp_cell_t) * sizeof_bmp * 8));
        return 0;
    }
#endif

    cellpos = bit / (sizeof(bmp_cell_t) * 8);
    bitpos  = bit % (sizeof(bmp_cell_t) * 8);

    bmp[cellpos] |= 1 << bitpos;

    return 1;
}

inline int _bmp_set_all(bmp_cell_t bmp[], unsigned int sizeof_bmp)
{
    unsigned int i;

    for (i=0; i<sizeof_bmp; i++)
        bmp[i] = (bmp_cell_t)-1;

    return 1;
}

inline int _bmp_clear(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit)
{
    bmp_cell_t   cellpos;
    unsigned int bitpos;

#ifdef BITMAP_VALIDATE_SIZE
    if (bit >= (sizeof(bmp_cell_t) * sizeof_bmp * 8)) {
        printf("%s(%d) *** CRITICAL *** Bit %u is out of range! Bitmap size = %u bits\n",
               __FUNCTION__, __LINE__, bit, (sizeof(bmp_cell_t) * sizeof_bmp * 8));
        return 0;
    }
#endif

    cellpos = bit / (sizeof(bmp_cell_t) * 8);
    bitpos  = bit % (sizeof(bmp_cell_t) * 8);

    bmp[cellpos] &= ~ (1 << bitpos);

    return 1;
}

inline int _bmp_clear_all(bmp_cell_t bmp[], unsigned int sizeof_bmp)
{
    unsigned int i;

    for (i=0; i<sizeof_bmp; i++)
        bmp[i] = 0;

    return 1;
}
                             
inline int _bmp_is_set(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit)
{
    bmp_cell_t   cellpos;
    unsigned int bitpos;

#ifdef BITMAP_VALIDATE_SIZE
    if (bit >= (sizeof(bmp_cell_t) * sizeof_bmp * 8)) {
        printf("%s(%d) *** CRITICAL *** Bit %u is out of range! Bitmap size = %u bits\n",
               __FUNCTION__, __LINE__, bit, (sizeof(bmp_cell_t) * sizeof_bmp * 8));
        return 0;
    }
#endif

    cellpos = bit / (sizeof(bmp_cell_t) * 8);
    bitpos  = bit % (sizeof(bmp_cell_t) * 8);

    return ((bmp[cellpos] >> bitpos) & 1);
}

inline int _bmp_is_all_set(bmp_cell_t bmp[], unsigned int sizeof_bmp)
{
    unsigned int i;

    for (i=0; i<sizeof_bmp; i++)
        if (bmp[i] != (bmp_cell_t)-1)
            return 0;

    return 1;
}

inline int _bmp_is_clear(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit)
{
    bmp_cell_t   cellpos;
    unsigned int bitpos;

#ifdef BITMAP_VALIDATE_SIZE
    if (bit >= (sizeof(bmp_cell_t) * sizeof_bmp * 8)) {
        printf("%s(%d) *** CRITICAL *** Bit %u is out of range! Bitmap size = %u bits\n",
               __FUNCTION__, __LINE__, bit, (sizeof(bmp_cell_t) * sizeof_bmp * 8));
        return 0;
    }
#endif

    cellpos = bit / (sizeof(bmp_cell_t) * 8);
    bitpos  = bit % (sizeof(bmp_cell_t) * 8);

    return !((bmp[cellpos] >> bitpos) & 1);
}

inline int _bmp_is_all_clear(bmp_cell_t bmp[], unsigned int sizeof_bmp)
{
    unsigned int i;

    for (i=0; i<sizeof_bmp; i++)
        if (bmp[i] != 0)
            return 0;

    return 1;
}

