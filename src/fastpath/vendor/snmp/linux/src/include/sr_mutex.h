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

#ifndef SR_SR_MUTEX_H
#define SR_SR_MUTEX_H

#ifdef  __cplusplus
extern "C" {
#endif

/* Mutex structure */

typedef struct _sr_mutex_t {
    char *descString;
    SR_INT32 placeholder;
} sr_mutex_t;

/* prototypes */
sr_mutex_t *sr_mutex_init(char *descString);
int sr_mutex_lock(sr_mutex_t *mutex_ptr);
int sr_mutex_trylock(sr_mutex_t *mutex_ptr);
int sr_mutex_unlock(sr_mutex_t *mutex_ptr);
int sr_mutex_free(sr_mutex_t *mutex_ptr);

int sr_sigsend_to_thread(SR_INT32 Id, int which_sig);
int sr_thread_yield(void);

#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_MUTEX_H */
