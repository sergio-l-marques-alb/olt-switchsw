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



#ifdef SR_CLEAR_MALLOC
#include <string.h>
#endif /* SR_CLEAR_MALLOC */

#include "sr_snmp.h"

#include <pthread.h>

#include "sr_mutex.h"
#include "sri/sr_wbuf.h"
#include "diag.h"
SR_FILENAME



/* Globals */
static SR_INT32 defaultWriteBufLengths[] = {100, 512, 4096, -1};


/*
 * "Frees" first element in the pending write buffer
 * by clearing it out and putting it on the appropriate freed Buffer list.
 */
static SR_INT32 WBufElmtFree(writeBuffer *wb);

/*
 * Gets a writeBufElmt structure from either the "freed" list or newly
 * mallocs one for the given buffer and length specified.
 *
 * Returns NULL on failure and a pointer to the write buffer on success.
 *
 */
static writeBufElmt *WBufElmtGetNew(writeBuffer *wb, char *buf, SR_INT32 len);

/* 
 * Adds given writeBufElmt structure to the list of outstanding
 * buffers.
 *
 * Returns -1 on failure and 0 on success.
 *
 */
static SR_INT32 WBufElmtAdd(writeBuffer *wb, writeBufElmt *writeBuf);

static SR_INT32 WbufRemoveElmt(writeBuffer *wb, writeBufElmt *curWriteBuf)
{
    SR_INT32 returnVal = -1;
    SR_INT32 bufferSize;
    freeBufElmt *curFreeBuf;
    /* FNAME("WbufRemoveElmt") */

    bufferSize = curWriteBuf->length;
    curWriteBuf->next_ptr = NULL;
#ifdef SR_CLEAR_MALLOC
    memset(&curWriteBuf->buf, 0, curWriteBuf->length);
#endif /* SR_CLEAR_MALLOC */
    curWriteBuf->lengthUsed = 0;
    curWriteBuf->freeFlag = FALSE;

    /*
     * Add write buffer to appropriate free buffer list
     * Buffer sizes are in increasing order.
     */
    for (curFreeBuf = wb->freeBufInfo; curFreeBuf != NULL;
        curFreeBuf = curFreeBuf->next_ptr) {
        /* Found correct free buffer list */
        if (curFreeBuf->bufferSize == curWriteBuf->length) {
            /* No elements in free list */
            if (curFreeBuf->freeBufHead == NULL) {
                curFreeBuf->freeBufHead = curWriteBuf;
                curFreeBuf->freeBufTail = curWriteBuf;
            }
            /* >= 1 element in free list */
            else {
                curFreeBuf->freeBufTail->next_ptr = curWriteBuf;
                curFreeBuf->freeBufTail = curWriteBuf;
            }
            break;
        }
    }
    if (curFreeBuf != NULL) {
        returnVal = 0;
    }
    else {
        returnVal = -1;
    }
    return returnVal;
}

/*
 * WBufElmtFree:
 *
 * Takes element currently at head of pending write buffers and "frees" it
 * by clearing it out and putting it on the appropriate freed Buffer list.
 *
 * Returns -1 on failure and 0 on success.
 */
static SR_INT32
WBufElmtFree(writeBuffer *wb)
{
    SR_INT32 returnVal = -1;
    writeBufElmt *curWriteBuf;
    /* FNAME("WBufElmtFree") */

    if (wb == NULL) {
        returnVal = -1;  
        goto done;
    }

    if (wb->writeBufHead != NULL) {
        if (wb->writeBufHead->freeFlag == TRUE) {
            curWriteBuf = wb->writeBufHead;

            /* Remove buffer from pending write buffer list */

            /* Only one element in current write buffer list */
            if (wb->writeBufHead->next_ptr == NULL) {
                wb->writeBufHead = NULL;
                wb->writeBufTail = NULL;
            }
            else {
                wb->writeBufHead = curWriteBuf->next_ptr;
            }

            /* "Free" element */
            returnVal = WbufRemoveElmt(wb, curWriteBuf);
        }
    }

done:
    return returnVal;
}

/*
 * WBufElmtGetNew:
 *
 * Gets a writeBufElmt structure from either the "freed" list or newly
 * mallocs one for the given buffer and length specified if none are available.
 *
 * Returns NULL on failure and a pointer to the structure on success.
 *
 */
