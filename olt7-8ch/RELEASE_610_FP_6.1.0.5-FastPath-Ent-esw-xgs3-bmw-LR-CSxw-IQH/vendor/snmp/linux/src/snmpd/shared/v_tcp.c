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

static const SR_UINT32 tcp_last_sid_array[] = {
    1, /* tcpRtoAlgorithm */
    2, /* tcpRtoMin */
    3, /* tcpRtoMax */
    4, /* tcpMaxConn */
    5, /* tcpActiveOpens */
    6, /* tcpPassiveOpens */
    7, /* tcpAttemptFails */
    8, /* tcpEstabResets */
    9, /* tcpCurrEstab */
    10, /* tcpInSegs */
    11, /* tcpOutSegs */
    12, /* tcpRetransSegs */
    14, /* tcpInErrs */
    15  /* tcpOutRsts */
};

const SnmpType tcpTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpRtoAlgorithm), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpRtoMin), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpRtoMax), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpMaxConn), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpActiveOpens), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpPassiveOpens), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpAttemptFails), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpEstabResets), -1 },
    { GAUGE_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpCurrEstab), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpInSegs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpOutSegs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpRetransSegs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpInErrs), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(tcp_t, tcpOutRsts), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry tcpGetInfo = {
    (SR_KGET_FPTR) new_k_tcp_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(tcp_t),
    I_tcp_max,
    (SnmpType *) tcpTypeTable,
    NULL,
    (short) offsetof(tcp_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the tcp family.
 *---------------------------------------------------------------------*/
VarBind *
tcp_get(OID *incoming, ObjectInfo *object, int searchType,
        ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &tcpGetInfo));
}

tcp_t *
new_k_tcp_get(int serialNum, ContextInfo *contextInfo,
              int nominator, int searchType,
              tcp_t *data)
{
    return k_tcp_get(serialNum, contextInfo, nominator);
}

static const SR_UINT32 tcpConnEntry_last_sid_array[] = {
    1, /* tcpConnState */
    2, /* tcpConnLocalAddress */
    3, /* tcpConnLocalPort */
    4, /* tcpConnRemAddress */
    5  /* tcpConnRemPort */
};

