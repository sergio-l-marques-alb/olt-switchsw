#ifdef _L7_OS_LINUX_
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  isapi_msg.c
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

  #include <stdio.h>
  #include <string.h>
  #include <pthread.h>

  #include <l7_common.h>
  #include <osapi.h>

  #include "osapi_priv.h"

/* PTin added: Debug */
#include "logger.h"

typedef struct osapi_msg_s
{
  struct osapi_msg_s *next;
  char buf[1]; /* length (1) is placeholder for actual message length */
} osapi_msg_t;

  #define OSAPI_MSG_HEADER_SIZE (&(((osapi_msg_t *)0)->buf[0]))

typedef struct osapi_msgq_s
{
  L7_uint32 flags;
  pthread_mutex_t lock;
  pthread_cond_t control;
  char *name;
  L7_uint32 msg_count, msg_max, msg_len, msg_high;
  osapi_msg_t *base, *free;
  osapi_msg_t *head, *tail;
  osapi_waitq_t send_wait, recv_wait;
  struct osapi_msgq_s *chain_prev, *chain_next;
  L7_uint32  mem_size;
} osapi_msgq_t;

  #define MSGQ_DELETED	0x00000001
  #define MSGQ_SEND_PEND	0x00000002
  #define MSGQ_RECV_PEND	0x00000004

static pthread_mutex_t msgq_list_lock = PTHREAD_MUTEX_INITIALIZER;
static osapi_msgq_t *msgq_list_head = NULL;

/**************************************************************************
* @purpose  Display information about message queues.
*
* @param    queue_id    @b{(input)}  Queue ID. If 0 then all queues.
*
* @returns  none
*
* @comments none
*
* @end
*************************************************************************/
void osapiMsgQueueShow(L7_uint32 queue_id)
{
  osapi_msgq_t *msg_queue;
  osapi_msgq_t *next_q;


  msg_queue = (osapi_msgq_t *) queue_id;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&msgq_list_lock);
  pthread_mutex_lock(&msgq_list_lock);

  next_q = msgq_list_head;
  while (next_q != 0)
  {
    if ((queue_id == 0) || (msg_queue == next_q))
    {
      printf("Queue ID: %d (0x%x)\n",
                  (int) next_q, 
                  (int) next_q);
      printf("Queue Name: %s\n",
                  next_q->name);
      printf("Mem Size: %d\n",
                  next_q->mem_size);
      printf("Max Msg: %d\n",
                  next_q->msg_max);
      printf("Num Msg: %d\n", 
                  next_q->msg_count);

      printf("\n\n");

    }
    next_q = next_q->chain_next;
  }

  pthread_cleanup_pop(1);
}

/**************************************************************************
* @purpose  Create a message queue.
*
* @param    queue_name    @b{(input)}  Not used, but kept for backwards compatability.
* @param    queue_size    @b{(input)}  is the max number of the messages on the queue.
* @param    message_size  @b{(input)}  is the size of each message in bytes.
*
* @returns  pointer to the Queue ID structure or L7_NULLPTR if the create failed.
*
* @comments    This routine creates a message queue capable of holding up to
* @comments    queue_size messages, each up to message_size bytes long.  The
* @comments    routine returns a void ptr used to identify the created message queue
* @comments    in all subsequent calls to routines in this library. The queue will be
* @comments    created as a FIFO queue.
*
* @end
*************************************************************************/
void * osapiMsgQueueCreate(L7_char8 *queue_name, L7_uint32 queue_size,
                           L7_uint32 message_size)
{

  L7_uint32 i, namelen, datalen, offset;
  char *namePtr;
  char synth_name[16];
  osapi_msg_t *msg;
  osapi_msgq_t *newMsgQ;

  /* change this to use one malloc for queue + name + data */

  if ((newMsgQ = (osapi_msgq_t *)osapiMalloc(L7_OSAPI_COMPONENT_ID, sizeof(osapi_msgq_t))) == NULL)
  {

    return(L7_NULLPTR);

  }

  if (queue_name == NULL)
  {

    sprintf(synth_name, "q%d", (int)newMsgQ);

    namePtr = synth_name;

  }
  else
  {

    namePtr = queue_name;

  }

  namelen = strlen(namePtr) + 1;

  if ((newMsgQ->name = osapiMalloc(L7_OSAPI_COMPONENT_ID, namelen)) == NULL)
  {

    return(L7_NULLPTR);

  }

  strcpy (newMsgQ->name, namePtr);

  offset = message_size + (L7_uint32)OSAPI_MSG_HEADER_SIZE;

  datalen = queue_size * offset;

  if ((newMsgQ->base = osapiMalloc(L7_OSAPI_COMPONENT_ID, datalen)) == NULL)
  {

    return(L7_NULLPTR);

  }

  newMsgQ->mem_size = datalen;
  newMsgQ->flags = 0;

  pthread_mutex_init(&(newMsgQ->lock), NULL);

  pthread_cond_init(&(newMsgQ->control), NULL);

  newMsgQ->msg_count = 0;

  newMsgQ->msg_max = queue_size;

  newMsgQ->msg_len = message_size;

  msg = newMsgQ->free = newMsgQ->base;

  for (i = 0; i < (queue_size - 1); i++)
  {

    msg->next = (osapi_msg_t *)((char *)msg + offset);

    msg = msg->next;

  }

  msg->next = NULL;

  newMsgQ->head = newMsgQ->tail = NULL;

  osapi_waitq_create(&(newMsgQ->send_wait), &(newMsgQ->lock), WAITQ_FIFO);

  osapi_waitq_create(&(newMsgQ->recv_wait), &(newMsgQ->lock), WAITQ_FIFO);

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&msgq_list_lock);
  pthread_mutex_lock(&msgq_list_lock);

  if (msgq_list_head != NULL)
  {

    msgq_list_head->chain_prev = newMsgQ;

  }

  newMsgQ->chain_next = msgq_list_head;

  msgq_list_head = newMsgQ;

  newMsgQ->chain_prev = NULL;

  pthread_cleanup_pop(1);

  return((void *)newMsgQ);

}

