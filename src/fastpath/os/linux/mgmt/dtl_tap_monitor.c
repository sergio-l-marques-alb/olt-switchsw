/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename dtl_tap_monitor.c
*
* @purpose Linux tap driver monitor 
*
* @component Device Transformation Layer
*
* @comments This header file is for use _only_ with 
*           a tap driver in place.  This api
*           allows for the registration of a file 
*           descriptor which the monitor task
*           will watch for traffic.  If any traffic is 
*           found it will send it via the dtlPduTransmit command
*
* @author  Neil Horman  
* @end
*
**********************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include "l7_common.h"
#include "osapi.h"
#include "dtlapi.h"
#include "sysapi.h"
#include "l7_product.h"

#include "dtl_tap_monitor.h"


/*
 *DEFINES
 */
#define MAX_FRAME_LEN L7_MAX_FRAME_SIZE 

#define PRINT_CTRL_MSG(format, arg...) do { if (ctrl_dbg) printf(format, ## arg); } while(0)

/*
 *TYPEDEFS, ENUMS, ETC
 */
struct monitor_entry_s
{
   int fd;
   L7_uint32   intIfNum;
   dtlCmdSend  SendFn;
   L7_BOOL valid; /*indicates a valid entry*/
   L7_BOOL skip; /*to prevent race during de-registration*/
};


typedef enum
{
   MSG_ADD_FD=0,
   MSG_DEL_FD,
   MSG_NOP,
}tap_ctrl_msg_e;

struct tap_control_s
{
   tap_ctrl_msg_e type;
   struct monitor_entry_s entry;
};
 
/*
 *STATIC DATA
 */
static struct monitor_entry_s *reg_table;
static fd_set master_set;
static fd_set control_set;
static int master_max_fd;
static int control_max_fd;
static int sp[2];
static int ctrl_dbg=0;
static L7_BOOL tap_monitor_init_complete=L7_FALSE;
static L7_uint32 max_tap_interfaces;
static osapiRWLock_t reg_table_lock;

/*
 *PROTOTYPES
 */
void tap_monitor_task_fn();
void process_tap_control_msg();
 

void tap_monitor_query_task()
{
   struct tap_control_s new_entry;
   new_entry.type = MSG_NOP;
   if(0 > write(sp[0],(const void *)&new_entry,sizeof(struct tap_control_s))){}
}

L7_RC_t tap_monitor_register(int fd, L7_uint32 intIfNum, dtlCmdSend func)
{
   struct tap_control_s new_entry;
   long fd_flags;
   /*
    *make sure the file descriptor is non-blocking
    */
   fd_flags = fcntl(fd,F_GETFL);
   fd_flags |= O_NONBLOCK;            /* PTIN DO NOT ADD: | O_ASYNC */
   fcntl(fd,F_SETFL,fd_flags);

   new_entry.entry.fd = fd;
   new_entry.entry.intIfNum = intIfNum;
   new_entry.entry.SendFn = func;
   new_entry.entry.valid = L7_TRUE;
   new_entry.entry.skip = L7_FALSE;
   new_entry.type = MSG_ADD_FD;

   /*
    *write this to the control socket
    */
   PRINT_CTRL_MSG("adding fd %d with function 0x%x to tap monitor\n",fd,(unsigned int)func); 
   if(write(sp[0],(const void *)&new_entry,sizeof(struct tap_control_s)) < 0)
      perror("Could not send control message to tap_monitor"); 

   return L7_SUCCESS;
}

L7_RC_t tap_monitor_unregister(int fd)
{
   struct tap_control_s del_entry;

   /*
    *set the message to be a delete 
    */
   del_entry.type = MSG_DEL_FD;
   del_entry.entry.fd = fd;

   /*
    *send the message
    */
   PRINT_CTRL_MSG("removing fd %d from tap monitor\n",fd);
   if(write(sp[0],(const void *)&del_entry,sizeof(struct tap_control_s)) < 0)
      perror("Could not send control message to tap monitor");

   return L7_SUCCESS;
}

