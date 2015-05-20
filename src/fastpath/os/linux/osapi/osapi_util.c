#ifdef _L7_OS_LINUX_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi.c
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

#include <sys/time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <l7_common.h>
#include "osapi_priv.h"

/* PTin added: Debug */
#include "logger.h"

static pthread_mutex_t queue_list_mutex = PTHREAD_MUTEX_INITIALIZER;
static osapi_waitq_t *queue_list_head = NULL;

void osapi_waitq_create(osapi_waitq_t *queue, pthread_mutex_t *lock,
                        L7_uint32 flags) {

   queue->flags = flags;

   queue->lock = lock;

   queue->count = 0;

   pthread_cond_init(&(queue->control), NULL);

   if (WAITQ_POLICY(queue) == WAITQ_FIFO) {

      queue->policy.fifo.head = queue->policy.fifo.tail = NULL;

   } else { /* priority queue */

      pthread_cond_init(&(queue->policy.prio.cond), NULL);

   }

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&queue_list_mutex);
   if (pthread_mutex_lock(&queue_list_mutex) != 0)
   {
       osapi_printf("osapi_waitq_create: queue_list_mutex error\n");
   }

   if (queue_list_head != NULL) {

      queue_list_head->chain_prev = queue;

   }

   queue->chain_next = queue_list_head;

   queue_list_head = queue;

   queue->chain_prev = NULL;

   pthread_cleanup_pop(1);

   return;
}

void osapi_waitq_destroy(osapi_waitq_t *queue) {

   /* assume flush already done */

   /* assume queue is already locked */

   pthread_cond_destroy(&(queue->control));

   if (WAITQ_POLICY(queue) == WAITQ_PRIO) {

      pthread_cond_destroy(&(queue->policy.prio.cond));
      queue->policy.fifo.head = NULL;

   }

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&queue_list_mutex);
   if (pthread_mutex_lock(&queue_list_mutex) != 0)
   {
       osapi_printf("osapi_waitq_destroy: queue_list_mutex error\n");
   }

   if (queue->chain_next != NULL) {

      queue->chain_next->chain_prev = queue->chain_prev;

   }

   if (queue->chain_prev != NULL) {

      queue->chain_prev->chain_next = queue->chain_next;

   } else { /* must be head of list */

      queue_list_head = queue->chain_next;

   }

   pthread_cleanup_pop(1);

   return;

}