/**************************************************************************
* @purpose  Delete a message queue.
*
* @param    queue_ptr     @b{(input)}  Pointer to message queue to delete
*
* @returns  L7_SUCCESS or L7_ERROR
*
* @comments    This routine deletes a message queue.
*
* @end
*************************************************************************/
L7_RC_t osapiMsgQueueDelete(void *queue_ptr)
{

  osapi_msgq_t *osapiMsgQ = (osapi_msgq_t *)queue_ptr;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&(osapiMsgQ->lock));
  pthread_mutex_lock(&(osapiMsgQ->lock));

  osapiMsgQ->flags |= MSGQ_DELETED;

  osapi_waitq_flush(&(osapiMsgQ->send_wait));

  osapi_waitq_destroy(&(osapiMsgQ->send_wait));

  osapi_waitq_flush(&(osapiMsgQ->recv_wait));

  osapi_waitq_destroy(&(osapiMsgQ->recv_wait));

  pthread_cond_destroy(&(osapiMsgQ->control));

  osapiFree(L7_OSAPI_COMPONENT_ID, osapiMsgQ->name);

  osapiFree(L7_OSAPI_COMPONENT_ID, osapiMsgQ->base);

  pthread_cleanup_pop(1);

  pthread_mutex_destroy(&(osapiMsgQ->lock));

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&msgq_list_lock);
  pthread_mutex_lock(&msgq_list_lock);

  if (osapiMsgQ->chain_next != NULL)
  {

    osapiMsgQ->chain_next->chain_prev = osapiMsgQ->chain_prev;

  }

  if (osapiMsgQ->chain_prev != NULL)
  {

    osapiMsgQ->chain_prev->chain_next = osapiMsgQ->chain_next;

  }
  else
  { /* must be head of list */

    msgq_list_head = osapiMsgQ->chain_next;

  }

  pthread_cleanup_pop(1);

  osapiFree(L7_OSAPI_COMPONENT_ID, osapiMsgQ);

  return(L7_SUCCESS);
}

static int osapi_msgq_send_waitq_remove_check(void *msgq)
{

  osapi_msgq_t *osapiMsgQ = (osapi_msgq_t *)msgq;

  if (osapiMsgQ->msg_count != osapiMsgQ->msg_max)
    return(WAITQ_REMOVE_OK);

  if ((osapiMsgQ->flags & MSGQ_DELETED) != 0)
    return(WAITQ_REMOVE_OK);

  return(~WAITQ_REMOVE_OK);

}

