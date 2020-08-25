#ifdef _L7_OS_LINUX_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_sem.c
*
* @purpose   OS independent API's
*
* @component osapi
*
* @comments
*
* @create    11/20/2001
*
* @author    John Linville
*
* @end
*
*********************************************************************/

/*************************************************************

*************************************************************/

#include <pthread.h>

#include "l7_common.h"
#include "osapi.h"

#include "osapi_priv.h"

/* still need delete safety and inversion safety */

typedef struct osapi_sem_s {
   L7_uint32 flags;
   pthread_mutex_t lock;
   L7_uint32 init_count, cur_count;
   osapi_task_t *owner; /* only for mutex */
   L7_int32 depth; /* only for mutex */
   osapi_waitq_t queue;
   osapi_task_t *last_owner;
   L7_uint32 num_waiting;
   struct osapi_sem_s *chain_prev, *chain_next;
} osapi_sem_t;

#define SEM_DELETED 0x00000001
#define SEM_FLUSHED 0x00000002
#define SEM_BINARY  0x00000004
#define SEM_COUNTING    0x00000008
#define SEM_MUTEX   0x00000010
#define SEM_Q_FIFO  0x00000020
#define SEM_Q_PRIO  0x00000040
#define SEM_DEL_SAFE    0x00000080
#define SEM_INV_SAFE    0x00000100

static pthread_mutex_t sem_list_lock = PTHREAD_MUTEX_INITIALIZER;
static osapi_sem_t *sem_list_head = NULL;

/**************************************************************************
*
* @purpose  Create a binary Semaphore
*
* @param    options @b{(input)}        queueing style - either Priority style or FIFO style
* @param    initialState @b{(input)}   FULL(available) or EMPTY (not
available)
*
* @returns  ptr to the semaphore or NULL if memory cannot be allocated
*
* @comments    none.
*
* @end
*
*************************************************************************/
void *osapiSemaBCreate(L7_int32 options, OSAPI_SEM_B_STATE initialState) {

   L7_uint32 q_options;
   osapi_sem_t *newSem;

   if ((newSem = (osapi_sem_t *)osapiMalloc(L7_OSAPI_COMPONENT_ID, sizeof(osapi_sem_t))) == NULL) {
      return (newSem);
   }

   pthread_mutex_init(&(newSem->lock), (pthread_mutexattr_t *)NULL);

   /* Do the options remapping */
   if ((options & OSAPI_SEM_Q_FIFO) != 0) {
      newSem->flags |= SEM_Q_FIFO;
      q_options = WAITQ_FIFO;
   } else /* if ((options & OSAPI_SEM_Q_PRIORITY) != 0) */ {
      newSem->flags |= SEM_Q_PRIO;
      q_options = WAITQ_PRIO;
   }
   if ((options & OSAPI_SEM_DELETE_SAFE) != 0) {
      /* invalid option */
      osapiFree(L7_OSAPI_COMPONENT_ID, newSem);
      return (NULL);
   }
   if ((options & OSAPI_SEM_INVERSION_SAFE) != 0) {
      /* invalid option */
      osapiFree(L7_OSAPI_COMPONENT_ID, newSem);
      return (NULL);
   }
   newSem->flags |= SEM_BINARY;

   newSem->init_count = (initialState == OSAPI_SEM_EMPTY) ? 0 : 1;
   newSem->cur_count = newSem->init_count;
   newSem->depth = 0;

   newSem->owner = (osapi_task_t *)NULL;
   newSem->last_owner = (osapi_task_t *)NULL;
   newSem->num_waiting = 0;

   osapi_waitq_create(&(newSem->queue), &(newSem->lock), q_options);

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&sem_list_lock);
   pthread_mutex_lock(&sem_list_lock);

   if (sem_list_head != NULL) {
      sem_list_head->chain_prev = newSem;
   }
   newSem->chain_next = sem_list_head;
   sem_list_head = newSem;
   newSem->chain_prev = NULL;

   pthread_cleanup_pop(1);

   return ((void *)newSem);

}

