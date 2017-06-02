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

#ifndef SR_SR_COND_H
#define SR_SR_COND_H

#ifdef  __cplusplus
extern "C" {
#endif

/* Conditional variable structure */

typedef struct _sr_cond_t {
    SR_INT32 tid;
} sr_cond_t;


/* prototypes */
sr_cond_t * sr_cond_init(SR_INT32 tid);
int sr_cond_destroy(sr_cond_t *cond_ptr);
int sr_cond_wait(sr_cond_t *cond_ptr, sr_mutex_t *mutex_ptr);
int sr_cond_timedwait(sr_cond_t *cond_ptr, 
		      sr_mutex_t *mutex_ptr,
                      SR_UINT32 secs,
                      SR_UINT32 nsecs);
int sr_cond_signal(sr_cond_t *cond_ptr);
int sr_cond_broadcast(sr_cond_t *cond_ptr);

extern SR_INT32 MAX_NUM_COND;

#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_COND_H */