L7_RC_t tap_monitor_init(L7_uint32 num_interfaces)
{
   struct monitor_entry_s *old_reg_table, *new_reg_table;
   L7_uint32 old_max_tap_interfaces;
   L7_RC_t rc;

   /*
    *check to see if we have been called before
    */
   PRINT_CTRL_MSG("calling tap_monitor_init\n"); 
   if(tap_monitor_init_complete == L7_TRUE)
   {
      if (num_interfaces <= max_tap_interfaces)
      {
	PRINT_CTRL_MSG("tap_monitor_init already called. returning\n");
	/*
	 *we need to be multi init-safe, since
	 *different packages call this from different places
	 */ 
	return L7_SUCCESS;	
      }
      else
      {
	PRINT_CTRL_MSG("tap_monitor_init called with more interfaces, need to re-allocate\n");
	old_max_tap_interfaces = max_tap_interfaces;
	old_reg_table = reg_table;
      }
   } 
   else 
   {     
     tap_monitor_init_complete=L7_TRUE;
     old_reg_table = NULL;
     old_max_tap_interfaces= 0;
     rc = osapiRWLockCreate(&reg_table_lock, OSAPI_RWLOCK_Q_FIFO);
     if (rc != L7_SUCCESS)
     {
        return L7_FAILURE;
     }
     /*
      *zero the master file descriptor set
      */
     PRINT_CTRL_MSG("Initalizing read set\n");
     FD_ZERO(&master_set);
     master_max_fd = -1;
     
     /*
      *create an AF_UNIX socket pair
      *to communicate inline messages to the 
      *monitor task
      */
     PRINT_CTRL_MSG("Creating socketpair\n");
     if(socketpair(AF_UNIX,SOCK_DGRAM,0,sp) < 0)
       {
	 SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "can not create control sockets: errno = %d\n",errno);
	 return L7_ERROR;
       }
     
     /*
      *add the read socket to the master set
      */
     PRINT_CTRL_MSG("adding control socket to read_set\n");
     FD_SET(sp[1],&master_set);
     master_max_fd = sp[1];
     master_max_fd++;
     
     /* setup control setup */
     FD_ZERO(&control_set);
     FD_SET(sp[1],&control_set);
     control_max_fd = sp[1] + 1;
     
     /*
      *create the monitor task
      */
     PRINT_CTRL_MSG("creating tap_monitor_task\n");
     if(osapiTaskCreate("tap_monitor_task",tap_monitor_task_fn,0,NULL,
			L7_DEFAULT_STACK_SIZE,
			L7_DEFAULT_TASK_PRIORITY,
			L7_DEFAULT_TASK_SLICE) == L7_ERROR)
       return L7_FAILURE;
   }
   /* Prevent lookups into or changes to the table while we're changing 
      it around */
   /* Wake up task first if it's there, to keep us from sleeping waiting for 
      a packet */
   tap_monitor_query_task();
   while (osapiWriteLockTake(reg_table_lock, L7_WAIT_FOREVER) != L7_SUCCESS) 
     ; /* Do nothing */   
   /*
    *initalize the reg_table
    */
   PRINT_CTRL_MSG("initalizing reg_table\n");  
   new_reg_table = osapiMalloc(L7_OSAPI_COMPONENT_ID, 
			       sizeof(struct monitor_entry_s)*num_interfaces);
   memset(new_reg_table,0,sizeof(struct monitor_entry_s)*num_interfaces);
   if (old_reg_table) 
   {
     PRINT_CTRL_MSG("Copying old reg_table to new one\n");
     memcpy(new_reg_table, old_reg_table, 
	    sizeof(struct monitor_entry_s)*old_max_tap_interfaces);
     osapiFree(L7_OSAPI_COMPONENT_ID, old_reg_table);
   }
   reg_table = new_reg_table;
   max_tap_interfaces = num_interfaces;
   osapiWriteLockGive(reg_table_lock);
   return L7_SUCCESS;
}

