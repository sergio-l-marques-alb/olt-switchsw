/*
 *
 * Copyright (C) 1992-2006 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#include "sr_conf.h"


#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <ctype.h>

#include <string.h>

#include <signal.h>




#include <errno.h>

#include "sr_snmp.h"

#include "diag.h"


SR_FILENAME

#include <pthread.h>
#include <sched.h>

#include "sr_mutex.h"


/*
 * sr_mutex_init:
 *
 * This routine creates a sr_mutex_t structure and fills in the structure based
 * on what platform it is running on.
 *
 * Returns:  success - a pointer to a sr_mutex_t structure to be used in calls
 *                     to the other mutex routines.
 *
 */
sr_mutex_t *
sr_mutex_init(char *descString)
{
    sr_mutex_t *mutex_ptr = NULL;
    FNAME("sr_mutex_init")

    if (descString != NULL) {
        mutex_ptr = (sr_mutex_t *)malloc(sizeof(sr_mutex_t) + strlen(descString)+ 2);
        if (mutex_ptr != NULL) {

#ifdef SR_CLEAR_MALLOC
            memset(mutex_ptr, 0x00, (sizeof(sr_mutex_t) + strlen(descString)+ 2));
#endif /* SR_CLEAR_MALLOC */
            memset(mutex_ptr, 0x00, (sizeof(sr_mutex_t) + strlen(descString)+ 2));  /* Quiet Purify */

            mutex_ptr->descString = (char *) (((char *)mutex_ptr) + sizeof(sr_mutex_t));
            strcpy(mutex_ptr->descString, descString);
        }
    }
    else {
        mutex_ptr = (sr_mutex_t *)malloc(sizeof(sr_mutex_t) + 2);
        if (mutex_ptr != NULL) {

#ifdef SR_CLEAR_MALLOC
            memset(mutex_ptr, 0x00, (sizeof(sr_mutex_t) + 2));
#endif /* SR_CLEAR_MALLOC */
            memset(mutex_ptr, 0x00, (sizeof(sr_mutex_t) + 2));  /* Quiet Purify */

            mutex_ptr->descString = (char *) (((char *)mutex_ptr) + sizeof(sr_mutex_t));
            strcpy(mutex_ptr->descString, "");
        }
    }
    if (mutex_ptr == NULL) {
        DPRINTF((APERROR, "%s: malloc of mutex_ptr failed.\n", Fname));
        return NULL;
    }
    return(mutex_ptr);
}

/* 
 * sr_mutex_lock:
 *
 * This routine takes the given mutex, mutex_ptr, and calls the
 * mutex "lock" routine. This routine should not return until the mutex
 * is available for use.
 *
 * Returns: success - 0
 *          failure - errno value or -1.
 *
 */
int
sr_mutex_lock(sr_mutex_t *mutex_ptr)
{
    SR_INT32 	return_val;

    return_val = 0;

    return(return_val);
}

/* 
 * sr_mutex_trylock:
 *
 * This routine takes the given mutex, mutex_ptr, and calls the
 * mutex "trylock" routine. This routine will return with the mutex
 * available for use or with an error indicating that another thread is
 * already using the mutex (or with some other error).
 *
 * Returns: success - 0
 *          failure - -1 or errno
 *
 */
int
sr_mutex_trylock(sr_mutex_t *mutex_ptr)
{
    SR_INT32 	return_val;

    return_val = 0;

    return(return_val);
}

/* 
 * sr_mutex_unlock:
 *
 * This routine takes the given mutex, mutex_ptr, and calls the
 * mutex "post" routine. This should allow another thread to access the
 * mutex.
 *
 * Returns: success - 0
 *          failure - -1 or errno
 *
 */
int
sr_mutex_unlock(sr_mutex_t *mutex_ptr)
{
    SR_INT32 	return_val;

    return_val = 0;

    return(return_val);
}

/* 
 * sr_mutex_free:
 *
 * This routine will free the given mutex, mutex_ptr, and then free
 * the structure and return.
 *
 * Returns: success -  0
 *          failure - -1 or errno
 *
 */
int
sr_mutex_free(sr_mutex_t *mutex_ptr)
{
    SR_INT32	return_val;
    FNAME("sr_mutex_free")

    return_val = 0;

    if (mutex_ptr == NULL) {
        DPRINTF((APTRACE, "%s: Incoming mutex_ptr pointer is NULL.\n", 
                 Fname));
        return -1;
    } 
    free(mutex_ptr);
    mutex_ptr = NULL;
    return return_val;
}

/*
 * sr_sigsend_to_thread
 *
 * This routine takes the given threadId and sig number, and calls the
 * pthread_kill() routine to send a signal to the specified thread.
 * 
 * Returns: success -  0
 *          failure -  errno
 *
 */

int
sr_sigsend_to_thread(SR_INT32 Id, int which_sig)
{
    SR_INT32 return_val;

    return_val = 0;


    return return_val;
}

/*
 * sr_pthread_yield
 *
 * This routine yields the processor from the currently executing thread 
 * to another ready to run, active thread of equal or higher priority.
 *
 * Return: success - 0
 *         failure - -1 or errno
 */

int
sr_thread_yield()
{
    SR_INT32 return_val;

    return_val = 0;


    return return_val;
}

