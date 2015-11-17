/* $Id: sand_os_interface.h,v 1.8 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/
#ifndef SOC_SAND_OS_INTERFACE_H_INCLUDED
#define SOC_SAND_OS_INTERFACE_H_INCLUDED
/* $Id: sand_os_interface.h,v 1.8 Broadcom SDK $
 * {
 */

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/drv.h>
/* some os methods accept a timeout parameter,
 * that might indicate (a) infinite timeout (b) no timeout.
 * We use this instead, inorder to be os indepenedent
 */
#define SOC_SAND_INFINITE_TIMEOUT (-1)
#define SOC_SAND_NO_TIMEOUT       0

/* Helper for sal_alloc */
#define sal_dpp_alloc(size,str) sal_alloc(size,str)

/* Helper for sal_rand */
#ifndef __KERNEL__
#include <sal/appl/sal.h>
#else
extern int  fixme_sal_rand_in_kernel_mode(void);
#define sal_rand fixme_sal_rand_in_kernel_mode
#endif
/*
 * Macro "SOC_SAND_PRINTF_ATTRIBUTE":
 * This define is originated from GCC capabilities to check
 * "printf" like function formating.
 * For exact formating refer to GCC docs section "Declaring Attributes of Functions".
 *
 * Define "SOC_SAND_PRINTF_ATTRIBUTE_ENABLE_CHECK"
 * In order to do the checks, define "SOC_SAND_PRINTF_ATTRIBUTE_ENABLE_CHECK"
 * in your compilation system. In GCC add "-DSAND_PRINTF_ATTRIBUTE_ENABLE_CHECK"
 * to the compilation command line.
 */
#ifdef SOC_SAND_PRINTF_ATTRIBUTE_ENABLE_CHECK
  #ifdef __GNUC__
    #define SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check)  \
      __attribute__ ((format (__printf__, string_index, first_to_check)))
  #else
    #error  "Add your system support for 'SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check)'."
  #endif

#elif defined(COMPILER_ATTRIBUTE)
  #define SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check) \
    COMPILER_ATTRIBUTE ((format (__printf__, string_index, first_to_check)))
#else
  #define SOC_SAND_PRINTF_ATTRIBUTE(string_index, first_to_check)
#endif

/*
 *  ANSI C forbids casts from function pointers to data pointers and vice versa, so you can't 
 *  use void * to pass a pointer to a function.
 */
typedef void (*SOC_SAND_FUNC_PTR)(void*);

/*
 * {
 * Sorting and Searching
 */

/*
 * qsort
 */
void
  soc_sand_os_qsort(
    void         *bot,
    uint32 nmemb,
    uint32 size,
    int (*compar)(void *, void *)
    ) ;
/*
 * bsearch
 */
void
  *soc_sand_os_bsearch(
    void    *key,
    void    *base0,
    uint32  nmemb,
    uint32  size,
    int (*compar)(void *, void *)
    ) ;

/*
 * }
 */

/*
 * {
 * Task management
 */

/*
 * Lock and unlock task switching.
 */
SOC_SAND_RET
  soc_sand_os_task_lock(
    void
    ) ;
SOC_SAND_RET
  soc_sand_os_task_unlock(
    void
    ) ;


/*
 * taskSpawn is vxworks for windows CreateThread() or unix fork()
 */
sal_thread_t
  soc_sand_os_task_spawn(
    SOC_SAND_IN     char          *name,     /* name of new task (stored at pStackBase) */
    SOC_SAND_IN     long          priority,  /* priority of new task */
    SOC_SAND_IN     long          options,   /* task option word */
    SOC_SAND_IN     long          stackSize, /* size (bytes) of stack needed plus name */
    SOC_SAND_IN     SOC_SAND_FUNC_PTR      entryPt,  /* entry point of new task */
    SOC_SAND_IN     long          arg1,      /* 1st of 10 req'd task args to pass to func */
    SOC_SAND_IN     long          arg2,
    SOC_SAND_IN     long          arg3,
    SOC_SAND_IN     long          arg4,
    SOC_SAND_IN     long          arg5,
    SOC_SAND_IN     long          arg6,
    SOC_SAND_IN     long          arg7,
    SOC_SAND_IN     long          arg8,
    SOC_SAND_IN     long          arg9,
    SOC_SAND_IN     long          arg10
  );
