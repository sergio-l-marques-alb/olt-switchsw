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

#ifndef SR_SRI_SR_WBUF_H
#define SR_SRI_SR_WBUF_H

#ifdef  __cplusplus
extern "C" {
#endif


#define SR_WBUF_APPEND		1    /* Put buffer at end of list */
#define SR_WBUF_PREPEND	2    /* Put buffer back at beginning of list */

/* Write buffer structures */
typedef struct _writeBufElmt {
    SR_INT32 length;		    /* full length of buf */
    SR_INT32 lengthUsed;            /* How much of buf is being used */
    SR_INT32 freeFlag;              /* Should entry be freed on next call of
                                     * WBufAdd() or WBufGet? TRUE or FALSE.
                                     * Can be reset to FALSE if "write" fails
                                     * again on current element.
                                     */
    struct _writeBufElmt *next_ptr; /* Next pointer in linked list */
    char buf;			    /* beginning of pending write buffer */
} writeBufElmt;

typedef struct _freeBufElmt {
    SR_INT32 bufferSize;	    /* length of each free buffer */
    writeBufElmt *freeBufHead;     /* Head ptr of the available write buffers */
    writeBufElmt *freeBufTail;     /* Tail ptr of the available write buffers */
    struct _freeBufElmt *next_ptr;
} freeBufElmt;

typedef struct _writeBuffer {
    writeBufElmt *writeBufHead;    /* Head ptr of the pending write buffers */
    writeBufElmt *writeBufTail;    /* Tail ptr of the pending write buffers */
    sr_mutex_t   *writeBufMutex;   /* Only relevant if threadSafeFlag == TRUE
                                    * when WBufInit() is called.
                                    */
    SR_INT32      totalBufSize;    /* How many bytes malloc'ed so far */
    SR_INT32      maxBufSize;
    SR_INT32      numBuffers;	   /* How many different buffer sizes do we
                                    * have?
                                    */
    freeBufElmt  *freeBufInfo;     /*
                                    * Free buffer information for each buffer
                                    * size.
                                    */
} writeBuffer;

/* Public APIs */
/*
 * Sets up writeBuffer structure, sets max. available memory to "size".
 * Returns structure on success, NULL on failure. 
 */
writeBuffer *WBufInit(SR_INT32 size, SR_INT32 *bufsizes,
                      SR_INT32 threadSafeFlag);

/*
 * Copies data from "buf" into a data structure to be written out at a future
 * time. Normally, "buf" is added to the end of the pending output list
 * (op == SR_WBUF_APPEND). If op == SR_WBUF_PREPEND, this buffer was just
 * "removed" from the pending output list, but we were not able to write
 * out the buffer. We need to put it back at the beginning of the list so
 * it will be the next buffer returned when the user calls WBufGet().
 *
 * Returns 0 on success, -1 on failure. 
 */
SR_INT32 WBufAdd(writeBuffer *wb, char *buf, SR_INT32 len, SR_INT32 writeOp);

/*
 * Gets the next buffer to be written out and places it in "buf". Caller 
 * should not free buffer.  The buffer will be freed the next
 * time WBufAdd(), WbufGet(), or WBufIsWritePending() is called. 
 *
 * Returns 0 on success, -1 on failure. 
 */
SR_INT32 WBufGet(writeBuffer *wb, char **buf, int *len);

/*
 * Free all resources associated with the given write buffer structure,
 * wb.
 *
 * Returns 0 on success, -1 on failure.
 */
SR_INT32 WBufFree(writeBuffer *wb);

/*
 * Are there any pending write buffers?
 *
 * Returns 1 if there is a write pending, 0 otherwise.
 */
SR_INT32 WBufIsWritePending(writeBuffer *wb);

void SetWbufRemoveFlag(writeBuffer *wb, writeBufElmt *wbm);

void WBufFreeMarkedBuffer(writeBuffer *wb);

writeBufElmt *WBufGetNextEmlt(writeBuffer *wb, writeBufElmt *wEmlt,
                              char **buf, int *len);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif                          /* SR_SRI_SR_WBUF_H */