void tap_monitor_task_fn() 
{
   fd_set read_set;
   int i,max_fd,select_rc = 0;
   L7_netBufHandle frame_buffer;
   L7_uchar8 *frame_data;
   ssize_t frame_length;
   struct tapDtlInfo_s tapInfo;
   struct timeval tv;
   L7_uint32 failCount = 0;

   /*
    *do this forever
    */
   PRINT_CTRL_MSG("tap_monitor starting\n");
   for(;;)
   {
      /* did the last select fail? */
      if(select_rc == -1)
      {
        /* on select failures, service the control fd 
         * this is useful if there is a control message
         * pending for and interface delete. servicing it
         * will remove the invalid sockfd that (may be) causing
         * select to cop-out.
        */
        max_fd = control_max_fd; 
        memcpy(&read_set,&control_set,sizeof(fd_set));

        /* but dont block, because we dont know what caused 
         * the failure. it might have been a one off
         * condition like a signal.
         */
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        PRINT_CTRL_MSG("Checking for tap control messages\n");
        select_rc = select(max_fd,&read_set,NULL,NULL,&tv);

        /* loop on timeout, this ensures we dont reset
         * the failure counter just yet
         */
        if(select_rc == 0)
          continue;
      }
      else
      {
        /* wait on select for a packet to come our way */
        max_fd = master_max_fd; 
        memcpy(&read_set,&master_set,sizeof(fd_set));

        PRINT_CTRL_MSG("Waiting on select call\n");
        select_rc = select(max_fd,&read_set,NULL,NULL,NULL);
      }

      /* loop on select failures */
      if(select_rc == -1)
      {
        PRINT_CTRL_MSG("\nTap select error: %s", strerror(errno));

        /* log continuous failures */
        failCount++;
        if(failCount == 100)
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
              "Tap monitor task is spinning on select failures."
              " Trouble reading the /dev/tap device, check the error"
              " message %s for details.", strerror(errno));
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSAPI_COMPONENT_ID, 
              "Tap monitor select failed: %s."
              " Trouble reading the /dev/tap device, check the error"
              " message for details." , strerror(errno));
        }

        continue;
      }

      /* reset failure counter */
      failCount = 0;

      /*
       * did the control socket wake us up
       */
      if(FD_ISSET(sp[1],&read_set))
      {
         PRINT_CTRL_MSG("Processing control message\n");
         process_tap_control_msg();
         select_rc--; 
      }       

      /*
       *now that we are here we have activity on 
       *one or more file descriptors.  The return 
       *code from select should tell us how many file 
       *descriptors we need to process, so lets just 
       *do a linear search of them
       */
      PRINT_CTRL_MSG("Return Code from Select is %d\n",select_rc);
      i=0;
      while (osapiReadLockTake(reg_table_lock, L7_WAIT_FOREVER) != L7_SUCCESS)
	; /* Do nothing */
      PRINT_CTRL_MSG("Main loop past read lock\n");
      while((select_rc > 0) && (i < max_tap_interfaces))
      {
         if((reg_table[i].valid == L7_TRUE) &&
            (reg_table[i].skip == L7_FALSE) &&
            (FD_ISSET(reg_table[i].fd,&read_set)))
         {
            PRINT_CTRL_MSG("fd %d is valid, not skipped  and in the read set, reading...\n",reg_table[i].fd);
            /*
             *we need to read from this descriptor
             *and send it through the dtl interface
             *start by getting a frame buffer
             */
             SYSAPI_NET_MBUF_GET(frame_buffer);
             if((void *)frame_buffer == NULL)
             {
                /*
                 *if the frame buffer is NULL
                 *then  we should just give up
                 *this iteration and retry from 
                 *the top of this loop 
                 *the frames will still be in the
                 *file descriptor streams and perhaps
                 *our next iteration will present an 
                 *available frame
                 */
                PRINT_CTRL_MSG("Unable to get a buffer!\n");
                i++;
                select_rc--;
                continue;
             }/*end if*/
 
            /*
             *once we have a frame buffer, get its data 
             *pointer
             */
            SYSAPI_NET_MBUF_GET_DATASTART(frame_buffer,frame_data);

            /*
             *and read the frame out of the appropriate 
             *file descriptor
             */
            frame_length = read(reg_table[i].fd,(void *)frame_data,MAX_FRAME_LEN);
            PRINT_CTRL_MSG("frame length is %d bytes\n",frame_length);

            /*
             *check to make sure we got something
             */
            if(frame_length <= 0) 
            {
               SYSAPI_NET_MBUF_FREE(frame_buffer);
               select_rc--;
               i++;
               continue;
            }
           
            SYSAPI_NET_MBUF_SET_DATALENGTH(frame_buffer,frame_length);
            
            /*
             *now call the function to get the dtl command send
             */
            PRINT_CTRL_MSG("Calling send function at 0x%x\n",(unsigned int)reg_table[i].SendFn);

	    /* send fn takes ownership of buffer */
            reg_table[i].SendFn(reg_table[i].fd,reg_table[i].intIfNum,frame_buffer,&tapInfo);

            /*
             *decrement the count of ready fd's
             */ 
            select_rc--;
         }/*end if*/
         /*
          *increment the reg_table index variable
          */
         i++;
      }/*end while*/
      osapiReadLockGive(reg_table_lock);
   }/*end for(;;)*/

   SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Premature exit from dtl monitor task!!!\n");
}