/**************************************************************************
*
* @purpose  Send a message to a message queue.
*
* @param    queue_ptr    @b{(input)}  Pointer to message queue.               
* @param    Message      @b{(input)}  pointer to the message.          
* @param    Size         @b{(input)}  size of the message in bytes.
* @param    Wait         @b{(input)}  a flag to wait or not. L7_NO_WAIT or L7_WAIT_FOREVER.
* @param    Priority     @b{(input)}  L7_MSG_PRIORITY_NORM or L7_MSG_PRIORITY_HI.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @comments This routine sends the message in buffer Message of length Size bytes
*           to the message queue queue_ptr. If any tasks are already waiting to
*           receive messages on the queue, the message will immediately
*           be delivered to the first waiting task. If no task is waiting to receive
*           messages, the message is saved in the message queue.
*
* @end
*
*************************************************************************/
L7_RC_t osapiMessageSend(void *queue_ptr, void *Message, L7_uint32 Size,
                         L7_uint32 Wait, L7_uint32 Priority)
{

  L7_RC_t rc = L7_SUCCESS;
  osapi_msg_t *newMsg;
  osapi_msgq_t *osapiMsgQ = (osapi_msgq_t *)queue_ptr;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&(osapiMsgQ->lock));
  pthread_mutex_lock(&(osapiMsgQ->lock));

  if (Size <= osapiMsgQ->msg_len)
  {

    while ((osapiMsgQ->msg_count == osapiMsgQ->msg_max)
           && ((osapiMsgQ->flags & MSGQ_DELETED) == 0))
    {

      osapiMsgQ->flags |= MSGQ_SEND_PEND;

      rc = osapi_waitq_enqueue(&(osapiMsgQ->send_wait), Wait,
                               osapi_msgq_send_waitq_remove_check,
                               (void *)osapiMsgQ);

      /* only remove the pending flag if no one is waiting */
      if (osapiMsgQ->recv_wait.count == 0)
        osapiMsgQ->flags &= ~MSGQ_RECV_PEND;

      if (rc != L7_SUCCESS) break;

    }

    if ((osapiMsgQ->flags & MSGQ_DELETED) != 0)
    {
      LOG_PT_ERR(LOG_CTX_MISC,"Message deleted");
      rc = L7_ERROR;

    }
    else if (rc == L7_SUCCESS)
    {

      newMsg = osapiMsgQ->free;

      osapiMsgQ->free = osapiMsgQ->free->next;

      newMsg->next = NULL;

      if (Priority == L7_MSG_PRIORITY_NORM)
      {

        if (osapiMsgQ->tail == NULL)
        {

          osapiMsgQ->head = newMsg;

        }
        else
        {

          osapiMsgQ->tail->next = newMsg;

        }

        osapiMsgQ->tail = newMsg;

      }
      else /* L7_MSG_PRIORITY_HI */
      {

        if (osapiMsgQ->head == NULL)
        {

          osapiMsgQ->tail = newMsg;

        }
        else
        {

          newMsg->next = osapiMsgQ->head;

        }

        osapiMsgQ->head = newMsg;

      }

      osapiMsgQ->msg_count++;
      if(osapiMsgQ->msg_count > osapiMsgQ->msg_high)
      {
         osapiMsgQ->msg_high = osapiMsgQ->msg_count;
      }

      memcpy(newMsg->buf, Message, Size);

      if ((osapiMsgQ->flags & MSGQ_RECV_PEND) != 0)
      {

        osapi_waitq_dequeue(&(osapiMsgQ->recv_wait));

      }

      rc = L7_SUCCESS;

    }

  }
  else
  {

    LOG_PT_ERR(LOG_CTX_MISC,"queue_ptr 0x%08x is full",(L7_uint32) queue_ptr);
    rc = L7_ERROR;

  }

  pthread_cleanup_pop(1);

  return(rc);

}

static int osapi_msgq_recv_waitq_remove_check(void *msgq)
{

  osapi_msgq_t *osapiMsgQ = (osapi_msgq_t *)msgq;

  if (osapiMsgQ->msg_count != 0)
    return(WAITQ_REMOVE_OK);

  if ((osapiMsgQ->flags & MSGQ_DELETED) != 0)
    return(WAITQ_REMOVE_OK);

  return(~WAITQ_REMOVE_OK);

}

