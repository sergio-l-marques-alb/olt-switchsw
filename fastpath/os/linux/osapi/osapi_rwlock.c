/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_rwlock.c
*
* @purpose   OS independent API's
*
* @component osapi
*
* @comments
*
* @create    08/08/2003
*
* @author    John W. Linville
*
* @end
*
*********************************************************************/

#include <pthread.h>

#include "l7_common.h"
#include "osapi.h"

#include "osapi_priv.h"

typedef struct osapi_rwlock_s {
   volatile L7_uint32 flags;
   pthread_mutex_t lock;
   volatile L7_uint32 rcount;
   volatile L7_uint32 wcount;
   osapi_waitq_t rqueue;
   osapi_waitq_t wqueue;
   struct osapi_rwlock_s *chain_prev, *chain_next;
} osapi_rwlock_t;

#define RWLOCK_DELETED     0x00000001
#define RWLOCK_W_PENDING   0x00000002
#define RWLOCK_Q_FIFO      0x00000004
#define RWLOCK_Q_PRIO      0x00000008

static pthread_mutex_t rwlock_list_lock = PTHREAD_MUTEX_INITIALIZER;
static osapi_rwlock_t *rwlock_list_head = NULL;

/**************************************************************************
*
* @purpose  Create a read/write lock
*
* @param    options @b{(input)} queueing style - either Priority or FIFO 
*
* @returns  ptr to the lock or NULL if lock cannot be allocated
*
* @comments None.
*
* @end
*
*************************************************************************/
L7_RC_t osapiRWLockCreate(osapiRWLock_t *rwlock, osapiRWLockOptions_t options)
{
   L7_uint32 q_options;
   osapi_rwlock_t *newRWLock;

   if ((newRWLock = (osapi_rwlock_t *)osapiMalloc(L7_OSAPI_COMPONENT_ID, sizeof(osapi_rwlock_t)))
    == NULL) {
      rwlock->handle = (void *)newRWLock;
      return (L7_FAILURE);
   }

   pthread_mutex_init(&(newRWLock->lock), (pthread_mutexattr_t *)NULL);

   /* Do the options remapping */
   if (options == OSAPI_RWLOCK_Q_FIFO) {
      newRWLock->flags |= RWLOCK_Q_FIFO;
      q_options = WAITQ_FIFO;
   } else /* if (options == OSAPI_RWLOCK_Q_PRIORITY) */ {
      newRWLock->flags |= RWLOCK_Q_PRIO;
      q_options = WAITQ_PRIO;
   }

   newRWLock->rcount = 0;
   newRWLock->wcount = 0;
   osapi_waitq_create(&(newRWLock->rqueue), &(newRWLock->lock), q_options);
   osapi_waitq_create(&(newRWLock->wqueue), &(newRWLock->lock), q_options);

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&rwlock_list_lock);
   pthread_mutex_lock(&rwlock_list_lock);

   if (rwlock_list_head != NULL) {
      rwlock_list_head->chain_prev = newRWLock;
   }
   newRWLock->chain_next = rwlock_list_head;
   rwlock_list_head = newRWLock;
   newRWLock->chain_prev = NULL;

   pthread_cleanup_pop(1);

   rwlock->handle = (void *)newRWLock;
   return (L7_SUCCESS);
}

static int osapi_rwlock_r_waitq_remove_check(void *rwlock)
{
   osapi_rwlock_t *osapiRWLock = (osapi_rwlock_t *)rwlock;


   if ((osapiRWLock->wcount == 0)
    && ((osapiRWLock->flags & RWLOCK_W_PENDING) == 0))
      return (WAITQ_REMOVE_OK);

   if ((osapiRWLock->flags & RWLOCK_DELETED) != 0)
      return (WAITQ_REMOVE_OK);

   return (~WAITQ_REMOVE_OK);
}