const SnmpType tcpConnEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(tcpConnEntry_t, tcpConnState), -1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_ONLY, offsetof(tcpConnEntry_t, tcpConnLocalAddress), 0 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(tcpConnEntry_t, tcpConnLocalPort), 1 },
    { IP_ADDR_PRIM_TYPE, SR_READ_ONLY, offsetof(tcpConnEntry_t, tcpConnRemAddress), 2 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(tcpConnEntry_t, tcpConnRemPort), 3 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo tcpConnEntryIndexInfo[] = {
#ifdef I_tcpConnLocalAddress
    { I_tcpConnLocalAddress, T_uint, -1 },
#endif /* I_tcpConnLocalAddress */
#ifdef I_tcpConnLocalPort
    { I_tcpConnLocalPort, T_uint, -1 },
#endif /* I_tcpConnLocalPort */
#ifdef I_tcpConnRemAddress
    { I_tcpConnRemAddress, T_uint, -1 },
#endif /* I_tcpConnRemAddress */
#ifdef I_tcpConnRemPort
    { I_tcpConnRemPort, T_uint, -1 },
#endif /* I_tcpConnRemPort */
    { -1, -1, -1 }
};

const SrGetInfoEntry tcpConnEntryGetInfo = {
    (SR_KGET_FPTR) new_k_tcpConnEntry_get,
#ifndef U_tcpConnEntry
    (SR_FREE_FPTR) NULL,
#else /* U_tcpConnEntry */
    (SR_FREE_FPTR) k_tcpConnEntryFreeUserpartData,
#endif /* U_tcpConnEntry */
    (int) sizeof(tcpConnEntry_t),
    I_tcpConnEntry_max,
    (SnmpType *) tcpConnEntryTypeTable,
    tcpConnEntryIndexInfo,
    (short) offsetof(tcpConnEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the tcpConnEntry family.
 *---------------------------------------------------------------------*/
VarBind *
tcpConnEntry_get(OID *incoming, ObjectInfo *object, int searchType,
                 ContextInfo *contextInfo, int serialNum)
{
#if (((! ( defined(I_tcpConnLocalAddress) ) || ! ( defined(I_tcpConnLocalPort) )) || ! ( defined(I_tcpConnRemAddress) )) || ! ( defined(I_tcpConnRemPort) ))
    return NULL;
#else	/* (((! ( defined(I_tcpConnLocalAddress) ) || ! ( defined(I_tcpConnLocalPort) )) || ! ( defined(I_tcpConnRemAddress) )) || ! ( defined(I_tcpConnRemPort) )) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &tcpConnEntryGetInfo));
#endif	/* (((! ( defined(I_tcpConnLocalAddress) ) || ! ( defined(I_tcpConnLocalPort) )) || ! ( defined(I_tcpConnRemAddress) )) || ! ( defined(I_tcpConnRemPort) )) */
}

tcpConnEntry_t *
new_k_tcpConnEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator, int searchType,
                       tcpConnEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_tcpConnEntry_get(serialNum, contextInfo, nominator,
                              searchType, data->tcpConnLocalAddress,
                              data->tcpConnLocalPort, data->tcpConnRemAddress,
                              data->tcpConnRemPort);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int tcpConnEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the tcpConnEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   tcpConnState_range[] = { { 1, 12 } };
static RangeTest_t   tcpConnLocalPort_range[] = { { 0, 65535 } };
static RangeTest_t   tcpConnRemPort_range[] = { { 0, 65535 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * tcpConnEntry family.
 */
static struct sr_member_test tcpConnEntry_member_test[] =
{
    /* tcpConnState */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(tcpConnState_range)/sizeof(RangeTest_t), /* 12 */
      tcpConnState_range, NULL },

    /* tcpConnLocalAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* tcpConnLocalPort */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(tcpConnLocalPort_range)/sizeof(RangeTest_t), /* 1 */
      tcpConnLocalPort_range, NULL },

    /* tcpConnRemAddress */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* tcpConnRemPort */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(tcpConnRemPort_range)/sizeof(RangeTest_t), /* 1 */
      tcpConnRemPort_range, NULL }
};


/*
 * Initialize SrTestInfoEntry for the tcpConnEntry family.
 */
const SrTestInfoEntry tcpConnEntryTestInfo = {
    &tcpConnEntryGetInfo,
    tcpConnEntry_member_test,
#ifdef tcpConnEntry_READ_CREATE
    k_tcpConnEntry_set_defaults,
#else /* tcpConnEntry_READ_CREATE */
    NULL,
#endif /* tcpConnEntry_READ_CREATE */
    k_tcpConnEntry_test,
    k_tcpConnEntry_ready,
#ifdef SR_tcpConnEntry_UNDO
    tcpConnEntry_undo,
#else /* SR_tcpConnEntry_UNDO */
    NULL,
#endif /* SR_tcpConnEntry_UNDO */
    tcpConnEntry_ready,
    tcpConnEntry_set,
    tcpConnEntry_cleanup,
#ifndef U_tcpConnEntry
    (SR_COPY_FPTR) NULL
#else /* U_tcpConnEntry */
    (SR_COPY_FPTR) k_tcpConnEntryCloneUserpartData
#endif /* U_tcpConnEntry */
};

/*----------------------------------------------------------------------
 * Free the tcpConnEntry data object.
 *---------------------------------------------------------------------*/
void
tcpConnEntry_free(tcpConnEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 tcpConnEntryGetInfo.type_table,
                 tcpConnEntryGetInfo.highest_nominator,
                 tcpConnEntryGetInfo.valid_offset,
                 tcpConnEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after tcpConnEntry set/undo
 *---------------------------------------------------------------------*/
static int
tcpConnEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &tcpConnEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the tcpConnEntry family
 *---------------------------------------------------------------------*/
tcpConnEntry_t *
Clone_tcpConnEntry(tcpConnEntry_t *tcpConnEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (tcpConnEntry_t *)SrCloneFamily(tcpConnEntry,
                         tcpConnEntryGetInfo.family_size,
                         tcpConnEntryGetInfo.type_table,
                         tcpConnEntryGetInfo.highest_nominator,
                         tcpConnEntryGetInfo.valid_offset,
                         tcpConnEntryTestInfo.userpart_clone_func,
                         tcpConnEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
tcpConnEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
                  doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &tcpConnEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
tcpConnEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &tcpConnEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
tcpConnEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_tcpConnEntry_set((tcpConnEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