L7_RC_t osapi_waitq_enqueue(osapi_waitq_t *queue,
                            L7_uint32 wait_msec,
                            removal_check_t removal_check,
                            void *removal_check_data) {

   struct timeval now;
   struct timespec timeout;
   L7_RC_t rc = L7_SUCCESS;
   osapi_task_t *self;
   int task_deleted = 0;
   int count = 0;
   int wait_rc;

   /* assume queue is already locked */

   if (wait_msec != L7_WAIT_FOREVER) {

      gettimeofday(&now, (struct timezone *)NULL);

      if (wait_msec == L7_NO_WAIT) {

         timeout.tv_sec = now.tv_sec;
         timeout.tv_nsec = (now.tv_usec * 1000);

      } else {

         timeout.tv_sec = now.tv_sec + (wait_msec / 1000);
         timeout.tv_nsec = ((now.tv_usec + ((wait_msec % 1000) * 1000)) * 1000);

      }
      /* Yes, this seems to matter to WRL 2.0's glibc */
      while (timeout.tv_nsec >= 1000000000) {
	timeout.tv_sec += 1;
	timeout.tv_nsec -= 1000000000;
      }
   }

   self = pthread_getspecific(osapi_task_key);

   if (self == NULL) {
#if 0//Added by MMelo
       osapi_printf("%s: invalid osapi_task_key\n", __FUNCTION__);
#endif
       return L7_ERROR;
   }

   pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                        (void *)&(self->lock));
   if (pthread_mutex_lock(&(self->lock)) != 0)
   {
       osapi_printf("osapi_waitq_enqueue: self->lock error\n");
   }

   task_deleted = (self->flags & TASK_DELETED);

   /*
   Check if the current task has been deleted, if not, we can add
   it to the waitq.
   */

   if (!task_deleted)
   {
       queue->count++;

       self->waiting = queue;
   }
   else
   {
       /*
       If the current task has been deleted, return an error to
       the calling task.  The sleep is to allow a lower-priority
       deleting task to be able to get the queue semaphore when
       the deleted task is running at high priority
       */
       pthread_mutex_unlock(&(self->lock));
       /*
       osapiSleepMSec(100);
       */
       return L7_ERROR;
   }

   pthread_cleanup_pop(1);

   if (WAITQ_POLICY(queue) == WAITQ_PRIO) {

      do {

         if (wait_msec == L7_WAIT_FOREVER) {

            pthread_cond_wait(&(queue->policy.prio.cond), queue->lock);

         } else {

            wait_rc = pthread_cond_timedwait(&(queue->policy.prio.cond),
					     queue->lock, &timeout);
	    if (wait_rc != 0) {
               rc = L7_ERROR;
               LOG_PT_CRITIC(LOG_CTX_MISC,"rc=%d, wait_rc=%d",rc,wait_rc);     /* PTin added: Debug */
               break;
            }

         }

         pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                              (void *)&(self->lock));
         if (pthread_mutex_lock(&(self->lock)) != 0)
         {
             osapi_printf("osapi_waitq_enqueuep: self->lock error\n");
         }

         task_deleted = (self->flags & TASK_DELETED);

         /*
         If the task has now been deleted, we want to exit from the
         main waitq waiting loop
         */

         pthread_cleanup_pop(1);

      } while ((removal_check(removal_check_data) != WAITQ_REMOVE_OK)
               && (task_deleted == 0));

      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                          (void *)&(self->lock));
      if (pthread_mutex_lock(&(self->lock)) != 0)
      {
          osapi_printf("osapi_waitq_enqueuep: self->lock error 2\n");
      }

      task_deleted = (self->flags & TASK_DELETED);

      /*
      If task has not been deleted, lower the count to reflect
      removal from the queue; if the task has been deleted, then
      the deleting task will take care of reducing the count
      */

      if (task_deleted == 0)
      {
          count = --queue->count;
      }
      else
      {
          count = queue->count;
      }

      if ((count == 0) && ((queue->flags & WAITQ_FLUSHED) != 0)) {

         pthread_cond_signal(&(queue->control));

      }

      self->waiting = NULL;

      pthread_cleanup_pop(1);

   } else { /* FIFO queue */

      if (queue->policy.fifo.head == NULL) { /* empty list */

         queue->policy.fifo.head = queue->policy.fifo.tail = self;

      } else {

         self->fifo_prev = queue->policy.fifo.tail;

         queue->policy.fifo.tail->fifo_next = self;

         queue->policy.fifo.tail = self;

      }

      do {

          if (wait_msec == L7_WAIT_FOREVER) {

             pthread_cond_wait(&(self->fifo_cond), queue->lock);

          } else {

	     wait_rc = pthread_cond_timedwait(&(self->fifo_cond), queue->lock,
					      &timeout); 
	     if (wait_rc != 0) {
                rc = L7_ERROR;
                //LOG_PT_CRITIC(LOG_CTX_MISC,"rc=%d, wait_rc=%d",rc,wait_rc);      /* PTin added: Debug */
                break;
             }

          }

          pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                               (void *)&(self->lock));
          if (pthread_mutex_lock(&(self->lock)) != 0)
          {
              osapi_printf("osapi_waitq_enqueuef: self->lock error\n");
          }

          task_deleted = (self->flags & TASK_DELETED);

          /*
          If the task has now been deleted, we want to exit from the
          main waitq waiting loop
          */

          pthread_cleanup_pop(1);

      } while ((removal_check(removal_check_data) != WAITQ_REMOVE_OK)
            && (task_deleted == 0));

      pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                           (void *)&(self->lock));
      if (pthread_mutex_lock(&(self->lock)) != 0)
      {
          osapi_printf("osapi_waitq_enqueuef: self->lock error 2\n");
      }

      if (self->fifo_prev == NULL) {

         queue->policy.fifo.head = self->fifo_next;

      } else {

         self->fifo_prev->fifo_next = self->fifo_next;

      }

      if (self->fifo_next == NULL) {

         queue->policy.fifo.tail = self->fifo_prev;

      } else {

         self->fifo_next->fifo_prev = self->fifo_prev;

      }

      self->fifo_prev = self->fifo_next = NULL;

      task_deleted = (self->flags & TASK_DELETED);

      /*
      If task has not been deleted, lower the count to reflect
      removal from the queue; if the task has been deleted, then
      the deleting task will take care of reducing the count
      */

      if (task_deleted == 0)
      {
          queue->count--;
      }

      if ((queue->flags & WAITQ_FLUSHED) != 0) {

         pthread_cond_signal(&(queue->control));

      }

      self->waiting = NULL;

      pthread_cleanup_pop(1);
   }

   return (rc);

}