/**************************************************************************
*
* @purpose  Take a read lock
*
* @param    rwlock    ID of the requested lock returned from osapiRWLockCreate
* @param    timeout   time to wait in milliseconds, L7_WAIT_FOREVER,
*                     or L7_NO_WAIT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if timeout or if lock does not exist
*
* @comments None.
*
* @end
*
*************************************************************************/
L7_RC_t osapiReadLockTake(osapiRWLock_t rwlock, L7_int32 timeout)
{
   L7_RC_t rc = L7_SUCCESS;
   osapi_rwlock_t *osapiRWLock = (osapi_rwlock_t *)rwlock.handle;

   /* Timeout is already in milliseconds, which is what 
      osapi_waitq_enqueue() needs */

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiRWLock->lock));
   pthread_mutex_lock(&(osapiRWLock->lock));

   if ((osapiRWLock->flags & RWLOCK_DELETED) == 0) {

      /* wait for writes to complete */
      while ((osapiRWLock->wcount > 0) 
          || ((osapiRWLock->flags & RWLOCK_W_PENDING) != 0)) {
         rc = osapi_waitq_enqueue(&(osapiRWLock->rqueue), timeout,
                                  osapi_rwlock_r_waitq_remove_check,
                                  (void *)osapiRWLock);

         if ((rc != L7_SUCCESS)
          || ((osapiRWLock->flags & RWLOCK_DELETED) != 0)) {

            rc = L7_FAILURE;
            break;

         }

      }

      if (rc == L7_SUCCESS) {

         osapiRWLock->rcount++;

      }

   } else {

      rc = L7_FAILURE;

   }

   pthread_cleanup_pop(1);

   return (rc);
}

/**************************************************************************
*
* @purpose  Give a read lock
*
* @param    rwlock     ID of the requested lock returned from osapiRWLockCreate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if lock is invalid
*
* @comments None.
*
* @end
*
*************************************************************************/
L7_RC_t osapiReadLockGive(osapiRWLock_t rwlock)
{
   L7_RC_t rc = L7_SUCCESS;
   osapi_rwlock_t *osapiRWLock = (osapi_rwlock_t *)rwlock.handle;

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiRWLock->lock));
   pthread_mutex_lock(&(osapiRWLock->lock));

   if (osapiRWLock->rcount == 0) {
     rc = L7_ERROR;
   } else {
     if (--osapiRWLock->rcount == 0) {
       osapi_waitq_dequeue(&(osapiRWLock->wqueue));
     }
   }
   pthread_cleanup_pop(1);

   return (rc);
}

static int osapi_rwlock_w_waitq_remove_check(void *rwlock)
{
   osapi_rwlock_t *osapiRWLock = (osapi_rwlock_t *)rwlock;

   if ((osapiRWLock->rcount == 0) && (osapiRWLock->wcount == 0))
      return (WAITQ_REMOVE_OK);

   if ((osapiRWLock->flags & RWLOCK_DELETED) != 0)
      return (WAITQ_REMOVE_OK);

   return (~WAITQ_REMOVE_OK);
}

/**************************************************************************
*
* @purpose  Take a write lock
*
* @param    rwlock    ID of the requested lock returned from osapiRWLockCreate
* @param    timeout   time to wait in milliseconds, L7_WAIT_FOREVER,
*                     or L7_NO_WAIT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if timeout or if lock does not exist
*
* @comments None.
*
* @end
*
*************************************************************************/
L7_RC_t osapiWriteLockTake(osapiRWLock_t rwlock, L7_int32 timeout)
{
   L7_RC_t rc = L7_SUCCESS;
   osapi_rwlock_t *osapiRWLock = (osapi_rwlock_t *)rwlock.handle;

   /* Timeout is already in milliseconds, which is what 
    osapi_waitq_enqueue() needs */

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiRWLock->lock));
   pthread_mutex_lock(&(osapiRWLock->lock));

   if ((osapiRWLock->flags & RWLOCK_DELETED) == 0) {

      /* indicate pending write */
      osapiRWLock->flags |= RWLOCK_W_PENDING;

      /* wait for reads to complete */
      while ((osapiRWLock->rcount > 0) || (osapiRWLock->wcount > 0)) {
         rc = osapi_waitq_enqueue(&(osapiRWLock->wqueue), timeout,
                                  osapi_rwlock_w_waitq_remove_check,
                                  (void *)osapiRWLock);

         if ((rc != L7_SUCCESS)
          || ((osapiRWLock->flags & RWLOCK_DELETED) != 0)) {

            osapiRWLock->flags &= ~(RWLOCK_W_PENDING);
            rc = L7_FAILURE;
            break;

         }

      }

      if (rc == L7_SUCCESS) {

         osapiRWLock->wcount++; /* result should always equal 1 */
         osapiRWLock->flags &= ~(RWLOCK_W_PENDING);

      }

   } else {

      rc = L7_FAILURE;

   }

   pthread_cleanup_pop(1);

   return (rc);
}

