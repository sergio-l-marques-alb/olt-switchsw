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

#include <sys/types.h>

#include "sr_type.h"


#include "sr_time.h"

#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "tmq.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "rowstatf.h"
#include "diag.h"
SR_FILENAME


/* this routine monitors transitions in the RowStauts textual convention
 * for sanity.  It returns 0 if no error, an error status otherwise */
int
CheckRowStatus(newstatus, oldstatus)
SR_INT32 newstatus;
SR_INT32 oldstatus;
{
    /* if no change, and not trying to set to create-request more than once,
     * continue checking */
    if(newstatus == RS_NOT_READY) return(WRONG_VALUE_ERROR);
    if(newstatus != oldstatus) {
	/* only allow creation of nonexistent rows */
	if((newstatus == RS_CREATE_AND_GO || newstatus == RS_CREATE_AND_WAIT)
	&& oldstatus != RS_UNINITIALIZED) {
	    return(INCONSISTENT_VALUE_ERROR);
	}

	/* only allow transitions to active and notInService from 
	 * completed rows */
	if((newstatus == RS_ACTIVE || newstatus == RS_NOT_IN_SERVICE)
	&& oldstatus == RS_UNINITIALIZED) {
	    /* note that transitions to active from notReady must be
	     * checked by the appropriate method routine */
	    return(INCONSISTENT_VALUE_ERROR);
	}
    }

    /* return a good status */
    return(0);
}

/* this routine frees data associated with a rowstatus timer */
int
FreeRowStatusTimerInfo(id, tifree)
    SR_INT32            id;
    RowStatusTimerInfoFreeProc tifree;
{
    TimerQueue     *myTimerQp;
    TimerEvent     *p;
#ifdef SR_DEBUG
    char *Fname = "FreeRowStatusTimerInfo";
#endif /* SR_DEBUG */
    TimeOutDescriptor *tod;

    myTimerQp = GetTimerQueue();
    for (p = myTimerQp->head->next; p != myTimerQp->tail; p = p->next) {
	if (p->id == id) {
	    break;
	}
    }
    if (p == myTimerQp->tail) {
	DPRINTF((APWARN, "%s: unknown timeout: %d\n", Fname, id));
	return -1;
    }
    tod = (TimeOutDescriptor *)p->info;
    if (tod->UserData1 == 1) {
        cidata_t *ud2 = (cidata_t *)tod->UserData2;
        FreeContextInfo(ud2->cip);
        free(ud2);
    }
    (*tifree) (tod);
     
    /* set TimeOutDescriptor to NULL so it won't be freed again
     * when this timer event is freed 
     */
    p->info = NULL;
    return 1;
}

/* cancel a timer event and free its associated data */
void
CancelRowStatusTimeout(id)
SR_INT32           id;
{
    DPRINTF((APTRACE, "CancelTimeoutWithInfo: cancelling timer %d\n", id));
    FreeRowStatusTimerInfo(id, (RowStatusTimerInfoFreeProc)free);
    CancelTimeout((int) id);
}

/* this routine creates a one-shot timeout given information about timeout
 * parameters and the interval.  It returns the timer id, or -1 if an error. */
SR_INT32
SetRowStatusTimeout(delay, data, callback)
    SR_INT32 delay;
    void *data;
    TodCallbackFunction callback;
{
    TimeOutDescriptor *TimeOut;
    SR_INT32 id;

    /* allocate the timeout descriptor */
    if((TimeOut = (TimeOutDescriptor *) malloc(sizeof(TimeOutDescriptor)))
    == NULL) {
	return(-1);
    }

    /* make sure we can set the timeout, if not free memory just allocated */
    if(CanSetTimeout() == (-1)) {
      free(TimeOut);   
      return(-1);
    }

    /* set up the timeout parameters */
    TimeOut->TimeOutType = 0;  /* Prevent Purify UMRs with EMANATE and ARL */
    TimeOut->CallBack = callback;
    TimeOut->UserData1 = 0;
    TimeOut->UserData2 = data;

    /* make the timeout relative to the current time */
    delay += GetTimeNow();

    /* install the timeout */
    id = SetOneshotTimeout((SR_UINT32) delay, (void *) TimeOut);
    DPRINTF((APTRACE, "SetRowStatusTimeout: id is %d\n", id));
    return((SR_INT32) id);
}

/* 
 * this routine creates a one-shot timeout given information about timeout
 * parameters and the interval.  It returns the timer id, or -1 if an error. 
 * the call back function will be called automatically.
 */

SR_INT32
SrSetRowStatusTimeout(delay, data, callback)
    SR_INT32 delay;
    void *data;
    SrTodCallbackFunction callback;
{
    SR_INT32 id;

    /* make sure we can set the timeout, if not free memory just allocated */
    if(CanSetTimeout() == (-1)) {
      return(-1);
    }

    /* make the timeout relative to the current time */
    delay += GetTimeNow();

    /* install the timeout */
    id = SrSetOneshotTimeout((SR_UINT32) delay, 0, (void *)callback, 0, data);
    DPRINTF((APTRACE, "SetRowStatusTimeout: id is %d\n", id));
    return((SR_INT32) id);
}

/* this routine creates a one-shot timeout given information about timeout
 * parameters and the interval.  It returns the timer id, or -1 if an error. */
SR_INT32
SetRowStatusTimeoutWithCI(
    SR_INT32 delay,
    ContextInfo *cip,
    void *data,
    TodCallbackFunction callback)
{
    TimeOutDescriptor *TimeOut;
    SR_INT32 id;
    cidata_t *ud2;

    /* make sure we can set the timeout */
    if(CanSetTimeout() == (-1)) return(-1);

    /* allocate the timeout descriptor */
    TimeOut = (TimeOutDescriptor *) malloc(sizeof(TimeOutDescriptor));
    if (TimeOut == NULL) {
	return(-1);
    }

    ud2 = (cidata_t *)malloc(sizeof(cidata_t));
    if (ud2 == NULL) {
        free(TimeOut);
        return -1;
    }

    ud2->cip = CloneContextInfo(cip);

    /* set up the timeout parameters */
    TimeOut->CallBack = callback;
    TimeOut->UserData1 = 1;
    TimeOut->UserData2 = ud2;
    ud2->data = data;

    /* make the timeout relative to the current time */
    delay += GetTimeNow();

    /* install the timeout */
    id = SetOneshotTimeout((SR_UINT32) delay, (void *) TimeOut);
    DPRINTF((APTRACE, "SetRowStatusTimeoutWithCI: id is %d\n", id));
    return((SR_INT32) id);
}

/* 
 * this routine creates a one-shot timeout given information about timeout
 * parameters and the interval.  It returns the timer id, or -1 if an error. 
 * the call back function will be called automatically.
 */
SR_INT32
SrSetRowStatusTimeoutWithCI(
    SR_INT32 delay,
    ContextInfo *cip,
    void *data,
    SrTodCallbackFunction callback)
{
    SR_INT32 id;
    cidata_t *ud2;

    /* make sure we can set the timeout */
    if(CanSetTimeout() == (-1)) return(-1);

    ud2 = (cidata_t *)malloc(sizeof(cidata_t));
    if (ud2 == NULL) {
        return -1;
    }

    ud2->cip = CloneContextInfo(cip);

    ud2->data = data;

    /* make the timeout relative to the current time */
    delay += GetTimeNow();

    /* install the timeout */
    id = SrSetOneshotTimeout((SR_UINT32) delay, 0, 
                           (void *) callback, 0, (void *)ud2);
    DPRINTF((APTRACE, "SetRowStatusTimeoutWithCI: id is %d\n", id));
    return((SR_INT32) id);
}