/**************************************************************************
*
* @purpose  create a Counting Semaphore
*
* @param    options @b{(input)}        queueing style - either Priority style or FIFO style
* @param    initialCount @b{(input)}   initialized to the specified initial count
*
* @returns  ptr to the semaphore or NULL if memory cannot be allocated
*
* @comments    none.
*
* @end
*
*************************************************************************/
void * osapiSemaCCreate (L7_int32 options, L7_int32 initialCount) {

   L7_uint32 q_options;
   osapi_sem_t *newSem;

   if ((newSem = osapiMalloc(L7_OSAPI_COMPONENT_ID, sizeof(osapi_sem_t))) == NULL) {
      return (newSem);
   }

   pthread_mutex_init(&(newSem->lock), (pthread_mutexattr_t *)NULL);

   /* Do the options remapping */
   if ((options & OSAPI_SEM_Q_FIFO) != 0) {
      newSem->flags |= SEM_Q_FIFO;
      q_options = WAITQ_FIFO;
   } else /* if ((options & OSAPI_SEM_Q_PRIORITY) != 0) */ {
      newSem->flags |= SEM_Q_PRIO;
      q_options = WAITQ_PRIO;
   }
   if ((options & OSAPI_SEM_DELETE_SAFE) != 0) {
      /* invalid option */
      osapiFree(L7_OSAPI_COMPONENT_ID, newSem);
      return (NULL);
   }
   if ((options & OSAPI_SEM_INVERSION_SAFE) != 0) {
      /* invalid option */
      osapiFree(L7_OSAPI_COMPONENT_ID, newSem);
      return (NULL);
   }

   newSem->flags |= SEM_COUNTING;

   newSem->init_count = initialCount;
   newSem->cur_count = newSem->init_count;
   newSem->depth = 0;

   newSem->owner = (osapi_task_t *)NULL;
   newSem->last_owner = (osapi_task_t *)NULL;
   newSem->num_waiting = 0;

   osapi_waitq_create(&(newSem->queue), &(newSem->lock), q_options);

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&sem_list_lock);
   pthread_mutex_lock(&sem_list_lock);

   if (sem_list_head != NULL) {
      sem_list_head->chain_prev = newSem;
   }
   newSem->chain_next = sem_list_head;
   sem_list_head = newSem;
   newSem->chain_prev = NULL;

   pthread_cleanup_pop(1);

   return ((void *)newSem);

}

/**************************************************************************
*
* @purpose  create a Mutual Exclusion Semaphore
*
* @param    options @b{(input)}        queueing style - either Priority style or FIFO style
*
* @returns  ptr to the semaphore or NULL if memory cannot be allocated
*
* @comments    none.
*
* @end
*
*************************************************************************/
void * osapiSemaMCreate (L7_int32 options) {

   L7_uint32 q_options;
   osapi_sem_t *newSem;

   if ((newSem = osapiMalloc(L7_OSAPI_COMPONENT_ID, sizeof(osapi_sem_t))) == NULL) {
      return (newSem);
   }

   pthread_mutex_init(&(newSem->lock), (pthread_mutexattr_t *)NULL);

   /* Do the options remapping */
   if ((options & OSAPI_SEM_Q_FIFO) != 0) {
      newSem->flags |= SEM_Q_FIFO;
      q_options = WAITQ_FIFO;
   } else /* if ((options & OSAPI_SEM_Q_PRIORITY) != 0) */ {
      newSem->flags |= SEM_Q_PRIO;
      q_options = WAITQ_PRIO;
   }
   if ((options & OSAPI_SEM_DELETE_SAFE) != 0) {
      newSem->flags |= SEM_DEL_SAFE;
   }
   if ((options & OSAPI_SEM_INVERSION_SAFE) != 0) {
      newSem->flags |= SEM_INV_SAFE;
   }

   newSem->flags |= SEM_MUTEX;

   newSem->init_count = 1;
   newSem->cur_count = newSem->init_count;
   newSem->depth = 0;

   newSem->owner = (osapi_task_t *)NULL;
   newSem->last_owner = (osapi_task_t *)NULL;
   newSem->num_waiting = 0;

   osapi_waitq_create(&(newSem->queue), &(newSem->lock), q_options);

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&sem_list_lock);
   pthread_mutex_lock(&sem_list_lock);

   if (sem_list_head != NULL) {
      sem_list_head->chain_prev = newSem;
   }
   newSem->chain_next = sem_list_head;
   sem_list_head = newSem;
   newSem->chain_prev = NULL;

   pthread_cleanup_pop(1);

   return ((void *)newSem);

}