static writeBufElmt *
WBufElmtGetNew(writeBuffer *wb, char *buf, SR_INT32 len)
{
    writeBufElmt *newWBufElmt = NULL;
    freeBufElmt *curFreeBuf = NULL;
    SR_INT32 newBufSize;
    SR_INT32 maxBufSize;
    SR_INT32 newBufLimit = -1;
    FNAME("WBufElmtGetNew")

    if ( (wb == NULL) || (buf == NULL) ) {
        goto fail;
    }

    /*
     * Look through the proper free buffer list to see if we have any
     * freed elements to use.
     */
    for (curFreeBuf = wb->freeBufInfo; curFreeBuf != NULL; 
         curFreeBuf = curFreeBuf->next_ptr) {
        maxBufSize = curFreeBuf->bufferSize;
        /* Found right set of buffers. Check for free ones to use. */
        if (len <= curFreeBuf->bufferSize) {
            newBufLimit = curFreeBuf->bufferSize;
            /* We have a free buffer to use, remove from the list. */
            if (curFreeBuf->freeBufHead != NULL) {
                newWBufElmt = curFreeBuf->freeBufHead;
                /* Only one element on list */
                if (curFreeBuf->freeBufHead == curFreeBuf->freeBufTail) {
                    curFreeBuf->freeBufHead = NULL;
                    curFreeBuf->freeBufTail = NULL;
                }
                /* >= one element on list */
                else {
                    curFreeBuf->freeBufHead = newWBufElmt->next_ptr;
                }
            }    
            break;
        }
    }

    /*
     * No free buffers to use. Malloc new one. Base size of buffer on matching
     * buffer size in free list instead of actual size of incoming buffer.
     */
    if (newWBufElmt == NULL) {
        /*
         * If newBufLimit is -1, the length of the incoming buffer is bigger
         * than any possible buffer size we support, so return error.
         */
        if (newBufLimit == -1) {
            DPRINTF((APERROR,
                     "%s: Cannot malloc buffer of size %d because it exceeds our maximum buffer size of %d.\n",
                     Fname, len, maxBufSize));
            goto fail;
        }
        newBufSize = newBufLimit + sizeof(writeBufElmt);
        if ((wb->totalBufSize + newBufSize) > wb->maxBufSize) {
            DPRINTF((APERROR,
                     "%s: Cannot malloc new buffer, would exceed maximum size, %d, allowed for buffers.\n",
                     Fname, wb->maxBufSize));
            goto fail;
        }
        newWBufElmt = (writeBufElmt *)malloc(newBufSize);
        if (newWBufElmt == NULL) {
            DPRINTF((APERROR, "%s: Cannot malloc new buffer.\n", Fname));
            goto fail;
        }
        wb->totalBufSize += newBufSize;
    }

    /* Set values for new write buffer element */
    newWBufElmt->length = newBufLimit;
    newWBufElmt->lengthUsed = len;
    newWBufElmt->freeFlag = FALSE;
    newWBufElmt->next_ptr = NULL;
#ifdef SR_CLEAR_MALLOC
    memset(&newWBufElmt->buf, 0, newBufLimit);
#endif /* SR_CLEAR_MALLOC */
    memcpy(&newWBufElmt->buf, buf, len);

    return newWBufElmt;

fail:
    return NULL;
}

/* 
 * WBufElmtAdd:
 *
 * Adds given writeBufElmt structure to the list of outstanding
 * buffers.
 *
 * Returns -1 on failure and 0 on success.
 *
 */
static SR_INT32
WBufElmtAdd(writeBuffer *wb, writeBufElmt *writeBuf)
{
    if ( (wb == NULL) || (writeBuf == NULL) ) {
        return -1;
    }

    /* If no elements in write buffer list, put in first element */
    if (wb->writeBufHead == NULL) {
        writeBuf->next_ptr = NULL;
        wb->writeBufHead = writeBuf;
        wb->writeBufTail = writeBuf;
    } 
    /* If there are one or more elements in the list */
    else {
        wb->writeBufTail->next_ptr = writeBuf;
        wb->writeBufTail = writeBuf;
        wb->writeBufTail->next_ptr = NULL;
    }

    return 0;
}

/*
 * WBufInit:
 *
 * Sets up writeBuffer structure, sets max. available memory to "size".
 *
 * Set up free buffer linked list, one for each buffer size. Incoming
 * parameter, bufsizes, must have buffer sizes listed in increasing values.
 *
 * Returns structure on success, NULL on failure.
 */
