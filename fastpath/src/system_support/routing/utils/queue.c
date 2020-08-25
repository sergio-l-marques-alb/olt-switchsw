/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename       queue.c
 *
 * @purpose        Queue management implementation
 *
 * @component      Routing Utils Component
 *
 * @comments
 *
 * @create         07/09/1998
 *
 * @author         Igor Bryskin
 *
 * @end                                                                        
 *
 * ********************************************************************/        
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\queue.c 1.1.3.1 2002/02/13 23:06:43EST ssaraswatula Development  $";
#endif


#include <string.h>

#include "std.h"
#include "xx.ext"
#include "queue.ext"

/* QUEUE MANAGEMENT FUNCTIONS */

typedef struct tagt_QueueObject
{
   struct tagt_QueueObject *next;
   struct tagt_QueueObject *prev;
#ifdef ERRCHK
   XX_Status status;
#endif
   void *head;
   void *tail;
   word queuesize;
   word maxsize;
}t_QueueObject;


t_QueueObject *QueueObjects;

/*Initiate queue management */
void Q_InitAll(void)
{
   QueueObjects = NULLP;
}

/*Shut down queue management */
void Q_ShutDown(void)
{
   t_QueueObject *queue;
   while((queue = QueueObjects) != NULLP)
   {
      XX_Freeze();
      QueueObjects = queue->next;
      Q_Destruct((t_Handle *)&queue);
      XX_Thaw();
   }
}

/* Create Queue object */
e_Err Q_Construct(  word queueSize, t_Handle *p_queueHandle )
{
   t_QueueObject *queue = (t_QueueObject *)XX_Malloc(sizeof(t_QueueObject));
   
   if(!queue)
      return E_RES_UNAVAIL;

   memset(queue, 0,  sizeof(t_QueueObject));
   queue->maxsize = queueSize;
#ifdef ERRCHK
   queue->status = XX_STAT_Valid;
#endif
   
   /* Attach new object to Queue objects list */
   XX_AddToDLList(queue, QueueObjects);
   *p_queueHandle = (t_Handle)queue;
   return E_OK;
}


/* Delete Queue object */
e_Err Q_Destruct ( t_Handle *p_queueHandle)
{
   t_QueueObject *queue = (t_QueueObject *)*p_queueHandle;
   ASSERT(queue->status == XX_STAT_Valid);
   
   while (queue->head)
   {
      void *ent;
      XX_Freeze();
      ent = queue->head;
      queue->head =*(void **)(queue->head);
      XX_Free(ent);
      XX_Thaw();
   }
   
   XX_DelFromDLList(queue, QueueObjects);
   
#ifdef ERRCHK
   queue->status = XX_STAT_Invalid;
#endif
   XX_Free(queue);
   *p_queueHandle = NULLP;
   return E_OK;
}


/* Add data to the queue head */
e_Err Q_AddHead( t_Handle queueHandle, void * entry)
{
   t_QueueObject *queue = (t_QueueObject *)queueHandle;
   ASSERT(queue->status == XX_STAT_Valid);
   if(queue->maxsize && queue->queuesize>= queue->maxsize)
      return E_RES_UNAVAIL;
   
   XX_Freeze();
   queue->queuesize++;
   if(queue->head)
    *(void **)entry = queue->head;
   else
   { 
    *(void **)entry = NULLP;
     queue->tail = entry;
   }

   queue->head = entry;
   XX_Thaw();
   return E_OK;
}


/* Add data to the queue tail */
e_Err Q_AddTail(t_Handle queueHandle, void * entry)
{
   t_QueueObject *queue = (t_QueueObject *)queueHandle;
   ASSERT(queue->status == XX_STAT_Valid);
   if(queue->maxsize && queue->queuesize>= queue->maxsize)
      return E_RES_UNAVAIL;
   
   *(void **)entry = NULLP;
   XX_Freeze();
   queue->queuesize++;
   if(queue->tail)
      *(void **)queue->tail = entry;
   else
      queue->head = entry;
   queue->tail = entry;
   XX_Thaw();
   return E_OK;
}


/* Get data from head but don't remove */
e_Err Q_GetHead( t_Handle queueHandle, void **entry)
{
   t_QueueObject *queue = (t_QueueObject *)queueHandle;
   ASSERT(entry);
   *entry = NULLP;
   ASSERT(queue->status == XX_STAT_Valid);
   if(!queue->head)
      return E_INVALID_PAR;
   if(entry)
      *entry = queue->head;
   return E_OK;
}


/* Get and remove data from the queue */
e_Err Q_GetHeadRemove( t_Handle queueHandle, void **entry)
{
   t_QueueObject *queue = (t_QueueObject *)queueHandle;
   ASSERT(queue->status == XX_STAT_Valid);
   if(!queue->head)
      return E_INVALID_PAR;
   if(entry)
      *entry = queue->head;
   XX_Freeze();
   queue->queuesize--;
   if((queue->head = *(void **)queue->head)==NULLP)
      queue->tail = NULLP;
   XX_Thaw();
   return E_OK;
}


/* Get number of items in the queue */
word Q_GetEntNumber( t_Handle queueHandle)
{
   t_QueueObject *queue = (t_QueueObject *)queueHandle;
   ASSERT(queue->status == XX_STAT_Valid);
   return queue->queuesize;
}

/* Get queue size */
word Q_GetSize( t_Handle queueHandle)
{
   t_QueueObject *queue = (t_QueueObject *)queueHandle;
   ASSERT(queue->status == XX_STAT_Valid);
   return queue->maxsize;
}