/**************************************************************************
*
* @purpose  Delete a Semaphore
*
* @param    SemID @b{(input)}   ID of the semaphore to delete
*
* @returns  OK, or error semaphore is invalid
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiSemaDelete (void * sem) {

   osapi_sem_t *osapiSem = (osapi_sem_t *)sem;

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiSem->lock));
   pthread_mutex_lock(&(osapiSem->lock));

   /* need to account for blocked tasks */
   /*  -- mark semaphore as deleted     */
   /*  -- mark semaphore as flushed     */
   /*  -- flush wait queue              */
   /*  -- delete wait queue             */

   osapiSem->flags |= (SEM_DELETED | SEM_FLUSHED);

   osapi_waitq_flush(&(osapiSem->queue));

   osapi_waitq_destroy(&(osapiSem->queue));

   pthread_cleanup_pop(1);

   pthread_mutex_destroy(&(osapiSem->lock));

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&sem_list_lock);
   pthread_mutex_lock(&sem_list_lock);

   if (osapiSem->chain_next != NULL) {

      osapiSem->chain_next->chain_prev = osapiSem->chain_prev;

   }

   if (osapiSem->chain_prev != NULL) {

      osapiSem->chain_prev->chain_next = osapiSem->chain_next;

   } else { /* must be head of list */

      sem_list_head = osapiSem->chain_next;

   }

   pthread_cleanup_pop(1);

   osapiFree(L7_OSAPI_COMPONENT_ID, osapiSem);

   return(L7_SUCCESS);

}

/**************************************************************************
*
* @purpose  Unblock any and all tasks waiting on the semaphore
*
* @param    SemID @b{(input)}   ID of the semaphore
*
* @returns  OK, or error semaphore is invalid
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiSemaFlush (void * sem) {

   osapi_sem_t *osapiSem = (osapi_sem_t *)sem;

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiSem->lock));
   pthread_mutex_lock(&(osapiSem->lock));

   /* need to account for blocked tasks */
   /*  -- mark semaphore as flushed     */
   /*  -- flush wait queue              */
   /*  -- unmark semaphore as flushed   */

   osapiSem->flags |= SEM_FLUSHED;

   osapi_waitq_flush(&(osapiSem->queue));

   osapiSem->num_waiting = 0;
   osapiSem->flags &= ~SEM_FLUSHED;

   pthread_cleanup_pop(1);

   return(L7_SUCCESS);

}

/**************************************************************************
*
* @purpose  Give a Semaphore
*
* @param    SemID @b{(input)}   ID of the semaphore to release
*
* @returns  OK, or error semaphore is invalid
*
* @comments    none.
*
* @end
*
*************************************************************************/

