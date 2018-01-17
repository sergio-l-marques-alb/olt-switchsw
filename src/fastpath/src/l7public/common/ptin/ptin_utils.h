/**
 * ptin_utils.h 
 *  
 * Implements general library functions (linked lists, bitmaps, etc) 
 *
 * Created on: 2011/07/19
 * Author: Alexandre Santos (alexandre-r-santos@ptinovacao.pt)
 */

#ifndef _PTIN_UTILS_H
#define _PTIN_UTILS_H

/*
 * Double-Linked Queues Structures
 */

/* Linked element */
typedef struct dl_queue_elem_s {
  struct dl_queue_elem_s *next;
  struct dl_queue_elem_s *prev;
} dl_queue_elem_t;

/* Double linked Queue */
typedef struct {
  unsigned int n_elems;   /* Number of elements on the queue */
  dl_queue_elem_t *head;  /* Pointer to the first element (NULL is none) */
  dl_queue_elem_t *tail;  /* Pointer to the last element (NULL is none) */
} dl_queue_t;


/*
 * Bitmaps Manipulation
 */
typedef unsigned char bmp_cell_t;   /* bitmap cell type (typically 8 bits) */

#define BITMAP_VALIDATE_SIZE        /* If defined, adds bitmap overflow check */

/* Bitmap instantiation */
#define BITMAP(name, n_bits)           bmp_cell_t name[(n_bits-1)/(sizeof(bmp_cell_t)*8) + 1]
#define BITMAP_VECTOR(name, v, n_bits) bmp_cell_t name[v][(n_bits-1)/(sizeof(bmp_cell_t)*8) + 1]

/* Bitmap manipulation */
#define BITMAP_SET(bmp, bit)        _bmp_set(bmp, sizeof(bmp), bit)
#define BITMAP_SET_ALL(bmp)         _bmp_set_all(bmp, sizeof(bmp))

#define BITMAP_CLEAR(bmp, bit)      _bmp_clear(bmp, sizeof(bmp), bit)
#define BITMAP_CLEAR_ALL(bmp)       _bmp_clear_all(bmp, sizeof(bmp))

#define BITMAP_IS_SET(bmp, bit)     _bmp_is_set(bmp, sizeof(bmp), bit)
#define BITMAP_IS_ALL_SET(bmp)      _bmp_is_all_set(bmp, sizeof(bmp))

#define BITMAP_IS_CLEAR(bmp, bit)   _bmp_is_clear(bmp, sizeof(bmp), bit)
#define BITMAP_IS_ALL_CLEAR(bmp)    _bmp_is_all_clear(bmp, sizeof(bmp))


/* Return codes (for queues functions) */
typedef enum {
  NOERR = 0,
  NOTSUPPORTED,
  INVPARAM,
  EMPTYQUEUE,
} RET_CODE_t;


extern void hexdump (void *buffer, size_t length, const char *startLine, const char *endLine);

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
extern RET_CODE_t dl_queue_init(dl_queue_t *queue);

/**
 * Add an element to the double-linked queue (tail)
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM
 */
extern RET_CODE_t dl_queue_add(dl_queue_t *queue, dl_queue_elem_t *elem);

/**
 * Add an element to the head of a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM
 */
extern RET_CODE_t dl_queue_add_head(dl_queue_t *queue, dl_queue_elem_t *elem);

/**
 * Add an element to the tail of a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM
 */
extern RET_CODE_t dl_queue_add_tail(dl_queue_t *queue, dl_queue_elem_t *elem);

/**
 * Remove an element from a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
extern RET_CODE_t dl_queue_remove(dl_queue_t *queue, dl_queue_elem_t *elem);

/**
 * Remove the first element from a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the removed element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
extern RET_CODE_t dl_queue_remove_head(dl_queue_t *queue, dl_queue_elem_t **elem);

/**
 * Remove the last element from a double-linked queue
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to the removed element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
extern RET_CODE_t dl_queue_remove_tail(dl_queue_t *queue, dl_queue_elem_t **elem);

/**
 * Get the first element from a double-linked queue (do not remove it)
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to a pointer to the read element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
extern RET_CODE_t dl_queue_get_head(dl_queue_t *queue, dl_queue_elem_t **elem);

/**
 * Get the last element from a double-linked queue (do not remove it)
 * 
 * @param queue Pointer to the queue
 * @param elem Pointer to a pointer to the read element
 * 
 * @return RET_CODE_t NOERR/INVPARAM/EMPTYQUEUE
 */
extern RET_CODE_t dl_queue_get_tail(dl_queue_t *queue, dl_queue_elem_t **elem);

/**
 * Get the next element (of a given element)
 *  
 * @param queue Pointer to the queue
 * @param elem Reference element
 * 
 * @return dl_queue_elem_t* Pointer to the next element (NULL if none)
 */
extern dl_queue_elem_t * dl_queue_get_next(dl_queue_t *queue, dl_queue_elem_t *elem);


/****************************************************************************** 
 * BITMAPS MANIPULATION
 ******************************************************************************/

extern inline int _bmp_set(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit);
extern inline int _bmp_set_all(bmp_cell_t bmp[], unsigned int sizeof_bmp);

extern inline int _bmp_clear(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit);
extern inline int _bmp_clear_all(bmp_cell_t bmp[], unsigned int sizeof_bmp);
                                    
extern inline int _bmp_is_set(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit);
extern inline int _bmp_is_all_set(bmp_cell_t bmp[], unsigned int sizeof_bmp);

extern inline int _bmp_is_clear(bmp_cell_t bmp[], unsigned int sizeof_bmp, unsigned int bit);
extern inline int _bmp_is_all_clear(bmp_cell_t bmp[], unsigned int sizeof_bmp);

#endif
