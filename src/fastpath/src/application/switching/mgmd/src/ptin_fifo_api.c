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

#include "ptin_fifo_api.h"
#include "ptin_mgmd_osapi.h"
#include "ptin_mgmd_logger.h"

typedef struct 
{
  PTIN_FIFO_ELEMENT_t data;

  void*               next;
  void*               prev;
} ELEMENT_t;

typedef struct 
{
  ELEMENT_t* first_free;
  ELEMENT_t* last_free;

  ELEMENT_t* first_used;
  ELEMENT_t* last_used;
} FIFO_QUEUE_t;


/**
 * Dump the contents of the FIFO queue
 * 
 * @param[in] fifoQueue : FIFO queue 
 * 
 * @return RC_t 
 */
void dumpQueue(PTIN_FIFO_t fifoQueue)
{
  ELEMENT_t    *curr_element;
  FIFO_QUEUE_t *queue = (FIFO_QUEUE_t*) fifoQueue;

  //Used elements
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Used Elements:");
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\tFirst Used: %p", queue->first_used);
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\tLast Used:  %p", queue->last_used);
  curr_element = queue->first_used;
  while(curr_element != PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\tElement: %p", curr_element);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\t\tData:    %p", curr_element->data);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\t\tNext:    %p", curr_element->next);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\t\tPrev:    %p", curr_element->prev);

    curr_element = curr_element->next;
  }

  //Free elements
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Queue (%p)", queue);
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Free Elements:");
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\tFirst Free: %p", queue->first_free);
  PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\tLast Free:  %p", queue->last_free);
  curr_element = queue->first_free;
  while(curr_element != PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\tElement: %p", curr_element);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\t\tData:    %p", curr_element->data);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\t\tNext:    %p", curr_element->next);
    PTIN_MGMD_LOG_TRACE(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "\t\t\tPrev:    %p", curr_element->prev);

    curr_element = curr_element->next;
  }
}

/**
 * Return the number of used elements in the FIFO.
 */
uint32 ptin_fifo_numUsedElements(PTIN_FIFO_t fifoQueue)
{
  uint32       num_elements = 0;
  ELEMENT_t    *curr_element;
  FIFO_QUEUE_t *queue = (FIFO_QUEUE_t*) fifoQueue;

  curr_element = queue->first_used;
  while(curr_element != PTIN_NULLPTR)
  {
    ++num_elements;
    curr_element = curr_element->next;
  }

  return num_elements;
}

/**
 * Return the number of free elements in the FIFO.
 */
uint32 ptin_fifo_numFreeElements(PTIN_FIFO_t fifoQueue)
{
  uint32       num_elements = 0;
  ELEMENT_t    *curr_element;
  FIFO_QUEUE_t *queue = (FIFO_QUEUE_t*) fifoQueue;

  curr_element = queue->first_free;
  while(curr_element != PTIN_NULLPTR)
  {
    ++num_elements;
    curr_element = curr_element->next;
  }

  return num_elements;
}

/**
 * Create a new FIFO queue.
 * 
 * @param[in|out] fifoQueue   : Ptr to the FIFO queue 
 * @param[in]     numElements : Maximum number of elements in the queue
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_create(PTIN_FIFO_t* fifoQueue, uint32 numElements)
{
  uint32       i;
  FIFO_QUEUE_t *queue;

  //Input validation
  if(fifoQueue == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Error: Abnormal context [fifoQueue:%p]", fifoQueue);
    return FAILURE;
  }

  //Create a new queue
  queue = (FIFO_QUEUE_t*) ptin_mgmd_malloc(sizeof(FIFO_QUEUE_t));
    
  //Initialization
  queue->first_used = PTIN_NULLPTR;
  queue->last_used  = PTIN_NULLPTR;
  queue->last_free  = PTIN_NULLPTR;
  for (i=0; i<numElements; ++i)
  {
    ELEMENT_t *new_element = (ELEMENT_t*) ptin_mgmd_malloc(sizeof(ELEMENT_t));

    new_element->prev = queue->last_free;
    new_element->next = PTIN_NULLPTR;
    new_element->data = PTIN_NULLPTR;
    
    if(i == 0)
    {
      queue->first_free = new_element;
    }
    else
    {
      queue->last_free->next = new_element;
    }
    queue->last_free = new_element;
  }

  *fifoQueue = (PTIN_FIFO_t) queue;

  return SUCCESS;
}


/**
 * Create a new FIFO queue.
 * 
 * @param[in] fifoQueue : FIFO queue 
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_destroy(PTIN_FIFO_t fifoQueue)
{
//FIFO_QUEUE_t *queue = (FIFO_QUEUE_t*) fifoQueue;
//ELEMENT_t    *element;

  //Input validation
  if(fifoQueue == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Error: Abnormal context [fifoQueue:%p]", fifoQueue);
    return FAILURE;
  }
//
///* If we were requested to destroy ourselvs, but we still have
// * free elements, issue out a warning as it will create a memory leak */
//if(queue->first_free != PTIN_NULLPTR)
//{
//  LOG_WARNING(LOG_CTX_PTIN_FIFO, "Memory leak: Destroying queue while some of its elements are still in use by others.", fifoQueue);
//}
//
////Free data pointers
//curr_element = fifoQueue->first_free;
//while(curr_element != PTIN_NULLPTR)
//{
//  curr_element = curr_element->next;
//}
//
////Free elements
//LOG_TRACE(LOG_CTX_PTIN_FIFO, "Queue (%p)", fifoQueue);
//LOG_TRACE(LOG_CTX_PTIN_FIFO, "\tFirst Used: %p", fifoQueue->first_used);
//LOG_TRACE(LOG_CTX_PTIN_FIFO, "\tLast Used:  %p", fifoQueue->last_used);
//curr_element = fifoQueue->first_used;
//while(curr_element != PTIN_NULLPTR)
//{
//  LOG_TRACE(LOG_CTX_PTIN_FIFO, "\t\tElement: %p", curr_element);
//  LOG_TRACE(LOG_CTX_PTIN_FIFO, "\t\t\tData:    %p", curr_element->data);
//  LOG_TRACE(LOG_CTX_PTIN_FIFO, "\t\t\tNext:    %p", curr_element->next);
//  LOG_TRACE(LOG_CTX_PTIN_FIFO, "\t\t\tPrev:    %p", curr_element->prev);
//
//  curr_element = curr_element->next;
//}
//
//

  return SUCCESS;
}