writeBuffer *
WBufInit(SR_INT32 size, SR_INT32 *bufsizes, SR_INT32 threadSafeFlag)
{
    SR_INT32 numBuffers, pos, wBufSize, prevBufSize = -1;
    writeBuffer *newWBuf = NULL;
    freeBufElmt *curFreeBufElmt = NULL;
    freeBufElmt *newFreeBufElmt;
    FNAME("WBufInit")

    numBuffers = 0;

    if (size <= 0) {
        return NULL;
    }
    if (bufsizes == NULL) {
        bufsizes = defaultWriteBufLengths;   
    }
    for (pos = 0; bufsizes[pos] != -1; pos++) {
        numBuffers++;
        if (bufsizes[pos] <= prevBufSize) {
            DPRINTF((APERROR,
                     "%s: Buffer sizes must increase in size.", Fname));
            DPRINTF((APERROR,
                     " Buffer size %d is <= previous buffer size, %d.\n",
                     bufsizes[pos], prevBufSize));
            goto fail;
        }
        prevBufSize = bufsizes[pos];            
    }

    wBufSize = sizeof(writeBuffer);

    newWBuf = (writeBuffer *) malloc(wBufSize);

    if (newWBuf == NULL) {
        DPRINTF((APERROR, "%s: Could not malloc writeBuffer.\n", Fname));
        goto fail;
    }    

    newWBuf->writeBufHead = NULL;
    newWBuf->writeBufTail = NULL;
    newWBuf->totalBufSize = 0;
    newWBuf->maxBufSize = size;
    newWBuf->numBuffers = numBuffers;
    newWBuf->writeBufMutex = NULL;
    newWBuf->freeBufInfo = NULL;

    for (pos = 0; pos < numBuffers; pos++) {
        newFreeBufElmt = (freeBufElmt *)malloc(sizeof(freeBufElmt));
        if (newFreeBufElmt == NULL) {
            DPRINTF((APERROR, "%s: Could not malloc freeBufElmt.\n", Fname));
            goto fail;
        }    
        newFreeBufElmt->bufferSize = bufsizes[pos];
        newFreeBufElmt->freeBufHead = NULL; 
        newFreeBufElmt->freeBufTail = NULL; 
        newFreeBufElmt->next_ptr = NULL; 

        /* First element? */
        if (newWBuf->freeBufInfo == NULL) {
            newWBuf->freeBufInfo = newFreeBufElmt;
        }  
        if (curFreeBufElmt == NULL) {
            curFreeBufElmt = newFreeBufElmt;
        }  
        /* Not first element */
        else {
            curFreeBufElmt->next_ptr = newFreeBufElmt;
            curFreeBufElmt = newFreeBufElmt;
        }  
    }
    if (threadSafeFlag == TRUE) {
        newWBuf->writeBufMutex = sr_mutex_init("writeBufMutex");
        if (newWBuf->writeBufMutex == NULL) {
            DPRINTF((APERROR, "%s: sr_mutex_init() returned NULL.\n", Fname));
            goto fail;
        }
    }

    return newWBuf;

fail:

    if (newWBuf != NULL) {
        WBufFree(newWBuf);
    }
    return NULL;
}

/*
 * WBufAdd:
 *
 * Copies data from "buf" into a data structure to be written out at a future
 * time. Normally, "buf" is added to the end of the pending output list
 * (op == SR_WBUF_APPEND). If op == SR_WBUF_PREPEND, this buffer was just
 * "removed" from the pending output list, but we were not able to write
 * out the buffer. We need to put it back at the beginning of the list so
 * it will be the next buffer returned when the user calls WBufGet().
 *
 * Returns 0 on success, -1 on failure.
 */
SR_INT32
WBufAdd(writeBuffer *wb, char *buf, SR_INT32 len, SR_INT32 writeOp)
{
    writeBufElmt *newWriteBuf = NULL;
    SR_INT32 returnVal;
    FNAME("WBufAdd")

    if (wb == NULL) {
        returnVal = -1;
        goto fail;
    }
    
    switch (writeOp) {
    case SR_WBUF_PREPEND:
        /*
         * If we have pending output and the first buffer is currently
         * supposed to be freed, mark it as not freed, so it will be the first
         * buffer taken off the list. This buffer was previously taken off
         * the list, but the application was not able to write out the buffer,
         * so it's being put back on the list to try again later.
         */
        if (wb->writeBufMutex != NULL) {
            sr_mutex_lock(wb->writeBufMutex);
        }
        if ( (wb->writeBufHead != NULL) && 
             (wb->writeBufHead->freeFlag == TRUE) ) {
            wb->writeBufHead->freeFlag = FALSE;
            returnVal = 0;
        }
        else {
            returnVal = -1;
        }
        if (wb->writeBufMutex != NULL) {
            sr_mutex_unlock(wb->writeBufMutex);
        }

        break;
    case SR_WBUF_APPEND:
        if ( (buf == NULL) || (len <= 0) ) {
            returnVal = -1;
            goto fail;
        }
    
        if (wb->writeBufMutex != NULL) {
            sr_mutex_lock(wb->writeBufMutex);
        }
        /* 
         * "Free" last write buffer retrieved if necessary.
         */
        WBufElmtFree(wb);

        /*
         * Check appropriate free buffer list for an available buffer.
         * IF none are free, the routine mallocs a new one of the
         * appropriate size.
         */
	newWriteBuf = WBufElmtGetNew(wb, buf, len);

        /*
         * Add new buffer to the current list of queued buffers.
         */
        if (newWriteBuf != NULL) {
            returnVal = WBufElmtAdd(wb, newWriteBuf);
        }
        else { 
            DPRINTF((APERROR,
                     "%s: WBufElmtGetNew failed to return a buffer to use.\n",
                     Fname));
            returnVal = -1;
        }
        if (wb->writeBufMutex != NULL) {
            sr_mutex_unlock(wb->writeBufMutex);
        }
        break;
    default:
        returnVal = -1;
        DPRINTF((APTRACE,
                 "%s: Illegal write operation type, %d, returning error.\n",
                 Fname, writeOp));
        break;
    }

    return(returnVal);

fail:

    return(returnVal);
}