L7_RC_t osapiSemaGive (void * sem) {

   L7_RC_t rc = L7_SUCCESS;
   osapi_sem_t *osapiSem = (osapi_sem_t *)sem;

   if (osapiSem==L7_NULL) {
       return L7_FAILURE;
   }

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiSem->lock));
   pthread_mutex_lock(&(osapiSem->lock));

   if ((osapiSem->flags & (SEM_DELETED | SEM_FLUSHED)) == 0) {

      if (osapiSem->depth == 0) {

         if ((((osapiSem->flags & SEM_COUNTING) != 0)
           || (osapiSem->cur_count != 1))
          && (((osapiSem->flags & SEM_MUTEX) == 0)
           || (osapiSem->owner == pthread_getspecific(osapi_task_key)))) {

            if (++osapiSem->cur_count == 1) {

	       osapiSem->last_owner = pthread_getspecific(osapi_task_key);
               osapi_waitq_dequeue(&(osapiSem->queue));

            }

            if ((osapiSem->flags & SEM_MUTEX) != 0) {

               osapiSem->owner = NULL;

               if ((osapiSem->flags & SEM_DEL_SAFE) != 0) {

                  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

               }
            }

         } else {

            rc = L7_FAILURE;

         }

      } else {

         if (((osapiSem->flags & SEM_MUTEX) == 0)
          || (osapiSem->owner != pthread_getspecific(osapi_task_key))) {

            rc = L7_FAILURE;

         } else {

            osapiSem->depth--;

         }

      }

   } else {

      rc = L7_FAILURE;

   }

   pthread_cleanup_pop(1);

   return (rc);

}

static int osapi_sem_waitq_remove_check(void *sem) {

   osapi_sem_t *osapiSem = (osapi_sem_t *)sem;

   if (osapiSem->cur_count > 0) {
     if ((osapiSem->num_waiting < 2) || 
	 (osapiSem->last_owner != pthread_getspecific(osapi_task_key))) {
       return (WAITQ_REMOVE_OK);
     }
   }

   if ((osapiSem->flags & SEM_FLUSHED) != 0)
      return (WAITQ_REMOVE_OK);

   return (~WAITQ_REMOVE_OK);

}

/**************************************************************************
*
* @purpose  Take a Semaphore
*
* @param    SemID @b{(input)}   ID of the requested semaphore
* @param    timeout @b{(input)}  time to wait in milliseconds, forever (-1), or no wait (0)
*
* @returns  OK, or error if timeout or if semaphore does not exist
*
* @comments    none.
*
* @end
*
*************************************************************************/
L7_RC_t osapiSemaTake (void *sem, L7_int32 timeout) {

   L7_RC_t rc = L7_SUCCESS;
   osapi_sem_t *osapiSem = (osapi_sem_t *)sem;

   if (osapiSem==L7_NULL) {
       return L7_FAILURE;
   }

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiSem->lock));
   pthread_mutex_lock(&(osapiSem->lock));

   if ((osapiSem->flags & (SEM_DELETED | SEM_FLUSHED)) == 0) {

      if (((osapiSem->flags & SEM_MUTEX) == 0)
       || (osapiSem->owner != pthread_getspecific(osapi_task_key))) {

         while ((osapiSem->cur_count==0) || 
		((osapiSem->last_owner == pthread_getspecific(osapi_task_key)) && 
		 (osapiSem->num_waiting != 0)))
	 {

	    osapiSem->num_waiting++;
            rc = osapi_waitq_enqueue(&(osapiSem->queue), timeout,
                                     osapi_sem_waitq_remove_check,
                                     (void *)osapiSem);
	    osapiSem->num_waiting--;

            if ((rc != L7_SUCCESS) || ((osapiSem->flags & SEM_FLUSHED) != 0)) {

               rc = L7_FAILURE;
               break;

            }

         }

         if ((rc == L7_SUCCESS) && 
	     ((osapiSem->num_waiting == 0) || 
	      (osapiSem->last_owner != pthread_getspecific(osapi_task_key))))
	   {

            osapiSem->cur_count--;
	    osapiSem->last_owner = NULL;

            if ((osapiSem->flags & SEM_MUTEX) != 0) {

               osapiSem->owner = pthread_getspecific(osapi_task_key);

               if ((osapiSem->flags & SEM_DEL_SAFE) != 0) {

                  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

               }

            }

         }

      } else {

         osapiSem->depth++;

      }

   } else {

      rc = L7_FAILURE;

   }

   pthread_cleanup_pop(1);

   return (rc);

}
#endif /* _L7_OS_LINUX_ */
