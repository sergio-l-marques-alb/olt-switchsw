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
 */


/*
 * Arguments used to create this file:
 * -apo -f mibcompflags -userpart -v_stubs_only 
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <malloc.h>

#include <string.h>


#include <stddef.h>
#include "sr_snmp.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "oid_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "diag.h"
SR_FILENAME
#include "min_v.h"
#include "mibout.h"

static const SR_UINT32 icmp_last_sid_array[] = {
    1, /* icmpInMsgs */
    2, /* icmpInErrors */
    3, /* icmpInDestUnreachs */
    4, /* icmpInTimeExcds */
    5, /* icmpInParmProbs */
    6, /* icmpInSrcQuenchs */
    7, /* icmpInRedirects */
    8, /* icmpInEchos */
    9, /* icmpInEchoReps */
    10, /* icmpInTimestamps */
    11, /* icmpInTimestampReps */
    12, /* icmpInAddrMasks */
    13, /* icmpInAddrMaskReps */
    14, /* icmpOutMsgs */
    15, /* icmpOutErrors */
    16, /* icmpOutDestUnreachs */
    17, /* icmpOutTimeExcds */
    18, /* icmpOutParmProbs */
    19, /* icmpOutSrcQuenchs */
    20, /* icmpOutRedirects */
    21, /* icmpOutEchos */
    22, /* icmpOutEchoReps */
    23, /* icmpOutTimestamps */
    24, /* icmpOutTimestampReps */
    25, /* icmpOutAddrMasks */
    26  /* icmpOutAddrMaskReps */
};

const SnmpType icmpTypeTable[] = {
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInMsgs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInErrors), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInDestUnreachs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInTimeExcds), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInParmProbs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInSrcQuenchs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInRedirects), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInEchos), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInEchoReps), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInTimestamps), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInTimestampReps), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInAddrMasks), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpInAddrMaskReps), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutMsgs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutErrors), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutDestUnreachs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutTimeExcds), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutParmProbs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutSrcQuenchs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutRedirects), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutEchos), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutEchoReps), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutTimestamps), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutTimestampReps), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutAddrMasks), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(icmp_t, icmpOutAddrMaskReps), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry icmpGetInfo = {
    (SR_KGET_FPTR) new_k_icmp_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(icmp_t),
    I_icmp_max,
    (SnmpType *) icmpTypeTable,
    NULL,
    (short) offsetof(icmp_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the icmp family.
 *---------------------------------------------------------------------*/
VarBind *
icmp_get(OID *incoming, ObjectInfo *object, int searchType,
         ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &icmpGetInfo));
}

icmp_t *
new_k_icmp_get(int serialNum, ContextInfo *contextInfo,
               int nominator, int searchType,
               icmp_t *data)
{
    return k_icmp_get(serialNum, contextInfo, nominator);
}