/*
 * WBufGet:
 *
 * Gets the next buffer to be written out and places it in "buf". Caller
 * should not free buffer.  The buffer will be freed the next
 * time WBufAdd() or WbufGet() is called.
 *
 * Returns 0 on success, -1 on failure.
 */
SR_INT32
WBufGet(writeBuffer *wb, char **buf, int *len)
{
    writeBufElmt *retWriteBuf = NULL;

    if (wb == NULL) {
        goto fail;
    }

    /* Free first element on write buffer list if necessary */
    if (wb->writeBufMutex != NULL) {
        sr_mutex_lock(wb->writeBufMutex);
    }

    /* 
     * "Free" last write buffer retrieved if necessary.
     */
    WBufElmtFree(wb);

    /* Find first element that needs to be written out, if any. */
    if (wb->writeBufHead == NULL) {
        if (wb->writeBufMutex != NULL) {
            sr_mutex_unlock(wb->writeBufMutex);
        }
        goto fail;
    }
    retWriteBuf = wb->writeBufHead;
    wb->writeBufHead->freeFlag = TRUE;
    *len = retWriteBuf->lengthUsed;
    *buf = &retWriteBuf->buf;

    if (wb->writeBufMutex != NULL) {
        sr_mutex_unlock(wb->writeBufMutex);
    }
    return 0;

fail:

    return -1;
}

writeBufElmt *
WBufGetNextEmlt(writeBuffer *wb, writeBufElmt *wEmlt, char **buf, int *len)
{
    writeBufElmt *retWriteBuf = NULL;

    if (wb == NULL) {
        goto done;
    }

    /* Free first element on write buffer list if necessary */
    if (wb->writeBufMutex != NULL) {
        sr_mutex_lock(wb->writeBufMutex);
    }

    if (wEmlt == NULL) {
        /* Find first element that needs to be written out, if any. */
        if (wb->writeBufHead == NULL) {
            goto done;
        }
        retWriteBuf = wb->writeBufHead;
        while (retWriteBuf != NULL) {
            if (retWriteBuf->freeFlag == TRUE) {
                retWriteBuf = retWriteBuf->next_ptr;
            }
            else {
                break;
            }
        }
    }
    else {
        retWriteBuf = wEmlt->next_ptr;
        while (retWriteBuf != NULL) {
            if (retWriteBuf->freeFlag == TRUE) {
                retWriteBuf = retWriteBuf->next_ptr;
            }
            else {
                break;
            }
        }
    }
    if (retWriteBuf != NULL) {
        *len = retWriteBuf->lengthUsed;
        *buf = &retWriteBuf->buf;
    }

done:
    if (wb->writeBufMutex != NULL) {
        sr_mutex_unlock(wb->writeBufMutex);
    }

    return retWriteBuf;
}

/*
 * Free all resources associated with the given write buffer structure,
 * wb.
 *
 * Returns 0 on success, -1 on failure.
 */