void osapi_waitq_dequeue(osapi_waitq_t *queue) {

   /* assume queue is already locked */

   if (queue->count != 0) {

      if (WAITQ_POLICY(queue) == WAITQ_PRIO) {

         pthread_cond_signal(&(queue->policy.prio.cond));

      } else { /* FIFO queue */

         pthread_cond_signal(&(queue->policy.fifo.head->fifo_cond));

      }

   }

   return;

}

/* Not quite the same as osapi_waitq_flush(). This does not wait for all the 
   wakeups to finish, osapi_waitq_flush() does. */
void osapi_waitq_dequeue_all(osapi_waitq_t *queue) {
  osapi_task_t *cur;

  /* assume queue is already locked */
  if (queue->count != 0) {
    if (WAITQ_POLICY(queue) == WAITQ_PRIO) {
      pthread_cond_broadcast(&(queue->policy.prio.cond));
    } else { /* FIFO queue */
      cur = queue->policy.fifo.head;
      while (cur) {
          pthread_cond_signal(&(cur->fifo_cond));
          cur = cur->fifo_next;
      }
    }    
  }
}

void osapi_waitq_flush(osapi_waitq_t *queue) {

   /* assume queue is already locked */

   if (queue->count != 0) {

      queue->flags |= WAITQ_FLUSHED;

      if (WAITQ_POLICY(queue) == WAITQ_PRIO) {

          pthread_cond_broadcast(&(queue->policy.prio.cond));

         while (queue->count != 0) {

            pthread_cond_wait(&(queue->control), queue->lock);

         }

      } else { /* FIFO queue */

         while (queue->count != 0) {

            pthread_cond_signal(&(queue->policy.fifo.head->fifo_cond));

            pthread_cond_wait(&(queue->control), queue->lock);

         }

      }

      queue->flags &= ~WAITQ_FLUSHED;

   }

   return;

}

void osapi_waitq_remove(osapi_waitq_t *queue, osapi_task_t *task) {

   /* assume queue is already locked */

   /* assume task is already locked */

   if (WAITQ_POLICY(queue) == WAITQ_FIFO) {

      if (task->waiting == queue) {

         pthread_cond_signal(&(task->fifo_cond));

      }

   }

   /* nothing to do for WAITQ_PRIO... tasks are always FIFO */

   return;

}

/* Rather than use system("echo foo > /proc/bar"), this way avoids fork/exec and 
   "system"'s security problems. Returns -1 on error, 0 on success. */
int osapi_proc_set(const char *path, const char *value) {
    int rc, fd;
    struct stat stat_info;

    /* Stat first, as some kernel bugs will hang the system if you open() a /proc 
       file that isn't there. */
    rc = stat(path, &stat_info);
    if (rc < 0) {
        return(-1);
    }
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        return(-1);
    }
    rc = write(fd, value, strlen(value));
    close(fd);
    if (rc < 0) {
        return(-1);
    }
    return(0);
}

#endif /* _L7_OS_LINUX_ */
