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

static const SR_UINT32 snmp_last_sid_array[] = {
    1, /* snmpInPkts */
    2, /* snmpOutPkts */
    3, /* snmpInBadVersions */
    4, /* snmpInBadCommunityNames */
    5, /* snmpInBadCommunityUses */
    6, /* snmpInASNParseErrs */
    8, /* snmpInTooBigs */
    9, /* snmpInNoSuchNames */
    10, /* snmpInBadValues */
    11, /* snmpInReadOnlys */
    12, /* snmpInGenErrs */
    13, /* snmpInTotalReqVars */
    14, /* snmpInTotalSetVars */
    15, /* snmpInGetRequests */
    16, /* snmpInGetNexts */
    17, /* snmpInSetRequests */
    18, /* snmpInGetResponses */
    19, /* snmpInTraps */
    20, /* snmpOutTooBigs */
    21, /* snmpOutNoSuchNames */
    22, /* snmpOutBadValues */
    24, /* snmpOutGenErrs */
    25, /* snmpOutGetRequests */
    26, /* snmpOutGetNexts */
    27, /* snmpOutSetRequests */
    28, /* snmpOutGetResponses */
    29, /* snmpOutTraps */
    30, /* snmpEnableAuthenTraps */
    31, /* snmpSilentDrops */
    32  /* snmpProxyDrops */
};

const SnmpType snmpTypeTable[] = {
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInPkts), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutPkts), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInBadVersions), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInBadCommunityNames), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInBadCommunityUses), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInASNParseErrs), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInTooBigs), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInNoSuchNames), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInBadValues), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInReadOnlys), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInGenErrs), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInTotalReqVars), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInTotalSetVars), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInGetRequests), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInGetNexts), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInSetRequests), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInGetResponses), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpInTraps), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutTooBigs), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutNoSuchNames), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutBadValues), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutGenErrs), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutGetRequests), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutGetNexts), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutSetRequests), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutGetResponses), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpOutTraps), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(snmp_t, snmpEnableAuthenTraps), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpSilentDrops), -1 },
    { COUNTER_32_TYPE, SR_READ_ONLY, offsetof(snmp_t, snmpProxyDrops), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry snmpGetInfo = {
    (SR_KGET_FPTR) new_k_snmp_get,
#ifndef U_snmp
    (SR_FREE_FPTR) NULL,
#else /* U_snmp */
    (SR_FREE_FPTR) k_snmpFreeUserpartData,
#endif /* U_snmp */
    (int) sizeof(snmp_t),
    I_snmp_max,
    (SnmpType *) snmpTypeTable,
    NULL,
    (short) offsetof(snmp_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the snmp family.
 *---------------------------------------------------------------------*/
VarBind *
snmp_get(OID *incoming, ObjectInfo *object, int searchType,
         ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &snmpGetInfo));
}

snmp_t *
new_k_snmp_get(int serialNum, ContextInfo *contextInfo,
               int nominator, int searchType,
               snmp_t *data)
{
    return k_snmp_get(serialNum, contextInfo, nominator);
}

#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int snmp_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the snmp family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   snmpEnableAuthenTraps_range[] = { { 1, 2 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * snmp family.
 */
static struct sr_member_test snmp_member_test[] =
{
    /* snmpInPkts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutPkts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInBadVersions */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInBadCommunityNames */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInBadCommunityUses */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInASNParseErrs */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInTooBigs */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInNoSuchNames */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInBadValues */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInReadOnlys */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInGenErrs */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInTotalReqVars */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInTotalSetVars */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInGetRequests */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInGetNexts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInSetRequests */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInGetResponses */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpInTraps */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutTooBigs */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutNoSuchNames */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutBadValues */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutGenErrs */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutGetRequests */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutGetNexts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutSetRequests */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutGetResponses */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpOutTraps */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpEnableAuthenTraps */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(snmpEnableAuthenTraps_range)/sizeof(RangeTest_t), /* 2 */
      snmpEnableAuthenTraps_range, NULL },

    /* snmpSilentDrops */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* snmpProxyDrops */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};


/*
 * Initialize SrTestInfoEntry for the snmp family.
 */
const SrTestInfoEntry snmpTestInfo = {
    &snmpGetInfo,
    snmp_member_test,
    NULL,
    k_snmp_test,
    k_snmp_ready,
#ifdef SR_snmp_UNDO
    snmp_undo,
#else /* SR_snmp_UNDO */
    NULL,
#endif /* SR_snmp_UNDO */
    snmp_ready,
    snmp_set,
    snmp_cleanup,
#ifndef U_snmp
    (SR_COPY_FPTR) NULL
#else /* U_snmp */
    (SR_COPY_FPTR) k_snmpCloneUserpartData
#endif /* U_snmp */
};

/*----------------------------------------------------------------------
 * cleanup after snmp set/undo
 *---------------------------------------------------------------------*/
static int
snmp_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &snmpTestInfo);
}

#ifdef SR_snmp_UNDO
/*----------------------------------------------------------------------
 * clone the snmp family
 *---------------------------------------------------------------------*/
snmp_t *
Clone_snmp(snmp_t *snmp)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (snmp_t *)SrCloneFamily(snmp,
                         snmpGetInfo.family_size,
                         snmpGetInfo.type_table,
                         snmpGetInfo.highest_nominator,
                         snmpGetInfo.valid_offset,
                         snmpTestInfo.userpart_clone_func,
                         snmpGetInfo.userpart_free_func);
}

#endif /* defined(SR_snmp_UNDO) */
/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
snmp_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
          doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &snmpTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
snmp_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &snmpTestInfo);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snmp_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_snmp_set((snmp_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


