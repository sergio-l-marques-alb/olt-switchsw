/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    11/12/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#ifndef _PTIN_FIFO_API_H_
#define _PTIN_FIFO_API_H_

#include "ptin_mgmd_defs.h"

typedef void* PTIN_FIFO_ELEMENT_t;
typedef void* PTIN_FIFO_t;


/**
 * Create a new FIFO queue.
 * 
 * @param[in|out] fifoQueue   : Ptr to the FIFO queue 
 * @param[in]     numElements : Maximum number of elements in the queue
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_create(PTIN_FIFO_t* fifoQueue, uint32 numElements);

/**
 * Create a new FIFO queue.
 * 
 * @param[in] fifoQueue : FIFO queue 
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_destroy(PTIN_FIFO_t fifoQueue);

/**
 * Return an element to the FIFO queue.
 * 
 * @param[in] fifoQueue : FIFO queue 
 * @param[in] element   : Element to insert in the queue
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_push(PTIN_FIFO_t fifoQueue, PTIN_FIFO_ELEMENT_t element);

/**
 * Get an element from the FIFO queue.
 * 
 * @param[in] fifoQueue : FIFO queue 
 * @param[in] element   : Ptr to element to get from the queue
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_pop(PTIN_FIFO_t fifoQueue, PTIN_FIFO_ELEMENT_t* element);

/**
 * Dump the contents of the FIFO queue
 * 
 * @param[in] fifoQueue : FIFO queue 
 * 
 * @return RC_t 
 */
void dumpQueue(PTIN_FIFO_t fifoQueue);

/**
 * Return the number of used elements in the FIFO.
 */
uint32 ptin_fifo_numUsedElements(PTIN_FIFO_t fifoQueue);

/**
 * Return the number of free elements in the FIFO.
 */
uint32 ptin_fifo_numFreeElements(PTIN_FIFO_t fifoQueue);

#endif //_PTIN_FIFO_API_H_
