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

static const SR_UINT32 interfaces_last_sid_array[] = {
    1  /* ifNumber */
};

const SnmpType interfacesTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(interfaces_t, ifNumber), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrGetInfoEntry interfacesGetInfo = {
    (SR_KGET_FPTR) new_k_interfaces_get,
    (SR_FREE_FPTR) NULL,
    (int) sizeof(interfaces_t),
    I_interfaces_max,
    (SnmpType *) interfacesTypeTable,
    NULL,
    (short) offsetof(interfaces_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the interfaces family.
 *---------------------------------------------------------------------*/
VarBind *
interfaces_get(OID *incoming, ObjectInfo *object, int searchType,
               ContextInfo *contextInfo, int serialNum)
{
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &interfacesGetInfo));
}

interfaces_t *
new_k_interfaces_get(int serialNum, ContextInfo *contextInfo,
                     int nominator, int searchType,
                     interfaces_t *data)
{
    return k_interfaces_get(serialNum, contextInfo, nominator);
}

static const SR_UINT32 ifEntry_last_sid_array[] = {
    1, /* ifIndex */
    2, /* ifDescr */
    3, /* ifType */
    4, /* ifMtu */
    5, /* ifSpeed */
    6, /* ifPhysAddress */
    7, /* ifAdminStatus */
    8, /* ifOperStatus */
    9, /* ifLastChange */
    10, /* ifInOctets */
    11, /* ifInUcastPkts */
    12, /* ifInNUcastPkts */
    13, /* ifInDiscards */
    14, /* ifInErrors */
    15, /* ifInUnknownProtos */
    16, /* ifOutOctets */
    17, /* ifOutUcastPkts */
    18, /* ifOutNUcastPkts */
    19, /* ifOutDiscards */
    20, /* ifOutErrors */
    21, /* ifOutQLen */
    22  /* ifSpecific */
};

const SnmpType ifEntryTypeTable[] = {
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifIndex), 0 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifDescr), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifType), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifMtu), -1 },
    { GAUGE_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifSpeed), -1 },
    { OCTET_PRIM_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifPhysAddress), -1 },
    { INTEGER_TYPE, SR_READ_WRITE, offsetof(ifEntry_t, ifAdminStatus), -1 },
    { INTEGER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifOperStatus), -1 },
    { TIME_TICKS_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifLastChange), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifInOctets), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifInUcastPkts), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifInNUcastPkts), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifInDiscards), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifInErrors), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifInUnknownProtos), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifOutOctets), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifOutUcastPkts), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifOutNUcastPkts), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifOutDiscards), -1 },
    { COUNTER_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifOutErrors), -1 },
    { GAUGE_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifOutQLen), -1 },
    { OBJECT_ID_TYPE, SR_READ_ONLY, offsetof(ifEntry_t, ifSpecific), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

const SrIndexInfo ifEntryIndexInfo[] = {
#ifdef I_ifIndex
    { I_ifIndex, T_uint, -1 },
#endif /* I_ifIndex */
    { -1, -1, -1 }
};

const SrGetInfoEntry ifEntryGetInfo = {
    (SR_KGET_FPTR) new_k_ifEntry_get,
#ifndef U_ifEntry
    (SR_FREE_FPTR) NULL,
#else /* U_ifEntry */
    (SR_FREE_FPTR) k_ifEntryFreeUserpartData,
#endif /* U_ifEntry */
    (int) sizeof(ifEntry_t),
    I_ifEntry_max,
    (SnmpType *) ifEntryTypeTable,
    ifEntryIndexInfo,
    (short) offsetof(ifEntry_t, valid)
};

/*---------------------------------------------------------------------
 * Retrieve data from the ifEntry family.
 *---------------------------------------------------------------------*/
VarBind *
ifEntry_get(OID *incoming, ObjectInfo *object, int searchType,
            ContextInfo *contextInfo, int serialNum)
{
#if ! ( defined(I_ifIndex) )
    return NULL;
#else	/* ! ( defined(I_ifIndex) ) */
    return (v_get(incoming, object, searchType, contextInfo, serialNum,
                  (SrGetInfoEntry *) &ifEntryGetInfo));
#endif	/* ! ( defined(I_ifIndex) ) */
}

ifEntry_t *
new_k_ifEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator, int searchType,
                  ifEntry_t *data)
{
    if (data == NULL) {
        return NULL;
    }
    return k_ifEntry_get(serialNum, contextInfo, nominator,
                         searchType, data->ifIndex);
}


#ifdef SETS 