/**************************************************************************
*
* @purpose  Give a write lock
*
* @param    rwlock     ID of the requested lock returned from osapiRWLockCreate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if lock is invalid
*
* @comments None.
*
* @end
*
*************************************************************************/
L7_RC_t osapiWriteLockGive(osapiRWLock_t rwlock)
{
   L7_RC_t rc = L7_SUCCESS;
   osapi_rwlock_t *osapiRWLock = (osapi_rwlock_t *)rwlock.handle;

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiRWLock->lock));
   pthread_mutex_lock(&(osapiRWLock->lock));

   if (osapiRWLock->wcount == 0) {
     rc = L7_ERROR;
   } else {
     osapiRWLock->wcount--; /* result should always equal 0 */
     osapi_waitq_dequeue(&(osapiRWLock->wqueue));
     /* Wake up _all_ waiting readers */
     osapi_waitq_dequeue_all(&(osapiRWLock->rqueue));
   }
   pthread_cleanup_pop(1);
   return (rc);
}

/**************************************************************************
*
* @purpose  Delete a read/write lock
*
* @param    rwlock     ID of the lock to delete
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if lock is invalid
*
* @comments Routine will suspend until there are no outstanding reads or writes.
*
* @end
*
*************************************************************************/
L7_RC_t osapiRWLockDelete(osapiRWLock_t rwlock)
{
   L7_RC_t rc = L7_SUCCESS;
   osapi_rwlock_t *osapiRWLock = (osapi_rwlock_t *)rwlock.handle;

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(osapiRWLock->lock));
   pthread_mutex_lock(&(osapiRWLock->lock));

   /* mark rwlock as deleted and wait for current readers and writers */

   osapiRWLock->flags |= RWLOCK_DELETED;

   while ((osapiRWLock->rcount > 0) || (osapiRWLock->wcount > 0)) {

      /* wait for reads and writes to complete */
      rc = osapi_waitq_enqueue(&(osapiRWLock->wqueue), L7_WAIT_FOREVER,
                               osapi_rwlock_w_waitq_remove_check,
                               (void *)osapiRWLock);

      if (rc != L7_SUCCESS) {

         /* This SHOULD NOT happen... */
         break;

      }

   }

   osapi_waitq_destroy(&(osapiRWLock->rqueue));

   osapi_waitq_destroy(&(osapiRWLock->wqueue));

   pthread_cleanup_pop(1);

   pthread_mutex_destroy(&(osapiRWLock->lock));

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&rwlock_list_lock);
   pthread_mutex_lock(&rwlock_list_lock);

   if (osapiRWLock->chain_next != NULL) {

      osapiRWLock->chain_next->chain_prev = osapiRWLock->chain_prev;

   }

   if (osapiRWLock->chain_prev != NULL) {

      osapiRWLock->chain_prev->chain_next = osapiRWLock->chain_next;

   } else { /* must be head of list */

      rwlock_list_head = osapiRWLock->chain_next;

   }

   pthread_cleanup_pop(1);

   osapiFree(L7_OSAPI_COMPONENT_ID, osapiRWLock);

   return(L7_SUCCESS);
}