/*
 * delete a task
 */
SOC_SAND_RET
  soc_sand_os_task_delete(
    SOC_SAND_IN     sal_thread_t          task_id
  );
/*
 * returns the running thread tid
 * should never be zero.
 */
sal_thread_t
  soc_sand_os_get_current_tid(
    void
  );

/*
 * }
 */

/*
 * {
 * Memory Management
 */

/*
 * malloc
 */
void
  *soc_sand_os_malloc(
    SOC_SAND_IN     uint32 size,
    char            *str
  );
/*
 * free
 */
SOC_SAND_RET
  soc_sand_os_free(
    SOC_SAND_INOUT  void          *memblock
  );

/*
 * malloc
 */
void
  *soc_sand_os_malloc_any_size(
    SOC_SAND_IN     uint32 size,
    char            *str
  );
/*
 * free
 */
SOC_SAND_RET
  soc_sand_os_free_any_size(
    SOC_SAND_INOUT  void          *memblock
  );
/*
 * Get amount of bytes are currently allocated by soc_sand_os_malloc()
 */
uint32
  soc_sand_os_get_total_allocated_memory(
    void
  );


/*
 * copy a buffer from source to destination
 */
SOC_SAND_RET
  soc_sand_os_memcpy(
    void          *destination, /* destination of copy */
    const void    *source,      /* source of copy */
    uint32 size          /* size of memory to copy (in bytes) */
    );
/*
 * compare 2 buffers
 */
int
  soc_sand_os_memcmp(
    const     void *s1, /* array 1 */
    const     void *s2, /* array 2 */
    uint32 n   /* size of memory to compare */
  );
/*
 * loads memory with a char
 */
SOC_SAND_RET
  soc_sand_os_memset(
    void *m,           /* array 1 */
    int   c,           /* array 2 */
    uint32 n   /* size of memory in bytes */
  );

/*
 * }
 */

/*
 * {
 * Sleep and Time
 */

/*
 * suspend a task for mili-second.
 */
SOC_SAND_RET
  soc_sand_os_task_delay_milisec(
    SOC_SAND_IN     uint32 delay_in_milisec
  );
/*
 * This routine returns the system clock rate.
 * The number of ticks per second of the system clock.
 */
int
  soc_sand_os_get_ticks_per_sec(
    void
  );
/*
 * most rt OS can suspend a task for several nano seconds
 */
int
  soc_sand_os_nano_sleep(
    uint32     nano,
    uint32     *premature
  );
/*
 * number of ticks passed since system start up
 */
SOC_SAND_RET
  soc_sand_os_tick_get(
    SOC_SAND_INOUT     uint32 *ticks
  );
/*
 * number of seconds (and nano seconds, withinn the last second)
 * passed since start up
 */
SOC_SAND_RET
  soc_sand_os_get_time(
    SOC_SAND_INOUT     uint32 *seconds,
    SOC_SAND_INOUT     uint32 *nano_seconds
  );

/*
 *	Get time difference. 
 *  Assumes start_nano_seconds and end_nano_seconds
 *  are less then 1 second.
 *  Assumes that end_seconds >= start_seconds
 */
SOC_SAND_RET
  soc_sand_os_get_time_diff(
    SOC_SAND_IN     uint32 start_seconds,
    SOC_SAND_IN     uint32 start_nano_seconds,
    SOC_SAND_IN     uint32 end_seconds,
    SOC_SAND_IN     uint32 end_nano_seconds,
    SOC_SAND_OUT    uint32 *diff_seconds,
    SOC_SAND_OUT    uint32 *diff_nano_seconds
  );
uint32
  soc_sand_os_get_time_micro(
    void
  );