/**
 * Return an element to the FIFO queue.
 * 
 * @param[in] fifoQueue : FIFO queue 
 * @param[in] element   : Element to insert in the queue
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_push(PTIN_FIFO_t fifoQueue, PTIN_FIFO_ELEMENT_t element)
{
  FIFO_QUEUE_t* queue = (FIFO_QUEUE_t*) fifoQueue;
  ELEMENT_t*    auxPtr;

  //Input validation
  if( (fifoQueue == PTIN_NULLPTR) || (element == PTIN_NULLPTR) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Error: Abnormal context [fifoQueue:%p element:%p]", fifoQueue, element);
    return FAILURE;
  }

  //Validation
  if(queue->first_free == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Exceeded maximum number of elements for which this queue was created");
    return TABLE_IS_EMPTY;
  }

  //Get element from the free queue
  auxPtr            = queue->first_free;
  queue->first_free = auxPtr->next;
  if(queue->first_free == PTIN_NULLPTR)
  {
    queue->last_free = PTIN_NULLPTR;
  }
  else
  {
    queue->first_free->prev = PTIN_NULLPTR;
  }

  //Restore data pointer
  auxPtr->data = element;

  //Add element to the used queue
  auxPtr->next = queue->first_used;
  auxPtr->prev = PTIN_NULLPTR;
  if(queue->first_used == PTIN_NULLPTR)
  {
    queue->last_used = auxPtr;
  }
  else
  {
    queue->first_used->prev = auxPtr;
  }
  queue->first_used = auxPtr;

  return SUCCESS;
}


/**
 * Get an element from the FIFO queue.
 * 
 * @param[in]  fifoQueue : FIFO queue 
 * @param[out] element   : Ptr to element to get from the queue
 * 
 * @return RC_t 
 */
RC_t ptin_fifo_pop(PTIN_FIFO_t fifoQueue, PTIN_FIFO_ELEMENT_t* element)
{
  FIFO_QUEUE_t* queue = (FIFO_QUEUE_t*) fifoQueue;
  ELEMENT_t*    auxPtr;

  //Input validation
  if( (fifoQueue == PTIN_NULLPTR) || (element == PTIN_NULLPTR) )
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Error: Abnormal context [fifoQueue:%p element:%p]", fifoQueue, element);
    return FAILURE;
  }

  //Validation
  if(queue->first_used == PTIN_NULLPTR)
  {
    PTIN_MGMD_LOG_ERR(PTIN_MGMD_LOG_CTX_PTIN_FIFO, "Error: No free elements left");
    return TABLE_IS_FULL;
  }

  *element = (PTIN_FIFO_ELEMENT_t*) queue->first_used->data;

  //Remove element from the used queue
  auxPtr            = queue->first_used;
  queue->first_used = auxPtr->next;
  if(queue->first_used == PTIN_NULLPTR)
  {
    queue->last_used = PTIN_NULLPTR;
  }
  else
  {
    queue->first_used->prev = PTIN_NULLPTR;
  }

  //Remove data pointer
  auxPtr->data = PTIN_NULLPTR;

  //Add element to the free queue
  auxPtr->next = queue->first_free;
  auxPtr->prev = PTIN_NULLPTR;
  if(queue->first_free == PTIN_NULLPTR)
  {
    queue->last_free = auxPtr;
  }
  else
  {
    queue->first_free->prev = auxPtr;
  }
  queue->first_free = auxPtr;

  return SUCCESS;
}

