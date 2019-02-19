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

#include "sr_mutex.h"
#include "sr_cond.h"
extern SR_INT32 MAX_NUM_COND;

/*
 * sr_cond_init:
 *
 * This routine takes the given threadID and initializes the conditional
 * variable for the given threadID.
 *
 * Returns: success - sr_cond_t *
 *          failure - NULL
 */

sr_cond_t *
sr_cond_init(SR_INT32 tid)
{
    sr_cond_t *cond_ptr;
    FNAME("sr_cond_init");


    cond_ptr = (sr_cond_t *)malloc(sizeof(sr_cond_t));
    if (cond_ptr == NULL) {
	DPRINTF((APERROR, "%s: malloc failed on sr_cond_t\n", Fname));
	return NULL;
    }

#ifdef SR_CLEAR_MALLOC
    memset(cond_ptr, 0x00, sizeof(sr_cond_t));
#endif /* SR_CLEAR_MALLOC */
    memset(cond_ptr, 0x00, sizeof(sr_cond_t));  /* Quiet Purify */

    cond_ptr->tid = tid;

    return cond_ptr;
}

/*
 * sr_cond_destroy:
 *
 * This routine takes the given sr_cond_t structure and destroy the 
 * conditional variable and sr_cond_t structure.
 *
 * Returns: success - 0
 *          failure - errno value other than 0
 */
int
sr_cond_destroy(sr_cond_t *cond_ptr)
{
    SR_INT32 return_val;

    return_val = 0;

    return return_val;
}

/*
 * sr_cond_wait:
 *
 * This routine takes the given r_cond_t structure and mutex_ptr , then calls 
 * pthread_cond_wait() to wait for this thread's condition variable to be 
 * signaled. Calling pthread_cond_wait() causes the mutex to be released and 
 * causes the calling thread to wait for it's specific condition variable 
 * to be signaled. The mutex is reacquired when pthread_cond_wait() returns.
 *
 * for Windows: we implement our own conditinal variable scheme which is 
 * equavalent to pthread_cond_wait().
 *
 * Returns: success - 0
 *          failure - errno value other than 0
 *                    linux & solaris > 0
 *                    hpux  = -1 with errno
 *                    windows -1
 */
int 
sr_cond_wait(sr_cond_t *cond_ptr, sr_mutex_t *mutex_ptr )
{
    SR_INT32 return_val;

    return_val = 0;


   return return_val;
}

/*
 * sr_cond_timedwait:
 *
 * This routine takes the given sr_cond_t structure, mutex_ptr and 
 * timeout value, then call pthread_cond_timedwait() to wait for 
 * this thread's condition variable to be signaled or timedout. 
 * Calling pthread_cond_timedwait() causes the mutex to be released and 
 * causes the calling thread to wait for it's specific condition variable 
 * to be signaled or timed out. The mutex is reacquired when 
 * pthread_cond_timedwait() returns.
 *
 * Returns: success - 0
 *          failure - errno value other than 0
 *                    linux & solaris > 0
 *                    hpux  = -1 with errno
 */
int 
sr_cond_timedwait(sr_cond_t *cond_ptr, 
		  sr_mutex_t *mutex_ptr, 
                  SR_UINT32 secs,
                  SR_UINT32 nsecs)
{
    SR_INT32 return_val;

    return_val = 0;


   return return_val;
}

/*
 * sr_cond_signal:
 *
 * This routine takes the given threadID and set a condition signal to 
 * wake up a thread blocked by "sr_cond_wait()" call.
 * 
 * Returns: success -0
 *          failure - errno value other than 0
 *                    linux & solaris > 0
 *                    hpux  = -1 with errno
 */

int
sr_cond_signal(sr_cond_t * cond_ptr)
{
    SR_INT32 return_val;

    return_val = 0;


   return return_val;

}

/*
 * sr_cond_broadcast:
 *
 * This routine set a cond_broadcast to 
 * wake up all threads blocked by "sr_cond_wait()" call.
 * unblocks all threads currently blocked on the specified condition 
 * variable cond.
 * 
 * Returns: success -0
 *          failure - -1
 */

int
sr_cond_broadcast(sr_cond_t *cond_ptr)
{


   return (0);

}