#ifdef __cplusplus
extern "C" {
#endif
static int ifEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));
#ifdef __cplusplus
}
#endif

/*
 * Syntax refinements for the ifEntry family
 *
 * For each object in this family in which the syntax clause in the MIB
 * defines a refinement to the size, range, or enumerations, initialize
 * a data structure with these refinements.
 */
static RangeTest_t   ifAdminStatus_range[] = { { 1, 3 } };

/*
 * Initialize the sr_member_test array with one entry per object in the
 * ifEntry family.
 */
static struct sr_member_test ifEntry_member_test[] =
{
    /* ifIndex */
    { MINV_NO_TEST, 0, NULL, NULL },

    /* ifDescr */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifType */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifMtu */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifSpeed */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifPhysAddress */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifAdminStatus */
    { MINV_INTEGER_RANGE_TEST, 
      sizeof(ifAdminStatus_range)/sizeof(RangeTest_t), /* 3 */
      ifAdminStatus_range, NULL },

    /* ifOperStatus */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifLastChange */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifInOctets */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifInUcastPkts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifInNUcastPkts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifInDiscards */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifInErrors */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifInUnknownProtos */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifOutOctets */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifOutUcastPkts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifOutNUcastPkts */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifOutDiscards */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifOutErrors */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifOutQLen */
    { MINV_NOT_WRITABLE, 0, NULL, NULL },

    /* ifSpecific */
    { MINV_NOT_WRITABLE, 0, NULL, NULL }
};


/*
 * Initialize SrTestInfoEntry for the ifEntry family.
 */
const SrTestInfoEntry ifEntryTestInfo = {
    &ifEntryGetInfo,
    ifEntry_member_test,
#ifdef ifEntry_READ_CREATE
    k_ifEntry_set_defaults,
#else /* ifEntry_READ_CREATE */
    NULL,
#endif /* ifEntry_READ_CREATE */
    k_ifEntry_test,
    k_ifEntry_ready,
#ifdef SR_ifEntry_UNDO
    ifEntry_undo,
#else /* SR_ifEntry_UNDO */
    NULL,
#endif /* SR_ifEntry_UNDO */
    ifEntry_ready,
    ifEntry_set,
    ifEntry_cleanup,
#ifndef U_ifEntry
    (SR_COPY_FPTR) NULL
#else /* U_ifEntry */
    (SR_COPY_FPTR) k_ifEntryCloneUserpartData
#endif /* U_ifEntry */
};

/*----------------------------------------------------------------------
 * Free the ifEntry data object.
 *---------------------------------------------------------------------*/
void
ifEntry_free(ifEntry_t *data)
{
    /* free function is only needed by old k_ routines with -row_status */
    SrFreeFamily(data,
                 ifEntryGetInfo.type_table,
                 ifEntryGetInfo.highest_nominator,
                 ifEntryGetInfo.valid_offset,
                 ifEntryGetInfo.userpart_free_func);
}

/*----------------------------------------------------------------------
 * cleanup after ifEntry set/undo
 *---------------------------------------------------------------------*/
static int
ifEntry_cleanup(doList_t *trash)
{
    return SrCleanup(trash, &ifEntryTestInfo);
}

/*----------------------------------------------------------------------
 * clone the ifEntry family
 *---------------------------------------------------------------------*/
ifEntry_t *
Clone_ifEntry(ifEntry_t *ifEntry)
{
    /* Clone function is not used by auto-generated */
    /* code, but may be used by user code */
    return (ifEntry_t *)SrCloneFamily(ifEntry,
                         ifEntryGetInfo.family_size,
                         ifEntryGetInfo.type_table,
                         ifEntryGetInfo.highest_nominator,
                         ifEntryGetInfo.valid_offset,
                         ifEntryTestInfo.userpart_clone_func,
                         ifEntryGetInfo.userpart_free_func);
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 *---------------------------------------------------------------------*/
int 
ifEntry_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
             doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_test(incoming, object, value, doHead, doCur, contextInfo,
                  &ifEntryTestInfo);
}

/*---------------------------------------------------------------------
 * Determine if entries in this SET request are consistent
 *---------------------------------------------------------------------*/
int 
ifEntry_ready(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
    return v_ready(doHead, doCur, contextInfo,
                  &ifEntryTestInfo);
}


/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ifEntry_set(doList_t *doHead, doList_t *doCur, ContextInfo *contextInfo)
{
  return (k_ifEntry_set((ifEntry_t *) (doCur->data),
            contextInfo, GetSetFunction(doCur->set_kind, doCur->state)));
}

#endif /* SETS */