/**************************************************************************
* @purpose  Receive a message from a message queue.
*
* @param    Queue_ptr @b{(input)}   Pointer to message queue.
* @param    Message @b{(output)}    Place to put the message.
* @param    Size @b{(input)}        Number of bytes to move into the message.
* @param    Wait @b{(input)}        a flag to wait or not. L7_NO_WAIT or L7_WAIT_FOREVER.
*
* @returns  L7_SUCCESS on success or L7_ERROR if an error occured.
*
* @comments    This routine receives a message from the message queue queue_ptr. The received message is
* @comments    copied into the specified buffer, Message, which is Size bytes in length.
* @comments    If the message is longer than Size, the remainder of the message is discarded (no
* @comments    error indication is returned).
*
* @end
*************************************************************************/
L7_RC_t osapiMessageReceive(void *queue_ptr, void *Message,
                            L7_uint32 Size, L7_uint32 Wait)
{

  L7_RC_t rc = L7_SUCCESS;
  osapi_msg_t *msg;
  osapi_msgq_t *osapiMsgQ = (osapi_msgq_t *)queue_ptr;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&(osapiMsgQ->lock));
  pthread_mutex_lock(&(osapiMsgQ->lock));

  while ((osapiMsgQ->msg_count == 0)
         && ((osapiMsgQ->flags & MSGQ_DELETED) == 0))
  {

    osapiMsgQ->flags |= MSGQ_RECV_PEND;

    rc = osapi_waitq_enqueue(&(osapiMsgQ->recv_wait), Wait,
                             osapi_msgq_recv_waitq_remove_check,
                             (void *)osapiMsgQ);

    /* only remove the pending flag if no one is waiting */
    if (osapiMsgQ->send_wait.count == 0)
      osapiMsgQ->flags &= ~MSGQ_RECV_PEND;

    if (rc != L7_SUCCESS)
    {
      //LOG_PT_CRITIC(LOG_CTX_MISC,"rc=%d (osapiMsgQ->send_wait.count=%u)", rc, osapiMsgQ->send_wait.count);    /* PTin added: Debug */
      break;
    }
  }

  if ((osapiMsgQ->flags & MSGQ_DELETED) != 0)
  {
    /* PTin modified: Just to help detecting the problem */
    rc = L7_NOT_EXIST; // L7_ERROR;
    LOG_PT_CRITIC(LOG_CTX_MISC,"rc=%d",rc);      /* PTin added: Debug */

  }
  else if (rc == L7_SUCCESS)
  {

    msg = osapiMsgQ->head;

    if ((osapiMsgQ->head = osapiMsgQ->head->next) == NULL)
    {

      osapiMsgQ->tail = NULL;

    }

    osapiMsgQ->msg_count--;

    memcpy(Message, msg->buf,
           ((Size < osapiMsgQ->msg_len) ? Size : osapiMsgQ->msg_len));

    msg->next = osapiMsgQ->free;

    osapiMsgQ->free = msg;

    if ((osapiMsgQ->flags & MSGQ_SEND_PEND) != 0)
    {

      osapi_waitq_dequeue(&(osapiMsgQ->send_wait));

    }

    rc = L7_SUCCESS;

  }

  pthread_cleanup_pop(1);

  return(rc);

}

L7_RC_t osapiMessagePeek(void *queue_ptr, void *Message,
                            L7_uint32 Size, L7_uint32 msgOffset)
{

  L7_RC_t rc = L7_FAILURE;
  osapi_msg_t *msg;
  osapi_msgq_t *osapiMsgQ = (osapi_msgq_t *)queue_ptr;


  if(osapiMsgQ->msg_count < msgOffset)
    return rc;

  msg = osapiMsgQ->head;

  while(msg)
  {
    if(!msgOffset)
    {
      memcpy(Message, msg->buf,
             ((Size < osapiMsgQ->msg_len) ? Size : osapiMsgQ->msg_len));
      rc = L7_SUCCESS;
      break;
    }


    msg = msg->next;
    msgOffset--;
  }
  

  return(rc);
}
/**************************************************************************
* @purpose  Returns the current number of messages on the specified message queue.
*
* @param    queue_ptr  @b{(input)}  Pointer to message queue.
* @param    queue_ptr  @b{(output)} Pointer to area to return number
*
* @returns  L7_SUCCESS or L7_ERROR.
*
* @comments    none.
*
* @end
*************************************************************************/
L7_RC_t osapiMsgQueueGetNumMsgs(void *queue_ptr, L7_int32 *bptr)
{

  osapi_msgq_t *osapiMsgQ = (osapi_msgq_t *)queue_ptr;

  pthread_cleanup_push((void (*)(void *))pthread_mutex_unlock,
                       (void *)&(osapiMsgQ->lock));
  pthread_mutex_lock(&(osapiMsgQ->lock));

  *bptr = osapiMsgQ->msg_count;

  pthread_cleanup_pop(1);

  return(L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Print information about message queues
*
* @param    void    
*
* @returns  void
*
* @comments  If the compId is zero, then all components are displayed
*
* @end
*
*************************************************************************/
void osapiDebugMsgQueuePrint(void)
{

  osapi_msgq_t *osapiMsgQ;

  osapiMsgQ = msgq_list_head;

  printf ("Queue      Queue             Messages   Num threads waiting to   Messages\n");
  printf ("  ID       Name              in Queue      Send       Receive     High\n");
  printf ("-----      -----             --------   ----------  -----------  --------\n");

  for (;;)
  {
      if (osapiMsgQ != L7_NULL)
      {

          printf ("%x - %-16.15s    %6d   %6d      %6d  %6d\n",
                        (L7_uint32)osapiMsgQ,
                        osapiMsgQ->name,
                        osapiMsgQ->msg_count,
                        osapiMsgQ->send_wait.count,
                        osapiMsgQ->recv_wait.count,
                        osapiMsgQ->msg_high);


          osapiMsgQ = osapiMsgQ->chain_next;

      }  /* osapiMsgQ != L7_NULL */
      else
      {
          /* End of list */
          printf("\n");
          break;
      }

  } /* for (;;)*/

}



#endif /* _L7_OS_LINUX_ */

