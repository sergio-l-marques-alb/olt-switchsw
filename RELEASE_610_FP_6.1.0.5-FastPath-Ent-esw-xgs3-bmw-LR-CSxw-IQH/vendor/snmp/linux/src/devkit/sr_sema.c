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




#include <errno.h>

#include "sr_snmp.h"

#include "diag.h"


SR_FILENAME

#ifdef SR_EPIC
#include "epic.h"
#endif /* SR_EPIC */

#include "sr_sema.h"

/*
 * sr_sem_init:
 *
 * This routine creates a sr_sem_t structure and fills in the structure based
 * on what platform it is running on.
 *
 * Returns:  success - a pointer to a sr_sem_t structure to be used in calls
 *                     to the other semaphore routines.
 *
 */
sr_sem_t *
sr_sem_init()
{
    sr_sem_t *semaphore_ptr = NULL;
    FNAME("sr_sem_init")

    semaphore_ptr = (sr_sem_t *)malloc(sizeof(sr_sem_t));
    if (semaphore_ptr == NULL) {
        DPRINTF((APERROR, "%s: malloc of semaphore_ptr failed.\n", Fname));
        return NULL;
    }
#ifdef SR_CLEAR_MALLOC
    memset(semaphore_ptr, 0x00, sizeof(sr_sem_t));
#endif /* SR_CLEAR_MALLOC */
    memset(semaphore_ptr, 0x00, sizeof(sr_sem_t));  /* Quiet Purify */

#ifdef SR_SEMAPHORE_OS
    semaphore_ptr->semaphore = (sem_t *)malloc(sizeof(sem_t));
    if (semaphore_ptr->semaphore == NULL) {
        DPRINTF((APERROR, "%s: malloc of semaphore_ptr failed.\n", Fname));
        free(semaphore_ptr);
        semaphore_ptr = NULL;
        return NULL;
    }
#ifdef SR_CLEAR_MALLOC
    memset(semaphore_ptr->semaphore, 0x00, sizeof(sem_t));
#endif /* SR_CLEAR_MALLOC */
    memset(semaphore_ptr->semaphore, 0x00, sizeof(sem_t));  /* Quiet Purify */
   
    if ((sem_init(semaphore_ptr->semaphore, 0, 1)) != 0) {
        DPRINTF((APERROR, "%s: sem_init failed with %s.\n", Fname,
                 sys_errname(errno)));
        free(semaphore_ptr->semaphore);
        free(semaphore_ptr);
        semaphore_ptr = NULL;
        return NULL;
    }


#endif /* SR_SEMAPHORE_OS */
    return(semaphore_ptr);
}

/* 
 * sr_sem_wait:
 *
 * This routine takes the given semaphore, semaphore_ptr, and calls the
 * semaphore "wait" routine. This routine should not return until the semaphore
 * is available for use.
 *
 * Returns: success - 0
 *          failure - -1
 *
 */
int
sr_sem_wait(sr_sem_t *semaphore_ptr)
{
    SR_INT32 	return_val;
#ifdef SR_SEMAPHORE_OS
    FNAME("sr_sem_wait")
#endif /* SR_SEMAPHORE_OS */

    return_val = 0;

#ifdef SR_SEMAPHORE_OS
    if (semaphore_ptr == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore_ptr pointer is NULL.\n", 
                 Fname));
        return(-1);
    } 
    if (semaphore_ptr->semaphore == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore pointer is NULL.\n", Fname));
        return(-1);
    }
    return_val = sem_wait(semaphore_ptr->semaphore);
    if (return_val == -1) {
        DPRINTF((APERROR, "%s: sem_wait returned error: %s\n", Fname,
                 sys_errname(errno)));
    }
#endif /* SR_SEMAPHORE_OS */
    return(return_val);
}

/* 
 * sr_sem_trywait:
 *
 * This routine takes the given semaphore, semaphore_ptr, and calls the
 * semaphore "trywait" routine. This routine will return with the semaphore
 * available for use or with an error indicating that another thread is
 * already using the semaphore (or with some other error).
 *
 * Returns: success - 0
 *          failure - -1
 *
 */
int
sr_sem_trywait(sr_sem_t *semaphore_ptr)
{
    SR_INT32 	return_val;
#ifdef SR_SEMAPHORE_OS
    FNAME("sr_sem_trywait")
#endif /* SR_SEMAPHORE_OS */

    return_val = 0;

#ifdef SR_SEMAPHORE_OS
    if (semaphore_ptr == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore_ptr pointer is NULL.\n", 
                 Fname));
        return(-1);
    } 
    if (semaphore_ptr->semaphore == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore pointer is NULL.\n", Fname));
        return(-1);
    }

    return_val = sem_trywait(semaphore_ptr->semaphore);

    if ( (return_val == -1) && (errno != EAGAIN) ) {
        DPRINTF((APERROR, "%s: sem_trywait returned error: %s\n", Fname,
                 sys_errname(errno)));
    }
#endif /* SR_SEMAPHORE_OS */
    return(return_val);
}

/* 
 * sr_sem_post:
 *
 * This routine takes the given semaphore, semaphore_ptr, and calls the
 * semaphore "post" routine. This should allow another thread to access the
 * semaphore.
 *
 * Returns: success - 0
 *          failure - -1
 *
 */
int
sr_sem_post(sr_sem_t *semaphore_ptr)
{
    SR_INT32 	return_val;
#ifdef SR_SEMAPHORE_OS
    FNAME("sr_sem_post")
#endif /* SR_SEMAPHORE_OS */

    return_val = 0;

#ifdef SR_SEMAPHORE_OS
    if (semaphore_ptr == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore_ptr pointer is NULL.\n", 
                 Fname));
        return(-1);
    } 
    if (semaphore_ptr->semaphore == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore pointer is NULL.\n", Fname));
        return(-1);
    }
    return_val = sem_post(semaphore_ptr->semaphore);
    if (return_val == -1) {
        DPRINTF((APERROR, "%s: sem_post returned error: %s\n", Fname,
                 sys_errname(errno)));
    }
#endif /* SR_SEMAPHORE_OS */
    return(return_val);
}

/* 
 * sr_sem_destroy:
 *
 * This routine will delete the given semaphore, semaphore_ptr, and then free
 * the structure and return.
 *
 * Returns: success -  0
 *          failure - -1
 *
 */
int
sr_sem_destroy(sr_sem_t *semaphore_ptr)
{
    SR_INT32	return_val;
#ifdef SR_SEMAPHORE_OS
#endif /* SR_SEMAPHORE_OS */
    FNAME("sr_sem_destroy")

    return_val = 0;

    if (semaphore_ptr == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore_ptr pointer is NULL.\n", 
                 Fname));
        return -1;
    } 
#ifdef SR_SEMAPHORE_OS
    if (semaphore_ptr->semaphore == NULL) {
        DPRINTF((APTRACE, "%s: Incoming semaphore pointer is NULL.\n", Fname));
        return_val = -1;
        goto done;
    } 
 
    return_val = sem_destroy(semaphore_ptr->semaphore);

    if (return_val == -1) {
        DPRINTF((APERROR, "%s: sem_destroy returned an error: %s.\n", 
                 Fname, sys_errname(errno)));
    }
    free(semaphore_ptr->semaphore);
done:
#endif /* SR_SEMAPHORE_OS */
    free(semaphore_ptr);
    semaphore_ptr = NULL;
    return return_val;
}