SR_INT32
WBufFree(writeBuffer *wb)
{
    writeBufElmt *curWBufElmt = NULL;
    writeBufElmt *nextWBufElmt = NULL;
    freeBufElmt *curFreeBufElmt = NULL;
    freeBufElmt *nextFreeBufElmt = NULL;

    if (wb == NULL) {
        return 0;
    }

    if (wb->writeBufMutex != NULL) {
        sr_mutex_lock(wb->writeBufMutex);
    }

    /* Go through pending write buffer list and free all elements. */
    curWBufElmt = wb->writeBufHead;
    while (curWBufElmt != NULL) {
         nextWBufElmt = curWBufElmt->next_ptr;
         free(curWBufElmt);
         curWBufElmt = nextWBufElmt;
    } 
 
    /* Go through each free writebuffer list and free the elements. */
    curFreeBufElmt = wb->freeBufInfo;
    while (curFreeBufElmt != NULL) {
        curWBufElmt = curFreeBufElmt->freeBufHead;
        while (curWBufElmt != NULL) {
             nextWBufElmt = curWBufElmt->next_ptr;
             free(curWBufElmt);
             curWBufElmt = nextWBufElmt;
        } 
        nextFreeBufElmt = curFreeBufElmt->next_ptr;
        free(curFreeBufElmt);
        curFreeBufElmt = nextFreeBufElmt;
    }
 
    if (wb->writeBufMutex != NULL) {
        sr_mutex_unlock(wb->writeBufMutex);
        sr_mutex_free(wb->writeBufMutex);
    }

    free(wb);
    wb = NULL;

    return(0);
}

/*
 * WBufIsWritePending:
 *
 * Are there any pending write buffers?
 *
 * Returns 1 if there is a write pending, 0 otherwise.
 */
SR_INT32
WBufIsWritePending(writeBuffer *wb)
{
    SR_INT32 returnVal;

    /*
     * If the wb structure is NULL or there are no outstanding write buffers,
     * return 0.
     */
    if (wb == NULL) {
        returnVal = 0;
        goto done;
    }

    if (wb->writeBufMutex != NULL) {
        sr_mutex_lock(wb->writeBufMutex);
    }
    /* 
     * "Free" last write buffer retrieved if necessary.
     */
    WBufElmtFree(wb);

    /*
     * If there are pending write buffers, return 1.
     */
    if ( (wb->writeBufHead != NULL) && (wb->writeBufHead->freeFlag == FALSE) ) {
        returnVal = 1;
    }
    else {
        returnVal = 0;
    }

    if (wb->writeBufMutex != NULL) {
        sr_mutex_unlock(wb->writeBufMutex);
    }

done:
    return(returnVal);
}

void
SetWbufRemoveFlag(writeBuffer *wb, writeBufElmt *wbm)
{
    writeBufElmt *curWBufElmt = NULL;

    if ((wb == NULL) || (wbm == NULL)) {
        return ;
    }

    if (wb->writeBufMutex != NULL) {
        sr_mutex_lock(wb->writeBufMutex);
    }
    curWBufElmt = wb->writeBufHead;
    while (curWBufElmt != NULL) {
        if (curWBufElmt == wbm) {
            curWBufElmt->freeFlag = TRUE;
        }
        curWBufElmt = curWBufElmt->next_ptr;
    }
    if (wb->writeBufMutex != NULL) {
        sr_mutex_unlock(wb->writeBufMutex);
    }
}

void
WBufFreeMarkedBuffer(writeBuffer *wb)
{
    writeBufElmt *curWBufElmt = NULL;
    writeBufElmt *nextWBufElmt = NULL;
    writeBufElmt *delWBufElmt = NULL;
    writeBufElmt *prevWBufElmt = NULL;

    if (wb == NULL) {
        return ;
    }

    if (wb->writeBufMutex != NULL) {
        sr_mutex_lock(wb->writeBufMutex);
    }

    /* Go through pending write buffer list and free all elements. */
    curWBufElmt = wb->writeBufHead;
    prevWBufElmt = wb->writeBufHead;
    while (curWBufElmt != NULL) {
        nextWBufElmt = curWBufElmt->next_ptr;
        if (curWBufElmt->freeFlag == TRUE) {
            if (curWBufElmt == wb->writeBufHead) {
                delWBufElmt = curWBufElmt;
                prevWBufElmt = wb->writeBufHead;
                if (nextWBufElmt == NULL) {
                    wb->writeBufHead = NULL;
                    wb->writeBufTail = NULL;
                }
                else {
                    wb->writeBufHead = nextWBufElmt;
                }
            }
            else {
                if (wb->writeBufTail == curWBufElmt) {
                    wb->writeBufTail = prevWBufElmt;
                }
                prevWBufElmt->next_ptr = nextWBufElmt;
                delWBufElmt = curWBufElmt;
            }
            curWBufElmt = nextWBufElmt;
            WbufRemoveElmt(wb, delWBufElmt);
        }
        else {
            prevWBufElmt = curWBufElmt;
            curWBufElmt = nextWBufElmt;
        }
    }

    if (wb->writeBufMutex != NULL) {
        sr_mutex_unlock(wb->writeBufMutex);
    }

    return;
}