/*
 * returns the number of system ticks in 1 second
 */
uint32
  soc_sand_os_sys_clk_rate_get(
    void
  );


/*
 * }
 */


/*
 * {
 * Semaphores
 */

/*
 * returns the sem_id to be used by the other procs
 */
sal_mutex_t
  soc_sand_os_mutex_create(
    void
  );
/*
 * deletes a mutex
 */
SOC_SAND_RET
  soc_sand_os_mutex_delete(
    SOC_SAND_IN     sal_mutex_t          sem_id
  );
/*
 * timeout might be SOC_SAND_INFINITE_TIMEOUT
 */
SOC_SAND_RET
  soc_sand_os_mutex_take(
    SOC_SAND_IN     sal_mutex_t          sem_id,
    SOC_SAND_IN     long          timeout
  );
/*
 * release a mutex
 */
SOC_SAND_RET
  soc_sand_os_mutex_give(
    SOC_SAND_IN     sal_mutex_t          sem_id
  );

/*
 * }
 */

/*
 * {
 * Level control
 */

/*
 * disable interrupts
 */
SOC_SAND_RET
  soc_sand_os_stop_interrupts(
    uint32 *flags
  );
/*
 * resotore interrupts
 */
SOC_SAND_RET
  soc_sand_os_start_interrupts(
    uint32 flags
  );

/*
 * }
 */

/*
 * {
 * Message queues
 */

/*
 * returns MSG_Q_ID used by the other procs
 */
void
  *soc_sand_os_msg_q_create(
    int max_msgs,
    int max_msglength
    ) ;
/*
 * deletes a message queue
 */
SOC_SAND_RET
  soc_sand_os_msg_q_delete(
    void *msg_q_id
    );
/*
 * sends a message to a message queue
 */
SOC_SAND_RET
  soc_sand_os_msg_q_send(
    void         *msg_q_id,
    char         *data,
    uint32 data_size,
    int          timeout,
    int          priority
  ) ;
/*
 * return = 0 => got a message
 * return > 0 => time out
 * return < 0 => error
 * timeout might be SOC_SAND_INFINITE_TIMEOUT
 */
int
  soc_sand_os_msg_q_recv(
    void          *msg_q_id,
    unsigned char *data,
    uint32  max_nbytes,
    long          timeout_in_ticks
    ) ;
int
  soc_sand_os_msg_q_num_msgs(
    void          *msg_q_id
    ) ;

/*
 * }
 */

/*
 * {
 * STDIO and String - only for error/debug/printing
 */


/*
 * determine the length of a string (ANSI)
 */
uint32
  soc_sand_os_strlen(
    const char* s /* string */
  );

/*
 * compare N chars
 */
int
  soc_sand_os_strncmp(
    const char *string1,
    const char *string2,
    uint32 count
  );

/*
 * copy a string from source to destination
 */
SOC_SAND_RET
  soc_sand_os_strncpy(
    char          *s1, /* string to copy to */
    const char    *s2, /* string to copy from */
    uint32 n   /* max no. of characters to copy */
  );

/* {  Random function. */
/*
 * The srand function sets the starting point for generating
 *  a series of pseudorandom integers
 */
void
  soc_sand_os_srand(
    uint32
  );
/*
 * Generates a pseudo-random integer between 0 and RAND_MAX
 */
uint32
  soc_sand_os_rand(
    void
  );
/* }  END Random function. */
/* {  IO printing functions. */
/*
 * printf
 */
int
  soc_sand_os_printf(
    SOC_SAND_IN char* format, ...
  ) SOC_SAND_PRINTF_ATTRIBUTE(1, 2);
/* } END IO printing functions. */

/*
 * }
 */


#ifdef SOC_SAND_DEBUG
/* { */

/*
 * Report on the taken resources.
 */
SOC_SAND_RET
  soc_sand_os_resource_print(
    void
  );

/* } */
#endif


#ifdef  __cplusplus
}
#endif


/*
 * }
 */
#endif