void process_tap_control_msg()
{
   struct tap_control_s tap_msg;

   /*
    *we need to read from the control socket 
    *a tap_control_msg_s struct
    */
   if(0 > read(sp[1],(void *)&tap_msg,sizeof(struct tap_control_s))){}

   /*
    *do whatever the command says to
    */
   PRINT_CTRL_MSG("tap_msg.type = %d\n",tap_msg.type);
   switch(tap_msg.type)
   {
       case MSG_ADD_FD:
       {
          int i;
          /*
           *we need to add a fd 
           */
	  PRINT_CTRL_MSG("Add fd waiting on write lock\n");
	  while (osapiWriteLockTake(reg_table_lock, L7_WAIT_FOREVER) != L7_SUCCESS)
	    ; /* Do nothing */
	  PRINT_CTRL_MSG("Add fd past write lock\n");
          for(i=0;i < max_tap_interfaces;i++)
          {
             if(reg_table[i].valid == L7_FALSE)
             {
                memcpy(&reg_table[i],&tap_msg.entry,sizeof(struct monitor_entry_s));
                master_max_fd = ((master_max_fd > (tap_msg.entry.fd+1)) ? master_max_fd : (tap_msg.entry.fd +1));
                FD_SET(tap_msg.entry.fd,&master_set);
		PRINT_CTRL_MSG("Added fd %d to table\n", tap_msg.entry.fd);
		osapiWriteLockGive(reg_table_lock);
                return;
	     }
          }
	  osapiWriteLockGive(reg_table_lock);
	  /* No place to put this entry? Fatal error - the interface will be 
	     non-functional */
	  LOG_ERROR(111);
          break; 
      }/*end case*/

      case MSG_DEL_FD:
      {
         int i;
         /*
          *search the reg_table for a matching fd
          */
	  PRINT_CTRL_MSG("Del fd waiting on write lock\n");
	  while (osapiWriteLockTake(reg_table_lock, L7_WAIT_FOREVER) != L7_SUCCESS)
	    ; /* Do nothing */
	  PRINT_CTRL_MSG("Del fd past write lock\n");
         for(i=0;i<max_tap_interfaces;i++)
         {
             if(tap_msg.entry.fd == reg_table[i].fd)
             {
                reg_table[i].valid = L7_FALSE;
                FD_CLR(tap_msg.entry.fd,&master_set);
		PRINT_CTRL_MSG("Removed fd %d from table\n", tap_msg.entry.fd);
                break;
             }
         }

         if (i >= max_tap_interfaces) 
	 { 
 	    osapiWriteLockGive(reg_table_lock);
	    break;
	 }
         if(master_max_fd == (tap_msg.entry.fd + 1))
         {
            master_max_fd = -1;
            for(i=0;i<max_tap_interfaces;i++)
            {
               if((reg_table[i].valid == L7_TRUE) &&
                  (reg_table[i].fd > master_max_fd))
               {
                  master_max_fd = reg_table[i].fd;
               }
            }
            master_max_fd++;
         }
	 osapiWriteLockGive(reg_table_lock);
         break;
      }/*end case*/
      case MSG_NOP:
      {
         PRINT_CTRL_MSG("Handling no op message\n");
         break;
      }
      default:
      {
         SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS, "Unknown msg type %d\n",tap_msg.type);
         break;
      }/*end default*/
   }/*end switch*/
   return;
}


void tap_monitor_dbg(int ctrl_mode)
{
   ctrl_dbg = ctrl_mode;
}

void tap_monitor_dump_reg_table()
{
   int i;
   for(i=0;i<max_tap_interfaces;i++)
   {
      printf("reg_table[%d].valid = %s\n",i,(reg_table[i].valid == L7_TRUE) ? "True":"False");
      printf("reg_table[%d].skip = %s\n",i,(reg_table[i].skip == L7_TRUE) ? "True":"False");
      printf("reg_table[%d].fd = %d\n",i,reg_table[i].fd);
      printf("reg_table[%d].SendFn=0x%x\n\n",i,(unsigned int)reg_table[i].SendFn);
   }
}
    

