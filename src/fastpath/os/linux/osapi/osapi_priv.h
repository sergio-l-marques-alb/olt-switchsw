/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_priv.h
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

#ifndef OSAPI_PRIV_H_INCLUDED
#define OSAPI_PRIV_H_INCLUDED

/* System includes, in case users don't include already... */
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

#define OSAPI_TICK_USEC	(1000000/100) /* 100 ticks per second */

#define OSAPI_MAX_PRIO 255

typedef int (*osapi_task_entry_t)(L7_uint32, void *);

typedef struct osapi_task_s {
   L7_uint32 flags;
   pthread_mutex_t lock;
   pthread_cond_t control;
   L7_uint32 osapi_prio;
   L7_uint32 pthread_prio;
   L7_uint32 cur_prio;
   char *name;
   osapi_task_entry_t *entry;
   L7_uint32 argc;
   void *argv;
   L7_uint32 stack_base_addr;
   L7_uint32 stack_size;
   struct osapi_waitq_s *waiting;
   pthread_cond_t fifo_cond;
   struct osapi_task_s *fifo_prev, *fifo_next;
   pthread_t thread;
   pthread_attr_t attr;
   struct sched_param param;
   struct osapi_task_s *chain_prev, *chain_next;
   pid_t PID; 
   int signal_lock;
#ifdef L7_STACK_USAGE_STATS
   L7_uint32 stack_usage_read_time;
#endif
   L7_uint32 prevCpuTime;
} osapi_task_t;

/* Need the following:
      -- wait queue of tasks waiting for this task's deletion
      -- delete safety count
      -- flags to indicate task's state
*/

#define TASK_TIME_SLICED	0x00000001
#define TASK_DELETE_SAFE	0x00000002
#define TASK_DELETED		0x00000004

extern pthread_key_t osapi_task_key;

typedef struct osapi_waitq_s {
   L7_uint32 flags;
   pthread_mutex_t *lock;
   pthread_cond_t control;
   L7_uint32 count;
   union {
      struct {
         osapi_task_t *head, *tail;
      } fifo;
      struct {
         pthread_cond_t cond;
      } prio;
   } policy;
   struct osapi_waitq_s *chain_prev, *chain_next;
} osapi_waitq_t;

#define WAITQ_FIFO		0
#define WAITQ_PRIO		1
#define WAITQ_POLICY_MASK	1
#define WAITQ_POLICY(q)	(q->flags & WAITQ_POLICY_MASK)

#define WAITQ_FLUSHED		2

typedef int (*removal_check_t)(void *);

#define WAITQ_REMOVE_OK	0

extern void osapi_waitq_create(osapi_waitq_t *queue, pthread_mutex_t *lock,
                               L7_uint32 flags);
extern void osapi_waitq_destroy(osapi_waitq_t *queue);
extern L7_RC_t osapi_waitq_enqueue(osapi_waitq_t *queue,
                                   L7_uint32 wait_msec,
                                   removal_check_t removal_check,
                                   void *removal_check_data);
extern void osapi_waitq_dequeue(osapi_waitq_t *queue);
extern void osapi_waitq_dequeue_all(osapi_waitq_t *queue);
extern void osapi_waitq_flush(osapi_waitq_t *queue);
extern void osapi_waitq_remove(osapi_waitq_t *queue, osapi_task_t *task);

extern int osapi_printf(const char *, ...);

extern int osapi_proc_set(const char *path, const char *value);

extern int osapiDevshellSymbolsLoad(void);

#endif /* OSAPI_PRIV_H_INCLUDED */
