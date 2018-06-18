/*
 *
 * Copyright (C) 1992-2003 by SNMP Research, Incorporated.
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

#ifndef SR_ROWSTATF_H
#define SR_ROWSTATF_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _cidata_t {
    void *data;
    ContextInfo *cip;
} cidata_t;

typedef void (*RowStatusTimerInfoFreeProc)(void *p);

int CheckRowStatus(
    SR_INT32 newstatus,
    SR_INT32 oldstatus);

int FreeRowStatusTimerInfo(
    SR_INT32 id,
    RowStatusTimerInfoFreeProc tifree);

void CancelRowStatusTimeout(SR_INT32 id);

SR_INT32 SetRowStatusTimeout(
    SR_INT32 delay,
    void *data,
    TodCallbackFunction tdp);

SR_INT32 SetRowStatusTimeoutWithCI(
    SR_INT32 delay,
    ContextInfo *cip,
    void *data,
    TodCallbackFunction tdp);

SR_INT32 SrSetRowStatusTimeout(
    SR_INT32 delay,
    void *data,
    SrTodCallbackFunction callback);

SR_INT32 SrSetRowStatusTimeoutWithCI(
    SR_INT32 delay,
    ContextInfo *cip,
    void *data,
    SrTodCallbackFunction callback);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* ! SR_ROWSTATF_H */
